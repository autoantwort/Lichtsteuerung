#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "programblock.h"
#include "spotify.hpp"
#include "spotify/spotify.h"
#include "controlpoint.hpp"
#include <QPointF>

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
    ProgramBlock * deletingProgramBlock = nullptr;
    // Variables for Spotify:
    SpotifyState spotifyState;
    Spotify::Spotify * spotify;
    std::vector<QMetaObject::Connection> spotifyConnections;
    int lastIndexOfCurrentBeat = -1;
    int lastIndexOfCurrentBar = -1;
    int lastIndexOfCurrentTatum = -1;
    int lastIndexOfCurrentSection = -1;
    int lastIndexOfCurrentSegment = -1;
    // Control Point
    ControlPoint controlPoint;

    friend class ProgramBlock;
    void run() noexcept;
public:
    Controller();
    /**
     * @brief getSpotifyState return the SpotifyState object managed by this controller
     */
    const SpotifyState & getSpotifyState(){return spotifyState;}
    /**
     * @brief getControlPoint return the ControlPoint object managed by this controller
     */
    const ControlPoint & getControlPoint(){return controlPoint;}
    /**
     * @brief setSpotify sets the Spotify object for this controller that is used to fill the Spotify state object
     * @param spotify the Spotify object, or a nullptr to unset the Spotify object
     */
    void setSpotify(Spotify::Spotify * spotify);
    /**
     * @brief updateControlPoint updates the control point used by the module programs
     * @param controlPointPosition The new control point position on the map
     */
    void updateControlPoint(const QPointF & controlPointPosition){
        controlPoint.x = controlPointPosition.x();
        controlPoint.y = controlPointPosition.y();
        controlPoint.positionChanged = true;
    }
    void start(){
        if(!run_){
            run_=true;
            thread = std::thread([this](){
                run();
            });
        }
    }
    void stop(){run_=false;}
    ~Controller(){run_ = false;if(thread.joinable())thread.join();}
private:
    void runProgramm(std::shared_ptr<ProgramBlock> pb);
    void stopProgramm(std::shared_ptr<ProgramBlock> pb);
    void stopProgramm(ProgramBlock* pb);
    bool isProgramRunning(ProgramBlock * pb);
private:
    /**
     * @brief haveAnalysis checks if currently a spotify analysis exists
     * @return true if exists, false otherwise
     */
    bool haveAnalysis()const;
    /**
     * @brief updateSpotifyState called per "tick" and updates the spotifyState object
     */
    void updateSpotifyState();
};
}

#endif // CONTROLLER_H
