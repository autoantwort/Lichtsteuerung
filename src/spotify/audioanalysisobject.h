#ifndef AUDIOANALYSISOBJECT_H
#define AUDIOANALYSISOBJECT_H

#include "sectionobject.h"
#include "segmentobject.h"
#include "timeintervalobject.h"
#include <QJsonObject>
#include <vector>

namespace Spotify::Objects {

/**
 * @brief The AudioAnalysisObject class https://developer.spotify.com/documentation/web-api/reference/tracks/get-audio-analysis/#audio-analysis-object
 */
class AudioAnalysisObject {
protected:
    /**
     * @brief getIndexForTime T must have a start member, find the index of the object where ms >= vec[i].start && ms < vec[i+1].start
     * @param vec the vector of the objects
     * @param ms the time
     * @return the index or -1, if not found
     */
    template <typename T>
    int getIndexForTime(const std::vector<T> &vec, int ms) const;

public:
    /**
     * @brief bars The time intervals of the bars throughout the track. A bar (or measure) is a segment of time defined as a given number of beats. Bar offsets also indicate downbeats, the first beat
     * of the measure.
     */
    const std::vector<TimeIntervalObject> bars;
    /**
     * @brief beats The time intervals of beats throughout the track. A beat is the basic time unit of a piece of music; for example, each tick of a metronome. Beats are typically multiples of tatums.
     */
    const std::vector<TimeIntervalObject> beats;
    /**
     * @brief sections Sections are defined by large variations in rhythm or timbre, e.g. chorus, verse, bridge, guitar solo, etc. Each section contains its own descriptions of tempo, key, mode,
     * time_signature, and loudness.
     */
    const std::vector<SectionObject> sections;
    /**
     * @brief segments Audio segments attempts to subdivide a song into many segments, with each segment containing a roughly consitent sound throughout its duration.
     */
    const std::vector<SegmentObject> segments;
    /**
     * @brief tatums A tatum represents the lowest regular pulse train that a listener intuitively infers from the timing of perceived musical events (segments). For more information about tatums, see
     * Rhythm (below).
     */
    const std::vector<TimeIntervalObject> tatums;

public:
    int getIndexInBarsForTime(int ms) const { return getIndexForTime(bars, ms); }
    int getIndexInBeatsForTime(int ms) const { return getIndexForTime(beats, ms); }
    int getIndexInSectionsForTime(int ms) const { return getIndexForTime(sections, ms); }
    int getIndexInSegmentsForTime(int ms) const { return getIndexForTime(segments, ms); }
    int getIndexInTatumsForTime(int ms) const { return getIndexForTime(tatums, ms); }

public:
    AudioAnalysisObject();
    AudioAnalysisObject(const QJsonObject &object);
};

template <typename T>
int AudioAnalysisObject::getIndexForTime(const std::vector<T> &vec, int ms) const {
    struct comperator {
        bool operator()(const T &l, const int r) { return l.start * 1000 < r; }
        bool operator()(const int l, const T &r) { return l < r.start * 1000; }
    };
    if (auto result = std::lower_bound(vec.cbegin(), vec.cend(), ms, comperator()); result != vec.cend()) {
        return static_cast<int>(std::distance(vec.cbegin(), result));
    }
    return -1;
}

} // namespace Spotify::Objects

#endif // AUDIOANALYSISOBJECT_H
