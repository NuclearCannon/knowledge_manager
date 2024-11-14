#include <QTextEdit>
#include <QMessageBox>
#include <QString>
#include <QFile>
#include <QDialog>
#include <QPushButton>
#include <QFileDialog>

#include "KMMainWindow.h"
#include "../EntryWidget/EntryWidget.h"
#include "../Compress/compression.h"
#include "../RecentKLWindow/RecentKLWindow.h"
#include "../public.h"

// 文件菜单相关的槽函数

// 槽：点击新建词条时，创建一个新的词条并加载到tab
void KMMainWindow::actCreateEntry()
{
	// 设置一个dialog输入词条名称
	QDialog dialog(this);
	QLabel label("请输入词条名称：", &dialog);
	QLineEdit line_edit(&dialog);
	QPushButton ok_btn("确定", &dialog);
	QPushButton cancel_btn("取消", &dialog);
	QHBoxLayout* layout1 = new QHBoxLayout;
	layout1->addWidget(&label);
	layout1->addWidget(&line_edit);
	QHBoxLayout* layout2 = new QHBoxLayout;
	layout2->addWidget(&ok_btn);
	layout2->addWidget(&cancel_btn);
	QVBoxLayout* layout = new QVBoxLayout;
	layout->addLayout(layout1);
	layout->addLayout(layout2);
	dialog.setLayout(layout);

	dialog.setWindowTitle("新建词条");

	connect(&ok_btn, &QPushButton::clicked, &dialog, &QDialog::accept);
	connect(&cancel_btn, &QPushButton::clicked, &dialog, &QDialog::reject);

	QString entry_title;
	int entry_id;

	// 打印所有元数据中的词条
	//for (auto entry : meta_data.getEntries()) {
	//	qDebug() << "entry: " << entry->title();
	//}

	// 如果重复则返回到对话框，清空输入框让用户重新输入，如果按下取消则退出循环、
	while (true)
	{
		if (dialog.exec() == QDialog::Accepted)
		{
			entry_title = line_edit.text();
			if (entry_title.isEmpty())
			{
				QMessageBox::warning(this, "错误", "词条名称不能为空！");
				continue;
			}
			// 添加到元数据

			// 打印所有元数据中的词条
			for (auto entry : meta_data.getEntries()) {
				qDebug() << "entry: " << entry->title();
			}

			entry_id = meta_data.addEntry(entry_title);
			
			if (entry_id == -1)
			{
				QMessageBox::warning(this, "错误", "词条已经存在！");
				//line_edit.clear();
				continue;
			}
			else
			{
				break;
			}
		}
		else  // 取消
		{
			return;
		}
	}

	// 创建一个新的tab
	EntryWidget* entry_widget = EntryWidget::construct(entry_id, ui.tab_widget, this);

	if (!addEntryToTab(entry_widget, entry_title)) {
		QMessageBox::warning(this, "错误", "添加词条到tab失败！");
		return;
	}

	handleKLChanged();

	//qDebug() << "void KMMainWindow::actCreateEntry-------------------------------------All widgets:";
	//QList<QWidget*> widgets = qApp->allWidgets();
	//for (QWidget* widget : widgets) {
	//	qDebug() << widget;
	//}
}

// 槽：点击删除文件时，询问并删除当前词条，同时删除对应的tab
void KMMainWindow::actDeleteEntry()
{
	if (ui.tab_widget->count() == 0)
	{
		QMessageBox::warning(this, "错误", "当前没有打开的词条！");
		return;
	}

	// 询问是否删除当前词条
	QMessageBox msg_box(QMessageBox::Warning, "警告", "是否删除当前词条？", QMessageBox::Yes | QMessageBox::No, this);
	msg_box.button(QMessageBox::Yes)->setText("是");
	msg_box.button(QMessageBox::No)->setText("否");
	int reply = msg_box.exec();

	if (reply == QMessageBox::Yes)
	{
		// 获取当前tab的entry_widget
		int current_index = ui.tab_widget->currentIndex();
		EntryWidget* entry_widget = static_cast<EntryWidget*>(ui.tab_widget->widget(current_index));

		// 删除当前tab
		ui.tab_widget->removeTab(current_index);

		// 删除元数据中的词条
		int entry_id = entry_widget->getEntryId();
		meta_data.removeEntry(entry_id);

		// 删除词条文件夹
		QString entry_path = getTempKLPath() + "/" + QString::number(entry_id);
		QDir entry_dir(entry_path);
		if (!entry_dir.removeRecursively()) {
			QMessageBox::warning(this, "错误", "删除词条文件夹失败！");
			return;
		}

		// 删除entry_widget
		delete entry_widget;
		entry_widget = nullptr;

		handleKLChanged();
	}
}

