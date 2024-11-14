#include "TextBlockWidget.h"
#include <QBoxLayout>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QDialogButtonBox>
#include <QApplication>
#include <QProcess>
#include <QDesktopServices>


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
    connect(text_browser, &TextBlockBrowser::contextMenuQuery, this, &TextBlockWidget::handleContextMenuQueryFromControls);

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

