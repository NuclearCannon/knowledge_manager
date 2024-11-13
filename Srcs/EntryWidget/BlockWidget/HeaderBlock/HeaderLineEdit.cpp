#include "HeaderBlockWidget.h"

HeaderLineEdit::HeaderLineEdit(QWidget* parent) :
    QLineEdit(parent)
{

}
void HeaderLineEdit::undo()
{
    this->QLineEdit::undo();
}

void HeaderLineEdit::redo()
{
    this->QLineEdit::redo();
}

void HeaderLineEdit::clearUndoStack()
{
    // QLineEdit没有直接访问undo stack的接口，我们要通过一点间接手段
    blockSignals(true);
    QString copy = text();
    clear();
    setText(copy);
    blockSignals(false);
}