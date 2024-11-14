#include "CodeBlockWidget.h"
#include <QBoxLayout>
#include <QMenu>

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
    connect(code_edit, &CodeEdit::contextMenuQuery, this, &CodeBlockWidget::handleContextMenuQueryFromControls);
    connect(languange_box, &LanguageComboBox::contextMenuQuery, this, &CodeBlockWidget::handleContextMenuQueryFromControls);
    
    // 安装事件过滤器
    code_edit->installEventFilter(filter);
    languange_box->installEventFilter(filter);
    // 代码块设置高亮
    updateHighlighter();
    justifyHeight();// 调整一次高度

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

void CodeBlockWidget::exportToQtXml(QDomElement& dest, QDomDocument& dom_doc)
{
    dest.setTagName("code-block");
    dest.setAttribute("language", languange_box->currentIndex());
    dest.appendChild(dom_doc.createTextNode(code_edit->toPlainText()));
}
void CodeBlockWidget::importFromQtXml(QDomElement& src)
{
    languange_box->setCurrentIndex(src.attribute("language").toInt());
    code_edit->setPlainText(src.firstChild().toText().data());
}

void CodeBlockWidget::contextMenuEvent(QContextMenuEvent* event)
{
    // 创建一个菜单
    QMenu contextMenu(nullptr);

    // 添加菜单项
    QAction* action_undo_edit = contextMenu.addAction("撤销(编辑框)");
    connect(action_undo_edit, &QAction::triggered, this, &CodeBlockWidget::handleUndoEdit);

    QAction* action_redo_edit = contextMenu.addAction("重做(编辑框)");
    connect(action_redo_edit, &QAction::triggered, this, &CodeBlockWidget::handleRedoEdit);

    QAction* action_undo_box = contextMenu.addAction("撤销(语言栏)");
    connect(action_undo_box, &QAction::triggered, this, &CodeBlockWidget::handleUndoBox);

    QAction* action_redo_box = contextMenu.addAction("重做(语言栏)");
    connect(action_redo_box, &QAction::triggered, this, &CodeBlockWidget::handleRedoBox);


    QAction* action_delete = contextMenu.addAction("删除");
    connect(action_delete, &QAction::triggered, this, &CodeBlockWidget::handleContextMenuDelete);

    QMenu* menu_insert_above = contextMenu.addMenu("在上方插入");
    QAction* action_insert_above_text = menu_insert_above->addAction("文本块");
    connect(action_insert_above_text, &QAction::triggered, this, &CodeBlockWidget::handleContextMenuInsertAboveText);

    QAction* action_insert_above_code = menu_insert_above->addAction("代码块");
    connect(action_insert_above_code, &QAction::triggered, this, &CodeBlockWidget::handleContextMenuInsertAboveCode);

    QAction* action_insert_above_image = menu_insert_above->addAction("图片块");
    connect(action_insert_above_image, &QAction::triggered, this, &CodeBlockWidget::handleContextMenuInsertAboveImage);

    QAction* action_insert_above_header = menu_insert_above->addAction("标题块");
    connect(action_insert_above_header, &QAction::triggered, this, &CodeBlockWidget::handleContextMenuInsertAboveHeader);

    QMenu* menu_insert_below = contextMenu.addMenu("在下方插入");
    QAction* action_insert_below_text = menu_insert_below->addAction("文本块");
    connect(action_insert_below_text, &QAction::triggered, this, &CodeBlockWidget::handleContextMenuInsertBelowText);

    QAction* action_insert_below_code = menu_insert_below->addAction("代码块");
    connect(action_insert_below_code, &QAction::triggered, this, &CodeBlockWidget::handleContextMenuInsertBelowCode);

    QAction* action_insert_below_image = menu_insert_below->addAction("图片块");
    connect(action_insert_below_image, &QAction::triggered, this, &CodeBlockWidget::handleContextMenuInsertBelowImage);

    QAction* action_insert_below_header = menu_insert_below->addAction("标题块");
    connect(action_insert_below_header, &QAction::triggered, this, &CodeBlockWidget::handleContextMenuInsertBelowHeader);

    // 在事件发生的位置显示菜单
    contextMenu.exec(event->globalPos());
}

void CodeBlockWidget::handleUndoEdit()
{
    code_edit->undo();
}
void CodeBlockWidget::handleRedoEdit()
{
    code_edit->redo();
}
void CodeBlockWidget::handleUndoBox()
{
    languange_box->undo();
}
void CodeBlockWidget::handleRedoBox()
{
    languange_box->redo();
}
//void CodeBlockWidget::handleContextMenuQueryFromControls(QContextMenuEvent* event)
//{
//    contextMenuEvent(event);
//}