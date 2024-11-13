﻿#include "HeaderBlockWidget.h"

QFont HeaderBlockWidget::h1Font("Arial", 32, QFont::Bold);
QFont HeaderBlockWidget::h2Font("Arial", 28, QFont::Bold);
QFont HeaderBlockWidget::h3Font("Arial", 24, QFont::Bold);
QFont HeaderBlockWidget::h4Font("Arial", 20, QFont::Bold);
QFont HeaderBlockWidget::h5Font("Arial", 16, QFont::Bold);
QFont HeaderBlockWidget::h6Font("Arial", 12, QFont::Bold);
const QFont* const HeaderBlockWidget::header_fonts[] = {NULL, &h1Font, &h2Font, &h3Font, &h4Font, &h5Font, &h6Font};

HeaderBlockWidget::HeaderBlockWidget(QWidget* parent) :
    BlockWidget(parent), line(0), level(1), layout(0)
{
    layout = new QVBoxLayout(this);
    line = new HeaderLineEdit(this);
    line->setMinimumHeight(32);
    line->setFont(h1Font);
    line->installEventFilter(filter);
    layout->addWidget(line);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    connect(line, &HeaderLineEdit::textChanged, this, &HeaderBlockWidget::emitContentChange);
}

HeaderBlockWidget::~HeaderBlockWidget()
{

}

BlockType HeaderBlockWidget::type() const
{

    return BlockType::Header;

}



int HeaderBlockWidget::getLevel() const
{
    return level;
}

std::string HeaderBlockWidget::toStdString() const
{
    return line->text().toStdString();
}

QString HeaderBlockWidget::toQString() const
{
    return line->text();
}

void HeaderBlockWidget::keyPressEvent(QKeyEvent* e) {
    // Ctrl + 1~6 = 切换级别
    if (e->modifiers() == Qt::Modifier::CTRL)
    {
        int new_level = -1;
        switch (e->key())
        {
        case Qt::Key_1:
            new_level = 1;
            line->setFont(h1Font);
            break;
        case Qt::Key_2:
            new_level = 2;
            line->setFont(h2Font);
            break;
        case Qt::Key_3:
            new_level = 3;
            line->setFont(h3Font);
            break;
        case Qt::Key_4:
            new_level = 4;
            line->setFont(h4Font);
            break;
        case Qt::Key_5:
            new_level = 5;
            line->setFont(h5Font);
            break;
        case Qt::Key_6:
            new_level = 6;
            line->setFont(h6Font);
            break;
        }
        if (new_level != -1 && new_level != level)
        {

            level = new_level;
            line->setFont(*header_fonts[new_level]);
            emitContentChange();
        }
    }


    BlockWidget::keyPressEvent(e);
}



void HeaderBlockWidget::exportToQtXml(QDomElement& dest, QDomDocument& dom_doc)
{
    dest.setTagName(QStringLiteral("header-block"));
    dest.setAttribute("level", level);
    dest.appendChild(dom_doc.createTextNode(line->text()));
}

void HeaderBlockWidget::importFromQtXml(QDomElement& src)
{
    line->setText(src.firstChild().toText().data());
    level = src.attribute("level").toInt();
    line->setFont(*header_fonts[level]);
}