#include <QMessageBox>
#include <QFile>
#include <QSplitter>
#include <QCloseEvent>
#include <QDir>
#include <QAbstractButton>

#include "KMMainWindow.h"
#include "../public.h"
#include "../Compress/compression.h"
#include "../EntryWidget/EntryWidget.h"

KMMainWindow* KMMainWindow::construct(QString kl_name, QString kl_path, QWidget* parent)
{
	// 文件检查
	QString _temp_kl_path = default_path_for_temp_kls + "/" + kl_name;
	QDir _temp_kl_dir(_temp_kl_path);
	QDir _original_kl_dir(kl_path);
	if (!_original_kl_dir.exists())  // 如果kl_path不存在，提示
	{
		QMessageBox::warning(parent, "错误", "知识库不存在：" + kl_path);
		return nullptr;
	}
	if (_temp_kl_dir.exists())  // 如果temp_kl_path已经存在，删除
	{
		if (!_temp_kl_dir.removeRecursively())
		{
			QMessageBox::warning(parent, "错误", "无法删除临时知识库：" + _temp_kl_path);
			return nullptr;
		}
	}
	//if (!kl_path.endsWith(kl_name + ".zip"))  // kl_path要以kl_name + ".zip"结尾
	if (!kl_path.endsWith(kl_name))  // kl_path要以kl_name + ".zip"结尾
	{
		QMessageBox::warning(parent, "错误", "知识库路径错误：" + kl_path);
		return nullptr;
	}


	// 添加到 最近打开 的知识库列表
	Status status = addKLToRecentKLList(kl_name, kl_path);
	if (status == Status::Error)
	{
		QMessageBox::warning(parent, "错误", "无法打开或操作文件：" + kl_path);
		return nullptr;
	}
	// 添加到 当前打开 的知识库列表
	status = addKLToCurrentKLList(kl_name, kl_path);
	if (status == Status::Error)
	{
		QMessageBox::warning(parent, "错误", "无法打开或操作文件：" + kl_path);
		return nullptr; 
	}
	else if (status == Status::Failure)
	{
		QMessageBox::warning(parent, "错误", "知识库已经打开\n知识库位置：" + kl_path);
		return nullptr;
	}

	KMMainWindow* km = new KMMainWindow(kl_name, kl_path);
	
	if (!km->initialize())
	{
		// 失败了要回滚
		status = removeKLFromRecentKLList(kl_name, kl_path);
		if (status == Status::Error) QMessageBox::warning(parent, "错误", "无法打开或操作文件：" + kl_path);
		removeKLFromCurrentKLList(kl_name, kl_path);

		delete km;
		return nullptr;
	}
	return km;
}

// 临时使用的函数，用于复制文件夹？？？
bool copyDirectory(const QString& srcPath, const QString& destPath) {
	QDir srcDir(srcPath);
	if (!srcDir.exists())
		return false;

	QDir destDir(destPath);
	if (!destDir.exists()) {
		destDir.mkpath(destPath);
	}

	foreach(QString fileName, srcDir.entryList(QDir::Files)) {
		QString srcFilePath = srcPath + QDir::separator() + fileName;
		QString destFilePath = destPath + QDir::separator() + fileName;
		QFile::copy(srcFilePath, destFilePath);
	}

	foreach(QString subDirName, srcDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
		QString srcSubDirPath = srcPath + QDir::separator() + subDirName;
		QString destSubDirPath = destPath + QDir::separator() + subDirName;
		copyDirectory(srcSubDirPath, destSubDirPath);
	}

	return true;
}

bool KMMainWindow::initialize()
{
	// 从original_kl_path中解压库文件到temp_kl_path
	QDir original_kl_dir(original_kl_path);
	QDir temp_kl_dir(temp_kl_path);

	// 解压
	//if (!decompress_zip(original_kl_path.toStdString(), temp_kl_path.toStdString()))
	//{
	//	QMessageBox::warning(this, "错误", "解压知识库失败：" + original_kl_path);
	//	return false;
	//}

	// 先用复制？？？
	if (!copyDirectory(original_kl_path, temp_kl_path))
	{
		QMessageBox::warning(this, "错误", "复制知识库失败：" + original_kl_path);
		return false;
	}

	QFile meta_data_file(temp_kl_path + "/meta_data.xml");
	if (!meta_data_file.exists())
	{
		QMessageBox::warning(this, "错误", "知识库元数据文件不存在：" + temp_kl_path + "/meta_data.xml");
		return false;
	}
	if (!meta_data_file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox::warning(this, "错误", "打开meta_data.xml失败：" + meta_data_file.errorString());
		return false;
	}
	meta_data.load(meta_data_file);

	meta_data_file.close();

	return true;
}

