#include <QTextEdit>
#include <QMessageBox>
#include <QString>
#include <QInputDialog>
#include <QFile>
#include <QDialog>

#include "KMMainWindow.h"
#include "../EntryWidget/EntryWidget.h"
#include "../Compress/compression.h"
#include "../NewKLGuidance/new_kl_guidance.h"
#include "../RecentKLWindow/RecentKLWindow.h"
#include "../public.h"

// tab相关的槽函数
// 点击tab关闭按钮时，关闭特定的tab
void KMMainWindow::actTabCloseRequested(int index)
{
	//qDebug() << "atabCloseRequested index = " << index << '\n';
	ui.tab_widget->removeTab(index);
}

// 文件菜单相关的槽函数

// 槽：点击新建文件时，创建一个新的tab
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
	// 如果是临时库，则询问是否保存
	if (is_temp_kl)
	{
		QMessageBox msg_box(QMessageBox::Warning, "警告", "是否保存当前知识库？", QMessageBox::Yes | QMessageBox::No, this);
		msg_box.button(QMessageBox::Yes)->setText("是");
		msg_box.button(QMessageBox::No)->setText("否");
		int reply = msg_box.exec();

		if (reply == QMessageBox::No)
		{
			return;
		}
	}


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

	meta_data.dump(meta_data_file);

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

	if (is_temp_kl) 
	{
		// 要保存的话，需要调用new_kl_guidance中的创建知识库的函数，压缩temp_kl_path到用户指定的路径
		NewKLGuidance new_kl_guidance(this, true, temp_kl_path);
		
		if (new_kl_guidance.exec() == QDialog::Accepted)
		{
			// 修改kl_name, original_kl_path，temp_kl_path
			QString old_kl_name = kl_name;
			kl_name = new_kl_guidance.getKLName();

			original_kl_path = new_kl_guidance.getKLPath();

			QString old_temp_kl_path = temp_kl_path;
			temp_kl_path = temp_kl_path.left(temp_kl_path.lastIndexOf('/')) + "/" + kl_name;

			// 修改临时知识库文件夹的名称为输入的名称
			QDir temp_kl_dir(old_temp_kl_path);
			if (!temp_kl_dir.rename(old_temp_kl_path, temp_kl_path))
			{
				QMessageBox::warning(this, "错误", "重命名临时知识库文件夹失败！");
				return;
			}

			// 修改current_kl_list
			Status status = modifyKLInCurrentKLList(old_kl_name, old_temp_kl_path, kl_name, original_kl_path);
			if (status == Status::Error)
			{
				QMessageBox::warning(this, "错误", "修改current_kl_list失败！");
				return;
			}
			else if (status == Status::Failure)
			{
				QMessageBox::warning(this, "错误", "修改current_kl_list失败！未找到原知识库！");
				return;
			}

			is_temp_kl = false;
		}
		else
		{
			return;
		}
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

// 槽：点击新建知识库时，新建一个知识库
void KMMainWindow::actCreateNewKnowledgeLibrary()
{
	NewKLGuidance new_kl_guidance(this);
	new_kl_guidance.setWindowFlags(Qt::Window); // 确保它作为一个独立的窗口显示
	new_kl_guidance.exec();
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

	// 解析出kl_name，并去掉".km"
	QString open_kl_name = open_kl_path.split('/').last();
	open_kl_name = open_kl_name.left(open_kl_name.lastIndexOf('.'));

	// 如果当前库是临时库，且没有作任何修改，则直接在当前窗口打开新的知识库
	if (is_temp_kl && is_saved)
	{
		// 添加到current_kl_list
		Status status = addKLToCurrentKLList(open_kl_name, open_kl_path);
		if (status == Status::Error)
		{
			QMessageBox::warning(this, "错误", "无法打开或操作文件：" + open_kl_path);
			return;
		}
		else if (status == Status::Failure)
		{
			QMessageBox::warning(this, "错误", "知识库已经打开\n知识库位置：" + open_kl_path);
			return;
		}

		// 删除current_kl_lits中的临时库
		status = removeKLFromCurrentKLList(kl_name, original_kl_path);
		if (status == Status::Error)
		{
			QMessageBox::warning(this, "错误", "删除current_kl_list中的临时库失败！");
			return;
		}

		// 写入recent_kl_list
		status = addKLToRecentKLList(open_kl_name, open_kl_path);
		if (status == Status::Error)
		{
			// 回滚current_kl_list
			status = modifyKLInCurrentKLList(open_kl_name, open_kl_path, kl_name, original_kl_path);
			if (status == Status::Error)
			{
				QMessageBox::warning(this, "错误", "修改current_kl_list失败！");
				return;
			}
			else if (status == Status::Failure)
			{
				QMessageBox::warning(this, "错误", "修改current_kl_list失败！未找到原知识库！");
				return;
			}
			QMessageBox::warning(this, "错误", "无法打开或操作文件：" + open_kl_path);
			return;
		}

		// 备份当前的kl_name，original_kl_path，temp_kl_path
		QString old_kl_name = kl_name;
		QString old_original_kl_path = original_kl_path;
		QString old_temp_kl_path = temp_kl_path;

		// 修改kl_name, original_kl_path，temp_kl_path，is_temp_kl，在这里修改是因为initialize函数会用到这些变量
		kl_name = open_kl_name;
		original_kl_path = open_kl_path;
		temp_kl_path = default_path_for_temp_kls + "/" + kl_name;
		is_temp_kl = false;

		// 初始化，加载元数据
		if (!initialize())
		{
			// 回滚变量
			kl_name = old_kl_name;
			original_kl_path = old_original_kl_path;
			temp_kl_path = old_temp_kl_path;
			is_temp_kl = true;

			// 回滚recent_kl_list
			status = removeKLFromRecentKLList(open_kl_name, open_kl_path);
			if (status == Status::Error) QMessageBox::warning(nullptr, "错误", "无法打开或操作文件：" + open_kl_path);
			removeKLFromCurrentKLList(open_kl_name, open_kl_path);
			// 回滚current_kl_list
			status = modifyKLInCurrentKLList(open_kl_name, open_kl_path, kl_name, original_kl_path);
			if (status == Status::Error)
			{
				QMessageBox::warning(this, "错误", "修改current_kl_list失败！");
				return;
			}
			else if (status == Status::Failure)
			{
				QMessageBox::warning(this, "错误", "修改current_kl_list失败！未找到原知识库！");
				return;
			}

			QMessageBox::warning(this, "错误", "无法打开库文件：" + open_kl_path);
			return;
		}

		anchorButtonClicked();

		setWindowTitle("km - " + kl_name);
	}
	else  // 否则，打开一个新的窗口
	{
		KMMainWindow* main_window = KMMainWindow::construct(open_kl_name, open_kl_path);
		if (main_window == nullptr)
		{
			QMessageBox::warning(this, "错误", "打开知识库失败！");
			return;
		}
		main_window->show();
	}
}

// 槽：点击最近打开的知识库时，弹出最近打开的知识库列表
void KMMainWindow::actRecentKnowledgeLibrary()
{
	RecentKLWindow* recent_kl_window = RecentKLWindow::construct(this);
	if (recent_kl_window == nullptr)
	{
		QMessageBox::warning(this, "错误", "打开最近打开的知识库列表失败！");
		return;
	}
	recent_kl_window->show();
}
