#ifndef PROGRAMBLOCKEDITOR_H
#define PROGRAMBLOCKEDITOR_H

#include "modules/modulemanager.h"
#include "modules/programblock.h"
#include <QQuickItem>
#include <QStringListModel>

namespace GUI {

namespace detail {
    class PropertyInformation : public QObject {
        Q_OBJECT
    public:
        typedef Modules::Property::Type Type;
        Q_ENUM(Type)
        Modules::Property *property = nullptr;
        // wenn dieses Feld ungleich 0 ist, dann wird hier die InputOutputLength gespeichert
        Modules::PropertyBase *named = nullptr;
        std::function<void()> updateCallback;

    private:
        void updateValue();
        // auto generated class:
        QVariant minValue;
        QVariant maxValue;
        QVariant value;
        QString name;
        QString description;
        Type type;
        bool forwardProperty;
        QString forwardPropertyName;

        Q_PROPERTY(QVariant minValue READ getMinValue WRITE setMinValue NOTIFY minValueChanged)
        Q_PROPERTY(QVariant maxValue READ getMaxValue WRITE setMaxValue NOTIFY maxValueChanged)
        Q_PROPERTY(QVariant value READ getValue WRITE setValue NOTIFY valueChanged)
        Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
        Q_PROPERTY(Type type READ getType WRITE setType NOTIFY typeChanged)
        Q_PROPERTY(bool forwardProperty READ getForwardProperty WRITE setForwardProperty NOTIFY forwardPropertyChanged)
        Q_PROPERTY(QString forwardPropertyName READ getForwardPropertyName WRITE setForwardPropertyName NOTIFY forwardPropertyNameChanged)
    public:
        void setMinValue(const QVariant _minValue) {
            if (_minValue != minValue) {
                minValue = _minValue;
                emit minValueChanged();
            }
        }
        QVariant getMinValue() const { return minValue; }
        void setMaxValue(const QVariant _maxValue) {
            if (_maxValue != maxValue) {
                maxValue = _maxValue;
                emit maxValueChanged();
            }
        }
        QVariant getMaxValue() const { return maxValue; }
        void setValue(const QVariant _value) {
            if (_value != value) {
                value = _value;
                updateValue();
                emit valueChanged();
            }
        }
        QVariant getValue() const { return value; }
        void setName(const QString _name) {
            if (_name != name) {
                name = _name;
                emit nameChanged();
            }
        }
        QString getName() const { return name; }
        void setDescription(const QString _description) {
            if (_description != description) {
                description = _description;
                emit descriptionChanged();
            }
        }
        QString getDescription() const { return description; }
        void setType(const Type _type) {
            if (_type != type) {
                type = _type;
                emit typeChanged();
            }
        }
        Type getType() const { return type; }
        void setForwardProperty(const bool _forwardProperty) {
            if (_forwardProperty != forwardProperty) {
                forwardProperty = _forwardProperty;
                emit forwardPropertyChanged();
            }
        }
        bool getForwardProperty() const { return forwardProperty; }
        void setForwardPropertyName(const QString _forwardPropertyName) {
            if (_forwardPropertyName != forwardPropertyName) {
                forwardPropertyName = _forwardPropertyName;
                emit forwardPropertyNameChanged();
            }
        }
        QString getForwardPropertyName() const { return forwardPropertyName; }

