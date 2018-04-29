#include "programblock.h"
#include <QJsonArray>

namespace Modules {


    namespace detail {

    }

    ProgramBlock::ProgramBlock()
    {

    }

    ProgramBlock::ProgramBlock(const QJsonObject& o){
        QJsonArray a;
        /*for(const auto & i : programs){
            a.push_back(i->get)
        }*/
        //TODO
    }


    bool ProgramBlock::doStep(time_diff_t diff){
        bool finished = false;
        for(auto & p : programs){
            const auto state = p->doStep(diff);
            finished |= state.finished;
            dataChanged[p] = state.outputDataChanged;
        }
        for(auto i = filter.cbegin(); i!= filter.cend();++i){
            if(doConnection(i->second)){
                static_cast<Filter*>(i->second.source)->filter();
            }
            dataChanged[i->second.source] |= static_cast<Filter*>(i->second.source)->doStep(diff);
        }
        for(auto i = consumer.cbegin(); i != consumer.cend();++i){
            if(doConnection(*i)){
                static_cast<Consumer*>(i->source)->show();
            }
        }
        return finished;
    }





}
