#include "codeeditorhelper.h"
#include <QTextCursor>
#include <QtQuick>
#include "settings.h"
#include "programms/compiler.h"

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

bool CodeCompletions::lessThan(const QModelIndex &left, const QModelIndex &right)const{
    CodeCompletionEntry* leftData = sourceModel()->data(left).value<CodeCompletionEntry*>();
    CodeCompletionEntry* rightData = sourceModel()->data(right).value<CodeCompletionEntry*>();
    return leftData->completion < rightData->completion;
}

bool CodeCompletions::filterAcceptsRow(int sourceRow,
          const QModelIndex &sourceParent) const
  {
      QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
      CodeCompletionEntry* data = sourceModel()->data(index).value<CodeCompletionEntry*>();
      return data->completion.contains(this->filterRegExp());
  }


void addBasicTypes(PossibleCodeCompletions & model){
    model.push_back(new CodeCompletionEntry("int","int","Ganze Zahlen von -2^31 bis 2^31-1"));
    model.push_back(new CodeCompletionEntry("unsigned int","unsigned int","Positive ganze Zahlen von 0 bis 2^32-1"));
    model.push_back(new CodeCompletionEntry("double","double","Fließkommazahlen mit 64 Bit genauigkeit"));
    model.push_back(new CodeCompletionEntry("float","float","Fließkommazahlen mit 32 Bit genauigkeit"));
    model.push_back(new CodeCompletionEntry("bool","bool","Ein Boolischer Wert, der entweder 1 oder 0 ist"));
    model.push_back(new CodeCompletionEntry("brightness_t","unsigned char","Ein Typ der Helligkeit representiert. Kann Werte von 0 bis 255 annehmen"));
    model.push_back(new CodeCompletionEntry("rgb_t","struct","Eine Klasse mit den Eigenschaften red/r, green/g, blue/b die jeweils Werte von 0 bis 255 annehmen können. Alternativ kann über den Indexoperator auf die Farben zugegriffen werden, z.B. auf Grün mit var_name[1]"));
}

void addArrayTypes(PossibleCodeCompletions & model, Modules::Module * m){
    bool haveInput = m->getType() == Modules::Module::Filter;
    bool haveOutput = true;
    QString inputType = toName(m->getInputType());
    QString outputType = toName(m->getOutputType());

    if(haveInput){
        model.push_back(new CodeCompletionEntry("BoundedArray<"+inputType+"> in(input, inputLength);","BoundedArray<"+inputType+">",""));
    }
    if(haveOutput){
        model.push_back(new CodeCompletionEntry("BoundedArray<"+outputType+"> out(input, outputLength);","BoundedArray<"+inputType+">",""));
    }

    if(haveInput){
        model.push_back(new CodeCompletionEntry("WrappedArray<"+inputType+"> in(input, inputLength);","WrappedArray<"+inputType+">",""));
    }
    if(haveOutput){
        model.push_back(new CodeCompletionEntry("WrappedArray<"+outputType+"> out(input, outputLength);","WrappedArray<"+inputType+">",""));
    }

    if(haveInput){
        model.push_back(new CodeCompletionEntry("CheckedArray<"+inputType+"> in(input, inputLength);","CheckedArray<"+inputType+">",""));
    }
    if(haveOutput){
        model.push_back(new CodeCompletionEntry("CheckedArray<"+outputType+"> out(input, outputLength);","CheckedArray<"+inputType+">",""));
    }
}

void addAudioTypes(PossibleCodeCompletions & model){
    model.push_back(new CodeCompletionEntry("supportAudio()","bool","Diese Funktion gibt an, ob auf diesem System Audioanalyse unterstützt wird"));
    model.push_back(new CodeCompletionEntry("fftOutput.","const Modules::FFTOutputView<float>&","Über fftOutput kann man auf die Analyse des AudioOuputs zugreifen. Diese besteht darin, das Audiosignal in einzele Frequenzbereiche einzuteilen.",false));
}

