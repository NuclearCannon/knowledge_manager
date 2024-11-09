#include <QFIle>
#include <QMessageBox>

#include "tag_manager.h"
#include "ui_tag_manager.h"
#include "../MainWindow/KMMainWindow.h"
#include "tag_edit_dialog.h"

TagManager::TagManager(KMMainWindow* _main_window, QWidget* parent)
	: QWidget(parent), main_window(_main_window), last_clicked_item(nullptr)
	, ui(new Ui::TagManager)
{
    ui->setupUi(this);

	// 设置窗口标题
	setWindowTitle("标签管理-" + main_window->getKLName());

	connect(ui->add_to_curren_entry_button, &QPushButton::clicked, this, &TagManager::actAddTag);
	connect(ui->new_tag_button, &QPushButton::clicked, this, &TagManager::actNewTag);
	connect(ui->delete_tag_button, &QPushButton::clicked, this, &TagManager::actDeleteTag);
	connect(ui->edit_tag_button, &QPushButton::clicked, this, &TagManager::actEditTag);
	connect(ui->sort_tag_button, &QPushButton::clicked, this, &TagManager::actSortTag);

	connect(ui->tag_list_widget, &QListWidget::itemClicked, this, &TagManager::tagItemClicked);
}

TagManager::~TagManager()
{
    delete ui;
}

// 刷新标签列表
void TagManager::refreshTagList()
{
	ui->tag_list_widget->clear();

	const MetaData& meta_data = main_window->getMetaData();


	for (auto& tag : meta_data.getTags())
	{
		QListWidgetItem* item = new QListWidgetItem();
		item->setSizeHint(QSize(0, 40));
		item->setData(IdRole, tag->id());
		item->setData(ColorRole, tag->color());
		item->setData(NameRole, tag->name());

		QWidget* show_tag_widget = new QWidget();
		QWidget* color_widget = new QWidget();
		color_widget->setFixedSize(20, 20);
		color_widget->setStyleSheet("background-color: " + tag->color().name());

		QHBoxLayout* layout = new QHBoxLayout();
		layout->addWidget(color_widget);
		layout->addWidget(new QLabel(tag->name()));
		show_tag_widget->setLayout(layout);

		ui->tag_list_widget->addItem(item);

		ui->tag_list_widget->setItemWidget(item, show_tag_widget);
	}
	// 将选中的item取消选中
	ui->tag_list_widget->setCurrentItem(nullptr);
}

// 槽：添加标签
void TagManager::actAddTag()
{
	QListWidgetItem* item = ui->tag_list_widget->currentItem();
	if (item == nullptr)
	{
		QMessageBox::warning(this, "错误", "请选择一个标签");
		return;
	}

	int tag_id = item->data(IdRole).toInt();
	main_window->addTagToCurrentEntry(tag_id);
}

// 槽：新建标签
void TagManager::actNewTag()
{
	// 弹出tag_edit_dialog
	TagEditDialog tag_edit_dialog(this);
	tag_edit_dialog.setWindowTitle("新建标签");

	// 设置标签为红色
	tag_edit_dialog.setColor("#FF0000");

	if (tag_edit_dialog.exec() == QDialog::Accepted)  // 用户点了确定
	{
		QString tag_name = tag_edit_dialog.getName();
		QColor color(tag_edit_dialog.getColor());

		if (tag_name.isEmpty())
		{
			QMessageBox::warning(this, "错误", "标签名不能为空");
			return;
		}

		if (!color.isValid())
		{
			QMessageBox::warning(this, "错误", "颜色格式错误");
			return;
		}

		int rnt = (main_window->getMetaData()).addTag(color, tag_name);
		if (rnt == -1)
		{
			QMessageBox::warning(this, "错误", "标签名重复");
			return;
		}

		// 刷新标签列表
		TagManager::refreshTagList();

		emit tagChanged();
	}
}

