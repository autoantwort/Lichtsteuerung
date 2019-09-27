#ifndef GRAPH_H
#define GRAPH_H

#include <QQuickItem>
#include <atomic>
#include <utility>
#include <vector>

namespace GUI {

class Graph : public QQuickItem {
    Q_OBJECT
    std::vector<float> data;
    QColor lineColor = QColor(0, 0, 0);
    Q_PROPERTY(QColor lineColor READ getLineColor WRITE setLineColor NOTIFY lineColorChanged)
    Q_PROPERTY(bool visibleForUser MEMBER visibleForUser NOTIFY visibleForUserChanged)
    bool visibleForUser = true;
    static Graph *lastCreated;
    std::atomic_bool haveNewData;

public:
    explicit Graph(QQuickItem *parent = nullptr);
    ~Graph() override;
    Q_DISABLE_COPY_MOVE(Graph)

    static Graph *getLast() { return lastCreated; }

    void showData(float *data, int size);

    void setLineColor(const QColor &c);
    QColor getLineColor() { return lineColor; }

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *transformNode) override;
    void timerEvent(QTimerEvent *event) override;
signals:
    void lineColorChanged();
    void visibleForUserChanged();
};

} // namespace GUI

#endif // GRAPH_H
