#pragma once
#include "BlockWidget.h"
#include <QLineEdit>
#include <QBoxLayout>
class HeaderBlockWidget : public BlockWidget
{
	Q_OBJECT
private:
    static QFont h1Font, h2Font, h3Font, h4Font, h5Font, h6Font;
    QLineEdit *line;
    QVBoxLayout* layout;
    int level;
    
protected:
    void keyPressEvent(QKeyEvent* e) override;
public:
    HeaderBlockWidget(QWidget* parent);
    ~HeaderBlockWidget();
    virtual BlockType type() const;
    virtual void exportToPugi(pugi::xml_node& dest);
    virtual void importFromPugi(const pugi::xml_node& node);
    int getLevel() const;
    std::string toStdString() const;
    QString toQString() const;
};

