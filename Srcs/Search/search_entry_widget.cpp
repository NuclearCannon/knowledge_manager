#include <QDir>
#include <QMessageBox>
#include <QButtonGroup>
#include <qoverload.h>

#include "../EntryWidget/EntryWidget.h"
#include "search_multi_kl_widget.h"
#include "search_entry_widget.h"
#include "ui_search_entry_widget.h"

search::search(QWidget* parent, QListWidget* muti_kl_nameandpath)
    : QWidget(parent)
    , father(parent)
    , multiklnameandpath(muti_kl_nameandpath)
    , buttonGroup(new QButtonGroup(this))  // 初始化 QButtonGroup
    , ui(new Ui::search)
{
    ui->setupUi(this);
    this->setWindowTitle("搜索词条");
    // 如果不是从多库进入的，那么隐藏按钮
    search_multi_kl_widget* parent_window1 = dynamic_cast<search_multi_kl_widget*>(this->parent());
    if (parent_window1){ui->pushButton->setVisible(true);}  else { ui->pushButton->setVisible(false);}
    ui->checkBox->setChecked(true);
    initial();
  
    connect(ui->pushButton, &QPushButton::clicked, this, &search::open_go_back);

    // 初始化 QButtonGroup
    //QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->checkBox, 0);
    buttonGroup->addButton(ui->checkBox_2, 1);
    buttonGroup->addButton(ui->checkBox_3, 2);

    connect(ui->search_text, &QLineEdit::textChanged, this, &search::onSearchTextChanged);//文本框内容改变
    // 连接复选框的状态变化信号到槽函数
    connect(buttonGroup, &QButtonGroup::idClicked, this, &search::onCheckBoxStateChanged);//选择框内容改变
}

