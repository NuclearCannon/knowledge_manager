#ifndef MERGE_KL_SAME_NAME_H
#define MERGE_KL_SAME_NAME_H

#include <QWidget>
#include <qlistwidget.h>
#include <QDir>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QScrollBar>

namespace Ui {
class merge_kl_same_name;
}

class merge_kl_same_name : public QWidget
{
    Q_OBJECT

public:
    explicit merge_kl_same_name(QWidget *parent = nullptr,QListWidget* resultListWidget = nullptr);
    ~merge_kl_same_name();

    void creactmergekl_same_name(QString kl_name,QString kl_path);
    bool copyDirectory_same_name(QString libraryName,QDir sourceDir, QString fileName, QDir targetDir, QString elsefilename);

signals:
    void samename_puttonewklOK(QString kl_name,QString kl_path,int comboBox);
    void goback();
public slots:

    void choose_klnameandpath();
    void syncScrollbars(int value);
    void open_go_back();
    void on_comboBox_currentIndexChanged(int index);
    void on_comboBox3_currentIndexChanged(int index);
protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    Ui::merge_kl_same_name *ui;
    QListWidget* resultListWidget_a;

};

#endif // MERGE_KL_SAME_NAME_H
