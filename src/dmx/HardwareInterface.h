#ifndef DMXINTERFACE_H
#define DMXINTERFACE_H
#include <QString>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace DMX {

/**
 * @brief The DMXInterface class soll eine Standard schnittstelle für Treiber bereitstellen
 */
class HardwareInterface {
    std::function<void(QString)> errorCallback;
    std::function<void(unsigned char *, int, double)> setValuesCallback;

protected:
    void setValues(unsigned char *values, int size, double time) {
        if (setValuesCallback) {
            setValuesCallback(values, size, time);
        }
    }
    void error(QString s) {
        if (errorCallback) errorCallback(s);
    }

public:
    HardwareInterface() {}
    void setErrorCallback(std::function<void(QString)> errorCallback) { this->errorCallback = errorCallback; }
    void setSetValuesCallback(std::function<void(unsigned char *, int, double)> setValuesCallback) { this->setValuesCallback = setValuesCallback; }
    virtual bool init() = 0;
    virtual bool isFunctioning() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void setWaitTime(std::chrono::milliseconds) = 0;
    virtual std::chrono::milliseconds getWaitTime() const = 0;
    virtual ~HardwareInterface() {}
};

class AbstractHardwareInterface : public HardwareInterface {
    std::thread thread;
    std::mutex mutex;
    std::condition_variable wait;
    std::atomic_bool run;
    std::atomic_bool stopThread;
    std::chrono::milliseconds sleepTime;

private:
    void loop() {
        std::this_thread::yield();
        while (!stopThread.load()) {
            auto lock = std::unique_lock<std::mutex>(mutex);
            while (run.load() && isFunctioning()) {
                setValuesDeviceDriver();
                std::this_thread::sleep_for(sleepTime);
                time += sleepTime.count() / 1000.;
            }
            time = 0;
            if (stopThread.load() == false) {
                wait.wait(lock);
            }
        }
    }

protected:
    virtual void setValuesDeviceDriver() = 0;
    double time = 0;

public:
    AbstractHardwareInterface() : thread([this] { loop(); }), run(false), stopThread(false), sleepTime(33) {}
    virtual ~AbstractHardwareInterface() {
        run = false;
        stopThread = true;
        wait.notify_all();
        thread.join();
    }
    virtual void start() final override {
        run = true;
        wait.notify_all();
    }
    virtual void stop() final override { run = false; }
    virtual void setWaitTime(std::chrono::milliseconds m) override { sleepTime = m; }
    virtual std::chrono::milliseconds getWaitTime() const override { return sleepTime; }
};

} // namespace DMX

#endif // DMXINTERFACE_H
