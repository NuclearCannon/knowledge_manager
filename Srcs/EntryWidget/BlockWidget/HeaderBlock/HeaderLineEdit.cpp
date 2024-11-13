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
    // QLineEditû��ֱ�ӷ���undo stack�Ľӿڣ�����Ҫͨ��һ�����ֶ�
    blockSignals(true);
    QString copy = text();
    clear();
    setText(copy);
    blockSignals(false);
}