void addForLoops(PossibleCodeCompletions & model, int tabs){
    QString brackets = "{\n";
    for(int i=-1;i<=tabs;++i)
        brackets.append('\t');
    brackets.append('\n');
    for(int i=-1;i<tabs;++i)
        brackets.append('\t');
    brackets.append('}');

    model.push_back(new CodeCompletionEntry("for (int i = 0; i < ... ; ++i)"+brackets,"","Eine normale for schleife, die bis ... zählt."));
    model.push_back(new CodeCompletionEntry("for (int o = 0; o < outputLength ; ++o)"+brackets,"","Eine for schleife, die über den output iteriert."));
    model.push_back(new CodeCompletionEntry("for (int i = 0; i < intputLength ; ++i)"+brackets,"","Eine for schleife, die über den input iteriert."));
    model.push_back(new CodeCompletionEntry("for (float value : fftOutput)"+brackets,"","Eine for schleife über die Werte des FFtOutputs."));
    model.push_back(new CodeCompletionEntry("for (auto i = fftOutput.begin(begin_frequency) ; i != fftOutput.end(end_frequency); ++i)"+brackets,"","Eine for schleife, die über den fftOutput von einer bis zu einer anderen Requenz iteriert"));

}
void addCompletionsForType(PossibleCodeCompletions & model, QString type){
    if(type=="rgb_t"){
        model.push_back(new CodeCompletionEntry("r","brightness_t","Der Rotanteil des RGBWerts von 0 - 255"));
        model.push_back(new CodeCompletionEntry("red","brightness_t","Der Rotanteil des RGBWerts von 0 - 255"));
        model.push_back(new CodeCompletionEntry("g","brightness_t","Der Grünanteil des RGBWerts von 0 - 255"));
        model.push_back(new CodeCompletionEntry("green","brightness_t","Der Grünanteil des RGBWerts von 0 - 255"));
        model.push_back(new CodeCompletionEntry("blue","brightness_t","Der Blauanteil des RGBWerts von 0 - 255"));
        model.push_back(new CodeCompletionEntry("b","brightness_t","Der Blauanteil des RGBWerts von 0 - 255"));
        model.push_back(new CodeCompletionEntry("[0]","brightness_t","Der Rotanteil des RGBWerts von 0 - 255"));
        model.push_back(new CodeCompletionEntry("[1]","brightness_t","Der Grünanteil des RGBWerts von 0 - 255"));
        model.push_back(new CodeCompletionEntry("[2]","brightness_t","Der Blauanteil des RGBWerts von 0 - 255"));
    }
    if(type=="fftOutput"){
        model.push_back(new CodeCompletionEntry("getSampleRate()","int","Gibt an, mit welcher Rate das Audiosignal abgetastet wurde(in Hz)."));
        model.push_back(new CodeCompletionEntry("getMaxFrequency()","int","Gibt die maxiamle Frequenz zurück, die analysiert wurde."));
        model.push_back(new CodeCompletionEntry("getBlockSize()","double","Das Intervall [0,maxFrequency] wird ein size() Blöcke eingeteil. Diese Methode gibt zurück, welche Frequenzspanne ein Block hat."));
        model.push_back(new CodeCompletionEntry("getLowerBlockFrequency(iterator i)","int","Gibt die niedrigste Frquenz eines Blocks an, auf den der iterator zeigt."));
        model.push_back(new CodeCompletionEntry("getLowerBlockFrequency(int index)","int","Gibt die niedrigste Frquenz eines Blocks an, der den Index index hat."));
        model.push_back(new CodeCompletionEntry("getUpperBlockFrequency(iterator i)","int","Gibt die höchste Frquenz eines Blocks an, auf den der iterator zeigt."));
        model.push_back(new CodeCompletionEntry("getUpperBlockFrequency(int index)","int","Gibt die höchste Frquenz eines Blocks an, der den Index index hat."));
        model.push_back(new CodeCompletionEntry("begin(int frequency)","iterator","Gibt einen iterator zu dem Block, in dem die Frequenz frequency liegt, zurück."));
        model.push_back(new CodeCompletionEntry("end(int frequency)","iterator","Gibt einen iterator hinter den Block, in dem die Frequenz frequency liegt, zurück."));
        model.push_back(new CodeCompletionEntry("atFrequency(int frequency)","float","Gibt die stärke einer bestimmten Frequenz zurück(Die des Blocks in dem die Frequenz liegt)."));
        model.push_back(new CodeCompletionEntry("at(int index)","float","Gibt den Wert des Blocks mit Index index zurück."));
        model.push_back(new CodeCompletionEntry("size()","int","Gibt an, wie viele Blöcke es gibt."));
    }
}

