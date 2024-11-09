#include <QFile>
#include <QMessageBox>
#include <QDir>
#include <windows.h>

#include "RecentKLWindow.h"
#include "ui_RecentKLWindow.h"
#include "../public.h"

RecentKLWindow* RecentKLWindow::construct(QWidget* parent, KMMainWindow* _main_window)
{
	RecentKLWindow* recent_kl_window = new RecentKLWindow(parent, _main_window);
	if (!recent_kl_window->refreshListWidget())
	{
		QMessageBox::warning(recent_kl_window, "错误", "无法打开或操作文件：" + data_path + "/recent_kl_list.txt");
		delete recent_kl_window;
		return nullptr;
	}
	return recent_kl_window;
}

RecentKLWindow::RecentKLWindow(QWidget* parent, KMMainWindow* _main_window)
	: QWidget(parent), main_window(_main_window)
{
	ui.setupUi(this);  // listWidget的样式表在ui文件中设置了

	setWindowTitle("最近打开的知识库");

	ui.hint_label->setText("最近打开的知识库：");
	ui.search_lineEdit->setStyleSheet("border: 0px;");

	// 点击listWidget中的item，就进入到这个item对应的知识库
	connect(ui.listwidget, &QListWidget::itemClicked, this, &RecentKLWindow::listWidgetItemClicked);
	
	// 搜索框被修改后，检查搜索框内的文本是否是空，如果是空则加载历史
	connect(ui.search_lineEdit, &QLineEdit::textChanged, this, &RecentKLWindow::checkSearchLineEdit);

	// 点击搜索按钮
	connect(ui.search_pushButton, &QPushButton::clicked, this, &RecentKLWindow::searchButtonClicked);
	// 搜索框回车
	connect(ui.search_lineEdit, &QLineEdit::returnPressed, this, &RecentKLWindow::searchButtonClicked);

	// 点击排序按钮
	connect(ui.sort_button, &QPushButton::clicked, this, &RecentKLWindow::sortButtonClicked);
}

RecentKLWindow::~RecentKLWindow()
{
}

// 加载listWidget
bool RecentKLWindow::refreshListWidget()
{
	ui.listwidget->clear();

	// 打开历史库文件，如果打不开应该是不存在，则创建一个
	QString recent_kl_list_path = data_path + "/recent_kl_list.txt";
	QFile recent_kl_list(recent_kl_list_path);
	if (!recent_kl_list.open(QIODevice::ReadOnly))
	{
		// 创建一个新的recent_kl_list.txt
		if (!recent_kl_list.open(QIODevice::WriteOnly)) {
			return false;
		}
		recent_kl_list.close();
		recent_kl_list.open(QIODevice::ReadOnly);
	}

	// 将recent_kl_list中的内容添加到listWidget中
	// recent_kl_list.txt上下两行分别是知识库名称和路径，每次读取两行
	QTextStream in(&recent_kl_list);
	while (!in.atEnd())
	{
		QString kl_name = in.readLine();
		QString kl_path = in.readLine();

		QListWidgetItem* item = new QListWidgetItem(ui.listwidget);

		// 设置小号字体
		QFont font = item->font();
		font.setPointSize(10);
		item->setFont(font);
		
		item->setText(kl_name + '\n' + kl_path);
		ui.listwidget->addItem(item);
	}

	// 关闭文件
	recent_kl_list.close();

	return true;
}

// 槽：点击listWidget中的item，就进入到这个item对应的知识库，这里是新建一个main_window
void RecentKLWindow::listWidgetItemClicked(QListWidgetItem* item)
{
	// 获取点击的知识库名称和路径
	QString kl_name = item->text().split('\n')[0];
	QString kl_path = item->text().split('\n')[1];

	Status status;
	// 检查文件是否存在，如果不存在，提示用户“知识库不存在或已被移动”，提示是否删除这个库的记录
	QFile dir(kl_path);
	if (dir.exists()) 
	{
		main_window->openKnowledgeLibrary(kl_path);
		this->close();
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle("错误");
		msgBox.setText("知识库不存在或已被移动。是否删除该库的记录？");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.button(QMessageBox::Yes)->setText("是");
		msgBox.button(QMessageBox::No)->setText("否");
		QMessageBox::StandardButton reply = static_cast<QMessageBox::StandardButton>(msgBox.exec());

		if (reply == QMessageBox::Yes) {
			// 删除历史库记录
			status = removeKLFromRecentKLList(kl_name, kl_path);
			if (status == Status::Error)
			{
				QMessageBox::warning(this, "错误", "无法打开或操作文件：" + kl_path);
				return;
			}
			// 刷新listWidget
			RecentKLWindow::refreshListWidget();
		}
	}
}

