#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

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
};
}

#endif // CONTROLLER_H
