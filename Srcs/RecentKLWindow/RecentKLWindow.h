#pragma once
#ifndef RecentKLWindow_H
#define RecentKLWindow_H

#include <QWidget>
#include "ui_RecentKLWindow.h"
#include "../MainWindow/KMMainWindow.h"

class RecentKLWindow : public QWidget
{
    Q_OBJECT

public:
	// 构造一个新的RecentKLWindow，禁止直接构造
	static RecentKLWindow* construct(QWidget* parent, KMMainWindow* _main_window);
	RecentKLWindow(const RecentKLWindow&) = delete;
	RecentKLWindow& operator=(const RecentKLWindow&) = delete;
    ~RecentKLWindow();

public slots:
	void listWidgetItemClicked(QListWidgetItem* item);  // 点击listWidget中的item，就进入到这个item对应的知识库
    void searchButtonClicked();  // 点击搜索库文件
	void checkSearchLineEdit();  // 搜索框被修改后，检查搜索框内的文本是否是空，如果是空则加载历史
	void sortButtonClicked();  // 点击排序按钮，对listWidget中的item进行字典序排序
	
private:
	explicit RecentKLWindow(QWidget* parent, KMMainWindow* _main_window);
	bool refreshListWidget();  // 刷新listWidget

private:
    Ui::recent_kl_widget ui;
	KMMainWindow* main_window;
};

#endif // RecentKLWindow_H