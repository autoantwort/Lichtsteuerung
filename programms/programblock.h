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

    /**
     * @brief The ProgramBlock class is the combination of programms, filter and Consumer. THE PROGRAMM_BLOCK OWNS THE POINTER!
     * Es legt fest welche Bereiche des outputs in welche bereiche des inputs gelangen(Programm -> Filter -> Consumer)
     */
    class ProgramBlock : public QObject, public AbstractProgramm
    {
        friend class ModuleManager;
        /*  Input n programms
            m filters
            k consumer

            Functions:
            save to json, save names of modules
            load from json, get object from Module Manager throug name
        */
        QString name;
        Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
        Q_OBJECT

        std::set<std::shared_ptr<Program>> programs;
        // verschiedene Ebenen von Filtern
        std::map<int,detail::Connection> filter;
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
            std::remove_if(consumer.begin(),consumer.end(),f);
        }
        const std::set<std::shared_ptr<Program>> & getPrograms()const{return programs;}
        // verschiedene Ebenen von Filtern
        const std::map<int,detail::Connection> & getFilter()const{return filter;}
        // a list of all consumer and their connections
        const std::vector<detail::Connection> getConsumer()const{return consumer;}

    public:
        ProgramBlock(QString name = "No name"):name(name){}
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
    public:
        ProgramBlock(const QJsonObject&);
        virtual bool doStep(time_diff_t)override;
        /**
         * @brief start starts all used Programs and Consumer
         */
        void start();
        /**
         * @brief stop stop all used Consumer
         */
        void stop();
        void writeJsonObject(QJsonObject&);
    };



    class ProgrammBlockVector : public ModelVector<std::shared_ptr<ProgramBlock>>{
        Q_OBJECT
    };

    class ProgramBlockManager{
    public:
        static ProgrammBlockVector model;
        static void writeToJsonObject(QJsonObject&);
        static void readFromJsonObject(const QJsonObject&);
    };


}

/*namespace std {
template<>
  struct __is_pointer_helper<std::shared_ptr<Modules::ProgramBlock>>
  : public true_type { };
}*/
#endif // PROGRAMBLOCK_H
