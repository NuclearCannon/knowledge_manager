#include <QDir>
#include <QMessageBox>
#include <QFileDialog>

#include "new_kl_guidance.h"
#include "ui_newklguidance.h"
#include "../MainWindow/KMMainWindow.h"
#include "../MainWindow/MetaData.h"
#include "../StartWindow/startwindow.h"
#include "../public.h"
#include "../Compress/compression.h"

NewKLGuidance::NewKLGuidance(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::new_kl_guidance)
{
    ui->setupUi(this);
	

	// 设置知识库路径为默认路径
	ui->kl_path_lineEdit->setText(default_path_for_all_kls);

	// 点击创建新的库文件按钮
	connect(ui->create_button, &QPushButton::clicked, this, &NewKLGuidance::createButtonClicked);
	// 点击选择路径按钮
	connect(ui->select_path_button, &QPushButton::clicked, this, &NewKLGuidance::selectPathButtonClicked);

	connect(ui->create_button, &QPushButton::clicked, this, &NewKLGuidance::emit_merge_kl_signal);
}

NewKLGuidance::~NewKLGuidance()
{
    delete ui;
}

// 槽：点击创建新的库文件按钮
void NewKLGuidance::createButtonClicked()
{
	// 获取用户输入的库名，如果输入的库名为空，则在创建按钮上面显示一个提示：知识库名不能为空
	QString kl_name = ui->kl_name_lineEdit->text();
	QString path = ui->kl_path_lineEdit->text();
	QDir target_dir(path);
	if (kl_name.isEmpty())
	{
		QMessageBox::warning(this, "错误", "知识库名不能为空");
		return;
	}
	else if (path.isEmpty())
	{
		QMessageBox::warning(this, "错误", "知识库路径不能为空");
		return;
	}
	else if (!target_dir.exists())
	{
		QMessageBox::warning(this, "错误", "知识库路径不存在");
		return;
	}
	else if (target_dir.exists(kl_name+".km"))
	{
		QMessageBox::warning(this, "错误", "目标文件夹存在同名知识库");
		return;
	}

	QString kl_path;

	// 将kl_path修改为path + '/' + kl_name
	if (path.endsWith('/'))
	{
		kl_path = path + kl_name;
	}
	else {
		kl_path = path + '/' + kl_name;
	}

	// 在对应位置建立一个文件夹
	QDir kl_dir(kl_path);
	if (!kl_dir.mkpath(kl_path))
	{
		QMessageBox::warning(this, "错误", "创建知识库文件夹失败！");
		return;
	}

	QFile meta_data_file(kl_path + "/meta_data.xml");

	if (!meta_data_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		//qDebug() << "无法打开文件用于写入:" << file.errorString();
		QMessageBox::warning(this, "错误", "无法打开文件用于写入：" + meta_data_file.errorString());
		return;
	}
	
	// 元数据初始化
	MetaData new_meta_data;
	try {
		new_meta_data.dump(meta_data_file);
	}
	catch (...) {
		QMessageBox::warning(this, "错误", "创建知识库元数据失败！");
		return;
	}

	meta_data_file.close();

	//压缩文件夹
	if (!compress_folder(kl_path.toStdString(), kl_path.toStdString() + ".km"))
	{
		// 删除刚刚创建的文件夹
		if (!kl_dir.removeRecursively())
		{
			QMessageBox::warning(this, "错误", "删除知识库文件夹失败！");
			return;
		}
	
		QMessageBox::warning(this, "错误", "压缩知识库失败！");
		return;
	}

	// 删除刚刚创建的文件夹
	if (!kl_dir.removeRecursively())
	{
		QMessageBox::warning(this, "错误", "删除知识库文件夹失败！");
		return;
	}

	// 先不压缩了，打开时解压也换成复制？？？

	// 关闭父窗口
	StartWindow* parent_window = dynamic_cast<StartWindow*>(this->parent());
	if (parent_window)
	{
		parent_window->close();
		//parent_window->deleteLater();
	}

	KMMainWindow* km = KMMainWindow::construct(kl_name, kl_path + ".km", nullptr);
	if (km == nullptr)
	{
		QMessageBox::warning(this, "错误", "打开知识库失败！");
		return;
	}
	km->show();

	// 关闭new_kl_guidance
	this->close();
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
		ui->kl_path_lineEdit->setText(kl_path);
	}
}

//新加
void NewKLGuidance::emit_merge_kl_signal() {
	QString kl_name = ui->kl_name_lineEdit->text();
	// 获取输入的知识库路径
	QString kl_path = ui->kl_path_lineEdit->text();
	if (kl_name.isEmpty())
	{
		// 调整输入框下面的占位标签，显示“知识库名不能为空”
		ui->hint_label->setText("知识库名不能为空");
		return;
	}
	else if (kl_path.isEmpty())
	{
		// 调整输入框下面的占位标签，显示“知识库路径不能为空”
		ui->hint_label->setText("知识库路径不能为空");
		return;
	}

	// 在输入的知识库路径下创建一个文件夹kl_name，如果创建失败则提示用户
	QDir dir;
	if (!dir.mkpath(kl_path + '/' + kl_name))
	{
		// 用一个对话框提示用户创建失败
		QMessageBox::warning(this, "错误", "知识库文件夹创建失败");
		return;
	}

	// 将km_path修改为kl_path + '/' + kl_name
	if (kl_path.endsWith('/'))
	{
		kl_path = kl_path + kl_name;
	}
	else {
		kl_path = kl_path + '/' + kl_name;
	}
	emit mergekl(kl_name,kl_path);
}