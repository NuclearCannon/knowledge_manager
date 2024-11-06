#include <QDir>
#include <QMessageBox>
#include <QListWidget>
#include <QCloseEvent>
#include <xkeycheck.h>
#include <QFileDialog>

#include "../public.h"
#include "separate_kl.h"
#include "ui_separate_kl.h"

extern QString default_path_for_all_kls;

separate_kl::separate_kl(QWidget *parent, const QString fileName, const QString filePath)
    : QWidget(parent)
    , kl_name(fileName)
    , kl_path(filePath)
    , buttonGroup(new QButtonGroup(this))  // 初始化 QButtonGroup
    , ui(new Ui::separate_kl)
{
    ui->setupUi(this);
    ui->label->setText(tr("分库库名：%1").arg(kl_name));
    ui->label_3->setText(tr("分库路径：%1").arg(kl_path));
    ui->checkBox_2->setChecked(true);
    initial(); 
    itemsum= ui->search_listWidget->count();//记录词条总数
     ui->search_text_3->setText(default_path_for_all_kls);// 设置知识库路径为默认路径
    ui->label_7->setVisible(false);
    ui->checkBox_4->setChecked(true);
    buttonGroup->addButton(ui->checkBox, 0);
    buttonGroup->addButton(ui->checkBox_2, 1);
    buttonGroup->addButton(ui->checkBox_3, 2);
    connect(ui->pushButton, &QPushButton::clicked, this, &separate_kl::gobacktoseparatekl);//返回
    connect(ui->search_text, &QLineEdit::textChanged, this, &separate_kl::onSearchTextChanged);//文本框内容改变
    connect(buttonGroup, &QButtonGroup::idClicked, this, &separate_kl::onCheckBoxStateChanged);//选择框内容改变
    connect(ui->show_listWidget_3, &QListWidget::itemClicked, this, &separate_kl::onListklClicked);//库列表中item被点击
    connect(ui->pushButton_3, &QPushButton::clicked, this, &separate_kl::allchoosedclicked);//全选
    connect(ui->pushButton_4, &QPushButton::clicked, this, &separate_kl::allclearedclicked);//清空
    connect(ui->pushButton_2, &QPushButton::clicked, this, &separate_kl::save);//保存
    connect(ui->checkBox_4, &QCheckBox::stateChanged, this, &separate_kl::on_checkBox_4_stateChanged);//过滤转为false
    connect(ui->pushButton_5, &QPushButton::clicked, this, &separate_kl::closeWidget);//全选
    connect(ui->select_path_button, &QPushButton::clicked, this, &separate_kl::select_path_button_clicked);
}