// 槽：点击保存库时，保存当前库
void KMMainWindow::actSaveKL()
{
	QFile meta_data_file(getTempKLPath() + "/meta_data.xml");

	// 先打印tags和entries
	//for (auto tag : meta_data.getTags())
	//{
	//	qDebug() << "tag: " << tag;
	//}

	//for (auto entry : meta_data.getEntries())
	//{
	//	qDebug() << "entry: " << entry->title();
	//}

	if (!meta_data_file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QMessageBox::warning(this, "错误", "打开meta_data.xml失败！");
		return;
	}

	meta_data.dumpQtXml(meta_data_file);

	meta_data_file.close();

	// 遍历tab_widget，保存每个词条
	for (int i = 0; i < ui.tab_widget->count(); i++)
	{
		EntryWidget* entry_widget = static_cast<EntryWidget*>(ui.tab_widget->widget(i));
		if (!entry_widget->saveEntry())
		{
			QString error_entry_title = meta_data.getEntry(entry_widget->getEntryId())->title();
			QMessageBox::warning(this, "错误", "保存词条失败！词条名称：" + error_entry_title);
			return;
		}
	}

	// 如果是临时知识库，需要保存到用户指定的路径，否则正常压缩知识库
	if (is_temp_kl) 
	{
		QString file_path = QFileDialog::getSaveFileName(
			this,
			"保存知识库",
			default_path_for_all_kls,
			"Knowledge Manager Files (*.km))"
		);

		// 点了x或者取消
		if (file_path == "") return;

		// 要保存的话，不调用创建知识库的函数，自己压缩temp_kl_path到用户指定的路径

		// 首先检查一下临时文件夹是否存在
		QDir temp_dir(temp_kl_path);
		if (!temp_dir.exists()) {
			QMessageBox::warning(this, QStringLiteral("错误"), "临时文件夹不存在：" + temp_kl_path);
			return;
		}

		// 压缩文件夹
		if (!compress_folder(temp_kl_path.toStdString(), file_path.toStdString()))
		{
			QMessageBox::warning(this, "错误", "压缩临时知识库失败！");
			return;
		}
		
		// 压缩完了，进行临时知识库文件夹的重命名，以及current_kl_list、recent_kl_list的修改
		
		// 修改kl_name, original_kl_path，temp_kl_path
		QString old_kl_name = kl_name;
		QString old_original_kl_path = original_kl_path;
		QString old_temp_kl_path = temp_kl_path;

		kl_name = (file_path.right(file_path.size() - file_path.lastIndexOf('/') - 1));
		kl_name = kl_name.left(kl_name.size() - 3);
		original_kl_path = file_path;

		// 修改current_kl_list
		Status status = removeKLFromCurrentKLList(old_original_kl_path);
		if (status == Status::Error)
		{
			removeKLFromRecentKLList(original_kl_path);
			QMessageBox::warning(this, "错误", "修改current_kl_list失败！");
			return;
		}
		else if (status == Status::Failure)
		{
			removeKLFromRecentKLList(original_kl_path);
			QMessageBox::warning(this, "错误", "修改current_kl_list失败！未找到原知识库！");
			return;
		}
		int rnt = addKLToCurrentKLList(false, kl_name, original_kl_path);
		if (rnt == -1) {
			removeKLFromRecentKLList(original_kl_path);
			QMessageBox::warning(this, "错误", "知识库已经打开\n知识库位置：" + original_kl_path);
			return;
		}
		else if (rnt == -2)
		{
			removeKLFromRecentKLList(original_kl_path);
			QMessageBox::warning(this, "错误", "无法打开或操作文件：" + original_kl_path);
			return;
		}

		temp_kl_path = default_path_for_temp_kls + "/" + QString::number(rnt) + "_" + kl_name;

		// 修改临时知识库文件夹的名称为输入的名称
		QDir temp_kl_dir(old_temp_kl_path);
		if (!temp_kl_dir.rename(old_temp_kl_path, temp_kl_path))
		{
			// 回滚current_kl_list
			removeKLFromCurrentKLList(original_kl_path);
			addKLToCurrentKLList(true, old_kl_name, old_original_kl_path);
			QMessageBox::warning(this, "错误", "重命名临时知识库文件夹失败！");
			return;
		}

		// 添加到 recnet_kl_list
		if (addKLToRecentKLList(kl_name, original_kl_path) == Status::Error)
		{
			removeKLFromCurrentKLList(original_kl_path);
			addKLToCurrentKLList(true, old_kl_name, old_original_kl_path);
			QMessageBox::warning(this, "错误", "添加到最近打开的知识库列表失败！");
			return;
		}

		is_temp_kl = false;
	}
	else 
	{
		// 压缩知识库
		if (!compress_folder(getTempKLPath().toStdString(), getOriginalKLPath().toStdString()))
		{
			QMessageBox::warning(this, "错误", "压缩知识库失败！");
			return;
		}
	}
	
	is_saved = true;
	setWindowTitle("km - " + kl_name);
}

