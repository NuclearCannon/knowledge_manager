#pragma once
#include "../BlockWidget.h"
#include <QLabel>
#include <QBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>


class ImageBlockWidget : public BlockWidget
{
	Q_OBJECT
public:

    static ImageBlockWidget* initialize(QWidget* parent, const QDir& attachment_dir);  // 获取一个图片块，如果失败返回nullptr
    static ImageBlockWidget* initializeFromQtXml(QWidget* parent, const QDir& attachment_dir, QDomElement& src);


    
    ~ImageBlockWidget();
    virtual BlockType type() const;
    virtual void exportToQtXml(QDomElement& dest, QDomDocument& dom_doc);
    virtual void importFromQtXml(QDomElement& src);
    virtual void deleteFile();
    void clearUndoStack();
protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void resizeEvent(QResizeEvent* e) override;

private:
	QLabel* label;
	QVBoxLayout* layout;
    bool loaded;
    QPixmap* pixmap;
    QString filename;
    QDir attachment_dir;

    bool getImageFile();
    ImageBlockWidget(QWidget* parent, const QDir& attachment_dir);
};







