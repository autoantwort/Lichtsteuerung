#include "oscillogram.h"
#include "QSGFlatColorMaterial"
#include <QSGGeometryNode>

namespace GUI {

Oscillogram *Oscillogram::lastCreated = nullptr;

Oscillogram::Oscillogram(QQuickItem *parent) : LineGeometry(parent) {
    lastCreated = this;
}

Oscillogram::~Oscillogram() {
    if (lastCreated == this) {
        lastCreated = nullptr;
    }
}

void Oscillogram::processNewData() {
    maxValue = std::max(maxValue, *std::max_element(getData().begin(), getData().end()));
    scale = std::min<float>(250.f, static_cast<float>(height()) / 2 - 50) / maxValue;
    maxValue *= 0.9999f;
}

void Oscillogram::fillVertexData(QSGGeometry::Point2D *vertices) {
    const auto &data = getData();
    for (size_t i = 0; i < data.size(); ++i) {
        vertices->x = static_cast<float>(i);
        vertices->y = static_cast<float>(height()) / 2 - (data[i]) * scale;
        ++vertices;
    }
}

} // namespace GUI
