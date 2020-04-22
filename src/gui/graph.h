#ifndef GRAPH_H
#define GRAPH_H

#include "linegeometry.h"

namespace GUI {

class Graph : public LineGeometry {
    Q_OBJECT
    static Graph *lastCreated;
    bool useLogarithmicScale_ = true;
    Q_PROPERTY(bool useLogarithmicScale READ useLogarithmicScale WRITE setUseLogarithmicScale NOTIFY useLogarithmicScaleChanged)

public:
    explicit Graph(QQuickItem *parent = nullptr);
    ~Graph() override;
    Q_DISABLE_COPY_MOVE(Graph)

    static Graph *getLast() { return lastCreated; }

    [[nodiscard]] bool useLogarithmicScale() const { return useLogarithmicScale_; }
    void setUseLogarithmicScale(bool use);

private:
    void fillVertexData(QSGGeometry::Point2D *vertices) override;
    void processNewData() override;

signals:
    void useLogarithmicScaleChanged();
};

} // namespace GUI

#endif // GRAPH_H
