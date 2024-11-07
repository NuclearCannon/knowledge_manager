#pragma once
#include <QtWidgets/QMainWindow>
#include <QListWidget>
#include <QPoint>

#include "ui_KMMainWindow.h"
#include "MetaData.h"
//#include "../EntryWidget/EntryWidget.h"

class EntryWidget;

// 临时使用的函数，用于复制文件夹？？？
bool copyDirectory(const QString& srcPath, const QString& destPath);

class KMMainWindow : public QMainWindow
{
    Q_OBJECT

private:
	Ui::kmuiClass ui;
	QString kl_name;  // 知识库名称
	QString original_kl_path;  // 知识库的zip文件存放的路径，只有在压缩解压和显示库信息时用到
	QString temp_kl_path;  // 知识库临时路径（解压后的路径），实际中用到的全是这个路径
	MetaData meta_data;  // 知识库的元数据
	int is_saved;  // 记录当前库是否已经保存，用于MainWindow标题的改变和关闭时询问

public:
	// 静态函数，用来构造一个新的KMMainWindow窗口，禁止直接构造
	static KMMainWindow* construct(QString kl_name, QString kl_path, QWidget* parent = nullptr);

	KMMainWindow(const KMMainWindow&) = delete;
	KMMainWindow& operator=(const KMMainWindow&) = delete;

    ~KMMainWindow();

	// 获取知识库的名称
	QString getKLName() const;
	// 获取压缩的知识库的路径
	QString getOriginalKLPath() const;
	// 获取解压的知识库的路径
	QString getTempKLPath() const;

	// 获取元数据
	const MetaData& getMetaData() const;

	//void addTab(QWidget* widget, const QString& label);//新加
	void onUpdateTabWidget(const QString& klName, const QString& klPath, QWidget* widget, const QString& fileName);//搜索完打开词条函数

	// 根据entry_id，跳转到对应的词条，如果词条已经打开，则直接跳转，否则打开词条
	// 返回是否成功跳转
	bool openEntry(int entry_id);

private slots:
	// 文件菜单相关的槽函数
	void actCreateEntry();  // 点击新建文件时，创建一个新的tab
	void acttabCloseRequested(int index);  // 点击tab关闭按钮时，关闭特定的tab
	void actDeleteEntry();  // 点击删除文件时，询问并删除当前词条，同时删除对应的tab
	void actSaveKL();  // 点击保存库时，保存当前库
	void actOpenStartWindow();  // 点击打开启动窗口
	void actSetCurrentEntryAsAnchor();  // 设置当前词条为锚点

	// 编辑菜单相关的槽函数
	void actAddTextBlock();  // 点击添加文本块时，添加一个文本块
	void actAddCodeBlock();  // 添加一个代码块
	void actAddImageBlock();  // 添加一个图片块
	void actAddHeaderBlock();  // 添加一个标题块

	// 标签部分槽函数
	void actSearchLabel();  // 搜索标签
	void actRecentLabel();  // 最近使用的标签
	void actManageLabel();  // 管理标签

	//新加
	void actSearchEntry();//搜索词条打开函数
	void actSearchkl();//搜索库打开函数
	//void actSearchMultikl();//多库搜素打开函数
	//void actopenmergekl();  // 合库
	//void actopenseparatekl();  // 分库

	// 锚点、关联标签、标签、大纲 槽函数
	void relatedEntriedButtonClicked();  // 关联词条
	void relatedEntryItemClicked(QListWidgetItem* item);  // 左键关联词条中的条目，跳转到对应的词条
	void anchorButtonClicked();  // 锚点
	void anchorItemClicked(QListWidgetItem* item);  // 左键锚点中的条目，跳转到对应的词条
	void synopsisButtonClicked();  // 大纲
	void synopsisItemClicked(QListWidgetItem* item);  // 左键大纲中的条目，跳转到对应的词条
	void tagButtonClicked();  // 标签

	// 知识库发生变化时，更新MainWindow的标题（在知识库名称后面加" *"）
	void handleKLChanged();  

signals:
	void klChanged();  // 知识库发生变化时，发出信号

protected:
	void closeEvent(QCloseEvent* event) override;  // 关闭时，询问未保存的词条，从 current_kl_list 中删除当前库

private:
	explicit KMMainWindow(QString _kl_name, QString _kl_path);

	// 构造KMMainWindow的辅助函数，用于加载库
	bool initialize();

	// 添加一个tab，返回是否添加成功，不要直接调用ui.tab_widget->addTab
	bool addEntryToTab(EntryWidget* entry_widget, const QString& entry_title);
};
