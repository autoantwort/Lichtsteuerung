#ifndef COLORPLOT_H
#define COLORPLOT_H

#include <QQuickItem>
#include <list>
#include <mutex>

namespace GUI {

class Colorplot : public QQuickItem {
    Q_OBJECT
    int blockSize = 0;
    int currentBlockCounter = -1;
    float zoom = 1;
    bool visibleForUser = true;
    std::mutex mutex;
    std::list<float *> dataBlocks;
    Q_PROPERTY(bool visibleForUser MEMBER visibleForUser NOTIFY visibleForUserChanged)
    Q_PROPERTY(float zoom MEMBER zoom NOTIFY zoomChanged)
    static Colorplot *lastCreated;
    std::atomic_bool haveNewData = false;
    const static unsigned int MAX_BLOCKS_COUNT = 900;

public:
    explicit Colorplot(QQuickItem *parent = nullptr);
    ~Colorplot() override;
    Q_DISABLE_COPY_MOVE(Colorplot)

    static Colorplot *getLast() { return lastCreated; }

    void startBlock();
    void endBlock();
    void pushDataToBlock(float d);
    void setBlockSize(int size);

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *transformNode) override;
    void timerEvent(QTimerEvent *event) override;

signals:
    void visibleForUserChanged();
    void zoomChanged();
};

} // namespace GUI

#endif // COLORPLOT_H
