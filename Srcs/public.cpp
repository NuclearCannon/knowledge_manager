/*
实现了一些公共函数，定义全局变量
*/

#include <QMessageBox>
#include <QString>
#include <QInputDialog>
#include <QFile>
#include <QDir>

#include "public.h"

QString current_path_of_mainfile;  // 当前程序所在的路径
QString data_path;  // 存放软件运行所需要的文件，历史记录等
QString default_path_for_all_kls;  // 默认的所有知识库的存放位置
QString default_path_for_temp_kls;  // 默认的临时知识库的存放位置

int recent_kl_list_limit = 10;  // 最近打开的知识库列表的最大长度
int recent_entry_list_limit = 10;  // 最近打开的词条列表最大长度

// 注意：存入文件的路径是绝对路径，且一定是带.km的路径

// 将打开的库文件名称加到最近打开的库文件列表中，作为列表第一项（就是放在txt文件开头）
Status addKLToRecentKLList(const QString kl_name, const QString kl_path)
{
	QString recent_kl_list_path = data_path + '/' + "recent_kl_list.txt";
	QFile recent_kl_list(recent_kl_list_path);
	if (!recent_kl_list.open(QIODevice::ReadOnly))
	{
		// 创建一个新的recent_kl_list.txt
		if (!recent_kl_list.open(QIODevice::WriteOnly)) {
			return Status::Error;
		}

		// 空的文件可以不检查了
		// 写入新的库名和路径
		QTextStream out(&recent_kl_list);
		out << kl_name << '\n' << kl_path << '\n';
		recent_kl_list.close();
	}
	else {
		// 读取现有内容，检查，如果存在则删除之前存在的相同库的历史记录
		QTextStream in(&recent_kl_list);
		QString existing_content;
		while (!in.atEnd())
		{
			QString _kl_name = in.readLine();
			QString _kl_path = in.readLine();
			if (kl_name == _kl_name && kl_path == _kl_path)
			{
				continue;
			}
			existing_content += _kl_name + '\n' + _kl_path + '\n';
		}

		recent_kl_list.close();

		// 关闭后只写打开（会清空文件），写入新的库名和路径和筛选后的内容
		if (!recent_kl_list.open(QIODevice::WriteOnly)) {
			return Status::Error;
		}

		QTextStream out(&recent_kl_list);
		out << kl_name << '\n' << kl_path << '\n';
		out << existing_content;

		recent_kl_list.close();
	}
	return Status::Success;
}

// 将某个库文件名称从最近打开的库文件列表中删除
Status removeKLFromRecentKLList(const QString kl_name, const QString kl_path)
{
	QString recent_kl_list_path = data_path + '/' + "recent_kl_list.txt";
	QFile recent_kl_list(recent_kl_list_path);
	if (!recent_kl_list.open(QIODevice::ReadOnly))
	{
		recent_kl_list.open(QIODevice::WriteOnly);
		recent_kl_list.close();
		return Status::Success;  // 空文件不用删除
	}

	QString existing_content = "";
	QTextStream in(&recent_kl_list);
	while (!in.atEnd())
	{
		QString _kl_name = in.readLine();
		QString _kl_path = in.readLine();
		if (kl_name == _kl_name && kl_path == _kl_path)
		{
			continue;
		}
		existing_content += _kl_name + '\n' + _kl_path + '\n';
	}
	recent_kl_list.close();

	if (!recent_kl_list.open(QIODevice::WriteOnly))
	{
		return Status::Error;
	}

	QTextStream out(&recent_kl_list);
	out << existing_content;

	recent_kl_list.close();
	return Status::Success;
}

