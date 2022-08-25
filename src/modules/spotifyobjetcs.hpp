#ifndef SPOTIFYOBJETCS_H
#define SPOTIFYOBJETCS_H

// #define CONVERT_FROM_SPOTIFY_OBJECTS

#ifdef CONVERT_FROM_SPOTIFY_OBJECTS
#include "spotify/audiofeaturesobject.h"
#include "spotify/currentplayingobject.h"
#include "spotify/sectionobject.h"
#include "spotify/segmentobject.h"
#include "spotify/timeintervalobject.h"
#include "spotify/trackobject.h"
#endif

namespace Modules {
/**
 * @brief The TimeIntervalObject class https://developer.spotify.com/documentation/web-api/reference/tracks/get-audio-analysis/#time-interval-object
 */
class TimeIntervalObject {
public:
    /**
     * @brief start The starting point (in milliSeconds) of the time interval.
     */
    const int start = 0;
    /**
     * @brief duration The duration (in milliSeconds) of the time interval.
     */
    const int duration = 0;
    /**
     * @brief confidence The confidence, from 0.0 to 1.0, of the reliability of the interval.
     */
    const float confidence = 0;

public:
    TimeIntervalObject() = default;
#ifdef CONVERT_FROM_SPOTIFY_OBJECTS
    template <typename T>
    TimeIntervalObject(const T &o) : start(static_cast<int>(o.start * 1000))
                                   , duration(static_cast<int>(o.start * 1000))
                                   , confidence(o.confidence) {}
#endif
};

/**
 * @brief The SectionObject class https://developer.spotify.com/documentation/web-api/reference/tracks/get-audio-analysis/#section-object
 */
class SectionObject : public TimeIntervalObject {
public:
    /**
     * @brief loudness The overall loudness of the section in decibels (dB). Loudness values are useful for comparing relative loudness of sections within tracks.
     */
    const float loudness = 0;
    /**
     * @brief tempo The overall estimated tempo of the section in beats per minute (BPM). In musical terminology, tempo is the speed or pace of a given piece and derives directly from the average beat
     * duration.
     */
    const float tempo = 0;
    /**
     * @brief tempo_confidence The confidence, from 0.0 to 1.0, of the reliability of the tempo. Some tracks contain tempo changes or sounds which don’t contain tempo (like pure speech) which would
     * correspond to a low value in this field.
     */
    const float tempo_confidence = 0;
    /**
     * @brief key The estimated overall key of the section. The values in this field ranging from 0 to 11 mapping to pitches using standard Pitch Class notation (E.g. 0 = C, 1 = C♯/D♭, 2 = D, and so
     * on). If no key was detected, the value is -1.
     */
    const int key = 0;
    /**
     * @brief key_confidence The confidence, from 0.0 to 1.0, of the reliability of the key. Songs with many key changes may correspond to low values in this field.
     */
    const float key_confidence = 0;
    /**
     * @brief mode Indicates the modality (major or minor) of a track, the type of scale from which its melodic content is derived. This field will contain a 0 for “minor”, a 1 for “major”, or a -1
     * for no result. Note that the major key (e.g. C major) could more likely be confused with the minor key at 3 semitones lower (e.g. A minor) as both keys carry the same pitches.
     */
    const int mode = 0;
    /**
     * @brief mode_confidence The confidence, from 0.0 to 1.0, of the reliability of the mode.
     */
    const float mode_confidence = 0;
    /**
     * @brief time_signature An estimated overall time signature of a track. The time signature (meter) is a notational convention to specify how many beats are in each bar (or measure). The time
     * signature ranges from 3 to 7 indicating time signatures of “3/4”, to “7/4”.
     */
    const int time_signature = 0;
    /**
     * @brief time_signature_confidence The confidence, from 0.0 to 1.0, of the reliability of the time_signature. Sections with time signature changes may correspond to low values in this field.
     */
    const float time_signature_confidence = 0;

public:
    SectionObject() = default;
#ifdef CONVERT_FROM_SPOTIFY_OBJECTS
    SectionObject(const Spotify::Objects::SectionObject &o)
        : TimeIntervalObject(o)
        , loudness(o.loudness)
        , tempo(o.tempo)
        , tempo_confidence(o.tempo_confidence)
        , key(o.key)
        , key_confidence(o.key_confidence)
        , mode(o.mode)
        , mode_confidence(o.mode_confidence)
        , time_signature(o.time_signature)
        , time_signature_confidence(o.time_signature_confidence) {}
#endif
};
/**
 * @brief The SegmentObject class https://developer.spotify.com/documentation/web-api/reference/tracks/get-audio-analysis/#segment-object
 */
class SegmentObject : public TimeIntervalObject {
public:
    /**
     * @brief loudness_start The onset loudness of the segment in decibels (dB). Combined with loudness_max and loudness_max_time, these components can be used to desctibe the “attack” of the segment.
     */
    const float loudness_start = 0;
    /**
     * @brief loudness_max The peak loudness of the segment in decibels (dB). Combined with loudness_start and loudness_max_time, these components can be used to desctibe the “attack” of the segment.
     */
    const float loudness_max = 0;
    /**
     * @brief loudness_max_time The segment-relative offset of the segment peak loudness in milliseconds. Combined with loudness_start and loudness_max, these components can be used to desctibe the
     * “attack” of the segment.
     */
    const int loudness_max_time = 0;
    /**
     * @brief loudness_end The offset loudness of the segment in decibels (dB). This value should be equivalent to the loudness_start of the following segment.
     */
    const float loudness_end = 0;
    /**
     * @brief pitches A “chroma” vector representing the pitch content of the segment, corresponding to the 12 pitch classes C, C#, D to B, with values ranging from 0 to 1 that describe the relative
     * dominance of every pitch in the chromatic scale. More details about how to interpret this vector can be found below..
     */
    const std::vector<float> pitches;
    /**
     * @brief timbre Timbre is the quality of a musical note or sound that distinguishes different types of musical instruments, or voices. Timbre vectors are best used in comparison with each other.
     * More details about how to interpret this vector can be found on the below..
     */
    const std::vector<float> timbre;

public:
    SegmentObject() = default;
#ifdef CONVERT_FROM_SPOTIFY_OBJECTS
    SegmentObject(const Spotify::Objects::SegmentObject &o)
        : TimeIntervalObject(o)
        , loudness_start(o.loudness_start)
        , loudness_max(o.loudness_max)
        , loudness_max_time(static_cast<int>(o.loudness_max_time * 1000))
        , loudness_end(o.loudness_end)
        , pitches(o.pitches)
        , timbre(o.timbre) {}
#endif
};

class TrackObject {
public:
    /**
     * @brief duration_ms The track length in milliseconds.
     */
    const int duration_ms = 0;
    /**
     * @brief acousticness A confidence measure from 0.0 to 1.0 of whether the track is acoustic. 1.0 represents high confidence the track is acoustic.
     */
    const float acousticness = 0;
    /**
     * @brief energy Energy is a measure from 0.0 to 1.0 and represents a perceptual measure of intensity and activity. Typically, energetic tracks feel fast, loud, and noisy. For example, death metal
     * has high energy, while a Bach prelude scores low on the scale. Perceptual features contributing to this attribute include dynamic range, perceived loudness, timbre, onset rate, and general
     * entropy.
     */
    const float energy = 0;
    /**
     * @brief instrumentalness Predicts whether a track contains no vocals. “Ooh” and “aah” sounds are treated as instrumental in this context. Rap or spoken word tracks are clearly “vocal”. The
     * closer the instrumentalness value is to 1.0, the greater likelihood the track contains no vocal content. Values above 0.5 are intended to represent instrumental tracks, but confidence is higher
     * as the value approaches 1.0.
     */
    const float instrumentalness = 0;
    /**
     * @brief key The key the track is in. Integers map to pitches using standard Pitch Class notation . E.g. 0 = C, 1 = C♯/D♭, 2 = D, and so on.
     */
    const int key = 0;
    /**
     * @brief liveness Detects the presence of an audience in the recording. Higher liveness values represent an increased probability that the track was performed live. A value above 0.8 provides
     * strong likelihood that the track is live.
     */
    const float liveness = 0;
    /**
     * @brief loudness The overall loudness of a track in decibels (dB). Loudness values are averaged across the entire track and are useful for comparing relative loudness of tracks. Loudness is the
     * quality of a sound that is the primary psychological correlate of physical strength (amplitude). Values typical range between -60 and 0 db.
     */
    const float loudness = 0;
    /**
     * @brief mode Mode indicates the modality (major or minor) of a track, the type of scale from which its melodic content is derived. Major is represented by 1 and minor is 0.
     */
    const int mode = 0;
    /**
     * @brief speechiness Speechiness detects the presence of spoken words in a track. The more exclusively speech-like the recording (e.g. talk show, audio book, poetry), the closer to 1.0 the
     * attribute value. Values above 0.66 describe tracks that are probably made entirely of spoken words. Values between 0.33 and 0.66 describe tracks that may contain both music and speech, either
     * in sections or layered, including such cases as rap music. Values below 0.33 most likely represent music and other non-speech-like tracks.
     */
    const float speechiness = 0;
    /**
     * @brief tempo The overall estimated tempo of a track in beats per minute (BPM). In musical terminology, tempo is the speed or pace of a given piece and derives directly from the average beat
     * duration.
     */
    const float tempo = 0;
    /**
     * @brief time_signature An estimated overall time signature of a track. The time signature (meter) is a notational convention to specify how many beats are in each bar (or measure).
     */
    const int time_signature = 0;
    /**
     * @brief valence A measure from 0.0 to 1.0 describing the musical positiveness conveyed by a track. Tracks with high valence sound more positive (e.g. happy, cheerful, euphoric), while tracks
     * with low valence sound more negative (e.g. sad, depressed, angry).
     */
    const float valence = 0;
    /**
     * @brief name The name of the track.
     */
    const std::string name;

public:
    TrackObject() = default;
    /**
     * @brief progress_ms Progress into the currently playing track.
     */
    int progress_ms = 0;
#ifdef CONVERT_FROM_SPOTIFY_OBJECTS
    TrackObject(const Spotify::Objects::AudioFeaturesObject &af, const Spotify::Objects::TrackObject_full &t)
        : duration_ms(af.duration_ms)
        , acousticness(af.acousticness)
        , energy(af.energy)
        , instrumentalness(af.instrumentalness)
        , key(af.key)
        , liveness(af.liveness)
        , loudness(af.loudness)
        , mode(af.mode)
        , speechiness(af.speechiness)
        , tempo(af.tempo)
        , time_signature(af.time_signature)
        , valence(af.valence)
        , name(t.name.toStdString()) {}
    void updateProgressInMs(const Spotify::Objects::CurrentPlayingObject &o) {
        progress_ms = o.getProgressInMs();
    }
#endif
};

} // namespace Modules

#endif // SPOTIFYOBJETCS_H
