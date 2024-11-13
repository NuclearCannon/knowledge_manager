#pragma once

#include "../BlockWidget.h"
#include <QTextBrowser>
#include <QDialog>
#include <QLineEdit>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>


#include "TextBlockWidget.h"

extern const int FORMAT_IS_CODE;  // 只要是个比较大的数字就行
extern const int FORMAT_IS_LINK;


class TextBlockWidget;


enum class FormatItem
{
    bold, italic, underline, strike
};

enum TextType
{
    normal, code, link
};

int createFormatCode(bool bold, bool italic, bool underline, bool strike);
void decodeFormatCode(int format_code, bool& bold, bool& italic, bool& underline, bool& strike);
TextType getQTextCharFormatType(const QTextCharFormat& format);
QTextCharFormat normalFormat(bool bold, bool italic, bool underline, bool strike);
QTextCharFormat codeFormat();
QTextCharFormat linkFormat(const QString& href);


// 自定义对话框类  
class InputUrlDialog : public QDialog {
    Q_OBJECT

public:
    InputUrlDialog(QWidget* parent, const QString& text);
    static bool getStringByDialog(const QString& text, QString& dest);
protected:
    void accept() override;
    void reject() override;
private:
    QLineEdit* lineEdit;
    bool resultAccepted = false;
};

class TextBlockBrowser : public QTextBrowser, public BlockControl
{
    Q_OBJECT
public:
    TextBlockBrowser(TextBlockWidget* parent);
    void setStyleOnSelection(FormatItem x, bool value = true);
    void setTypeOnSelection(TextType type);
    void undo();
    void redo();
    void clearUndoStack();
};


class TextBlockWidget : public BlockWidget
{
	Q_OBJECT
private:
	TextBlockBrowser* text_browser;
public:
    TextBlockWidget(QWidget* parent);
    virtual ~TextBlockWidget();
    virtual BlockType type() const;
    virtual void exportToPugi(pugi::xml_node& dest);
    virtual void importFromPugi(const pugi::xml_node& node);

    virtual void exportToQtXml(QDomElement& dest, QDomDocument& dom_doc);
    virtual void importFromQtXml(QDomElement& src);

private slots:
    void justifyHeight();
    void onAnchorClicked(const QUrl& url);
};

