#ifndef SPOTIFYREACTOR_H
#define SPOTIFYREACTOR_H

#include "spotifyobjetcs.hpp"

namespace Modules {

/**
 * @brief The SpotifyState class holds data of the currently availible spootify data
 */
class SpotifyState {
public:
    /**
     * @brief currentTrack the current bar object, if haveCurrentTrack == true, otherwise default empty object
     */
    TimeIntervalObject currentBar;
    /**
     * @brief newTrack true if in one step(doStep) a new bar emerges
     */
    bool newBar = false;
    /**
     * @brief currentTrack the current beat object, if haveCurrentTrack == true, otherwise default empty object
     */
    TimeIntervalObject currentBeat;
    /**
     * @brief newTrack true if in one step(doStep) a new beat emerges
     */
    bool newBeat = false;
    /**
     * @brief currentTrack the current tatum section, if haveCurrentTrack == true, otherwise default empty object
     */
    SectionObject currentSection;
    /**
     * @brief newTrack true if in one step(doStep) a new section emerges
     */
    bool newSection = false;
    /**
     * @brief currentTrack the current segment object, if haveCurrentTrack == true, otherwise default empty object
     */
    SegmentObject currentSegment;
    /**
     * @brief newTrack true if in one step(doStep) a new segment emerges
     */
    bool newSegment = false;
    /**
     * @brief currentTrack the current tatum object, if haveCurrentTrack == true, otherwise default empty object
     */
    TimeIntervalObject currentTatum;
    /**
     * @brief newTrack true if in one step(doStep) a new tatum emerges
     */
    bool newTatum = false;
    /**
     * @brief currentTrack the current track, if haveCurrentTrack == true, otherwise default empty object
     */
    TrackObject currentTrack;
    /**
     * @brief newTrack true if in one step(doStep) a new Track emerges
     */
    bool newTrack = false;
    /**
     * @brief haveCurrentTrack true if a current Track exists
     */
    bool haveCurrentTrack = false;
    /**
     * @brief isPlaying true if the current track is playing
     */
    bool isPlaying = false;
    /**
     * @brief enabled true if an user is logged in and give the necessary permission
     */
    bool enabled = false;
};

} // namespace Modules

#endif // SPOTIFYREACTOR_H
