#ifndef SEARCH_H
#define SEARCH_H

#include <QWidget>
#include <qlistwidget.h>
#include <QButtonGroup>
namespace Ui {
    class search;
}

class search : public QWidget
{
    Q_OBJECT

public:
    explicit search(QWidget* parent = nullptr, QListWidget* muti_kl_nameandpath = nullptr);
    ~search();
    //void open_entry(const QString& filePath, const QString fileName, QWidget* parent);
    void initial();
signals:
    void updateTabWidget(const QString& klName, const QString& klPath, QWidget* widget, const QString& fileName);
    void gobacktoklOK();
private slots:
    void on_search_content_changed();
    void on_search_title_changed();
    void on_search_label_changed();
    void onListItemClicked(QListWidgetItem* item);
    void open_entry(const QString& klName, const QString& klPath, const QString fileName);
    void open_go_back();
    void onSearchTextChanged(const QString& text);
    void onCheckBoxStateChanged(int id);
private:
    Ui::search* ui;
    QWidget father;
    QListWidget* multiklnameandpath;  // 存放搜索的库
    QButtonGroup* buttonGroup;
};

#endif // SEARCH_H
