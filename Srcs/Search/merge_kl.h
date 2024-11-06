#ifndef MERGE_KL_H
#define MERGE_KL_H

#include <QWidget>
#include <qlistwidget.h>
#include <QTextEdit>
#include <QDir>
namespace Ui {
class merge_kl;
}

class merge_kl : public QWidget
{
    Q_OBJECT

public:
    explicit merge_kl(QWidget *parent = nullptr);
    ~merge_kl();

    inline void merge_initializa_search_kl_list();
    bool copyDirectory(QDir sourceDir, QDir targetDir);
    bool NOTcopyDirectory(QDir sourceDir, QDir targetDir, QListWidget* resultListWidget,int comboBox);
    void gobackOK();
public slots:
    void merge_searchklclicked();//搜素库按钮点击触发
    void merge_searchclicked();//搜素按钮点击触发
    void merge_onSearchListklClicked(QListWidgetItem* item);//搜素库item点击触发
    void merge_onListklClicked(QListWidgetItem* item);//库列表item点击触发
    void allchoosedclicked();
    void allclearedclicked();
    void creactmergekl(QString kl_name,QString kl_path);//没重名
    void creactmergekl_same_name(QString kl_name,QString kl_path, int comboBox);//有重名
    void onSearchTextChanged(const QString& text);
private:
    Ui::merge_kl *ui;
};

#endif // MERGE_KL_H
