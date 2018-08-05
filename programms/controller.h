#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "programms/programblock.h"

namespace Modules {

/**
 * @brief The Controller class controlls the whole Module system. Its load and execute it
 */
class Controller
{
    std::atomic_bool run_;
    std::thread thread;
    std::mutex mutex;
    std::condition_variable wait;
    std::vector<std::shared_ptr<ProgramBlock>> runningProgramms;
    std::mutex vectorLock;
    void run() noexcept;
public:
    Controller();
    void start(){
        if(!run_){
            run_=true;
            thread = std::thread([this](){
                run();
            });
        }
    }
    void stop(){run_=false;}
    ~Controller(){run_ = false;thread.join();}
    void runProgramm(std::shared_ptr<ProgramBlock> pb);
    void stopProgramm(std::shared_ptr<ProgramBlock> pb);
};
}

#endif // CONTROLLER_H