void addDefaultVariables(PossibleCodeCompletions & model, Modules::Module * m){

    model.push_back(new CodeCompletionEntry("inputLength","unsigned int","Die Länge des Inputs"));
    model.push_back(new CodeCompletionEntry("outputLength","unsigned int","Die Länge des Outputs"));
    model.push_back(new CodeCompletionEntry("output[i]","unsigned int","Ein Element im Output an der Position index"));
    model.push_back(new CodeCompletionEntry("output","unsigned int * ","Der Outputarray"));
    model.push_back(new CodeCompletionEntry("input[i]","unsigned int","Ein Element im Input an der Position index"));
    model.push_back(new CodeCompletionEntry("input","unsigned int * ","Der Inputarray"));
}

void skipWhitespaces(int & cursor, QTextDocument * d){
    while (cursor>=0 && d->characterAt(cursor).isSpace()) {
        --cursor;
    }
}

/**
 * @brief checkBlock checks if the cursor is inside a block
 * @param startIndex the index where the block starts
 * @param cursor the position of the cursor
 * @param doc the document
 * @return the end index of the Block or 0 if inside
 */
int checkBlock(int startIndex, int cursor, QTextDocument * doc){
    if(doc->characterAt(startIndex)=='{')
        ++startIndex;
    int depth = 1;
    while (startIndex<doc->characterCount()) {
        if(startIndex == cursor)
            return 0;
        if(doc->characterAt(startIndex)=='{')
            ++depth;
        if(doc->characterAt(startIndex)=='}'){
            --depth;
            if(depth == 0){
                return startIndex;
            }
        }
        ++startIndex;
    }
    return startIndex;
}

