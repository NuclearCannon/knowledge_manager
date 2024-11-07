#ifndef SEARCH_H
#define SEARCH_H


#include <QListWidget>
#include <QButtonGroup>
#include <ui_search_entry_widget.h>
#include <QDialog>
#include <list>
#include "../MainWindow/MetaData.h"

int getIdBySearchEntryDialog(const MetaData& meta);

class SeachEntryItem : public QListWidgetItem
{
private:
    int id;
    QString title;
public:
    SeachEntryItem(int id, const QString& title);
    ~SeachEntryItem();
    int getId() const;
};


class search : public QDialog
{
    Q_OBJECT
private:
    Ui::search* ui;
    QListWidget* entrys_list_widget;  // 存放搜索的库
    QButtonGroup* button_group;
    const MetaData& meta;
    int result;

public:
    explicit search(QWidget* parent, const MetaData& meta);
    ~search();
    int getResult() const;
signals:
    void updateTabWidget(const QString& klName, const QString& klPath, QWidget* widget, const QString& fileName);
private slots:
    void onListItemClicked(QListWidgetItem* item);
    void onSearchTextChanged();
    void onCheckBoxStateChanged(int id);
private:
    void putAllEntries();  // 显示所有结果到list中
    void putEntriesByTitle(const QString& target);
    void putEntriesByTags(const std::set<int>& tag_set);
    void putEntriesByTags(const QString& tag_str);

    void addItem(int id, const QString& title);
};

#endif // SEARCH_H
