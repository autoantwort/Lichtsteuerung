#include "channelprogrammeditor.h"
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>
#include <QSGVertexColorMaterial>
#include <cmath>

namespace GUI {

CurrentTimePointWrapper::CurrentTimePointWrapper(ChannelProgrammEditor *editor) : editor(editor) {}

void CurrentTimePointWrapper::setValue(unsigned char value) {
    auto cur = editor->getCurrentTimePoint();
    if (cur) {
        if (cur->value != value) {
            cur->value = value;
            emit valueChanged();
            editor->update();
        }
    }
}

unsigned char CurrentTimePointWrapper::getValue() const {
    auto cur = editor->getCurrentTimePoint();
    if (cur) return cur->value;
    return 0;
}
bool CurrentTimePointWrapper::hasCurrent() {
    return editor->haveCurrentTimePoint();
}
void CurrentTimePointWrapper::setTime(double time) {
    if (editor->channelProgramm) {
        auto cur = editor->currentTimePoint;
        if (cur == editor->channelProgramm->timeline.cend()) return;
        double min = 0.;
        if (cur != editor->channelProgramm->timeline.begin()) {
            --cur;
            min = cur->time;
            ++cur;
        }
        ++cur;
        double max = 99999.;
        if (cur != editor->channelProgramm->timeline.cend()) {
            max = cur->time;
        }
        time = std::max(min, std::min(max, time));
        --cur;
        if (cur->time != time) {
            editor->channelProgramm->changeTimeOfTimePoint(cur->time, time);
            editor->currentTimePoint = editor->channelProgramm->timeline.cend();
            emit hasCurrentChanged();
            emit timeChanged();
            editor->update();
        }
    }
}

double CurrentTimePointWrapper::getTime() const {
    auto cur = editor->getCurrentTimePoint();
    if (cur) return cur->time;
    return 0;
}

ChannelProgrammEditor::ChannelProgrammEditor() : currentTimePointWrapper(this), modifier() {
    setFlag(ItemHasContents, true);
    setFlag(ItemIsFocusScope, true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setEnabled(true);
    setFocus(true);
    setActiveFocusOnTab(true);
    setKeepMouseGrab(true);
    setAntialiasing(true);
}

void ChannelProgrammEditor::setChannelProgramm(DMX::ChannelProgramm *p) {
    if (p != channelProgramm) {
        channelProgramm = p;
        currentTimePoint = channelProgramm->timeline.cend();
        currentSegment = channelProgramm->timeline.cend();
        update();
        emit channelProgrammChanged();
    }
}

QSGNode *ChannelProgrammEditor::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *nodeData) {
    Q_UNUSED(nodeData)
    ChannelProgrammEditor::updatePolish();
    QSGTransformNode *node = nullptr;
    QSGGeometryNode *lineNode = nullptr;
    QSGGeometry *lineGeometry = nullptr;
    QSGGeometryNode *pointNode = nullptr;
    QSGGeometry *pointGeometry = nullptr;
    const int segments = 128;
    // effective version: const int count = channelProgramm?channelProgramm->timeline.empty()?0:channelProgramm->timeline.crbegin()->time*16:0;
    const int count = channelProgramm ? channelProgramm->timeline.empty() ? 0 : channelProgramm->timeline.begin()->time < 0.01 ? (channelProgramm->timeline.size() - 1) * segments : channelProgramm->timeline.size() * segments : 0;
    const int pointCount = channelProgramm ? channelProgramm->timeline.size() * 6 : 0;

    if (!oldNode) {
        node = new QSGTransformNode;
        lineNode = new QSGGeometryNode;
        lineGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), count);
        lineGeometry->setLineWidth(1);
        lineGeometry->setDrawingMode(QSGGeometry::DrawLineStrip);
        lineNode->setGeometry(lineGeometry);
        lineNode->setFlag(QSGNode::OwnsGeometry);

        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(graphColor);
        lineNode->setMaterial(material);
        lineNode->setFlag(QSGNode::OwnsMaterial);
        node->appendChildNode(lineNode);