    signals:
        void minValueChanged();
        void maxValueChanged();
        void valueChanged();
        void nameChanged();
        void descriptionChanged();
        void typeChanged();
        void forwardPropertyChanged();
        void forwardPropertyNameChanged();
    };

} // namespace detail

class PropertyInformationModel : public ModelVector<detail::PropertyInformation *> {
    Q_OBJECT
};

class ProgramBlockEditor : public QQuickItem {
    Q_OBJECT
    Modules::ProgramBlock *programBlock = nullptr;
    PropertyInformationModel propertyInformationModel;
    bool showProperties;
    QStringListModel possibleEntryModel;
    QStringListModel outputDataProducerModel;
    QStringListModel inputDataConsumerModel;
    Q_PROPERTY(Modules::ProgramBlock *programBlock READ getProgramBlock WRITE setProgramBlock NOTIFY programBlockChanged)
    Q_PROPERTY(PropertyInformationModel *propertyInformationModel READ getPropertyInformationModel CONSTANT)
    Q_PROPERTY(QAbstractListModel *possibleEntryModel READ getPossibleEntryModel CONSTANT)
    Q_PROPERTY(QAbstractListModel *outputDataProducerModel READ getOutputDataProducerModel CONSTANT)
    Q_PROPERTY(QAbstractListModel *inputDataConsumerModel READ getInputDataConsumerModel CONSTANT)
    Q_PROPERTY(bool showProperties READ getShowProperties WRITE setShowProperties NOTIFY showPropertiesChanged)
    QQmlComponent programBlockEntry;
    QQmlComponent programBlockConnection;
    enum { None, MovePermanent, MoveTemporarily, AddConnection, AddReverseConnection } dragType = None;
    QQuickItem *dragStartItem = nullptr;
    // QPoint dragStartPoint;
    QPointF dragOffsetInItem;
    QQuickItem *dragEndItem = nullptr;
    double scale = 2;
    int spaceBetweenLayers = 70;
    bool run;
    QMetaObject::Connection connection;

private:
    void recreateView();
    /**
     * @brief getItemWithPropertyBase travels through the hierarchy to find an entry component
     * @return the component or nullptr
     */
    QQuickItem *getItemWithPropertyBase(QMouseEvent *event);

public:
    /** Wird vom ModuleProgramView aufgerufen wenn ein neuer entry hinzugefügt werden soll
     */
    Q_INVOKABLE void updatePossibleEntries();
    /**
     * @brief updateInputOutputModels Wird vom ModuleProgramView aufgerufen, wenn eine neue Connection hinzugefügt werden soll.
     */
    Q_INVOKABLE void updateInputOutputModels();
    /**
     * @brief addConnection adds a connection between a inputDataConsumer and an OuputDataProducer, this function is meant to be called from qml
     * @param inputIndex The index in the inputDataConsumerModel
     * @param length the Length of the connection
     * @param outputIndex The index in the outputDataProducerModel
     * @param startIndex the start index in the output from the outputDataProducer
     */
    Q_INVOKABLE void addConnection(int inputIndex, int length, int outputIndex, int startIndex);
    /**
     * @brief addConnection you can call this method once when the signal @see askToAddConnection(QString,QString) gets emitted
     * @param length the length of the connection
     * @param startIndex the start index in the output of the target
     */
    Q_INVOKABLE void addConnection(int length, int startIndex);
    /**
     * @brief addEntry fügt den Entry an position index im possibleEntryModel hinzu
     * @param index Der index im possibleEntryModel
     * @param size Die inputoutput größe des entries
     */
    Q_INVOKABLE void addEntry(int index, int size);
    /**
     * @brief removeEntry removes the entry selected by mouse
     */
    Q_INVOKABLE void removeEntry();
    /**
     * @brief removeIncomingConnections removes all incoming connections from the current entry
     */
    Q_INVOKABLE void removeIncomingConnections();
    static QQmlEngine *engine;
    ProgramBlockEditor();
    ~ProgramBlockEditor() override { QObject::disconnect(connection); }
    void setProgramBlock(Modules::ProgramBlock *_programBlock) {
        if (_programBlock != programBlock) {
            if (programBlock) {
                QObject::disconnect(programBlock, &Modules::ProgramBlock::propertyBaseChanged, this, &ProgramBlockEditor::propertyBaseChanged);
            }
            programBlock = _programBlock;
            recreateView();
            setShowProperties(false);
            emit programBlockChanged();
            QObject::disconnect(connection);
            if (programBlock) {
                connection = QObject::connect(programBlock, &QObject::destroyed, [this]() { setProgramBlock(nullptr); });
                QObject::connect(programBlock, &Modules::ProgramBlock::propertyBaseChanged, this, &ProgramBlockEditor::propertyBaseChanged);
            }
        }
    }
    Modules::ProgramBlock *getProgramBlock() const { return programBlock; }
    PropertyInformationModel *getPropertyInformationModel() { return &propertyInformationModel; }
    QAbstractListModel *getPossibleEntryModel() { return &possibleEntryModel; }
    QAbstractListModel *getInputDataConsumerModel() { return &inputDataConsumerModel; }
    QAbstractListModel *getOutputDataProducerModel() { return &outputDataProducerModel; }

    void setShowProperties(const bool _showProperties) {
        // if(_showProperties != showProperties){
        showProperties = _showProperties;
        emit showPropertiesChanged();
        //}
    }
    bool getShowProperties() const { return showProperties; }

protected:
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
protected slots:
    void propertyBaseChanged(Modules::PropertyBase *oldPB, Modules::PropertyBase *newPB);
signals:
    void programBlockChanged();
    void showPropertiesChanged();
    void runChanged();
    void askToAddConnection(QString from, QString to);
    void openRightClickEntry(int x, int y);
};

} // namespace GUI

#endif // PROGRAMBLOCKEDITOR_H
