#include "CodeBlockWidget.h"
//#include "../../EntryArea.h"
//#include <QBoxLayout>
//#include <sstream>

void HighlightingRules::initCppRules()
{
    // 关键字格式：蓝色加粗
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);

    // 构造关键字列表  
    QStringList keywordPatterns;
    keywordPatterns << "\\bif\\b" << "\\belse\\b" << "\\bwhile\\b"
        << "\\bfor\\b" << "\\bint\\b" << "\\bfloat\\b"
        << "\\breturn\\b" << "\\bclass\\b";

    QRegularExpression keywordRule(QString("(\\b(%1)\\b)").arg(keywordPatterns.join("|")));
    cppRules.push_back({ keywordRule, keywordFormat });

    // 设置注释规则  
    QRegularExpression commentRule("//.*");
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::green);
    cppRules.push_back({ commentRule, commentFormat });

    // 设置字符串规则
    QRegularExpression stringRule("\"[^\"]*\"");
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::red);
    cppRules.push_back({ stringRule, stringFormat });
}

void HighlightingRules::initPythonRules()
{
    // 关键字格式：蓝色加粗
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);

    // 构造关键字列表  
    QStringList keywordPatterns;
    keywordPatterns << "\\bif\\b" << "\\belse\\b" << "\\belif\\b" << "\\bwhile\\b"
        << "\\bfor\\b" << "\\bint\\b" << "\\bfloat\\b"
        << "\\breturn\\b" << "\\bclass\\b" << "\\bdef\\b";

    QRegularExpression keywordRule(QString("(\\b(%1)\\b)").arg(keywordPatterns.join("|")));
    pythonRules.push_back({ keywordRule, keywordFormat });

    // 设置注释规则  
    QRegularExpression commentRule("#.*");
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::green);
    pythonRules.push_back({ commentRule, commentFormat });

    // 设置字符串规则
    QRegularExpression stringRule("(\"[^\"]*\")|('[^']*')");
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::red);
    pythonRules.push_back({ stringRule, stringFormat });
}

void HighlightingRules::initHtmlRule()
{
    // 设置格式  
    QTextCharFormat tagFormat;
    tagFormat.setForeground(Qt::blue);
    tagFormat.setFontWeight(QFont::Bold);
    htmlRules.append({ QRegularExpression("<[^>]+>"), tagFormat });

    QTextCharFormat attributeFormat;
    attributeFormat.setForeground(Qt::darkGreen);
    htmlRules.append({ QRegularExpression("\\b[a-zA-Z-]+\\s*=\\s*\"[^\"]*\"\\s*"), attributeFormat });

    QTextCharFormat entityFormat;
    entityFormat.setForeground(Qt::red);
    htmlRules.append({ QRegularExpression("&[a-zA-Z]+;"), entityFormat });
}

HighlightingRules::HighlightingRules()
{
    initCppRules();
    initPythonRules();
    initHtmlRule();
}


const QVector<HighlightingRule>& HighlightingRules::getCppRules()
{
    return highlighting_rules.cppRules;
}

const QVector<HighlightingRule>& HighlightingRules::getPythonRules()
{
    return highlighting_rules.pythonRules;
}

const QVector<HighlightingRule>& HighlightingRules::getHtmlRules()
{
    return highlighting_rules.htmlRules;
}

HighlightingRules HighlightingRules::highlighting_rules = HighlightingRules();



// 代码高亮器

CodeBlockHighlighter::CodeBlockHighlighter(QTextDocument* parent, const QVector<HighlightingRule>& rules)
    : QSyntaxHighlighter(parent), rules(rules)
{


}

void CodeBlockHighlighter::highlightBlock(const QString& text)
{
    for (const HighlightingRule& rule : rules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

}