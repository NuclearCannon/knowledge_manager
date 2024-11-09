#ifndef SEARCH_KL_WIDGET_H
#define SEARCH_KL_WIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QTextEdit>
#include "../MainWindow/KMMainWindow.h"
#include <ui_search_kl_widget.h>

class search_kl_widget : public QWidget
{
    Q_OBJECT

public:
    explicit search_kl_widget(KMMainWindow* _main_window, QWidget *parent = nullptr, QString kl_name = QString(), bool ifseparatekl = 0);
    ~search_kl_widget();
    void initial();//初始化
private slots:
    void searchclicked();
    void searchTextChanged(const QString& text);
    void listklClicked(QListWidgetItem* item);
    //void open_kl(const QString& filePath, const QString fileName);
   // void recentkl();
    void gobackseparatekl();
private:
    Ui::search_kl_widget_ui ui;
    QWidget father;
    bool if_separate_kl;
    QString now_kl_name;
	KMMainWindow* main_window;
};

#endif // SEARCH_KL_WIDGET_H