void addUserVariables(PossibleCodeCompletions & model, int cursorPos, Modules::Module * m, QTextDocument * doc){
    for(auto p : *m->getPropertiesP()){
        model.push_back(new CodeCompletionEntry(p->getName(),toName(p->getType()),m->getDescription()));
    }
#ifdef CHECK_END
#error ALREADY_DEFINED
#endif
#define CHECK_END(i) if(i>=doc->characterCount() || i<0)return;
    int index=0;
    while (index<doc->characterCount()) {
        auto c = doc->characterAt(index);
        if(c.isSpace() || c == '}'){
            ++index;
            continue;
        }
        if(c == '{'){
            int newIndex = checkBlock(index,cursorPos,doc);
            if (newIndex == 0) {
                ++index;
            }else{
                index = newIndex+1;
            }
            continue;
        }
        if(doc->characterCount()-index > 10){
            if(c=='f'&&doc->characterAt(index+1)=='o'&&doc->characterAt(index+2)=='r'&&(doc->characterAt(index+3)==' '||doc->characterAt(index+3)=='(')){
                while (doc->characterAt(index)!='(') {
                    ++index;
                    CHECK_END(index)
                }
                // check for: for(type name : container)
                int rangeLoopIndex = index;
                while(doc->characterAt(rangeLoopIndex)!=')'){
                    if(doc->characterAt(rangeLoopIndex)==':'){
                        break;
                    }
                    ++rangeLoopIndex;
                    CHECK_END(rangeLoopIndex)
                }
                if(doc->characterAt(rangeLoopIndex)==':'){
                    --rangeLoopIndex;
                    skipWhitespaces(rangeLoopIndex,doc);
                    QString type_name;
                    while(doc->characterAt(rangeLoopIndex) != '('){
                        type_name.push_front(doc->characterAt(rangeLoopIndex));
                        --rangeLoopIndex;
                        CHECK_END(rangeLoopIndex)
                    }
                    model.push_back(new CodeCompletionEntry(type_name.mid(type_name.lastIndexOf(' ')),type_name.left(type_name.lastIndexOf(' ')),""));
                }else{
                    // not a range loop
                    while (doc->characterAt(index) != '=' && doc->characterAt(index) != ';') {
                        ++index;
                        CHECK_END(index);
                    }
                    rangeLoopIndex = index-1;
                    skipWhitespaces(rangeLoopIndex,doc);
                    QString type_name;
                    while(doc->characterAt(rangeLoopIndex) != '('){
                        type_name.push_front(doc->characterAt(rangeLoopIndex));
                        --rangeLoopIndex;
                        CHECK_END(rangeLoopIndex)
                    }
                    if(type_name.size()>4){
                        model.push_back(new CodeCompletionEntry(type_name.mid(type_name.lastIndexOf(' ')),type_name.left(type_name.lastIndexOf(' ')),""));
                    }
                }
                while (doc->characterAt(index)!='{') {
                    ++index;
                    CHECK_END(index)
                }
                continue;
            }
            {// check for return
                bool is_return = true;
                for (int i = 0; i < 6;++i) {
                    is_return &= doc->characterAt(index+i) == "return"[i];
                }
                if(is_return){
                    while (doc->characterAt(index)!=';') {
                        ++index;
                        CHECK_END(index);
                    }
                    ++index;
                    continue;
                }
            }
            //check for if
            if(c=='i' &&doc->characterAt(index+1)=='f' && (doc->characterAt(index+2)==' '||doc->characterAt(index+2)=='(')){
                while (doc->characterAt(index)!=';'&&doc->characterAt(index)!='{') {
                    ++index;
                    CHECK_END(index);
                }
                ++index;
                continue;
            }


        }
        int start = index;
        // a variable declaration can be:   a function can be:
        // type name = ...; or type name;   type name(...) { ... }
        bool continueAtStart = false;
        while (c != ';' && c != '=') {
            if(c=='(')
                break;
            if(c=='}' || c == '\n'){ // maybe we are in the line where the cursor is and we have ne real end like ';'
                continueAtStart = true;
                break;
            }
            c = doc->characterAt(++index);
            CHECK_END(index)
        }
        if(continueAtStart){
            continue;
        }
        if(c!='('){
            // variable declaration or assignment found
            int i = index-1;
            skipWhitespaces(i,doc);
            QString variableName = "";
            while (!doc->characterAt(i).isSpace()) {
                variableName.push_front(doc->characterAt(i));
                --i;
                CHECK_END(i)
            }
            if(i>start+1){// no assigments
                QString type = "";
                while (start != i) {
                    type += doc->characterAt(start);
                    ++start;
                }
                model.push_back(new CodeCompletionEntry(variableName,type,""));
            }
            while (doc->characterAt(index)!=';') {
                ++index;
                CHECK_END(index)
            }
            ++index;
            continue;
        }else{// function declaration
            int startIndex = index+1;
            while (doc->characterAt(index)!='{') {
                ++index;
                CHECK_END(index)
            }
            auto endIndex = checkBlock(index,cursorPos,doc);
            if(endIndex != 0){
                index = endIndex+1;
                continue;
            }
            //parseInto function
            QString parameters ;
            while (doc->characterAt(startIndex) != ')') {
                parameters.push_back(doc->characterAt(startIndex));
                ++startIndex;
                CHECK_END(startIndex)
            }
            for(const auto & parts : parameters.splitRef(",",QString::SplitBehavior::SkipEmptyParts)){
                model.push_back(new CodeCompletionEntry(parts.mid(parts.lastIndexOf(" ")).toString(),parts.left(parts.lastIndexOf(" ")).toString(),""));
            }
        }
    }
#undef CHECK_END
}



