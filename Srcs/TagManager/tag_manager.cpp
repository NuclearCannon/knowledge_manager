#include <QFIle>
#include <QMessageBox>

#include "tag_manager.h"
#include "ui_tag_manager.h"
#include "../MainWindow/KMMainWindow.h"
#include "tag_edit_dialog.h"

TagManager::TagManager(QWidget *parent)
    : QWidget(parent)
	, ui(new Ui::TagManager), mainwindow(static_cast<KMMainWindow*>(parentWidget()))
{
    ui->setupUi(this);

	// 设置窗口标题
	setWindowTitle("标签管理-" + TagManager::mainwindow->getKLName());

	connect(ui->new_tag_button, &QPushButton::clicked, this, &TagManager::actNewTag);
	connect(ui->delete_tag_button, &QPushButton::clicked, this, &TagManager::actDeleteTag);
	connect(ui->edit_tag_button, &QPushButton::clicked, this, &TagManager::actEditTag);
}

TagManager::~TagManager()
{
    delete ui;
}

// 刷新标签列表，从知识库中的tags.txt中读取标签值
void TagManager::refreshTagList()
{
	ui->tag_list_widget->clear();



	// 将选中的item取消选中
	ui->tag_list_widget->setCurrentItem(nullptr);
}

// 槽：新建标签，具体是向知识库中的tags.txt开头插入一个标签值
void TagManager::actNewTag()
{
	// 弹出tag_edit_dialog
	TagEditDialog* tag_edit_dialog = new TagEditDialog(this);
	tag_edit_dialog->setWindowTitle("新建标签");

	// 设置标签为红色
	tag_edit_dialog->setColor("#FF0000");

	if (tag_edit_dialog->exec() == QDialog::Accepted)  // 用户点了确定
	{
		QString tag = tag_edit_dialog->getName();
		QString color = tag_edit_dialog->getColor();

		// 检查标签是否已经存在


		// 刷新标签列表
		TagManager::refreshTagList();
	}

	// 释放对话框资源
	delete tag_edit_dialog;
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

	QString tag = item->text();
	QString hint = "将删除知识库 " + mainwindow->getKLName() + " 内所有 " + tag + " 标签，是否继续？";
	QMessageBox* hint_box = new QMessageBox(QMessageBox::Warning, "警告", hint, QMessageBox::Yes | QMessageBox::No, this);
	hint_box->button(QMessageBox::Yes)->setText("是");
	hint_box->button(QMessageBox::No)->setText("否");
	if (hint_box->exec() == QMessageBox::Yes)
	{
		// 先从标签集中删除标签

		// 刷新标签列表
		TagManager::refreshTagList();

		// 从所有词条中删除标签？？？

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

	TagEditDialog* tag_edit_dialog = new TagEditDialog(this);
	tag_edit_dialog->setWindowTitle("修改标签");

	// 获取当前标签的名字和颜色，并应用到tag_edit_dialog中
	QString old_tag = item->text();
	tag_edit_dialog->setName(old_tag);
	tag_edit_dialog->setColor(item->background().color().name());  // .color()返回QColor对象，.name()返回颜色的十六进制字符串

	if (tag_edit_dialog->exec() == QDialog::Accepted)  // 用户点了确定
	{
		QString tag = tag_edit_dialog->getName();
		QString color = tag_edit_dialog->getColor();

		// 提醒是否真的修改
		QString hint = "将修改知识库 " + mainwindow->getKLName() + " 内所有 " + old_tag + " 标签为 " + tag + " ，是否继续？";
		QMessageBox* hint_box = new QMessageBox(QMessageBox::Warning, "警告", hint, QMessageBox::Yes | QMessageBox::No, this);
		hint_box->button(QMessageBox::Yes)->setText("是");
		hint_box->button(QMessageBox::No)->setText("否");
		if (hint_box->exec() == QMessageBox::No)
		{
			delete tag_edit_dialog;
			return;
		}



		// 刷新标签列表
		TagManager::refreshTagList();
	}

	// 释放对话框资源
	delete tag_edit_dialog;
}

// 槽：标签字典序排序
void TagManager::actSortTag()
{
	// 将list_widget按字典序排序
	QList<QListWidgetItem*> items;
	while(ui->tag_list_widget->count() > 0)
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
