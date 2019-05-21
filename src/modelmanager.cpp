#include "modelmanager.h"


DMX::Device *ModelManager::getDeviceById(ID::value_type id){
    for(const auto & d : devices){
        if(d->getID() == id){
            return d.get();
        }
    }
    return nullptr;
}

DMX::DevicePrototype *ModelManager::getDevicePrototypeById(ID::value_type id){
    for(const auto & d : devicePrototypes){
        if(d->getID() == id){
            return d.get();
        }
    }
    return nullptr;
}

DMX::Programm *ModelManager::getProgramById(ID::value_type id){
    for(const auto & d : programs){
        if(d->getID() == id){
            return d.get();
        }
    }
    return nullptr;
}

DMX::ProgrammPrototype *ModelManager::getProgramPrototypeById(ID::value_type id){
    for(const auto & d : programPrototypes){
        if(d->getID() == id){
            return d.get();
        }
    }
    return nullptr;
}

