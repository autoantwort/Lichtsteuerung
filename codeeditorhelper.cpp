#include "codeeditorhelper.h"
#include <QTextCursor>
#include <QtQuick>
#include "settings.h"
#include "programms/compiler.h"

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
                    << "\\bauto\\b" << "\\bfor\\b"  << "\\bwhile\\b"  << "\\breturn\\b"  ;
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

QString generateProgrammCode(){
    QString code = "int getProgrammLengthInMS(){\n\treturn Program::Infinite;\n}\n\n";
    code += "void start(){\n\t\n}\n\n";
    code += "ProgramState doStep(time_diff_t diff_ms){\n\treturn {false/*finished*/,true/*output changed*/};\n}\n\n";
    return code;
}

QString generateFilterCode(){
    QString code = "unsigned int computeOutputLength(unsigned int inputLength){\n\treturn inputLength;\n}\n\n";
    code += "void filter(){\n\t\n}\n\n";
    code += "bool doStep(time_diff_t diff_ms){\n\treturn false/*output changed*/;\n}\n\n";
    return code;
}

void CodeEditorHelper::typeChanged(){
    if(module->getType()==Modules::Module::Program){
        emit insertText(generateProgrammCode(),0);
    }else if(module->getType()==Modules::Module::Filter){
        emit insertText(generateFilterCode(),0);
    }
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
            if(tabs==0)
                return;
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

QString toName(Modules::Module::Type t){
    switch (t) {
        case Modules::Module::Filter:
        return "Filter";
        case Modules::Module::Program:
        return "Program";
        case Modules::Module::LoopProgram:
        return "LoopProgram";
        default:
            return "wrong_type";
    }
}
QString toName(Modules::ValueType t){
    switch (t) {
        case Modules::Brightness:
        return "brightness_t";
        case Modules::RGB:
        return "rgb_t";
        default:
            return "wrong_type";
    }
}
QString toName(Modules::Property::Type t){
    switch (t) {
    case Modules::Property::Bool:
    return "bool";
    case Modules::Property::Double:
    return "double";
    case Modules::Property::Float:
    return "float";
    case Modules::Property::Int:
    return "int";
    case Modules::Property::Long:
    return "long";
    case Modules::Property::String:
    return "std::string";
        default:
            return "wrong_type";
    }
}


QTextStream& writeDeclaration(QTextStream& out, const Modules::detail::PropertyInformation *p){
    using namespace Modules;
    if(p->getType() == Property::Bool){
        out << "BoolProperty _"<< p->getName()<< ';' << endl;
    }else if(p->getType() == Property::String){
        out << "StringProperty _"<< p->getName()<< " = \"\";" << endl;
    }else{
        out << "NumericProperty<"<< toName(p->getType())<<"> _"<<p->getName()<<";"<<endl;
    }
    return out;
}
QTextStream& writeConstructor(QTextStream& out, const Modules::detail::PropertyInformation *p){
    using namespace Modules;
    out << "properties.push_back(& _"<< p->getName()<< ");" << endl;
    out << "_" << p->getName() << ".setName(\""<< p->getName() << "\");" << endl;
    out << "_" << p->getName() << ".setDescription(\""<< p->getDescription() << "\");" << endl;
    return out;
}

std::vector<int> findPropertyInsertionPoints(const QString &userCode){
    std::vector<int> pos;
    int lastIndex = 0;
    while (lastIndex>=0) {
        int index = userCode.indexOf("filter",lastIndex);
        if(index<0)
            index = userCode.indexOf("doStep",lastIndex);
        if(index>=0){
            lastIndex = index = userCode.indexOf("{",index) + 1;
            pos.push_back(lastIndex);
        }else{
            lastIndex = -1;
        }
    }
    return pos;
}

void writeLocalPropertiesAssigments(QTextStream& out, const Modules::PropertiesVector & vec){
    for(const auto p : vec){
        switch (p->getType()) {
        case Modules::Property::Int:
        case Modules::Property::Double:
        case Modules::Property::Float:
        case Modules::Property::Long:
            out << "\tauto " << p->getName() << " = _" << p->getName() << ".asNumeric<" << toName(p->getType()) << ">()->getValue();\n";
            break;
        case Modules::Property::Bool:
            out << "\tauto " << p->getName() << " = _" << p->getName() << ".asBool()->getValue();\n";
            break;
        case Modules::Property::String:
            out << "\tauto " << p->getName() << " = _" << p->getName() << ".asString()->getString();\n";
            break;
        }
    }
}

QTextStream& writeUserCode(QTextStream& out, QString userCode, const Modules::PropertiesVector & vec){
    using namespace Modules;
    auto positions = findPropertyInsertionPoints(userCode);
    if(positions.size()==0){
        out << userCode;
    }else if(positions.size()==1){
        out << userCode.leftRef(positions[0]) << '\n';
        writeLocalPropertiesAssigments(out,vec);
        out << userCode.rightRef(userCode.length() - positions[0]);
    }else{
        positions.insert(positions.begin(),0);
        for(unsigned int i = 1 ; i < positions.size(); ++i){
            out << userCode.midRef(positions[i-1],positions[i]-positions[i-1]) << '\n';
            writeLocalPropertiesAssigments(out,vec);
        }
        out << userCode.mid(positions.back());
    }
    return out;
}
QString getPropertiesNumericContructors(const Modules::PropertiesVector & vec){
    QString s = ":";
    for(const auto p : vec){
        switch (p->getType()) {
        case Modules::Property::Int:
        case Modules::Property::Double:
        case Modules::Property::Float:
        case Modules::Property::Long:
            s += "_" + p->getName() + "("+ QString::number(p->getMinValue())+","+QString::number(p->getMaxValue())+","+QString::number(p->getDefaultValue()) +"),";
            break;
        case Modules::Property::Bool:
            s += "_" + p->getName() + "("+ QString::number(p->getDefaultValue()) +"),";
        }
    }
    if(s.length()==1)
        return "";
    return s.remove(s.length()-1,1);
}

void CodeEditorHelper::compile(){
    if(!module)
        return;
    for(const auto p1 : module->getProperties()){
        for(const auto p2 : module->getProperties()){
            if(p1 != p2 && p1->getName() == p2->getName()){
                emit information("Es gibt mehrere Properties mit dem Namen : " + p1->getName() +"\nBreche Compilieren ab.");
                return;
            }
        }
    }

    Settings s;
    QFile file( s.getModuleDirPath() + "/" + module->getName() + ".cpp" );
    file.remove();
    if ( file.open(QIODevice::ReadWrite) )
    {
        QString typeName = toName(module->getType());
        QString valueName = toName(module->getValueType());
        QTextStream stream( &file );
        stream << "#define MODULE_LIBRARY" << endl;
        switch (module->getType()) {
            case Modules::Module::Filter:
                stream << "#define HAVE_FILTER" << endl;
                break;
            case Modules::Module::Program:
                stream << "#define HAVE_PROGRAM" << endl;
                break;
            case Modules::Module::LoopProgram:
                stream << "#define HAVE_LOOP_PROGRAM" << endl;
                break;
            default:
                return;
        }
        stream << "#include <programms/module.h>" << endl;
        stream << "#define _USE_MATH_DEFINES"<<endl;
        stream << "#include <cmath>"<<endl;
        stream << "" << endl;
        stream << "using namespace Modules;" << endl;
        stream << "using namespace std;" << endl;
        stream << "" << endl;
        if(module->getType() == Modules::Module::Filter){
            stream << "class Impl : public Typed" << typeName << "<"<<valueName<<","<<valueName<<">{"<< endl;
        }else{
            stream << "class Impl : public Typed" << typeName << "<"<<valueName<<">{"<< endl;
        }
        for(const auto &p : module->getProperties()){
            writeDeclaration(stream,p);
        }
        stream << "public:" << endl;
        stream << "Impl()" << getPropertiesNumericContructors(module->getProperties()) <<"{" << endl;
        for(const auto &p : module->getProperties()){
            writeConstructor(stream,p);
        }
        stream << "}" << endl;
        stream << "const char* getName()const final override{" << endl;
        stream <<   "return \"" << module->getName()<< "\";" << endl;
        stream << "}" << endl;
        stream << "" << endl;
        writeUserCode(stream,document->toPlainText(),module->getProperties()); // write Code from document
        stream << "" << endl;
        stream << "};" << endl; // class end
        stream << "" << endl;
        stream << "" << endl;
        stream << "unsigned int getNumberOf"<<typeName<<"s(){" << endl;
        stream << " return 1;" << endl;
        stream << "}" << endl;
        stream << "" << endl;
        stream << "char const * getNameOf"<<typeName<<"(unsigned int index){" << endl;
        stream <<   "switch (index) {" << endl;
        stream <<       "case 0:" << endl;
        stream <<           "return \""<<module->getName()<<"\";" << endl;
        stream <<       "default:" << endl;
        stream <<           "return \"Wrong index\";" << endl;
        stream <<   "}" << endl;
        stream << "}" << endl;
        stream << "" << endl;
        stream << "char const * getDescriptionOf"<<typeName<<"(unsigned int index){" << endl;
        stream <<   "switch (index) {" << endl;
        stream <<       "case 0:" << endl;
        stream <<           "return \""<<module->getDescription()<<"\";" << endl;
        stream <<       "default:" << endl;
        stream <<           "return \"Wrong index\";" << endl;
        stream <<   "}" << endl;
        stream << "}" << endl;
        stream << "" << endl;
        stream << typeName << " * create"<< typeName <<"(unsigned int index){ " << endl;
        stream << "  switch (index) {" << endl;
        stream << "    case 0:" << endl;
        stream << "      return new Impl;" << endl;
        stream << "    default:" << endl;
        stream << "      return nullptr;" << endl;
        stream << "  }" << endl;
        stream << "}" << endl;
        stream.flush();
        file.close();
        auto result = Modules::Compiler::compileToLibrary(file,module->getName()+".dll");
        if(result.first){
            QFileInfo finfo = file;
            emit information(result.second.replace(finfo.absoluteFilePath(),""));
        }else
            emit information("Compilieren erfolgreich.");
    }
}
