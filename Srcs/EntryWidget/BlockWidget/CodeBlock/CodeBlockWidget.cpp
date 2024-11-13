#include "CodeBlockWidget.h"
#include "../../EntryArea.h"
#include <QBoxLayout>


CodeBlockWidget::CodeBlockWidget(QWidget* parent) :
    BlockWidget(parent), 
    code_edit(0)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    code_edit = new CodeEdit(this);
    layout->addWidget(code_edit);
    // combo box
    languange_box = new LanguageComboBox(this);
    layout->addWidget(languange_box);
    setLayout(layout);

    connect(code_edit, &CodeEdit::textChanged, this, &CodeBlockWidget::justifyHeight);
    connect(code_edit, &CodeEdit::textChanged, this, &CodeBlockWidget::emitContentChange);
    connect(languange_box, &LanguageComboBox::languageBoxUndoRedo, this, &CodeBlockWidget::handleLanguageChanged);
    connect(languange_box, &LanguageComboBox::currentIndexChanged, this, &CodeBlockWidget::handleLanguageChanged);

    
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
    dest.append_attribute("language").set_value(languange_box->currentIndex());
    dest.text().set(code_edit->toPlainText().toStdString().c_str());
}

void CodeBlockWidget::importFromPugi(const pugi::xml_node& node)
{
    qDebug() << "CodeBlockWidget::importFromPugi(" << node.attribute("language").as_int() << ")";

    languange_box->setCurrentIndex(node.attribute("language").as_int());
    code_edit->setPlainText(QString::fromStdString(node.text().as_string()));
}

BlockType CodeBlockWidget::type() const
{
    return BlockType::Code;
}

void CodeBlockWidget::updateHighlighter()  // 根据当前languange_box的选择更新highlighter
{

    qDebug() << "CodeBlockWidget::updateHighlighter: text=" << languange_box->currentText();

    switch (languange_box->currentIndex())
    {
    case 1:
        qDebug() << "C++";
        code_edit->setHighlighter(HighlightingRules::getCppRules());
        break;
    case 2:
        qDebug() << "Python";
        code_edit->setHighlighter(HighlightingRules::getPythonRules());
        break;
    case 3:
        qDebug() << "HTML";
        code_edit->setHighlighter(HighlightingRules::getHtmlRules());
        break;
    default:
        qDebug() << "Default Or PlainText";
        code_edit->removeHighlighter();
        break;
    }
}

void CodeBlockWidget::handleLanguageChanged()
{
    updateHighlighter();
    emitContentChange();
}