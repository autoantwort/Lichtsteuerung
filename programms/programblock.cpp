#include "programblock.h"
#include <QJsonArray>
#include <stdexcept>
#include "modulemanager.h"


namespace Modules {


    namespace detail {
    template<typename PointerType>
    class SharedPtrComperator{
        public:
        bool operator()(const PointerType *l,const PointerType *r)const{
            return l<r;
        }
        bool operator()(const std::shared_ptr<PointerType> &l,const std::shared_ptr<PointerType> &r)const{
            return l.get()<r.get();
        }
        bool operator()(const PointerType*l,const std::shared_ptr<PointerType> &r)const{
            return l<r.get();
        }
        bool operator()(const std::shared_ptr<PointerType> &l,const PointerType*r)const{
            return l.get()<r;
        }
    };
    }

    ProgramBlock::ProgramBlock()
    {

    }

    ProgramBlock::ProgramBlock(const QJsonObject& o){
        // Wir erstellen erstmal alle objecte mit passendem Typ und ID
        std::map<QString,std::shared_ptr<Programm>> programs;
        std::map<QString,std::shared_ptr<Filter>> filter;
        std::map<QString,std::shared_ptr<Consumer>> consumer;
        {
            const auto progs = o["programs"].toArray();
            for(const auto & v_ : progs){
                QJsonObject obj = v_.toObject();
                const auto qstr = obj["typename"].toString();
                const auto str = qstr.toStdString();
                auto p = ModuleManager::singletone()->createProgramm(str);
                if(!p){
                    throw std::runtime_error(std::string("Program with name '") + str + "' does not exist.");
                }
                programs.insert(std::make_pair(qstr,p));
            }
        }
        {
            const auto progs = o["filter"].toArray();
            for(const auto & v_ : progs){
                QJsonObject obj = v_.toObject();
                const auto qstr = obj["typename"].toString();
                const auto str = qstr.toStdString();
                auto p = ModuleManager::singletone()->createFilter(str);
                if(!p){
                    throw std::runtime_error(std::string("Filter with name '") + str + "' does not exist.");
                }
                filter.insert(std::make_pair(qstr,p));
            }
        }
        {
            const auto progs = o["consumer"].toArray();
            for(const auto & v_ : progs){
                QJsonObject obj = v_.toObject();
                const auto qstr = obj["typename"].toString();
                const auto str = qstr.toStdString();
                auto p = ModuleManager::singletone()->createConsumer(str);
                if(!p){
                    throw std::runtime_error(std::string("Consumer with name '") + str + "' does not exist.");
                }
                consumer.insert(std::make_pair(qstr,p));
            }
        }
        // Wir haben schon alle benötogten Objecte erstellt und wissen zu welcher ID welches Object gehört.
        {
            const auto filterCon = o["filterConnections"].toArray();
            for(const auto & i : filterCon){
                const auto iter = filter.find(i["source"].toString());
                if(iter == filter.cend()){
                    throw std::runtime_error("Connection has invalid source.");
                }
                detail::Connection c(iter->second);
                const auto targets = i["targets"].toArray();
                for(const auto & t : targets){
                    OutputDataProducer * outputDataProducer;
                    const auto targetName = t["target"].toString();
                    const auto pIter = programs.find(targetName);
                    if(pIter != programs.cend()){
                        outputDataProducer = pIter->second.get();
                    }else{
                        const auto fIter = filter.find(targetName);
                        if(fIter != filter.cend()){
                            outputDataProducer = fIter->second.get();
                        }else{
                            throw std::runtime_error("Connection target not found :" + targetName.toStdString());
                        }
                    }
                    c.addTarget(t["length"].toInt(),outputDataProducer,t["targetStartIndex"].toInt());
                }
                this->filter.insert(std::make_pair(i["level"].toInt(),c));
            }
        }
        {
            const auto consumerCon = o["consumerConnections"].toArray();
            for(const auto & i : consumerCon){
                const auto iter = consumer.find(i["source"].toString());
                if(iter == consumer.cend()){
                    throw std::runtime_error("Connection has invalid source.");
                }
                detail::Connection c(iter->second);
                const auto targets = i["targets"].toArray();
                for(const auto & t : targets){
                    OutputDataProducer * outputDataProducer;
                    const auto targetName = t["target"].toString();
                    const auto pIter = programs.find(targetName);
                    if(pIter != programs.cend()){
                        outputDataProducer = pIter->second.get();
                    }else{
                        const auto fIter = filter.find(targetName);
                        if(fIter != filter.cend()){
                            outputDataProducer = fIter->second.get();
                        }else{
                            throw std::runtime_error("Connection target not found :" + targetName.toStdString());
                        }
                    }
                    c.addTarget(t["length"].toInt(),outputDataProducer,t["targetStartIndex"].toInt());
                }
                this->consumer.push_back(c);
            }
        }
    }

