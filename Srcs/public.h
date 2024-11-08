/*
实现了一些公共函数，定义全局变量
*/
#ifndef PUBLIC_H
#define PUBLIC_H

#include <QString>
#include <QWidget>

extern QString current_path_of_mainfile;
extern QString data_path;
extern QString default_path_for_all_kls;
extern QString default_path_for_temp_kls;

enum class Status
{
	Success,  // 要执行的操作成功
	Failure,  // 一般是逻辑错误
	Error  // 一般是文件操作错误或者其他错误
};

// 将打开的库文件名称加到最近打开的库文件列表中，作为列表第一项（就是放在txt文件开头）
Status addKLToRecentKLList(const QString kl_name, const QString kl_path);

// 将某个库文件名称从最近打开的库文件列表中删除
Status removeKLFromRecentKLList(const QString kl_name, const QString kl_path);

// 将打开的库文件加入到当前打开的库文件列表中
Status addKLToCurrentKLList(const QString kl_name, const QString kl_path);

// 将某个库文件名称从当前打开的库文件列表中删除
Status removeKLFromCurrentKLList(const QString kl_name, const QString kl_path);

// 获取当前打开的库文件列表中最新的以temp开头的知识库名称
QString getLatestTempKLName();

// 根据旧的库名和路径，新的库名和路径，修改当前打开的库文件列表中的库名和路径
Status modifyKLInCurrentKLList(const QString& old_kl_name, const QString& old_kl_path, const QString& new_kl_name, const QString& new_kl_path);


#endif // !PUBLIC_H