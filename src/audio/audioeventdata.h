#ifndef AUDIOEVENTDATA_H
#define AUDIOEVENTDATA_H

#include <QObject>
#include <cmath>
#include <deque>
#include <mutex>

namespace Audio {

/**
 * This class locks a value (by reference), as long as this class exists, the value is locked
 */
template <typename Value, typename Mutex>
class Locked {
    Value &value;
    Mutex &mutex;

public:
    /**
     * @brief Locked creates a Locked value. The constrctor calls lock() on the given mutex
     * @param value the value that should be protected with a lock
     * @param mutex the mutex to lock and unlock
     */
    Locked(Value &value, Mutex &mutex) : value(value), mutex(mutex) { mutex.lock(); }
    Q_DISABLE_COPY_MOVE(Locked)
    ~Locked() { mutex.unlock(); }
    Value *operator->() { return &value; }
    const Value *operator->() const { return &value; }
    Value &operator*() { return value; }
    const Value &operator*() const { return value; }
};

/**
 * @brief The EventSeries class holds a serie of events. The class is thread safe.
 */
class EventSeries {
    /**
     * @brief newestSample is the count of the newest sample, there is no newer sample
     */
    unsigned newestSample = 0;
    const unsigned samplesPerSecond;
    /**
     * @brief events that are older than newestSample - neededRange gets deleted
     */
    unsigned neededRange;
    /**
     * @brief the events. a events consists of one timestamp measured in samples
     */
    std::deque<unsigned> events;
    /**
     * @brief eventMutex is used to protect events
     */
    mutable std::mutex eventMutex;

    /**
     * @brief DEFAULT_DURATION_NEEDED_IN_SECONDS how long events should be kept in the buffer by default
     */
    static constexpr unsigned DEFAULT_DURATION_NEEDED_IN_SECONDS = 60;

public:
    /**
     * @brief EventSeries creates a new EventSeries to save events
     * @param samplesPerSecond the samples per second
     */
    explicit EventSeries(unsigned samplesPerSecond) : samplesPerSecond(samplesPerSecond), neededRange(samplesPerSecond * DEFAULT_DURATION_NEEDED_IN_SECONDS) {}
    /**
     * @brief increaseNewestSampleBy increases the newest sample count, this should be done if new samples are availible. Also discard old events
     * @param count the amount of new samples
     */
    void increaseNewestSampleBy(unsigned count) {
        newestSample += count;
        if (neededRange < newestSample) {
            while (!events.empty() && events.front() < newestSample - neededRange) {
                events.pop_front();
            }
        }
    }
    /**
     * @brief getNewestSample returns the newest sample, which can be used, how old an event is relative to now
     * @return the newest sample
     */
    unsigned getNewestSample() const { return newestSample; }
    /**
     * @brief getNeededRange returns the needed range in samples. Events older than newestSample - neededRange gets removed
     * @return the needed range
     */
    unsigned getNeededRange() const { return neededRange; }
    /**
     * @brief getSamplesPerSecond return the samples per second
     * @return samples per second
     */
    unsigned getSamplesPerSecond() const { return samplesPerSecond; }
    /**
     * @brief addEvent adds an event to the event list. The event must be newer than the newest existing event.
     * @param atSample the position of the new event
     */
    void addEvent(unsigned atSample) {
        std::unique_lock l(eventMutex);
        events.emplace_back(atSample);
    }
    /**
     * @brief getEvents return the events in a wrapper that locks the list, you should release the Locked object as
     * fast as possible, otherwise you block the whole system
     * @return A Locked object holding the event list
     */
    [[nodiscard]] Locked<std::deque<unsigned>, std::mutex> getEvents() { return Locked(events, eventMutex); }
    /**
     * @brief getEvents return the events in a wrapper that locks the list, you should release the Locked object as
     * fast as possible, otherwise you block the whole system
     * @return A Locked object holding the event list
     */
    [[nodiscard]] Locked<const std::deque<unsigned>, std::mutex> getEvents() const { return Locked(events, eventMutex); }
};

/**
 * @brief The OnsetDataSeries class holds all data produced by an OnsetAnalysis class.
 */
class OnsetDataSeries : public EventSeries {
    /**
     * @brief the maximum onset value ever in the series (maybe now deleted)
     */
    float maxOnsetValue = 0.00001f;
    /**
     * @brief the maximum threshold value ever in the series (maybe now deleted)
     */
    float maxThreshold = 0.00001f;

public:
    /**
     * @brief OnsetDataSeries creates an OnsetDataSeries object to collect the data produces from a OnsetAnalysis
     * @param samplesPerSecond the samples per second used
     */
    explicit OnsetDataSeries(unsigned samplesPerSecond) : EventSeries(samplesPerSecond) {}

