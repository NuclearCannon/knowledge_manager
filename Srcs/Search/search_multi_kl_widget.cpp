#include <QDir>
#include <QMessageBox>
#include <QObject.h>

#include "search_multi_kl_widget.h"
#include "ui_search_multi_kl_widget.h"
#include "search_entry_widget.h"
#include "../MainWindow/KMMainWindow.h"


extern QString data_path;


search_multi_kl_widget::search_multi_kl_widget(QWidget *parent, QString kl_name )
    : QWidget(parent)
    , now_kl_name(kl_name)
    , ui(new Ui::search_multi_kl_widget)
{
    ui->setupUi(this);
    this->setWindowTitle("多库搜索");
    initializa_search_kl_list();//初始化搜索框
  
    connect(ui->search_text, &QLineEdit::textChanged, this, &search_multi_kl_widget::onSearchTextChanged);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &search_multi_kl_widget::searchclicked);//搜索按钮被点击
    connect(ui->show_listWidget, &QListWidget::itemClicked, this, &search_multi_kl_widget::onListklClicked);//库列表中item被点击
    connect(ui->pushButton, &QPushButton::clicked, this, &search_multi_kl_widget::allchoosedclicked);//全选
    connect(ui->pushButton_3, &QPushButton::clicked, this, &search_multi_kl_widget::allclearedclicked);//清空
}


