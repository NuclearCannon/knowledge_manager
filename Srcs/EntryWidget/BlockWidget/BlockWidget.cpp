#include "BlockWidget.h"
#include <QPropertyAnimation>
#include <QApplication>


BlockWidget::BlockWidget(QWidget* parent) :
	QWidget(parent)
{

}

// 基类BlockWidget析构函数
BlockWidget::~BlockWidget()
{
	return;
}



void BlockWidget::emitContentChange()
{
	emit contentChange();
}


void BlockWidget::handleContextMenuInsertAboveText()
{
	emit insertAbove(this, BlockType::Text);
}
void BlockWidget::handleContextMenuInsertAboveCode()
{
	emit insertAbove(this, BlockType::Code);
}
void BlockWidget::handleContextMenuInsertAboveImage()
{
	emit insertAbove(this, BlockType::Image);
}
void BlockWidget::handleContextMenuInsertAboveHeader()
{
	emit insertAbove(this, BlockType::Header);
}

void BlockWidget::handleContextMenuInsertBelowText()
{
	emit insertBelow(this, BlockType::Text);
}
void BlockWidget::handleContextMenuInsertBelowCode()
{
	emit insertBelow(this, BlockType::Code);
}
void BlockWidget::handleContextMenuInsertBelowImage()
{
	emit insertBelow(this, BlockType::Image);
}
void BlockWidget::handleContextMenuInsertBelowHeader()
{
	emit insertBelow(this, BlockType::Header);
}

void BlockWidget::handleContextMenuDelete()
{
	emit deletThisBlock(this);
}
void BlockWidget::handleContextMenuQueryFromControls(QContextMenuEvent* event)
{
	this->contextMenuEvent(event);
}





void BlockWidget::deleteFile()
{
	// do nothing
}

BlockWidget* BlockWidget::focusBlockWidget()
{
	QWidget* widget = QApplication::focusWidget();
	BlockWidget* block = qobject_cast<BlockWidget*>(widget);
	if (block)return block;
	BlockControl* control = dynamic_cast<BlockControl*>(widget);
	if (control)return control->getParent();
	return nullptr;
}

BlockControl::BlockControl(BlockWidget* parent) :parent(parent) {};

BlockWidget* BlockControl::getParent() const
{
	return parent;
}