// 槽：设置当前词条为锚点
void KMMainWindow::actSetCurrentEntryAsAnchor()
{
	if (ui.tab_widget->count() == 0)
	{
		QMessageBox::warning(this, "错误", "当前没有打开的词条！");
		return;
	}

	int current_index = ui.tab_widget->currentIndex();
	EntryWidget* entry_widget = static_cast<EntryWidget*>(ui.tab_widget->widget(current_index));
	
	int rnt = meta_data.addAnchor(entry_widget->getEntryId());

	if (rnt == -1)
	{
		QMessageBox::warning(this, "错误", "该锚点已存在！设置锚点失败！");
	}
	else if (rnt == -2)
	{
		QMessageBox::warning(this, "错误", "设置锚点失败！");
	}
	else
	{
		handleKLChanged();
	}

	// 如果左边在展示锚点，则刷新锚点列表
	if (ui.left_tab_widget->currentIndex() == 0)
	{
		anchorButtonClicked();
	}

}

// 槽：点击新建知识库时，新建一个空白知识库
void KMMainWindow::actCreateNewKnowledgeLibrary()
{
	//QString file_path = QFileDialog::getSaveFileName(
	//	this,
	//	"保存知识库",
	//	default_path_for_all_kls,
	//	"Knowledge Manager Files (*.km))"
	//);

	//if (file_path != "") {
	//	//创建知识库的函数，在用户指定的路径构建文件夹并压缩，然后打开
	//	createNewKnowledgeLibraryHelper(file_path);
	//}

	KMMainWindow* main_window = KMMainWindow::construct();
	if (main_window == nullptr)
	{
		QMessageBox::warning(this, "错误", "新建空白知识库失败！");
		return;
	}
	main_window->show();
}

// 槽：点击打开知识库时，打开一个知识库
void KMMainWindow::actOpenKnowledgeLibrary()
{
	// 打开一个文件选择对话框，选择一个.km文件
	QString open_kl_path = QFileDialog::getOpenFileName(this, "打开知识库", "", "知识库文件(*.km)");
	if (open_kl_path.isEmpty())
	{
		return;
	}

	// 打开知识库
	openKnowledgeLibrary(open_kl_path);  // 函数遇到错误会弹出错误对话框
}

// 槽：点击最近打开的知识库时，弹出最近打开的知识库列表
void KMMainWindow::actRecentKnowledgeLibrary()
{
	RecentKLWindow* recent_kl_window = RecentKLWindow::construct(this, this);
	if (recent_kl_window == nullptr)
	{
		QMessageBox::warning(this, "错误", "打开最近打开的知识库列表失败！");
		return;
	}
	recent_kl_window->setWindowFlags(Qt::Window);
	recent_kl_window->show();
}
