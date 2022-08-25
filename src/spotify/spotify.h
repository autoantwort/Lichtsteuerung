#ifndef SPOTIFY_H
#define SPOTIFY_H

#include "audioanalysisobject.h"
#include "audiofeaturesobject.h"
#include "currentplayingobject.h"
#include "modelvector.h"
#include "userobject.h"
#include <QJsonObject>
#include <QOAuth2AuthorizationCodeFlow>
#include <QObject>
#include <optional>
#include <set>

namespace Spotify {

namespace detail {
    class KnownUserVector : public ModelVector<Objects::UserObject> {
        Q_OBJECT
    };
} // namespace detail

/**
 * @brief A singletone that handels the logged in users and check what track is currently playing and update the audio analysis for the current track
 */
class Spotify : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariant currentUser READ getCurrentUserAsVariant NOTIFY currentUserChanged)
    Q_PROPERTY(QAbstractListModel *knownUser READ getKnownUserForQML CONSTANT)
    std::optional<Objects::CurrentPlayingObject> currentPlayingObject;
    std::optional<Objects::AudioAnalysisObject> currentAudioAnalysis;
    std::optional<Objects::AudioFeaturesObject> currentAudioFeatures;
    std::optional<Objects::UserObject> currentUser;
    QOAuth2AuthorizationCodeFlow spotify;
    detail::KnownUserVector knownUser;
    bool isAutoLoginingUser = false;
    std::size_t currentAutoLoginedUser;
    explicit Spotify(QObject *parent = nullptr);

public:
    static Spotify &get() {
        static Spotify s;
        return s;
    }
    void loadFromJsonObject(const QJsonObject &object);
    void writeToJsonObject(QJsonObject &object);
    /**
     * @brief loginNewUser opens the default browser where the user can log in and give this application the rights to access the users data
     */
    Q_INVOKABLE void loginNewUser();
    /**
     * @brief loginUser log in a user that already exists
     */
    void loginUser(const Objects::UserObject &);
    /**
     * @brief autoLoginUser log in the first user where music is played
     */
    void autoLoginUser();
    const std::optional<Objects::CurrentPlayingObject> &getCurrentPlayingObject() { return currentPlayingObject; }
    const std::optional<Objects::AudioAnalysisObject> &getCurrentAudioAnalysis() { return currentAudioAnalysis; }
    const std::optional<Objects::AudioFeaturesObject> &getCurrentAudioFeatures() { return currentAudioFeatures; }
    QVariant getCurrentUserAsVariant() const;
    QAbstractListModel *getKnownUserForQML() { return &knownUser; }
    const std::vector<Objects::UserObject> &getKnownUser() const { return knownUser.getVector(); }
    const std::optional<Objects::UserObject> &getCurrentUser() const { return currentUser; }
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
    void checkLoginNextUser();

protected:
    void updatePlayer();
};

inline Spotify &get() {
    return Spotify::get();
}

} // namespace Spotify

#endif // SPOTIFY_H
