#include "programblock.h"
#include <QJsonArray>
#include <stdexcept>
#include "modulemanager.h"
#include "json_storage.h"


namespace Modules {


    ProgrammBlockVector ProgramBlockManager::model;

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



    template<typename T>
    void loadProperties(const QJsonObject &o, T*t){
        const auto prop = o["properties"].toObject();
        JsonLoadObject lo(prop);
        t->load(lo);
    }

    ProgramBlock::ProgramBlock(const QJsonObject& o):name(o["name"].toString()),id(o){
        // Wir erstellen erstmal alle objecte mit passendem Typ und ID
        std::map<QString,std::shared_ptr<Program>> programs;
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
                loadProperties(obj,p.get());
                programs.insert(std::make_pair(obj["id"].toString(),p));
                this->programs.insert(p);
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
                loadProperties(obj,p.get());
                filter.insert(std::make_pair(obj["id"].toString(),p));
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
                loadProperties(obj,p.get());
                consumer.insert(std::make_pair(obj["id"].toString(),p));
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
        assert(c.sourceType == detail::Connection::Filter);
        filter.emplace(layer,std::move(c));
    }

    void ProgramBlock::addConsumer(detail::Connection c){
        for(const auto & con : c.targeds){
            if(!haveOutputDataProducer(con.second.targed)){
                throw std::runtime_error("one outputdataproducer is unknown");
            }
        }
        assert(c.sourceType == detail::Connection::Consumer);
        consumer.push_back(std::move(c));
    }


    std::vector<std::shared_ptr<Program>> ProgramBlock::getUsedProgramsByName(const std::string &name)const{
        std::vector<std::shared_ptr<Program>> vec;
        for(const auto & p : programs){
            if(std::strcmp(p->getName(),name.c_str())==0)
                vec.push_back(p);
        }
        return vec;
    }
    std::vector<std::shared_ptr<Consumer>> ProgramBlock::getUsedConsumersByName(const std::string &name)const{
        std::vector<std::shared_ptr<Consumer>> vec;
        for(const auto & p : consumer){
            if(std::strcmp(dynamic_cast<Named*>(p.source.get())->getName(),name.c_str())==0)
                vec.push_back(std::dynamic_pointer_cast<Consumer>(p.source));
        }
        return vec;
    }
    std::vector<std::shared_ptr<Filter>> ProgramBlock::getUsedFiltersByName(const std::string &name)const{
        std::vector<std::shared_ptr<Filter>> vec;
        for(const auto & p : filter){
            if(std::strcmp(dynamic_cast<Named*>(p.second.source.get())->getName(),name.c_str())==0)
                vec.push_back(std::dynamic_pointer_cast<Filter>(p.second.source));
        }
        return vec;
    }

    void ProgramBlock::replaceProgram(std::shared_ptr<Program> original, std::shared_ptr<Program> newProgram){
        const auto it = programs.find(original);
        if(it==programs.cend())
            return;
        transferProperties(original.get(),newProgram.get());
        bool isRunning = ModuleManager::singletone()->controller().isProgramRunning(this);
        programs.erase(it);
        programs.insert(newProgram);
        if(isRunning)
            newProgram->start();
        for(auto & f : filter){
            for(auto &con : f.second.targeds){
                if(con.second.targed == original.get()){
                    con.second.targed = newProgram.get();
                }
            }
        }
        for(auto & f : consumer){
            for(auto &con : f.targeds){
                if(con.second.targed == original.get()){
                    con.second.targed = newProgram.get();
                }
            }
        }
        emit propertyBaseChanged(original.get(),newProgram.get());
    }

    void ProgramBlock::replaceFilter(std::shared_ptr<Filter> original, std::shared_ptr<Filter> newFilter){
        transferProperties(original.get(),newFilter.get());
        bool foundOne = false;
        for(auto & f : filter){
            if(f.second.source == original){
                f.second.source = newFilter;
                foundOne = true;
            }
            for(auto & t : f.second.targeds){
                if(t.second.targed == original.get()){
                    t.second.targed = newFilter.get();
                }
            }
        }
        for(auto & f : consumer){
            for(auto &con : f.targeds){
                if(con.second.targed == original.get()){
                    con.second.targed = newFilter.get();
                }
            }
        }
        if(foundOne)
            emit propertyBaseChanged(original.get(),newFilter.get());
    }

    void ProgramBlock::replaceConsumer(std::shared_ptr<Consumer> original, std::shared_ptr<Consumer> newConsumer){
        if(std::find_if(std::cbegin(consumer),std::cend(consumer),[&](const auto &c){return c.source == original;}) == std::end(consumer))
            return;
        if(getStatus() == Running)
            original->stop();
        transferProperties(original.get(),newConsumer.get());
        for(auto & f : consumer){
            if(f.source == original){
                f.source = newConsumer;
            }
        }
        if(getStatus() == Running)
            newConsumer->start();
        emit propertyBaseChanged(original.get(),newConsumer.get());
    }


    void ProgramBlock::restart(Controller * c){
        stop();
        start(c);
    }

    void ProgramBlock::start(){
        start(&ModuleManager::singletone()->controller());
    }

    void ProgramBlock::restart(){
        restart(&ModuleManager::singletone()->controller());
    }
    void ProgramBlock::resume(){
        resume(&ModuleManager::singletone()->controller());
    }

