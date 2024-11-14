#ifndef NEWKLGUIDANCE_H
#define NEWKLGUIDANCE_H

#pragma once
#include <QWidget>
#include <QDialog>
#include "ui_newklguidance.h"

class NewKLGuidance : public QDialog
{
    Q_OBJECT

public:
	// 创建一个新的库文件的引导界面，is_temp_kl为true表示临时库需要保存，temp_kl_path是临时库的位置（不带.km）
	explicit NewKLGuidance(QWidget* parent = nullptr, bool _is_temp_kl = false, QString _temp_kl_path = "");
    ~NewKLGuidance();

	// 获得输入的库名
	QString getKLName() const;

	// 获得输入的库路径，用户输入的路径/库名.km
	QString getKLPath() const;

//signals:
//    void mergekl(QString kl_name,QString kl_path);//新加
public slots:
	void createButtonClicked();  // 点击创建新的库文件按钮
	void selectPathButtonClicked();  // 点击选择路径按钮
	
	void accept() override;  // 是否有效，如果无效则accept事件被忽略

    //void emit_merge_kl_signal();//新加
private:
    Ui::new_kl_guidance ui;
	bool is_temp_kl;  // 是否是临时库
	QString temp_kl_path;  // 临时库的位置（不带.km）
	QString new_kl_name;  // 库名
	QString new_kl_path;  // 库路径，用户输入的路径/库名.km
	bool valid;  // 是否有效，如果无效则accept事件被忽略
};

#endif // NEWKLGUIDANCE_H
