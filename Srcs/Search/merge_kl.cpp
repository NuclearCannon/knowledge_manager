#include <QDir>
#include <QMessageBox>
#include <QObject.h>
#include <QColor>  
#include <QMap>  
#include <QListWidget>  
#include <QString>  
#include <QList>  
#include <QPair>  
#include <map> 

#include "merge_kl.h"
#include "ui_merge_kl.h"
#include "../MainWindow/KMMainWindow.h"
#include "../StartWindow/new_kl_guidance.h"
#include "merge_kl_same_name.h"
#include "../public.h"

extern QString data_path;
merge_kl::merge_kl(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::merge_kl)
{
    ui->setupUi(this);
    this->setWindowTitle("合库");
    merge_initializa_search_kl_list();//初始化搜索框

    
    connect(ui->pushButton_2, &QPushButton::clicked, this, &merge_kl::merge_searchclicked);//搜索按钮被点击,,核心按钮
    connect(ui->show_listWidget, &QListWidget::itemClicked, this, &merge_kl::merge_onListklClicked);//库列表中item被点击
    connect(ui->search_text, &QLineEdit::textChanged, this, &merge_kl::onSearchTextChanged);
    connect(ui->pushButton, &QPushButton::clicked, this, &merge_kl::allchoosedclicked);//全选
    connect(ui->pushButton_3, &QPushButton::clicked, this, &merge_kl::allclearedclicked);//清空
}
merge_kl::~merge_kl()
{
    delete ui;
}

void merge_kl::merge_initializa_search_kl_list() {
    ui->show_listWidget_2->clear();
    QString filePath = QDir(data_path).filePath("recent_kl_list.txt");// 定义文件路径（在当前路径下添加\data\recent_kl_list.txt） 
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
    disconnect(ui->show_listWidget_2, &QListWidget::itemClicked, this, &merge_kl::merge_onSearchListklClicked);
    connect(ui->show_listWidget_2, &QListWidget::itemClicked, this, &merge_kl::merge_onSearchListklClicked);

    //合库后删除原库按钮初始化
    ui->checkBox_2->setChecked(false);

};

