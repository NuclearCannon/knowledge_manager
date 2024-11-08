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
#include "MetaData.h"

KMMainWindow* KMMainWindow::construct(QString kl_name, QString kl_path, QWidget* parent)
{
	// 文件检查
	QString _temp_kl_path = default_path_for_temp_kls + "/" + kl_name;
	QDir _temp_kl_dir(_temp_kl_path);
	QFile _original_kl_file(kl_path);
	if (!_original_kl_file.exists())  // 如果kl_path不存在，提示
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
	if (!kl_path.endsWith(kl_name + ".km"))  // kl_path要以kl_name + ".zip"结尾
	//if (!kl_path.endsWith(kl_name))  // kl_path要以kl_name + ".zip"结尾
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

	// 默认先显示锚点
	km->anchorButtonClicked();  

	return km;
}

bool KMMainWindow::initialize()
{
	// 从original_kl_path中解压库文件到temp_kl_path
	QDir original_kl_dir(original_kl_path);
	QDir temp_kl_dir(temp_kl_path);

	// 解压
	if (!decompress_zip(original_kl_path.toStdString(), temp_kl_path.toStdString()))
	{
		QMessageBox::warning(this, "错误", "解压知识库失败：" + original_kl_path);
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
	ui.left_part->setStyleSheet("background-color: transparent; border: none; padding-left: 5px; padding-right: 2px;");
	ui.left_tab_widget->setStyleSheet("QTabWidget::pane { border: 0; margin-top: 10px}");

	out_label->setStyleSheet("background-color: transparent; font-size: 14px;");
	out_entries_list->setStyleSheet("background-color: transparent; border: none;");
	in_label->setStyleSheet("background-color: transparent; font-size: 14px;");
	in_entries_list->setStyleSheet("background-color: transparent; border: none;");

	// 锚点
	QListWidget* anchor_list = new QListWidget(ui.left_tab_widget);

	// 标签
	QListWidget* tag_list = new QListWidget(ui.left_tab_widget);

	// 大纲
	QListWidget* synopsis_list = new QListWidget(ui.left_tab_widget);

	// 锚点、关联词条、标签、大纲，对应tab_widget的index分别是0，1，2，3
	ui.left_tab_widget->addTab(anchor_list, "锚点");
	ui.left_tab_widget->addTab(related_entries_widget, "关联");
	ui.left_tab_widget->addTab(tag_list, "标签");
	ui.left_tab_widget->addTab(synopsis_list, "大纲");

	for (int i = 0; i < ui.left_tab_widget->tabBar()->count(); ++i) {
		ui.left_tab_widget->tabBar()->setTabButton(i, QTabBar::RightSide, nullptr); // 移除关闭按钮
	}

	ui.left_tab_widget->tabBar()->hide();  // 隐藏tabbar

	// 设置右键菜单，缺少该设置，右键菜单无法弹出
	anchor_list->setContextMenuPolicy(Qt::CustomContextMenu);   
	in_entries_list->setContextMenuPolicy(Qt::CustomContextMenu);
	out_entries_list->setContextMenuPolicy(Qt::CustomContextMenu);


	// 左边的锚点、关联、标签、大纲的按钮
	connect(ui.anchor_button, &QPushButton::clicked, this, &KMMainWindow::anchorButtonClicked);
	connect(ui.related_entries_button, &QPushButton::clicked, this, &KMMainWindow::relatedEntriedButtonClicked);
	connect(ui.tag_button, &QPushButton::clicked, this, &KMMainWindow::tagButtonClicked);
	connect(ui.synopsis_button, &QPushButton::clicked, this, &KMMainWindow::synopsisButtonClicked);
	// 左键点击
	connect(synopsis_list, &QListWidget::itemClicked, this, &KMMainWindow::synopsisItemClicked);
	connect(out_entries_list, &QListWidget::itemClicked, this, &KMMainWindow::relatedEntryItemClicked);
	connect(in_entries_list, &QListWidget::itemClicked, this, &KMMainWindow::relatedEntryItemClicked);
	connect(anchor_list, &QListWidget::itemClicked, this, &KMMainWindow::anchorItemClicked);
	// 右键点击
	connect(anchor_list, &QListWidget::customContextMenuRequested, this, &KMMainWindow::anchorItemRightClicked);
	connect(in_entries_list, &QListWidget::customContextMenuRequested, this, &KMMainWindow::relatedEntryItemRightClicked);
	connect(out_entries_list, &QListWidget::customContextMenuRequested, this, &KMMainWindow::relatedEntryItemRightClicked);

	// tabwidget相关
	connect(ui.tab_widget, &QTabWidget::tabCloseRequested, this, &KMMainWindow::acttabCloseRequested);  // 点击tab关闭按钮时，关闭特定的tab
	connect(ui.tab_widget, &QTabWidget::currentChanged, this, &KMMainWindow::tabWidgetChanged);  // tab改变时，更新左边的锚点、关联、标签、大纲

	// 文件菜单
	connect(ui.act_create_entry, &QAction::triggered, this, &KMMainWindow::actCreateEntry);  // 点击新建文件时，创建一个新的tab
	connect(ui.act_delete_entry, &QAction::triggered, this, &KMMainWindow::actDeleteEntry);  // 点击删除文件时，询问并删除当前词条，同时删除对应的tab
	connect(ui.act_save_kl, &QAction::triggered, this, &KMMainWindow::actSaveKL);  // 点击保存库时，保存当前库
	connect(ui.act_open_start_window, &QAction::triggered, this, &KMMainWindow::actOpenStartWindow);  // 点击打开启动窗口
	connect(ui.act_set_current_entry_as_anchor, &QAction::triggered, this, &KMMainWindow::actSetCurrentEntryAsAnchor);  // 设置当前词条为锚点

	// 编辑菜单
	connect(ui.act_add_text_block, &QAction::triggered, this, &KMMainWindow::actAddTextBlock);  // 添加一个文本块
	connect(ui.act_add_code_block, &QAction::triggered, this, &KMMainWindow::actAddCodeBlock);  // 添加一个代码块
	connect(ui.act_add_image_block, &QAction::triggered, this, &KMMainWindow::actAddImageBlock);  // 添加一个图片块
	connect(ui.act_add_header_block, &QAction::triggered, this, &KMMainWindow::actAddHeaderBlock);  // 添加一个标题块
	// 样式菜单
	connect(ui.act_set_type_code, &QAction::triggered, this, &KMMainWindow::actSetTypeCode);
	connect(ui.act_set_type_link, &QAction::triggered, this, &KMMainWindow::actSetTypeLink);
	connect(ui.act_set_type_normal, &QAction::triggered, this, &KMMainWindow::actSetTypeNormal);
	connect(ui.act_bold, &QAction::triggered, this, &KMMainWindow::actBold);
	connect(ui.act_italic, &QAction::triggered, this, &KMMainWindow::actItalic);
	connect(ui.act_strike, &QAction::triggered, this, &KMMainWindow::actStrike);
	connect(ui.act_underline, &QAction::triggered, this, &KMMainWindow::actUnderline);
	connect(ui.act_remove_bold, &QAction::triggered, this, &KMMainWindow::actRemoveBold);
	connect(ui.act_remove_italic, &QAction::triggered, this, &KMMainWindow::actRemoveItalic);
	connect(ui.act_remove_strike, &QAction::triggered, this, &KMMainWindow::actRemoveStrike);
	connect(ui.act_remove_underline, &QAction::triggered, this, &KMMainWindow::actRemoveUnderline);

	// 标签部分
	connect(ui.act_search_label, &QAction::triggered, this, &KMMainWindow::actSearchLabel);  // 搜索标签
	connect(ui.act_recent_label, &QAction::triggered, this, &KMMainWindow::actRecentLabel);  // 最近使用的标签
	connect(ui.act_manage_label, &QAction::triggered, this, &KMMainWindow::actManageLabel);  // 管理标签


	connect(ui.act_search_entry, &QAction::triggered, this, &KMMainWindow::actSearchEntry);//点击搜素词条时，打开搜索框
	connect(ui.act_search_kl, &QAction::triggered, this, &KMMainWindow::actSearchkl);//点击搜素库时，打开搜索框
	//connect(ui.act_search_multi_kl, &QAction::triggered, this, &KMMainWindow::actSearchMultikl);//点击多库搜素时，打开搜索框
	//connect(ui.act_merge_kl, &QAction::triggered, this, &KMMainWindow::actopenmergekl);//点击合库时，打开搜索框
	//connect(ui.act_separate_kl, &QAction::triggered, this, &KMMainWindow::actopenseparatekl);//点击分库时，打开搜索框

	// 知识库改变时，更新窗口标题
	connect(this, &KMMainWindow::klChanged, this, &KMMainWindow::handleKLChanged);

	// 关联部分
	connect(ui.act_point_out, &QAction::triggered, this, &KMMainWindow::addPointOut);
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

// 根据entry_id，跳转到对应的词条，如果词条已经打开，则直接跳转，否则打开词条，返回是否成功跳转
bool KMMainWindow::openEntry(int entry_id)
{
	// 先判断是否已经加到tab_widget中，如果是就直接将其设为当前tab
	for (int i = 0; i < ui.tab_widget->count(); ++i)
	{
		EntryWidget* entry_widget = static_cast<EntryWidget*>(ui.tab_widget->widget(i));
		if (entry_widget->getEntryId() == entry_id)
		{
			ui.tab_widget->setCurrentWidget(entry_widget);
			return true;
		}
	}

	// 如果不在tab_widget中，就新建一个tab
	const EntryMeta* entry_meta = meta_data.getEntry(entry_id);
	if (entry_meta == nullptr) return false;

	EntryWidget* entry_widget = EntryWidget::construct(entry_id, ui.tab_widget, this);

	if (!addEntryToTab(entry_widget, entry_meta->title()))  // 这里会判断entry_widget是否为nullptr
	{
		return false;
	}

	return true;
}

// 关闭时，询问未保存的词条，从 current_kl_list 中删除当前库
void KMMainWindow::closeEvent(QCloseEvent* event)
{
	// 询问是否保存
	if (!is_saved)
	{
		QMessageBox box(QMessageBox::Warning, "警告", "是否保存当前知识库？", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);
		box.button(QMessageBox::Yes)->setText("是");
		box.button(QMessageBox::No)->setText("否");
		box.button(QMessageBox::Cancel)->setText("取消");
		int rnt = box.exec();
		if (rnt == QMessageBox::Yes)
		{
			actSaveKL();
		}
		else if (rnt == QMessageBox::Cancel)
		{
			event->ignore();
			return;
		}
	}
	
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
MetaData& KMMainWindow::getMetaData()
{
	return meta_data;
}

// 获得tab_widget当前的EntryWidget
EntryWidget* KMMainWindow::getCurrentEntryWidget()
{
	if (ui.tab_widget->count() == 0) return nullptr;
	return static_cast<EntryWidget*>(ui.tab_widget->currentWidget());
}

// tab改变时，更新锚点、关联词条、大纲、标签
void KMMainWindow::tabWidgetChanged(int index)
{
	if (index == -1) return;

	int i = ui.left_tab_widget->currentIndex();
	switch (i)  // 锚点不需要更新
	{
		case 1:
			relatedEntriedButtonClicked();
			break;
		case 2:
			tagButtonClicked();
			break;
		case 3:
			synopsisButtonClicked();
			break;
	}
}

// 槽：关联词条
void KMMainWindow::relatedEntriedButtonClicked()
{
	// 设置三个按钮的样式
	ui.related_entries_button->setStyleSheet("border: none; background: transparent; padding-bottom: 10px; font-size: 16px; border-bottom: 3px solid black; font-weight: bold;");
	ui.anchor_button->setStyleSheet("border: none; background: transparent; padding-bottom: 10px; font-size: 16px;");
	ui.synopsis_button->setStyleSheet("border: none; background: transparent; padding-bottom: 10px; font-size: 16px;");
	ui.tag_button->setStyleSheet("border: none; background: transparent; padding-bottom: 10px; font-size: 16px;");

	// 设置left_tab_widget的当前tab
	ui.left_tab_widget->setCurrentIndex(1);

	QWidget* related_entries_widget = static_cast<QWidget*>(ui.left_tab_widget->widget(1));

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
			QListWidgetItem* item = new QListWidgetItem(out_entries_list);
			item->setText(entry_meta->title());
			item->setData(Qt::UserRole, entry_id);
			out_entries_list->addItem(item);
		}
	}

	if (in_set != nullptr)
	{
		for (int entry_id : *in_set)
		{
			const EntryMeta* entry_meta = meta_data.getEntry(entry_id);
			if (entry_meta == nullptr) continue;
			QListWidgetItem* item = new QListWidgetItem(in_entries_list);
			item->setText(entry_meta->title());
			item->setData(Qt::UserRole, entry_id);
			in_entries_list->addItem(item);
		}
	}
}

// 槽：// 左键关联词条中的条目，跳转到对应的词条
void KMMainWindow::relatedEntryItemClicked(QListWidgetItem* item)
{
	int entry_id = item->data(Qt::UserRole).toInt();

	QListWidget* out_entries_list = static_cast<QListWidget*>(ui.left_tab_widget->widget(1)->layout()->itemAt(1)->widget());
	QListWidget* in_entries_list = static_cast<QListWidget*>(ui.left_tab_widget->widget(1)->layout()->itemAt(3)->widget());
	out_entries_list->setCurrentItem(nullptr);
	in_entries_list->setCurrentItem(nullptr);

	if (!openEntry(entry_id))
	{
		QMessageBox::warning(this, "错误", "打开词条失败：" + item->text());
	}
}

// 槽：右键关联词条中的条目，弹出菜单，指出词条的菜单包括 跳转、删除，指入词条的菜单包括 跳转
void KMMainWindow::relatedEntryItemRightClicked(const QPoint& pos)
{
	QWidget* related_entries_widget = ui.left_tab_widget->widget(1);
	QListWidget* out_entries_list = static_cast<QListWidget*>(related_entries_widget->layout()->itemAt(1)->widget());
	QListWidget* in_entries_list = static_cast<QListWidget*>(related_entries_widget->layout()->itemAt(3)->widget());

	bool point_to = true;  // 是否是指出的词条

	QListWidgetItem* item = out_entries_list->itemAt(pos);
	if (item == nullptr)
	{
		point_to = false;
		item = in_entries_list->itemAt(pos);
		if (item == nullptr) return;
	}

	QMenu menu(related_entries_widget);
	menu.setStyleSheet(
		"QMenu { background-color: #ffffff; border: 1px solid #cccccc; }"
		"QMenu::item { background-color: transparent; padding: 8px 16px; margin: 0px; }"
		"QMenu::item:selected { background-color: #f0f0f0; color: #000000; }"
	);

	QAction* jump_action = new QAction("跳转", &menu);
	menu.addAction(jump_action);

	connect(jump_action, &QAction::triggered, this, [=]() {
		relatedEntryItemClicked(item);
	});

	if (point_to) {
		QAction* delete_action = new QAction("删除", &menu);
		menu.addAction(delete_action);

		connect(delete_action, &QAction::triggered, this, [=]() {
			int entry_id = getCurrentEntryWidget()->getEntryId();
			int point_to_entry_id = item->data(Qt::UserRole).toInt();
			int rnt = meta_data.removeLinkRelationship(entry_id, point_to_entry_id);
			if (rnt == -2)
			{
				QMessageBox::warning(this, "错误", "删除关联失败，关联对应的词条不存在");
				return;
			}
			relatedEntriedButtonClicked();

			handleKLChanged();
		});
	}

	menu.exec(QCursor::pos());
}

// 槽：锚点
void KMMainWindow::anchorButtonClicked()
{
	ui.related_entries_button->setStyleSheet("border: none; background: transparent; padding-bottom: 10px; font-size: 16px;");
	ui.anchor_button->setStyleSheet("border: none; background: transparent; padding-bottom: 10px; font-size: 16px; border-bottom: 3px solid black; font-weight: bold;");
	ui.synopsis_button->setStyleSheet("border: none; background: transparent; padding-bottom: 10px; font-size: 16px;");
	ui.tag_button->setStyleSheet("border: none; background: transparent; padding-bottom: 10px; font-size: 16px;");
	ui.left_tab_widget->setCurrentIndex(0);

	// 从元数据刷新锚点列表
	QListWidget* anchor_list = static_cast<QListWidget*>(ui.left_tab_widget->widget(0));
	anchor_list->clear();

	const auto& anchor_set = meta_data.getAnchors();
	for (int entry_id : anchor_set)
	{
		const EntryMeta* entry_meta = meta_data.getEntry(entry_id);
		if (entry_meta == nullptr) continue;
		QListWidgetItem* item = new QListWidgetItem(anchor_list);
		item->setText(entry_meta->title());
		item->setData(Qt::UserRole, entry_id);
		anchor_list->addItem(item);

		qDebug() << "锚点：" << entry_meta->title();
	}
}

// 槽：左键锚点中的条目，跳转到对应的词条
void KMMainWindow::anchorItemClicked(QListWidgetItem* item)
{
	int entry_id = item->data(Qt::UserRole).toInt();

	QListWidget* anchor_list = static_cast<QListWidget*>(ui.left_tab_widget->widget(0));
	anchor_list->setCurrentItem(nullptr);

	if (!openEntry(entry_id))
	{
		QMessageBox::warning(this, "错误", "打开词条失败：" + item->text());
	}
}

// 槽：右键锚点中的条目，弹出菜单
void KMMainWindow::anchorItemRightClicked(const QPoint& pos)
{
	QListWidget* anchor_list = static_cast<QListWidget*>(ui.left_tab_widget->widget(0));
	QListWidgetItem* item = anchor_list->itemAt(pos);
	if (item == nullptr) return;

	QMenu menu(anchor_list);
	QAction* jump_action = new QAction("跳转", &menu);
	QAction* delete_action = new QAction("删除", &menu);
	menu.addAction(jump_action);
	menu.addAction(delete_action);

	menu.setStyleSheet(
		"QMenu { background-color: #ffffff; border: 1px solid #cccccc; }"
		"QMenu::item { background-color: transparent; padding: 8px 16px; margin: 0px; }"
		"QMenu::item:selected { background-color: #f0f0f0; color: #000000; }"
	);

	connect(jump_action, &QAction::triggered, this, [=]() {
		anchorItemClicked(item);
	});


	connect(delete_action, &QAction::triggered, this, [=]() {
		int entry_id = item->data(Qt::UserRole).toInt();
		int rnt = meta_data.removeAnchor(entry_id);
		if (rnt == -1)
		{
			QMessageBox::warning(this, "错误", "删除锚点失败，不存在该锚点");
			return;
		}
		else if (rnt == -2) 
		{
			QMessageBox::warning(this, "错误", "删除锚点失败，锚点对应的词条不存在");
			return;
		}
		anchorButtonClicked();

		handleKLChanged();
	});

	menu.exec(QCursor::pos());
}

// 槽：标签
void KMMainWindow::tagButtonClicked()
{
	ui.related_entries_button->setStyleSheet("border: none; background: transparent; padding-bottom: 10px; font-size: 16px;");
	ui.anchor_button->setStyleSheet("border: none; background: transparent; padding-bottom: 10px; font-size: 16px;");
	ui.synopsis_button->setStyleSheet("border: none; background: transparent; padding-bottom: 10px; font-size: 16px;");
	ui.tag_button->setStyleSheet("border: none; background: transparent; padding-bottom: 10px; font-size: 16px; border-bottom: 3px solid black; font-weight: bold;");
	ui.left_tab_widget->setCurrentIndex(2);

	QListWidget* tag_list = static_cast<QListWidget*>(ui.left_tab_widget->widget(2));
	tag_list->clear();

	if (ui.tab_widget->count() == 0) return;

	EntryWidget* entry_widget = static_cast<EntryWidget*>(ui.tab_widget->currentWidget());
	int entry_id = entry_widget->getEntryId();
	const EntryMeta* entry_meta = meta_data.getEntry(entry_id);

	const auto& tags = entry_meta->getTags();
	

	for (int tag_id : tags)
	{
		const Tag* tag = meta_data.getTag(tag_id);
		if (tag == nullptr) continue;
		QListWidgetItem* item = new QListWidgetItem(tag_list);
		item->setText(tag->name());
		item->setData(Qt::UserRole, tag_id);
		tag_list->addItem(item);
	}
}

// 处于大纲tab时，刷新大纲
void KMMainWindow::refreshSynopsis()
{
	// 如果当前不是展示大纲，直接返回
	if (ui.left_tab_widget->currentIndex() != 3) return;

	// 从当前的词条获得大纲
	QListWidget* synopsis_list = static_cast<QListWidget*>(ui.left_tab_widget->widget(3));
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

// 槽：大纲
void KMMainWindow::synopsisButtonClicked()
{
	ui.related_entries_button->setStyleSheet("border: none; background: transparent; padding-bottom: 10px; font-size: 16px;");
	ui.anchor_button->setStyleSheet("border: none; background: transparent; padding-bottom: 10px; font-size: 16px;");
	ui.synopsis_button->setStyleSheet("border: none; background: transparent; padding-bottom: 10px; font-size: 16px; border-bottom: 3px solid black; font-weight: bold;");
	ui.tag_button->setStyleSheet("border: none; background: transparent; padding-bottom: 10px; font-size: 16px;");
	ui.left_tab_widget->setCurrentIndex(3);

	refreshSynopsis();
}

// 槽：大纲列表的左键点击
void KMMainWindow::synopsisItemClicked(QListWidgetItem* item)
{
	EntryWidget* entry_widget = static_cast<EntryWidget*>(ui.tab_widget->currentWidget());
	EntryArea* entry_area = entry_widget->getEntryArea();
	entry_area->rollTo(item->data(Qt::UserRole).toInt());

	QListWidget* synopsis_list = static_cast<QListWidget*>(ui.left_tab_widget->widget(3));
	synopsis_list->setCurrentItem(nullptr);
}

bool KMMainWindow::addEntryToTab(EntryWidget* entry_widget, const QString& entry_title)
{
	if (entry_widget == nullptr)
	{
		return false;
	}
	ui.tab_widget->addTab(entry_widget, entry_title);

	connect(entry_widget->getEntryArea(), &EntryArea::contentChange, this, &KMMainWindow::handleKLChanged);
	connect(entry_widget->getEntryArea(), &EntryArea::titleChange, this, &KMMainWindow::refreshSynopsis);

	ui.tab_widget->setCurrentWidget(entry_widget);

	return true;
};
