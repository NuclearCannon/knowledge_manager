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
    ImageBlockWidget(QWidget* parent, const QDir& attachment_dir);
    ~ImageBlockWidget();
protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* e) override;
public:
    virtual BlockType type() const;

    virtual void exportToQtXml(QDomElement& dest, QDomDocument& dom_doc);
    virtual void importFromQtXml(QDomElement& src);

private:
	QLabel* label;
	QVBoxLayout* layout;
    bool loaded;
    QPixmap* pixmap;
    QString filename;
    QDir attachment_dir;
};