// 槽：删除标签，提醒库中所有词条将删除此标签
void TagManager::actDeleteTag()
{
	QListWidgetItem* item = ui->tag_list_widget->currentItem();
	if (item == nullptr)
	{
		QMessageBox::warning(this, "错误", "请选择一个标签");
		return;
	}

	QString tag_name = item->data(NameRole).toString();
	QString hint = "将删除知识库 " + main_window->getKLName() + " 内所有 " + tag_name + " 标签，是否继续？";
	QMessageBox* hint_box = new QMessageBox(QMessageBox::Warning, "警告", hint, QMessageBox::Yes | QMessageBox::No, this);
	hint_box->button(QMessageBox::Yes)->setText("是");
	hint_box->button(QMessageBox::No)->setText("否");
	if (hint_box->exec() == QMessageBox::Yes)
	{
		MetaData& meta_data = main_window->getMetaData();
		int tag_id = item->data(IdRole).toInt();
		int rnt = meta_data.removeTag(tag_id);
		if (rnt == -1)
		{
			QMessageBox::warning(this, "错误", "标签不存在");
			return;
		}

		// 刷新标签列表
		TagManager::refreshTagList();

		emit tagChanged();
	}
}

// 槽：修改标签，先弹出tag_edit_dialog，修改再点确定后，提醒本操作将修改所有词条
void TagManager::actEditTag()
{
	QListWidgetItem* item = ui->tag_list_widget->currentItem();
	if (item == nullptr)
	{
		QMessageBox::warning(this, "错误", "请选择一个标签");
		return;
	}

	TagEditDialog tag_edit_dialog(this);
	tag_edit_dialog.setWindowTitle("修改标签");

	// 获取当前标签的名字和颜色，并应用到tag_edit_dialog中
	QString old_tag = item->data(NameRole).toString();
	tag_edit_dialog.setName(old_tag);
	tag_edit_dialog.setColor(item->data(ColorRole).toString());

	if (tag_edit_dialog.exec() == QDialog::Accepted)  // 用户点了确定
	{
		QString tag_name = tag_edit_dialog.getName();
		QString color = tag_edit_dialog.getColor();

		// 提醒是否真的修改
		QString hint = "将修改知识库 " + main_window->getKLName() + " 内所有 " + old_tag + " 标签为 " + tag_name + " ，是否继续？";
		QMessageBox* hint_box = new QMessageBox(QMessageBox::Warning, "警告", hint, QMessageBox::Yes | QMessageBox::No, this);
		hint_box->button(QMessageBox::Yes)->setText("是");
		hint_box->button(QMessageBox::No)->setText("否");
		if (hint_box->exec() == QMessageBox::No)
		{
			return;
		}

		QColor new_color(color);
		if (!new_color.isValid())
		{
			QMessageBox::warning(this, "错误", "颜色格式错误");
			return;
		}

		MetaData& meta_data = main_window->getMetaData();
		int tag_id = item->data(IdRole).toInt();
		int rnt = meta_data.modifyTagName(tag_id, tag_name);
		if (rnt == -1)
		{
			QMessageBox::warning(this, "错误", "要修改的标签不存在");
			return;
		}
		else if (rnt == -2)
		{
			QMessageBox::warning(this, "错误", "标签名重复");
			return;
		}

		// 刷新标签列表
		TagManager::refreshTagList();

		emit tagChanged();
	}
}

// 槽：标签字典序排序
void TagManager::actSortTag()
{
	// 将list_widget按字典序排序
	QList<QListWidgetItem*> items;
	while (ui->tag_list_widget->count() > 0)
	{
		// takeItem()函数会从listWidget中移除item，但不会删除item（不释放item的空间）
		items.append(ui->tag_list_widget->takeItem(0));  // 一直移除第一个item
	}
	std::sort(items.begin(), items.end(), [](QListWidgetItem* a, QListWidgetItem* b) {return a->text() < b->text(); });

	// 重新添加item
	for (QListWidgetItem* item : items)
	{
		ui->tag_list_widget->addItem(item);
	}
}

// 槽：选中后再次点击取消选中
void TagManager::tagItemClicked(QListWidgetItem* item)
{
	if (item == nullptr)
	{
		return;
	}

	if (last_clicked_item == item)
	{
		ui->tag_list_widget->setCurrentItem(nullptr);
		last_clicked_item = nullptr;
	}
	else
	{
		ui->tag_list_widget->setCurrentItem(item);
		last_clicked_item = item;
	}
}