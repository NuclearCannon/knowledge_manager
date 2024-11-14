#include "EntryArea.h"
TextBlockWidget* EntryArea::createTextBlock()
{
    TextBlockWidget* widget = new TextBlockWidget(this->widget());
    connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
    connect(widget, &BlockWidget::insertAbove, this, &EntryArea::handleInsertAboveFromBlock);
    connect(widget, &BlockWidget::insertBelow, this, &EntryArea::handleInsertBelowFromBlock);
    connect(widget, &BlockWidget::deletThisBlock, this, &EntryArea::handleDeleteFromBlock);
    return widget;
}
CodeBlockWidget* EntryArea::createCodeBlock()
{
    CodeBlockWidget* widget = new CodeBlockWidget(this->widget());
    connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
    connect(widget, &BlockWidget::insertAbove, this, &EntryArea::handleInsertAboveFromBlock);
    connect(widget, &BlockWidget::insertBelow, this, &EntryArea::handleInsertBelowFromBlock);
    connect(widget, &BlockWidget::deletThisBlock, this, &EntryArea::handleDeleteFromBlock);
    return widget;
}
ImageBlockWidget* EntryArea::createImageBlock()
{
    
    ImageBlockWidget* widget = ImageBlockWidget::initialize(this->widget(), attachment_dir);
    if (!widget)return nullptr;
    connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
    connect(widget, &BlockWidget::insertAbove, this, &EntryArea::handleInsertAboveFromBlock);
    connect(widget, &BlockWidget::insertBelow, this, &EntryArea::handleInsertBelowFromBlock);
    connect(widget, &BlockWidget::deletThisBlock, this, &EntryArea::handleDeleteFromBlock);
    return widget;
}
HeaderBlockWidget* EntryArea::createHeaderBlock()
{
    HeaderBlockWidget* widget = new HeaderBlockWidget(this->widget());
    connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
    connect(widget, &BlockWidget::insertAbove, this, &EntryArea::handleInsertAboveFromBlock);
    connect(widget, &BlockWidget::insertBelow, this, &EntryArea::handleInsertBelowFromBlock);
    connect(widget, &BlockWidget::contentChange, this, &EntryArea::titleChangeSlot);
    connect(widget, &BlockWidget::deletThisBlock, this, &EntryArea::handleDeleteFromBlock);
    
    return widget;
}


int EntryArea::find(BlockWidget* block_widget)
{
    return blocks.indexOf(block_widget);
    
}

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

BlockWidget* EntryArea::getWidgetByIndex(int index) {
    return blocks[index];
    
}



BlockWidget* EntryArea::newBlockWidgetByType(BlockType type)
{
    BlockWidget* new_block;
    switch (type)
    {

    case BlockType::Text:
        new_block = createTextBlock();
        break;
    case BlockType::Code:
        new_block = createCodeBlock();
        break;
    case BlockType::Image:
        new_block = createImageBlock();
        break;
    case BlockType::Header:
        new_block = createHeaderBlock();
        break;
    default:
        throw "unknown type of block insert";
        break;
    }
    return new_block;
}

void EntryArea::insertBlockAbove(BlockWidget* block_widget, BlockType type)
{
    
    BlockWidget* new_block = newBlockWidgetByType(type);
    if(new_block)insertBlockAbove(block_widget, new_block);
}

void EntryArea::insertBlockAbove(BlockWidget* block, BlockWidget* new_block)
{
    int index = blocks.indexOf(block);
    blocks.insert(index, new_block);
    layout->insertWidget(index, new_block);
    contentChangeSlot();
    if (block->type() == BlockType::Header)
    {
        titleChangeSlot();
    }
}

int EntryArea::insertBlockAbove(BlockType type)
{
    BlockWidget* current = FocusEventFilter::getFocus();
    if (current == nullptr)return 1;
    if (!has(current))return 2;
    try {
        insertBlockAbove(current, type);
    }
    catch (...) {
        return 3;
    }
    return 0;

}




void EntryArea::insertBlockBelow(BlockWidget* block_widget, BlockType type)
{
    BlockWidget* new_block = newBlockWidgetByType(type);
    if (new_block)insertBlockBelow(block_widget, new_block);

}

void EntryArea::insertBlockBelow(BlockWidget* block, BlockWidget* new_block)
{
    int index = blocks.indexOf(block)+1;
    blocks.insert(index, new_block);
    layout->insertWidget(index, new_block);
    contentChangeSlot();
    if (block->type() == BlockType::Header)
    {
        titleChangeSlot();
    }
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
    BlockWidget* new_block = newBlockWidgetByType(type);
    if (new_block)appendBlock(new_block);
}

void EntryArea::appendBlock(BlockWidget* block)
{
    layout->insertWidget(blocks.size(), block);
    blocks.append(block);
    contentChangeSlot();
    if (block->type() == BlockType::Header)
    {
        titleChangeSlot();
    }
}
void EntryArea::deleteBlockWidget(BlockWidget* block)
{
    BlockType type = block->type();
    block->setParent(nullptr);
    block->deleteFile();// 清空附件，如果有的话
    blocks.removeAll(block);
    layout->removeWidget(block);
    block->hide();
    delete block;
    contentChangeSlot();
    if (type == BlockType::Header)titleChangeSlot();
}

int EntryArea::deleteCurrentBlock()
{
    
    BlockWidget* current = FocusEventFilter::getFocus();
    if (current == nullptr)return 1;
    if (!has(current))return 2;
    try {
        deleteBlockWidget(current);
    }
    catch (...) {
        return 3;
    }
    return 0;
}

