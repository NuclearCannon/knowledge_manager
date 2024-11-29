#include "CodeBlockWidget.h"
// LanguageComboBox

LanguageComboBox::LanguageComboBox(CodeBlockWidget* parent) :
    QComboBox(parent),
    BlockControl(parent)
{
    blockSignals(true);
    addItem("Plain Text");
    addItem("C++");
    addItem("Python");
    addItem("HTML");
    current_index = 0;
    setCurrentIndex(0);
    // 修改后，删除redo栈，并且压入undo栈
    connect(this, &LanguageComboBox::currentIndexChanged, this, &LanguageComboBox::clearRedoStackOnly);
    connect(this, &LanguageComboBox::currentIndexChanged, this, &LanguageComboBox::undoRecord);
    
    setFixedHeight(21);  // 锁定高度
    blockSignals(false);
};


void LanguageComboBox::undo()
{
    if (undo_stack.empty())
    {
        // 撤销栈为空，什么都不做
        qDebug() << "LanguageComboBox: undo stack is empty!";
    }
    else
    {
        redo_stack.push(current_index);
        blockSignals(true);
        current_index = undo_stack.top();
        setCurrentIndex(current_index);
        blockSignals(false);
        undo_stack.pop();

        emit languageBoxUndoRedo();
    }
}
void LanguageComboBox::redo()
{
    if (redo_stack.empty())
    {
        // 重做栈为空，什么都不做
        qDebug() << "LanguageComboBox: redo stack is empty!";
    }
    else
    {
        undo_stack.push(currentIndex());
        blockSignals(true);
        current_index = redo_stack.top();
        setCurrentIndex(current_index);
        blockSignals(false);
        redo_stack.pop();

        emit languageBoxUndoRedo();
    }
}
void LanguageComboBox::clearUndoStack()
{
    clearUndoStackOnly();
    clearRedoStackOnly();
}

void LanguageComboBox::clearUndoStackOnly()
{
    std::stack<int> empty;
    undo_stack.swap(empty);
}

void LanguageComboBox::clearRedoStackOnly()
{
    std::stack<int> empty;
    redo_stack.swap(empty);
}
void LanguageComboBox::undoRecord()
{
    undo_stack.push(current_index);
    current_index = currentIndex();
}

void LanguageComboBox::contextMenuEvent(QContextMenuEvent* event)
{
    emit contextMenuQuery(event);
}