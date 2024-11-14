#include "EntryArea.h"
#include <sstream>


int EntryArea::loadQtXml()
{
    blockSignals(true);
    // 读取文件内容
    if (!(entry_file.open(QFile::ReadOnly | QFile::Text)))
    {
        // 文件打开失败
        entry_file.close();
        return 0;
    }
    // 文件打开成功
    QDomDocument doc;
    doc.setContent(entry_file.readAll());
    entry_file.close();
    QDomElement root = doc.documentElement();

    for (auto child = root.firstChildElement(); !child.isNull(); child = child.nextSiblingElement())
    {
        
        QString name = child.tagName();
        if (name == "text-block")
        {
            TextBlockWidget* widget = new TextBlockWidget(this->widget());
            widget->importFromQtXml(child);
            appendBlock(widget);
            connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
            connect(widget, &BlockWidget::insertAbove, this, &EntryArea::handleInsertAboveFromBlock);
            connect(widget, &BlockWidget::insertBelow, this, &EntryArea::handleInsertBelowFromBlock);
            connect(widget, &BlockWidget::deletThisBlock, this, &EntryArea::handleDeleteFromBlock);
        }
        else if (name == "code-block")
        {
            CodeBlockWidget* widget = new CodeBlockWidget(this->widget());
            widget->importFromQtXml(child);
            appendBlock(widget);
            connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
            connect(widget, &BlockWidget::insertAbove, this, &EntryArea::handleInsertAboveFromBlock);
            connect(widget, &BlockWidget::insertBelow, this, &EntryArea::handleInsertBelowFromBlock);
            connect(widget, &BlockWidget::deletThisBlock, this, &EntryArea::handleDeleteFromBlock);
        }

        else if (name == "image-block")
        {
            ImageBlockWidget* widget = ImageBlockWidget::initializeFromQtXml(this->widget(), attachment_dir, child);
            appendBlock(widget);
            connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
            connect(widget, &BlockWidget::insertAbove, this, &EntryArea::handleInsertAboveFromBlock);
            connect(widget, &BlockWidget::insertBelow, this, &EntryArea::handleInsertBelowFromBlock);
            connect(widget, &BlockWidget::deletThisBlock, this, &EntryArea::handleDeleteFromBlock);
        }
        else if (name == "header-block")
        {
            HeaderBlockWidget* widget = new HeaderBlockWidget(this->widget());
            widget->importFromQtXml(child);
            appendBlock(widget);
            connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
            connect(widget, &BlockWidget::insertAbove, this, &EntryArea::handleInsertAboveFromBlock);
            connect(widget, &BlockWidget::insertBelow, this, &EntryArea::handleInsertBelowFromBlock);
            connect(widget, &BlockWidget::deletThisBlock, this, &EntryArea::handleDeleteFromBlock);
            connect(widget, &BlockWidget::contentChange, this, &EntryArea::titleChangeSlot);
        }
        else
        {
            qDebug() << "Unknown type of xml child:" << name;
        }
    }
    
    blockSignals(false);
    saved = true;  // 加载完的是保存好的
    return 1;

}

int EntryArea::dumpQtXml()
{
    QDomDocument doc;

    QDomElement root = doc.createElement("entry");
    doc.appendChild(root);

    for (BlockWidget* iter : blocks)
    {
        QDomElement block_elem = doc.createElement("temp-block");
        iter->exportToQtXml(block_elem, doc);
        root.appendChild(block_elem);
        qDebug() << block_elem.tagName();
        qDebug();
    }
    // 导出为文件
    if (!(entry_file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)))
    {
        // 文件打开失败
        entry_file.close();
        return 0;
    }
    // 文件打开成功
    QTextStream stream(&entry_file);
    stream << doc.toString();
    
    entry_file.close();
    saved = true;
    // 清空undo栈
    for (BlockWidget* p : blocks)
    {
        p->clearUndoStack();
    }

    return 1;
}