search::~search()
{
    delete ui;
}
void search::initial() {
    QList<QPair<QString, QString>> libraryPaths; // 用于存储库名和路径的对  
    // 清空QListWidget中的旧结果  
    ui->show_listWidget->clear();
  
    // 遍历 multiklnameandpath 中的所有项，提取库名和路径
    for (int i = 0; i < multiklnameandpath->count(); ++i) {
        QListWidgetItem* item = multiklnameandpath->item(i);
        QString itemText = item->text();
        int separatorIndex = itemText.indexOf("-"); // 找到名字和路径之间的分隔符
        if (separatorIndex != -1) {
            QString libraryName = itemText.left(separatorIndex).trimmed(); // 提取库名部分
            QString path = itemText.mid(separatorIndex + 1).trimmed(); // 提取路径部分
            libraryPaths.append(qMakePair(libraryName, path)); // 将库名和路径添加到列表中


        }
    }

    for (const QPair<QString, QString>& libraryPath : libraryPaths) {
        QString libraryName = libraryPath.first;
        QString searchPath = libraryPath.second;
       
        // 使用QDir来搜索文件  
        QDir dir(searchPath);
        QStringList filters;
        filters << "*.txt";
        QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);

        // 遍历文件列表，查找匹配的文件名  
        for (const QFileInfo& file : fileList) {
         
                // 构造包含库名、文件名和路径的字符串，中间用“-”隔开  
                QString displayName = libraryName + " - " + file.fileName() + " - " + file.absolutePath();

                // 将匹配的文件名及其路径添加到显示结果的QListWidget中  
                QListWidgetItem* newItem = new QListWidgetItem(displayName, ui->show_listWidget);
   
        }
    }
    disconnect(ui->show_listWidget, &QListWidget::itemClicked, this, &search::onListItemClicked); // 断开先前的连接  
    if (ui->show_listWidget->count() == 0) {
       // QMessageBox::warning(this, "搜索结果", "没有找到匹配的文件！");
    }
    else {
        connect(ui->show_listWidget, &QListWidget::itemClicked, this, &search::onListItemClicked); // 建立连接  
    }
};
void search::onSearchTextChanged(const QString& text)
{      
    if (text.isEmpty()) {
        initial();//调用初始化函数
        return;
    }
    if (ui->checkBox->isChecked()) {
       on_search_title_changed();// 调用搜索函数
    }
    else  if (ui->checkBox_2->isChecked()) {
       on_search_label_changed();// 调用搜索函数
    }
    else  if (ui->checkBox_3->isChecked()) {
        on_search_content_changed();// 调用搜索函数
    }
}
void search::onCheckBoxStateChanged(int id)
{
    // 确保只有一个复选框被选中
    for (int i = 0; i < buttonGroup->buttons().size(); ++i) {
        if (i != id) {
            buttonGroup->button(i)->setChecked(false);
        }
    }
    QString text = ui->search_text->text();
    onSearchTextChanged(text);
}
void search::on_search_content_changed() {
    QString searchText = ui->search_text->text();
    QList<QPair<QString, QString>> libraryPaths; // 用于存储库名和路径的对  
    bool foundMatch = false; // 引入布尔变量来跟踪是否找到匹配项  

    // 检查搜索框是否为空  
    if (searchText.isEmpty()) {
        // 清空QListWidget中的旧结果  
        ui->show_listWidget->clear();
        QMessageBox::warning(this, "搜索提示", "请输入搜索内容！");
        return; // 如果搜索框为空，则直接返回  
    }

    // 清空显示结果的QListWidget  
    ui->show_listWidget->clear();

    // 遍历 multiklnameandpath 中的所有项，提取库名和路径  
    for (int i = 0; i < multiklnameandpath->count(); ++i) {
        QListWidgetItem* item = multiklnameandpath->item(i);
        QString itemText = item->text();
        int separatorIndex = itemText.indexOf("-"); // 找到库名和路径之间的分隔符  
        if (separatorIndex != -1) {
            QString libraryName = itemText.left(separatorIndex).trimmed(); // 提取库名部分  
            QString path = itemText.mid(separatorIndex + 1).trimmed(); // 提取路径部分  
            libraryPaths.append(qMakePair(libraryName, path)); // 将库名和路径添加到列表中  
        }
    }

    // 遍历所有路径，搜索匹配的文件  
    for (const QPair<QString, QString>& libraryPathPair : libraryPaths) {
        QString libraryName = libraryPathPair.first;
        QString searchPath = libraryPathPair.second;
        QDir dir(searchPath);
        QStringList filters;
        filters << "*.txt";
        QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);

        for (const QFileInfo& file : fileList) {
            QFile fileObject(file.absoluteFilePath());
            if (fileObject.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&fileObject);
                QString line;
                while (!in.atEnd()) {
                    line = in.readLine();
                    if (line.contains(searchText, Qt::CaseInsensitive)) {
                        // 构造包含库名、文件名和路径的字符串，三者用“-”隔开  
                        QString displayName = libraryName + " - " + file.fileName() + " - " + file.absolutePath();

                        // 将匹配的文件信息添加到显示结果的QListWidget中  
                        QListWidgetItem* newItem = new QListWidgetItem(displayName, ui->show_listWidget);
                        foundMatch = true; // 设置找到匹配项的标志  
                    }
                }
                fileObject.close();
            }
        }
    }

    // 根据是否找到匹配项来连接或断开信号  
    if (!foundMatch) {
        disconnect(ui->show_listWidget, &QListWidget::itemClicked, this, &search::onListItemClicked);
       // QMessageBox::warning(this, "搜索结果", "没有找到匹配的文件内容！");
    }
    else {
        connect(ui->show_listWidget, &QListWidget::itemClicked, this, &search::onListItemClicked);
    }
}
void search::on_search_title_changed() {
    QString searchText = ui->search_text->text();
    QList<QPair<QString, QString>> libraryPaths; // 用于存储库名和路径的对  
    bool foundMatch = false; // 引入布尔变量来跟踪是否找到匹配项  

    // 检查搜索框是否为空  
    if (searchText.isEmpty()) {
        // 清空QListWidget中的旧结果  
        ui->show_listWidget->clear();
        QMessageBox::warning(this, "搜索提示", "请输入搜索内容！");
        return; // 如果搜索框为空，则直接返回，不执行后续代码  
    }
    // 清空QListWidget中的旧结果  
    ui->show_listWidget->clear();

    // 遍历 multiklnameandpath 中的所有项，提取库名和路径  
    for (int i = 0; i < multiklnameandpath->count(); ++i) {
        QListWidgetItem* item = multiklnameandpath->item(i);
        QString itemText = item->text();
        int separatorIndex = itemText.indexOf("-"); // 找到名字和路径之间的分隔符  
        if (separatorIndex != -1) {
            QString libraryName = itemText.left(separatorIndex).trimmed(); // 提取库名部分  
            QString path = itemText.mid(separatorIndex + 1).trimmed(); // 提取路径部分  
            libraryPaths.append(qMakePair(libraryName, path)); // 将库名和路径添加到列表中  
        }
    }

    for (const QPair<QString, QString>& libraryPath : libraryPaths) {
        QString libraryName = libraryPath.first;
        QString searchPath = libraryPath.second;

        // 使用QDir来搜索文件  
        QDir dir(searchPath);
        QStringList filters;
        filters << "*.txt";
        QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);

        // 遍历文件列表，查找匹配的文件名  
        for (const QFileInfo& file : fileList) {
            if (file.fileName().contains(searchText, Qt::CaseInsensitive)) {
                // 构造包含库名、文件名和路径的字符串，中间用“-”隔开  
                QString displayName = libraryName + " - " + file.fileName() + " - " + file.absolutePath();

                // 将匹配的文件名及其路径添加到显示结果的QListWidget中  
                QListWidgetItem* newItem = new QListWidgetItem(displayName, ui->show_listWidget);
                foundMatch = true; // 设置找到匹配项的标志  
            }
        }
    }

    disconnect(ui->show_listWidget, &QListWidget::itemClicked, this, &search::onListItemClicked); // 断开先前的连接  

    if (ui->show_listWidget->count() == 0) {
        //QMessageBox::warning(this, "搜索结果", "没有找到匹配的文件！");
    }
    else {
        connect(ui->show_listWidget, &QListWidget::itemClicked, this, &search::onListItemClicked); // 建立连接  
    }
}
void search::on_search_label_changed() {


};
void search::onListItemClicked(QListWidgetItem* item) {
    if (item) {
        QString itemText = item->text();
        QStringList parts = itemText.split("-", Qt::SkipEmptyParts);

        if (parts.size() == 3) {
            QString klName = parts[0].trimmed();
            QString fileName = parts[1].trimmed();
            QString klPath = parts[2].trimmed();

            // 注意：这里假设fullPath已经是完整的路径，不需要再拼接  
            // 如果需要拼接，请确保逻辑正确，例如：QString klNameFullPath = fullPath; // 实际上fullPath已经包含了完整路径信息  

            // 调用函数，传递库名、完整路径（这里直接传递fullPath，因为它已经包含了路径信息）和文件名  
            open_entry(klName, klPath, fileName);
        }
        else {
            // 处理错误情况，比如分隔符数量不正确  
            QMessageBox::warning(this, "错误", "无法解析文件路径或库名！");
        }
    }
}
void search::open_entry(const QString& klName, const QString& klPath, const QString fileName) {
    QString filepath = klPath + "/" + fileName;
    // 打开文件的逻辑，比如使用QFile或其他文件处理类  
    QFile file(filepath); // 创建一个QFile对象，表示要打开的文件，filePath是文件的路径。  

    // 尝试以只读和文本模式打开文件  
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file); // 创建一个QTextStream对象，用于从已打开的文件中读取数据。  
        QString content = in.readAll(); // 读取文件的全部内容，并将其存储在QString对象中。  
        file.close(); // 读取完成后，关闭文件。  
        //打开文件
		// 创建EntryWidget实例  ？？？为了运行测试，暂时注释掉下面几行代码
            //EntryWidget* entryWidget = new EntryWidget(nullptr, false, fileName);
            //// 获取EntryWidget中的QTextEdit并设置内容  
            //QTextEdit* textEdit = entryWidget->getTextEditPoint();
            //if (textEdit) {
            //    textEdit->setPlainText(content);
            //}
            //emit updateTabWidget(klName, klPath, textEdit, fileName);//发送信号
        this->close();
    }
    else {
        QString errorMessage = QString("无法打开文件：%1\n可能的原因包括：\n1. 文件路径不存在。\n2. 没有足够的权限访问该文件。\n3. 文件正在被其他程序使用。").arg(filepath);
        QMessageBox::warning(this, "打开文件失败", errorMessage);

    }

}

void search::open_go_back() {
    this->close();
    emit gobacktoklOK();
}


