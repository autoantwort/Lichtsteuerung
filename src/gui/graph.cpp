#include "graph.h"
#include "QSGFlatColorMaterial"
#include <QSGGeometryNode>
#include <cmath>

namespace GUI {

Graph *Graph::lastCreated = nullptr;

Graph::Graph(QQuickItem *parent) : QQuickItem(parent), haveNewData(false) {
    setFlag(ItemHasContents);
    lastCreated = this;
    startTimer(15);
}

Graph::~Graph() {
    if (lastCreated == this) {
        lastCreated = nullptr;
    }
}

void Graph::showData(float *data, int size) {
    setImplicitWidth(size * 2);
    this->data.assign(data, data + size);
    haveNewData.store(true);
}

void Graph::setLineColor(const QColor &c) {
    lineColor = c;
    update();
    lineColorChanged();
}

QSGNode *Graph::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *transformNode) {
    Q_UNUSED(transformNode)
    QSGNode *node = nullptr;
    QSGGeometryNode *lineNode = nullptr;
    QSGGeometry *lineGeometry = nullptr;
    if (!oldNode) {
        node = new QSGNode;

        lineNode = new QSGGeometryNode;
        lineGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), static_cast<int>(data.size()));
        lineGeometry->setLineWidth(1);
        lineGeometry->setDrawingMode(QSGGeometry::DrawLineStrip);
        lineNode->setGeometry(lineGeometry);
        lineNode->setFlag(QSGNode::OwnsGeometry);

        auto *material = new QSGFlatColorMaterial;
        material->setColor(lineColor);
        lineNode->setMaterial(material);
        lineNode->setFlag(QSGNode::OwnsMaterial);

        node->appendChildNode(lineNode);
    } else {
        node = oldNode;

        lineNode = static_cast<QSGGeometryNode *>(node->childAtIndex(0));
        lineGeometry = lineNode->geometry();
        lineGeometry->allocate(static_cast<int>(data.size()));
        static_cast<QSGFlatColorMaterial *>(lineNode->material())->setColor(lineColor);
    }

    QSGGeometry::Point2D *vertices = lineGeometry->vertexDataAsPoint2D();

    for (int i = 0; i < static_cast<int>(data.size()); ++i) {
        vertices->x = i * 2;
        vertices->y = height() - 2 * data[i];
        ++vertices;
    }

    haveNewData = false;
    lineNode->markDirty(QSGNode::DirtyGeometry);
    return node;
}

void Graph::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event)
    if (visibleForUser && haveNewData) {
        update();
    }
}

} // namespace GUI