        pointNode = new QSGGeometryNode;
        pointGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), pointCount);
        pointGeometry->setDrawingMode(QSGGeometry::DrawTriangles);
        pointNode->setGeometry(pointGeometry);
        pointNode->setFlag(QSGNode::OwnsGeometry);

        pointNode->setMaterial(new QSGVertexColorMaterial);
        pointNode->setFlag(QSGNode::OwnsMaterial);

        node->appendChildNode(pointNode);
    } else {
        node = static_cast<QSGTransformNode *>(oldNode);
        lineNode = static_cast<QSGGeometryNode *>(node->childAtIndex(0));
        lineGeometry = lineNode->geometry();
        lineGeometry->allocate(count);
        static_cast<QSGFlatColorMaterial *>(lineNode->material())->setColor(graphColor);
        pointNode = static_cast<QSGGeometryNode *>(node->childAtIndex(1));
        pointGeometry = pointNode->geometry();
        pointGeometry->allocate(pointCount);
    }

    QMatrix4x4 matrix;

    matrix.translate(totalXOffset, 0);
    matrix.scale(xScale, 1.0);
    node->setMatrix(matrix);

    if (count) {

        QRectF bounds = boundingRect();
        QSGGeometry::Point2D *vertices = lineGeometry->vertexDataAsPoint2D();

        // const qreal stepSize = 1./segments;
        auto iter = channelProgramm->timeline.cbegin();
        /*
         * effective version :
         *
        int i=0;
        if(iter->time>0.001){
            const auto from = channelProgramm->timeline.crbegin()->value;
            const auto to = iter->value;
            for(;;++i){
                const auto progress = i*stepSize / iter->time;
                if(progress>0.99){
                    vertices[i].y = to;
                    break;
                }
                vertices[i].y = channelProgramm->timeline.crbegin()->easingCurveToNextPoint.valueForProgress(progress) * (to-from)+from;
            }
        }
        for(;iter!=channelProgramm->timeline.cend()&&i<count;++i) {

        }*/
        if (iter->time > 0.01) {
            const auto from = channelProgramm->timeline.crbegin()->value;
            const auto to = iter->value;
            for (int i = 0; i < segments; ++i, ++vertices) {
                const auto progress = i / (segments - 1.);
                vertices->x = progress * iter->time;
                vertices->y = 255 - (channelProgramm->timeline.crbegin()->easingCurveToNextPoint.valueForProgress(progress) * (to - from) + from);
                vertices->y /= 255.;
                vertices->y *= bounds.height();
            }
        }
        for (;;) {
            const auto from = iter->value;
            const auto offsetTime = iter->time;
            const auto &curve = iter->easingCurveToNextPoint;
            ++iter;
            if (iter == channelProgramm->timeline.cend()) {
                break;
            }
            const auto to = iter->value;
            const auto timeDiff = iter->time - offsetTime;
            for (int i = 0; i < segments; ++i, ++vertices) {
                const auto progress = i / (segments - 1.);
                vertices->x = progress * timeDiff + offsetTime;
                vertices->y = 255 - (curve.valueForProgress(progress) * (to - from) + from);
                vertices->y /= 255.;
                vertices->y *= bounds.height();
            }
        }
        {

            QRectF bounds = boundingRect();
            QSGGeometry::ColoredPoint2D *vertices = pointGeometry->vertexDataAsColoredPoint2D();
            for (auto iter = channelProgramm->timeline.cbegin(); iter != channelProgramm->timeline.cend(); ++iter) {
                for (int i = 0; i < 6; ++i) {
                    constexpr std::array<double, 6> xDiffs = {-2, 2, -2, -2, 2, 2};
                    constexpr std::array<float, 6> yDiffs = {-2, -2, 2, 2, -2, 2};
                    vertices->x = static_cast<float>(iter->time + xDiffs[i] * 1 / xScale);
                    vertices->y = 255.f - iter->value;
                    vertices->y /= 255.f;
                    vertices->y *= static_cast<float>(bounds.height());
                    vertices->y += yDiffs[i];
                    if (iter == currentTimePoint) {
                        vertices->r = 0;
                        vertices->g = 255;
                        vertices->b = 0;
                        vertices->a = 255;
                    } else {
                        if (lastMousePosition != INVALID_POS) {
                            const auto xDiff = (vertices->x - mapFromVisualX(lastMousePosition.x())) * (xScale);
                            const auto yDiff = vertices->y - lastMousePosition.y();
                            auto disstance = (int)std::sqrt(xDiff * xDiff + yDiff * yDiff) * 2;
                            if (disstance < 255) {
                                vertices->r = 255 - disstance;
                                vertices->g = 0;
                                vertices->b = disstance;
                                vertices->a = 255;
                            } else {
                                vertices->r = 0;
                                vertices->g = 0;
                                vertices->b = 255;
                                vertices->a = 255;
                            }
                        } else {
                            vertices->r = 0;
                            vertices->g = 0;
                            vertices->b = 255;
                            vertices->a = 255;
                        }
                    }
                    ++vertices;
                }
            }
        }
    }

    lineNode->markDirty(QSGNode::DirtyGeometry);
    pointNode->markDirty(QSGNode::DirtyGeometry);

    return node;
}

