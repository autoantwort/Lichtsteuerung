#include "gridbackground.h"
#include "QSGFlatColorMaterial"
#include <QSGGeometryNode>

GridBackground::GridBackground()
{
    setFlag(ItemHasContents);
}

QSGNode * GridBackground::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *){
    QSGNode *node = nullptr;
    QSGGeometryNode *lineNode = nullptr;
    QSGGeometry *lineGeometry = nullptr;
    QSGGeometryNode *pointNode = nullptr;
    QSGGeometry *pointGeometry = nullptr;
    int count = int(width())/distanceBetweenLines*2+int(height())/distanceBetweenLines*2+4;
    if (!oldNode) {
        node  = new QSGNode;
        {
            lineNode = new QSGGeometryNode;
            lineGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), count);
            lineGeometry->setLineWidth(1);
            lineGeometry->setDrawingMode(QSGGeometry::DrawLines);
            lineNode->setGeometry(lineGeometry);
            lineNode->setFlag(QSGNode::OwnsGeometry);


            QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
            material->setColor(lineColor);
            lineNode->setMaterial(material);
            lineNode->setFlag(QSGNode::OwnsMaterial);
        }

        {
            pointNode = new QSGGeometryNode;
            pointGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 4);
            pointGeometry->setLineWidth(1);
            pointGeometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);
            pointNode->setGeometry(pointGeometry);
            pointNode->setFlag(QSGNode::OwnsGeometry);

            QSGFlatColorMaterial * material = new QSGFlatColorMaterial;
            material->setColor(backgroundColor);
            pointNode->setMaterial(material);
            pointNode->setFlag(QSGNode::OwnsMaterial);
        }
        node->appendChildNode(pointNode);
        node->appendChildNode(lineNode);
    } else {
        node = static_cast<QSGNode *>(oldNode);

        lineNode = static_cast<QSGGeometryNode *>(node->childAtIndex(1));
        lineGeometry = lineNode->geometry();
        lineGeometry->allocate(count);
        static_cast<QSGFlatColorMaterial*>(lineNode->material())->setColor(lineColor);

        pointNode = static_cast<QSGGeometryNode *>(node->childAtIndex(0));
        pointGeometry = pointNode->geometry();
        pointGeometry->allocate(4);
        static_cast<QSGFlatColorMaterial*>(pointNode->material())->setColor(backgroundColor);
    }

    QSGGeometry::Point2D *vertices = lineGeometry->vertexDataAsPoint2D();
    vertices->x=0;
    vertices->y=0;
    ++vertices;
    vertices->x=width();
    vertices->y=0;
    ++vertices;
    vertices->x=0;
    vertices->y=0;
    ++vertices;
    vertices->x=0;
    vertices->y=height();
    ++vertices;
    for(int i = distanceBetweenLines;i<width();i+=distanceBetweenLines){
        vertices->x=i;
        vertices->y=0;
        ++vertices;
        vertices->x=i;
        vertices->y=height();
        ++vertices;
    }
    for(int i = distanceBetweenLines;i<height();i+=distanceBetweenLines){
        vertices->x=0;
        vertices->y=i;
        ++vertices;
        vertices->x=width();
        vertices->y=i;
        ++vertices;
    }

    vertices = (QSGGeometry::Point2D*)pointGeometry->vertexData();
    vertices->x=0;
    vertices->y=0;
    ++vertices;
    vertices->x=width();
    vertices->y=0;
    ++vertices;
    vertices->x=0;
    vertices->y=height();
    ++vertices;
    vertices->x=width();
    vertices->y=height();

    lineNode->markDirty(QSGNode::DirtyGeometry);
    pointNode->markDirty(QSGNode::DirtyGeometry);

    return node;

}