// 槽：点击搜索按钮
void RecentKLWindow::searchButtonClicked() {
	// 获取搜索文本  
	QString search_text = ui.search_lineEdit->text();
	// 清空QListWidget中的旧结果  
	ui.listwidget->clear();

	// 检查搜索文本是否为空  
	if (search_text.isEmpty()) {
		QMessageBox::warning(this, "输入错误", "搜索文本不能为空。");
		return;
	}
	
	QString filePath = QDir(data_path).filePath("recent_kl_list.txt");
	// 注意：在Windows上，应使用正斜杠("/")或双反斜杠("\\")作为路径分隔符，  
	// 单个反斜杠("\")在字符串中是转义字符的开始。  

	// 打开文件  
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QString errorMessage = QString("无法打开文件，请检查文件路径：%1").arg(filePath);
		QMessageBox::warning(this, "错误", errorMessage);
		return;
	}

	// 读取文件内容  
	QTextStream in(&file);
	QString line;
	QStringList libraryNames;
	QStringList libraryPaths;
	int lineNumber = 0; // 引入行计数器  
	while (!in.atEnd()) {
		line = in.readLine().trimmed(); // 读取并修剪库名行  
		lineNumber++;

		if (lineNumber % 2 == 1) { // 奇数行，是库名  
			QString displayName = line; // 使用一个临时变量来存储将要显示的库名  

			libraryNames.append(displayName); // 将修改后的库名添加到列表中  

			// 无论库名是否匹配，都继续读取下一行路径（因为路径总是与库名成对出现）  
			if (!in.atEnd()) {
				line = in.readLine().trimmed(); // 读取并修剪路径行  
				libraryPaths.append(line); // 将路径添加到列表中  
				lineNumber++; // 路径行也计入行计数器（虽然循环会自动增加，但为了清晰还是写上）  
			}
		}
		// 注意：偶数行处理被隐含在上面的逻辑中了（即总是读取并添加路径，无论库名是否匹配）  
	}
	file.close();

	// 检查是否读取到任何内容  
	if (libraryNames.isEmpty() || libraryPaths.isEmpty()) {
		QMessageBox::warning(this, "错误", "recent_kl_list.txt 文件为空或格式不正确。");
		return;
	}

	// 确保库名和路径的数量匹配  
	if (libraryNames.size() != libraryPaths.size()) {
		QString message = QString("recent_kl_list.txt 文件中库名和路径的数量不匹配。\n库名数量: %1\n路径数量: %2")
			.arg(libraryNames.size())
			.arg(libraryPaths.size());
		QMessageBox::warning(this, "错误", message);
		return;
	}

	ui.hint_label->setText("搜索结果：");

	// 搜索并打印结果  
	for (int i = 0; i < libraryNames.count(); ++i) {
		if (libraryNames[i].contains(search_text, Qt::CaseInsensitive)) {
			// 如果库名包含搜索文本，则显示库名和路径  
			//item = new QListWidgetItem(libraryNames[i] + " \n " + libraryPaths[i]);
			QListWidgetItem* item = new QListWidgetItem(ui.listwidget);

			// 设置小号字体
			QFont font = item->font();
			font.setPointSize(10);
			item->setFont(font);

			item->setText(libraryNames[i] + '\n' + libraryPaths[i]);
			ui.listwidget->addItem(item);
		}
	}
	// 没有找到匹配项  
	if (ui.listwidget->count() == 0) {
		QMessageBox::information(this, "搜索结果", "没有找到匹配的库");
	}
};

// 槽：搜索框被修改后，检查搜索框内的文本是否是空，如果是空则加载历史
void RecentKLWindow::checkSearchLineEdit()
{
	// 这里是历史库就不需要刷新了
	if (ui.search_lineEdit->text().isEmpty() && ui.hint_label->text() != "最近打开的知识库：")
	{
		ui.listwidget->clear();
		ui.hint_label->setText("最近打开的知识库：");
		RecentKLWindow::refreshListWidget();  // 刷新两个listWidget
	}
}

// 槽：点击排序按钮，对listWidget中的item进行字典序排序
void RecentKLWindow::sortButtonClicked()
{
	// 获取listWidget中的所有item
	QList<QListWidgetItem*> items;
	while (ui.listwidget->count() > 0)
	{
		// takeItem()函数会从listWidget中移除item，但不会删除item（不释放item的空间）
		items.append(ui.listwidget->takeItem(0));  // 一直移除第一个item
	}

	// 对item进行升序排序
	std::sort(items.begin(), items.end(), [](QListWidgetItem* a, QListWidgetItem* b) {
		return a->text() < b->text();
		});

	ui.hint_label->setText("排序结果（字典序）：");

	// 重新添加item
	for (QListWidgetItem* item : items)
	{
		ui.listwidget->addItem(item);
	}
}

