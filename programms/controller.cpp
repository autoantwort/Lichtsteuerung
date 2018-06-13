#include "controller.h"
#include <mutex>

namespace Modules {

Controller::Controller()
{

}

void Controller::run() noexcept{
    while (run_) {

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

}
