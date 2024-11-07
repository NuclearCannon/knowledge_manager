#include "CodeBlockWidget.h"
#include "../EntryArea.h"
#include <QBoxLayout>
#include <sstream>

// HighlightingRules

void HighlightingRules::initCppRules()
{
    // 关键字格式：蓝色加粗
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);

    // 构造关键字列表  
    QStringList keywordPatterns;
    keywordPatterns << "\\bif\\b" << "\\belse\\b" << "\\bwhile\\b"
        << "\\bfor\\b" << "\\bint\\b" << "\\bfloat\\b"
        << "\\breturn\\b" << "\\bclass\\b";

    QRegularExpression keywordRule(QString("(\\b(%1)\\b)").arg(keywordPatterns.join("|")));
    cppRules.push_back({ keywordRule, keywordFormat });

    // 设置注释规则  
    QRegularExpression commentRule("//.*");
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::green);
    cppRules.push_back({ commentRule, commentFormat });

    // 设置字符串规则
    QRegularExpression stringRule("\"[^\"]*\"");
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::red);
    cppRules.push_back({ stringRule, stringFormat });
}

void HighlightingRules::initPythonRules()
{
    // 关键字格式：蓝色加粗
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);

    // 构造关键字列表  
    QStringList keywordPatterns;
    keywordPatterns << "\\bif\\b" << "\\belse\\b" << "\\belif\\b" << "\\bwhile\\b"
        << "\\bfor\\b" << "\\bint\\b" << "\\bfloat\\b"
        << "\\breturn\\b" << "\\bclass\\b" << "\\bdef\\b";

    QRegularExpression keywordRule(QString("(\\b(%1)\\b)").arg(keywordPatterns.join("|")));
    pythonRules.push_back({ keywordRule, keywordFormat });

    // 设置注释规则  
    QRegularExpression commentRule("#.*");
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::green);
    pythonRules.push_back({ commentRule, commentFormat });

    // 设置字符串规则
    QRegularExpression stringRule("(\"[^\"]*\")|('[^']*')");
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::red);
    pythonRules.push_back({ stringRule, stringFormat });
}

void HighlightingRules::initHtmlRule()
{
    // 设置格式  
    QTextCharFormat tagFormat;
    tagFormat.setForeground(Qt::blue);
    tagFormat.setFontWeight(QFont::Bold);
    htmlRules.append({ QRegularExpression("<[^>]+>"), tagFormat });

    QTextCharFormat attributeFormat;
    attributeFormat.setForeground(Qt::darkGreen);
    htmlRules.append({ QRegularExpression("\\b[a-zA-Z-]+\\s*=\\s*\"[^\"]*\"\\s*"), attributeFormat });

    QTextCharFormat entityFormat;
    entityFormat.setForeground(Qt::red);
    htmlRules.append({ QRegularExpression("&[a-zA-Z]+;"), entityFormat });
}

HighlightingRules::HighlightingRules()
{
    initCppRules();
    initPythonRules();
    initHtmlRule();
}


const QVector<HighlightingRule>& HighlightingRules::getCppRules()
{
    return highlighting_rules.cppRules;
}

const QVector<HighlightingRule>& HighlightingRules::getPythonRules()
{
    return highlighting_rules.pythonRules;
}

const QVector<HighlightingRule>& HighlightingRules::getHtmlRules()
{
    return highlighting_rules.htmlRules;
}

HighlightingRules HighlightingRules::highlighting_rules = HighlightingRules();


// 代码高亮器




CodeBlockHighlighter::CodeBlockHighlighter(QTextDocument* parent, const QVector<HighlightingRule>& rules)
    : QSyntaxHighlighter(parent), rules(rules)
{

    
}

