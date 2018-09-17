#include "colorplot.h"
#include "QSGFlatColorMaterial"
#include <QSGGeometryNode>
#include <QSGVertexColorMaterial>


Colorplot::Colorplot():haveNewData(false)
{
setFlag(ItemHasContents);
lastCreated = this;
}

Colorplot * Colorplot::lastCreated = nullptr;



QSGNode * Colorplot::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *){
    QSGNode *node = nullptr;
    QSGGeometryNode *pointNode = nullptr;
    QSGGeometry *pointGeometry = nullptr;
    QSGGeometryNode *lineNode = nullptr;
    QSGGeometry *lineGeometry = nullptr;
    mutex.lock();
    int size = dataBlocks.size() * blockSize;
    int size2 = dataBlocks.size() * (blockSize+1);
    if (!oldNode) {
        node  = new QSGNode;
        /*{
            pointNode = new QSGGeometryNode;
            pointGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), size);
            pointGeometry->setLineWidth(2);
            pointGeometry->setDrawingMode(QSGGeometry::DrawPoints);
            pointNode->setGeometry(pointGeometry);
            pointNode->setFlag(QSGNode::OwnsGeometry);

            pointNode->setMaterial(new QSGVertexColorMaterial);
            pointNode->setFlag(QSGNode::OwnsMaterial);
        }
        node->appendChildNode(pointNode);*/
        {
            lineNode = new QSGGeometryNode;
            lineGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), size2);
            lineGeometry->setLineWidth(1);
            lineGeometry->setDrawingMode(QSGGeometry::DrawLineStrip);
            lineNode->setGeometry(lineGeometry);
            lineNode->setFlag(QSGNode::OwnsGeometry);

            lineNode->setMaterial(new QSGVertexColorMaterial);
            lineNode->setFlag(QSGNode::OwnsMaterial);
        }
        node->appendChildNode(lineNode);
    } else {
        node = static_cast<QSGNode *>(oldNode);

        /*pointNode = static_cast<QSGGeometryNode *>(node->childAtIndex(0));
        pointGeometry = pointNode->geometry();
        pointGeometry->allocate(size);*/

        lineNode = static_cast<QSGGeometryNode *>(node->childAtIndex(0));
        lineGeometry = lineNode->geometry();
        lineGeometry->allocate(size2);

    }

    //QSGGeometry::ColoredPoint2D *points = pointGeometry->vertexDataAsColoredPoint2D();
    QSGGeometry::ColoredPoint2D *lines = lineGeometry->vertexDataAsColoredPoint2D();

    int counter = 0;
    int x = 0;
    for(auto block : dataBlocks){
        ++x;
        lines->x = x + 50;
        lines->y = height()-(blockSize-1)*2-1;
        ++lines;
        for (int i = 0; i < blockSize; ++i) {
            lines->x = x + 50;
            //           points->x = x*2;
            lines->y /*= points->y*/ = height()-i*2-1;
            lines->a /*= points->a*/ = 255;
            lines->r /*= points->r*/ = (unsigned char)(block[i]*0.5>255.f?255:block[i]*0.5);
            lines->g /*= points->g*/ = (unsigned char)std::min(block[i]*0.2f,255.f);
            lines->g /*= points->g*/ = (unsigned char)std::min(block[i]/20*255,255.f);
            lines->r /*= points->b*/ = (unsigned char)(block[i]/10 * 255);
            ++lines; //++points;
            ++counter;
        }
    }
    //qDebug()<<"counter : " << counter << " size : "<< size << " blocks : "<< dataBlocks.size();

    mutex.unlock();

    haveNewData = false;
    //pointNode->markDirty(QSGNode::DirtyGeometry);
    lineNode->markDirty(QSGNode::DirtyGeometry);
    return node;

}
