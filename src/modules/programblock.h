#ifndef PROGRAMBLOCK_H
#define PROGRAMBLOCK_H

#include "program.hpp"
#include "filter.hpp"
#include "consumer.hpp"
#include <stdexcept>
#include <map>
#include <set>
#include <QJsonObject>
#include <QObject>
#include <memory>
#include <modelvector.h>
#include "id.h"


namespace Modules {

    namespace detail{

        struct Connection{
            struct TargedInfo{
                OutputDataProducer* targed;
                unsigned int startIndex;
            };
            typedef unsigned int length_t;
            enum SourceType {Filter,Consumer} sourceType;
            std::shared_ptr<InputDataConsumer> source;
            template<typename Type>
            /**
             * @brief Connection Verbindet den Input eines InputDataConsumers mit dem Output von OutputDataProducern
             * @param source Der InputConsumer
             */
            explicit Connection(std::shared_ptr<Type> source):sourceType(Filter),source(source){
                static_assert (std::is_base_of<InputDataConsumer,Type>::value,"Der Typ ist keine Subklasse eines InputDataConsumer.");
                static_assert (std::is_base_of<Modules::Filter,Type>::value||std::is_base_of<Modules::Consumer,Type>::value,"Der Typ ist Filter oder Consumer.");
                if(std::is_base_of<Modules::Filter,Type>::value){
                    *const_cast<SourceType*>(&sourceType) = Filter;
                }else{
                    *const_cast<SourceType*>(&sourceType) = Consumer;
                }
            }
            Connection(const QJsonObject &o);
            /**
             * @brief addTarget add a target for a range in the InputDataConsumer
             * @param length the length in the InputDataConsumer input array, starting at the sum of the previous targets lengths
             * @param target the OutputDataProducer
             * @param targetStartIndex the start index in the output array of the OutputDataProducer
             */
            void addTarget(length_t length, OutputDataProducer* target, unsigned int targetStartIndex){
                targeds.push_back(std::pair<length_t,TargedInfo>(length,{target,targetStartIndex}));
            }
            //maps block in Source Range to Targed Range
            /*
             * eg:
             * T=Targed(index)
             *          T1(3)    T1(50)
             *          |   T2(0)|
             * Source:|___|_____|__|
             *
             */
            std::vector<std::pair<length_t, TargedInfo>> targeds;
            /**
             * @brief isValid checks if the InputDataConsumer and the OutputDataProducer have the same ValueType(rgb_t,brightness_t).
             * @return true if valid;
             */
            bool isValid()const {
                for(const auto & r : targeds){
                    if(r.second.targed->getOutputType()!=source->getInputType()){
                        return false;
                    }
                }
                return true;
            }
        };
    }

    class ModuleManager;
    class Controller;

    /**
     * @brief The ProgramBlock class is the combination of programms, filter and Consumer. THE PROGRAMM_BLOCK OWNS THE POINTER!
     * Es legt fest welche Bereiche des outputs in welche bereiche des inputs gelangen(Programm -> Filter -> Consumer)
     */
    class ProgramBlock : public QObject, public AbstractProgramm, public std::enable_shared_from_this<ProgramBlock>
    {
        Q_OBJECT
        friend class ModuleManager;
        /*  Input n programms
            m filters
            k consumer

            Functions:
            save to json, save names of modules
            load from json, get object from Module Manager throug name
        */
        QString name;
        Controller * controller = nullptr;
        ID id;
    public:
        enum Status{Stopped=0, Running=1, Paused=2}status = Stopped;
        Q_ENUM(Status)
    private:
        Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(Status status READ getStatus NOTIFY statusChanged)


        std::set<std::shared_ptr<Program>> programs;
        // verschiedene Ebenen von Filtern
        std::multimap<int,detail::Connection> filter;
        // a list of all consumer and their connections
        std::vector<detail::Connection> consumer;