// 将打开的库文件加入到当前打开的库文件列表中
Status addKLToCurrentKLList(const QString kl_name, const QString kl_path)
{
	QString current_kl_list_path = data_path + '/' + "current_kl_list.txt";
	QFile current_kl_list(current_kl_list_path);
	if (!current_kl_list.open(QIODevice::ReadOnly))
	{
		// 创建一个新的current_kl_list.txt
		if (!current_kl_list.open(QIODevice::WriteOnly)) {
			return Status::Error;
		}

		// 空的文件可以不检查了
		// 写入新的库名
		QTextStream out(&current_kl_list);
		out << kl_name << '\n' << kl_path << '\n';
		current_kl_list.close();
	}
	else {
		// 检查要打开的库文件是否已经在当前打开的库文件列表中，如果在则不添加
		QTextStream in(&current_kl_list);
		QString existing_content;
		while (!in.atEnd())
		{
			QString _kl_name = in.readLine();
			QString _kl_path = in.readLine();
			if (kl_name == _kl_name && kl_path == _kl_path)
			{
				current_kl_list.close();
				return Status::Failure;  // 已经在当前打开的库文件列表中
			}
			existing_content += _kl_name + '\n' + _kl_path + '\n';
		}

		current_kl_list.close();

		// 关闭后只写打开（会清空文件），写入新的库名和路径和筛选后的内容
		if (!current_kl_list.open(QIODevice::WriteOnly)) {
			return Status::Error;
		}

		QTextStream out(&current_kl_list);
		out << kl_name << '\n' << kl_path << '\n';
		out << existing_content;

		current_kl_list.close();
	}
	return Status::Success;
}

// 将某个库文件名称从当前打开的库文件列表中删除
Status removeKLFromCurrentKLList(const QString kl_name, const QString kl_path)
{
	QString current_kl_list_path = data_path + '/' + "current_kl_list.txt";
	QFile current_kl_list(current_kl_list_path);
	if (!current_kl_list.open(QIODevice::ReadOnly))
	{
		current_kl_list.open(QIODevice::WriteOnly);
		current_kl_list.close();
		return Status::Success;  // 空文件不用删除
	}

	QString existing_content = "";
	QTextStream in(&current_kl_list);
	while (!in.atEnd())
	{
		QString _kl_name = in.readLine();
		QString _kl_path = in.readLine();
		if (kl_name == _kl_name && kl_path == _kl_path)
		{
			continue;
		}
		existing_content += _kl_name + '\n' + _kl_path + '\n';
	}
	current_kl_list.close();

	if (!current_kl_list.open(QIODevice::WriteOnly)) return Status::Error;

	QTextStream out(&current_kl_list);
	out << existing_content;

	current_kl_list.close();
	return Status::Success;
}

// 获取当前打开的库文件列表中最新的以"未命名的知识库"开头的知识库名称
QString getLatestTempKLName()
{
	QString current_kl_list_path = data_path + '/' + "current_kl_list.txt";
	QFile current_kl_list(current_kl_list_path);
	if (!current_kl_list.open(QIODevice::ReadOnly))
	{
		if (!current_kl_list.open(QIODevice::WriteOnly))
		{
			return "Error";
		}

		return "";
	}

	QString latest_temp_kl_name = "";
	QTextStream in(&current_kl_list);
	while (!in.atEnd())
	{
		QString kl_name = in.readLine();
		QString kl_path = in.readLine();
		if (kl_name.startsWith("未命名的知识库"))
		{
			latest_temp_kl_name = kl_name;
			break;  // 第一个以"未命名的知识库"开头的就是最新的
		}
	}
	current_kl_list.close();
	return latest_temp_kl_name;
}

// 根据旧的库名和路径，新的库名和路径，修改当前打开的库文件列表中的库名和路径，如果找不到旧的库名和路径，则返回Failure，如果修改失败，则返回Error
Status modifyKLInCurrentKLList(const QString& old_kl_name, const QString& old_kl_path, const QString& new_kl_name, const QString& new_kl_path)
{
	QString current_kl_list_path = data_path + '/' + "current_kl_list.txt";
	QFile current_kl_list(current_kl_list_path);
	if (!current_kl_list.open(QIODevice::ReadOnly))
	{
		return Status::Error;
	}

	bool found = false;

	QString existing_content = "";
	QTextStream in(&current_kl_list);
	while (!in.atEnd())
	{
		QString _kl_name = in.readLine();
		QString _kl_path = in.readLine();
		if (old_kl_name == _kl_name && old_kl_path == _kl_path)
		{
			existing_content += new_kl_name + '\n' + new_kl_path + '\n';
			found = true;
		}
		else
		{
			existing_content += _kl_name + '\n' + _kl_path + '\n';
		}
	}
	current_kl_list.close();

	if (!found) return Status::Failure;

	if (!current_kl_list.open(QIODevice::WriteOnly)) return Status::Error;

	QTextStream out(&current_kl_list);
	out << existing_content;

	current_kl_list.close();
	return Status::Success;
}