void CodeBlockHighlighter::highlightBlock(const QString& text)
{
    for (const HighlightingRule& rule : rules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    
}





// 代码编辑框
CodeEdit::CodeEdit(CodeBlockWidget* parent) :
    QTextEdit(parent),
    parent_ptr(parent),
    highlighter(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setTabStopDistance(20);
    setStyleSheet("border-bottom: 0px;");
    
}

CodeEdit::~CodeEdit()
{
    if (highlighter)
    {
        delete highlighter;
        highlighter = nullptr;
    }
}

void CodeEdit::setHighlighter(const QVector<HighlightingRule>& rules)
{
    if (highlighter)delete highlighter;
    highlighter = new CodeBlockHighlighter(document(), rules);

}

void CodeEdit::removeHighlighter()
{
    if (highlighter)
    {
        delete highlighter;
        highlighter = nullptr;
    }
}

void CodeEdit::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        qDebug() << "Enter handled";
        // 拦截回车键  
        QTextCursor cursor = textCursor();
        // 在当前光标位置插入换行符  
        cursor.insertText("\n");
        // 设置光标位置（如果需要的话，可以在这里调整）  
        setTextCursor(cursor);
        // 阻止默认行为（即创建新段落）  
        event->accept();
    }
    else {
        // 对于其他按键，调用基类的keyPressEvent方法  
        QTextEdit::keyPressEvent(event);
    }
}



CodeBlockWidget* CodeEdit::toBlockParent() const
{
    return parent_ptr;
}


// LanguageComboBox

LanguageComboBox::LanguageComboBox(CodeBlockWidget* parent) :
    QComboBox(parent) 
{
    addItem("C++");
    addItem("Python");
    addItem("HTML");
    setEditable(true);
};


//代码块
CodeBlockWidget::CodeBlockWidget(QWidget* parent) :BlockWidget(parent), code_edit(0)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    code_edit = new CodeEdit(this);
    layout->addWidget(code_edit);
    // combo box
    languange_box = new LanguageComboBox(this);
    layout->addWidget(languange_box);
    this->setLayout(layout);
    // 把文本框的“文本变化”信号连接到槽“调整高度”上
    connect(code_edit, &CodeEdit::textChanged, this, &CodeBlockWidget::justifyHeight);
    connect(code_edit, &CodeEdit::textChanged, this, &CodeBlockWidget::emitContentChange);
    connect(languange_box, &LanguageComboBox::currentTextChanged, this, &CodeBlockWidget::emitContentChange);
    connect(languange_box, &LanguageComboBox::currentTextChanged, this, &CodeBlockWidget::updateHighlighter);
    
    // 安装事件过滤器

    code_edit->installEventFilter(filter);
    languange_box->installEventFilter(filter);
    // 代码块设置高亮
    updateHighlighter();
    justifyHeight();// 调整一次高度

    qDebug() << "CodeBlockWidget 构造时尺寸" << size();
}




void CodeBlockWidget::justifyHeight()
{
    code_edit->document()->adjustSize();//将文档调整到合理的大小。
    int code_edit_height = code_edit->document()->size().rheight() + 5;
    code_edit->setFixedHeight(code_edit_height);
    setFixedHeight(code_edit_height + languange_box->height());
}


CodeBlockWidget::~CodeBlockWidget()
{

}




void CodeBlockWidget::exportToPugi(pugi::xml_node& dest)
{
    dest.set_name("code-block");
    dest.append_attribute("language").set_value(languange_box->currentText().toStdString().c_str());
    dest.text().set(code_edit->toPlainText().toStdString().c_str());
}

void CodeBlockWidget::importFromPugi(const pugi::xml_node& node)
{
    languange_box->setCurrentText(node.attribute("languange").as_string());
    code_edit->setPlainText(QString::fromStdString(node.text().as_string()));
}

BlockType CodeBlockWidget::type() const
{
    return BlockType::Code;
}

void CodeBlockWidget::updateHighlighter()  // 根据当前languange_box的选择更新highlighter
{

    QString current = languange_box->currentText();

    qDebug() << "updateHighlighter: text=" << current;

    if (current == "C++")
    {
        qDebug() << "C++";
        code_edit->setHighlighter(HighlightingRules::getCppRules());
    }
    else if (current == "Python")
    {
        qDebug() << "Python";
        code_edit->setHighlighter(HighlightingRules::getPythonRules());
    }
    else if (current == "HTML")
    {
        qDebug() << "HTML";
        code_edit->setHighlighter(HighlightingRules::getHtmlRules());
    }
    else
    {
        qDebug() << "Default";
        code_edit->removeHighlighter();
    }

}