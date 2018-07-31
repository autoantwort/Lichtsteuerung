#ifndef CODEEDITORHELPER_H
#define CODEEDITORHELPER_H

#include <QQuickTextDocument>
#include <QtDebug>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <programms/modulemanager.h>

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

class CodeEditorHelper : public QObject
{
    QQuickTextDocument* documentWrapper = nullptr;
    QTextDocument * document = nullptr;
    QMetaObject::Connection typeConnection;
    Q_PROPERTY(QQuickTextDocument* document READ getDocument WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(Modules::Module* module READ getModule WRITE setModule NOTIFY moduleChanged)
    Q_OBJECT
    /** Wird automatisch gelöscht, wenn das textdocument zerstört wird
     * @brief highlighter
     */
    CodeHighlighter * highlighter = nullptr;
    Modules::Module * module = nullptr;

protected:
    int countTabs(int startPos);

public:

    void setModule(  Modules::Module* _module){
            if(_module != module){
                    module = _module;
                    QObject::disconnect(typeConnection);
                    if(module)
                        typeConnection = QObject::connect(module,&Modules::Module::typeChanged,this,&CodeEditorHelper::typeChanged);
                    emit moduleChanged();
            }
    }
    Modules::Module *getModule() const {
            return module;
    }

    CodeEditorHelper();
    Q_INVOKABLE void compile();

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
   void insertText(QString newText, int pos);
   protected:
   void typeChanged();
   void contentsChange(int from, int charsRemoved, int charsAdded);

};

#endif // CODEEDITORHELPER_H
