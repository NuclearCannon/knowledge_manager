#ifndef SEARCH_MULTI_KL_WIDGET_H
#define SEARCH_MULTI_KL_WIDGET_H

#include <QWidget>
#include <qlistwidget.h>
#include <QTextEdit>

namespace Ui {
class search_multi_kl_widget;
}

class search_multi_kl_widget : public QWidget
{
    Q_OBJECT

public:
    explicit search_multi_kl_widget(QWidget *parent = nullptr, QString kl_name = QString());
    ~search_multi_kl_widget();
    inline void initializa_search_kl_list();

signals:
    void search_multi_kl_updateTabWidget(const QString& klName, const QString& klPath, QWidget* widget, const QString& fileName);//搜索完打开词条函数
public slots:
    void searchklclicked();//搜素库按钮点击触发
    void searchclicked();//搜素按钮点击触发
    void onSearchListklClicked(QListWidgetItem* item);//搜素库item点击触发
    void onListklClicked(QListWidgetItem* item);//库列表item点击触发
    void allchoosedclicked();
    void allclearedclicked();
    void goback_from_entry_widget();
    void onSearchTextChanged(const QString& text);
private:
    Ui::search_multi_kl_widget *ui;
    QString now_kl_name;
};

#endif // SEARCH_MULTI_KL_WIDGET_H
