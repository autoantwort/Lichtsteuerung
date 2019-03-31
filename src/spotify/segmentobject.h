#ifndef SEGMENTOBJECT_H
#define SEGMENTOBJECT_H

#include <vector>
#include <QJsonObject>

namespace Spotify::Objects{

/**
 * @brief The SegmentObject class https://developer.spotify.com/documentation/web-api/reference/tracks/get-audio-analysis/#segment-object
 */
class SegmentObject
{
public:
    /**
     * @brief start The starting point (in seconds) of the segment.
     */
    const float start;
    /**
     * @brief duration The duration (in seconds) of the segment.
     */
    const float duration;
    /**
     * @brief confidence The confidence, from 0.0 to 1.0, of the reliability of the segmentation. Segments of the song which are difficult to logically segment (e.g: noise) may correspond to low values in this field.
     */
    const float confidence;
    /**
     * @brief loudness_start The onset loudness of the segment in decibels (dB). Combined with loudness_max and loudness_max_time, these components can be used to desctibe the “attack” of the segment.
     */
    const float loudness_start;
    /**
     * @brief loudness_max The peak loudness of the segment in decibels (dB). Combined with loudness_start and loudness_max_time, these components can be used to desctibe the “attack” of the segment.
     */
    const float loudness_max;
    /**
     * @brief loudness_max_time The segment-relative offset of the segment peak loudness in seconds. Combined with loudness_start and loudness_max, these components can be used to desctibe the “attack” of the segment.
     */
    const float loudness_max_time;
    /**
     * @brief loudness_end The offset loudness of the segment in decibels (dB). This value should be equivalent to the loudness_start of the following segment.
     */
    const float loudness_end;
    /**
     * @brief pitches A “chroma” vector representing the pitch content of the segment, corresponding to the 12 pitch classes C, C#, D to B, with values ranging from 0 to 1 that describe the relative dominance of every pitch in the chromatic scale. More details about how to interpret this vector can be found below..
     */
    const std::vector<float> pitches;
    /**
     * @brief timbre Timbre is the quality of a musical note or sound that distinguishes different types of musical instruments, or voices. Timbre vectors are best used in comparison with each other. More details about how to interpret this vector can be found on the below..
     */
    const std::vector<float> timbre;
public:
    SegmentObject();
    SegmentObject(const QJsonObject &object);
};

}

#endif // SEGMENTOBJECT_H
