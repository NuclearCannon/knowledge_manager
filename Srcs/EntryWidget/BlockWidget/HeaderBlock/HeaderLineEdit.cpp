#include "HeaderBlockWidget.h"

HeaderLineEdit::HeaderLineEdit(QWidget* parent) :
    QLineEdit(parent)
{
    setStyleSheet(
        "HeaderLineEdit:hover { background-color: rgba(0, 0, 0, 20); border: none;}  "
        "HeaderLineEdit:focus { background-color: rgba(0, 0, 0, 20); border: none;}  "
        "HeaderLineEdit { background-color: rgba(0, 0, 0, 0); border: none;}  "
    );
    setFocus();
    
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

void HeaderLineEdit::contextMenuEvent(QContextMenuEvent* event)
{
    emit contextMenuQuery(event);
}

