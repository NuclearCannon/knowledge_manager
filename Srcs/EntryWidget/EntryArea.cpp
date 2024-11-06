#include "EntryArea.h"
#include "BlockWidget/BlockWidgets.h"
#include <QApplication>
#include <sstream>


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
        return false;
    }

    if (!sourceFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open source file for reading:" << sourceFilePath;
        destinationFile.close();
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

// EntryArea::OutlineItem
EntryArea::OutlineItem::OutlineItem(QString text, int level, int position) :text(text), level(level), position(position) {};

// EntryArea(private)
int EntryArea::insert(int index, BlockWidget* ptr)
{
    layout->insertWidget(index, ptr);
    return 0;
}

int EntryArea::find(BlockWidget* block_widget)
{
    for (int i = 0; i < layout->count(); ++i) {
        QLayoutItem* item = layout->itemAt(i);
        if (item->widget() == block_widget)
        {
            return i;
        }
    }
    return -1;
}

void EntryArea::load()
{
    // 创建一个pugi::xml_document对象  
    pugi::xml_document doc;
    doc.load_file(root_dir.absoluteFilePath("entry.xml").toStdString().c_str());

    // 从字符串加载XML内容到doc中  
    pugi::xml_node root = doc.root();
    for (pugi::xml_node child = root.first_child(); child; child = child.next_sibling())
    {

        if (strcmp(child.name(), "text-block") == 0)
        {
            TextBlockWidget* widget = new TextBlockWidget(this->widget());
            widget->importFromPugi(child);
            appendBlock(widget);
            connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
        }
        else if (strcmp(child.name(), "code-block") == 0)
        {
            CodeBlockWidget* widget = new CodeBlockWidget(this->widget());
            widget->importFromPugi(child);
            appendBlock(widget);
            connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
        }

        else if (strcmp(child.name(), "image-block") == 0)
        {
            ImageBlockWidget* widget = new ImageBlockWidget(this->widget(), attachment_dir);
            widget->importFromPugi(child);
            appendBlock(widget);
            connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
        }
        else if (strcmp(child.name(), "header-block") == 0)
        {
            HeaderBlockWidget* widget = new HeaderBlockWidget(this->widget());
            widget->importFromPugi(child);
            appendBlock(widget);
            connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
            connect(widget, &BlockWidget::contentChange, this, &EntryArea::titleChangeSlot);
        }
        else
        {
            qDebug() << "Unknown type of xml child:" << child.name();
        }
    }
    return;

}

// EntryArea(public)
// 构造函数
EntryArea::EntryArea(QWidget* parent, const QDir& root) : 
    QScrollArea(parent), 
    layout(0), 
    head(0), 
    tail(0),
    focus(nullptr),
    root_dir(root),
    attachment_dir(root_dir.absoluteFilePath("attachments")),
    saved(true)

{
    setWidget(new QWidget);
    setWidgetResizable(true);
    qDebug() << "BlockArea initialized";
    layout = new QVBoxLayout(this->widget());
    layout->addStretch(1);

}
// 使用默认析构函数
bool EntryArea::has(BlockWidget* block) const
{
    return blocks.contains(block);
}

int EntryArea::rollTo(int position)
{
    BlockWidget* p = getWidgetByIndex(position);
    if (p == nullptr)
    {
        return 0;
    }
    else
    {
        this->ensureWidgetVisible(p);
        return 1;
    }
}

int EntryArea::length() const
{
    return layout->count() - 1;  // 减去的这一个1是Stretch
}


BlockWidget* EntryArea::getWidgetByIndex(int index) {
    if (index < 0 || index >= length())
    {
        return nullptr;  // 如果下标无效，返回 nullptr  
    }
    QLayoutItem* item = layout->itemAt(index);
    QWidget* widget = item->widget();
    BlockWidget* block_widget = qobject_cast<BlockWidget*>(widget);

    return block_widget;
}



BlockWidget* EntryArea::newBlockWidgetByType(BlockType type)
{
    BlockWidget* new_block;
    switch (type)
    {
    
    case BlockType::Text:
        new_block = new TextBlockWidget(this->widget());
        break;
    case BlockType::Code:
        new_block = new CodeBlockWidget(this->widget());
        break;
    case BlockType::Image:
        new_block = new ImageBlockWidget(this->widget(), attachment_dir);
        break;
    case BlockType::Header: 
        new_block = new HeaderBlockWidget(this->widget());
        // 对于标题块来说，内容的变动就意味着标题的EntryArea标题的变动！
        connect(new_block, &BlockWidget::contentChange, this, &EntryArea::titleChangeSlot);
        // 手动调用titleChangeSlot
        titleChangeSlot();
        break; 
    default:
        throw "unknown type of block insert";
        break;
    }
    connect(new_block, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
    // 新块意味着内容变动，手动调用contentChangeSlot
    contentChangeSlot();
    return new_block;
}

void EntryArea::insertBlockAbove(BlockWidget* block_widget, BlockType type)
{
    BlockWidget* new_block = newBlockWidgetByType(type);
    insertBlockAbove(block_widget, new_block);
}

void EntryArea::insertBlockAbove(BlockWidget* block, BlockWidget* new_block)
{
    BlockWidget* last = block->getLast();
    new_block->setLast(last);
    new_block->setNext(block);

    if (last)last->setNext(new_block);
    block->setLast(new_block);

    layout->insertWidget(find(block), new_block);
    // 如果新插入的块成为了head，需要更新head
    if (block == head)head = new_block;
    blocks.insert(block);

}

int EntryArea::insertBlockAbove(BlockType type)
{
    BlockWidget* current = FocusEventFilter::getFocus();
    if (current == nullptr)return 1;
    if (!has(current))return 2;
    try {
        insertBlockAbove(current, type);
    }
    catch(...) {
        return 3;
    }
    return 0;

}




void EntryArea::insertBlockBelow(BlockWidget* block_widget, BlockType type)
{
    BlockWidget* new_block = newBlockWidgetByType(type);
    insertBlockBelow(block_widget, new_block);

}

void EntryArea::insertBlockBelow(BlockWidget* block, BlockWidget* new_block)
{
    BlockWidget* next = block->getNext();
    new_block->setNext(next);
    new_block->setLast(block);

    if (next)next->setLast(new_block);
    block->setNext(new_block);

    layout->insertWidget(find(block) + 1, new_block);
    // 如果新插入的块成为了末尾，需要更新tail
    if (block == tail)tail = new_block;
    blocks.insert(block);
}

int EntryArea::insertBlockBelow(BlockType type)
{
    BlockWidget* current = FocusEventFilter::getFocus();
    if (current == nullptr)return 1;
    if (!has(current))return 2;
    try {
        insertBlockBelow(current, type);
    }
    catch (...) {
        return 3;
    }
    return 0;
}


void EntryArea::appendBlock(BlockType type)
{
    appendBlock(newBlockWidgetByType(type));
}

void EntryArea::appendBlock(BlockWidget* block)
{
    if (tail) // 如果有至少一个块，取最后的那个块
    {
        insertBlockBelow(tail, block);
    }
    else  // 如果一个块都没有
    {
        head = tail = block;
        layout->insertWidget(0, block);
        blocks.insert(block);
       
    }
}

int EntryArea::deleteCurrentBlock()
{
    BlockWidget* current = FocusEventFilter::getFocus();
    if (current == nullptr)return 1;
    if (!has(current))return 2;
    try {
        blocks.remove(current);
        current->last->setNext(current->next);
        current->next->setLast(current->last);
        current->last = current->next = nullptr;
        layout->removeWidget(current);
        current->hide();
        delete current;

    }
    catch (...) {
        return 3;
    }
    return 0;
}


void EntryArea::dump()
{
    pugi::xml_document doc;
    pugi::xml_node root = doc.root();

    root.set_name("entry");

    for (BlockWidget* iter = head; iter; iter = iter->next)
    {
        std::ostringstream oss;
        root.print(oss);
        

        pugi::xml_node block_node = root.append_child(pugi::node_element);
        if (block_node.empty())
        {
            qDebug() << "BlockArea::exportBlocksToStdString: empty!";
            continue;
        }
        iter->exportToPugi(block_node);

    }
    // 导出为文件
    
    doc.save_file(root_dir.absoluteFilePath("entry.xml").toStdString().c_str());

    this->saved = true;

}



void EntryArea::clear()
{
    BlockWidget* p = head, * q;
    while (p)
    {
        q = p->next;
        layout->removeWidget(p);
        delete p;
        p = q;

    }
    head = tail = nullptr;
}


std::list<EntryArea::OutlineItem> EntryArea::getOutline() const
{
    std::list<EntryArea::OutlineItem> result;

    int position = 0;
    for (BlockWidget* p = head; p; p = p->next, position++)
    {
        if (HeaderBlockWidget* q=qobject_cast<HeaderBlockWidget*>(p))
        {
            result.push_back(EntryArea::OutlineItem(
                q->toQString(),
                q->getLevel(),
                position
            ));
        }
    }
    return result;
}


EntryArea* EntryArea::open(QWidget* parent, const QDir& dir)
{
    EntryArea* p = new EntryArea(parent, dir);
    p->load();
    return p;

}
EntryArea* EntryArea::initialize(QWidget* parent, const QDir& dir)
{
    EntryArea* p = new EntryArea(parent, dir);
    p->dump();
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
    }

}

void EntryArea::titleChangeSlot()
{

	qDebug() << "titleChange\n";
    emit titleChange();
}

void EntryArea::emitLinkClicked(const QUrl& url)
{
    emit linkClicked(url);
}
