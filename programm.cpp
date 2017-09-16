#include "programm.h"



Programm::Programm()
{

}

void Programm::fill(unsigned char *data, size_t length, double time){
    // für jedes Programm
    for(const auto p : Programm::getAllIDBases()){
        if(p->isRunning()){
            //für jedes Device Programm
            for(const auto dp : p->programms){

                //für jedes Channel Programm
                for(const auto cp : dp.programmPrototype->getChannelProgramms()){
                    const auto channelNummer = dp.device->getStartDMXChannel() + cp.channel->getIndex();
                    if(channelNummer<length){
                        data[channelNummer] = cp.getValueForTime(time + dp.offset);
                    }
                }
            }
        }
    }
}
