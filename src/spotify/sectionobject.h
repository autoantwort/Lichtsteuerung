#ifndef SECTIONOBJECT_H
#define SECTIONOBJECT_H

#include <QJsonObject>

namespace Spotify::Objects {

/**
 * @brief The SectionObject class https://developer.spotify.com/documentation/web-api/reference/tracks/get-audio-analysis/#section-object
 */
class SectionObject {
public:
    /**
     * @brief start The starting point (in seconds) of the section.
     */
    const float start;
    /**
     * @brief duration The duration (in seconds) of the section.
     */
    const float duration;
    /**
     * @brief confidence The confidence, from 0.0 to 1.0, of the reliability of the section’s “designation”.
     */
    const float confidence;
    /**
     * @brief loudness The overall loudness of the section in decibels (dB). Loudness values are useful for comparing relative loudness of sections within tracks.
     */
    const float loudness;
    /**
     * @brief tempo The overall estimated tempo of the section in beats per minute (BPM). In musical terminology, tempo is the speed or pace of a given piece and derives directly from the average beat
     * duration.
     */
    const float tempo;
    /**
     * @brief tempo_confidence The confidence, from 0.0 to 1.0, of the reliability of the tempo. Some tracks contain tempo changes or sounds which don’t contain tempo (like pure speech) which would
     * correspond to a low value in this field.
     */
    const float tempo_confidence;
    /**
     * @brief key The estimated overall key of the section. The values in this field ranging from 0 to 11 mapping to pitches using standard Pitch Class notation (E.g. 0 = C, 1 = C♯/D♭, 2 = D, and so
     * on). If no key was detected, the value is -1.
     */
    const int key;
    /**
     * @brief key_confidence The confidence, from 0.0 to 1.0, of the reliability of the key. Songs with many key changes may correspond to low values in this field.
     */
    const float key_confidence;
    /**
     * @brief mode Indicates the modality (major or minor) of a track, the type of scale from which its melodic content is derived. This field will contain a 0 for “minor”, a 1 for “major”, or a -1
     * for no result. Note that the major key (e.g. C major) could more likely be confused with the minor key at 3 semitones lower (e.g. A minor) as both keys carry the same pitches.
     */
    const int mode;
    /**
     * @brief mode_confidence The confidence, from 0.0 to 1.0, of the reliability of the mode.
     */
    const float mode_confidence;
    /**
     * @brief time_signature An estimated overall time signature of a track. The time signature (meter) is a notational convention to specify how many beats are in each bar (or measure). The time
     * signature ranges from 3 to 7 indicating time signatures of “3/4”, to “7/4”.
     */
    const int time_signature;
    /**
     * @brief time_signature_confidence The confidence, from 0.0 to 1.0, of the reliability of the time_signature. Sections with time signature changes may correspond to low values in this field.
     */
    const float time_signature_confidence;

public:
    SectionObject(const QJsonObject &object);
};

} // namespace Spotify::Objects

#endif // SECTIONOBJECT_H
