#include "TextBlockWidget.h"
#include <QMenu>


void TextBlockEdit::contextMenuEvent(QContextMenuEvent* event)
{
    emit contextMenuQuery(event);
}


void TextBlockWidget::contextMenuEvent(QContextMenuEvent* event)
{
    // 创建一个菜单
    QMenu contextMenu(nullptr);

    // 添加菜单项
    QAction* action_undo = contextMenu.addAction("撤销");
    connect(action_undo, &QAction::triggered, this, &TextBlockWidget::handleContextMenuUndo);

    QAction* action_redo = contextMenu.addAction("重做");
    connect(action_redo, &QAction::triggered, this, &TextBlockWidget::handleContextMenuRedo);

    QAction* action_delete = contextMenu.addAction("删除");
    connect(action_delete, &QAction::triggered, this, &TextBlockWidget::handleContextMenuDelete);

    QMenu* menu_insert_above = contextMenu.addMenu("在上方插入");
    QAction* action_insert_above_text = menu_insert_above->addAction("文本块");
    connect(action_insert_above_text, &QAction::triggered, this, &TextBlockWidget::handleContextMenuInsertAboveText);

    QAction* action_insert_above_code = menu_insert_above->addAction("代码块");
    connect(action_insert_above_code, &QAction::triggered, this, &TextBlockWidget::handleContextMenuInsertAboveCode);

    QAction* action_insert_above_image = menu_insert_above->addAction("图片块");
    connect(action_insert_above_image, &QAction::triggered, this, &TextBlockWidget::handleContextMenuInsertAboveImage);

    QAction* action_insert_above_header = menu_insert_above->addAction("标题块");
    connect(action_insert_above_header, &QAction::triggered, this, &TextBlockWidget::handleContextMenuInsertAboveHeader);

    QMenu* menu_insert_below = contextMenu.addMenu("在下方插入");
    QAction* action_insert_below_text = menu_insert_below->addAction("文本块");
    connect(action_insert_below_text, &QAction::triggered, this, &TextBlockWidget::handleContextMenuInsertBelowText);

    QAction* action_insert_below_code = menu_insert_below->addAction("代码块");
    connect(action_insert_below_code, &QAction::triggered, this, &TextBlockWidget::handleContextMenuInsertBelowCode);

    QAction* action_insert_below_image = menu_insert_below->addAction("图片块");
    connect(action_insert_below_image, &QAction::triggered, this, &TextBlockWidget::handleContextMenuInsertBelowImage);

    QAction* action_insert_below_header = menu_insert_below->addAction("标题块");
    connect(action_insert_below_header, &QAction::triggered, this, &TextBlockWidget::handleContextMenuInsertBelowHeader);

    // 在事件发生的位置显示菜单
    contextMenu.exec(event->globalPos());
}


void TextBlockWidget::handleContextMenuUndo()
{
    this->text_edit->undo();
}
void TextBlockWidget::handleContextMenuRedo()
{
    this->text_edit->redo();
}
