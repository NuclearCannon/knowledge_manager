#include <QtWidgets/QApplication>
#include <QFile>
#include <QDir>
#include <QMessageBox>

#include "MainWindow/KMMainWindow.h"
#include "StartWindow/StartWindow.h"
#include "public.h"

/*
    本项目约定：
		km：knowledge manager，项目名称
		kl: knowledge library，知识库
		act: action，即ui设计中的action
		？？？：合并时需要修改的地方
*/

bool initGlobalVariables();  // 初始化全局变量

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //KMMainWindow w;
    //w.show();

	// 全局变量初始化，对数据文件夹检查
	if (!initGlobalVariables())
	{
		return -1;
	}

	StartWindow sw;
	// 初始化listWidget
	sw.refreshListWidget();
	sw.show();
    return a.exec();
}

bool initGlobalVariables()
{
	current_path_of_mainfile = QCoreApplication::applicationDirPath();  // 当前程序所在的路径
	data_path = current_path_of_mainfile + "/data";
	default_path_for_all_kls = data_path + "/all_kls";
	default_path_for_temp_kls = data_path + "/temp_kls";

	// 检查data文件夹是否存在，不存在则创建
	QDir data_dir(data_path);
	if (!data_dir.exists())
	{
		if (!data_dir.mkpath(data_path))
		{
			QMessageBox::warning(nullptr, "错误", "无法创建data文件夹，请检查权限：" + data_path);
			return false;
		}
	}

	// 检查all_kls文件夹是否存在，不存在则创建
	QDir all_kls_dir(default_path_for_all_kls);
	if (!all_kls_dir.exists())
	{
		if (!all_kls_dir.mkpath(default_path_for_all_kls))
		{
			QMessageBox::warning(nullptr, "错误", "无法创建all_kls文件夹，请检查权限：" + default_path_for_all_kls);
			return false;
		}
	}

	// 检查temp_kls文件夹是否存在，不存在则创建
	QDir temp_kls_dir(default_path_for_temp_kls);
	if (!temp_kls_dir.exists())
	{
		if (!temp_kls_dir.mkpath(default_path_for_temp_kls))
		{
			QMessageBox::warning(nullptr, "错误", "无法创建temp_kls文件夹，请检查权限：" + default_path_for_temp_kls);
			return false;
		}
	}

	// 为了防止程序异常退出，没有删除current_kl_list文件，所以每次启动时都要删除
	QFile current_kl_list_file(data_path + "/current_kl_list.txt");
	if (current_kl_list_file.exists())
	{
		if (!current_kl_list_file.remove())
		{
			QMessageBox::warning(nullptr, "错误", "无法删除current_kl_list文件，请检查权限：" + data_path + "/current_kl_list.txt");
			return false;
		}
	}

	return true;
}