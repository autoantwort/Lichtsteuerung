#ifndef OSCILLOGRAM_H
#define OSCILLOGRAM_H

#include "linegeometry.h"

namespace GUI {

class Oscillogram : public LineGeometry {
    Q_OBJECT
    float scale = 1;
    float maxValue = 0;
    static Oscillogram *lastCreated;

public:
    explicit Oscillogram(QQuickItem *parent = nullptr);
    ~Oscillogram() override;
    Q_DISABLE_COPY_MOVE(Oscillogram)

    static Oscillogram *getLast() { return lastCreated; }

private:
    void processNewData() override;
    void fillVertexData(QSGGeometry::Point2D *vertices) override;
};

} // namespace GUI

#endif // OSCILLOGRAM_H
