#include <QTextEdit>
#include <QMessageBox>
#include <QString>
#include <QInputDialog>
#include <QFile>
#include <QDialog>

#include "KMMainWindow.h"
#include "../EntryWidget/EntryWidget.h"
#include "../StartWindow/startwindow.h"
#include "../Compress/compression.h"

// tab相关的槽函数
// 点击tab关闭按钮时，关闭特定的tab
void KMMainWindow::acttabCloseRequested(int index)
{
	//qDebug() << "atabCloseRequested index = " << index << '\n';
	ui.tab_widget->removeTab(index);
}

// 文件菜单相关的槽函数
// 点击新建文件时，创建一个新的tab
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

	//qDebug() << "void KMMainWindow::actCreateEntry-------------------------------------All widgets:";
	//QList<QWidget*> widgets = qApp->allWidgets();
	//for (QWidget* widget : widgets) {
	//	qDebug() << widget;
	//}
}

// 点击删除文件时，询问并删除当前词条，同时删除对应的tab
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
	}
}

// 点击保存库时，保存当前库
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

	// 压缩知识库
	//if (!compress_folder(getTempKLPath().toStdString(), getOriginalKLPath().toStdString()))
	//{
	//	QMessageBox::warning(this, "错误", "压缩知识库失败！");
	//	return;
	//}

	// 先不压缩，直接复制？？？
	QDir temp_kl_dir(getTempKLPath());
	QDir original_kl_dir(getOriginalKLPath());
	if (!temp_kl_dir.exists())
	{
		QMessageBox::warning(this, "错误", "临时知识库不存在！");
		return;
	}
	if (!original_kl_dir.exists())
	{
		if (!original_kl_dir.mkpath(getOriginalKLPath()))
		{
			QMessageBox::warning(this, "错误", "无法创建原始知识库文件夹！");
			return;
		}
	}

	// 删除原始知识库文件夹下的所有文件
	if (!original_kl_dir.removeRecursively())
	{
		QMessageBox::warning(this, "错误", "删除原始知识库文件夹失败！");
		return;
	}


	if (!copyDirectory(getTempKLPath(), getOriginalKLPath()))
	{
		QMessageBox::warning(this, "错误", "复制知识库失败！");
		return;
	}

	is_saved = true;
	setWindowTitle("km-" + kl_name);
}

// 槽：点击打开启动窗口
void KMMainWindow::actOpenStartWindow()
{
	StartWindow* start_window = new StartWindow();
	start_window->setWindowFlags(Qt::Window);  // 独立窗口
	start_window->refreshListWidget();  // 刷新listWidget
	start_window->show();
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


