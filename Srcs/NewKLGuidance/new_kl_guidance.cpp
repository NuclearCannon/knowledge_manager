#include <QDir>
#include <QMessageBox>
#include <QFileDialog>

#include "new_kl_guidance.h"
#include "ui_newklguidance.h"
#include "../MainWindow/KMMainWindow.h"
#include "../MainWindow/MetaData.h"
#include "../public.h"
#include "../Compress/compression.h"

NewKLGuidance::NewKLGuidance(QWidget* parent, bool _is_temp_kl, QString _temp_kl_path)
    : QDialog(parent)
	, is_temp_kl(_is_temp_kl)
	, temp_kl_path(_temp_kl_path)
	, new_kl_name("")
	, new_kl_path("")
	, valid(true)
{
    ui.setupUi(this);
	
	setWindowTitle("新建知识库");
	
	// 设置知识库路径为默认路径
	ui.kl_path_lineEdit->setText(default_path_for_all_kls);

	// 点击创建新的库文件按钮
	connect(ui.create_button, &QPushButton::clicked, this, &NewKLGuidance::createButtonClicked);
	// 点击选择路径按钮
	connect(ui.select_path_button, &QPushButton::clicked, this, &NewKLGuidance::selectPathButtonClicked); 
	connect(ui.create_button, &QPushButton::clicked, this, &NewKLGuidance::accept);

	//connect(ui.create_button, &QPushButton::clicked, this, &NewKLGuidance::emit_merge_kl_signal);
}

NewKLGuidance::~NewKLGuidance()
{
}

// 获得输入的库名
QString NewKLGuidance::getKLName() const
{
	return new_kl_name;
}

// 获得输入的库路径
QString NewKLGuidance::getKLPath() const
{
	return new_kl_path;
}

// 槽：点击创建新的库文件按钮
void NewKLGuidance::createButtonClicked()
{
	valid = true;

	// 获取用户输入的库名，如果输入的库名为空，则在创建按钮上面显示一个提示：知识库名不能为空
	QString kl_name = ui.kl_name_lineEdit->text();
	QString target_kl_path = ui.kl_path_lineEdit->text();
	QDir target_kl_dir(target_kl_path);  // 目标文件夹，新的知识库将放在这个文件夹下
	if (kl_name.isEmpty())
	{
		QMessageBox::warning(this, "错误", "知识库名不能为空");
		valid = false;
		return;
	}
	else if (target_kl_path.isEmpty())
	{
		QMessageBox::warning(this, "错误", "知识库路径不能为空");
		valid = false;
		return;
	}
	else if (!target_kl_dir.exists())
	{
		QMessageBox::warning(this, "错误", "知识库路径不存在");
		valid = false;
		return;
	}
	else if (target_kl_dir.exists(kl_name + ".km"))
	{
		QMessageBox::warning(this, "错误", "目标文件夹存在同名知识库");
		valid = false;
		return;
	}

	// 将kl_path修改为path + '/' + kl_name
	if (target_kl_path.endsWith('/'))
	{
		target_kl_path = target_kl_path + kl_name;
	}
	else {
		target_kl_path = target_kl_path + '/' + kl_name;
	}

	// 更新自己的变量
	new_kl_name = kl_name;
	new_kl_path = target_kl_path + ".km";

	if (!is_temp_kl)
	{
		QDir kl_dir(target_kl_path);  // 知识库文件夹
		// 在对应位置创建知识库文件夹
		if (!kl_dir.mkpath(target_kl_path))
		{
			QMessageBox::warning(this, "错误", "创建知识库文件夹失败！");
			valid = false;
			return;
		}

		QFile meta_data_file(target_kl_path + "/meta_data.xml");

		if (!meta_data_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			//qDebug() << "无法打开文件用于写入:" << file.errorString();
			QMessageBox::warning(this, "错误", "无法打开文件用于写入：" + meta_data_file.errorString());
			valid = false;
			return;
		}

		// 元数据初始化
		MetaData new_meta_data;
		try {
			new_meta_data.dump(meta_data_file);
		}
		catch (...) {
			QMessageBox::warning(this, "错误", "创建知识库元数据失败！");
			valid = false;
			return;
		}

		meta_data_file.close();

		//压缩文件夹
		if (!compress_folder(target_kl_path.toStdString(), new_kl_path.toStdString()))
		{
			// 删除刚刚创建的文件夹
			if (!kl_dir.removeRecursively())
			{
				QMessageBox::warning(this, "错误", "删除知识库文件夹失败！");
				valid = false;
				return;
			}

			QMessageBox::warning(this, "错误", "压缩知识库失败！");
			valid = false;
			return;
		}

		// 删除刚刚创建的文件夹
		if (!kl_dir.removeRecursively())
		{
			QMessageBox::warning(this, "错误", "删除知识库文件夹失败！");
			valid = false;
			return;
		}

		KMMainWindow* km = KMMainWindow::construct(new_kl_name, new_kl_path);
		if (km == nullptr)
		{
			QMessageBox::warning(this, "错误", "打开知识库失败！");
			valid = false;
			return;
		}
		km->show();

		// 关闭new_kl_guidance
		this->close();
	}
	else
	{
		// 首先检查一下临时文件夹是否存在
		QDir temp_dir(temp_kl_path);
		if (!temp_dir.exists()) {
			QMessageBox::warning(this, QStringLiteral("错误"), "临时文件夹不存在：" + temp_kl_path);
			valid = false;
			return;
		}

		// 压缩文件夹
		if (!compress_folder(temp_kl_path.toStdString(), target_kl_path.toStdString() + ".km"))
		{
			QMessageBox::warning(this, "错误", "压缩知识库失败！");
			valid = false;
			return;
		}
	}
	//this->deleteLater();

	// 打印所有的widget
	//qDebug() << "-------------------------------------All widgets:";
	//QList<QWidget*> widgets = qApp->allWidgets();
	//for (QWidget* widget : widgets) {
	//	qDebug() << widget;
	//}

}

