/*
实现了一些公共函数，定义全局变量
*/
#include <QCoreApplication>
#include <QMessageBox>
#include <QString>
#include <QInputDialog>
#include <QFile>
#include <QDir>

#include "public.h"

//QString current_path_of_mainfile;  // 当前程序所在的路径
//QString data_path;  // 存放软件运行所需要的文件，历史记录等
//QString default_path_for_all_kls;  // 默认的所有知识库的存放位置
//QString default_path_for_temp_kls;  // 默认的临时知识库的存放位置

Globals globals;

int recent_kl_list_limit = 10;  // 最近打开的知识库列表的最大长度
int recent_entry_list_limit = 10;  // 最近打开的词条列表最大长度


Globals::Globals():
	m_successfully_inited(false)
{
	
	current_path_of_mainfile = QCoreApplication::applicationDirPath();  // 当前程序所在的路径
	data_path = current_path_of_mainfile + "/data";
	default_path_for_all_kls = data_path + "/all_kls";
	default_path_for_temp_kls = data_path + "/temp_kls";

	// 检查data文件夹是否存在，不存在则创建
	QDir data_dir(data_path);
	if (!data_dir.exists())
	{
		if (!data_dir.mkpath(data_path))
		{
			QMessageBox::warning(nullptr, "错误", "无法创建data文件夹，请检查权限：" + data_path);
			return;
		}
	}

	// 检查all_kls文件夹是否存在，不存在则创建
	QDir all_kls_dir(default_path_for_all_kls);
	if (!all_kls_dir.exists())
	{
		if (!all_kls_dir.mkpath(default_path_for_all_kls))
		{
			QMessageBox::warning(nullptr, "错误", "无法创建all_kls文件夹，请检查权限：" + default_path_for_all_kls);
			return;
		}
	}

	// 检查temp_kls文件夹是否存在，不存在则创建
	QDir temp_kls_dir(default_path_for_temp_kls);
	if (!temp_kls_dir.exists())
	{
		if (!temp_kls_dir.mkpath(default_path_for_temp_kls))
		{
			QMessageBox::warning(nullptr, "错误", "无法创建temp_kls文件夹，请检查权限：" + default_path_for_temp_kls);
			return;
		}
	}

	// 为了防止程序异常退出，没有删除current_kl_list文件，所以每次启动时都要删除
	QFile current_kl_list_file(data_path + "/current_kl_list.txt");
	QDir dir(data_path);

	if (current_kl_list_file.exists())
	{
		if (!current_kl_list_file.remove())
		{
			QMessageBox::warning(nullptr, "错误", "无法删除current_kl_list文件，请检查权限：" + data_path + "/current_kl_list.txt");
			return;
		}
	}
	m_successfully_inited = true;
	return;
}

bool Globals::successfully_inited() const
{
	return m_successfully_inited;
}

// 注意：存入文件的路径是绝对路径，且一定是带.km的路径
// 
// current_kl_list 的每一项是 id_{kl_name}  \n  kl_original_path  \n，其中 id的最后一位只能是1或0，0表示永久库，1表示临时库，这个id是临时文件区的id
// recent_kl_list 的每一项是 kl_name  \n  kl_original_path  \n
//
// current_kl_list 对于临时库列表和永久库列表，id都是有序的，因为每次写入都是在文件开头写入，所以最新的库的id是最大的

