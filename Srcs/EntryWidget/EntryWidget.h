#pragma once
/*
	继承自QWidget，用于显示词条的内容
*/

#include <QWidget>
#include <QTextEdit>
#include "../MainWindow/KMMainWindow.h"
#include "../EntryWidget/EntryArea.h"

// 只负责打开已有的词条，不负责创建新的词条
class EntryWidget : public QWidget
{
	Q_OBJECT

private:
	bool is_saved;  // 记录当前词条是否已经保存，用于加快保存进度
	int entry_id;  // 词条的id
	EntryArea* entry_area;  // 词条的编辑区
	KMMainWindow* main_window;  // 主窗口

public:
	// 构造词条编辑器窗口，用于打开词条，当entry_id对应的文件夹不存在将初始化一个entry_id对应的文件夹
	static EntryWidget* construct(int _entry_id, QWidget* parent, KMMainWindow* main_window);
	EntryWidget(const EntryWidget&) = delete;
	EntryWidget& operator=(const EntryWidget&) = delete;
	~EntryWidget();

	int getEntryId() const;

	void handleContentChange();

	bool saveEntry();

	EntryArea* getEntryArea() const {
		return entry_area;
	}

private:
	explicit EntryWidget(int entry_id, QWidget* parent, KMMainWindow* main_window);
};