// 槽：点击选择路径按钮
void NewKLGuidance::selectPathButtonClicked()
{
	// 打开一个对话框，让用户选择路径
	QString kl_path = QFileDialog::getExistingDirectory(this, "选择知识库路径", QDir::homePath());
	if (!kl_path.isEmpty())
	{
		// 将选择的路径显示在输入框中
		ui.kl_path_lineEdit->setText(kl_path);
	}
}

void NewKLGuidance::accept()
{
	if (valid)
	{
		QDialog::accept();
	}
}

//新加
//void NewKLGuidance::emit_merge_kl_signal() {
//	QString kl_name = ui.kl_name_lineEdit->text();
//	// 获取输入的知识库路径
//	QString kl_path = ui.kl_path_lineEdit->text();
//	if (kl_name.isEmpty())
//	{
//		// 调整输入框下面的占位标签，显示“知识库名不能为空”
//		ui.hint_label->setText("知识库名不能为空");
//		return;
//	}
//	else if (kl_path.isEmpty())
//	{
//		// 调整输入框下面的占位标签，显示“知识库路径不能为空”
//		ui.hint_label->setText("知识库路径不能为空");
//		return;
//	}
//
//	// 在输入的知识库路径下创建一个文件夹kl_name，如果创建失败则提示用户
//	QDir dir;
//	if (!dir.mkpath(kl_path + '/' + kl_name))
//	{
//		// 用一个对话框提示用户创建失败
//		QMessageBox::warning(this, "错误", "知识库文件夹创建失败");
//		return;
//	}
//
//	// 将km_path修改为kl_path + '/' + kl_name
//	if (kl_path.endsWith('/'))
//	{
//		kl_path = kl_path + kl_name;
//	}
//	else {
//		kl_path = kl_path + '/' + kl_name;
//	}
//	emit mergekl(kl_name,kl_path);
//}