    /**
     * @brief getMaxOnsetValue return the maximum onset value ever in the series (maybe now deleted)
     * @return the maximum onset value ever in the series (maybe now deleted)
     */
    float getMaxOnsetValue() const { return maxOnsetValue; }
    /**
     * @brief getMaxThreshold returns the maximum threshold value ever in the series (maybe now deleted)
     * @return the maximum threshold value ever in the series (maybe now deleted)
     */
    float getMaxThreshold() const { return maxThreshold; }

    /**
     * @brief The onset_data_t struct holds the data for one timepoint
     */
    struct onset_data_t {
        /**
         * @brief sample the position of the data in samples
         */
        const unsigned sample;
        /**
         * @brief onsetValue the onset value from the OnsetAnalysis at time sample
         */
        const float onsetValue;
        /**
         * @brief currentThreshold the threshold that was current at time sample
         */
        const float currentThreshold;
        onset_data_t(unsigned sample, float onsetValue, float currentThreshold) : sample(sample), onsetValue(onsetValue), currentThreshold(currentThreshold) {}
    };

private:
    /**
     * @brief onsetData the onset values
     */
    std::deque<onset_data_t> onsetData;
    /**
     * @brief onsetDatatMutex is a mutex to protect the onsetData list
     */
    mutable std::mutex onsetDatatMutex;

public:
    /**
     * @brief addOnsetData adds onset data to the onset data list. The data must be newer than the newest existing data.
     * @param sample the position of the new data
     * @param onsetValue the onset value at the timepoint
     * @param currentThreshold the threshold at the timepoint
     */
    void addOnsetData(unsigned sample, float onsetValue, float currentThreshold) {
        std::unique_lock l(onsetDatatMutex);
        onsetData.emplace_back(sample, onsetValue, currentThreshold);
        maxOnsetValue = std::max(maxOnsetValue, onsetValue);
        maxThreshold = std::max(maxThreshold, currentThreshold);
    }
    /**
     * @brief increaseNewestSampleBy increases the newest sample count, this should be done if new samples are availible. Also discard old events and onset values
     * @param count the amount of new samples
     */
    void increaseNewestSampleBy(unsigned count) {
        EventSeries::increaseNewestSampleBy(count);
        if (getNeededRange() < getNewestSample()) {
            while (!onsetData.empty() && onsetData.front().sample < getNewestSample() - getNeededRange()) {
                onsetData.pop_front();
            }
        }
    }
    /**
     * @brief getOnsetData return the onset values in a wrapper that locks the list, you should release the Locked object as
     * fast as possible, otherwise you block the whole system
     * @return A Locked object holding the onset value list
     */
    [[nodiscard]] Locked<std::deque<onset_data_t>, std::mutex> getOnsetData() { return Locked(onsetData, onsetDatatMutex); }

    /**
     * @brief getOnsetData return the onset values in a wrapper that locks the list, you should release the Locked object as
     * fast as possible, otherwise you block the whole system
     * @return A Locked object holding the onset value list
     */
    [[nodiscard]] Locked<const std::deque<onset_data_t>, std::mutex> getOnsetData() const { return Locked(onsetData, onsetDatatMutex); }
};

} // namespace Audio

#endif // AUDIOEVENTDATA_H
