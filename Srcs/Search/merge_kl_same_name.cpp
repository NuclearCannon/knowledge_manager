#include <QDir>
#include <QMessageBox>
#include <QApplication>
#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QColor>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QScrollBar>
#include "../StartWindow/new_kl_guidance.h"
#include "merge_kl_same_name.h"
#include "ui_merge_kl_same_name.h"
#include "../public.h"

merge_kl_same_name::merge_kl_same_name(QWidget* parent, QListWidget* resultListWidget) 
        : QWidget(parent)
        , resultListWidget_a(resultListWidget)
        , ui(new Ui::merge_kl_same_name)

    {
        ui->setupUi(this);

        //for (int i = 0; i < resultListWidget->count(); ++i) {
        //    QListWidgetItem* item = resultListWidget->takeItem(0); // 从列表中移除第一个 item 并返回它  
        //    ui->show_listWidget_2->addItem(item); // 将移除的 item 添加到另一个列表中  
        //}

        QString memeryfileName;
        QString memeryfileName1;
        int color = 0;
       while(resultListWidget->count()!=0) {
            QListWidgetItem* item = resultListWidget->takeItem(0); // 从列表中移除第一个 item 并返回它  
           // item->setBackground(QColor(Qt::yellow));
            QString itemText = item->text();

            // 假设 itemText 的格式为 "库名-路径"，使用 "-" 分割字符串  
            QStringList parts = itemText.split("-");
            QString libraryName = parts[0].trimmed(); // 库名  
            QString fileName = parts[1].trimmed(); // 词条名
            QString sourcePath = parts[2].trimmed(); // 路径 
            QString fullItemText= QString("%1-%2").arg(fileName).arg(sourcePath);
            QListWidgetItem* newItem = new QListWidgetItem(fullItemText);
            QListWidgetItem* libraryNameitem = new QListWidgetItem(libraryName);

            memeryfileName = fileName;//灰白色相间
            if(memeryfileName!= memeryfileName1){
                memeryfileName1 = memeryfileName;
                color++;
            }
            if(color%2){                  
                newItem->setBackground(QColor(235, 235, 235));
                libraryNameitem->setBackground(QColor(235, 235, 235));
            }
            ui->show_listWidget_2->addItem(newItem); // 将移除的 item 添加到另一个列表中  
            ui->show_listWidget_3->addItem(libraryNameitem); // 将移除的 item 添加到另一个列表中
        }
       //可以使用QStringList 一次写入多个数据
       QStringList strList,strList1;
       strList << "默认重命名" << "词条尾加（库名）以区分";
       ui->comboBox->addItems(strList);
       ui->comboBox->setCurrentIndex(0);//设置默认值
       on_comboBox_currentIndexChanged(0);
       strList1 << "合并为一个标签" << "自动重命名";
       ui->comboBox_3->addItems(strList1);
       ui->comboBox_3->setCurrentIndex(0);//设置默认值
       on_comboBox3_currentIndexChanged(0);
       connect(ui->pushButton, &QPushButton::clicked, this, &merge_kl_same_name::choose_klnameandpath);
       connect(ui->goback, &QPushButton::clicked, this, &merge_kl_same_name::open_go_back);
       // 安装事件过滤器
       ui->show_listWidget_2->installEventFilter(this);
       ui->show_listWidget_3->installEventFilter(this);

       // 连接滚动条的 valueChanged 信号
       connect(ui->show_listWidget_2->verticalScrollBar(), &QScrollBar::valueChanged, this, &merge_kl_same_name::syncScrollbars);
       connect(ui->show_listWidget_3->verticalScrollBar(), &QScrollBar::valueChanged, this, &merge_kl_same_name::syncScrollbars);
       connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_currentIndexChanged(int)));
       connect(ui->comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox3_currentIndexChanged(int)));
    };

merge_kl_same_name::~merge_kl_same_name()
{
    delete ui;
}
bool merge_kl_same_name::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::Wheel) {
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
        int delta = wheelEvent->angleDelta().y();

        // 获取滚动条对象
        QScrollBar* scrollBar1 = ui->show_listWidget_2->verticalScrollBar();
        QScrollBar* scrollBar2 = ui->show_listWidget_3->verticalScrollBar();

        // 计算新的滚动条位置
        int currentPos1 = scrollBar1->value();
        int currentPos2 = scrollBar2->value();
        int newPos1 = qBound(scrollBar1->minimum(), currentPos1 - delta, scrollBar1->maximum());
        int newPos2 = qBound(scrollBar2->minimum(), currentPos2 - delta, scrollBar2->maximum());

        // 更新滚动条位置
        scrollBar1->setValue(newPos1);
        scrollBar2->setValue(newPos2);

        return true; // 事件已处理
    }
    return QWidget::eventFilter(obj, event);
}

