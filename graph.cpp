#include "graph.h"
#include "QSGFlatColorMaterial"
#include <QSGGeometryNode>
#include <cmath>

Graph::Graph():haveNewData(false)
{
setFlag(ItemHasContents);
lastCreated = this;
}

Graph * Graph::lastCreated = nullptr;

float mins = 1;
int counter = 1;

QSGNode * Graph::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *){
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

        /*{
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
        node->appendChildNode(pointNode);*/
        node->appendChildNode(lineNode);
    } else {
        node = static_cast<QSGNode *>(oldNode);

        lineNode = static_cast<QSGGeometryNode *>(node->childAtIndex(0));
        lineGeometry = lineNode->geometry();
        lineGeometry->allocate(size);
        static_cast<QSGFlatColorMaterial*>(lineNode->material())->setColor(lineColor);

        /*pointNode = static_cast<QSGGeometryNode *>(node->childAtIndex(0));
        pointGeometry = pointNode->geometry();
        pointGeometry->allocate(4);
        static_cast<QSGFlatColorMaterial*>(pointNode->material())->setColor(backgroundColor);*/
    }

    QSGGeometry::Point2D *vertices = lineGeometry->vertexDataAsPoint2D();

    /*if(haveNewData.load()){
        float min = 999999;
        float max = 0;
        for(int i = 0 ; i< size; ++i){
            data[i] = (std::log10(data[i]));
            min = std::min(min,data[i]);
            max = std::max(max,data[i]);
        }
        for(int i = 0 ; i< size; ++i){
            data[i]-=min;
            data[i]/=(max-min);
            data[i] = std::pow(data[i],2);
        }

        if(min > 0)
        mins += min;
        ++counter;
    }

    float scale = 0.1;
    float curMin = mins/counter;
    qDebug() << ' '<<mins << ' ' << curMin <<' '<<counter;
*/
    for(int i = 0 ; i< size; ++i){
        vertices->x = i*2;
        vertices->y = height() - 50 - 20* (std::log10(data[i]));
        ++vertices;
    }

    haveNewData = false;
    lineNode->markDirty(QSGNode::DirtyGeometry);
    return node;

}