void  merge_kl::onSearchTextChanged(const QString& text)
{
    if (text.isEmpty()) {
        merge_initializa_search_kl_list();//调用初始化函数
    }
    else {
        merge_searchklclicked(); // 调用搜索函数
    }
}
//搜素库按钮点击触发
void merge_kl::merge_searchklclicked() {
    QString filePath = QDir(data_path).filePath("recent_kl_list.txt");// 定义文件路径（在当前路径下添加\data\recent_kl_list.txt） 

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
        QMessageBox::warning(this, "错误", "kl_list.txt 文件为空或格式不正确。");
        return;
    }

    // 确保库名和路径的数量匹配    
    if (libraryNames.size() != libraryPaths.size()) {
        QString message = QString("kl_list.txt 文件中库名和路径的数量不匹配。\n库名数量: %1\n路径数量: %2")
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
        if ( libraryNames[i].contains(search_text, Qt::CaseInsensitive)&&!alreadyExists ) {
            item = new QListWidgetItem(combinedText);
            ui->show_listWidget_2->addItem(item);
        }
    }

    // 断开并重新连接信号，以确保只有在有搜索结果时才响应点击事件  
    disconnect(ui->show_listWidget_2, &QListWidget::itemClicked, this, &merge_kl::merge_onSearchListklClicked);
    if (ui->show_listWidget_2->count() == 0) {
    
    }
    else {
        connect(ui->show_listWidget_2, &QListWidget::itemClicked, this, &merge_kl::merge_onSearchListklClicked);
    }
};
//合库按钮点击触发
void merge_kl::merge_searchclicked() {
    if (ui->show_listWidget->count() == 0) {
        QMessageBox::warning(this, "警告", "没有选择任何库！");
        return;
    }
    bool foundMatch = false;
    QList<QPair<QString, QString>> libraryPaths; // 用于存储库名和路径的对  
    // 遍历 multiklnameandpath（这里假设是ui->show_listWidget中的内容）中的所有项，提取库名和路径  
    for (int i = 0; i < ui->show_listWidget->count(); ++i) {
        QListWidgetItem* item = ui->show_listWidget->item(i);
        QString itemText = item->text();
        int separatorIndex = itemText.indexOf("-");
        if (separatorIndex != -1) {
            QString libraryName = itemText.left(separatorIndex).trimmed();
            QString path = itemText.mid(separatorIndex + 1).trimmed();
            libraryPaths.append(qMakePair(libraryName, path));
        }
    }

    // 创建一个新的 QListWidget 来显示结果  
    QListWidget* resultListWidget = new QListWidget();
    QHash<QString, int> fileNameCounts;
    QHash<int, QString> fileCountsName;
    QHash<QString, int> fileNameCounts1;
    int i = 0;//存不重名的个数
    for (const auto& libraryPathPair : libraryPaths) {
        QString libraryName = libraryPathPair.first;
        QString path = libraryPathPair.second;
        QDir dir(path);
        if (!dir.exists()) {
            QMessageBox::warning(this, "错误", "源文件不存在！"+ path);
            return; // 停止运行
        }
        QStringList filters;
        filters << "*.txt";
        QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);
        
        for (const QFileInfo& fileInfo : fileList) {
            QString fileName = fileInfo.fileName();

            // 更新文件名出现的次数  
            fileNameCounts[fileName]++;

            // 检查是否至少有两个相同的文件名  
            if (fileNameCounts[fileName] < 2) {
                QString fullItemText = QString("%1-%2-%3").arg(libraryName).arg(fileName).arg(path);
                fileCountsName[i] = fullItemText;
                fileNameCounts1[fileName] = i;
                i++;
            }
            else  if (fileNameCounts[fileName] == 2) {//遇到的第二次，所以也要把第一次的加进去
                QString fullItemText = fileCountsName[fileNameCounts1[fileName]];
                QListWidgetItem* resultItem = new QListWidgetItem(fullItemText);
            
                resultListWidget->addItem(resultItem);
                fullItemText = QString("%1-%2-%3").arg(libraryName).arg(fileName).arg(path);
                resultItem = new QListWidgetItem(fullItemText);
              
                resultListWidget->addItem(resultItem);
          
                foundMatch = true;
            }else if (fileNameCounts[fileName] > 2) {//遇到的大于三次，直接装进去即可
                bool foundMatch1 = false; // 初始化标志位  
                QString fullItemText; // 用于存储完整的item文本  
                QList<QListWidgetItem*> items = resultListWidget->findItems("", Qt::MatchContains); // 获取所有item，这里使用空字符串和MatchContains是为了获取所有item进行遍历，但后面我们会根据fileName进行过滤  

                // 遍历所有item  
                for (QListWidgetItem* item : items) {
                    QStringList parts = item->text().split("-");
                    if (parts[1] == fileName) { // 假设fileName是第二部分  
                        foundMatch1 = true;
                        // 在找到匹配的item之后插入新的item  
                        // 注意：这里我们假设fullItemText已经根据新的libraryName、fileName和path构建好了  
                        fullItemText = QString("%1-%2-%3").arg(libraryName).arg(fileName).arg(path);
                        QListWidgetItem* newItem = new QListWidgetItem(fullItemText);
                        int rowIndex = resultListWidget->row(item) + 1; // 获取匹配item的行索引，并加1以在其后插入  
                   
                        resultListWidget->insertItem(rowIndex, newItem);

                        // 跳出循环，因为我们已经找到了匹配的item并插入了新的item  
                        break;
                    }
                }
            }
        }
    }
    // 如果没有找到匹配项
    if (!foundMatch) {
    delete resultListWidget; 
    NewKLGuidance* merge_NewKLGuidance = new NewKLGuidance(this);
    connect(merge_NewKLGuidance, &NewKLGuidance::mergekl, this,&merge_kl::creactmergekl);
    merge_NewKLGuidance->setWindowFlags(Qt::Window); // 确保它作为一个独立的窗口显示
    merge_NewKLGuidance->show();
    this->close();
    }
    else {
        merge_kl_same_name* new_merge_kl_same_name = new  merge_kl_same_name(this, resultListWidget);
        connect(new_merge_kl_same_name, &merge_kl_same_name::samename_puttonewklOK, this, &merge_kl::creactmergekl_same_name);
        connect(new_merge_kl_same_name, &merge_kl_same_name::goback, this, &merge_kl::gobackOK);
        new_merge_kl_same_name->setWindowFlags(Qt::Window); // 确保它作为一个独立的窗口显示
        new_merge_kl_same_name->show();
        delete resultListWidget;
        this->close();
    }
};
//搜素库item点击触发
void merge_kl::merge_onSearchListklClicked(QListWidgetItem* item) {
  if (!item) {
        return; // 如果没有选中任何有效项，或者项不在show_listWidget_2中，则直接返回  
   }
    // 从show_listWidget_2中移除项，并获取其所有权  
    QListWidgetItem* movedItem = ui->show_listWidget_2->takeItem(ui->show_listWidget_2->row(item));
    // 将移除的项添加到show_listWidget中  
    // 注意：此时movedItem的所有权已经转移给了show_listWidget，我们不需要（也不应该）删除它  
    ui->show_listWidget->addItem(movedItem);
    // 由于movedItem现在属于show_listWidget，我们不需要进行任何额外的内存管理  
};
//库列表item点击触发
void merge_kl::merge_onListklClicked(QListWidgetItem* item) {
    if (!item) {
        return; // 如果没有选中任何有效项，或者项不在show_listWidget中，则直接返回  
    }
    // 从show_listWidget中移除项，并获取其指针（但在这里我们不需要保留它）  
    QListWidgetItem* removedItem = ui->show_listWidget->takeItem(ui->show_listWidget->row(item));
    QString text = ui->search_text->text();
    QStringList parts = item->text().split("-");
    if (parts[0].contains(text)) {
        ui->show_listWidget_2->addItem(item->clone());
        ui->show_listWidget->takeItem(ui->show_listWidget->row(item));
        disconnect(ui->show_listWidget_2, &QListWidget::itemClicked, this, &merge_kl::merge_onSearchListklClicked);
        connect(ui->show_listWidget_2, &QListWidget::itemClicked, this, &merge_kl::merge_onSearchListklClicked);
    }
    else {
        ui->show_listWidget->takeItem(ui->show_listWidget->row(item));
    }
};
//处理没有命名冲突的新库
void merge_kl::creactmergekl(QString kl_name,QString kl_path) {
    // 创建一个 QDir 对象，指向目标文件夹，如果文件夹不存在则创建它  
    QDir targetDir(kl_path);
    if (!targetDir.exists()) {
        targetDir.mkpath(".");
    }

    // 获取 QListWidget 中的所有 item  
    QList<QListWidgetItem*> items = ui->show_listWidget->findItems("", Qt::MatchContains);

    // 遍历每个 item  
    for (QListWidgetItem* item : items) {
        QString itemText = item->text();

        // 假设 itemText 的格式为 "库名-路径"，使用 "-" 分割字符串  
        QStringList parts = itemText.split("-");
        QString libraryName = parts[0].trimmed(); // 库名  
        QString sourcePath = parts[1].trimmed(); // 路径  

        // 创建 QDir 对象，指向源文件夹
        QDir sourceDir(sourcePath);
        if (!sourceDir.exists()) {
            QMessageBox::warning(this, "错误", "源文件不存在！");
            targetDir.removeRecursively();
            if (removeKLFromRecentKLList(kl_name, kl_path) == Status::Error)
            {
                QMessageBox::warning(this, "错误", "文件无法打开或操作" + kl_path);
                return;
            }
            return; // 停止运行
        }

        // 尝试复制源文件夹中的内容到目标文件夹
        if (!copyDirectory(sourceDir, targetDir)) {
            // 如果复制操作失败，则打印错误信息并停止运行
            QMessageBox::warning(this, "错误", "合库失败！");
            if (removeKLFromRecentKLList(kl_name, kl_path) == Status::Error)
            {
                QMessageBox::warning(this, "错误", "文件无法打开或操作" + kl_path);
                return;
            }
            targetDir.removeRecursively();
            return; // 停止运行
        }

    }
    if (ui->checkBox_2->isChecked()){
        // 遍历每个源路径并删除（请谨慎使用!!!!!，确保数据已安全复制）  
        for (QListWidgetItem* item : items) {
            QString itemText = item->text();
            QStringList parts = itemText.split("-");
            QString libraryName = parts[0].trimmed();
            QString sourcePath = parts[1].trimmed();
            QDir sourceDir(sourcePath);

            // 如果源文件夹存在，则删除它及其内容  
            if (sourceDir.exists()) {
                bool success = sourceDir.removeRecursively();
                if (!success) {
                    QMessageBox::warning(this, "警告", "删除失败");

                }
            }           
            if (removeKLFromRecentKLList(libraryName, sourcePath) == Status::Error)
            {
                QMessageBox::warning(this, "错误", "文件无法打开或操作" + sourcePath);
                return;
            }
        }
    }
};

