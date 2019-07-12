#include "colorplot.h"
#include "QSGFlatColorMaterial"
#include <QSGGeometryNode>
#include <QSGVertexColorMaterial>

namespace GUI{

Colorplot::Colorplot()
{
setFlag(ItemHasContents);
lastCreated = this;
}

Colorplot * Colorplot::lastCreated = nullptr;



QSGNode * Colorplot::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *){
    QSGNode *node = nullptr;
    QSGGeometryNode *lineNode = nullptr;
    QSGGeometry *lineGeometry = nullptr;
    int size2 = MAX_BLOCKS_COUNT * (blockSize+1);
    if (!oldNode) {
        node  = new QSGNode;
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
        lineNode = static_cast<QSGGeometryNode *>(node->childAtIndex(0));
        lineGeometry = lineNode->geometry();
        lineGeometry->allocate(size2);
    }
    QSGGeometry::ColoredPoint2D *lines = lineGeometry->vertexDataAsColoredPoint2D();
    int x = 0;
    for(int i = 0; i < MAX_BLOCKS_COUNT ;++i){
        ++x;
        lines->x = x + 50;
        lines->y = height()-(blockSize-1)*2-1;
        ++lines;
        auto random = rand()%255;
        for (int i = 0; i < static_cast<int>(blockSize); ++i) {
            lines->x = x + 50;
            lines->y  = height()-i*2-1;
            lines->a  = 255;
            lines->r  = random;
            lines->g  = 200;
            lines->b  = 0;
            ++lines; //++points;
        }
    }
    lineNode->markDirty(QSGNode::DirtyGeometry);
    return node;

}

} // namespace GUI
