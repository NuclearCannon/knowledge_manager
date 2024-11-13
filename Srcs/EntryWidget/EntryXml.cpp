#include "EntryArea.h"
#include <sstream>


int EntryArea::loadQtXml()
{
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

    for (auto child = root.firstChild(); !child.isNull(); child = child.nextSibling())
    {
        auto elem = child.toElement();
        if (elem.isNull())
        {
            qDebug() << child.nodeType();  // 3, text
            if (child.isText())
            {
                qDebug() << "Text:" << child.toText().data();
            }
            qDebug();

        }
        QString name = elem.tagName();

        if (name == "text-block")
        {
            TextBlockWidget* widget = new TextBlockWidget(this->widget());
            widget->importFromQtXml(elem);
            appendBlock(widget);
            connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
        }
        else if (name == "code-block")
        {
            CodeBlockWidget* widget = new CodeBlockWidget(this->widget());
            widget->importFromQtXml(elem);
            appendBlock(widget);
            connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
        }

        else if (name == "image-block")
        {
            ImageBlockWidget* widget = new ImageBlockWidget(this->widget(), attachment_dir);
            widget->importFromQtXml(elem);
            appendBlock(widget);
            connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
        }
        else if (name == "header-block")
        {
            HeaderBlockWidget* widget = new HeaderBlockWidget(this->widget());
            widget->importFromQtXml(elem);
            appendBlock(widget);
            connect(widget, &BlockWidget::contentChange, this, &EntryArea::contentChangeSlot);
            connect(widget, &BlockWidget::contentChange, this, &EntryArea::titleChangeSlot);
        }
        else
        {
            qDebug() << "Unknown type of xml child:" << name;
        }
    }
    return 1;

}

int EntryArea::dumpQtXml()
{
    QDomDocument doc;

    QDomElement root = doc.createElement("entry");
    doc.appendChild(root);

    for (BlockWidget* iter = head; iter; iter = iter->next)
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
    this->saved = true;
    entry_file.close();
    return 1;
}