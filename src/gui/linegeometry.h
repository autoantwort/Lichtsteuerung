#ifndef LINEGEOMETRY_H
#define LINEGEOMETRY_H

#include <QQuickItem>
#include <QSGGeometry>
#include <atomic>
#include <utility>
#include <vector>

namespace GUI {

class LineGeometry : public QQuickItem {
    Q_OBJECT
    std::vector<float> data;
    QColor lineColor = QColor(0, 0, 0);
    Q_PROPERTY(QColor lineColor READ getLineColor WRITE setLineColor NOTIFY lineColorChanged)
    Q_PROPERTY(bool visibleForUser MEMBER visibleForUser NOTIFY visibleForUserChanged)
    bool visibleForUser = true;
    std::atomic_bool haveNewData;

protected:
    [[nodiscard]] const std::vector<float> &getData() const { return data; }

public:
    explicit LineGeometry(QQuickItem *parent = nullptr);

    void showData(float *data, int size);

    void setLineColor(const QColor &c);
    QColor getLineColor() { return lineColor; }

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *transformNode) override;
    void timerEvent(QTimerEvent *event) override;

private:
    virtual int getNeededVertexCount() { return static_cast<int>(data.size()); }
    virtual void fillVertexData(QSGGeometry::Point2D *vertices) = 0;
    virtual void processNewData() {}
signals:
    void lineColorChanged();
    void visibleForUserChanged();
};

} // namespace GUI

#endif // LINEGEOMETRY_H
