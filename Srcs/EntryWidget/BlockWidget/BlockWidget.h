#pragma once
#include <QWidget>
#include "../pugixml/pugixml.hpp"
#include <QFocusEvent>

class EntryArea;  // 对BlockArea进行声明，方便定义友元

enum class BlockType  // 块的四种类型
{
	Text, Code, Image, Header
};


// 块窗口基类
class BlockWidget : public QWidget
{
	Q_OBJECT
private:
	friend class EntryArea;
	void setLast(BlockWidget* p);
	void setNext(BlockWidget* p);
protected:
	BlockWidget* last, * next;
public:
	BlockWidget(QWidget* parent);
	virtual ~BlockWidget();
	BlockWidget* getLast() const;
	BlockWidget* getNext() const;
	// 下面这些纯虚函数的存在使得该类本身不能实例化，只有实现了这些函数子类才能实例化
	virtual BlockType type() const = 0;
	virtual void exportToPugi(pugi::xml_node& dest) = 0;
	virtual void importFromPugi(const pugi::xml_node& node) = 0;

signals:
	void contentChange();

signals:
	void linkClicked(QUrl);
protected slots:
	void emitContentChange();
	void emitLinkClicked(QUrl url);

};


class FocusEventFilter : public QObject
{
	Q_OBJECT
public:
	FocusEventFilter(BlockWidget* target) :target(target) {};
	static BlockWidget* getFocus();
private:
	BlockWidget* const target;
	static BlockWidget* focus;
protected:
	bool eventFilter(QObject* watched, QFocusEvent* event);
};

