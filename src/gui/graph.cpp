#include "graph.h"
#include "QSGFlatColorMaterial"
#include <QSGGeometryNode>
#include <cmath>

namespace GUI {

Graph *Graph::lastCreated = nullptr;

Graph::Graph(QQuickItem *parent) : LineGeometry(parent) { lastCreated = this; }

Graph::~Graph() {
    if (lastCreated == this) {
        lastCreated = nullptr;
    }
}

void Graph::fillVertexData(QSGGeometry::Point2D *vertices) {
    const auto &data = getData();
    for (int i = 0; i < static_cast<int>(data.size()); ++i) {
        vertices->x = i * 2;
        vertices->y = height() - 2 * data[i];
        ++vertices;
    }
}

void Graph::processNewData() { setImplicitWidth(getData().size() * 2); }

} // namespace GUI