    void ProgramBlock::start(Controller * c){
        if(status!=Running && c){
            for(auto & p : programs){
                p->start();
            }
            for(auto i = consumer.cbegin(); i != consumer.cend();++i){
                static_cast<Consumer*>(i->source.get())->start();
            }
            setStatus(Running);
            c->runProgramm(shared_from_this());
            controller = c;
        }
    }

    void ProgramBlock::stop(){
        if(status==Running && controller){
            for(auto i = consumer.cbegin(); i != consumer.cend();++i){
                static_cast<Consumer*>(i->source.get())->stop();
            }
            setStatus(Stopped);
            controller->stopProgramm(shared_from_this());
            controller = nullptr;
        }
    }

    void ProgramBlock::pause(){
        if(status==Running&&controller){
            for(auto i = consumer.cbegin(); i != consumer.cend();++i){
                static_cast<Consumer*>(i->source.get())->stop();
            }
            setStatus(Paused);
            controller->stopProgramm(shared_from_this());
            controller = nullptr;
        }
    }

    void ProgramBlock::resume(Controller * c){
        if(status==Paused && c){
            //start consumer
            for(auto i = consumer.cbegin(); i != consumer.cend();++i){
                static_cast<Consumer*>(i->source.get())->start();
            }
            setStatus(Running);
            controller = c;
            controller->runProgramm(shared_from_this());
        }else if (status == Stopped) {
            start(c);
        }
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
            dataChanged[dynamic_cast<Named*>(i->second.source.get())] |= static_cast<Filter*>(i->second.source.get())->doStep(diff);
        }
        for(auto i = consumer.cbegin(); i != consumer.cend();++i){
            if(doConnection(*i)){
                static_cast<Consumer*>(i->source.get())->show();
            }
            static_cast<Consumer*>(i->source.get())->doStep(diff);
        }
        return finished;
    }

    template<typename Type>
    void saveProperties(QJsonObject &o, Type * p){
        QJsonObject prop;
        JsonSaveObject jo(prop);
        p->save(jo);
        o["properties"] = prop;
    }

    void ProgramBlock::writeJsonObject(QJsonObject &o){
        id.writeJsonObject(o);
        o["name"] = name;
        // Wir speichern zu jedem Programm/Filter/Consumer ein Object ab, das den Typnamen und eine ID, wir nehmen die Adresse im Arbeitsspeicher.
        // Wir speichern die Connections, indem wir für die Source und die Targets die Adressen(IDs) der jeweiligen Pointer speichern.
        // --- Programs ---
        {QJsonArray progs;
        for(auto i = programs.cbegin();i!=programs.cend();++i){
            QJsonObject p;
            p["typename"] = (*i)->getName();
            p["id"] = QString::number(reinterpret_cast<size_t>(static_cast<Named*>(i->get())));
            saveProperties(p,i->get());
            static_assert (sizeof (size_t) == sizeof (Program*), "Pointer must have same size as size_t");
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
            con["source"] = QString::number(reinterpret_cast<size_t>(dynamic_cast<Named*>(i.second.source.get())));
            e["typename"] = dynamic_cast<Named*>(i.second.source.get())->getName();
            e["id"] = QString::number(reinterpret_cast<size_t>(dynamic_cast<Named*>(i.second.source.get())));
            saveProperties(e,static_cast<Filter*>(i.second.source.get()));
            con["level"] = i.first;
            static_assert (sizeof (size_t) == sizeof (Filter*), "Pointer must have same size as size_t");
            filter.push_back(e);
            QJsonArray targets;
            for(const auto & c : i.second.targeds){
                QJsonObject target;
                target["target"] = QString::number(reinterpret_cast<size_t>(dynamic_cast<Named*>(c.second.targed)));
                target["length"] = static_cast<int>(c.first);
                target["targetStartIndex"] = static_cast<int>(c.second.startIndex);
                targets.push_back(target);
            }
            con["targets"] = targets;
            filterCon.push_back(con);
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
                con["source"] = QString::number(reinterpret_cast<size_t>(dynamic_cast<Named*>(i.source.get())));
                e["typename"] = dynamic_cast<Named*>(i.source.get())->getName();
                e["id"] = QString::number(reinterpret_cast<size_t>(dynamic_cast<Named*>(i.source.get())));
                saveProperties(e,static_cast<Consumer*>(i.source.get()));
                static_assert (sizeof (size_t) == sizeof (Filter*), "Pointer must have same size as size_t");
                consumer.push_back(e);
                QJsonArray targets;
                for(const auto & c : i.targeds){
                    QJsonObject target;
                    target["target"] = QString::number(reinterpret_cast<size_t>(dynamic_cast<Named*>(c.second.targed)));
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

    void ProgramBlockManager::writeToJsonObject(QJsonObject & o){
        QJsonArray a;
        for(const auto & m : model){
            QJsonObject o1;
            m->writeJsonObject(o1);
            a.append(o1);
        }
        o.insert("model",a);
    }
    void ProgramBlockManager::readFromJsonObject(const QJsonObject & o){
        auto a = o["model"].toArray();
        for(auto val_:a){
            QJsonObject ob = val_.toObject();
            try {
                model.push_back(std::make_shared<ProgramBlock>(ob));
            } catch (std::runtime_error e) {
                qDebug() << "Cant parse ProgramBlock : " << e.what();
            }
        }
    }


}