separate_kl::~separate_kl()
{
    delete ui;
}
void separate_kl::initial() {
    // 清空QListWidget中的旧结果  
    ui->search_listWidget->clear();

    QString libraryName = kl_name;
    QString searchPath = kl_path;

    // 使用QDir来搜索文件  
    QDir dir(searchPath);
    QStringList filters;
    filters << "*.txt";
    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);
    
    // 遍历文件列表，查找匹配的文件名  
    for (const QFileInfo& file : fileList) {
        
        // 构造包含库名、文件名和路径的字符串，中间用“-”隔开  
        QString displayName = file.fileName();

        // 检查 ui->show_listWidget 中是否已存在相同的词条  
        bool alreadyExists = false;
        for (int j = 0; j < ui->show_listWidget_3->count(); ++j) {
            QListWidgetItem* existingItem = ui->show_listWidget_3->item(j);
            if (existingItem->text() == displayName) {
                alreadyExists = true;
                break;
            }
        }
        for (int j = 0; j < already_saved.count(); ++j) {
            if (already_saved[j] == displayName) {
                alreadyExists = true;
                break;
            }
        }
        // 如果不存在，则添加到 ui->show_listWidget_2  
        if (!alreadyExists) {
            // 将匹配的文件名及其路径添加到显示结果的QListWidget中  
            QListWidgetItem* newItem = new QListWidgetItem(displayName, ui->search_listWidget);
           
        }

    }
    
    disconnect(ui->search_listWidget, &QListWidget::itemClicked, this, &separate_kl::onListItemClicked); // 断开先前的连接  
    if (ui->search_listWidget->count() == 0) {
    }
    else {
        connect(ui->search_listWidget, &QListWidget::itemClicked, this, &separate_kl::onListItemClicked); // 建立连接  
    }
    updateLabel();
    updateLabelkl();
};
void separate_kl::updateLabel() {
    int itemCount = ui->search_listWidget->count();
    ui->label_4->setText(QString("原库词条数：%1").arg(itemCount));
}
void separate_kl::updateLabelkl() {
    int itemCount = ui->show_listWidget_3->count();
    ui->label_5->setText(QString("新库词条数：%1").arg(itemCount));
}
void separate_kl::onSearchTextChanged(const QString& text)
{
    if (text.isEmpty()) {
        initial();//调用初始化函数
        return;
    }
    if (ui->checkBox_2->isChecked()) {
        on_search_title_changed();// 调用搜索函数
    }
    else  if (ui->checkBox_3->isChecked()) {
        on_search_label_changed();// 调用搜索函数
    }
    else  if (ui->checkBox->isChecked()) {
        on_search_content_changed();// 调用搜索函数
    }
    updateLabel();
    updateLabelkl();
}
void separate_kl::onCheckBoxStateChanged(int id) {
    // 确保只有一个复选框被选中
    for (int i = 0; i < buttonGroup->buttons().size(); ++i) {
        if (i != id) {
            buttonGroup->button(i)->setChecked(false);
        }
    }
    QString text = ui->search_text->text();
    onSearchTextChanged(text);
}
void separate_kl::onListItemClicked(QListWidgetItem* item) {
    if (!item) {
        return; // 如果没有选中任何有效项，或者项不在show_listWidget_2中，则直接返回  
    }
    QListWidgetItem* movedItem = ui->search_listWidget->takeItem(ui->search_listWidget->row(item));
    ui->show_listWidget_3->addItem(movedItem);
    updateLabel();
    updateLabelkl();
}
void separate_kl::onListklClicked(QListWidgetItem* item) {
    if (!item) {
        return; // 如果没有选中任何有效项，或者项不在show_listWidget中，则直接返回  
    }

    QString text = ui->search_text->text();
    if (text.isEmpty()) {
        ui->search_listWidget->addItem(item->clone());
        ui->show_listWidget_3->takeItem(ui->show_listWidget_3->row(item));
        disconnect(ui->search_listWidget, &QListWidget::itemClicked, this, &separate_kl::onListItemClicked);
        connect(ui->search_listWidget, &QListWidget::itemClicked, this, &separate_kl::onListItemClicked);
    }else 
    if (ui->checkBox_2->isChecked()) {
        if (item->text().contains(text)) {
            ui->search_listWidget->addItem(item->clone());
            ui->show_listWidget_3->takeItem(ui->show_listWidget_3->row(item));
            disconnect(ui->search_listWidget, &QListWidget::itemClicked, this, &separate_kl::onListItemClicked);
            connect(ui->search_listWidget, &QListWidget::itemClicked, this, &separate_kl::onListItemClicked);
        }
        else {
            ui->show_listWidget_3->takeItem(ui->show_listWidget_3->row(item));
        }
    }
    else if (ui->checkBox->isChecked()) {
        QString searchPath = kl_path + "/" + item->text();
        if (!QFile::exists(searchPath)) {
            QMessageBox::warning(nullptr, "警告", "文件不存在: " + searchPath);
            return;
        }
        QFile file(searchPath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "无法打开文件:" << searchPath;
            QMessageBox::warning(nullptr, "警告", "无法打开文件: " + searchPath);
            return;
        }
        // 读取文件内容并检查是否包含指定文本
        QTextStream in(&file);
        bool found = false;
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.contains(text)) {
                found = true;
                break;
            }
        }
        file.close();
        // 显示结果
        if (found) {
            ui->search_listWidget->addItem(item->clone());
            ui->show_listWidget_3->takeItem(ui->show_listWidget_3->row(item));
            disconnect(ui->search_listWidget, &QListWidget::itemClicked, this, &separate_kl::onListItemClicked);
            connect(ui->search_listWidget, &QListWidget::itemClicked, this, &separate_kl::onListItemClicked);
        }
        else {
            ui->show_listWidget_3->takeItem(ui->show_listWidget_3->row(item));
        }
    }
    updateLabel();
    updateLabelkl();
}


