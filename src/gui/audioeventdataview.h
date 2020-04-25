#ifndef AUDIOEVENTDATAVIEW_H
#define AUDIOEVENTDATAVIEW_H

#include "modelvector.h"
#include "audio/audiocapturemanager.h"
#include <QQuickItem>
#include <array>
#include <chrono>
#include <map>

namespace GUI {

class AudioEventDataView : public QQuickItem {
    Q_OBJECT
    std::map<enum Audio::Aubio::OnsetDetectionFunction, const Audio::OnsetDataSeries *> onsetData;
    std::map<enum Audio::Aubio::OnsetDetectionFunction, Audio::TempoAnalysisData> beatData;
    Q_PROPERTY(bool visibleForUser MEMBER visibleForUser NOTIFY visibleForUserChanged)
    Q_PROPERTY(int pixelPerSecond MEMBER pixelPerSecond NOTIFY pixelPerSecondChanged)
    Q_PROPERTY(QAbstractItemModel *names READ getNames CONSTANT)
    Q_PROPERTY(QColor ownColor READ getOwnColor WRITE setOwnColor NOTIFY ownColorChanged)
    int pixelPerSecond = 100;
    bool visibleForUser = true;
    float getX(const Audio::EventSeries *e, int sample);
    ModelVector<QString> names;
    QColor ownColor;
    Audio::EventSeries ownEvents;
    std::chrono::steady_clock::time_point start;

public:
    enum DataType { BeatEvent, OnsetEvent, OnsetValue, Last = OnsetValue };
    Q_ENUM(DataType)

private:
    std::array<std::array<std::pair<bool, QColor>, DataType::Last + 1>, static_cast<int>(Audio::Aubio::OnsetDetectionFunction::Last) + 1> colors;

public:
    explicit AudioEventDataView(QQuickItem *parent = nullptr);

    Q_INVOKABLE int getNumberOfOnsetDetectionFunctions() const { return static_cast<int>(Audio::Aubio::OnsetDetectionFunction::Last) + 1; }
    Q_INVOKABLE QString getNameOfOnsetDetectionFunctions(int f) const { return Audio::Aubio::toQString(Audio::Aubio::toOnsetDetectionFunction(f)); }

    Q_INVOKABLE void enableDetectionFor(int onsetDetectionFunction, DataType type, bool enabled = true) { enableDetectionFor(Audio::Aubio::toOnsetDetectionFunction(onsetDetectionFunction), type, enabled); }
    void enableDetectionFor(Audio::Aubio::OnsetDetectionFunction onsetDetectionFunction, DataType type, bool enabled = true);

    Q_INVOKABLE bool isDetectionEnabledFor(int onsetDetectionFunction, DataType type) { return isDetectionEnabledFor(Audio::Aubio::toOnsetDetectionFunction(onsetDetectionFunction), type); }
    bool isDetectionEnabledFor(Audio::Aubio::OnsetDetectionFunction onsetDetectionFunction, DataType type);

    Q_INVOKABLE void setColor(int onsetDetectionFunction, DataType usage, const QColor &color) { setColor(Audio::Aubio::toOnsetDetectionFunction(onsetDetectionFunction), usage, color); }
    void setColor(Audio::Aubio::OnsetDetectionFunction onsetDetectionFunction, DataType usage, const QColor &color);

    Q_INVOKABLE QColor getColor(int onsetDetectionFunction, DataType usage) const { return getColor(Audio::Aubio::toOnsetDetectionFunction(onsetDetectionFunction), usage); }
    [[nodiscard]] QColor getColor(Audio::Aubio::OnsetDetectionFunction onsetDetectionFunction, DataType usage) const;

    Q_INVOKABLE void ownTick();

    QAbstractItemModel *getNames() { return &names; }

    void setOwnColor(QColor ownColor);
    QColor getOwnColor() const { return ownColor; }

signals:
    void visibleForUserChanged();
    void pixelPerSecondChanged();
    void ownColorChanged(QColor ownColor);

protected:
    void timerEvent(QTimerEvent *e) override;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *transformNode) override;
};
} // namespace GUI

#endif // AUDIOEVENTDATAVIEW_H
