#include "TextBlockWidget.h"
#include <QBoxLayout>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QDialogButtonBox>
#include <QLabel>
#include <QApplication>
#include <QProcess>
static const int FORMAT_IS_CODE = 1000;  // 只要是个比较大的数字就行
static const int FORMAT_IS_LINK = 1001;


// 这些静态函数仅用于本文件内
static int createFormatCode(bool bold, bool italic, bool underline, bool strike)
{
    int format_code = 0;
    if (bold)format_code |= 0b1000;
    if (italic)format_code |= 0b100;
    if (underline)format_code |= 0b10;
    if (strike)format_code |= 0b1;
    return format_code;
}

static void decodeFormatCode(int format_code, bool& bold, bool& italic, bool& underline, bool& strike)
{
    strike = format_code & 0b1;
    underline = format_code & 0b10;
    italic = format_code & 0b100;
    bold = format_code & 0b1000;
}



static TextType getQTextCharFormatType(const QTextCharFormat& format)
{

    if(format.hasProperty(FORMAT_IS_CODE))return TextType::code;
    if(format.hasProperty(FORMAT_IS_LINK))return TextType::link;
    return TextType::normal;

}


static QTextCharFormat normalFormat(bool bold, bool italic, bool underline, bool strike)
{
    QTextCharFormat format;
    if (bold)format.setFontWeight(QFont::Bold);
    format.setFontItalic(italic);
    format.setFontUnderline(underline);
    format.setFontStrikeOut(strike);
    return format;
}

static QTextCharFormat codeFormat()
{
    QTextCharFormat code_format;
    // 设置字体  
    QFont font;
    font.setFamily("Courier New"); // 使用等宽字体来模拟代码风格  
    font.setFixedPitch(true);       // 确保字体是等宽的  
    font.setPointSize(10);          // 设置字体大小  
    code_format.setFont(font); 
    QColor codeColor(0, 0, 128);
    code_format.setForeground(codeColor);
    code_format.setProperty(FORMAT_IS_CODE, true);
    return code_format;
}

static QTextCharFormat linkFormat(const QString& href)
{
    QTextCharFormat link_format;
    link_format.setAnchor(true);
    link_format.setAnchorHref(href);
    link_format.setForeground(QColor(Qt::blue));
    link_format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    link_format.setProperty(FORMAT_IS_LINK, true);
    return link_format;
}

static void openUrlInDefaultBrowser(const QUrl& url) {
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

InputUrlDialog::InputUrlDialog(QWidget* parent, const QString& text) : QDialog(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* label = new QLabel(text, this);
    lineEdit = new QLineEdit(this);

    layout->addWidget(label);
    layout->addWidget(lineEdit);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &InputUrlDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &InputUrlDialog::reject);

    layout->addWidget(buttonBox);
}


bool InputUrlDialog::getStringByDialog(const QString& text, QString& dest)
{
    InputUrlDialog dialog(0, text);
    if (dialog.exec() == QDialog::Accepted)
    {
        dest = dialog.lineEdit->text();
        return true;
    }
    else
    {
        return false;
    }
}


void InputUrlDialog::accept()
{
    resultAccepted = true;
    QDialog::accept();
}

void InputUrlDialog::reject()
{
    resultAccepted = false;
    QDialog::reject();
}



TextBlockBrowser::TextBlockBrowser(TextBlockWidget* parent) :
    QTextBrowser(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setReadOnly(false);
    setOpenLinks(false);
    setTabStopDistance(20);
    setStyleSheet("TextBlockBrowser:hover { background-color: #E0E0E0; border: none; }  TextBlockBrowser { background-color: #F3F3F3; border: none; }");
    //setTextCursor()
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

void TextBlockWidget::exportToPugi(pugi::xml_node& dest)
{


    QTextDocument* doc = this->text_browser->document();
    QTextBlock block = doc->begin();


    dest.set_name("text-block");
    while (block.isValid())
    {

        for(QTextBlock::Iterator iter = block.begin(); !iter.atEnd();iter++)
        {
            QTextFragment fragment = iter.fragment();
            QString text = fragment.text();
            if (text == "\n")
            {
                dest.append_child("br");
                continue;
            }
            QTextCharFormat format = fragment.charFormat();
            bool bold = 0, italic = 0, underline = 0, strike = 0;
            pugi::xml_node new_node;

            switch (getQTextCharFormatType(format))
            {
            case TextType::code:
                new_node = dest.append_child("code");
                break;
            case TextType::link:
                new_node = dest.append_child("link");
                new_node.append_attribute("href").set_value(format.anchorHref().toStdString().c_str());
                break;
            
            default:
                bold = format.fontWeight() == QFont::Bold;
                italic = format.fontItalic();
                underline = format.fontUnderline();
                strike = format.fontStrikeOut();
                new_node = dest.append_child("text");
                new_node.append_attribute("format-code").set_value(createFormatCode(bold, italic, underline, strike));
                break;
           
            }
            new_node.text().set(text.toStdString().c_str());
        }
        block = block.next();
        dest.append_child("br");
    }

    return;
}
void TextBlockWidget::importFromPugi(const pugi::xml_node& src)
{
    text_browser->clear();
    QTextCursor cursor = text_browser->textCursor();
    for (pugi::xml_node child = src.first_child(); child; child = child.next_sibling())
    {
        if (std::strcmp(child.name(), "text") == 0)
        {
            int format_code = child.attribute("format-code").as_int();
            bool bold, italic, underline, strike;
            decodeFormatCode(format_code, bold, italic, underline, strike);
            cursor.insertText(
                child.child_value(),
                normalFormat(bold, italic, underline, strike)
            );
        }
        else if (std::strcmp(child.name(), "br") == 0)
        {
            cursor.insertText("\n");
            //qDebug() << "insert newline";
        }
        else if (std::strcmp(child.name(), "code") == 0)
        {
            cursor.insertText(
                child.child_value(),
                codeFormat()
            );
        }
        else if (std::strcmp(child.name(), "link") == 0)
        {
            cursor.insertText(
                child.child_value(),
                linkFormat(child.attribute("href").as_string())
            );
        }
        
        else
        {
            qDebug() << "Unknown child.name() = " << child.name();

        }
    }
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