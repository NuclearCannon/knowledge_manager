#include "ImageBlockWidget.h"
#include <QMenu>
#include "../BlockWidget.h"
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
    // 如果目标文件夹不存在则先创建
    if (!destinationDir.exists())
    {
        qDebug() << destinationDir.path();
        destinationDir.mkdir(destinationDir.path());
    }
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


ImageBlockWidget* ImageBlockWidget::initialize(QWidget* parent, const QDir& attachment_dir)
{
    ImageBlockWidget* p = new ImageBlockWidget(parent, attachment_dir);
    if (!p)return nullptr;
    bool succeed = p->getImageFile();
    if (succeed)
    {
        return p;
    }
    else
    {
        delete p;
        return nullptr;
    }
}

ImageBlockWidget* ImageBlockWidget::initializeFromQtXml(QWidget* parent, const QDir& attachment_dir, QDomElement& src)
{
    ImageBlockWidget* p = new ImageBlockWidget(parent, attachment_dir);
    if (!p)return nullptr;
    p->importFromQtXml(src);
    return p;
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

bool ImageBlockWidget::getImageFile()
{
    QString new_file_name;
    QString sourceFilePath = QFileDialog::getOpenFileName(this, "选择图片", "", "Images (*.png *.jpg)");
    bool succeed = copyFileWithUniqueName(sourceFilePath, attachment_dir, new_file_name);
    if (!succeed)
    {
        qWarning() << "handleImageBlockCopy Not Succeed";
        return false;
    }
    if (!new_file_name.isEmpty())
    {
        
        pixmap = new QPixmap(attachment_dir.absoluteFilePath(new_file_name));
        if (!pixmap->isNull())
        {
            loaded = true;
            int dest_width = this->width() * 0.5;
            dest_width = dest_width < 50 ? 50 : dest_width;
            attachment_dir.remove(filename);
            filename = new_file_name;
            label->setPixmap(pixmap->scaledToWidth(dest_width));
            return true;
        }
        else
        {
            // 显示无法加载图片的对话框  
            QMessageBox::critical(nullptr, "错误", "无法加载图片", QMessageBox::Ok);
            return false;
        }
    }
    else
    {
        qWarning() << "How could new_file_name is empty???";
        attachment_dir.remove(new_file_name);
        return false;
    }

}

void ImageBlockWidget::resizeEvent(QResizeEvent* e) {
    QWidget::resizeEvent(e);
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

void ImageBlockWidget::exportToQtXml(QDomElement& dest, QDomDocument& dom_doc)
{
    dest.setTagName("image-block");
    dest.setAttribute("src", filename);
}

void ImageBlockWidget::importFromQtXml(QDomElement& src)
{
    filename = src.attribute("src");
    pixmap = new QPixmap(attachment_dir.absoluteFilePath(filename));
    loaded = true;
    int dest_width = this->width() * 0.5;
    dest_width = dest_width < 50 ? 50 : dest_width;
    label->setPixmap(pixmap->scaledToWidth(dest_width));
}



void ImageBlockWidget::contextMenuEvent(QContextMenuEvent* event)
{
    // 创建一个菜单
    QMenu contextMenu(nullptr);

    // 添加菜单项

    QAction* action_delete = contextMenu.addAction("删除");
    connect(action_delete, &QAction::triggered, this, &ImageBlockWidget::handleContextMenuDelete);

    QMenu* menu_insert_above = contextMenu.addMenu("在上方插入");
    QAction* action_insert_above_text = menu_insert_above->addAction("文本块");
    connect(action_insert_above_text, &QAction::triggered, this, &ImageBlockWidget::handleContextMenuInsertAboveText);

    QAction* action_insert_above_code = menu_insert_above->addAction("代码块");
    connect(action_insert_above_code, &QAction::triggered, this, &ImageBlockWidget::handleContextMenuInsertAboveCode);

    QAction* action_insert_above_image = menu_insert_above->addAction("图片块");
    connect(action_insert_above_image, &QAction::triggered, this, &ImageBlockWidget::handleContextMenuInsertAboveImage);

    QAction* action_insert_above_header = menu_insert_above->addAction("标题块");
    connect(action_insert_above_header, &QAction::triggered, this, &ImageBlockWidget::handleContextMenuInsertAboveHeader);

    QMenu* menu_insert_below = contextMenu.addMenu("在下方插入");
    QAction* action_insert_below_text = menu_insert_below->addAction("文本块");
    connect(action_insert_below_text, &QAction::triggered, this, &ImageBlockWidget::handleContextMenuInsertBelowText);

    QAction* action_insert_below_code = menu_insert_below->addAction("代码块");
    connect(action_insert_below_code, &QAction::triggered, this, &ImageBlockWidget::handleContextMenuInsertBelowCode);

    QAction* action_insert_below_image = menu_insert_below->addAction("图片块");
    connect(action_insert_below_image, &QAction::triggered, this, &ImageBlockWidget::handleContextMenuInsertBelowImage);

    QAction* action_insert_below_header = menu_insert_below->addAction("标题块");
    connect(action_insert_below_header, &QAction::triggered, this, &ImageBlockWidget::handleContextMenuInsertBelowHeader);

    // 在事件发生的位置显示菜单
    contextMenu.exec(event->globalPos());
}

void ImageBlockWidget::deleteFile()
{
    attachment_dir.remove(filename);
}

void ImageBlockWidget::clearUndoStack()
{
    // do nothing
}