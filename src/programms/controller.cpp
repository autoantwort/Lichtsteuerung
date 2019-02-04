#include "controller.h"
#include <mutex>

namespace Modules {

Controller::Controller():run_(false)
{

}

void Controller::run() noexcept{
    while (run_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::unique_lock<std::mutex> l(vectorLock);
        for(auto pb = runningProgramms.begin() ; pb != runningProgramms.end();){
            if((*pb)->doStep(1)){
                deletingProgramBlock = (*pb).get();
                (**pb).stop();
                deletingProgramBlock = nullptr;
                pb = runningProgramms.erase(pb);
            }else{
                ++pb;
            }
        }
    }
}

void Controller::runProgramm(std::shared_ptr<ProgramBlock> pb){
    std::unique_lock<std::mutex> l(vectorLock);
    runningProgramms.push_back(pb);  
}

void Controller::stopProgramm(std::shared_ptr<ProgramBlock> pb){
    // preventing deadlocks, this method is called when call stop on a ProgramBlock, wich is called in function run
    if(deletingProgramBlock == pb.get())
        return;
    std::unique_lock<std::mutex> l(vectorLock);
    runningProgramms.erase(std::remove(runningProgramms.begin(),runningProgramms.end(),pb),runningProgramms.end());    
}
void Controller::stopProgramm(ProgramBlock* pb){
    std::unique_lock<std::mutex> l(vectorLock);
    runningProgramms.erase(std::remove_if(runningProgramms.begin(),runningProgramms.end(),[&](const auto &v){return v.get()==pb;}),runningProgramms.end());    
}

bool Controller::isProgramRunning(ProgramBlock * pb){
    std::unique_lock<std::mutex> l(vectorLock);
    return std::any_of(runningProgramms.cbegin(),runningProgramms.cend(),[=](const auto & p){return p.get()==pb;});
}

}
