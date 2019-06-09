#ifndef SPOTIFY_H
#define SPOTIFY_H

#include <QObject>
#include <optional>
#include "currentplayingobject.h"
#include "userobject.h"
#include "audioanalysisobject.h"
#include "audiofeaturesobject.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QJsonObject>
#include <set>
#include "modelvector.h"

namespace Spotify {

namespace detail{
    class KnownUserVector : public ModelVector<std::unique_ptr<Objects::UserObject>>{
        Q_OBJECT
    };
}

/**
 * @brief A singletone that handels the logged in users and check what track is currently playing and update the audio analysis for the current track
 */
class Spotify : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Objects::UserObject * currentUser READ getCurrentUserAsPointer NOTIFY currentUserChanged)
    Q_PROPERTY(QAbstractListModel * knownUser READ getKnownUserForQML CONSTANT)
    Q_PROPERTY(Objects::TrackObject_full * currentTrack READ getCurrentTrackAsPointer NOTIFY currentTrackChanged)
    Q_PROPERTY(Objects::CurrentPlayingObject * currentPlayingStatus READ getCurrentPlayingObjectAsPointer NOTIFY currentPlayingObjectChanged)
    //Q_PROPERTY(QAbstractListModel * knownUser READ getKnownUserForQML CONSTANT)
    std::optional<Objects::CurrentPlayingObject> currentPlayingObject;
    std::optional<Objects::TrackObject_full> currentTrack;
    std::optional<Objects::AudioAnalysisObject> currentAudioAnalysis;
    std::optional<Objects::AudioFeaturesObject> currentAudioFeatures;
    std::unique_ptr<Objects::UserObject> currentUser = nullptr;
    QOAuth2AuthorizationCodeFlow spotify;
    detail::KnownUserVector knownUser;

    explicit Spotify(QObject *parent = nullptr);
public:
    static Spotify& get(){static Spotify s; return s;}
    void loadFromJsonObject(const QJsonObject & object);
    void writeToJsonObject(QJsonObject & object);
    /**
      * @brief loginNewUser opens the default browser where the user can log in and give this application the rights to access the users data
      */
    Q_INVOKABLE void loginNewUser();
    /**
     * @brief loginUser log in a user that already exists
     */
    void loginUser(const Objects::UserObject &);
    const std::optional<Objects::CurrentPlayingObject>& getCurrentPlayingObject(){return currentPlayingObject;}
    const std::optional<Objects::AudioAnalysisObject>& getCurrentAudioAnalysis(){return currentAudioAnalysis;}
    const std::optional<Objects::AudioFeaturesObject>& getCurrentAudioFeatures(){return currentAudioFeatures;}
    Objects::UserObject * getCurrentUserAsPointer();
    QAbstractListModel * getKnownUserForQML(){return &knownUser;}
    const std::vector<std::unique_ptr<Objects::UserObject>> & getKnownUser()const{return knownUser.getVector();}
    const std::unique_ptr<Objects::UserObject> & getCurrentUser()const{return currentUser;}
    Objects::TrackObject_full * getCurrentTrackAsPointer();
    Objects::CurrentPlayingObject * getCurrentPlayingObjectAsPointer();
signals:
    void currentPlayingObjectChanged();
    void currentTrackChanged();
    void playingStatusChanged();
    void audioAnalysisChanged();
    void audioFeaturesChanged();
    void currentUserChanged();
protected slots:
    void updateAudioAnalysis();
    void updateAudioFeatures();
protected:
    void updatePlayer();
};

inline Spotify & get(){
    return Spotify::get();
}

}

#endif // SPOTIFY_H
