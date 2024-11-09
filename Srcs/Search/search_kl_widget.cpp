#include <QDir>
#include <QMessageBox>

#include "search_kl_widget.h"
#include "ui_search_kl_widget.h"
#include "../MainWindow/KMMainWindow.h"
#include "../public.h"

search_kl_widget::search_kl_widget(KMMainWindow* _main_window, QWidget *parent, QString kl_name,bool ifseparatekl)
    : QWidget(parent)
	, father(parent)
    , now_kl_name(kl_name)
    , if_separate_kl(ifseparatekl)
	, main_window(_main_window)
{
    ui.setupUi(this);
    this->setWindowTitle("搜索库");
    initial();//初始化
     // 连接搜索框的 textChanged 信号到 onSearchTextChanged 槽函数
    connect(ui.search_text, &QLineEdit::textChanged, this, &search_kl_widget::searchTextChanged);
   // connect(ui.recent_kl, &QPushButton::clicked, this, &search_kl_widget::recentkl);
}

search_kl_widget::~search_kl_widget()
{
}
//初始化
void search_kl_widget::initial() {
    QString filePath = QDir(data_path).filePath("recent_kl_list.txt");// 定义文件路径（在当前路径下添加\data\kl_list.txt）   
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
            libraryNames.append(displayName); // 将库名添加到列表中（此时未修改）  

            // 读取下一行路径（总是与库名成对出现）  
            if (!in.atEnd()) {
                line = in.readLine().trimmed(); // 读取并修剪路径行    
                libraryPaths.append(line); // 将路径添加到列表中    
                lineNumber++; // 路径行也计入行计数器  
            }
        }
    }
    file.close();
	ui.show_listWidget->clear();  // 需要清空列表再添加新的内容
    QListWidgetItem* item;
    for (int i = 0; i < libraryNames.count(); ++i) {
        QString combinedText = libraryNames[i] + " - " + libraryPaths[i];
        item = new QListWidgetItem(combinedText);
        ui.show_listWidget->addItem(item);
    }
    disconnect(ui.show_listWidget, &QListWidget::itemClicked, this, &search_kl_widget::listklClicked);//断开之前的连接
    // 没有找到匹配项  
    if (ui.show_listWidget->count() == 0) {
        QMessageBox::information(this, "搜索结果", "没有最近使用的库。");
    }
    else {
        connect(ui.show_listWidget, &QListWidget::itemClicked, this, &search_kl_widget::listklClicked);//建立连接
    }
};

void search_kl_widget::searchTextChanged(const QString& text)
{
    if (text.isEmpty()) {
        initial();//调用初始化函数
    }
    else {
        searchclicked(); // 调用搜索函数
    }
}
//搜素库的函数
void search_kl_widget::searchclicked() {
    // 获取搜索文本  
    QString search_text = ui.search_text->text();
   // 清空QListWidget中的旧结果  
    ui.show_listWidget->clear();
    // 定义文件路径（在当前路径下添加\data\kl_list.txt）  
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

    // 搜索并打印结果  
    QListWidgetItem* item;
    for (int i = 0; i < libraryNames.count(); ++i) {
        if (libraryNames[i].contains(search_text, Qt::CaseInsensitive)) {
            // 如果库名包含搜索文本，则显示库名和路径  
            item = new QListWidgetItem(libraryNames[i] + " - " + libraryPaths[i]);
            ui.show_listWidget->addItem(item);
        }
    }
    disconnect(ui.show_listWidget, &QListWidget::itemClicked, this, &search_kl_widget::listklClicked);//断开之前的连接
    // 没有找到匹配项  
    if (ui.show_listWidget->count() == 0) {
    }
    else {
        connect(ui.show_listWidget, &QListWidget::itemClicked, this, &search_kl_widget::listklClicked);//建立连接
    }
}


void search_kl_widget::listklClicked(QListWidgetItem* item) {
    if (item) {
        QString fullText = item->text(); // 获取完整的文本，包含名字和路径  
        QStringList parts = fullText.split("-"); // 使用 "-" 分割字符串  
        QString fileName;
        QString searchPath;
        if (parts.size() >= 2) {
             fileName = parts[0].trimmed(); // 获取前半段作为文件名，并去除可能的空白字符  
             searchPath = parts[1].trimmed(); // 获取后半段作为路径，并去除可能的空白字符  
             // 此时，fileName 包含名字，searchPath 包含路径  
        }
        else {
            QMessageBox::warning(nullptr, "格式错误", "项目文本格式不正确，请确保使用 '-' 分隔名字和路径。", QMessageBox::Ok);
        }
        QString filePath = searchPath;
		main_window->openKnowledgeLibrary(filePath);  // openKnowledgeLibrary函数遇到错误会弹出错误对话框
        this->close();
     }
} 

void search_kl_widget::gobackseparatekl() {
    this->show();
};