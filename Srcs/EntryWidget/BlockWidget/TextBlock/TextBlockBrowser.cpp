#include "TextBlockWidget.h"
#include <QApplication>
#include <QDesktopServices>
#include <QThread>


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
}

void TextBlockBrowser::setStyleOnSelection(FormatItem x, bool value)
{
    // 获取QTextEdit的当前文本光标  
    QTextCursor cursor = textCursor();

    // 检查是否有选中的文本，如果没有选中的文本，则不执行任何操作  
    if (!cursor.hasSelection())return;

    int start = cursor.selectionStart(), end = cursor.selectionEnd();
    cursor.clearSelection();
    cursor.setPosition(start);
    cursor.clearSelection();
    while (cursor.position() != end)
    {

        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        QTextCharFormat currentFormat = cursor.charFormat();
        // 代码、链接和库内链接不可以被设置格式
        if (getQTextCharFormatType(currentFormat) == TextType::normal)
        {
            QTextCharFormat newFormat = currentFormat;
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
            default:
                throw "Unknown FormatItem in setStyleOnSelection";
                break;

            }
            cursor.mergeCharFormat(newFormat);

        }
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

