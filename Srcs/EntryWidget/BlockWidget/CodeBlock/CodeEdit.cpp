#include "CodeBlockWidget.h"
// 代码编辑框
CodeEdit::CodeEdit(CodeBlockWidget* parent) :
    QTextEdit(parent),
    parent_ptr(parent),
    highlighter(0),
    BlockControl(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setTabStopDistance(20);
    setStyleSheet("border-bottom: 0px;");
    setFocus();
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

void CodeEdit::undo()
{
    this->QTextEdit::undo();
}
void CodeEdit::redo()
{
    this->QTextEdit::redo();
}
void CodeEdit::clearUndoStack()
{
    this->document()->clearUndoRedoStacks();
}

void CodeEdit::contextMenuEvent(QContextMenuEvent* event)
{
    emit contextMenuQuery(event);
}

void CodeEdit::focusOutEvent(QFocusEvent* event)
{
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection())
    {
        cursor.clearSelection();
    }
    cursor.setPosition(0);
    setTextCursor(cursor);
    QTextEdit::focusOutEvent(event);
}