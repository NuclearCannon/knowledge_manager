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

class TextBlockEdit : public QTextEdit, public BlockControl
{
    Q_OBJECT
private:

public:
    TextBlockEdit(TextBlockWidget* parent);
    void setStyleOnSelection(FormatItem x, bool value = true);
    void setTypeOnSelection(TextType type);
    void undo();
    void redo();
    void clearUndoStack();
protected:
    void mousePressEvent(QMouseEvent* event);
    void contextMenuEvent(QContextMenuEvent* event);
    void focusOutEvent(QFocusEvent* event);
    void keyPressEvent(QKeyEvent* event) override;
signals:
    void contextMenuQuery(QContextMenuEvent*);

};


class TextBlockWidget : public BlockWidget
{
	Q_OBJECT
private:
	TextBlockEdit* text_edit;
public:
    TextBlockWidget(QWidget* parent);
    virtual ~TextBlockWidget();
    virtual BlockType type() const;

    void clearUndoStack();
    virtual void exportToQtXml(QDomElement& dest, QDomDocument& dom_doc);
    virtual void importFromQtXml(QDomElement& src);

private slots:
    void justifyHeight();
    void handleContextMenuUndo();
    void handleContextMenuRedo();

protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void resizeEvent(QResizeEvent* event) override;
};