void CodeEditorHelper::updateCodeCompletionModel(int cursorPos){
    for(auto i : codeCompletions.model){
        delete i;
    }
    codeCompletions.model.clear();
    QString find;
    //find if cursor is in text
    {
        int start = cursorPos-1;
        while (start>=0 && (document->characterAt(start).isLetterOrNumber() || document->characterAt(start) == '_')) {
            --start;
        }
        if(start == cursorPos-1){
            codeCompletions.setFilterFixedString("");
        }else{
            for(int i = start+1; i < cursorPos; ++i){
                find += document->characterAt(i);
            }
            codeCompletions.setFilterCaseSensitivity(Qt::CaseInsensitive);
            codeCompletions.setFilterRegExp("^" + find);
        }
    }
    bool objectCompletion = false;
    // check if we access properties of a object
    {
        int start = cursorPos - 1;
        skipWhitespaces(start,document);
        // skip things linke <here 'gre'>: rgb_t.gre
        while (start>0 && (document->characterAt(start).isLetterOrNumber() || document->characterAt(start)=='_')) {
            --start;
        }
        if(start > 0 && (document->characterAt(start--) == '.' || (document->characterAt(start+1) == '>' && document->characterAt(start--) == '-')) ){
            // get variable
            skipWhitespaces(start,document);
            // skip index access if existing
            if(document->characterAt(start)==']'){
                while (document->characterAt(--start)!='[');
                --start;
            }
            QString reverseVariable;
            while (document->characterAt(start).isLetterOrNumber() || document->characterAt(start) == '_') {
                reverseVariable += document->characterAt(start);
                --start;
            }
            if(reverseVariable.back().isNumber()){
                goto noObject;
            }
            QString variable;
            for(auto i = reverseVariable.crbegin(); i!= reverseVariable.crend() ;++i){
                variable += *i;
            }
            //getType
            addCompletionsForType(codeCompletions.model,getType(variable,start));
            objectCompletion = true;
        }
        noObject:;
    }
    if(!objectCompletion){
        addBasicTypes(codeCompletions.model);
        addDefaultVariables(codeCompletions.model,module);
        addArrayTypes(codeCompletions.model,module);
        addAudioTypes(codeCompletions.model);
        addForLoops(codeCompletions.model,countTabs(cursorPos));
        addUserVariables(codeCompletions.model,cursorPos,module,document);
    }
}

QString CodeEditorHelper::getType(QString variable, int pos){
    if(variable == "inputLength")
        return "unsigned int";
    if(variable == "outputLength")
        return "unsigned int";
    if(variable == "output" || variable == "input"){
        if(module->getOutputType() == Modules::Brightness)
            return "brightness_t";
        if(module->getOutputType() == Modules::RGB)
            return "rgb_t";
    }
    if(variable == "fftOutput"){
        return "fftOutput";
    }
    return "unknown";
}


CodeEditorHelper::CodeEditorHelper(){
    codeCompletions.model.push_back(new CodeCompletionEntry("2test","21test","1test"));
    codeCompletions.model.push_back(new CodeCompletionEntry("2test2","12tes2t","2test2"));
    codeCompletions.setDynamicSortFilter(true);
    codeCompletions.sort(0);
}

QString generateProgrammCode(){
    QString code = "int getProgrammLengthInMS()override{\n\treturn Program::Infinite;\n}\n\n";
    code += "void start()override{\n\t\n}\n\n";
    code += "ProgramState doStep(time_diff_t diff_ms)override{\n\treturn {false/*finished*/,true/*output changed*/};\n}\n\n";
    return code;
}

