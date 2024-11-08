#include <QFile>

#include "KMMainWindow.h"
#include "../TagManager/tag_manager.h"

// 搜索标签
void KMMainWindow::actSearchLabel()
{
	// 打印所有的widget
	qDebug() << "-------------------------------------All widgets:";
	QList<QWidget*> widgets = qApp->allWidgets();
	for (QWidget* widget : widgets) {
		qDebug() << widget;
	}
}

// 最近使用的标签
void KMMainWindow::actRecentLabel()
{

}

// 管理标签
void KMMainWindow::actManageLabel()
{
	TagManager* tag_manager = new TagManager(this, this);
	connect(tag_manager, &TagManager::tagChanged, this, &KMMainWindow::handleKLChanged);
	tag_manager->refreshTagList();
	tag_manager->setWindowFlags(Qt::Window);
	tag_manager->show();
}