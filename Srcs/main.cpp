#include <QtWidgets/QApplication>
#include <QFile>
#include <QDir>
#include <QMessageBox>

#include "MainWindow/KMMainWindow.h"
#include "public.h"

int real_main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	KMMainWindow* km_main_window = KMMainWindow::construct();
	if (km_main_window == nullptr)return -1;
	km_main_window->show();
	int rtn = a.exec();

	delete km_main_window;
	return rtn;

}

int main(int argc, char* argv[])
{
	// 全局变量初始化，对数据文件夹检查
	if(!globals.successfully_inited())
	{
		return -1;
	}

	int rtn = real_main(argc, argv);


	return rtn;
}