void merge_kl_same_name::syncScrollbars(int value)
{
    QScrollBar* scrollBar1 = ui->show_listWidget_2->verticalScrollBar();
    QScrollBar* scrollBar2 = ui->show_listWidget_3->verticalScrollBar();

    // 确保两个滚动条同步
    if (sender() == scrollBar1) {
        scrollBar2->setValue(value);
    }
    else if (sender() == scrollBar2) {
        scrollBar1->setValue(value);
    }
}
void merge_kl_same_name::choose_klnameandpath() {
    NewKLGuidance* merge_NewKLGuidance = new NewKLGuidance(this);
    connect(merge_NewKLGuidance, &NewKLGuidance::mergekl, this, &merge_kl_same_name::creactmergekl_same_name);
    merge_NewKLGuidance->setWindowFlags(Qt::Window); // 确保它作为一个独立的窗口显示
    merge_NewKLGuidance->show();
    this->close();
};

void merge_kl_same_name::creactmergekl_same_name(QString kl_name,QString kl_path) {
    // 创建一个 QDir 对象，指向目标文件夹，如果文件夹不存在则创建它  
    QDir targetDir(kl_path);
    if (!targetDir.exists()) {
        targetDir.mkpath(".");
    }

    // 获取 QListWidget 中的所有 item  
    QList<QListWidgetItem*> items = ui->show_listWidget_2->findItems("", Qt::MatchContains);

    QHash<QString, int> sameKLandITEM;
    //QHash<int, QString> fileCountsName;
    // 遍历每个 item  
    for (QListWidgetItem* item : items) {
        QString itemText = item->text();

        // 获取当前 item 在 show_listWidget_2 中的行号
        int row = ui->show_listWidget_2->row(item);

        // 根据行号从 show_listWidget_3 中获取对应的库名
        QListWidgetItem* libraryItem = ui->show_listWidget_3->item(row);
        QString libraryName = libraryItem->text();
        // 假设 itemText 的格式为 "库名-路径"，使用 "-" 分割字符串  
        QStringList parts = itemText.split("-"); 
        QString fileName = parts[0].trimmed(); // 词条名
        QString sourcePath = parts[1].trimmed(); // 路径  
       // QString filePath = sourcePath + "/" + fileName; // 路径 
        // 创建 QDir 对象，指向源文件夹  
        QDir sourceDir(sourcePath);
        if (!sourceDir.exists()) {
            // 如果源文件夹不存在，则打印错误信息并跳过该路径  
            QMessageBox::warning(this, "错误", "源文件夹不存在：" + sourcePath);
            if (removeKLFromRecentKLList(kl_name, kl_path) == Status::Error)
            {
                QMessageBox::warning(this, "错误", "文件无法打开或操作" + kl_path);
                return;
            }
            targetDir.removeRecursively();
            return; // 停止运行
        }
        QString elsefilename=QString();
        switch (ui->comboBox->currentIndex())
        {
            case 0:  
                sameKLandITEM[libraryName + "_" + fileName]++;
                if(sameKLandITEM[libraryName + "_" + fileName]>1){
                    int count = sameKLandITEM[libraryName + "_" + fileName] - 1;
                    elsefilename =  "(" + QString::number(count) + ")";
                }
                break;
            case 1:
                sameKLandITEM[libraryName + "_" + fileName]++;
                if (sameKLandITEM[libraryName + "_" + fileName] > 1) {
                    int count = sameKLandITEM[libraryName + "_" + fileName] - 1;
                    elsefilename = "(" + QString::number(count) + ")";
                }
                break;
            default:
                break;
        }
        // 复制源文件夹中的内容到目标文件夹
        if (!copyDirectory_same_name(libraryName, sourceDir, fileName, targetDir,elsefilename)) {
            QMessageBox::warning(this, "错误", "合库失败！");
            if (removeKLFromRecentKLList(kl_name, kl_path) == Status::Error)
            {
                QMessageBox::warning(this, "错误", "文件无法打开或操作" + kl_path);
                return;
            }
            targetDir.removeRecursively();
            return; // 停止运行
        };

    }
    emit samename_puttonewklOK(kl_name,kl_path, ui->comboBox->currentIndex());
}