    void ProgramBlock::addFilter(detail::Connection c, int layer){
        for(const auto & con : c.targeds){
            if(!haveOutputDataProducer(con.second.targed)){
                throw std::runtime_error("one outputdataproducer is unknown");
            }
        }
        filter.emplace(layer,std::move(c));
    }

    void ProgramBlock::addConsumer(detail::Connection c){
        for(const auto & con : c.targeds){
            if(!haveOutputDataProducer(con.second.targed)){
                throw std::runtime_error("one outputdataproducer is unknown");
            }
        }
        consumer.push_back(std::move(c));
    }


    bool ProgramBlock::doStep(time_diff_t diff){
        bool finished = false;
        for(auto & p : programs){
            const auto state = p->doStep(diff);
            finished |= state.finished;
            dataChanged[p.get()] = state.outputDataChanged;
        }
        for(auto i = filter.cbegin(); i!= filter.cend();++i){
            if(doConnection(i->second)){
                static_cast<Filter*>(i->second.source.get())->filter();
            }
            dataChanged[i->second.source.get()] |= static_cast<Filter*>(i->second.source.get())->doStep(diff);
        }
        for(auto i = consumer.cbegin(); i != consumer.cend();++i){
            if(doConnection(*i)){
                static_cast<Consumer*>(i->source.get())->show();
            }
        }
        return finished;
    }


    void ProgramBlock::writeJsonObject(QJsonObject &o){
        // Wir speichern zu jedem Programm/Filter/Consumer ein Object ab, das den Typnamen und eine ID, wir nehmen die Adresse im Arbeitsspeicher.
        // Wir speichern die Connections, indem wir für die Source und die Targets die Adressen(IDs) der jeweiligen Pointer speichern.
        // --- Programs ---
        {QJsonArray progs;
        for(auto i = programs.cbegin();i!=programs.cend();++i){
            QJsonObject p;
            p["typename"] = (*i)->getName();
            p["id"] = QString::number(reinterpret_cast<size_t>(i->get()));
            static_assert (sizeof (size_t) == sizeof (Programm*), "Pointer must have same size as size_t");
            progs.push_back(p);
        }
        o["programs"] = progs;
        }
        // --- Filter ---
        {
        QJsonArray filter;
        QJsonArray filterCon;
        for(const auto & i : this->filter){
            QJsonObject e;
            QJsonObject con;
            con["source"] = QString::number(reinterpret_cast<size_t>(i.second.source.get()));
            e["typename"] = reinterpret_cast<Named*>(i.second.source.get())->getName();
            e["id"] = QString::number(reinterpret_cast<size_t>(i.second.source.get()));
            con["level"] = i.first;
            static_assert (sizeof (size_t) == sizeof (Filter*), "Pointer must have same size as size_t");
            filter.push_back(e);
            QJsonArray targets;
            for(const auto & c : i.second.targeds){
                QJsonObject target;
                target["target"] = QString::number(reinterpret_cast<size_t>(c.second.targed));
                target["length"] = static_cast<int>(c.first);
                target["targetStartIndex"] = static_cast<int>(c.second.startIndex);
                targets.push_back(target);
            }
            con["targets"] = targets;
        }
        o["filter"] = filter;
        o["filterConnections"] = filterCon;
        }
        // --- Consumer ---
        {
            QJsonArray consumer;
            QJsonArray consumerCon;
            for(const auto & i : this->consumer){
                QJsonObject e;
                QJsonObject con;
                con["source"] = QString::number(reinterpret_cast<size_t>(i.source.get()));
                e["typename"] = reinterpret_cast<Named*>(i.source.get())->getName();
                e["id"] = QString::number(reinterpret_cast<size_t>(i.source.get()));
                {
                    QJsonObject prop;
                    // TODO save properties of object
                    //e["properties"] =
                }
                static_assert (sizeof (size_t) == sizeof (Filter*), "Pointer must have same size as size_t");
                consumer.push_back(e);
                QJsonArray targets;
                for(const auto & c : i.targeds){
                    QJsonObject target;
                    target["target"] = QString::number(reinterpret_cast<size_t>(c.second.targed));
                    target["length"] = static_cast<int>(c.first);
                    target["targetStartIndex"] = static_cast<int>(c.second.startIndex);
                    targets.push_back(target);
                }
                con["targets"] = targets;
                consumerCon.push_back(con);
            }
            o["consumer"] = consumer;
            o["consumerConnections"] = consumerCon;
        }
    }




}
