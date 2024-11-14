#include "EntryArea.h"
#include "BlockWidget/BlockWidgets.h"
#include <QApplication>
#include <sstream>
#include <QMenu>


// EntryArea::OutlineItem
EntryArea::OutlineItem::OutlineItem(QString text, int level, int position) :text(text), level(level), position(position) {};

// EntryArea(private)
int EntryArea::insert(int index, BlockWidget* ptr)
{
    layout->insertWidget(index, ptr);
    return 0;
}



// 构造函数
EntryArea::EntryArea(QWidget* parent, const QDir& root) :
    QScrollArea(parent),
    layout(0),
    head(0),
    tail(0),
    focus(nullptr),
    root_dir(root),
    attachment_dir(root_dir.absoluteFilePath("attachments")),
    saved(true),
    entry_file(root.absoluteFilePath("entry.xml"))
{
    setWidget(new QWidget);
    setWidgetResizable(true);
    qDebug() << "EntryArea initialized";
    layout = new QVBoxLayout(this->widget());
    layout->addStretch(1);
    // 打开文件不由这里负责
    
    qDebug() << "EntryWidget(filePath=" << root.absoluteFilePath("entry.xml");

}

EntryArea::~EntryArea()
{
    // 关闭文件
    if (entry_file.isOpen())
    {
        entry_file.close();
    }

}


int EntryArea::length() const
{
    return blocks.size();

}






std::list<EntryArea::OutlineItem> EntryArea::getOutline() const
{
    std::list<EntryArea::OutlineItem> result;

    int position = 0;
    for (BlockWidget* p : blocks)
    {
        if (HeaderBlockWidget* q=qobject_cast<HeaderBlockWidget*>(p))
        {
            result.push_back(EntryArea::OutlineItem(
                q->toQString(),
                q->getLevel(),
                position
            ));
        }
        position++;
    }
    return result;
}


EntryArea* EntryArea::open(QWidget* parent, const QDir& dir)
{
    EntryArea* p = new EntryArea(parent, dir);
    
    //p->load();
    p->loadQtXml();
    return p;

}
EntryArea* EntryArea::initialize(QWidget* parent, const QDir& dir)
{
    EntryArea* p = new EntryArea(parent, dir);
    
    //p->dump();
    p->dumpQtXml();
    return p;
}

QDir EntryArea::getRootDir() const
{
    return root_dir;
}


// slots:
void EntryArea::contentChangeSlot()
{
    if (saved)
    {
        saved = false;
        emit contentChange();  // 仅在下降沿触发
        //qDebug() << "contentChange\n";
    }

}

void EntryArea::titleChangeSlot()
{
    emit titleChange();
}


void EntryArea::handleInsertAboveFromBlock(BlockWidget* block, BlockType type)
{
    insertBlockAbove(block, type);
}
void EntryArea::handleInsertBelowFromBlock(BlockWidget* block, BlockType type)
{
    insertBlockBelow(block, type);
}
void EntryArea::handleDeleteFromBlock(BlockWidget* block)
{
    try {
        deleteBlockWidget(block);
    }
    catch (...) {
        qWarning() << "EntryArea::handleDeleteFromBlock(BlockWidget* block) ERROR";
        return;
    }
}

void EntryArea::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(nullptr);
    connect(menu.addAction("在末尾添加文本块"), &QAction::triggered, this, &EntryArea::handleAppendTextBlock);
    connect(menu.addAction("在末尾添加代码块"), &QAction::triggered, this, &EntryArea::handleAppendCodeBlock);
    connect(menu.addAction("在末尾添加图片块"), &QAction::triggered, this, &EntryArea::handleAppendImageBlock);
    connect(menu.addAction("在末尾添加标题块"), &QAction::triggered, this, &EntryArea::handleAppendHeaderBlock);
    menu.exec(event->globalPos());
}

void EntryArea::handleAppendTextBlock()
{
    appendBlock(BlockType::Text);
}
void EntryArea::handleAppendCodeBlock()
{
    appendBlock(BlockType::Code);
}
void EntryArea::handleAppendImageBlock()
{
    appendBlock(BlockType::Image);
}
void EntryArea::handleAppendHeaderBlock()
{
    appendBlock(BlockType::Header);
}