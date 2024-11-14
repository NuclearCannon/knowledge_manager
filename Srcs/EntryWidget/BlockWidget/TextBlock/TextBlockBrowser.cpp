#include "TextBlockWidget.h"
#include <QApplication>
#include <QDesktopServices>
#include <QThread>
#include <QMessageBox>

//void openUrlInDefaultBrowser(const QUrl& url) {
//    // 将QUrl转换为QString
//    QString urlString = url.toString();
//
//    // 构建命令行指令
//    QString command = QString("start %1").arg(urlString);
//
//    // 使用QProcess执行命令
//    QProcess process;
//    process.start(command);
//
//    // 检查进程是否成功启动
//    if (!process.waitForStarted()) {
//        qWarning() << "Failed to start process:" << process.errorString();
//    }
//    else {
//        qDebug() << "URL opened in default browser.";
//    }
//}




TextBlockBrowser::TextBlockBrowser(TextBlockWidget* parent) :
    QTextBrowser(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setReadOnly(false);
    setOpenLinks(false);
    setOpenExternalLinks(true);
    setTabStopDistance(20);
    setStyleSheet("TextBlockBrowser:hover { background-color: #E0E0E0; border: none; }  TextBlockBrowser { background-color: #F3F3F3; border: none; }");
    setUndoRedoEnabled(true);
}

void TextBlockBrowser::setStyleOnSelection(FormatItem x, bool value)
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

void TextBlockBrowser::setTypeOnSelection(TextType type)
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


void TextBlockBrowser::undo()
{
    this->QTextBrowser::undo();
}
void TextBlockBrowser::redo()
{
    this->QTextBrowser::redo();
}
void TextBlockBrowser::clearUndoStack()
{
    this->document()->clearUndoRedoStacks();
}

void TextBlockBrowser::mousePressEvent(QMouseEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        QString anchor = anchorAt(event->pos());
        qDebug() << anchor;
        QUrl url(anchor);

        QDesktopServices::openUrl(url);
        QTextBrowser::mousePressEvent(event);
    }
    else
    {
        QTextBrowser::mousePressEvent(event);
    }
}