KMMainWindow::KMMainWindow(QString _kl_name, QString _kl_path)
	: QMainWindow(nullptr), kl_name(_kl_name), original_kl_path(_kl_path), is_saved(true)
{
	temp_kl_path = default_path_for_temp_kls + "/" + kl_name;

	ui.setupUi(this);

	// 设置窗口的大小
	this->resize(1200, 600);

	// 设置窗口的标题
	this->setWindowTitle("km-" + kl_name);

	// 刚进入程序时，清空tab_widget
	ui.tab_widget->clear();

	// 设置分割，一共有两个部分，左边是词条列表，中间是词条内容
	QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
	// 设置水平布局的间隙
	splitter->setContentsMargins(0, 0, 0, 0);

	splitter->addWidget(ui.left_part);
	splitter->addWidget(ui.tab_widget);
	splitter->setHandleWidth(1);
	splitter->setStyleSheet("QSplitter::handle{background-color: rgb(200, 200, 200);}");
	// 设置两个部分不能覆盖任何部分
	splitter->setCollapsible(0, true);
	splitter->setCollapsible(1, false);
	//splitter->setCollapsible(2, true);
	// 设置两部分的大小，左边占250px，右边占950px
	splitter->setSizes({ 250, 950 });

	setCentralWidget(splitter);

	// 关联词条
	QWidget* related_entries_widget = new QWidget(ui.left_tab_widget);

	QLabel* out_label = new QLabel(related_entries_widget);
	out_label->setText("指出的词条");
	QListWidget* out_entries_list = new QListWidget(related_entries_widget);
	
	QLabel* in_label = new QLabel(related_entries_widget);
	in_label->setText("指入的词条");
	QListWidget* in_entries_list = new QListWidget(related_entries_widget);

	QHBoxLayout* layout1 = new QHBoxLayout();
	layout1->addWidget(out_label);
	layout1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	QHBoxLayout* layout2 = new QHBoxLayout();
	layout2->addWidget(in_label);
	layout2->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addLayout(layout1);
	layout->addWidget(out_entries_list);
	layout->addLayout(layout2);
	layout->addWidget(in_entries_list);
	related_entries_widget->setLayout(layout);
	// 设置layout的间隙
	layout->setContentsMargins(0, 0, 0, 0);
	// 设置背景为无色，无边框
	related_entries_widget->setStyleSheet("background-color: transparent; border: none;");

	// 设置样式表
	ui.left_part->setStyleSheet("background-color: transparent; border: none;");
	ui.left_tab_widget->setStyleSheet("QTabWidget::pane { border: 0; }");

	out_label->setStyleSheet("background-color: transparent; font-size: 14px;");
	out_entries_list->setStyleSheet("background-color: transparent; border: none;");
	in_label->setStyleSheet("background-color: transparent; font-size: 14px;");
	in_entries_list->setStyleSheet("background-color: transparent; border: none;");

	// 锚点
	QListWidget* anchor_list = new QListWidget(ui.left_tab_widget);

	// 大纲
	QListWidget* synopsis_list = new QListWidget(ui.left_tab_widget);

	// 锚点、大纲，对应的index分别是0、1、2
	ui.left_tab_widget->addTab(related_entries_widget, "关联词条");
	ui.left_tab_widget->addTab(anchor_list, "锚点");
	ui.left_tab_widget->addTab(synopsis_list, "大纲");


	relatedEntriedButtonClicked();  // 默认显示关联词条

	for (int i = 0; i < ui.left_tab_widget->tabBar()->count(); ++i) {
		ui.left_tab_widget->tabBar()->setTabButton(i, QTabBar::RightSide, nullptr); // 移除关闭按钮
	}

	ui.left_tab_widget->tabBar()->hide();  // 隐藏tabbar

	//ui.left_tab_widget->tabBar()->setStyleSheet("QTabBar { height: 0px; }");  // 隐藏tabbar


	// 左边的关联词条、锚点、大纲
	connect(ui.related_entries_button, &QAbstractButton::clicked, this, &KMMainWindow::relatedEntriedButtonClicked);
	connect(ui.anchor_button, &QAbstractButton::clicked, this, &KMMainWindow::anchorButtonClicked);
	connect(ui.synopsis_button, &QAbstractButton::clicked, this, &KMMainWindow::synopsisButtonClicked);
	// 大纲列表的左键点击
	//connect(synopsis_list, &QListWidget::itemClicked, this, &KMMainWindow::synopsisItemClicked);
	// 大刚列表的右键点击
	//connect(synopsis_list, &QListWidget::customContextMenuRequested, this, &KMMainWindow::synopsisItemRightClicked);

	// tabwidget相关
	connect(ui.tab_widget, &QTabWidget::tabCloseRequested, this, &KMMainWindow::acttabCloseRequested);  // 点击tab关闭按钮时，关闭特定的tab

	// 文件菜单
	connect(ui.act_create_entry, &QAction::triggered, this, &KMMainWindow::actCreateEntry);  // 点击新建文件时，创建一个新的tab
	connect(ui.act_delete_entry, &QAction::triggered, this, &KMMainWindow::actDeleteEntry);  // 点击删除文件时，询问并删除当前词条，同时删除对应的tab
	connect(ui.act_save_kl, &QAction::triggered, this, &KMMainWindow::actSaveKL);  // 点击保存库时，保存当前库
	connect(ui.act_open_start_window, &QAction::triggered, this, &KMMainWindow::actOpenStartWindow);  // 点击打开启动窗口

	// 编辑菜单
	connect(ui.act_add_text_block, &QAction::triggered, this, &KMMainWindow::actAddTextBlock);  // 添加一个文本块
	connect(ui.act_add_code_block, &QAction::triggered, this, &KMMainWindow::actAddCodeBlock);  // 添加一个代码块
	connect(ui.act_add_image_block, &QAction::triggered, this, &KMMainWindow::actAddImageBlock);  // 添加一个图片块
	connect(ui.act_add_header_block, &QAction::triggered, this, &KMMainWindow::actAddHeaderBlock);  // 添加一个标题块

	// 标签部分
	connect(ui.act_search_label, &QAction::triggered, this, &KMMainWindow::actSearchLabel);  // 搜索标签
	connect(ui.act_recent_label, &QAction::triggered, this, &KMMainWindow::actRecentLabel);  // 最近使用的标签
	connect(ui.act_manage_label, &QAction::triggered, this, &KMMainWindow::actManageLabel);  // 管理标签


	connect(ui.act_search_entry, &QAction::triggered, this, &KMMainWindow::actSearchEntry);//点击搜素词条时，打开搜索框
	connect(ui.act_search_kl, &QAction::triggered, this, &KMMainWindow::actSearchkl);//点击搜素库时，打开搜索框
	connect(ui.act_search_multi_kl, &QAction::triggered, this, &KMMainWindow::actSearchMultikl);//点击多库搜素时，打开搜索框
	connect(ui.act_merge_kl, &QAction::triggered, this, &KMMainWindow::actopenmergekl);//点击合库时，打开搜索框
	connect(ui.act_separate_kl, &QAction::triggered, this, &KMMainWindow::actopenseparatekl);//点击分库时，打开搜索框

	// 知识库改变时，更新窗口标题
	connect(this, &KMMainWindow::klChanged, this, &KMMainWindow::handleKLChanged);
}

