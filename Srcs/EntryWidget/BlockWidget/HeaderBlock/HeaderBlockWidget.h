#pragma once
#include "../BlockWidget.h"
#include <QLineEdit>
#include <QBoxLayout>

class HeaderLineEdit : public QLineEdit, public BlockControl
{
    Q_OBJECT
public:
    HeaderLineEdit(QWidget* parent);
    void undo();
    void redo();
    void clearUndoStack();
signals:
    void contextMenuQuery(QContextMenuEvent* event);
protected:
    void contextMenuEvent(QContextMenuEvent* event);
};

class HeaderBlockWidget : public BlockWidget
{
	Q_OBJECT
private:
    static QFont h1Font, h2Font, h3Font, h4Font, h5Font, h6Font;
    static const QFont* const header_fonts[];

    HeaderLineEdit *line;
    QVBoxLayout* layout;
    int level;
    
protected:
    void keyPressEvent(QKeyEvent* e) override;
public:
    HeaderBlockWidget(QWidget* parent);
    ~HeaderBlockWidget();
    virtual BlockType type() const;


    virtual void exportToQtXml(QDomElement& dest, QDomDocument& dom_doc);
    virtual void importFromQtXml(QDomElement& src);

    int getLevel() const;
    std::string toStdString() const;
    QString toQString() const;

    void clearUndoStack();
private slots:
    void handleContextMenuUndo();
    void handleContextMenuRedo();
protected:
    void contextMenuEvent(QContextMenuEvent* event);
};

