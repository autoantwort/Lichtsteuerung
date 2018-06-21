#include "codeeditorhelper.h"
#include <QTextCursor>
#include <QtQuick>

CodeHighlighter::CodeHighlighter(QTextDocument * parent):QSyntaxHighlighter (parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bbool\\b"
                    << "\\bauto\\b" << "\\bfor\\b"  << "\\bwhile\\b"  ;
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}

CodeEditorHelper::CodeEditorHelper(){

}

int CodeEditorHelper::countTabs(int startPos){
    int counter = 0;
    while (startPos>=0 && document->characterAt(startPos) != QChar::ParagraphSeparator) {
        if(document->characterAt(startPos) == QChar::Tabulation)
            ++counter;
        --startPos;
    }
    return counter;
}

void CodeEditorHelper::contentsChange(int from, int charsRemoved, int charsAdded){
    qDebug() << "pos " << from << " removed : " << charsRemoved << " added : " << charsAdded;
    if(charsAdded == 1 && document->characterAt(from) == QChar::ParagraphSeparator){
        qDebug() << "Enter pressed :  " << document->characterAt(from-1);
        if(document->characterAt(from-1) == '{'){
            qDebug()<<"write";
            int tabs = countTabs(from-1);
            QString newText;
            for(int i = 0 ; i<= tabs;++i)
                newText  += (QString(QChar::Tabulation));
            newText += '\n';
            for(int i = 0 ; i< tabs;++i)
                newText += (QString(QChar::Tabulation));
            newText += '}';
            emit insertText(newText,from + tabs+ 2);
        }else{
            int tabs = countTabs(from-1);
            QString newText;
            qDebug()<<"write2 : "<<tabs;
            for(int i = 0 ; i< tabs;++i)
                newText += (QString(QChar::Tabulation));
            emit insertText(newText,from + tabs +1);
        }
    }
    if(charsAdded==1 && document->characterAt(from).isSpace() &&
            document->characterAt(from-1) == 'r' &&
            document->characterAt(from-2) == 'o' &&
            document->characterAt(from-3) == 'f'){
        QString variableName = "i";
        int tabs = countTabs(from-1);
        QString newText = "(int " + variableName + " = 0; "+variableName+" < 10; ++"+variableName + "){\n";
        for(int i = 0 ; i<= tabs;++i)
            newText  += (QString(QChar::Tabulation));
        auto pos = newText.length();
        newText += '\n';
        for(int i = 0 ; i< tabs;++i)
            newText += (QString(QChar::Tabulation));
        newText += '}';
        emit insertText(newText,from + pos + 1);
    }
    /*auto d = qDebug();
    for(int i = 0 ; i < document->characterCount() ; ++i){
        d << document->characterAt(i) << document->characterAt(i).combiningClass();
    }*/
}


void CodeHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
