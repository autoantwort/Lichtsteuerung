#ifndef CODEEDITORHELPER_H
#define CODEEDITORHELPER_H

#include <QQuickTextDocument>
#include <QtDebug>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

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
    Q_PROPERTY(QQuickTextDocument* document READ getDocument WRITE setDocument NOTIFY documentChanged)
    Q_OBJECT
    /** Wird automatisch gelöscht, wenn das textdocument zerstört wird
     * @brief highlighter
     */
    CodeHighlighter * highlighter = nullptr;

protected:
    int countTabs(int startPos);

public:
    CodeEditorHelper();
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
   void documentChanged();
   void insertText(QString newText, int pos);
   protected:
   void contentsChange(int from, int charsRemoved, int charsAdded);

};

#endif // CODEEDITORHELPER_H
