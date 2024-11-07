#ifndef LABEL_MANAGER_H
#define LABEL_MANAGER_H

#include <QWidget>

#include "../MainWindow/KMMainWindow.h"

namespace Ui {
class TagManager;
}

class TagManager : public QWidget
{
    Q_OBJECT

public:
    explicit TagManager(KMMainWindow* main_window, QWidget *parent = nullptr);
    ~TagManager();
	void refreshTagList();  // 刷新标签列表

signals:
	void tagChanged();  // 标签改变（新增、删除或者编辑）

public slots:
	void actNewTag();  // 新建标签
	void actDeleteTag();  // 删除标签
	void actEditTag();  // 修改标签
	void actSortTag();  // 标签字典序排序

private:
    Ui::TagManager *ui;
	KMMainWindow* main_window;  // 主窗口，用来获得当前打开的知识库相关信息
};

#endif // LABEL_MANAGER_H