//处理有命名冲突的新库
void merge_kl::creactmergekl_same_name(QString kl_name,QString kl_path, int comboBox) {
    bool foundMatch = false;
    QList<QPair<QString, QString>> libraryPaths; // 用于存储库名和路径的对  
    // 遍历 multiklnameandpath（这里假设是ui->show_listWidget中的内容）中的所有项，提取库名和路径  
    for (int i = 0; i < ui->show_listWidget->count(); ++i) {
        QListWidgetItem* item = ui->show_listWidget->item(i);
        QString itemText = item->text();
        int separatorIndex = itemText.indexOf("-");
        if (separatorIndex != -1) {
            QString libraryName = itemText.left(separatorIndex).trimmed();
            QString path = itemText.mid(separatorIndex + 1).trimmed();
            libraryPaths.append(qMakePair(libraryName, path));
        }
    }
    // 创建一个新的 QListWidget 来显示结果  
    QListWidget* resultListWidget = new QListWidget();
    QHash<QString, int> fileNameCounts;
    QHash<int, QString> fileCountsName;
    QHash<QString, int> fileNameCounts1;
    int i = 0;//存不重名的个数
    for (const auto& libraryPathPair : libraryPaths) {
        QString libraryName = libraryPathPair.first;
        QString path = libraryPathPair.second;
        QDir dir(path);
        if (!dir.exists()) {
            QMessageBox::warning(this, "错误", "源文件不存在："+path);
            return; // 停止运行
        }
        QStringList filters;
        filters << "*.txt";
        QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);

        for (const QFileInfo& fileInfo : fileList) {
            QString fileName = fileInfo.fileName();

            // 更新文件名出现的次数  
            fileNameCounts[fileName]++;

            // 检查是否至少有两个相同的文件名  
            if (fileNameCounts[fileName] < 2) {
                QString fullItemText = QString("%1-%2-%3").arg(libraryName).arg(fileName).arg(path);
                fileCountsName[i] = fullItemText;
                fileNameCounts1[fileName] = i;
                i++;
            }
            else  if (fileNameCounts[fileName] == 2) {//遇到的第二次，所以也要把第一次的加进去
                QString fullItemText = fileCountsName[fileNameCounts1[fileName]];
                QListWidgetItem* resultItem = new QListWidgetItem(fullItemText);
                resultListWidget->addItem(resultItem);
                fullItemText = QString("%1-%2-%3").arg(libraryName).arg(fileName).arg(path);
                resultItem = new QListWidgetItem(fullItemText);
                resultListWidget->addItem(resultItem);

                foundMatch = true;
            }
            else if (fileNameCounts[fileName] >= 2) {//遇到的大于三次，直接装进去即可
                bool foundMatch1 = false; // 初始化标志位  
                QString fullItemText; // 用于存储完整的item文本  
                QList<QListWidgetItem*> items = resultListWidget->findItems("", Qt::MatchContains); // 获取所有item，这里使用空字符串和MatchContains是为了获取所有item进行遍历，但后面我们会根据fileName进行过滤  

                // 遍历所有item  
                for (QListWidgetItem* item : items) {
                    QStringList parts = item->text().split("-");
                    if (parts[1] == fileName) { // 假设fileName是第二部分  
                        foundMatch1 = true;
                        // 在找到匹配的item之后插入新的item  
                        // 注意：这里我们假设fullItemText已经根据新的libraryName、fileName和path构建好了  
                        fullItemText = QString("%1-%2-%3").arg(libraryName).arg(fileName).arg(path);
                        QListWidgetItem* newItem = new QListWidgetItem(fullItemText);
                        int rowIndex = resultListWidget->row(item) + 1; // 获取匹配item的行索引，并加1以在其后插入  
                        resultListWidget->insertItem(rowIndex, newItem);

                        // 跳出循环，因为我们已经找到了匹配的item并插入了新的item  
                        break;
                    }
                }
            }
        }
    }
    // 创建一个 QDir 对象，指向目标文件夹，如果文件夹不存在则创建它  
    QDir targetDir(kl_path);
    if (!targetDir.exists()) {
        targetDir.mkpath(".");
    }

    // 获取 QListWidget 中的所有 item  
    QList<QListWidgetItem*> items = ui->show_listWidget->findItems("", Qt::MatchContains);

    // 遍历每个 item  
    for (QListWidgetItem* item : items) {
        QString itemText = item->text();

        // 假设 itemText 的格式为 "库名-路径"，使用 "-" 分割字符串  
        QStringList parts = itemText.split("-");
        QString libraryName = parts[0].trimmed(); // 库名  
        QString sourcePath = parts[1].trimmed(); // 路径  

        // 创建 QDir 对象，指向源文件夹  
        QDir sourceDir(sourcePath);
        if (!sourceDir.exists()) {
            // 如果源文件夹不存在，则打印错误信息并跳过该路径  
            QMessageBox::warning(this, "错误", "源文件不存在:"+sourcePath);
            targetDir.removeRecursively();
			if (removeKLFromRecentKLList(kl_name, kl_path) == Status::Error)
			{
				QMessageBox::warning(this, "错误", "文件无法打开或操作" + kl_path);
				return;
			}
            return; // 停止运行
        }

        // 复制源文件夹中的内容到目标文件夹
        if (!NOTcopyDirectory(sourceDir, targetDir, resultListWidget,comboBox)) {
            QMessageBox::warning(this, "错误", "合库失败！" );
            if (removeKLFromRecentKLList(kl_name, kl_path) == Status::Error)
            {
                QMessageBox::warning(this, "错误", "文件无法打开或操作" + kl_path);
                return;
            }
            targetDir.removeRecursively();
            return; // 停止运行
        };

    }
    if (ui->checkBox_2->isChecked()) {
        // 遍历每个源路径并删除（请谨慎使用!!!!!，确保数据已安全复制）  
        for (QListWidgetItem* item : items) {
            QString itemText = item->text();
            QStringList parts = itemText.split("-");
            QString libraryName = parts[0].trimmed();
            QString sourcePath = parts[1].trimmed();
            QDir sourceDir(sourcePath);

            // 如果源文件夹存在，则删除它及其内容  
            if (sourceDir.exists()) {
                bool success = sourceDir.removeRecursively();
                if (!success) {
                    QMessageBox::warning(this, "警告", "删除失败");
                }
            }
            //removeKLFromKLList(libraryName, this);
            if (removeKLFromRecentKLList(libraryName, sourcePath) == Status::Error)
            {
                QMessageBox::warning(this, "错误", "文件无法打开或操作" + sourcePath);
                return;
            }
        }
    }

}

