#ifndef AUDIOEVENTDATAVIEW_H
#define AUDIOEVENTDATAVIEW_H

#include "audio/audiocapturemanager.h"
#include <QQuickItem>
#include <array>
#include <map>

namespace GUI {

class AudioEventDataView : public QQuickItem {
    Q_OBJECT
    std::map<enum Audio::Aubio::OnsetDetectionFunction, const Audio::OnsetDataSeries *> onsetData;
    std::map<enum Audio::Aubio::OnsetDetectionFunction, const Audio::EventSeries *> beatData;
    Q_PROPERTY(bool visibleForUser MEMBER visibleForUser NOTIFY visibleForUserChanged)
    Q_PROPERTY(int pixelPerSecond MEMBER pixelPerSecond NOTIFY pixelPerSecondChanged)
    int pixelPerSecond = 100;
    bool visibleForUser = true;
    float getX(const Audio::EventSeries *e, int sample);

public:
    enum DataType { BeatEvent, OnsetEvent, OnsetValue, ThresholdValue, Last = ThresholdValue };
    Q_ENUM(DataType)

private:
    std::array<std::array<std::pair<bool, QColor>, DataType::Last>, static_cast<int>(Audio::Aubio::OnsetDetectionFunction::Last)> colors;

public:
    explicit AudioEventDataView(QQuickItem *parent = nullptr);

    Q_INVOKABLE int getNumberOfOnsetDetectionFunctions() const { return static_cast<int>(Audio::Aubio::OnsetDetectionFunction::Last); }
    Q_INVOKABLE QString getNameOfOnsetDetectionFunctions(int f) const { return Audio::Aubio::toQString(Audio::Aubio::toOnsetDetectionFunction(f)); }

    Q_INVOKABLE void enableDetectionFor(int onsetDetectionFunction, DataType type, bool enabled = true) { enableDetectionFor(Audio::Aubio::toOnsetDetectionFunction(onsetDetectionFunction), type, enabled); }
    void enableDetectionFor(Audio::Aubio::OnsetDetectionFunction onsetDetectionFunction, DataType type, bool enabled = true);

    Q_INVOKABLE bool isDetectionEnabledFor(int onsetDetectionFunction, DataType type) { return isDetectionEnabledFor(Audio::Aubio::toOnsetDetectionFunction(onsetDetectionFunction), type); }
    bool isDetectionEnabledFor(Audio::Aubio::OnsetDetectionFunction onsetDetectionFunction, DataType type);

    Q_INVOKABLE void setColor(int onsetDetectionFunction, DataType usage, const QColor &color) { setColor(Audio::Aubio::toOnsetDetectionFunction(onsetDetectionFunction), usage, color); }
    void setColor(Audio::Aubio::OnsetDetectionFunction onsetDetectionFunction, DataType usage, const QColor &color);

    Q_INVOKABLE QColor getColor(int onsetDetectionFunction, DataType usage) const { return getColor(Audio::Aubio::toOnsetDetectionFunction(onsetDetectionFunction), usage); }
    [[nodiscard]] QColor getColor(Audio::Aubio::OnsetDetectionFunction onsetDetectionFunction, DataType usage) const;

signals:
    void visibleForUserChanged();
    void pixelPerSecondChanged();

protected:
    void timerEvent(QTimerEvent *e) override;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *transformNode) override;
};
} // namespace GUI

#endif // AUDIOEVENTDATAVIEW_H
