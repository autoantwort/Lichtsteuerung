#ifndef CODEEDITORHELPER_H
#define CODEEDITORHELPER_H

#include <QQuickTextDocument>
#include <QtDebug>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include "modules/modulemanager.h"
#include <QSortFilterProxyModel>

// Highlight code from https://doc.qt.io/qt-5/qtwidgets-richtext-syntaxhighlighter-example.html
class CodeHighlighter : public QSyntaxHighlighter{
private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
protected:
    void highlightBlock(const QString &text) override;
public:
    CodeHighlighter(QTextDocument * doc);
    CodeHighlighter(const CodeHighlighter &) = delete ;
};

class CodeCompletionEntry : public QObject{
    Q_OBJECT
public:
    const QString type;
    const QString description;
    const QString completion;
    const bool closeAfterCompletion;
    Q_PROPERTY(QString type MEMBER type CONSTANT)
    Q_PROPERTY(QString description MEMBER description CONSTANT)
    Q_PROPERTY(QString completion MEMBER completion CONSTANT)
    Q_PROPERTY(bool closeAfterCompletion MEMBER closeAfterCompletion CONSTANT)
public:
    CodeCompletionEntry(QString completion,QString type,QString description,bool closeAfterCompletion = true):type(type.trimmed()),description(description),completion(completion.trimmed()),closeAfterCompletion(closeAfterCompletion){

    }
};


class PossibleCodeCompletions : public ModelVector<CodeCompletionEntry*>{
  Q_OBJECT
};

class CodeCompletions : public QSortFilterProxyModel{
    Q_OBJECT
public:
    PossibleCodeCompletions model;
    CodeCompletions(){
        setSourceModel(&model);
    }
protected:
    virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right)const override;
    virtual bool filterAcceptsRow(int sourceRow,const QModelIndex &sourceParent) const override;
};

class CodeEditorHelper : public QObject
{
    QQuickTextDocument* documentWrapper = nullptr;
    QTextDocument * document = nullptr;
    QMetaObject::Connection typeConnection;
    QMetaObject::Connection spotifyResponderConnection;
    CodeCompletions codeCompletions;

    Q_PROPERTY(QQuickTextDocument* document READ getDocument WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(Modules::Module* module READ getModule WRITE setModule NOTIFY moduleChanged)
    Q_PROPERTY(QAbstractItemModel * codeCompletions READ getCodeCompletions CONSTANT)
    Q_OBJECT
    /** Wird automatisch gelöscht, wenn das textdocument zerstört wird
     * @brief highlighter
     */
    CodeHighlighter * highlighter = nullptr;
    Modules::Module * module = nullptr;

protected:
    int countTabs(int startPos);
    QString getType(QString variable, int pos);
public:
    QAbstractItemModel * getCodeCompletions(){
        return &codeCompletions;
    }

    void setModule(  Modules::Module* _module);
    Modules::Module *getModule() const {
            return module;
    }

    CodeEditorHelper();
    Q_INVOKABLE void compile();
    Q_INVOKABLE void updateCodeCompletionModel(int cursorPosition);

    void setDocument(  QQuickTextDocument* _document){
       if(_document != documentWrapper){
             documentWrapper = _document;
             emit documentChanged();
             qDebug() << "add connection";
             if(documentWrapper){
                QObject::connect(documentWrapper->textDocument(),&QTextDocument::contentsChange,this,&CodeEditorHelper::contentsChange);
                document = documentWrapper->textDocument();
             }else
                document = nullptr;
             if(document && !highlighter){
                 highlighter = new CodeHighlighter(document);
             }else if(document){
                 highlighter->setDocument(document);
             }
       }
    }
   QQuickTextDocument* getDocument() const {
        return documentWrapper;
   }
signals:
   void information(QString text);
   void moduleChanged();
   void documentChanged();
   /**
    * @brief insertText send a signal to the qml editor component to inform the component, that text should be inserted at the current cursor pos
    * @param newText the text that should be inserted
    * @param pos the new cursor after text inserting.
    */
   void insertText(QString newText, int pos);
protected:
   void typeChanged();
   void spotifyResponderChanged();
   void contentsChange(int from, int charsRemoved, int charsAdded);

};

#endif // CODEEDITORHELPER_H