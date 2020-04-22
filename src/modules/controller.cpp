#include "controller.h"
#include "compiler.h"
#include "errornotifier.h"
#include "system_error_handler.h"
#include <QDebug>
#include <QProcess>
#include <mutex>

namespace Modules {

Controller::Controller() {
    moveToThread(&thread);

    QObject::connect(&thread, &QThread::started, [this]() { timerId = startTimer(1); });
    QObject::connect(&thread, &QThread::finished, [this]() {
        killTimer(timerId);
        timerId = -1;
    });
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

Controller::~Controller() {
    if (thread.isRunning()) {
        thread.exit();
        thread.wait();
    }
}

bool Controller::haveAnalysis() const { return spotify && spotify->getCurrentAudioAnalysis() && spotify->getCurrentPlayingObject() && spotify->getCurrentPlayingObject()->item; }

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

void Controller::run() noexcept {
    using namespace std::chrono;
    int time_diff = 1;
    if (lastElapsedMilliseconds == -1) {
        startPoint = steady_clock::now();
        lastElapsedMilliseconds = 0;
    } else {
        auto elapsedMilliseconds = duration_cast<duration<double, std::milli>>(steady_clock::now() - startPoint).count();
        time_diff = static_cast<int>(std::round(elapsedMilliseconds)) - lastElapsedMilliseconds;
        lastElapsedMilliseconds = elapsedMilliseconds;
    }
    std::unique_lock<std::mutex> l(vectorLock);
    updateSpotifyState();
    for (auto pb = runningProgramms.begin(); pb != runningProgramms.end();) {
        try {
            if ((*pb)->doStep(time_diff)) {
                deletingProgramBlock = (*pb).get();
                (**pb).stop();
                deletingProgramBlock = nullptr;
                pb = runningProgramms.erase(pb);
            } else {
                ++pb;
            }
        } catch (const error::crash_error &e) {
            QString msg(e.what());
            for (const auto &frame : e.getStacktrace()) {
                const auto asString = boost::stacktrace::to_string(frame);
                if (asString.find("____") != std::string::npos) {
                    QString filename;
                    int lineNumber = -1;
                    QProcess p;
                    QString cmd = Compiler::compilerCmd;
#ifdef Q_OS_WIN
                    if (!cmd.endsWith(QLatin1String("g++.exe"))) {
                        qDebug() << "Error in " << __FILE__ << " line " << __LINE__ << ": Can not build command to translate address to line. g++.exe not found in Compiler::cmd";
                    } else {
                        cmd.replace(QLatin1String("g++.exe"), QLatin1String("addr2line.exe"));

                        auto pos = asString.find(" in ");
                        if (pos == std::string::npos) {
                            pos = asString.find(" at ");
                            if (pos == std::string::npos) {
                                pos = 0;
                            } else {
                                pos += 4;
                            }
                        } else {
                            pos += 4;
                        }
                        p.start(cmd, QStringList() << QStringLiteral("-e") << QString::fromStdString(asString).mid(pos) << QString::number(reinterpret_cast<std::size_t>(frame.address()), 16));
                        p.waitForFinished();
                        QByteArray output = p.readAllStandardOutput();
                        int seperator = output.lastIndexOf(':');
                        filename = output.left(seperator);
                        lineNumber = output.mid(seperator + 1).trimmed().toInt();
                    }
#else
#warning OS not supported
#endif
                    if (lineNumber >= 0) {
                        QFile file(filename);
                        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            break;
                        }
                        msg += " in line " + QString::number(lineNumber) + " in file " + filename.mid(filename.lastIndexOf('/') + 1) + "\n";
                        int currentLineNumber = 0;
                        while (!file.atEnd() && currentLineNumber < lineNumber + 2) {
                            ++currentLineNumber;
                            QByteArray line = file.readLine();
                            if (currentLineNumber == lineNumber) {
                                msg += "->" + line;
                            } else if (currentLineNumber == lineNumber - 1 || currentLineNumber == lineNumber + 1) {
                                msg += "     " + line;
                            }
                        }
                        file.close();
                    }
                    break;
                }
            }
            ErrorNotifier::showError(QStringLiteral("Error while executing ") + (**pb).getName() + ":\n" + msg);
            // if it was in a doStep call
            if (deletingProgramBlock == nullptr) {
                try {
                    deletingProgramBlock = (*pb).get();
                    (**pb).stop();
                } catch (...) {
                    // we ignore the error (the module is completly broken)
                }
            }
            // maybe inside the stop call
            deletingProgramBlock = nullptr;
            // we remove the programm from the running list
            pb = runningProgramms.erase(pb);
        }
    }
    spotifyState.newTrack = false;
    controlPoint.positionChanged = false;
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
    return std::any_of(runningProgramms.cbegin(), runningProgramms.cend(), [=](const auto &p) { return p.get() == pb; });
}

} // namespace Modules
