﻿#include "ImageBlockWidget.h"
//#include "EntryArea.h"
#include "BlockWidget.h"
#include <QLabel>
#include <QBoxLayout>
#include <QFileDialog>
#include <QMessageBox>


// 静态函数
static QString generateUniqueFileName(const QDir& dir, const QString& baseFileName) {
    QFileInfo fileInfo(baseFileName);
    QString fileName = fileInfo.completeBaseName(); // 获取文件的基本名称（不包括扩展名）  
    QString suffix = fileInfo.suffix(); // 获取文件的扩展名  
    QString newFileName = fileName + '.' + suffix; // 初始文件名  
    int counter = 1;

    // 检查文件名是否已存在，如果存在则添加后缀  
    while (dir.exists(newFileName)) {
        QString counterStr = QString::number(counter++);
        newFileName = fileName + "(" + counterStr + ")." + suffix;
    }

    return newFileName;
}


static bool copyFileWithUniqueName(const QString& sourceFilePath, const QDir& destinationDir, QString& fileNameDest) {
    QFile sourceFile(sourceFilePath);
    if (!sourceFile.exists()) {
        qWarning() << "Source file does not exist:" << sourceFilePath;
        return false;
    }

    QFileInfo fileInfo(sourceFile);
    QString baseFileName = fileInfo.fileName();
    QString uniqueFileName = generateUniqueFileName(destinationDir, baseFileName);
    fileNameDest = uniqueFileName;
    QString destinationFilePath = destinationDir.absoluteFilePath(uniqueFileName);

    QFile destinationFile(destinationFilePath);
    if (!destinationFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open destination file for writing:" << destinationFilePath;
        destinationFile.remove();
        return false;
    }

    if (!sourceFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open source file for reading:" << sourceFilePath;
        destinationFile.close();
        destinationFile.remove();
        return false;
    }

    char buffer[4096];
    qint64 bytesRead;

    while ((bytesRead = sourceFile.read(buffer, sizeof(buffer))) > 0)
    {
        destinationFile.write(buffer, bytesRead);
    }

    sourceFile.close();
    destinationFile.close();

    return true;
}


ImageBlockWidget::ImageBlockWidget(QWidget* parent, const QDir& attachment_dir) :
    BlockWidget(parent),
    label(new QLabel("双击插入图片", this)),
    layout(new QVBoxLayout(this)),
    loaded(false),
    pixmap(0),
    attachment_dir(attachment_dir)
{
    // 设置布局和标签属性  
    layout->addWidget(label);
    setLayout(layout);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("QLabel { font-size: 20px; }");


}

ImageBlockWidget::~ImageBlockWidget()
{
    if (pixmap)
    {
        delete pixmap;
        pixmap = nullptr;
    }

}

void ImageBlockWidget::mouseDoubleClickEvent(QMouseEvent* event) 
{

    if (label->rect().contains(event->pos()))
    {

        QString new_file_name;
        bool succeed;
        QString sourceFilePath = QFileDialog::getOpenFileName(this, "选择图片", "", "Images (*.png *.jpg)");
        succeed = copyFileWithUniqueName(sourceFilePath, attachment_dir, new_file_name);

        if (!succeed)
        {
            qWarning() << "handleImageBlockCopy Not Succeed";
            return;
        }

        if (!new_file_name.isEmpty())
        {
            QPixmap* new_pixmap = new QPixmap(new_file_name);

            if (!new_pixmap->isNull())
            {
                emitContentChange();
                loaded = true;
                int dest_width = this->width() * 0.5;
                dest_width = dest_width < 50 ? 50 : dest_width;
                attachment_dir.remove(filename);
                filename = new_file_name;
                delete pixmap;
                pixmap = new_pixmap;
                label->setPixmap(pixmap->scaledToWidth(dest_width));
            }
            else
            {
                // 显示无法加载图片的对话框  
                QMessageBox::critical(nullptr, "错误", "无法加载图片", QMessageBox::Ok);

            }
        }
        else
        {
            qWarning() << "How could new_file_name is empty???";
            attachment_dir.remove(new_file_name);
        }
    }
    else
    {
        BlockWidget::mouseDoubleClickEvent(event);
    }
}

void ImageBlockWidget::resizeEvent(QResizeEvent* e) {
    QWidget::resizeEvent(e);
    qDebug() << "resize!";
    if (loaded)
    {
        int dest_width = this->width() * 0.5;
        dest_width = dest_width < 50 ? 50 : dest_width;

        label->setPixmap(pixmap->scaledToWidth(dest_width));
    }
}




BlockType ImageBlockWidget::type() const
{

    return BlockType::Image;

}

void ImageBlockWidget::exportToPugi(pugi::xml_node& dest)
{
    dest.set_name("image-block");
    dest.append_attribute("src").set_value(filename.toStdString().c_str());
}
void ImageBlockWidget::importFromPugi(const pugi::xml_node& node)
{
    filename = node.attribute("src").as_string();
    pixmap = new QPixmap(attachment_dir.absoluteFilePath(filename));
}