void ChannelProgrammEditor::updateHoverSegment() {
    auto i = channelProgramm->timeline.upper_bound(mapFromVisualX(lastMousePosition.x()));
    double newX;
    double newLength;
    if (i == channelProgramm->timeline.end()) {
        newX = 0;
        newLength = 0;
    } else {
        // i is the first element
        if (i == channelProgramm->timeline.begin()) {
            newX = mapToVisualX(0);
            newLength = xScale * i->time;
        } else {
            auto previous = i;
            --previous;
            newX = mapToVisualX(previous->time);
            newLength = xScale * (i->time - previous->time);
        }
    }
    if (newX != hoverSegmentX) {
        hoverSegmentX = newX;
        emit hoverSegmentXChanged();
    }
    if (newLength != hoverSegmentLength) {
        hoverSegmentLength = newLength;
        emit hoverSegmentLengthChanged();
    }
}

void ChannelProgrammEditor::updateSelectedSegment() {
    double newX;
    double newLength;
    if (currentSegment == channelProgramm->timeline.end()) {
        newX = 0;
        newLength = 0;
    } else {
        auto next = currentSegment;
        ++next;
        // i is the first segment
        if (next == channelProgramm->timeline.end()) {
            newX = mapToVisualX(0);
            newLength = xScale * channelProgramm->timeline.begin()->time;
        } else {
            newX = mapToVisualX(currentSegment->time);
            newLength = xScale * (next->time - currentSegment->time);
        }
    }
    if (newX != selectedSegmentX) {
        selectedSegmentX = newX;
        emit selectedSegmentXChanged();
    }
    if (newLength != selectedSegmentLength) {
        selectedSegmentLength = newLength;
        emit selectedSegmentLengthChanged();
    }
}

decltype(DMX::ChannelProgramm::timeline)::iterator ChannelProgrammEditor::getTimePointForPosition(int x_, int y_) {
    auto x = mapFromVisualX(x_);
    auto y = getScaledY(y_);
    if (channelProgramm->timeline.empty()) {
        return channelProgramm->timeline.cend();
    }
    auto best = channelProgramm->timeline.end();
    auto minDist = clickRadius;
    auto begin = channelProgramm->timeline.cbegin();
    auto end = channelProgramm->timeline.cend();
    if (begin == channelProgramm->timeline.cend()) {
        begin = channelProgramm->timeline.end();
        --begin;
    }
    for (auto i = begin; i != end; ++i) {
        const auto xDiff = (i->time - x) * (xScale);
        const auto yDiff = i->value - y;
        auto dist = std::sqrt(xDiff * xDiff + yDiff * yDiff);        
        if (dist < minDist) {
            minDist = dist;
            best = i;
        }
    }
    return best;
}

