#include "TextBlockWidget.h"
#include <QBoxLayout>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QDialogButtonBox>
#include <QApplication>
#include <QProcess>


void openUrlInDefaultBrowser(const QUrl& url) {
    // 将QUrl转换为QString
    QString urlString = url.toString();

    // 构建命令行指令
    QString command = QString("start %1").arg(urlString);

    // 使用QProcess执行命令
    QProcess process;
    process.start(command);

    // 检查进程是否成功启动
    if (!process.waitForStarted()) {
        qWarning() << "Failed to start process:" << process.errorString();
    }
    else {
        qDebug() << "URL opened in default browser.";
    }
}




TextBlockBrowser::TextBlockBrowser(TextBlockWidget* parent) :
    QTextBrowser(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setReadOnly(false);
    setOpenLinks(false);
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

TextBlockWidget::TextBlockWidget(QWidget* parent) :
    BlockWidget(parent), text_browser(0)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    text_browser = new TextBlockBrowser(this);
    layout->addWidget(text_browser);
    setLayout(layout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 安装事件过滤器
    text_browser->installEventFilter(filter);
    this->installEventFilter(filter);

    // 连接更改信号
    connect(text_browser, &TextBlockBrowser::textChanged, this, &TextBlockWidget::emitContentChange);
    connect(text_browser, &TextBlockBrowser::textChanged, this, &TextBlockWidget::justifyHeight);
    connect(text_browser, &TextBlockBrowser::anchorClicked, this, &TextBlockWidget::onAnchorClicked);

    justifyHeight();// 调整一次高度

    qDebug() << "TextBlockWidget 构造时尺寸" << size();
}

TextBlockWidget::~TextBlockWidget() = default;


BlockType TextBlockWidget::type() const
{
    return BlockType::Text;
}


void TextBlockWidget::justifyHeight()
{
    
    text_browser->document()->adjustSize();//将文档调整到合理的大小。
    int height = text_browser->document()->size().rheight() + 5;
    text_browser->setFixedHeight(height);
    setFixedHeight(height);
}

void TextBlockWidget::onAnchorClicked(const QUrl& url)
{
    
    qDebug() << "onAnchorClicked(link), href=" << url.toString();
    if (QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        openUrlInDefaultBrowser(url);
    }
    else
    {
        // do nothing
        qDebug() << "onAnchorClicked blocked because ctrl is not pressed";
    }
    
}

void TextBlockWidget::exportToQtXml(QDomElement& dest, QDomDocument& dom_doc)
{


    QTextDocument* doc = this->text_browser->document();
    QTextBlock block = doc->begin();

    dest.setTagName(QStringLiteral("text-block"));

    while (block.isValid())
    {
        for (QTextBlock::Iterator iter = block.begin(); !iter.atEnd(); iter++)
        {
            QTextFragment fragment = iter.fragment();
            QString text = fragment.text();
            if (text == "\n")
            {
                dest.appendChild(dom_doc.createElement("br"));
                continue;
            }
            QTextCharFormat format = fragment.charFormat();
            bool bold = 0, italic = 0, underline = 0, strike = 0;
            QDomElement new_node;

            switch (getQTextCharFormatType(format))
            {
            case TextType::code:

                new_node = dom_doc.createElement("code");
                dest.appendChild(new_node);
                break;
            case TextType::link:
                new_node = dom_doc.createElement("link");
                dest.appendChild(new_node);
                new_node.setAttribute("href", format.anchorHref());
                break;

            default:
                bold = format.fontWeight() == QFont::Bold;
                italic = format.fontItalic();
                underline = format.fontUnderline();
                strike = format.fontStrikeOut();
                new_node = dom_doc.createElement("text");
                new_node.setAttribute("format-code", createFormatCode(bold, italic, underline, strike));
                break;

            }
            new_node.appendChild(dom_doc.createTextNode(text));
            dest.appendChild(new_node);
        }
        block = block.next();
        dest.appendChild(dom_doc.createElement("br"));
    }
    return;
}


void TextBlockWidget::importFromQtXml(QDomElement& src)
{
    text_browser->clear();
    QTextCursor cursor = text_browser->textCursor();
    for (QDomNode child = src.firstChild(); !child.isNull(); child = child.nextSibling())
    {
        QDomElement elem = child.toElement();
        assert(!elem.isNull());

        if (elem.tagName() == "br")
        {
            cursor.insertText("\n");
        }
        else
        {
            QDomText text_node = elem.firstChild().toText();
            assert(!text_node.isNull());
            QString text = text_node.data();
            QString name = elem.tagName();

            if (name == "text")
            {
                int format_code = elem.attribute("format-code").toInt();
                bool bold, italic, underline, strike;
                decodeFormatCode(format_code, bold, italic, underline, strike);
                cursor.insertText(text,normalFormat(bold, italic, underline, strike)
                );
            }
            else if (name == "code")
            {
                cursor.insertText(text,codeFormat());
            }
            else if (name == "link")
            {
                cursor.insertText(text,linkFormat(elem.attribute("href")));
            }
            else
            {
                qDebug() << "Unknown child.name() = " << name;

            }
        }

        
    }
}