void separate_kl::gobacktoseparatekl() {
    this->close();
    emit goback_separatekl();
}
void separate_kl::on_search_content_changed() {
    QString searchText = ui->search_text->text();
    QList<QPair<QString, QString>> libraryPaths; // 用于存储库名和路径的对  
    bool foundMatch = false; // 引入布尔变量来跟踪是否找到匹配项  

    // 检查搜索框是否为空  
    if (searchText.isEmpty()) {
        // 清空QListWidget中的旧结果  
        ui->search_listWidget->clear();
        QMessageBox::warning(this, "搜索提示", "请输入搜索内容！");
        return; // 如果搜索框为空，则直接返回  
    }

    // 清空显示结果的QListWidget  
    ui->search_listWidget->clear();
    // 遍历所有路径，搜索匹配的文件  
    QString libraryName = kl_name;
    QString searchPath = kl_path;
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
                    QString displayName = file.fileName();
                    // 检查 ui->show_listWidget 中是否已存在相同的词条  
                    bool alreadyExists = false;
                    for (int j = 0; j < ui->show_listWidget_3->count(); ++j) {
                        QListWidgetItem* existingItem = ui->show_listWidget_3->item(j);
                        if (existingItem->text() == displayName) {
                            alreadyExists = true;
                            break;
                        }
                    }
                    for (int j = 0; j < already_saved.count(); ++j) {
                        if (already_saved[j] == displayName) {
                            alreadyExists = true;
                            break;
                        }
                    }
                    // 如果不存在，则添加到 ui->show_listWidget_2  
                    if (!alreadyExists) {
                        // 将匹配的文件名及其路径添加到显示结果的QListWidget中  
                        QListWidgetItem* newItem = new QListWidgetItem(displayName, ui->search_listWidget);
                        foundMatch = true; // 设置找到匹配项的标志 
                    }
                }
                fileObject.close();
            }
        }

        // 根据是否找到匹配项来连接或断开信号  
        if (!foundMatch) {
            disconnect(ui->search_listWidget, &QListWidget::itemClicked, this, &separate_kl::onListItemClicked);
            // QMessageBox::warning(this, "搜索结果", "没有找到匹配的文件内容！");
        }
        else {
            connect(ui->search_listWidget, &QListWidget::itemClicked, this, &separate_kl::onListItemClicked);
        }
    }
}
void separate_kl::on_search_title_changed() {
    QString searchText = ui->search_text->text();
    QList<QPair<QString, QString>> libraryPaths; // 用于存储库名和路径的对  
    bool foundMatch = false; // 引入布尔变量来跟踪是否找到匹配项  

    // 清空QListWidget中的旧结果  
    ui->search_listWidget->clear();


    QString libraryName = kl_name;
    QString searchPath = kl_path;

    // 使用QDir来搜索文件  
    QDir dir(searchPath);
    QStringList filters;
    filters << "*.txt";
    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);

    // 遍历文件列表，查找匹配的文件名  
    for (const QFileInfo& file : fileList) {
        if (file.fileName().contains(searchText, Qt::CaseInsensitive)) {
            // 构造包含库名、文件名和路径的字符串，中间用“-”隔开  
            QString displayName = file.fileName();


            // 检查 ui->show_listWidget 中是否已存在相同的词条  
            bool alreadyExists = false;
            for (int j = 0; j < ui->show_listWidget_3->count(); ++j) {
                QListWidgetItem* existingItem = ui->show_listWidget_3->item(j);
                if (existingItem->text() == displayName) {
                    alreadyExists = true;
                    break;
                }
            }
            for (int j = 0; j < already_saved.count(); ++j) {
                if (already_saved[j] == displayName) {
                    alreadyExists = true;
                    break;
                }
            }
            // 如果不存在，则添加到 ui->show_listWidget_2  
            if (!alreadyExists) {
                // 将匹配的文件名及其路径添加到显示结果的QListWidget中  
                QListWidgetItem* newItem = new QListWidgetItem(displayName, ui->search_listWidget);
                foundMatch = true; // 设置找到匹配项的标志  
            }
        } 
    }
    disconnect(ui->search_listWidget, &QListWidget::itemClicked, this, &separate_kl::onListItemClicked); // 断开先前的连接  

    if (ui->search_listWidget->count() == 0) {
        //QMessageBox::warning(this, "搜索结果", "没有找到匹配的文件！");
    }
    else {
        connect(ui->search_listWidget, &QListWidget::itemClicked, this, &separate_kl::onListItemClicked); // 建立连接  
    }
};
void separate_kl::on_search_label_changed() {

};
//全选按钮
void separate_kl::allchoosedclicked() {
    int count = ui->search_listWidget->count();
    for (int i = 0; i < count; ++i) {
        QListWidgetItem* item = ui->search_listWidget->takeItem(0); // 从列表中移除第一个 item 并返回它  
        ui->show_listWidget_3->addItem(item); // 将移除的 item 添加到另一个列表中  
    }
    updateLabel();
    updateLabelkl();
};
//全部删除
void separate_kl::allclearedclicked() {
    int count = ui->show_listWidget_3->count();
    for (int i = 0; i < count; ++i) {
        QListWidgetItem* item = ui->show_listWidget_3->takeItem(0); // 从列表中移除第一个 item 并返回它  
        onListklClicked(item);
    }
    disconnect(ui->search_listWidget, &QListWidget::itemClicked, this, &separate_kl::onListItemClicked); // 断开先前的连接
    connect(ui->search_listWidget, &QListWidget::itemClicked, this, &separate_kl::onListItemClicked); 
    updateLabel();
    updateLabelkl();
}
void separate_kl::save() {
    // 获取用户输入的库名，如果输入的库名为空，则在创建按钮上面显示一个提示：知识库名不能为空
    QString new_kl_name = ui->search_text_2->text();
    QString new_path = ui->search_text_3->text();
    QDir target_dir(new_path);
    if (new_kl_name.isEmpty())
    {
        QMessageBox::warning(this, "错误", "知识库名不能为空");
        return;
    }
    else if (new_path.isEmpty())
    {
        QMessageBox::warning(this, "错误", "知识库路径不能为空");
        return;
    }
    else if (!target_dir.exists())
    {
        QMessageBox::warning(this, "错误", "知识库路径不存在");
        return;
    }
    else if (ui->show_listWidget_3->count() == 0) {
        QMessageBox::warning(this, "错误", "没有选择任何词条");
        return;

    }
    QString new_kl_path;

    // 将kl_path修改为path + '/' + kl_name
    if (new_path.endsWith('/'))
    {
        new_kl_path = new_path + new_kl_name;
    }
    else {
        new_kl_path = new_path + '/' + new_kl_name;
    }

    if (addKLToRecentKLList(new_kl_name, new_kl_path) == Status::Error)
	{
		QMessageBox::warning(this, "错误", "文件无法打开或操作" + new_kl_path);
		return;
	}

    // 在对应位置建立一个文件夹
    QDir kl_dir(new_kl_path);
    if (kl_dir.exists()) {
        QMessageBox::warning(this, "错误", "知识库文件夹已存在: " + kl_path);
 
        removeKLFromRecentKLList(new_kl_name, new_kl_path);
        return;
    }
    if (!kl_dir.mkpath(new_kl_path))
    {
        // 失败了要回滚
		if (removeKLFromRecentKLList(new_kl_name, new_kl_path) == Status::Error)
		{
			QMessageBox::warning(this, "错误", "文件无法打开或操作" + new_kl_path);
			return;
		}
        QMessageBox::warning(this, "错误", "创建知识库文件夹失败！");
        return;
    }
    // 将 show_listWidget_3 中每个 item 对应的文件复制到新创建的文件夹中
    for (int i = 0; i < ui->show_listWidget_3->count(); ++i) {
        QListWidgetItem* item = ui->show_listWidget_3->item(i);
        QString sourceFilePath = kl_path + "/" + item->text();
        QString destinationFilePath = kl_dir.filePath(item->text());

        if (!QFile::copy(sourceFilePath, destinationFilePath)) {
            kl_dir.removeRecursively();
            // 失败了要回滚
            if (removeKLFromRecentKLList(new_kl_name, new_kl_path) == Status::Error)
            {
                QMessageBox::warning(this, "错误", "文件无法打开或操作" + new_kl_path);
                return;
            }
            QMessageBox::warning(this, "错误", QString("分库文件失败: %1 到 %2").arg(sourceFilePath, destinationFilePath));
            return;
        }
    }
    QMessageBox::warning(nullptr, "保存", "保存成功！");
    ui->search_text_2->clear();
    ui->search_text_3->setText(default_path_for_all_kls);// 设置知识库路径为默认路径
    if (ui->checkBox_4->isChecked()) {
        for (int i = 0; i < ui->show_listWidget_3->count(); ++i) {
            QListWidgetItem* item = ui->show_listWidget_3->item(i);
            already_saved.append(item->text()); // 添加每个 item 的内容到 QStringList
        }
        ui->show_listWidget_3->clear();  
        if (already_saved.count() == itemsum) {
            ui->label_7->setText("库内元素已空");
            ui->label_7->setVisible(true);
        }
        updateLabel();
        updateLabelkl();
    }
    else {
        int count = ui->show_listWidget_3->count();
        for (int i = 0; i < count; ++i) {
            QListWidgetItem* item = ui->show_listWidget_3->takeItem(0); // 从列表中移除第一个 item 并返回它  
            onListklClicked(item);
        }
        disconnect(ui->search_listWidget, &QListWidget::itemClicked, this, &separate_kl::onListItemClicked); // 断开先前的连接
        connect(ui->search_listWidget, &QListWidget::itemClicked, this, &separate_kl::onListItemClicked);
        updateLabel();
        updateLabelkl();
    }
    
}
void separate_kl::on_checkBox_4_stateChanged(int state) {
    if (state == Qt::Unchecked) { // 从 true 变为 false
        // 执行相应的逻辑
        already_saved.clear();
        ui->label_7->clear();// setText("库内元素已空");
        ui->label_7->setVisible(false);
        QString text = ui->search_text->text();
        onSearchTextChanged(text);//进行一次搜索
    }
}
void separate_kl::closeWidget() {  
    this->close();
}
void separate_kl::closeEvent(QCloseEvent* event)  {
    if (ui->checkBox_5->checkState()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Message",
            "确定删除原库?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            QDir sourceDir(kl_path);

            // 如果源文件夹存在，则删除它及其内容  
            if (sourceDir.exists()) {
                bool success = sourceDir.removeRecursively();
                if (!success) {
                    QMessageBox::warning(this, "警告", "删除失败");
                }
                else {
                    if (removeKLFromRecentKLList(kl_name, kl_path) == Status::Error)
                    {
                        QMessageBox::warning(this, "错误", "文件无法打开或操作" + kl_path);
                        return;
                    }
                }
            }
            // 继续关闭事件  
            event->accept();
        }
        else {
            // 忽略关闭事件  
            event->ignore();
        }
    }

}
// 槽：点击选择路径按钮
void separate_kl::select_path_button_clicked()
{
    // 打开一个对话框，让用户选择路径
    QString kl_path = QFileDialog::getExistingDirectory(this, "选择知识库路径", QDir::homePath());
    if (!kl_path.isEmpty())
    {
        // 将选择的路径显示在输入框中
        ui->search_text_3->setText(kl_path);
    }
}