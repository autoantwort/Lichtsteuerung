#include "dmxchannelfilter.h"
#include "programm.h"
#include "device.h"


void DMXChannelFilter::filterValue(unsigned char *value){
    if (shouldOverrideValue_) {
        *value = this->value;
    }else{
        if (minValue!=0&&maxValue!=255) {
            if(minOperation==REMAP&&maxOperation==REMAP){
                *value = (unsigned char)(*value * (maxValue-minValue)/255.f) + minValue;
            }else if(minOperation==CUT&&maxOperation==REMAP){
                *value = (unsigned char)(*value * maxValue/255.f);
                if(*value<minValue)
                    *value = minValue;
            }else if(minOperation==REMAP&&maxOperation==CUT){
                *value = (unsigned char)(*value * (255-minValue)/255.f) + minValue;
                if(*value>maxValue)
                    *value = maxValue;
            }else /*if(minOperation==CUT&&maxOperation==CUT)*/{
                if(*value<minValue)
                    *value = minValue;
                if(*value>maxValue)
                    *value = maxValue;
            }
        }else if(minValue!=0){
            if (minOperation==CUT) {
                if(*value<minValue)
                    *value = minValue;
            }else{ //REMAP
                *value = (unsigned char)(*value * (255-minValue)/255.f) + minValue;
            }
        }else if(maxValue!=255){
            if (maxOperation==CUT) {
                if(*value>maxValue)
                    *value = maxValue;
            }else{ //REMAP
                *value = (unsigned char)(*value * maxValue/255.f);
            }
        }
    }
}

void DMXChannelFilter::initValue(unsigned char *value){
    *value = this->value;
}

void DMXChannelFilter::initValues(unsigned char *values, unsigned int numberOfChannels){
    for(const auto d : IDBase<Device>::getAllIDBases()){
        for(const auto f : d->getChannelFilter()){
            const auto index = d->getStartDMXChannel()+f.first->getIndex();
            if(index<numberOfChannels){
                f.second->initValue(values+index);
            }
        }
    }
}

void DMXChannelFilter::filterValues(unsigned char *values, unsigned int numberOfChannels){
    for(const auto d : IDBase<Device>::getAllIDBases()){
        for(const auto f : d->getChannelFilter()){
            const auto index = d->getStartDMXChannel()+f.first->getIndex();
            if(index<numberOfChannels){
                f.second->filterValue(values+index);
            }
        }
    }
}
