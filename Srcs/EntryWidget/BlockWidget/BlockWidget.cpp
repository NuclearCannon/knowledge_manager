#include "BlockWidget.h"
#include <QPropertyAnimation>



BlockWidget::BlockWidget(QWidget* parent) :
	QWidget(parent), 
	last(0), 
	next(0),
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

BlockWidget* BlockWidget::getLast() const { return last; }
BlockWidget* BlockWidget::getNext() const { return next; }
void BlockWidget::setLast(BlockWidget* p) { last = p; }
void BlockWidget::setNext(BlockWidget* p) { next = p; }

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
	//case QMouseEvent::Enter:
	//	target->setStyleSheet("background-color: #EEEEEE;");
	//	return true;
	//case QMouseEvent::Leave:
	//	target->setStyleSheet("");
	//	return true;
	default:
		// 对于其他事件类型，继续传递事件  
		return QObject::eventFilter(watched, event);
	}
}