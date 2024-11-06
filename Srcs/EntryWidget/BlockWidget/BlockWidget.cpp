#include "BlockWidget.h"

// BlockWidget相关内容
// 基类BlockWidget构造函数
BlockWidget::BlockWidget(QWidget* parent) :
	QWidget(parent), 
	last(0), 
	next(0)
{

}

// 基类BlockWidget析构函数
BlockWidget::~BlockWidget() = default;

BlockWidget* BlockWidget::getLast() const { return last; }
BlockWidget* BlockWidget::getNext() const { return next; }
void BlockWidget::setLast(BlockWidget* p) { last = p; }
void BlockWidget::setNext(BlockWidget* p) { next = p; }
//EntryArea* BlockWidget::getEntry() const { return entry; }

void BlockWidget::emitContentChange()
{
	emit contentChange();
}


void BlockWidget::emitLinkClicked(QUrl url)
{
	emit linkClicked(url);
}

// FocusEventFilter相关内容
// static BlockWidget* focus;
BlockWidget* FocusEventFilter::focus = nullptr;


BlockWidget* FocusEventFilter::getFocus()
{
	return focus;
}

bool FocusEventFilter::eventFilter(QObject* watched, QFocusEvent* event)
{
	// 检查事件类型  
	if (event->type() == QFocusEvent::FocusIn) {
		focus = target;
		return true;
	}
	if (event->type() == QFocusEvent::FocusOut) {
		focus = 0;
		return true; 
	}
	
	// 对于其他事件类型，继续传递事件  
	return QObject::eventFilter(watched, event);
	
}