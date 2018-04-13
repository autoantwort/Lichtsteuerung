#ifndef PROGRAMBLOCK_H
#define PROGRAMBLOCK_H

#include "programm.h"
#include "filter.h"
#include "consumer.h"
#include <stdexcept>
#include <map>

namespace Modules {

    namespace detail{

    class Range{
        unsigned int startIndex, endIndex;
    public:
        void setRange(unsigned int start, unsigned int end){
            if(end>start){

            }
        }
    ];

        template<typename Source, typename Targed>
    class Connection{
        Range sourceRange;
        std::vector<std::pair<int, Targed
    };
    }

    class ProgramBlock
    {
        /*  Input n programms
            m filters
            k consumer

            Functions:
            save to json, save names of modules
            load from json, get object from Module Manager throug name
        */
        std::vector<Programm*> programs;
        std::map<int,detail::Connection<Filter*,Programm*>> filter;
        std::vector<detail::Connection<Consumer*,Filter*> consumer;
    public:
        ProgramBlock();
    };

}

#endif // PROGRAMBLOCK_H
