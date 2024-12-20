#include <QtWidgets/QApplication>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>

#include "MainWindow/KMMainWindow.h"
#include "public.h"

int app_main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	//qDebug() << "current_path_of_mainfile: " << QCoreApplication::applicationDirPath();

	// 全局变量初始化，对数据文件夹检查
	if (!globals.initial(QCoreApplication::applicationDirPath()))
	{
		return -1;
	}

	KMMainWindow* km_main_window = KMMainWindow::construct();
	if (km_main_window == nullptr)return -1;
	km_main_window->show();
	int rtn = a.exec();

	delete km_main_window;
	return rtn;

}

int main(int argc, char* argv[])
{
	int rtn = app_main(argc, argv);
	return rtn;
}


