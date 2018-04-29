#ifndef PROGRAMBLOCK_H
#define PROGRAMBLOCK_H

#include "programm.hpp"
#include "filter.hpp"
#include "consumer.hpp"
#include <stdexcept>
#include <map>
#include <QJsonObject>


namespace Modules {

    namespace detail{

        struct Connection{
            struct TargedInfo{
                OutputDataProducer* targed;
                unsigned int startIndex;
            };
            typedef unsigned int length_t;
            InputDataConsumer* source;
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
     * @brief The ProgramBlock class is the combination of programms, filter and Consumer.
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
        std::vector<Programm*> programs;
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
            dataChanged[connection.source]=outputUpdated;
            return outputUpdated;
        }
    public:
        ProgramBlock();
        ProgramBlock(const QJsonObject&);
        virtual bool doStep(time_diff_t) = 0;
        void writeJsonObject(QJsonObject&);
    };

}

#endif // PROGRAMBLOCK_H