bool merge_kl::copyDirectory(QDir sourceDir, QDir targetDir) {
    QFileInfoList fileList = sourceDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files, QDir::DirsFirst | QDir::Name);
    QStringList failedFiles;

    for (const QFileInfo& fileInfo : fileList) {
        QString relativePath = sourceDir.relativeFilePath(fileInfo.absoluteFilePath());
        QString destPath = targetDir.filePath(relativePath);

        if (fileInfo.isDir()) {
            if (!QDir(destPath).exists()) {
                if (!QDir().mkpath(destPath)) {
                    failedFiles.append(QString("创建目录失败: %1").arg(destPath));
                    continue;
                }
            }
            copyDirectory(fileInfo.absoluteFilePath(), destPath);
        }
        else {
            if (!QFile::copy(fileInfo.absoluteFilePath(), destPath)) {
                failedFiles.append(QString("没有命名冲突，复制文件失败: %1\n").arg(fileInfo.absoluteFilePath()));
            }
        }
    }

    if (!failedFiles.isEmpty()) {
        QMessageBox::warning(this, "警告", "以下操作失败:\n" + failedFiles.join("\n"));
        return 0;
    }
    return 1;
}
//将所有库中不包含resultListWidget的内容写进去
bool merge_kl::NOTcopyDirectory(QDir sourceDir, QDir targetDir, QListWidget* resultListWidget,int comboBox) {
    // 获取resultListWidget中所有的文件名
    QStringList excludedFileNames;
    QStringList excludedkl_FileNames;
    QStringList failedFiles;
    for (int i = 0; i < resultListWidget->count(); ++i) {
        QString itemText = resultListWidget->item(i)->text();
        // 假设格式为 “库名-文件名-路径”，提取文件名部分
        QStringList parts = itemText.split("-");
        if (parts.size() > 1) {
            excludedFileNames.append(parts.at(1));
            if (comboBox == 0) {
                excludedkl_FileNames.append(parts.at(0)+"_"+parts.at(1));//保证添加完重名的之后更新筛选列表
            }
            else if (comboBox == 1) {
                // 分离文件名和后缀
                QString baseName = parts.at(1).left(parts.at(1).lastIndexOf('.'));
                QString suffix = parts.at(1).mid(parts.at(1).lastIndexOf('.'));
                excludedkl_FileNames.append(baseName +"("+parts.at(0)+")"+ suffix);//保证添加完重名的之后更新筛选列表
            }
        }
    }

    // 遍历源文件夹中的所有文件和子文件夹
    QFileInfoList fileList = sourceDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files, QDir::DirsFirst | QDir::Name);
    for (const QFileInfo& fileInfo : fileList) {
        QString relativePath = sourceDir.relativeFilePath(fileInfo.absoluteFilePath());
        QString destPath = targetDir.filePath(relativePath);

        if (fileInfo.isDir()) {
            // 如果是文件夹，则创建对应的文件夹及其所有父文件夹
            if (!QDir(destPath).exists()) {
                QDir().mkpath(destPath);
            }
            // 递归调用以复制子文件夹的内容
           // NOTcopyDirectory(fileInfo.absoluteFilePath(), destPath, resultListWidget);//暂时不写这个
        }
        else {
            // 检查文件是否在排除列表中
            if (!excludedFileNames.contains(fileInfo.fileName())) {
                if (!excludedkl_FileNames.contains(fileInfo.fileName())) {
                     // 如果是文件且不在排除列表中，则复制文件
                    if (!QFile::copy(fileInfo.absoluteFilePath(), destPath)) {
                        // 如果复制失败，则显示警告消息框，并包含失败的文件路径
                        failedFiles.append(QString("有命名冲突，复制文件失败: %1\n").arg(fileInfo.absoluteFilePath()));
                        //QMessageBox::warning(nullptr, "警告", QString("复制文件失败: %1").arg(fileInfo.absoluteFilePath()));
                    }

                }
                //保证（那些本来不重名但是加入重命名的之后重名了的）全部加进去
                else {
                    // 分离文件名和后缀
                    QString baseName = fileInfo.fileName().left(fileInfo.fileName().lastIndexOf('.'));
                    QString suffix = fileInfo.fileName().mid(fileInfo.fileName().lastIndexOf('.'));
                    // 在文件名后缀前插入 elsefilename
                    fileInfo.fileName() = baseName + "(" + "原-" + sourceDir.dirName() + ")" + suffix;
                    QString newFileName =  fileInfo.fileName();
                    QString newDestPath = targetDir.filePath(newFileName);
                    if (!QFile::copy(fileInfo.absoluteFilePath(), newDestPath)) {
                        // 如果复制失败，则显示警告消息框，并包含失败的文件路径
                        failedFiles.append(QString("有命名冲突，复制文件失败: %1\n").arg(fileInfo.absoluteFilePath()));
                        //QMessageBox::warning(nullptr, "警告", QString("复制文件失败: %1").arg(fileInfo.absoluteFilePath()));
                    }

                }
            }
        }
    }
    if (!failedFiles.isEmpty()) {
        QMessageBox::warning(this, "警告", "以下操作失败:\n" + failedFiles.join("\n"));
        return 0;
    }
    return 1;
}
//全选按钮
void merge_kl::allchoosedclicked() {
    int count = ui->show_listWidget_2->count();
    for (int i = 0; i < count; ++i) {
        QListWidgetItem* item = ui->show_listWidget_2->takeItem(0); // 从列表中移除第一个 item 并返回它  
        ui->show_listWidget->addItem(item); // 将移除的 item 添加到另一个列表中  
    }
};
//全部删除
void merge_kl::allclearedclicked() {
    int count = ui->show_listWidget->count();
    for (int i = 0; i < count; ++i) {
        QListWidgetItem* item = ui->show_listWidget->takeItem(0); // 从列表中移除第一个 item 并返回它  
        merge_onListklClicked(item);
    }
    disconnect(ui->show_listWidget_2, &QListWidget::itemClicked, this, &merge_kl::merge_onSearchListklClicked);
    connect(ui->show_listWidget_2, &QListWidget::itemClicked, this, &merge_kl::merge_onSearchListklClicked);
}

void merge_kl::gobackOK() {
    this->show();
};
