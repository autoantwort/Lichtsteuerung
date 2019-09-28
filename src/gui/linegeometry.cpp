#include "linegeometry.h"
#include "QSGFlatColorMaterial"
#include <QSGGeometryNode>
#include <cmath>

namespace GUI {

LineGeometry::LineGeometry(QQuickItem *parent) : QQuickItem(parent), haveNewData(false) {
    setFlag(ItemHasContents);
    startTimer(15);
}

void LineGeometry::showData(float *data, int size) {
    this->data.assign(data, data + size);
    haveNewData.store(true);
    processNewData();
}

void LineGeometry::setLineColor(const QColor &c) {
    lineColor = c;
    update();
    lineColorChanged();
}

QSGNode *LineGeometry::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *transformNode) {
    Q_UNUSED(transformNode)
    QSGNode *node = nullptr;
    QSGGeometryNode *lineNode = nullptr;
    QSGGeometry *lineGeometry = nullptr;
    if (!oldNode) {
        node = new QSGNode;

        lineNode = new QSGGeometryNode;
        lineGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), getNeededVertexCount());
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
        lineGeometry->allocate(getNeededVertexCount());
        static_cast<QSGFlatColorMaterial *>(lineNode->material())->setColor(lineColor);
    }

    QSGGeometry::Point2D *vertices = lineGeometry->vertexDataAsPoint2D();

    fillVertexData(vertices);

    haveNewData = false;
    lineNode->markDirty(QSGNode::DirtyGeometry);
    return node;
}

void LineGeometry::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event)
    if (visibleForUser && haveNewData) {
        update();
    }
}

} // namespace GUI
