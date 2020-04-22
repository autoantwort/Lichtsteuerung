#include "audioeventdataview.h"
#include "audio/audiocapturemanager.h"

#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>

using namespace Audio::Aubio;

namespace GUI {

float AudioEventDataView::getX(const Audio::EventSeries *e, int sample) {
    return static_cast<float>(width()) - (static_cast<float>(e->getNewestSample()) - sample) / (e->getSamplesPerSecond() / pixelPerSecond);
}

AudioEventDataView::AudioEventDataView(QQuickItem *parent) : QQuickItem(parent) {
    setFlag(ItemHasContents);
    startTimer(15);
    for (int osf = 0; osf <= to_integral(OnsetDetectionFunction::Last); ++osf) {
        for (int dt = 0; dt <= Last; ++dt) {
            setColor(osf, static_cast<DataType>(dt), QColor(rand() % 255, rand() % 255, rand() % 255));
        }
    }
    enableDetectionFor(OnsetDetectionFunction::KullbackLiebler, OnsetEvent);
    enableDetectionFor(OnsetDetectionFunction::KullbackLiebler, OnsetValue);
}

void AudioEventDataView::enableDetectionFor(OnsetDetectionFunction f, AudioEventDataView::DataType type, bool enabled) {
    if (enabled) {
        if (type == BeatEvent) {
            if (beatData.find(f) == beatData.end()) {
                auto p = Audio::AudioCaptureManager::get().requestTempoAnalysis(f);
                if (!p) {
                    return;
                }
                beatData.emplace(f, p);
            }
        } else {
            if (onsetData.find(f) == onsetData.end()) {
                auto p = Audio::AudioCaptureManager::get().requestOnsetAnalysis(f);
                if (!p) {
                    return;
                }
                onsetData.emplace(f, p);
            }
        }
    }
    colors[to_integral(f)][type].first = enabled;
    names.clear();
    for (auto &[f, data] : beatData) {
        if (isDetectionEnabledFor(f, BeatEvent)) {
            names.push_back("Beat: " + Audio::Aubio::toQString(f));
        }
    }
    for (auto &[f, data] : onsetData) {
        if (isDetectionEnabledFor(f, OnsetEvent) || isDetectionEnabledFor(f, OnsetValue)) {
            names.push_back("Onset: " + Audio::Aubio::toQString(f));
        }
    }
}

bool AudioEventDataView::isDetectionEnabledFor(OnsetDetectionFunction onsetDetectionFunction, AudioEventDataView::DataType type) { return colors[to_integral(onsetDetectionFunction)][type].first; }

void AudioEventDataView::setColor(OnsetDetectionFunction onsetDetectionFunction, AudioEventDataView::DataType usage, const QColor &color) { colors[to_integral(onsetDetectionFunction)][usage].second = color; }

QColor AudioEventDataView::getColor(OnsetDetectionFunction onsetDetectionFunction, AudioEventDataView::DataType usage) const { return colors[to_integral(onsetDetectionFunction)][usage].second; }

void AudioEventDataView::timerEvent(QTimerEvent *e) {
    Q_UNUSED(e)
    if (visibleForUser) {
        update();
    }
}

QSGGeometryNode *createNode() {
    auto *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0);
    geometry->setLineWidth(1);

    auto gNode = new QSGGeometryNode;
    gNode->setGeometry(geometry);
    gNode->setFlag(QSGNode::OwnsGeometry);

    auto material = new QSGFlatColorMaterial;
    gNode->setMaterial(material);
    gNode->setFlag(QSGNode::OwnsMaterial);

    gNode->setFlag(QSGNode::OwnedByParent);
    return gNode;
}

QSGNode *AudioEventDataView::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *transformNode) {
    if (!node) {
        node = new QSGNode;
    }
    int currentReused = 0;
    const auto getGeometry = [&](const auto &color) {
        QSGGeometryNode *gNode;
        if (currentReused >= node->childCount()) {
            gNode = createNode();
            node->appendChildNode(gNode);
            currentReused = 9999999;
        } else {
            gNode = static_cast<QSGGeometryNode *>(node->childAtIndex(currentReused));
            gNode->markDirty(QSGNode::DirtyGeometry);
            ++currentReused;
        }
        auto m = static_cast<QSGFlatColorMaterial *>(gNode->material());
        if (m->color() != color) {
            m->setColor(color);
            gNode->markDirty(QSGNode::DirtyMaterial);
        }
        return gNode->geometry();
    };
    int eventsEnabledCount = 0;
    for (auto &[f, data] : beatData) {
        if (isDetectionEnabledFor(f, BeatEvent)) {
            ++eventsEnabledCount;
        }
    }
    for (auto &[f, data] : onsetData) {
        if (isDetectionEnabledFor(f, OnsetEvent) || isDetectionEnabledFor(f, OnsetValue)) {
            ++eventsEnabledCount;
        }
    }
    const auto sectionHeight = height() / eventsEnabledCount;
    auto sectionOffset = 0;
    const auto fillEvents = [this, &sectionOffset, sectionHeight](auto geometry, const auto &data, float confidence = 1) {
        auto events = data->getEvents();
        geometry->allocate(events->size() * 2);
        auto vertexData = geometry->vertexDataAsPoint2D();
        for (const auto &e : *events) {
            const auto x = getX(data, e);
            vertexData->x = x;
            vertexData->y = sectionOffset + sectionHeight * (1.f - confidence);
            ++vertexData;
            vertexData->x = x;
            vertexData->y = sectionOffset + sectionHeight * confidence;
            ++vertexData;
        }
        sectionOffset += sectionHeight;
        geometry->setDrawingMode(QSGGeometry::DrawLines);
    };
    for (auto &[f, data] : beatData) {
        if (isDetectionEnabledFor(f, BeatEvent)) {
            fillEvents(getGeometry(getColor(f, BeatEvent)), data.events, *data.confidence);
        }
    }
    for (auto &[f, data] : onsetData) {
        if (isDetectionEnabledFor(f, OnsetEvent)) {
            fillEvents(getGeometry(getColor(f, OnsetEvent)), data);
        } else if (isDetectionEnabledFor(f, OnsetValue)) {
            sectionOffset += sectionHeight;
        }
        if (isDetectionEnabledFor(f, OnsetValue)) {
            QSGGeometry *geometry = getGeometry(getColor(f, OnsetValue));
            const auto lockedData = data->getOnsetData();
            geometry->allocate(lockedData->size());
            auto vertexData = geometry->vertexDataAsPoint2D();
            for (const auto &o : *lockedData) {
                vertexData->x = getX(data, o.sample);
                vertexData->y = sectionOffset - ((o.onsetValue / data->getMaxOnsetValue()) * (sectionHeight));
                ++vertexData;
            }
            geometry->setDrawingMode(QSGGeometry::DrawLineStrip);
        }
        if (isDetectionEnabledFor(f, OnsetEvent)) {
            fillEvents(getGeometry(getColor(f, OnsetEvent)), data);
        }
    }
    for (auto &[f, data] : beatData) {
        if (isDetectionEnabledFor(f, BeatEvent)) {
            fillEvents(getGeometry(getColor(f, BeatEvent)), data);
        }
    }
    while (currentReused < node->childCount()) {
        auto n = node->childAtIndex(currentReused);
        node->removeChildNode(n);
        delete n;
    }
    return node;
}

} // namespace GUI
