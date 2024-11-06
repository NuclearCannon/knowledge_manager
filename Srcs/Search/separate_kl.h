#ifndef SEPARATE_KL_H
#define SEPARATE_KL_H

#include <QWidget>
#include <QButtonGroup>
#include<qlistwidget.h>
#include <QCloseEvent>
#include <xkeycheck.h>
namespace Ui {
class separate_kl;
}

class separate_kl : public QWidget
{
    Q_OBJECT

public:
    explicit separate_kl(QWidget *parent = nullptr, const QString fileName = QString(), const QString filePath = QString());
    ~separate_kl();
    void initial();
signals :
    void goback_separatekl();//返回
public slots:
    void on_search_content_changed();
    void on_search_title_changed();
    void on_search_label_changed();
    void gobacktoseparatekl();//返回
    void onSearchTextChanged(const QString& text);
    void onCheckBoxStateChanged(int id);
    void onListItemClicked(QListWidgetItem* item);
    void onListklClicked(QListWidgetItem* item);
    void allchoosedclicked();
    void allclearedclicked();
    void updateLabel();
    void updateLabelkl();
    void save();
    void on_checkBox_4_stateChanged(int state);
    void closeWidget();
    void select_path_button_clicked();  // 点击选择路径按钮
protected:
    void closeEvent(QCloseEvent* event) override;
private:
    Ui::separate_kl *ui;
    QString kl_name;  // 当前打开的知识库名称
    QString kl_path;  // 存放知识库的路径
    QButtonGroup* buttonGroup;
    QStringList already_saved;
    int itemsum;
};

#endif // SEPARATE_KL_H