QEasingCurve *ChannelProgrammEditor::getCurveForPoint(int x) {
    if (!channelProgramm) return nullptr;
    if (channelProgramm->timeline.empty()) return nullptr;
    auto lowerBound = channelProgramm->timeline.lower_bound(x);
    if (lowerBound == channelProgramm->timeline.cend()) {
        return &channelProgramm->timeline.crbegin()->easingCurveToNextPoint;
    } else {
        return &lowerBound->easingCurveToNextPoint;
    }
}

void ChannelProgrammEditor::setSelectedEasingCurve(QEasingCurve::Type type) {
    if (currentSegment != channelProgramm->timeline.end() && currentSegment->easingCurveToNextPoint.type() != type) {
        currentSegment->easingCurveToNextPoint.setType(type);
        emit selectedEasingCurveChanged();
        update();
    }
}

void ChannelProgrammEditor::mousePressEvent(QMouseEvent *event) {
    update();
    lastMousePosition = event->pos();
    forceActiveFocus(Qt::MouseFocusReason);
    event->accept();
    mousePressTimestamp = event->timestamp();

    if (!channelProgramm) return;    
    if (channelProgramm->timeline.empty()) {
        if (modifier.testFlag(N_PRESSED)) {
            currentTimePoint = channelProgramm->timeline.insert(DMX::TimePoint(mapFromVisualX(event->position().x()), getScaledY(event->position().y()), QEasingCurve::Linear)).first;
            update();
            updateHoverSegment();
        }
        return;
    }
    if (modifier.testFlag(D_PRESSED)) {
        auto iter = getTimePointForPosition(event->position().x(), event->position().y());
        if (iter != channelProgramm->timeline.end()) {
            if (iter == currentSegment) {
                currentSegment = channelProgramm->timeline.end();
                emit selectedEasingCurveChanged();
                updateSelectedSegment();
            }
            channelProgramm->timeline.erase(iter);
            update();
            updateHoverSegment();
        }
        currentTimePoint = channelProgramm->timeline.cend();
        emit currentTimePointWrapper.hasCurrentChanged();
    } else {
        if (modifier.testFlag(N_PRESSED)) {
            if (getTimePointForPosition(event->position().x(), event->position().y()) == channelProgramm->timeline.end()) {
                currentTimePoint = channelProgramm->timeline.insert(DMX::TimePoint(mapFromVisualX(event->position().x()), getScaledY(event->position().y()), QEasingCurve::Linear)).first;
                update();
                updateHoverSegment();
            }
        } else {
            currentTimePoint = getTimePointForPosition(event->position().x(), event->position().y());
            if (currentTimePoint == channelProgramm->timeline.end()) {
                // no timepoints gets selected, lets select a segment
                auto i = channelProgramm->timeline.upper_bound(mapFromVisualX(event->position().x()));
                // the timepoint for the segment before the first timepoint is the last timepoint
                if (i == channelProgramm->timeline.begin()) {
                    i = channelProgramm->timeline.end();
                    // we want the left timepoint of an segment
                    --i;
                } else if (i != channelProgramm->timeline.end()) {
                    // we want the left timepoint of an segment
                    --i;
                }
                if (i != currentSegment) {
                    currentSegment = i;
                    emit selectedEasingCurveChanged();
                    updateSelectedSegment();
                }
            } else {
                if (currentSegment != channelProgramm->timeline.end()) {
                    currentSegment = channelProgramm->timeline.end();
                    emit selectedEasingCurveChanged();
                    updateSelectedSegment();
                }
            }
        }
        emit currentTimePointWrapper.hasCurrentChanged();
        emit currentTimePointWrapper.valueChanged();
        emit currentTimePointWrapper.timeChanged();
    }
}

