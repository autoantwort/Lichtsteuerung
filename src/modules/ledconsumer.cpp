#include "ledconsumer.h"

namespace Modules {

LedConsumer::LedConsumer() : Consumer(ValueType::RGB), name("No Name") {
    allLedConsumer.push_back(this);
    name.setName("Name");
    name.setDescription("A name to recognize this led strip in the led virtualisation view.");
    properties.push_back(&name);
    using namespace std::chrono;
    startTimer(1s);
}

void LedConsumer::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event)
    if (lastName != name.getString()) {
        emit nameChanged();
        lastName = name.getString();
    }
}
void LedConsumer::doStep(time_diff_t diff) {
    waitCounter += diff;
    if (waitCounter >= 15) {
        waitCounter = 0;
        // only update every 15 ms, the screen only has 60 fps
        dataChanged(firstChangedIndex, changedLength);
        changedLength = 0;
        firstChangedIndex = 0;
    }
}

void LedConsumer::setInputData(void *data, unsigned int index, unsigned int length) {
    const auto maxIndex = std::min(index + length, static_cast<unsigned int>(size()));
    const auto maxLength = maxIndex - index;
    auto *rgbs = static_cast<rgb_t *>(data);
    if (!std::equal(rgbs, rgbs + maxLength, getVector().data() + index)) {
        std::copy(rgbs, rgbs + maxLength, getVector().data() + index);
        if (changedLength == 0) {
            firstChangedIndex = static_cast<int>(index);
            changedLength = static_cast<int>(maxLength);
        } else if (changedLength != static_cast<int>(maxLength) || firstChangedIndex != static_cast<int>(index)) {
            // another range is changed, so simply mark the whole range as changed
            const auto maxIndex = std::max(firstChangedIndex + changedLength, static_cast<int>(index) + static_cast<int>(maxLength));
            firstChangedIndex = std::min(firstChangedIndex, static_cast<int>(index));
            changedLength = maxIndex - firstChangedIndex;
        }
    }
}

QHash<int, QByteArray> LedConsumer::roleNames() const {
    QHash<int, QByteArray> r = ModelVector::roleNames();
    r[RedDataRole] = "r";
    r[GreenDataRole] = "g";
    r[BlueDataRole] = "b";
    r[ColorDataRole] = "ledColor";
    return r;
}
QVariant LedConsumer::data(const QModelIndex &index, int role) const {
    Q_UNUSED(role);
    if (index.row() >= 0 && index.row() < int(size())) {
        switch (role) {
        case RedDataRole: return getVector()[index.row()].r;
        case GreenDataRole: return getVector()[index.row()].g;
        case BlueDataRole: return getVector()[index.row()].b;
        case ColorDataRole: const auto &rgb = getVector()[index.row()]; return QColor(rgb.r, rgb.g, rgb.b);
        }
    }
    return QVariant();
}

} // namespace Modules
