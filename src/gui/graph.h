#ifndef GRAPH_H
#define GRAPH_H

#include "linegeometry.h"

namespace GUI {

class Graph : public LineGeometry {
    Q_OBJECT
    static Graph *lastCreated;

public:
    explicit Graph(QQuickItem *parent = nullptr);
    ~Graph() override;
    Q_DISABLE_COPY_MOVE(Graph)

    static Graph *getLast() { return lastCreated; }

private:
    void fillVertexData(QSGGeometry::Point2D *vertices) override;
    void processNewData() override;
};

} // namespace GUI

#endif // GRAPH_H