void ChannelProgrammEditor::keyReleaseEvent(QKeyEvent *event) {
    if (!event->modifiers().testFlag(Qt::ShiftModifier)) {
        switch (event->key()) {
        case Qt::Key::Key_N: modifier.setFlag(Modifier::N_PRESSED, false); break;
        case Qt::Key::Key_D: modifier.setFlag(Modifier::D_PRESSED, false); break;
        case Qt::Key::Key_X: modifier.setFlag(Modifier::X_PRESSED, false); break;
        case Qt::Key::Key_Y: modifier.setFlag(Modifier::Y_PRESSED, false); break;
        }
    }
}

void ChannelProgrammEditor::keyPressEvent(QKeyEvent *event) {
    event->accept();
    switch (event->key()) {
    case Qt::Key::Key_N: modifier.setFlag(Modifier::N_PRESSED); break;
    case Qt::Key::Key_D:
        modifier.setFlag(Modifier::D_PRESSED);
        if (haveCurrentTimePoint()) {
            if (currentTimePoint == currentSegment) {
                currentSegment = channelProgramm->timeline.end();
                emit selectedEasingCurveChanged();
                updateSelectedSegment();
            }
            channelProgramm->timeline.erase(currentTimePoint);
            currentTimePoint = channelProgramm->timeline.end();
            emit currentTimePointWrapper.hasCurrentChanged();
            update();
            updateHoverSegment();
        }
        break;
    case Qt::Key::Key_X: modifier.setFlag(Modifier::X_PRESSED); break;
    case Qt::Key::Key_Y: modifier.setFlag(Modifier::Y_PRESSED); break;
    }
    if (haveCurrentTimePoint()) {
        switch (event->key()) {
        case Qt::Key::Key_Up: {
            auto oldValue = currentTimePoint->value;
            if (event->modifiers().testFlag(Qt::MetaModifier)) {
                currentTimePoint->value = 255;
            } else if (event->modifiers().testFlag(Qt::ControlModifier)) {
                currentTimePoint->value = std::min(255, currentTimePoint->value + 25);
            } else if (event->modifiers().testFlag(Qt::ShiftModifier)) {
                currentTimePoint->value = std::min(255, currentTimePoint->value + 1);
            } else {
                currentTimePoint->value = std::min(255, currentTimePoint->value + 5);
            }
            if (oldValue != currentTimePoint->value) {
                update();
                emit currentTimePointWrapper.valueChanged();
            }
        } break;
        case Qt::Key::Key_Down: {
            auto oldValue = currentTimePoint->value;
            if (event->modifiers().testFlag(Qt::MetaModifier)) {
                currentTimePoint->value = 0;
            } else if (event->modifiers().testFlag(Qt::ControlModifier)) {
                currentTimePoint->value = std::max(0, currentTimePoint->value - 25);
            } else if (event->modifiers().testFlag(Qt::ShiftModifier)) {
                currentTimePoint->value = std::max(0, currentTimePoint->value - 1);
            } else {
                currentTimePoint->value = std::max(0, currentTimePoint->value - 5);
            }
            if (oldValue != currentTimePoint->value) {
                update();
                emit currentTimePointWrapper.valueChanged();
            }
        } break;
        case Qt::Key::Key_Left:
            // TODO
            break;
        case Qt::Key::Key_Right:
            // TODO
            break;
        }
    }
}