search_multi_kl_widget::~search_multi_kl_widget()
{
    delete ui;
}
inline void search_multi_kl_widget::initializa_search_kl_list() {
    ui->show_listWidget_2->clear();
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
    QListWidgetItem* item;
    for (int i = 0; i < libraryNames.count(); ++i) {
        QString combinedText = libraryNames[i] + " - " + libraryPaths[i];
        // 检查 ui->show_listWidget 中是否已存在相同的词条  
        bool alreadyExists = false;
        for (int j = 0; j < ui->show_listWidget->count(); ++j) {
            QListWidgetItem* existingItem = ui->show_listWidget->item(j);
            if (existingItem->text() == combinedText) {
                alreadyExists = true;
                break;
            }
        }
        // 如果不存在，则添加到 ui->show_listWidget_2  
        if (!alreadyExists) {
            item = new QListWidgetItem(combinedText);
            ui->show_listWidget_2->addItem(item);
        }
    }
    disconnect(ui->show_listWidget_2, &QListWidget::itemClicked, this, &search_multi_kl_widget::onSearchListklClicked);
    connect(ui->show_listWidget_2, &QListWidget::itemClicked, this, &search_multi_kl_widget::onSearchListklClicked);
};
void search_multi_kl_widget::onSearchTextChanged(const QString& text)
{
    if (text.isEmpty()) {
        initializa_search_kl_list();//调用初始化函数
    }
    else {
        searchklclicked(); // 调用搜索函数
    }
}
//搜素库按钮点击触发
void search_multi_kl_widget::searchklclicked() {
    QString filePath = QDir(data_path).filePath("recent_kl_list.txt");// 定义文件路径（在当前路径下添加\data\kl_list.txt） 
   
    // 获取搜索文本    
    QString search_text = ui->search_text->text();
    // 清空QListWidget中的旧结果    
    ui->show_listWidget_2->clear();

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

   
    QListWidgetItem* item;
    for (int i = 0; i < libraryNames.count(); ++i) {
        QString combinedText = libraryNames[i] + " - " + libraryPaths[i];

        // 检查 ui->show_listWidget 中是否已存在相同的词条  
        bool alreadyExists = false;
        for (int j = 0; j < ui->show_listWidget->count(); ++j) {
            QListWidgetItem* existingItem = ui->show_listWidget->item(j);
            if (existingItem->text() == combinedText) {
                alreadyExists = true;
                break;
            }
        }
        // 如果不存在，则添加到 ui->show_listWidget_2  
        if (!alreadyExists && libraryNames[i].contains(search_text, Qt::CaseInsensitive)) {
            item = new QListWidgetItem(combinedText);
            ui->show_listWidget_2->addItem(item);
        }
    }

    // 断开并重新连接信号，以确保只有在有搜索结果时才响应点击事件  
    disconnect(ui->show_listWidget_2, &QListWidget::itemClicked, this, &search_multi_kl_widget::onSearchListklClicked);
    if (ui->show_listWidget_2->count() == 0) {
       
    }
    else {
        connect(ui->show_listWidget_2, &QListWidget::itemClicked, this, &search_multi_kl_widget::onSearchListklClicked);
    }
};
//搜索按钮点击触发
void search_multi_kl_widget::searchclicked() { 
    if (ui->show_listWidget->count() == 0) {
        QMessageBox::warning(this, "警告", "没有选择任何库！");
        return;
    }
    search* new_search_widget = new search(this, ui->show_listWidget);
    connect(new_search_widget, &search::updateTabWidget, this, &search_multi_kl_widget::search_multi_kl_updateTabWidget);//建立连接，打开相应文件
    connect(new_search_widget, &search::gobacktoklOK, this, &search_multi_kl_widget::goback_from_entry_widget);//返回
    new_search_widget->setWindowFlags(Qt::Window); // 确保它作为一个独立的窗口显示
    new_search_widget->setWindowTitle("多库搜索");
    new_search_widget->show();
    this->close();
};
// 搜索库item点击触发，将项从show_listWidget_2移动到show_listWidget  
void search_multi_kl_widget::onSearchListklClicked(QListWidgetItem* item) {
    if (!item) {
        return; // 如果没有选中任何有效项，或者项不在show_listWidget_2中，则直接返回  
    }
    // 从show_listWidget_2中移除项，并获取其所有权  
    QListWidgetItem* movedItem = ui->show_listWidget_2->takeItem(ui->show_listWidget_2->row(item));
    // 将移除的项添加到show_listWidget中  
    // 注意：此时movedItem的所有权已经转移给了show_listWidget，我们不需要（也不应该）删除它  
    ui->show_listWidget->addItem(movedItem);
    // 由于movedItem现在属于show_listWidget，我们不需要进行任何额外的内存管理  
}
// 库列表item点击触发，将项从show_listWidget中移动回去 
void search_multi_kl_widget::onListklClicked(QListWidgetItem* item) {
    if (!item) {
        return; // 如果没有选中任何有效项，或者项不在show_listWidget中，则直接返回  
    }
   
    QString text = ui->search_text->text();
    QStringList parts = item->text().split("-");
    if (parts[0].contains(text)) {
        ui->show_listWidget_2->addItem(item->clone());
        ui->show_listWidget->takeItem(ui->show_listWidget->row(item));
        disconnect(ui->show_listWidget_2, &QListWidget::itemClicked, this, &search_multi_kl_widget::onSearchListklClicked);
        connect(ui->show_listWidget_2, &QListWidget::itemClicked, this, &search_multi_kl_widget::onSearchListklClicked);
    }
    else {
        ui->show_listWidget->takeItem(ui->show_listWidget->row(item));
    }
}
//全选按钮
void search_multi_kl_widget::allchoosedclicked() {
        int count = ui->show_listWidget_2->count();
        for (int i = 0; i < count; ++i) {
            QListWidgetItem* item =ui->show_listWidget_2->takeItem(0); // 从列表中移除第一个 item 并返回它  
            ui->show_listWidget->addItem(item); // 将移除的 item 添加到另一个列表中  
        }
};
//全部删除
void search_multi_kl_widget::allclearedclicked() {
        int count = ui->show_listWidget->count();
        for (int i = 0; i < count; ++i) {
            QListWidgetItem* item = ui->show_listWidget->takeItem(0); // 从列表中移除第一个 item 并返回它  
            onListklClicked(item);
        }
        disconnect(ui->show_listWidget_2, &QListWidget::itemClicked, this, &search_multi_kl_widget::onSearchListklClicked);
        connect(ui->show_listWidget_2, &QListWidget::itemClicked, this, &search_multi_kl_widget::onSearchListklClicked);
}

void search_multi_kl_widget::goback_from_entry_widget() {
    this->show();
};