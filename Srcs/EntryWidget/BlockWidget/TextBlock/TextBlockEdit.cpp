#include "TextBlockWidget.h"
#include <QApplication>
#include <QDesktopServices>
#include <QThread>
#include <QMessageBox>
#include <QClipboard>


QString local_clipboard;

TextBlockEdit::TextBlockEdit(TextBlockWidget* parent) :
    QTextEdit(parent),
    BlockControl(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setReadOnly(false);
    setTabStopDistance(20);
    setStyleSheet(
        "TextBlockEdit:hover { background-color: rgba(0, 0, 0, 20); border: none; }  "
        "TextBlockEdit:focus { background-color: rgba(0, 0, 0, 20); border: none; }  "
        "TextBlockEdit { background-color: rgba(0, 0, 0, 0); border: none; }");
    setUndoRedoEnabled(true);
    
    setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    setLineWrapMode(QTextEdit::WidgetWidth);
    setFocus();
}

void TextBlockEdit::setStyleOnSelection(FormatItem x, bool value)
{
    // 获取QTextEdit的当前文本光标  
    QTextCursor cursor = textCursor();

    // 检查是否有选中的文本，如果没有选中的文本，则不执行任何操作  
    if (!cursor.hasSelection())return;

    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    bool legal = true;
    QTextCursor temp_cursor = cursor;
    temp_cursor.clearSelection();
    temp_cursor.setPosition(start);

    while (temp_cursor.position() < end)
    {
        QTextCharFormat charFormat = temp_cursor.charFormat();
        TextType type = getQTextCharFormatType(charFormat);
        if (type == TextType::code)
        {
            QMessageBox::warning(nullptr, "Warning", QStringLiteral("不可以对内联代码修改样式！"), QMessageBox::Ok);
            legal = false;
            break;

        }
        if (type == TextType::link)
        {
            QMessageBox::warning(nullptr, "Warning", QStringLiteral("不可以对链接修改样式！"), QMessageBox::Ok);
            legal = false;
            break;
        }
        temp_cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, 1);
    }

    if (legal)
    {
        QTextCharFormat newFormat;
        switch (x)
        {
        case FormatItem::bold:
            newFormat.setFontWeight(value ? QFont::Bold : QFont::Normal);
            break;
        case FormatItem::italic:
            newFormat.setFontItalic(value);
            break;
        case FormatItem::underline:
            newFormat.setFontUnderline(value);
            break;
        case FormatItem::strike:
            newFormat.setFontStrikeOut(value);
            break;
        }
        cursor.mergeCharFormat(newFormat);
        cursor.clearSelection();
    }
}

void TextBlockEdit::setTypeOnSelection(TextType type)
{
    // 获取QTextEdit的当前文本光标  
    QTextCursor cursor = textCursor();

    // 检查是否有选中的文本，如果没有选中的文本，则不执行任何操作  
    if (!cursor.hasSelection())return;

    QTextCharFormat newFormat;  // 生成普通样式
    switch (type)
    {
    case TextType::normal: {
        break;
    }

    case TextType::code: {
        newFormat = codeFormat();
        break;
    }
    case TextType::link: {
        QString href;
        bool succeed = InputUrlDialog::getStringByDialog(QStringLiteral("请输入超链接"), href);
        if (!succeed)return;
        newFormat = linkFormat(href);
        break;

    }

    default: {
        throw "Unknown text type in setTypeOnSelection";
    }

    }

    cursor.setCharFormat(newFormat);
    cursor.clearSelection();

}


void TextBlockEdit::undo()
{
    this->QTextEdit::undo();
}
void TextBlockEdit::redo()
{
    this->QTextEdit::redo();
}
void TextBlockEdit::clearUndoStack()
{
    this->document()->clearUndoRedoStacks();
}

void TextBlockEdit::mousePressEvent(QMouseEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        QString anchor = anchorAt(event->pos());
        qDebug() << anchor;
        QUrl url(anchor);

        QDesktopServices::openUrl(url);
        QTextEdit::mousePressEvent(event);
    }
    else
    {
        QTextEdit::mousePressEvent(event);
    }
}

void TextBlockEdit::focusOutEvent(QFocusEvent* event)
{
    QWidget* focus_widget = QApplication::focusWidget();
    qDebug() << "focused widget:" << focus_widget;
    if (focus_widget && focus_widget !=this)
    {
        QTextCursor cursor = textCursor();
        if (cursor.hasSelection())
        {
            cursor.clearSelection();
        }
        cursor.setPosition(0);
        setTextCursor(cursor);
    }
    QTextEdit::focusOutEvent(event);
}




void TextBlockEdit::keyPressEvent(QKeyEvent* event) {
    if ((event->key() == Qt::Key_C || event->key() == Qt::Key_X) && (event->modifiers() & Qt::ControlModifier)) {
        QTextEdit::keyPressEvent(event);
        QClipboard* clipboard = QApplication::clipboard();
        local_clipboard = clipboard->mimeData()->html();

    }
    else if (event->key() == Qt::Key_V && (event->modifiers() & Qt::ControlModifier)) {
        // 拦截 Ctrl+V 粘贴
        QClipboard* clipboard = QApplication::clipboard();
        const QMimeData* mimeData = clipboard->mimeData();
        if (mimeData->hasHtml() && mimeData->html() != local_clipboard)
        {
            QString plainText = mimeData->text();
            insertPlainText(plainText);
            return;
        }
        else
        {
            QTextEdit::keyPressEvent(event);
        }
    }
    else
    {
        QTextEdit::keyPressEvent(event);
    }
    
    
}