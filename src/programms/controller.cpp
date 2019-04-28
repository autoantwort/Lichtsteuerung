#include "controller.h"
#include <mutex>

namespace Modules {

Controller::Controller():run_(false)
{

}

void Controller::setSpotify(Spotify::Spotify *spotify){
    if(spotify==this->spotify)
        return;
    for(const auto & c : spotifyConnections){
        QObject::disconnect(c);
    }
    spotifyConnections.clear();
    this->spotify = spotify;
    spotifyState.enabled = spotify != nullptr;
    if(spotify&&spotify->getCurrentPlayingObject()){
        spotifyState.enabled = true;
        spotifyConnections.push_back(QObject::connect(spotify,&Spotify::Spotify::currentTrackChanged,[&,spotify](){
            if(spotify->getCurrentAudioFeatures()&&spotify->getCurrentPlayingObject()&&spotify->getCurrentPlayingObject()->item){
                spotifyState.currentTrack.~TrackObject();
                new (&spotifyState.currentTrack) TrackObject(*spotify->getCurrentAudioFeatures(),*spotify->getCurrentPlayingObject()->item);
                spotifyState.newTrack = true;
            }else{
                spotifyState.currentTrack.~TrackObject();
                new (&spotifyState.currentTrack) TrackObject();
            }
        }));
        spotifyConnections.push_back(QObject::connect(spotify,&QObject::destroyed,[this](){
            this->setSpotify(nullptr);
        }));
        spotifyConnections.push_back(QObject::connect(spotify,&Spotify::Spotify::audioFeaturesChanged,[&,spotify](){
            if(spotify->getCurrentAudioFeatures()&&spotify->getCurrentPlayingObject()&&spotify->getCurrentPlayingObject()->item){
                spotifyState.currentTrack.~TrackObject();
                new (&spotifyState.currentTrack) TrackObject(*spotify->getCurrentAudioFeatures(),*spotify->getCurrentPlayingObject()->item);
                spotifyState.newTrack = true;
            }else{
                spotifyState.currentTrack.~TrackObject();
                new (&spotifyState.currentTrack) TrackObject();
            }
        }));
    }
}

bool Controller::haveAnalysis()const{
    return spotify&&spotify->getCurrentAudioAnalysis()&&spotify->getCurrentPlayingObject()&&spotify->getCurrentPlayingObject()->item;
}

void Controller::updateSpotifyState(){
    if(haveAnalysis()){
        spotifyState.enabled = true;
        spotifyState.currentTrack.progress_ms = spotify->getCurrentPlayingObject()->getProgressInMs();
        spotifyState.isPlaying = spotify->getCurrentPlayingObject()->is_playing;        
#define UPDATE_OBJECT(type,Type,Object) \
    if(int newIndex = spotify->getCurrentAudioAnalysis()->getIndexIn ## Type ## sForTime(spotifyState.currentTrack.progress_ms);newIndex != lastIndexOfCurrent ## Type){ \
        lastIndexOfCurrent ## Type = newIndex; \
        spotifyState.current ## Type .~ Object(); \
        if(newIndex>=0){ \
            new (&spotifyState.current ## Type) Object(spotify->getCurrentAudioAnalysis()->type ## s[static_cast<unsigned>(newIndex)]); \
            spotifyState.new ## Type = true; \
        } else {\
            new (&spotifyState.current ## Type) Object(); \
            spotifyState.new ## Type = true; \
        } \
    }else{ \
        spotifyState.new##Type = false;\
    }
        if(spotifyState.isPlaying){
            UPDATE_OBJECT(bar,Bar,TimeIntervalObject)
            UPDATE_OBJECT(tatum,Tatum,TimeIntervalObject)
            UPDATE_OBJECT(beat,Beat,TimeIntervalObject)
            UPDATE_OBJECT(section,Section,SectionObject)
            UPDATE_OBJECT(segment,Segment,SegmentObject)
#undef UPDATE_OBJECT
        }
    }else{
#define UPDATE_OBJECT(Type,Object) \
        spotifyState.current ## Type .~ Object(); \
        new (&spotifyState.current ## Type) Object(); \
        spotifyState.new ## Type = false;

        UPDATE_OBJECT(Bar, TimeIntervalObject)
        UPDATE_OBJECT(Beat, TimeIntervalObject)
        UPDATE_OBJECT(Tatum, TimeIntervalObject)
        UPDATE_OBJECT(Section, SectionObject)
        UPDATE_OBJECT(Segment, SegmentObject)
        UPDATE_OBJECT(Track, TrackObject)
#undef UPDATE_OBJECT
        spotifyState.haveCurrentTrack = false;
        spotifyState.isPlaying = false;
        spotifyState.enabled = false;
    }

}

void Controller::run() noexcept{
    while (run_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::unique_lock<std::mutex> l(vectorLock);
        updateSpotifyState();
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
        spotifyState.newTrack = false;
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
