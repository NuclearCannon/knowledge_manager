﻿#pragma once

#include <QTextEdit>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QComboBox>
#include "BlockWidget.h"
#include "../pugixml/pugixml.hpp"

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

class CodeEdit : public QTextEdit
{
    Q_OBJECT
public:
    CodeEdit(CodeBlockWidget* parent = nullptr);
    ~CodeEdit();
    CodeBlockWidget* toBlockParent() const;
    void setHighlighter(const QVector<HighlightingRule>& rules);
    void removeHighlighter();
protected:
    void keyPressEvent(QKeyEvent* event) override;
private:
    CodeBlockWidget* const parent_ptr;
    CodeBlockHighlighter* highlighter;
};



class LanguageComboBox : public QComboBox
{
    Q_OBJECT
public:
    LanguageComboBox(CodeBlockWidget* parent);

    
};


class CodeBlockWidget : public BlockWidget  // 代码块窗口
{
    Q_OBJECT
public:
    // 构造函数
    CodeBlockWidget(QWidget* parent);
    virtual ~CodeBlockWidget();
    virtual void exportToPugi(pugi::xml_node& dest);
    virtual void importFromPugi(const pugi::xml_node& node);
    virtual BlockType type() const;
private:
    CodeEdit* code_edit;
    LanguageComboBox* languange_box;
private slots:
    void updateHighlighter();  // 根据当前languange_box的选择更新highlighter
public slots:
    void justifyHeight();
};