void ChannelProgrammEditor::wheelEvent(QWheelEvent *event) {
#ifdef Q_OS_WIN
    auto xDiff = event->angleDelta().y();
#else
    auto xDiff = event->pixelDelta().x();
#endif
    if (modifier.testFlag(X_PRESSED)) {
        xScale += xDiff * 1 / 32.;
        if (xScale < 0.05) {
            xScale = 0.05;
        }
    } else {
        totalXOffset += xDiff;
        if (totalXOffset > 0) // nach links beschränken
            totalXOffset = 0;
        if (channelProgramm) { // nach rechts beschränken
            if (!channelProgramm->timeline.empty()) {
                if (channelProgramm->timeline.crbegin()->time * xScale < -totalXOffset) {
                    totalXOffset = -channelProgramm->timeline.crbegin()->time * xScale;
                }
            }
        }
    }
    event->accept();
    update();
    updateHoverSegment();
    updateSelectedSegment();
}

void ChannelProgrammEditor::hoverMoveEvent(QHoverEvent *event) {
    lastMousePosition = event->position();
    update();
    event->accept();
    // update hover segment
    updateHoverSegment();
    emit mouseXChanged();
}

void ChannelProgrammEditor::hoverLeaveEvent(QHoverEvent *e) {
    lastMousePosition = INVALID_POS;
    emit mouseXChanged();
    update();
    e->accept();
    // no hover segment any more
    hoverSegmentLength = 0;
    emit hoverSegmentLengthChanged();
}

void ChannelProgrammEditor::mouseMoveEvent(QMouseEvent *event) {
    lastMousePosition = event->position();
    event->accept();
    if (haveCurrentTimePoint()) {
        const auto timeOffset = 0.05;
        auto before = currentTimePoint;
        auto next = currentTimePoint;
        if (before == channelProgramm->timeline.cbegin()) {
            // we are at the first time Point
            before = channelProgramm->timeline.cend();
        }
        --before;
        ++next;
        auto newX = mapFromVisualX(event->position().x());
        auto newY = std::max(0, std::min(255, getScaledY(event->position().y())));

        if (currentTimePoint == channelProgramm->timeline.cbegin()) {
            if (newX < 0) {
                newX = 0;
            } else if (newX > next->time - timeOffset) {
                newX = next->time - timeOffset;
            }
        } else if (next == channelProgramm->timeline.cend()) {
            if (newX < before->time + timeOffset) {
                newX = before->time + timeOffset;
            }
        } else {
            if (newX < before->time + timeOffset) {
                newX = before->time + timeOffset;
            } else if (newX > next->time - timeOffset) {
                newX = next->time - timeOffset;
            }
        }

        /*if (modifier.testFlag(X_PRESSED)&&!modifier.testFlag(Y_PRESSED)) {
            newY = currentTimePoint->value;
        }else if(!modifier.testFlag(X_PRESSED)&&modifier.testFlag(Y_PRESSED)){
            newX = currentTimePoint->time;
        }*/

        if (currentTimePoint->time != newX) {
            *const_cast<double *>(&currentTimePoint->time) = newX;
            emit currentTimePointWrapper.timeChanged();
        }

        if (currentTimePoint->value != newY) {
            currentTimePoint->value = newY;
            emit currentTimePointWrapper.valueChanged();
        }
        update();
    }
    emit mouseXChanged();
    // update hover segment
    updateHoverSegment();
    updateSelectedSegment();
}

void ChannelProgrammEditor::mouseReleaseEvent(QMouseEvent *event) {
    event->accept();
    lastMousePosition = INVALID_POS;
    emit mouseXChanged();
    if (event->timestamp() - mousePressTimestamp < 300) {
        if (event->button() == Qt::LeftButton) {
            emit click(event->position().x(), event->position().y());
        } else if (event->button() == Qt::RightButton) {
            emit rightClick(event->position().x(), event->position().y());
        }
    }
}

void ChannelProgrammEditor::hoverEnterEvent(QHoverEvent *e) {
    lastMousePosition = e->position();
    e->accept();
    forceActiveFocus(Qt::MouseFocusReason);
}

DMX::TimePoint *ChannelProgrammEditor::getCurrentTimePoint() {
    if (haveCurrentTimePoint()) {
        return const_cast<DMX::TimePoint *>(&*currentTimePoint);
    }
    return nullptr;
}

} // namespace GUI
