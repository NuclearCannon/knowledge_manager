#include "TextBlockWidget.h"

const int FORMAT_IS_CODE = 1000;  // 只要是个比较大的数字就行
const int FORMAT_IS_LINK = 1001;

int createFormatCode(bool bold, bool italic, bool underline, bool strike)
{
    int format_code = 0;
    if (bold)format_code |= 0b1000;
    if (italic)format_code |= 0b100;
    if (underline)format_code |= 0b10;
    if (strike)format_code |= 0b1;
    return format_code;
}

void decodeFormatCode(int format_code, bool& bold, bool& italic, bool& underline, bool& strike)
{
    strike = format_code & 0b1;
    underline = format_code & 0b10;
    italic = format_code & 0b100;
    bold = format_code & 0b1000;
}



TextType getQTextCharFormatType(const QTextCharFormat& format)
{

    if (format.hasProperty(FORMAT_IS_CODE))return TextType::code;
    if (format.hasProperty(FORMAT_IS_LINK))return TextType::link;
    return TextType::normal;

}


QTextCharFormat normalFormat(bool bold, bool italic, bool underline, bool strike)
{
    QTextCharFormat format;
    if (bold)format.setFontWeight(QFont::Bold);
    format.setFontItalic(italic);
    format.setFontUnderline(underline);
    format.setFontStrikeOut(strike);
    return format;
}

QTextCharFormat codeFormat()
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

QTextCharFormat linkFormat(const QString& href)
{
    QTextCharFormat link_format;
    link_format.setAnchor(true);
    link_format.setAnchorHref(href);
    link_format.setForeground(QColor(Qt::blue));
    link_format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    link_format.setProperty(FORMAT_IS_LINK, true);
    return link_format;
}