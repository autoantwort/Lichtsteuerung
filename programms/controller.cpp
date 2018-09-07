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
                (**pb).stop();
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
    pb->start();
}

void Controller::stopProgramm(std::shared_ptr<ProgramBlock> pb){
    std::unique_lock<std::mutex> l(vectorLock);
    runningProgramms.erase(std::remove(runningProgramms.begin(),runningProgramms.end(),pb),runningProgramms.end());
    pb->stop();
}
void Controller::stopProgramm(ProgramBlock* pb){
    std::unique_lock<std::mutex> l(vectorLock);
    runningProgramms.erase(std::remove_if(runningProgramms.begin(),runningProgramms.end(),[&](const auto &v){return v.get()==pb;}),runningProgramms.end());
    pb->stop();
}

bool Controller::isProgramRunning(ProgramBlock * pb){
    std::unique_lock<std::mutex> l(vectorLock);
    return std::any_of(runningProgramms.cbegin(),runningProgramms.cend(),[=](const auto & p){return p.get()==pb;});
}

}
