#ifndef ERRORNOTIFIER_H
#define ERRORNOTIFIER_H

#include <QObject>
/**
 * @brief The ErrorNotifier class is a wrapper to qml to show the lastest error
 */
class ErrorNotifier : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString errorMessage MEMBER errorMessage NOTIFY errorMessageChanged)
    /**
     * @brief errorMessage the last error Message
     */
    QString errorMessage;
private:
    explicit ErrorNotifier(QObject *parent = nullptr);
public:
    /**
     * @brief get return the singletone
     * @return a pointer to the singletone notifier instance
     */
    static ErrorNotifier * get(){static ErrorNotifier e;return &e;}
    /**
     * @brief newError notify qml to show the error
     * @param errorMessage The Message to show
     */
    void newError(const QString &errorMessage){
        this->errorMessage += errorMessage;
        if(!this->errorMessage.endsWith(".")){
            this->errorMessage += ".";
        }
        this->errorMessage += "\n";
        emit errorMessageChanged();
    }
    static void showError(const QString &errorMessage){
        get()->newError(errorMessage);
    }
signals:
    void errorMessageChanged();
public slots:
};

#endif // ERRORNOTIFIER_H
