#include "spotify.h"
#include <QNetworkAccessManager>
#include <QDesktopServices>
#include <QtNetworkAuth>
#include <queue>

namespace Spotify {

class OwnNetworkHandler : public QNetworkAccessManager{
    QOAuth2AuthorizationCodeFlow * flow;
public:
    explicit OwnNetworkHandler(QOAuth2AuthorizationCodeFlow * f):QNetworkAccessManager(f), flow(f){}
protected:
    virtual QNetworkReply* createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData = nullptr)override{
        if(originalReq.url() == flow->accessTokenUrl()){
            QNetworkRequest r(originalReq);
            r.setRawHeader("Authorization","Basic "+(flow->clientIdentifier() + ":" + flow->clientIdentifierSharedKey()).toUtf8().toBase64());
            return QNetworkAccessManager::createRequest(op,r,outgoingData);
        }
        return QNetworkAccessManager::createRequest(op,originalReq,outgoingData);
    }
};

Spotify::Spotify(QObject *parent) : QObject(parent)
{
    spotify.setNetworkAccessManager(new OwnNetworkHandler(&spotify));
    auto replyHandler = new QOAuthHttpServerReplyHandler(8080, this);
    spotify.setReplyHandler(replyHandler);
    spotify.setContentType(QAbstractOAuth::ContentType::WwwFormUrlEncoded);
    spotify.setAuthorizationUrl(QUrl("https://accounts.spotify.com/authorize"));
    spotify.setAccessTokenUrl(QUrl("https://accounts.spotify.com/api/token"));
    spotify.setClientIdentifier("f18e2587c0c442c1bc89b1c73b72af5b");
    spotify.setClientIdentifierSharedKey("31e9e039bc9e48518476b9203d601b06");
    spotify.setScope("user-read-email user-read-currently-playing user-read-playback-state ");    
    // if log in a new user, the last user should not be logged in automatically
    spotify.setModifyParametersFunction([](auto stage, auto parameters){
        if(stage == QAbstractOAuth::Stage::RequestingAuthorization){
            parameters->insert("show_dialog","true");
        }
    });
    connect(&spotify, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);
    connect(&spotify, &QOAuth2AuthorizationCodeFlow::granted,[this](){        
        auto reply = spotify.get(QUrl("https://api.spotify.com/v1/me"));
        connect(reply, &QNetworkReply::finished, [this, reply]() {
            if (reply->error() != QNetworkReply::NoError) {
                qDebug() << "error : " << reply->error() << " : " << reply->errorString();
                return;
            }
            const auto data = reply->readAll();
            const auto document = QJsonDocument::fromJson(data);
            const auto root = document.object();
            currentUser.emplace(Objects::UserObject(root,spotify.refreshToken()));
            emit currentUserChanged();
            if(std::all_of(knownUser.cbegin(),knownUser.cend(),[&](const auto & uo){return uo.id != currentUser->id;})){
                knownUser.emplace_back(*currentUser);
            }

            reply->deleteLater();
        });
    });
    connect(this, &Spotify::currentTrackChanged,this, &Spotify::updateAudioAnalysis);
    connect(this, &Spotify::currentTrackChanged,this, &Spotify::updateAudioFeatures);
    /**
     * @brief timer a timer to poll the spotify player status every 2.5 seconds
     */
    auto timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->setInterval(std::chrono::milliseconds(2500));
    timer->connect(timer,&QTimer::timeout,this,&Spotify::updatePlayer);
    timer->start();
}

void Spotify::loadFromJsonObject(const QJsonObject &object){
    QJsonArray a = object["knownUser"].toArray();
    for (const auto o : a) {
        knownUser.emplace_back(o.toObject());
    }
}

void Spotify::writeToJsonObject(QJsonObject &object){
    QJsonArray a;
    for (const auto & u : knownUser) {
        QJsonObject o;
        u.writeToJsonObject(o);
        a.push_back(o);
    }
    object["knownUser"] = a;
}

void Spotify::loginNewUser(){
    spotify.grant();
}

void Spotify::loginUser(const Objects::UserObject & user){
    if(user.refreshToken.length()){
        spotify.setRefreshToken(user.refreshToken);
        spotify.refreshAccessToken();
    }
}

QVariant Spotify::getCurrentUserAsVariant() const{
    if(currentUser)
        return QVariant::fromValue(*currentUser);
    return {};
}

void Spotify::updateAudioAnalysis(){
    if(currentUser&&currentPlayingObject&&currentPlayingObject->item){
        auto reply = spotify.get(QUrl("https://api.spotify.com/v1/audio-analysis/"+currentPlayingObject->item->id));
        QObject::connect(reply,&QNetworkReply::finished,[this,reply]{
            if(reply->error() == QNetworkReply::NoError){
                currentAudioAnalysis.emplace(QJsonDocument::fromJson(reply->readAll()).object());
                emit audioAnalysisChanged();
            }else{
                qDebug() << "error : " << reply->error() << " : " << reply->errorString();
            }
            reply->deleteLater();
        });
    }else{
        if(currentAudioAnalysis){
            currentAudioAnalysis.reset();
            emit audioAnalysisChanged();
        }
    }
}

void Spotify::updateAudioFeatures(){
    if(currentUser&&currentPlayingObject&&currentPlayingObject->item){
        auto reply = spotify.get(QUrl("https://api.spotify.com/v1/audio-features/"+currentPlayingObject->item->id));
        QObject::connect(reply,&QNetworkReply::finished,[this,reply]{            
            if(reply->error() == QNetworkReply::NoError){
                currentAudioFeatures.emplace(QJsonDocument::fromJson(reply->readAll()).object());
                emit audioFeaturesChanged();
            }else{
                qDebug() << "error : " << reply->error() << " : " << reply->errorString();
            }
            reply->deleteLater();
        });
    }else{
        if(currentAudioFeatures){
            currentAudioFeatures.reset();
            emit audioFeaturesChanged();
        }
    }
}

void Spotify::updatePlayer(){
    using namespace std::chrono;
    if(currentUser){
        auto reply = spotify.get(QUrl("https://api.spotify.com/v1/me/player/currently-playing"));
        QObject::connect(reply,&QNetworkReply::finished,[this,reply]{
            if(reply->error() == 204){
                if(currentPlayingObject){
                    bool wasPlaying = currentPlayingObject->is_playing;
                    bool wasTrackActive = currentPlayingObject->item.has_value();
                    currentPlayingObject.reset();
                    emit currentPlayingObjectChanged();
                    if(wasPlaying){
                        emit playingStatusChanged();
                    }
                    if(wasTrackActive){
                        emit currentTrackChanged();
                    }
                }
            }else if(reply->error() == QNetworkReply::NoError){
                bool wasPlaying = false;
                QString trackID = "no id";
                if(currentPlayingObject){
                    wasPlaying = currentPlayingObject->is_playing;
                    if(currentPlayingObject->item.has_value()){
                        trackID = currentPlayingObject->item->id;
                    }
                }
                const auto body = reply->readAll();
                currentPlayingObject.emplace(QJsonDocument::fromJson(body).object(),std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
                emit currentPlayingObjectChanged();
                if(wasPlaying != currentPlayingObject->is_playing){
                    emit playingStatusChanged();
                }
                if(currentPlayingObject->item.has_value()){
                    if(currentPlayingObject->item->id != trackID){
                        emit currentTrackChanged();
                    }
                }
            }
            reply->deleteLater();

        });
    }else{
        if(currentPlayingObject){
            currentPlayingObject.reset();
            emit currentPlayingObjectChanged();
        }
    }
}

}
