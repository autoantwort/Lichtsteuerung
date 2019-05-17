#include "oscillogram.h"
#include "QSGFlatColorMaterial"
#include <QSGGeometryNode>

namespace GUI{

Oscillogram::Oscillogram():haveNewData(false)
{
setFlag(ItemHasContents);
lastCreated = this;
}

Oscillogram * Oscillogram::lastCreated = nullptr;


QSGNode * Oscillogram::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *){
    QSGNode *node = nullptr;
    QSGGeometryNode *lineNode = nullptr;
    QSGGeometry *lineGeometry = nullptr;
    if (!oldNode) {
        node  = new QSGNode;
        {
            lineNode = new QSGGeometryNode;
            lineGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), size);
            lineGeometry->setLineWidth(1);
            lineGeometry->setDrawingMode(QSGGeometry::DrawLineStrip);
            lineNode->setGeometry(lineGeometry);
            lineNode->setFlag(QSGNode::OwnsGeometry);


            QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
            material->setColor(lineColor);
            lineNode->setMaterial(material);
            lineNode->setFlag(QSGNode::OwnsMaterial);
        }
        node->appendChildNode(lineNode);
    } else {
        node = static_cast<QSGNode *>(oldNode);

        lineNode = static_cast<QSGGeometryNode *>(node->childAtIndex(0));
        lineGeometry = lineNode->geometry();
        lineGeometry->allocate(size);
        static_cast<QSGFlatColorMaterial*>(lineNode->material())->setColor(lineColor);

    }

    QSGGeometry::Point2D *vertices = lineGeometry->vertexDataAsPoint2D();

    for(int i = 0 ; i< size; ++i){
        vertices->x = i;
        vertices->y = height()/2 - (data[i])*scale;
        ++vertices;
    }

    haveNewData = false;
    lineNode->markDirty(QSGNode::DirtyGeometry);
    return node;

}

} // namespace GUI
