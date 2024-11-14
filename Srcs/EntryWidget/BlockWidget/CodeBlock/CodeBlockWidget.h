#pragma once

#include <QTextEdit>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QComboBox>
#include "../BlockWidget.h"
#include <stack>


class CodeBlockWidget;

// 该文件定义代码块及其需要的语法高亮器
struct HighlightingRule
{
    QRegularExpression pattern;
    QTextCharFormat format;
};

class HighlightingRules
{
private:
    QVector<HighlightingRule> cppRules;
    QVector<HighlightingRule> pythonRules;
    QVector<HighlightingRule> htmlRules;
    static HighlightingRules highlighting_rules;
    void initCppRules();
    void initPythonRules();
    void initHtmlRule();
    HighlightingRules();
public:
    static const QVector<HighlightingRule>& getCppRules();
    static const QVector<HighlightingRule>& getPythonRules();
    static const QVector<HighlightingRule>& getHtmlRules();
};


class CodeBlockHighlighter : public QSyntaxHighlighter  // 代码语法高亮器（原理测试）
{
    Q_OBJECT
public:
    const QVector<HighlightingRule>& rules;
    CodeBlockHighlighter(QTextDocument* parent, const QVector<HighlightingRule>& rules);
    void highlightBlock(const QString& text);
};

class CodeEdit : public QTextEdit, public BlockControl
{
    Q_OBJECT
public:
    CodeEdit(CodeBlockWidget* parent = nullptr);
    ~CodeEdit();
    CodeBlockWidget* toBlockParent() const;
    void setHighlighter(const QVector<HighlightingRule>& rules);
    void removeHighlighter();
    void undo();
    void redo();
    void clearUndoStack();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event);
private:
    CodeBlockWidget* const parent_ptr;
    CodeBlockHighlighter* highlighter;
signals:
    void contextMenuQuery(QContextMenuEvent*);
};



class LanguageComboBox : public QComboBox, public BlockControl
{
    Q_OBJECT

private:
    std::stack<int> undo_stack, redo_stack;
    int current_index;
public:
    LanguageComboBox(CodeBlockWidget* parent);
    void undo();  // 不会触发currendIndexChanged, 而会触发languageBoxUndoRedo
    void redo();  // 不会触发currendIndexChanged, 而会触发languageBoxUndoRedo
    void clearUndoStack();
private:
    void clearUndoStackOnly();
    void contextMenuEvent(QContextMenuEvent* event);
private slots:
    void clearRedoStackOnly();
    void undoRecord();
signals:
    void languageBoxUndoRedo();
signals:
    void contextMenuQuery(QContextMenuEvent*);
};


class CodeBlockWidget : public BlockWidget  // 代码块窗口
{
    Q_OBJECT
public:
    // 构造函数
    CodeBlockWidget(QWidget* parent);
    virtual ~CodeBlockWidget();
    virtual void exportToQtXml(QDomElement& dest, QDomDocument& dom_doc);
    virtual void importFromQtXml(QDomElement& src);
    virtual BlockType type() const;
private:
    CodeEdit* code_edit;
    LanguageComboBox* languange_box;
private slots:
    void updateHighlighter();  // 根据当前languange_box的选择更新highlighter
    void handleLanguageChanged();

public slots:
    //void handleContextMenuQueryFromControls(QContextMenuEvent* event);
    void justifyHeight();
    void handleUndoEdit();
    void handleRedoEdit();
    void handleUndoBox();
    void handleRedoBox();
protected:
    void contextMenuEvent(QContextMenuEvent* event);
};

