#include "BlockWidget.h"
#include <QPropertyAnimation>



BlockWidget::BlockWidget(QWidget* parent) :
	QWidget(parent), 
	//last(0), 
	//next(0),
	filter(new FocusEventFilter(this))
{

}

// 基类BlockWidget析构函数
BlockWidget::~BlockWidget()
{
	if (filter)
	{
		delete filter;
		filter = nullptr;
	}
}

//BlockWidget* BlockWidget::getLast() const { return last; }
//BlockWidget* BlockWidget::getNext() const { return next; }
//void BlockWidget::setLast(BlockWidget* p) { last = p; }
//void BlockWidget::setNext(BlockWidget* p) { next = p; }

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



// FocusEventFilter相关内容
// static BlockWidget* focus;
BlockWidget* FocusEventFilter::focus = nullptr;


BlockWidget* FocusEventFilter::getFocus()
{
	return focus;
}

bool FocusEventFilter::eventFilter(QObject* watched, QEvent* event) 
{

	// 检查事件类型  
	switch (event->type())
	{
	case QFocusEvent::FocusIn:
		focus = target;
		return false;
	case QFocusEvent::FocusOut:
		focus = 0;
		return false;
	default:
		// 对于其他事件类型，继续传递事件  
		return QObject::eventFilter(watched, event);
	}
}