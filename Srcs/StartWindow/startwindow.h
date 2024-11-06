#pragma once
#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include "ui_startwindow.h"
#include <QWidget>

namespace Ui {
    class StartWindow;
}

class StartWindow : public QWidget
{
    Q_OBJECT

public:
	// 调用构造函数后必须调用refreshListWidget()来刷新listWidget
    explicit StartWindow(QWidget* parent = nullptr);
    ~StartWindow();
	void refreshListWidget();  // 刷新listWidget

public slots:
	void createNewKLButtonClicked();  // 点击创建新的库文件
	void listWidgetItemClicked(QListWidgetItem* item);  // 点击listWidget中的item，就进入到这个item对应的知识库
    void searchButtonClicked();  // 点击搜索库文件
	void checkSearchLineEdit();  // 搜索框被修改后，检查搜索框内的文本是否是空，如果是空则加载历史
	void sortButtonClicked();  // 点击排序按钮，对listWidget中的item进行字典序排序

private:
    Ui::start_window_widget* ui;
};

#endif // STARTWINDOW_H