        // saves for every Programm, filter, consumer, the state, whether the output data changed
        std::map<Named*,bool> dataChanged;
    private:
        // Funktionen um Module neu zu laden
        std::vector<std::shared_ptr<Program>> getUsedProgramsByName(const std::string &)const;
        std::vector<std::shared_ptr<Consumer>> getUsedConsumersByName(const std::string &)const;
        std::vector<std::shared_ptr<Filter>> getUsedFiltersByName(const std::string &)const;
        /**
         * @brief replaceProgram ersetzt alle vorkomnisse von dem Programm durch das neue Programm
         * @param original Das zu ersetztende Programm
         * @param newProgram Das neue Programm
         */
        void replaceProgram(std::shared_ptr<Program> original, std::shared_ptr<Program> newProgram);
        /**
         * @brief replaceConsumer ersetzt alle vorkomnisse von dem Consumer durch das neue Consumer
         * @param original Das zu ersetztende Consumer
         * @param newConsumer Das neue Consumer
         */
        void replaceConsumer(std::shared_ptr<Consumer> original, std::shared_ptr<Consumer> newConsumer);
        /**
         * @brief replaceFilter ersetzt alle vorkomnisse von dem Filter durch das neue Filter
         * @param original Das zu ersetztende Filter
         * @param newFilter Das neue Filter
         */
        void replaceFilter(std::shared_ptr<Filter> original, std::shared_ptr<Filter> newFilter);
    private:
        /**
         * @brief doConnection copy from the targeds output to the source input
         * @param connection the connection to process
         * @return true if one of the outputs change and the filter/consumer should run
         */
        bool doConnection(const detail::Connection & connection){
            bool outputUpdated = false;
            if(connection.isValid()){
                unsigned int sourceStartIndex = 0;                
                for(const auto & c : connection.targeds){
                    if(dataChanged[dynamic_cast<Named*>(c.second.targed)]){
                        char * start = reinterpret_cast<char*>(c.second.targed->getOutputData());
                        start += c.second.startIndex * getSizeOfEnumType(c.second.targed->getOutputType());
                        connection.source->setInputData(start,sourceStartIndex,c.first);
                        sourceStartIndex += c.first;
                        outputUpdated = true;
                    }
                }
            }
            dataChanged[dynamic_cast<Named*>(connection.source.get())]=outputUpdated;
            return outputUpdated;
        }
        bool haveOutputDataProducer(OutputDataProducer * p){
            // egal wenn wir einen anderen Pointer da raus machen, wir wollen nur die addresse
            for(const auto &c : programs){
                // alle filter sind auch OutputDataProducer und wenn nicht, dann kann p nicht die gleiche addresse wie einer haben
                if(c.get()==p){
                    return true;
                }
            }
            for(const auto &c : filter){
                // alle filter sind auch OutputDataProducer und wenn nicht, dann kann p nicht die gleiche addresse wie einer haben
                if(dynamic_cast<OutputDataProducer*>(c.second.source.get())==p){
                    return true;
                }
            }
            return false;
        }
        void removeConnectionsToOutputDataProducer(OutputDataProducer*out){
            for(auto & i : filter){
                // entfere alle targets ab der ersten verbinndung zu dem zu entfernenem filter
                auto first = i.second.targeds.cend();
                for(auto t = i.second.targeds.cbegin() ; t != i.second.targeds.cend();++t){
                    if(t->second.targed == out){
                        first = t;
                        break;
                    }
                }
                i.second.targeds.erase(first,i.second.targeds.cend());
            }
            for(auto & i : consumer){
                // entfere alle targets ab der ersten verbinndung zu dem zu entfernenem filter
                auto first = i.targeds.cend();
                for(auto t = i.targeds.cbegin() ; t != i.targeds.cend();++t){
                    if(t->second.targed == out){
                        first = t;
                        break;
                    }
                }
                i.targeds.erase(first,i.targeds.cend());
            }
        }
    public:
        /**
         * @brief addProgramm adds a pointer to a programm. the pointer is owned
         */
        void addProgramm(std::shared_ptr<Program> p){programs.insert(p);}
        /**
         * @brief addFilter add filter. throw exeption if one datasource is unknown
         * @param c the connection
         * @param layer the layer with lower index get executed first
         */
        void addFilter(detail::Connection c, int layer);
        /**
         * @brief addConsumer add consumer. throw an exeption if one datasource is unknown
         * @param c
         */
        void addConsumer(detail::Connection c);
        template<typename F>
        void removeConsumer(F f){
            // see https://stackoverflow.com/questions/24263259/c-stdseterase-with-stdremove-if
            for (auto it{consumer.begin()}; it != consumer.end(); ) {
                if (f(*it)) {
                    it = consumer.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
        template<typename F>
        void removeFilter(F f){
            // see https://stackoverflow.com/questions/24263259/c-stdseterase-with-stdremove-if
            for (auto it{filter.begin()}; it != filter.end(); ) {
                if (f(*it)) {
                    removeConnectionsToOutputDataProducer(static_cast<Filter*>(it->second.source.get()));
                    it = filter.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
        template<typename F>
        void removeProgram(F f){
            // see https://stackoverflow.com/questions/24263259/c-stdseterase-with-stdremove-if
            for (auto it{programs.begin()}; it != programs.end(); ) {
                if (f(*it)) {
                    removeConnectionsToOutputDataProducer(static_cast<Program*>(it->get()));
                    it = programs.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
        void removeConsumer(Consumer * c){
            removeConsumer([=](const auto &f){return f.source.get()==c;});
        }
        void removeFilter(Filter * filter){
            removeFilter([=](const auto &f){return f.second.source.get()==filter;});
        }
        void removeProgram(Program * program){
            removeProgram([=](const auto &f){return f.get()==program;});
        }
        const std::set<std::shared_ptr<Program>> & getPrograms()const{return programs;}
        // verschiedene Ebenen von Filtern
        const std::multimap<int,detail::Connection> & getFilter()const{return filter;}
        // a list of all consumer and their connections
        const std::vector<detail::Connection> & getConsumer()const{return consumer;}

         std::set<std::shared_ptr<Program>> & getPrograms(){return programs;}
        // verschiedene Ebenen von Filtern
         std::multimap<int,detail::Connection> & getFilter(){return filter;}
        // a list of all consumer and their connections
         std::vector<detail::Connection> & getConsumer(){return consumer;}

    public:
        ProgramBlock(QString name = "No name"):name(name){}
        ID getID()const{return id;}
        void setName( const QString _name){
                if(_name != name){
                        name = _name;
                        emit nameChanged();
                }
        }
        QString getName() const {
                return name;
        }
    signals:
        void nameChanged();
        void statusChanged();
        /**
         * @brief propertyBaseChanged this signal is emitted, when replace* is called and one Program/Filter/Consumer is affected
         * @param oldPB the PropertyBase of the old Program/Filter/Consumer
         * @param newPB the PropertyBase of the new Program/Filter/Consumer
         */
        void propertyBaseChanged(PropertyBase * oldPB, PropertyBase * newPB);
    public:
        ProgramBlock(const QJsonObject&);
        virtual bool doStep(time_diff_t)override;
        /**
         * @brief start start the program block if not running
         */
        void start(Controller * c);
        Q_INVOKABLE void start();
        /**
         * @brief restart this function restart or start the program
         */
        void restart(Controller * c);
        Q_INVOKABLE void restart();
        /**
         * @brief stop stop all used Consumer
         */
        Q_INVOKABLE void stop();
        /**
         * @brief pause an running program or do nothing
         */
        Q_INVOKABLE void pause();
        /**
         * @brief resume resume an paused program or start an stopped program
         */
        void resume(Controller * c);
        Q_INVOKABLE void resume();
        Status getStatus()const{return status;}
        void writeJsonObject(QJsonObject&);
    private:
        /**
         * @brief setStatus use this function to set the status, this function emit a signal when the status changed
         * @param s the new status
         */
        void setStatus(Status s){
            if(s!=status){
                status = s;
                emit statusChanged();
            }
        }
    };



    class ProgrammBlockVector : public ModelVector<std::shared_ptr<ProgramBlock>>{
        Q_OBJECT
    public:
        virtual QVariant data(const QModelIndex &index, int role) const override{
            auto res = ModelVector<std::shared_ptr<ProgramBlock>>::data(index,role);
            if(res.isValid() && res.type() != QVariant::String && role == Qt::DisplayRole){
                return res.value<ProgramBlock*>()->getName();
            }
            return res;
        }
    };

    class ProgramBlockManager{
    public:
        static ProgrammBlockVector model;
        static void writeToJsonObject(QJsonObject&);
        static void readFromJsonObject(const QJsonObject&);
    };


}
Q_DECLARE_METATYPE(Modules::ProgramBlock::Status)

/*namespace std {
template<>
  struct __is_pointer_helper<std::shared_ptr<Modules::ProgramBlock>>
  : public true_type { };
}*/
#endif // PROGRAMBLOCK_H
