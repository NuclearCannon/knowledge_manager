#pragma once
#include <QtWidgets/QMainWindow>
#include <QListWidget>
#include <QPoint>

#include "ui_KMMainWindow.h"
#include "MetaData.h"

class EntryWidget;

class KMMainWindow : public QMainWindow
{
    Q_OBJECT

private:
	Ui::kmuiClass ui;
	QString kl_name;  // 知识库名称
	QString original_kl_path;  // 知识库的km文件存放的路径，只有在压缩解压和显示库信息时用到，末尾带.km
	QString temp_kl_path;  // 知识库临时路径（解压后的路径），实际中用到的全是这个路径，末尾不带.km
	MetaData meta_data;  // 知识库的元数据
	int is_saved;  // 记录当前库是否已经保存，用于MainWindow标题的改变和关闭时询问
	int is_temp_kl;  // 记录当前库是否是临时库，用于关闭时询问以及保存时的判断

public:
	// 静态函数，用来构造一个新的KMMainWindow窗口，禁止直接构造，参数kl_path是知识库的路径，带.km后缀
	static KMMainWindow* construct(QString kl_path);
	// 静态函数，用来构造一个新的KMMainWindow窗口，禁止直接构造，此函数用于建立临时知识库（即刚打开软件时的知识库）
	static KMMainWindow* construct();

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
	MetaData& getMetaData();

	//void addTab(QWidget* widget, const QString& label);//新加
	void onUpdateTabWidget(const QString& klName, const QString& klPath, QWidget* widget, const QString& fileName);//搜索完打开词条函数

	// 根据entry_id，跳转到对应的词条，如果词条已经打开，则直接跳转，否则打开词条
	// 返回是否成功跳转
	bool openEntry(int entry_id);

	// 打开知识库函数，kl_path带.km后缀，该函数遇到错误会弹出错误对话框，返回是否成功打开
	// 如果是临时知识库且没有保存，则在这个MainWindow打开，否则新建一个窗口打开
	bool openKnowledgeLibrary(const QString& open_kl_path);  

	// 获得tab_widget当前的EntryWidget*，如果没有则返回nullptr
	EntryWidget* getCurrentEntryWidget();

	// 添加标签，这是给TagManager调用的
	void addTagToCurrentEntry(int tag_id);

	// 新建知识库的帮助函数，参数是知识库的路径，返回是否成功新建，路径是带.km后缀的
	bool createNewKnowledgeLibraryHelper(QString file_path);

	// 焦点过滤器
	bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
	// 文件菜单相关的槽函数
	void actCreateEntry();  // 点击新建词条时，创建一个新的词条并加载到tab
	void actTabCloseRequested(int index);  // 点击tab关闭按钮时，关闭特定的tab
	void actDeleteEntry();  // 点击删除词条时，询问并删除当前词条，同时删除对应的tab
	void actSetCurrentEntryAsAnchor();  // 设置当前词条为锚点
	void actSaveKL();  // 点击保存库时，保存当前库
	void actCreateNewKnowledgeLibrary();  // 点击新建知识库时，新建一个空白知识库
	void actOpenKnowledgeLibrary();  // 点击打开知识库
	void actRecentKnowledgeLibrary();  // 点击最近打开的知识库时，弹出最近打开的知识库列表

	// 编辑菜单相关的槽函数
	void actAddTextBlock();  // 点击添加文本块时，添加一个文本块
	void actAddCodeBlock();  // 添加一个代码块
	void actAddImageBlock();  // 添加一个图片块
	void actAddHeaderBlock();  // 添加一个标题块
	void actUndo();
	void actRedo();


	// 样式部分槽函数
	void actBold();
	void actItalic();
	void actStrike();
	void actUnderline();
	void actRemoveBold();
	void actRemoveItalic();
	void actRemoveStrike();
	void actRemoveUnderline();
	void actSetTypeCode();
	void actSetTypeLink();
	void actSetTypeNormal();

	// 标签部分槽函数
	void actManageTag();  // 管理标签

	// 搜索部分槽函数
	void actSearchEntry();//搜索词条打开函数
	//void actSearchkl();//搜索库打开函数

	// 锚点、关联标签、标签、大纲 槽函数
	void relatedEntriedButtonClicked();  // 关联词条
	void relatedEntryItemClicked(QListWidgetItem* item);  // 左键关联词条中的条目，跳转到对应的词条
	void relatedEntryItemRightClicked(const QPoint& pos);  // 右键关联词条中的条目，弹出菜单，指出词条的菜单包括 跳转、删除，指入词条的菜单包括 跳转
	void anchorButtonClicked();  // 锚点
	void anchorItemClicked(QListWidgetItem* item);  // 左键锚点中的条目，跳转到对应的词条
	void anchorItemRightClicked(const QPoint& pos);  // 右键锚点中的条目，弹出菜单，包括 跳转、删除
	void synopsisButtonClicked();  // 大纲
	void refreshSynopsis();  // 处于大纲tab时，刷新大纲，设置的目的是响应 EntryArea::titleChange 信号，防止大纲更新时从其他标签切换到大纲
	void synopsisItemClicked(QListWidgetItem* item);  // 左键大纲中的条目，跳转到对应的词条
	void tagButtonClicked();  // 标签
	void tagItemClicked(QListWidgetItem* item);  // 左键点击后标签取消选中
	void tagItemRightClicked(const QPoint& pos);  // 右键标签中的条目，弹出菜单，包括 删除

	// 词条tab相关的槽函数
	void tabWidgetChanged(int index);  // tab改变时，更新锚点、关联词条、大纲、标签
	void showTabContextMenu(const QPoint& pos);  // 右键tab，弹出菜单，包括 关闭，关闭其他，删除，修改名称

	// 知识库发生变化时，更新MainWindow的标题（在知识库名称后面加" *"）
	void handleKLChanged();  

	// 指出指入
	void addPointOut();


signals:
	void klChanged();  // 知识库发生变化时，发出信号

protected:
	void closeEvent(QCloseEvent* event) override;  // 关闭时，询问未保存的词条，从 current_kl_list 中删除当前库

private:
	explicit KMMainWindow(bool temp_kl, QString _kl_name, QString _original_kl_path, QString _temp_kl_path);

	// 构造KMMainWindow的辅助函数，用于加载库：解压original_kl_path到temp_kl_path，加载meta_data
	bool initialize();

	// 添加一个tab，返回是否添加成功，不要直接调用ui.tab_widget->addTab
	bool addEntryToTab(EntryWidget* entry_widget, const QString& entry_title);
};