QString generateLoopProgrammCode(){
    QString code = "int getProgrammLengthInMS()override{\n\treturn 50 * outputLength; // or Program::Infinite \n}\n\n";
    code += "void loopProgram()override{\n\tfor (int i = 0; i < outputLength; ++i){\n\t\t//output[i] = ... ;\n\t\twait(50);//waits 50 ms und shows the output\n\t}\n}\n\n";
    return code;
}

QString generateFilterCode(){
    QString code = "unsigned int computeOutputLength(unsigned int inputLength)override{\n\treturn inputLength;\n}\n\n";
    code += "void filter()override{\n\t\n}\n\n";
    code += "bool doStep(time_diff_t diff_ms)override{\n\treturn false/*output changed*/;\n}\n\n";
    return code;
}

void CodeEditorHelper::setModule(  Modules::Module* _module){
        if(_module != module){
                module = _module;
                QObject::disconnect(typeConnection);
                if(module){
                    typeConnection = QObject::connect(module,&Modules::Module::typeChanged,this,&CodeEditorHelper::typeChanged);
                    if(module->getCode().isEmpty()){
                        //create default code
                        if(module->getType()==Modules::Module::Program){
                            module->setCode(generateProgrammCode());
                        }else if(module->getType()==Modules::Module::LoopProgram){
                            module->setCode(generateLoopProgrammCode());
                        }else if(module->getType()==Modules::Module::Filter){
                            module->setCode(generateFilterCode());
                        }
                    }
                }
                emit moduleChanged();
        }
}

