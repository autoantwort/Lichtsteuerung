#include "audioanalysisobject.h"
#include "QJsonArray"

namespace Spotify::Objects {

template <typename T>
std::vector<T> createVector(const QJsonValue &value){
    std::vector<T> v;
    for(const auto e : value.toArray()){
        v.emplace_back(e.toObject());
    }
    return v;
}

AudioAnalysisObject::AudioAnalysisObject(const QJsonObject &object):
    bars(createVector<TimeIntervalObject>(object["bars"])),
    beats(createVector<TimeIntervalObject>(object["beats"])),
    sections(createVector<SectionObject>(object["sections"])),
    segments(createVector<SegmentObject>(object["segments"])),
    tatums(createVector<TimeIntervalObject>(object["tatums"]))
{

}

}
