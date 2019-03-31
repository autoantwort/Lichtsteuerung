#ifndef LOOPPROGRAMM_H
#define LOOPPROGRAMM_H

#include "program.hpp"
#include "types.h"
#include <memory>


namespace Modules {

class LoopProgram;

/**
 * @brief The LoopProgramContextSwitcher class is responsible for the context switching in the LoopProgramm
 */
class LoopProgramContextSwitcher{
    void (LoopProgram::*startFunction)() = nullptr;
    LoopProgram * loopProgram = nullptr;
protected:
    void startLoopProgram(){
        if(loopProgram && startFunction)
            (loopProgram->*startFunction)();
    }
public:
    /**
     * @brief yield The function should stop the execution of the startfunction and give the control back to the resume caller
     */
    virtual void yield()=0;
    /**
     * @brief resume the function should resume the execution of the function wich called the yield function
     * @return true if the startFunction is finished
     */
    virtual bool resume()=0;
    /**
     * @brief start must be called before you can use the resume and yield functions. This function should reset and initialize
     * the context switcher
     */
    virtual void start()=0;
    /**
     * @brief setStartFunction set the function wich contain the aktual loopProgramm
     * @param loopProgram the function that the contextSwitcher should call the first time when starts
     */
    void setStartFunction(LoopProgram * loopProgram, void (LoopProgram::*startFunction)()){
        this->loopProgram = loopProgram;
        this->startFunction = startFunction;
    }
    virtual ~LoopProgramContextSwitcher() = default;
};

/**
 * @brief Subklassen der LoopPogramm Klasse müssen nicht die doStep Methode implementieren, um einen Schritt
 * in ihrem Programm zu machen, sie können einfach die loopProgram Methode implementieren und um pausen zu machen
 * die wait Methode benutzen.
 * @note Die Klasse nimmt Kontextwechsel vor, um das loopProgramm zu simulieren. Das LoopProgramm wird in der doStep
 * funktion gestartet, dann ruft dieses irgendwann wait auf, dort geht dann der Programmablauf zu der doStep funktion über.
 * Wird diese wieder aufgerufen, wird in der wait funktion ab dem letztem befehl dort weitergemacht, bis die wait funktion
 * wieder aufgerufen wird.
 */
class LoopProgram : public Program
{
    std::unique_ptr<LoopProgramContextSwitcher> contextSwitcher;
    bool finished = false;
    time_diff_t currentWaitTime = 0;
    time_diff_t timeToWait = 0;
protected:
    /**
     * @brief wait the wait funktion "waits" ms millisecends. it brings the control flow back to the doStep method
     * @param ms the time to wait in ms
     */
    void wait(time_diff_t ms){
        currentWaitTime = 0;
        timeToWait = ms;
        if(contextSwitcher)
            contextSwitcher->yield();
    }
    /**
     * @brief loopProgram the subclasses implement here there program
     */
    virtual void loopProgram() = 0;
public:
    /**
     * @brief setContextSwitcher sets the context switcher used by the LoopProgram, should be set before the program starts
     * @param contextSwitcher the contextSwitcher object
     */
    void setContextSwitcher(std::unique_ptr<LoopProgramContextSwitcher> contextSwitcher){
        this->contextSwitcher = std::move(contextSwitcher);
        if(this->contextSwitcher){
            this->contextSwitcher->setStartFunction(this,&LoopProgram::loopProgram);
        }
    }
    LoopProgram(const ValueType valueType):Program (valueType){}
    virtual ProgramState doStep(time_diff_t t)override{
        if(finished)
            return {true,false};
        currentWaitTime+=t;
        if(currentWaitTime>=timeToWait){
            currentWaitTime = 0;
            if(contextSwitcher)
                finished = contextSwitcher->resume();
            return {finished,true};
        }
        return {false,false};
    }
    virtual void start()final override{
        if(contextSwitcher){
            contextSwitcher->start();
            finished = false;
            currentWaitTime = 0;
            timeToWait = 0;
        }
    }
};



template<typename OutputType>
using TypedLoopProgram = TypedProgram<OutputType,LoopProgram>;

}/*Modules namespace end*/

#endif // LOOPPROGRAMM_H
