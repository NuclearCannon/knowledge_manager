#ifndef SEARCH_KL_WIDGET_H
#define SEARCH_KL_WIDGET_H

#include <QWidget>
#include <qlistwidget.h>
#include <QTextEdit>

namespace Ui {
class search_kl_widget;
}

class search_kl_widget : public QWidget
{
    Q_OBJECT

public:
    explicit search_kl_widget(QWidget *parent = nullptr, QString kl_name = QString(),bool ifseparatekl = 0);
    ~search_kl_widget();
    void initial();//初始化
private slots:
    void searchclicked();
    void onSearchTextChanged(const QString& text);
    void onListklClicked(QListWidgetItem* item);
    void open_kl(const QString& filePath, const QString fileName);
   // void recentkl();
    void gobackseparatekl();
private:
    Ui::search_kl_widget* ui;
    QWidget father;
    bool if_separate_kl;
    QString now_kl_name;
};

#endif // SEARCH_KL_WIDGET_H
