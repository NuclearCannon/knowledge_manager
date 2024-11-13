#include <QTextEdit>
#include <QMessageBox>
#include <QString>
#include <QInputDialog>
#include <QFile>

#include "KMMainWindow.h"
//#include "../Search/search_kl_widget.h"
#include "../Search/search_entry_widget.h"

//搜索词条打开函数
void KMMainWindow::actSearchEntry()
{
	int result_id = getIdBySearchEntryDialog(meta_data);
	if (result_id < 0)
	{
		qDebug() << "KMMainWindow::actSearchEntry(): result_id < 0:" << result_id;
	}
	bool success = openEntry(result_id);
	if (!success)
	{
		qWarning() << "KMMainWindow::actSearchEntry(): openEntry(" << result_id << ") fail!";
	}
};
//搜索完打开词条函数
void KMMainWindow::onUpdateTabWidget(const QString& klName, const QString& klPath, QWidget* widget, const QString& fileName) {
	// 需要修改，参考第38行，52行
	
	//if (klName == kl_name) {

	//	for (int i = 0; i < ui.tab_widget->count(); ++i) {
	//		if ((ui.tab_widget->widget(i) == widget) || (ui.tab_widget->tabText(i) == fileName)) {
	//			// 如果找到了，切换到那个标签页  
	//			ui.tab_widget->setCurrentIndex(i);
	//			return;
	//		}
	//	}//没找到就添加  ？？？禁止

	//	addTab(static_cast<EntryWidget*>(widget), fileName);

	//	//ui.tab_widget->addTab(widget, fileName);
	//	for (int i = 0; i < ui.tab_widget->count(); ++i) {
	//		if (ui.tab_widget->widget(i) == widget || ui.tab_widget->tabText(i) == fileName) {
	//			ui.tab_widget->setCurrentIndex(i);
	//			return;
	//		}
	//	}
	//}
	//else {
	//	// 新建一个main_window
	//	KMMainWindow* main_window = KMMainWindow::construct(klName, klPath);
	//	main_window->addTab(widget, fileName);
	//	main_window->show();
	//}
}
////搜索库打开函数
//void KMMainWindow::actSearchkl() {
//	search_kl_widget* search_kl = new search_kl_widget(this, this, kl_name);
//	search_kl->setWindowFlags(Qt::Window); // 确保它作为一个独立的窗口显示
//	search_kl->show();
//};