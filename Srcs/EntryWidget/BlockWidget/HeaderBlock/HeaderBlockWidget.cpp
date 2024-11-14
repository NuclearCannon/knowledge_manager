#include "HeaderBlockWidget.h"
#include <QMenu>

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
    connect(line, &HeaderLineEdit::contextMenuQuery, this, &HeaderBlockWidget::handleContextMenuQueryFromControls);
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


//void HeaderBlockWidget::handleContextMenuQueryFromControls(QContextMenuEvent* event)
//{
//    contextMenuEvent(event);
//}

void HeaderBlockWidget::contextMenuEvent(QContextMenuEvent* event)
{
    // 创建一个菜单
    QMenu contextMenu(nullptr);

    // 添加菜单项
    QAction* action_undo = contextMenu.addAction("撤销");
    connect(action_undo, &QAction::triggered, this, &HeaderBlockWidget::handleContextMenuUndo);

    QAction* action_redo = contextMenu.addAction("重做");
    connect(action_redo, &QAction::triggered, this, &HeaderBlockWidget::handleContextMenuRedo);

    QAction* action_delete = contextMenu.addAction("删除");
    connect(action_delete, &QAction::triggered, this, &HeaderBlockWidget::handleContextMenuDelete);

    QMenu* menu_insert_above = contextMenu.addMenu("在上方插入");
    QAction* action_insert_above_text = menu_insert_above->addAction("文本块");
    connect(action_insert_above_text, &QAction::triggered, this, &HeaderBlockWidget::handleContextMenuInsertAboveText);

    QAction* action_insert_above_code = menu_insert_above->addAction("代码块");
    connect(action_insert_above_code, &QAction::triggered, this, &HeaderBlockWidget::handleContextMenuInsertAboveCode);

    QAction* action_insert_above_image = menu_insert_above->addAction("图片块");
    connect(action_insert_above_image, &QAction::triggered, this, &HeaderBlockWidget::handleContextMenuInsertAboveImage);

    QAction* action_insert_above_header = menu_insert_above->addAction("标题块");
    connect(action_insert_above_header, &QAction::triggered, this, &HeaderBlockWidget::handleContextMenuInsertAboveHeader);

    QMenu* menu_insert_below = contextMenu.addMenu("在下方插入");
    QAction* action_insert_below_text = menu_insert_below->addAction("文本块");
    connect(action_insert_below_text, &QAction::triggered, this, &HeaderBlockWidget::handleContextMenuInsertBelowText);

    QAction* action_insert_below_code = menu_insert_below->addAction("代码块");
    connect(action_insert_below_code, &QAction::triggered, this, &HeaderBlockWidget::handleContextMenuInsertBelowCode);

    QAction* action_insert_below_image = menu_insert_below->addAction("图片块");
    connect(action_insert_below_image, &QAction::triggered, this, &HeaderBlockWidget::handleContextMenuInsertBelowImage);

    QAction* action_insert_below_header = menu_insert_below->addAction("标题块");
    connect(action_insert_below_header, &QAction::triggered, this, &HeaderBlockWidget::handleContextMenuInsertBelowHeader);

    // 在事件发生的位置显示菜单
    contextMenu.exec(event->globalPos());
}

void HeaderBlockWidget::handleContextMenuUndo()
{
    line->undo();
}
void HeaderBlockWidget::handleContextMenuRedo()
{
    line->redo();
}

void HeaderBlockWidget::clearUndoStack()
{
    line->clearUndoStack();
}