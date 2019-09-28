#include "colorplot.h"
#include "QSGFlatColorMaterial"
#include <QSGGeometryNode>
#include <QSGVertexColorMaterial>
#include <cmath>

namespace GUI {

Colorplot *Colorplot::lastCreated = nullptr;

Colorplot::Colorplot(QQuickItem *parent) : QQuickItem(parent) {
    setFlag(ItemHasContents);
    lastCreated = this;
    startTimer(15);
}

Colorplot::~Colorplot() {
    for (const auto &data : dataBlocks) {
        delete[] data;
    }
    if (lastCreated == this) {
        lastCreated = nullptr;
    }
}

void Colorplot::startBlock() {
    mutex.lock();
    if (dataBlocks.size() >= MAX_BLOCKS_COUNT) {
        currentBlockCounter = 0;
        auto data = dataBlocks.front();
        dataBlocks.pop_front();
        dataBlocks.push_back(data);
    } else {
        currentBlockCounter = 0;
        const auto blockSize = static_cast<size_t>(this->blockSize);
        auto data = new float[blockSize];
        memset(data, 0, blockSize * sizeof(float));
        dataBlocks.push_back(data);
    }
}

void Colorplot::endBlock() {
    haveNewData = true;
    mutex.unlock();
}

void Colorplot::pushDataToBlock(float d) {
    if (currentBlockCounter >= 0) {
        dataBlocks.back()[currentBlockCounter] = d; //*0.4f;//std::pow(1+d*0.001,3)*20;
        ++currentBlockCounter;
        if (currentBlockCounter >= blockSize) {
            currentBlockCounter = -1;
        }
    } else {
        qDebug() << "Error : currentBlockCounter less then 0";
    }
}

void Colorplot::setBlockSize(int size) {
    Q_ASSERT(size >= 0);
    if (size != this->blockSize) {
        for (const auto i : dataBlocks) {
            delete[] i;
        }
        dataBlocks.clear();
        currentBlockCounter = -1;
        blockSize = size;
    }
}

QSGNode *Colorplot::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData * /*transformNode*/) {
    QSGNode *node = nullptr;
    QSGGeometryNode *lineNode = nullptr;
    QSGGeometry *lineGeometry = nullptr;
    if (!oldNode) {
        node = new QSGNode;

        lineNode = new QSGGeometryNode;

        lineGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), 0);
        lineGeometry->setLineWidth(1);
        lineGeometry->setDrawingMode(QSGGeometry::DrawLineStrip);

        lineNode->setGeometry(lineGeometry);
        lineNode->setFlag(QSGNode::OwnsGeometry);

        lineNode->setMaterial(new QSGVertexColorMaterial);
        lineNode->setFlag(QSGNode::OwnsMaterial);

        node->appendChildNode(lineNode);
        lineNode->setFlag(QSGNode::OwnedByParent);
        node->setFlag(QSGNode::OwnedByParent);
    } else {
        node = oldNode;
        lineNode = dynamic_cast<QSGGeometryNode *>(node->childAtIndex(0));
        lineGeometry = lineNode->geometry();
    }

    mutex.lock();

    int x = static_cast<int>(width());
    const auto height = static_cast<float>(this->height());
    const auto lastVisibleBlock = std::min(static_cast<int>(height / zoom), blockSize);
    const auto dataBlocksInWidth = std::min(static_cast<int>(dataBlocks.size()), static_cast<int>(width()));

    lineGeometry->allocate(dataBlocksInWidth * (lastVisibleBlock + 1));
    auto *lines = lineGeometry->vertexDataAsColoredPoint2D();

    for (auto blockI = dataBlocks.crbegin(); blockI != dataBlocks.crend(); ++blockI) {
        const auto block = *blockI;
        --x;
        if (x == -1) {
            break;
        }
        const auto currentX = static_cast<float>(x);

        lines->x = currentX;
        lines->y = height - (static_cast<float>(lastVisibleBlock - 1) * zoom) - 1;
        ++lines;
        for (int i = 0; i < lastVisibleBlock; ++i) {
            lines->x = currentX;
            lines->y = height - static_cast<float>(i) * zoom - 1;
            lines->a = 255;
            lines->r = static_cast<unsigned char>(block[i] * 0.5f > 255.f ? 255 : block[i] * 0.5f);
            lines->g = static_cast<unsigned char>(std::min(block[i] / 20 * 255, 255.f));
            lines->r = static_cast<unsigned char>(block[i] / 10 * 255);
            ++lines;
        }
    }
    mutex.unlock();

    haveNewData = false;
    // pointNode->markDirty(QSGNode::DirtyGeometry);
    lineNode->markDirty(QSGNode::DirtyGeometry);
    return node;
}

void Colorplot::timerEvent(QTimerEvent * /*event*/) {
    if (visibleForUser && haveNewData) {
        update();
    }
}

} // namespace GUI
