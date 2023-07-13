#include "dmxchannelfilter.h"
#include "device.h"
#include "modelmanager.h"
#include "programm.h"

namespace DMX {

DMXChannelFilter::DMXChannelFilter(const QJsonObject &o)
    : maxOperation(static_cast<Operation>(o["maxOperation"].toInt())), minOperation(static_cast<Operation>(o["minOperation"].toInt())), maxValue(o["maxValue"].toInt()),
      minValue(o["minValue"].toInt()), value(o["value"].toInt()), shouldOverrideValue_(o["shouldOverride"].toBool()) {}

void DMXChannelFilter::writeJsonData(QJsonObject &o) {

    o.insert("maxOperation", maxOperation);
    o.insert("minOperation", minOperation);
    o.insert("maxValue", maxValue);
    o.insert("minValue", minValue);
    o.insert("value", value);
    o.insert("shouldOverride", shouldOverrideValue_);
}

void DMXChannelFilter::filterValue(unsigned char *value) {
    if (shouldOverrideValue_) {
        *value = this->value;
    } else {
        if (minValue != 0 && maxValue != 255) {
            if (minOperation == REMAP && maxOperation == REMAP) {
                *value = (unsigned char)(*value * (maxValue - minValue) / 255.f) + minValue;
            } else if (minOperation == CUT && maxOperation == REMAP) {
                *value = (unsigned char)(*value * maxValue / 255.f);
                if (*value < minValue) *value = minValue;
            } else if (minOperation == REMAP && maxOperation == CUT) {
                *value = (unsigned char)(*value * (255 - minValue) / 255.f) + minValue;
                if (*value > maxValue) *value = maxValue;
            } else /*if(minOperation==CUT&&maxOperation==CUT)*/ {
                if (*value < minValue) *value = minValue;
                if (*value > maxValue) *value = maxValue;
            }
        } else if (minValue != 0) {
            if (minOperation == CUT) {
                if (*value < minValue) *value = minValue;
            } else { // REMAP
                *value = (unsigned char)(*value * (255 - minValue) / 255.f) + minValue;
            }
        } else if (maxValue != 255) {
            if (maxOperation == CUT) {
                if (*value > maxValue) *value = maxValue;
            } else { // REMAP
                *value = (unsigned char)(*value * maxValue / 255.f);
            }
        }
    }
}

void DMXChannelFilter::initValue(unsigned char *value) {
    *value = this->value;
}

void DMXChannelFilter::initValues(unsigned char *values, unsigned int numberOfChannels) {
    for (const auto &d : ModelManager::get().getDevices()) {
        for (const auto &f : d->getChannelFilter()) {
            const auto index = d->getStartDMXChannel() + f.first->getIndex();
            if (index < numberOfChannels) {
                f.second->initValue(values + index);
            }
        }
    }
}

void DMXChannelFilter::filterValues(unsigned char *values, unsigned int numberOfChannels) {
    for (const auto &d : ModelManager::get().getDevices()) {
        for (const auto &f : d->getChannelFilter()) {
            const auto index = d->getStartDMXChannel() + f.first->getIndex();
            if (index < numberOfChannels) {
                f.second->filterValue(values + index);
            }
        }
    }
}

} // namespace DMX
