#pragma once

#include "BlockWidget.h"
#include <QTextBrowser>
#include <QDialog>
#include <QLineEdit>

class TextBlockWidget;


enum class FormatItem
{
    bold, italic, underline, strike
};

enum TextType
{
    normal, code, link
};




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

class TextBlockBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    TextBlockBrowser(TextBlockWidget* parent);
    void setStyleOnSelection(FormatItem x, bool value = true);
    void setTypeOnSelection(TextType type);
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
private slots:
    void justifyHeight();
    void onAnchorClicked(const QUrl& url);
};