// 复制源文件夹中的内容到目标文件夹
bool merge_kl_same_name::copyDirectory_same_name(QString libraryName, QDir sourceDir, QString fileName, QDir targetDir, QString elsefilename) {
    // 构建源文件的完整路径
    QString sourceFilePath = sourceDir.absoluteFilePath(fileName);
    QStringList failedFiles;
    QString targetFileName;
    QString baseName;
    QString suffix;
    switch (ui->comboBox->currentIndex())
    {
        case 0:
            // 分离文件名和后缀
            baseName = fileName.left(fileName.lastIndexOf('.'));
            suffix = fileName.mid(fileName.lastIndexOf('.'));
            // 在文件名后缀前插入 elsefilename
            fileName = baseName + elsefilename + suffix;
           targetFileName = libraryName + "_" + fileName;
           break;
        case 1:    
            // 分离文件名和后缀
            baseName = fileName.left(fileName.lastIndexOf('.'));
            suffix = fileName.mid(fileName.lastIndexOf('.'));
            // 在文件名后缀前插入 elsefilename
            targetFileName = baseName +"(" + libraryName + ")"+ elsefilename + suffix;
            break;
        default:
            break;
    }
 
    QString targetFilePath = targetDir.absoluteFilePath(targetFileName);
    // 创建目标目录（如果不存在）
    if (!targetDir.exists()) {
        if (!targetDir.mkpath(".")) {
            failedFiles.append(QString("Failed to create target directory: %1\n").arg(targetDir.path()));
           // QMessageBox::warning(this, "Error", "Failed to create target directory: " + );
        }
    }
    // 复制文件
    if (!QFile::copy(sourceFilePath, targetFilePath)) {
        failedFiles.append(QString("Failed to copy file from " + sourceFilePath + " to " + targetFilePath+"\n"));
       // QMessageBox::warning(this, "Error", "Failed to copy file from " + sourceFilePath + " to " + targetFilePath);
    }
    if (!failedFiles.isEmpty()) {
        QMessageBox::warning(this, "警告", "以下操作失败:\n" + failedFiles.join("\n"));
        return 0;
    }
    return 1;
}
void merge_kl_same_name::open_go_back() {
    this->close();
    emit goback();
}
void merge_kl_same_name::on_comboBox_currentIndexChanged(int index) {
    // 根据当前索引设置新的标签文本
    switch (index) {
        case 0:
            ui->label_5->setText("解释：\n   (1)来自A库的B词条将被重命名为“A_B”\n   (2)不重名的词条如果原名为“A_B”与重\n命名后的新词条冲突，则会命名为\n“A_B(原-***库)”，\n （3）库名词条名均相同将随机命名为\n“A_B（数字号）”\n注意：\n （1）词条原名不得命名为A_B(原-***库)\n （2）请规范命名，否则将合库失败！");
            break;
        case 1:
            ui->label_5->setText("解释：\n   (1)来自A库的B词条将被重命名为“B（A）”\n   (2)不重名的词条如果原名为“B（A）”与重\n命名后的冲突，则会命名为\n“B（A）(原-***库)”，\n （3）库名词条名均相同将随机命名为\n“B（A）（数字号）”\n注意：\n （1）词条原名不得命名为B（A）(原-***库)\n （2）请规范命名，否则将合库失败！");
            break;
        default:
            ui->label_5->setText("未知选项");
    }
}
void merge_kl_same_name::on_comboBox3_currentIndexChanged(int index) {
    // 根据当前索引设置新的标签文本
    switch (index) {
    case 0:
        ui->label_6->setText("解释：\n   (1)不区分库的来源，合并为一个标签");
        break;
    case 1:
        ui->label_6->setText("解释：\n   (1)来自A库的B标签将被重命名为“A-B”。\n   (2)库名相同标签相同则合并为一个标签\n   后续用户可以对这些标签重新重命名");
        break;
    default:
        ui->label_6->setText("未知选项");
    }
}

