#ifndef PROGRAMBLOCKEDITOR_H
#define PROGRAMBLOCKEDITOR_H

#include <QQuickItem>
#include "programms/programblock.h"

class ProgramBlockEditor : public QQuickItem
{
    Q_OBJECT
    Modules::ProgramBlock * programBlock;
    Q_PROPERTY(Modules::ProgramBlock* programBlock READ getProgramBlock WRITE setProgramBlock NOTIFY programBlockChanged)
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
signals:
    void programBlockChanged();
};

#endif // PROGRAMBLOCKEDITOR_H
