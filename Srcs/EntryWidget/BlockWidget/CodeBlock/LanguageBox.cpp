#include "CodeBlockWidget.h"
// LanguageComboBox

LanguageComboBox::LanguageComboBox(CodeBlockWidget* parent) :
    QComboBox(parent)
{
    blockSignals(true);
    addItem("Plain Text");
    addItem("C++");
    addItem("Python");
    addItem("HTML");
    current_index = 0;
    setCurrentIndex(0);
    // �޸ĺ�ɾ��redoջ������ѹ��undoջ
    connect(this, &LanguageComboBox::currentIndexChanged, this, &LanguageComboBox::clearRedoStackOnly);
    connect(this, &LanguageComboBox::currentIndexChanged, this, &LanguageComboBox::undoRecord);
    blockSignals(false);
};


void LanguageComboBox::undo()
{
    if (undo_stack.empty())
    {
        // ����ջΪ�գ�ʲô������
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
        // ����ջΪ�գ�ʲô������
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