#include "EntryArea.h"
TextBlockWidget* EntryArea::createTextBlock()
{
    TextBlockWidget* widget = new TextBlockWidget(this->widget());
    connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
    connect(widget, &BlockWidget::insertAbove, this, &EntryArea::handleInsertAboveFromBlock);
    connect(widget, &BlockWidget::insertBelow, this, &EntryArea::handleInsertBelowFromBlock);
    return widget;
}
CodeBlockWidget* EntryArea::createCodeBlock()
{
    CodeBlockWidget* widget = new CodeBlockWidget(this->widget());
    connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
    connect(widget, &BlockWidget::insertAbove, this, &EntryArea::handleInsertAboveFromBlock);
    connect(widget, &BlockWidget::insertBelow, this, &EntryArea::handleInsertBelowFromBlock);
    return widget;
}
ImageBlockWidget* EntryArea::createImageBlock()
{
    ImageBlockWidget* widget = new ImageBlockWidget(this->widget(), attachment_dir);
    connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
    connect(widget, &BlockWidget::insertAbove, this, &EntryArea::handleInsertAboveFromBlock);
    connect(widget, &BlockWidget::insertBelow, this, &EntryArea::handleInsertBelowFromBlock);
    return widget;
}
HeaderBlockWidget* EntryArea::createHeaderBlock()
{
    HeaderBlockWidget* widget = new HeaderBlockWidget(this->widget());
    connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
    connect(widget, &BlockWidget::insertAbove, this, &EntryArea::handleInsertAboveFromBlock);
    connect(widget, &BlockWidget::insertBelow, this, &EntryArea::handleInsertBelowFromBlock);
    connect(widget, &BlockWidget::contentChange, this, &EntryArea::titleChangeSlot);
    
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
    connect(new_block, &BlockWidget::insertAbove, this, &EntryArea::handleInsertAboveFromBlock);
    connect(new_block, &BlockWidget::insertBelow, this, &EntryArea::handleInsertBelowFromBlock);
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
    int index = blocks.indexOf(block);
    blocks.insert(index, new_block);
    layout->insertWidget(index, new_block);
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
    insertBlockBelow(block_widget, new_block);

}

void EntryArea::insertBlockBelow(BlockWidget* block, BlockWidget* new_block)
{
    int index = blocks.indexOf(block)+1;
    blocks.insert(index, new_block);
    layout->insertWidget(index, new_block);
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
    layout->insertWidget(blocks.size(), block);
    blocks.append(block);
}

int EntryArea::deleteCurrentBlock()
{
    BlockWidget* current = FocusEventFilter::getFocus();
    if (current == nullptr)return 1;
    if (!has(current))return 2;
    try {
        blocks.removeAll(current);
        layout->removeWidget(current);
        current->hide();
        delete current;
    }
    catch (...) {
        return 3;
    }
    return 0;
}

