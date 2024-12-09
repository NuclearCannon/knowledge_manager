#include "TextBlockWidget.h"
#include <QBoxLayout>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QDialogButtonBox>
#include <QApplication>
#include <QProcess>
#include <QDesktopServices>


TextBlockWidget::TextBlockWidget(QWidget* parent) :
    BlockWidget(parent), text_edit(0)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    text_edit = new TextBlockEdit(this);
    qDebug() << text_edit->wordWrapMode() << text_edit->lineWrapMode();
    layout->addWidget(text_edit);
    setLayout(layout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 连接更改信号
    connect(text_edit, &TextBlockEdit::textChanged, this, &TextBlockWidget::emitContentChange);
    connect(text_edit, &TextBlockEdit::textChanged, this, &TextBlockWidget::justifyHeight);
    connect(text_edit, &TextBlockEdit::contextMenuQuery, this, &TextBlockWidget::handleContextMenuQueryFromControls);

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
    int height = text_edit->document()->size().rheight() + 5;
    text_edit->setFixedHeight(height);
    setFixedHeight(height);
}



void TextBlockWidget::exportToQtXml(QDomElement& dest, QDomDocument& dom_doc)
{


    QTextDocument* doc = this->text_edit->document();
    QTextBlock block = doc->begin();

    dest.setTagName(QStringLiteral("text-block"));
    bool first = true;
    while (block.isValid())
    {
        if (first)
        {
            first = false;
        }
        else
        {
            dest.appendChild(dom_doc.createElement("br"));
        }
        for (QTextBlock::Iterator iter = block.begin(); !iter.atEnd(); iter++)
        {
            QTextFragment fragment = iter.fragment();
            QString text = fragment.text();
            if (text == "\n")
            {
                qWarning() << "pure newline appear";
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
        
    }
    return;
}


void TextBlockWidget::importFromQtXml(QDomElement& src)
{
    text_edit->clear();
    QTextCursor cursor = text_edit->textCursor();
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

void TextBlockWidget::clearUndoStack()
{
    text_edit->clearUndoStack();
}