KMMainWindow::~KMMainWindow() {}

void KMMainWindow::handleKLChanged()
{
	if (is_saved) {
		// 设置窗口的标题
		this->setWindowTitle("km-" + kl_name + " *");
		is_saved = false;
	}
}

bool KMMainWindow::addEntryToTab(EntryWidget* entry_widget, const QString& entry_title) 
{
	if (entry_widget == nullptr)
	{
		return false;
	}
	ui.tab_widget->addTab(entry_widget, entry_title);

	connect(entry_widget->getEntryArea(), &EntryArea::contentChange, this, &KMMainWindow::handleKLChanged);
	connect(entry_widget->getEntryArea(), &EntryArea::titleChange, this, &KMMainWindow::synopsisButtonClicked);

	ui.tab_widget->setCurrentWidget(entry_widget);

	return true;
};

// 关闭时，询问未保存的词条，从 current_kl_list 中删除当前库
void KMMainWindow::closeEvent(QCloseEvent* event)
{
	// 保存知识库
	actSaveKL();
	
	// 从 current_kl_list 中删除当前库
	if (removeKLFromCurrentKLList(getKLName(), getOriginalKLPath()) == Status::Error)
	{
		QMessageBox::warning(this, "错误", "无法从 current_kl_list 中删除当前库：" + KMMainWindow::kl_name);
		// 询问是否继续关闭
		QMessageBox box(QMessageBox::Warning, "警告", "是否关闭程序？", QMessageBox::Yes | QMessageBox::No, this);
		box.button(QMessageBox::Yes)->setText("是");
		box.button(QMessageBox::No)->setText("否");
		if (box.exec() == QMessageBox::No)
		{
			event->ignore();
			return;
		}
		else
		{

			event->accept();
			return;
		}
	}

	event->accept();
	return;
}

// 获得知识库的名称
QString KMMainWindow::getKLName() const
{
	return kl_name;
}

