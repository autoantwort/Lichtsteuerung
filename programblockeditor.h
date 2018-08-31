#ifndef PROGRAMBLOCKEDITOR_H
#define PROGRAMBLOCKEDITOR_H

#include <QQuickItem>
#include "programms/programblock.h"
#include "programms/modulemanager.h"

namespace detail {
    class PropertyInformation : public QObject{
        Q_OBJECT
    public:
        typedef Modules::Property::Type Type;
        Q_ENUM(Type)
    private:
        //auto generated class:
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
        void setMinValue( const QVariant _minValue){
                if(_minValue != minValue){
                        minValue = _minValue;
                        emit minValueChanged();
                }
        }
        QVariant getMinValue() const {
                return minValue;
        }
        void setMaxValue( const QVariant _maxValue){
                if(_maxValue != maxValue){
                        maxValue = _maxValue;
                        emit maxValueChanged();
                }
        }
        QVariant getMaxValue() const {
                return maxValue;
        }
        void setValue( const QVariant _value){
                if(_value != value){
                        value = _value;
                        emit valueChanged();
                }
        }
        QVariant getValue() const {
                return value;
        }
        void setName( const QString _name){
                if(_name != name){
                        name = _name;
                        emit nameChanged();
                }
        }
        QString getName() const {
                return name;
        }
        void setDescription( const QString _description){
                if(_description != description){
                        description = _description;
                        emit descriptionChanged();
                }
        }
        QString getDescription() const {
                return description;
        }
        void setType( const Type _type){
                if(_type != type){
                        type = _type;
                        emit typeChanged();
                }
        }
        Type getType() const {
                return type;
        }
        void setForwardProperty( const bool _forwardProperty){
                if(_forwardProperty != forwardProperty){
                        forwardProperty = _forwardProperty;
                        emit forwardPropertyChanged();
                }
        }
        bool getForwardProperty() const {
                return forwardProperty;
        }
        void setForwardPropertyName( const QString _forwardPropertyName){
                if(_forwardPropertyName != forwardPropertyName){
                        forwardPropertyName = _forwardPropertyName;
                        emit forwardPropertyNameChanged();
                }
        }
        QString getForwardPropertyName() const {
                return forwardPropertyName;
        }

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


}

class PropertyInformationModel : public ModelVector<detail::PropertyInformation*>{
    Q_OBJECT
};

class ProgramBlockEditor : public QQuickItem
{
    Q_OBJECT
    Modules::ProgramBlock * programBlock;
    PropertyInformationModel propertyInformationModel;
    bool showProperties;
    Q_PROPERTY(Modules::ProgramBlock* programBlock READ getProgramBlock WRITE setProgramBlock NOTIFY programBlockChanged)
    Q_PROPERTY(PropertyInformationModel * propertyInformationModel READ getPropertyInformationModel CONSTANT)
    Q_PROPERTY(bool showProperties READ getShowProperties WRITE setShowProperties NOTIFY showPropertiesChanged)
    QQmlComponent programBlockEntry;
    QQmlComponent programBlockConnection;
private:
    void recreateView();
public:
    static QQmlEngine * engine;
    ProgramBlockEditor();void setProgramBlock( Modules::ProgramBlock* _programBlock){
        if(_programBlock != programBlock){
            qDebug () << "Program Block changed";
                programBlock = _programBlock;
                recreateView();
                emit programBlockChanged();
        }
    }
    Modules::ProgramBlock* getProgramBlock() const {
        return programBlock;
    }
    PropertyInformationModel *getPropertyInformationModel() {
            return &propertyInformationModel;
    }
    void setShowProperties( const bool _showProperties){
            if(_showProperties != showProperties){
                    showProperties = _showProperties;
                    emit showPropertiesChanged();
            }
    }
    bool getShowProperties() const {
            return showProperties;
    }
protected:
    virtual void mouseReleaseEvent(QMouseEvent *event)override;
    virtual void mouseMoveEvent(QMouseEvent *event)override;
    virtual void mousePressEvent(QMouseEvent *event)override;
signals:
    void programBlockChanged();
    void showPropertiesChanged();
};

#endif // PROGRAMBLOCKEDITOR_H
