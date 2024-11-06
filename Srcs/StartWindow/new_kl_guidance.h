#ifndef NEWKLGUIDANCE_H
#define NEWKLGUIDANCE_H

#pragma once
#include <QWidget>
#include "ui_newklguidance.h"

namespace Ui {
class NewKLGuidance;
}

class NewKLGuidance : public QWidget
{
    Q_OBJECT

public:
    explicit NewKLGuidance(QWidget *parent = nullptr);
    ~NewKLGuidance();


signals:
    void mergekl(QString kl_name,QString kl_path);//新加
public slots:
	void createButtonClicked();  // 点击创建新的库文件按钮
	void selectPathButtonClicked();  // 点击选择路径按钮

    void emit_merge_kl_signal();//新加
private:
    Ui::new_kl_guidance *ui;
};

#endif // NEWKLGUIDANCE_H
