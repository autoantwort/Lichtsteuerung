#ifndef PROGRAMBLOCK_H
#define PROGRAMBLOCK_H

#include "programm.hpp"
#include "filter.hpp"
#include "consumer.hpp"
#include <stdexcept>
#include <map>
#include <set>
#include <QJsonObject>
#include <memory>


namespace Modules {

    namespace detail{

        struct Connection{
            struct TargedInfo{
                OutputDataProducer* targed;
                unsigned int startIndex;
            };
            typedef unsigned int length_t;
            std::shared_ptr<InputDataConsumer> source;
            Connection(std::shared_ptr<InputDataConsumer> source):source(source){}
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

    /**
     * @brief The ProgramBlock class is the combination of programms, filter and Consumer. THE PROGRAMM_BLOCK OWNS THE POINTER!
     * Es legt fest welche Bereiche des outputs in welche bereiche des inputs gelangen(Programm -> Filter -> Consumer)
     */
    class ProgramBlock : public AbstractProgramm
    {
        /*  Input n programms
            m filters
            k consumer

            Functions:
            save to json, save names of modules
            load from json, get object from Module Manager throug name
        */

        std::set<std::shared_ptr<Programm>> programs;
        // verschiedene Ebenen von Filtern
        std::map<int,detail::Connection> filter;
        // a list of all consumer and their connections
        std::vector<detail::Connection> consumer;

        // saves for every Programm, filter, consumer, the state, whether the output data changed
        std::map<void*,bool> dataChanged;
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
                    if(dataChanged[c.second.targed]){
                        char * start = reinterpret_cast<char*>(c.second.targed->getOutputData());
                        start += c.second.startIndex * getSizeOfEnumType(c.second.targed->getOutputType());
                        connection.source->setInputData(start,sourceStartIndex,c.first);
                        sourceStartIndex += c.first;
                        outputUpdated = true;
                    }
                }
            }
            dataChanged[connection.source.get()]=outputUpdated;
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
                if(reinterpret_cast<OutputDataProducer*>(c.second.source.get())==p){
                    return true;
                }
            }
            return false;
        }
    protected:
        /**
         * @brief addProgramm adds a pointer to a programm. the pointer is owned
         */
        void addProgramm(Programm *p){programs.insert(std::shared_ptr<Programm>(p));}
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
    public:
        ProgramBlock();
        ProgramBlock(const QJsonObject&);
        virtual bool doStep(time_diff_t) = 0;
        void writeJsonObject(QJsonObject&);
    };

}

#endif // PROGRAMBLOCK_H