void CodeEditorHelper::typeChanged(){    
    QString text = document->toPlainText().trimmed();
    // check if we can replace the whole text(when it is empty or a default code)
    bool replace = text.trimmed().isEmpty() ? true : text == generateFilterCode().trimmed() || text == generateLoopProgrammCode().trimmed() || text == generateProgrammCode().trimmed();
    if(module->getType()==Modules::Module::Program){
        // we only have to generate code if the current code dont have the right functions
        if(replace)
            module->setCode(generateProgrammCode());
        else if(text.indexOf("int getProgrammLengthInMS()") <0 ||  text.indexOf("void start()") <0 ||  text.indexOf("ProgramState doStep(time_diff_t") <0){
            //emit insertText(generateProgrammCode(),0);
            module->setCode(generateProgrammCode() + module->getCode());
            qDebug ()  << "insert ProgrammCode" << generateProgrammCode();
        }
    }else if(module->getType()==Modules::Module::LoopProgram){
        // we only have to generate code if the current code dont have the right functions
        if(replace)
            module->setCode(generateLoopProgrammCode());
        else if(text.indexOf("int getProgrammLengthInMS()") <0 || text.indexOf("void loopProgram()") <0){
            emit insertText(generateLoopProgrammCode(),0);
            qDebug ()  << "insert LoopProgrammCode" << generateLoopProgrammCode();
        }
    }else if(module->getType()==Modules::Module::Filter){
        // we only have to generate code if the current code dont have the right functions
        if(replace)
            module->setCode(generateFilterCode());
        else if(text.indexOf("unsigned int computeOutputLength(unsigned int inputLength)") <0 ||  text.indexOf("void filter()") <0 ||  text.indexOf("bool doStep(time_diff_t ") <0){
            emit insertText(generateFilterCode(),0);
            qDebug ()  << "insert FilterCode " << generateFilterCode();
        }
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
        /*if(document->characterAt(from-1) == '{'){
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
        }else{*/
            int tabs = countTabs(from-1);
            if(document->characterAt(from-1) == '{'){
                tabs += 1;
            }
            if(tabs==0)
                return;
            QString newText;
            qDebug()<<"write2 : "<<tabs;
            for(int i = 0 ; i< tabs;++i)
                newText += (QString(QChar::Tabulation));
            emit insertText(newText,from + tabs +1);
        //}
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

/**
 * @brief checkForDuplicatedPropertiesNames Check if no own symbol with the same name of a propertie is declared
 * @param userCode The code to check
 * @param vec the Vector of the properties to check
 * @param func a callback that is called with the name of a property if a own symbol with the same name is found
 * @return true if found one match
 */
template<typename Function>
bool checkForDuplicatedPropertiesNames(const QString &userCode, const Modules::PropertiesVector & vec, Function func){
    QString typeRegex = "(unsigned|int|bool|float|double|long|void|char|short) *\\**&? *%1[^\\w]";
    bool matches = false;
    for(const auto & prop : vec){
        QRegularExpression regex(typeRegex.arg(prop->getName()),QRegularExpression::CaseInsensitiveOption|QRegularExpression::MultilineOption);
        if(userCode.contains(regex)){
            func(prop->getName());
            matches = true;
        }
    }
    return matches;
}

void replacePropertiesUsages(QString &code, const Modules::PropertiesVector & vec){
    for(const auto p : vec){
        QRegularExpression regex("(?<!\\w)"+p->getName()+"(?!\\w)");
        switch (p->getType()) {
        case Modules::Property::Int:
        case Modules::Property::Double:
        case Modules::Property::Float:
        case Modules::Property::Long:
            code.replace(regex,"_" + p->getName() + ".asNumeric<" + toName(p->getType()) + ">()->getValue()");
            break;
        case Modules::Property::Bool:
            code.replace(regex,"_" + p->getName() + ".asBool()->getValue()");
            break;
        case Modules::Property::String:
            code.replace(regex,"_" + p->getName() + ".asString()->getString()");
            break;
        case Modules::Property::RGB:
            code.replace(regex,"_" + p->getName() + ".asRGB()");
            break;
        }
    }
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
    QString userCode = document->toPlainText();
    for(const auto p1 : module->getProperties()){
        for(const auto p2 : module->getProperties()){
            if(p1 != p2 && p1->getName() == p2->getName()){
                emit information("Es gibt mehrere Properties mit dem Namen : " + p1->getName() +"\nBreche Compilieren ab.");
                return;
            }
        }
    }
    {
       QString msg = "Fehler:\n";
       if(checkForDuplicatedPropertiesNames(userCode,module->getProperties(),[&](auto name){
           msg += "Es darf keine eigene Variable/Function mit dem Namen " + name + " deklariert werden.\n";
       })){
           emit information(msg);
           return;
       };
    }
    replacePropertiesUsages(userCode,module->getProperties());

    Settings s;
    QFile file( s.getModuleDirPath() + "/" + module->getName() + ".cpp" );
    file.remove();
    if ( file.open(QIODevice::ReadWrite) )
    {
        QString typeName = toName(module->getType());
        QString inputType = toName(module->getInputType());
        QString outputType = toName(module->getOutputType());
        QTextStream stream( &file );
        stream << "#define MODULE_LIBRARY" << endl;
        stream << "#define HAVE_AUDIO" << endl;
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
        stream << "#include <module.h>" << endl;
        stream << "#include <iostream>" << endl;
        stream << "#define _USE_MATH_DEFINES"<<endl;
        stream << "#include <cmath>"<<endl;
        stream << "" << endl;
        stream << "using namespace Modules;" << endl;
        stream << "using namespace std;" << endl;
        stream << "" << endl;
        if(module->getType() == Modules::Module::Filter){
            stream << "class Impl : public Typed" << typeName << "<"<<inputType<<","<<outputType<<">{"<< endl;
        }else{
            stream << "class Impl : public Typed" << typeName << "<"<<outputType<<">{"<< endl;
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
        stream << userCode << endl; // write Code from document
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


        auto result = Modules::Compiler::compileAndLoadModule(file,module->getName());

        if(result.first){
            QFileInfo finfo = file;
            emit information(result.second.replace(finfo.absoluteFilePath(),""));
        }else
            emit information("Compilieren erfolgreich.");
    }else{
        emit information("Cant open file : " + s.getModuleDirPath() + "/" + module->getName() + ".cpp");
    }
}