// 将打开的库文件名称加到最近打开的库文件列表中，作为列表第一项（就是放在txt文件开头）
Status addKLToRecentKLList(const QString kl_name, const QString kl_path)
{
	QString recent_kl_list_path = globals.data_path + '/' + "recent_kl_list.txt";
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
Status removeKLFromRecentKLList(const QString kl_path)
{
	QString recent_kl_list_path = globals.data_path + '/' + "recent_kl_list.txt";
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
		if (kl_path == _kl_path)
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

// 将打开的库文件加入到当前打开的库文件列表中，成功返回添加的id，重名失败返回-1，文件操作失败返回-2
int addKLToCurrentKLList(bool is_temp_kl, const QString kl_name, const QString kl_path)
{
	int my_id = 0;
	QString current_kl_list_path = globals.data_path + '/' + "current_kl_list.txt";
	QFile current_kl_list(current_kl_list_path);
	if (!current_kl_list.open(QIODevice::ReadOnly))
	{
		// 创建一个新的current_kl_list.txt
		if (!current_kl_list.open(QIODevice::WriteOnly)) {
			return -2;
		}

		// 空的文件可以不检查了
		// 写入新的库名
		QTextStream out(&current_kl_list);
		QString w_kl_name;
		if (is_temp_kl) {
			w_kl_name = "01_" + kl_name;
			my_id = 1;
		}
		else {
			w_kl_name = "00_" + kl_name;
			my_id = 0;
		}

		out << w_kl_name << '\n' << kl_path << '\n';
		current_kl_list.close();
	}
	else {
		int max_id = 0;

		// 检查要打开的库文件是否已经在当前打开的库文件列表中，如果在则不添加
		QTextStream in(&current_kl_list);
		QString existing_content;
		while (!in.atEnd())
		{
			QString _kl_name = in.readLine();
			QString _kl_path = in.readLine();
			if (kl_path == _kl_path)
			{
				current_kl_list.close();
				return -1;  // 已经在当前打开的库文件列表中
			}

			// 以第一个'_'为分割，取出id和kl_name
			int index = _kl_name.indexOf('_');
			if (index == -1) continue;  // 非法的库名，跳过了
			int id = _kl_name.left(index).toInt();
			if (is_temp_kl) {
				if (id % 10 == 1) {
					if (id > max_id) max_id = id;
				}
			}
			else {
				if (id % 10 == 0) {
					if (id > max_id) max_id = id;
				}
			}

			existing_content += _kl_name + '\n' + _kl_path + '\n';
		}

		current_kl_list.close();

		// 关闭后只写打开（会清空文件），写入新的库名和路径和筛选后的内容
		if (!current_kl_list.open(QIODevice::WriteOnly)) {
			return -2;
		}

		QTextStream out(&current_kl_list);

		my_id = (max_id / 10 + 1) * 10 + int(is_temp_kl);  // 获得了一个临时文件区id

		QString w_kl_name = QString::number(my_id) + "_" + kl_name;
		out << w_kl_name << '\n' << kl_path << '\n';
		out << existing_content;

		current_kl_list.close();
	}
	return my_id;
}

// 将某个库文件名称从当前打开的库文件列表中删除
Status removeKLFromCurrentKLList(const QString kl_path)
{
	QString current_kl_list_path = globals.data_path + '/' + "current_kl_list.txt";
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
		if (kl_path == _kl_path)
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

// 获取当前打开的库文件列表中最新的临时文件区id（临时库和永久库分别计数），如果is_temp_kl为true则找临时库的；文件错误则返回-1
int getLatestCurrentKLID(bool is_temp_kl)
{
	int max_id = 0;

	QString current_kl_list_path = globals.data_path + '/' + "current_kl_list.txt";
	QFile current_kl_list(current_kl_list_path);
	if (!current_kl_list.open(QIODevice::ReadOnly))
	{
		if (!current_kl_list.open(QIODevice::WriteOnly))
		{
			return -1;
		}
		return int(is_temp_kl);  // 如果是临时库，返回1，否则返回0
	}

	QTextStream in(&current_kl_list);
	while (!in.atEnd())
	{
		QString kl_name = in.readLine();
		QString kl_path = in.readLine();

		// 以第一个'_'为分割，取出id和kl_name
		int index = kl_name.indexOf('_');
		if (index == -1) continue;  // 非法的库名，跳过了
		int id = kl_name.left(index).toInt();

		if (is_temp_kl) {
			if (id % 10 == 1) {
				if (id > max_id) max_id = id;
			}
		}
		else {
			if (id % 10 == 0) {
				if (id > max_id) max_id = id;
			}
		}
	}
	current_kl_list.close();
	return max_id;
}

//// 根据旧的库名和路径，新的库名和路径，修改当前打开的库文件列表中的库名和路径，如果找不到旧的库名和路径，则返回Failure，如果修改失败，则返回Error
//Status modifyKLInCurrentKLList(const QString& old_kl_name, const QString& old_kl_path, const QString& new_kl_name, const QString& new_kl_path)
//{
//	QString current_kl_list_path = data_path + '/' + "current_kl_list.txt";
//	QFile current_kl_list(current_kl_list_path);
//	if (!current_kl_list.open(QIODevice::ReadOnly))
//	{
//		return Status::Error;
//	}
//
//	bool found = false;
//
//	QString existing_content = "";
//	QTextStream in(&current_kl_list);
//	while (!in.atEnd())
//	{
//		QString _kl_name = in.readLine();
//		QString _kl_path = in.readLine();
//		if (old_kl_name == _kl_name && old_kl_path == _kl_path)
//		{
//			existing_content += new_kl_name + '\n' + new_kl_path + '\n';
//			found = true;
//		}
//		else
//		{
//			existing_content += _kl_name + '\n' + _kl_path + '\n';
//		}
//	}
//	current_kl_list.close();
//
//	if (!found) return Status::Failure;
//
//	if (!current_kl_list.open(QIODevice::WriteOnly)) return Status::Error;
//
//	QTextStream out(&current_kl_list);
//	out << existing_content;
//
//	current_kl_list.close();
//	return Status::Success;
//}