// 获得压缩的知识库的路径
QString KMMainWindow::getOriginalKLPath() const
{
	return original_kl_path;
}

// 获得解压的知识库的路径
QString KMMainWindow::getTempKLPath() const
{
	return temp_kl_path;
}

// 获得元数据
const MetaData& KMMainWindow::getMetaData() const
{
	return meta_data;
}

// 槽：关联词条
void KMMainWindow::relatedEntriedButtonClicked()
{
	// 设置三个按钮的样式
	ui.related_entries_button->setStyleSheet("border: none; background: transparent; padding-bottom: 5px; border-bottom: 2px solid black; font-weight: bold;");
	ui.anchor_button->setStyleSheet("border: none; background: transparent; padding-bottom: 5px;");
	ui.synopsis_button->setStyleSheet("border: none; background: transparent; padding-bottom: 5px;");

	// 设置left_tab_widget的当前tab
	ui.left_tab_widget->setCurrentIndex(0);

	QWidget* related_entries_widget = static_cast<QWidget*>(ui.left_tab_widget->widget(0));

	QListWidget* out_entries_list = static_cast<QListWidget*>(related_entries_widget->layout()->itemAt(1)->widget());
	QListWidget* in_entries_list = static_cast<QListWidget*>(related_entries_widget->layout()->itemAt(3)->widget());

	out_entries_list->clear();
	in_entries_list->clear();

	// 如果没有词条，直接返回
	if (ui.tab_widget->count() == 0) return;

	// 获得当前的词条id
	int current_entry_id = (static_cast<EntryWidget*>(ui.tab_widget->currentWidget()))->getEntryId();

	const auto out_set = meta_data.getOut(current_entry_id);
	const auto in_set = meta_data.getIn(current_entry_id);

	if (out_set != nullptr)
	{
		for (int entry_id : *out_set)
		{
			const EntryMeta* entry_meta = meta_data.getEntry(entry_id);
			if (entry_meta == nullptr) continue;
			out_entries_list->addItem(entry_meta->title());
		}
	}

	if (in_set != nullptr)
	{
		for (int entry_id : *in_set)
		{
			const EntryMeta* entry_meta = meta_data.getEntry(entry_id);
			if (entry_meta == nullptr) continue;
			in_entries_list->addItem(entry_meta->title());
		}
	}
}

// 槽：锚点
void KMMainWindow::anchorButtonClicked()
{
	ui.related_entries_button->setStyleSheet("border: none; background: transparent; padding-bottom: 5px;");
	ui.anchor_button->setStyleSheet("border: none; background: transparent; padding-bottom: 5px; border-bottom: 2px solid black; font-weight: bold;");
	ui.synopsis_button->setStyleSheet("border: none; background: transparent; padding-bottom: 5px;");
	ui.left_tab_widget->setCurrentIndex(1);

	// 从元数据刷新锚点列表
	QListWidget* anchor_list = static_cast<QListWidget*>(ui.left_tab_widget->widget(1));
	anchor_list->clear();

	const auto& anchor_set = meta_data.getAnchors();
	for (int entry_id : anchor_set)
	{
		const EntryMeta* entry_meta = meta_data.getEntry(entry_id);
		if (entry_meta == nullptr) continue;
		anchor_list->addItem(entry_meta->title());
	}
}

// 槽：大纲
void KMMainWindow::synopsisButtonClicked()
{
	ui.related_entries_button->setStyleSheet("border: none; background: transparent; padding-bottom: 5px;");
	ui.anchor_button->setStyleSheet("border: none; background: transparent; padding-bottom: 5px;");
	ui.synopsis_button->setStyleSheet("border: none; background: transparent; padding-bottom: 5px; border-bottom: 2px solid black; font-weight: bold;");
	ui.left_tab_widget->setCurrentIndex(2);

	// 当前的词条获得大纲
	QListWidget* synopsis_list = static_cast<QListWidget*>(ui.left_tab_widget->widget(2));
	synopsis_list->clear();

	if (ui.tab_widget->count() == 0) return;

	EntryWidget* entry_widget = static_cast<EntryWidget*>(ui.tab_widget->currentWidget());
	const EntryArea* entry_area = entry_widget->getEntryArea();
	const auto synopsis = entry_area->getOutline();

	// 列出大纲，并且根据大纲的层级，设置不同的缩进
	for (const auto& it : synopsis)
	{
		QListWidgetItem* item = new QListWidgetItem(synopsis_list);
		
		QString text;
		for (int i = 1; i < it.level; ++i) text += "  ";
		text += it.text;

		item->setText(text);
		item->setData(Qt::UserRole, it.position);
		synopsis_list->addItem(item);
	}
}
