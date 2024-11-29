#pragma once
#include <QWidget>
#include <QFocusEvent>
#include <QtXml>

class EntryArea;  // 对BlockArea进行声明，方便定义友元
class FocusEventFilter;


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

protected:
	FocusEventFilter* filter;

public:
	BlockWidget(QWidget* parent);
	virtual ~BlockWidget();
	// 下面这些纯虚函数的存在使得该类本身不能实例化，只有实现了这些函数子类才能实例化
	virtual BlockType type() const = 0;
	virtual void exportToQtXml(QDomElement& dest, QDomDocument& dom_doc) = 0;
	virtual void importFromQtXml(QDomElement& src) = 0;
	virtual void deleteFile();  // 专为含有文件的块设计，默认行为是什么也不做
	virtual void clearUndoStack() = 0;
signals:
	void contentChange();
signals:
	void insertAbove(BlockWidget*, BlockType);  // 当用户通过右键菜单在上方插入时发送
signals:
	void insertBelow(BlockWidget*, BlockType);  // 当用户通过右键菜单在下方插入时发送
signals:
	void deletThisBlock(BlockWidget*);  // 当用户通过右键菜单删除此块时发送

protected slots:
	void emitContentChange();

	void handleContextMenuInsertAboveText();
	void handleContextMenuInsertAboveCode();
	void handleContextMenuInsertAboveImage();
	void handleContextMenuInsertAboveHeader();

	void handleContextMenuInsertBelowText();
	void handleContextMenuInsertBelowCode();
	void handleContextMenuInsertBelowImage();
	void handleContextMenuInsertBelowHeader();

	void handleContextMenuDelete();

	virtual void handleContextMenuQueryFromControls(QContextMenuEvent* event);

};

// 焦点移动事件过滤器
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
	bool eventFilter(QObject* watched, QEvent* event);
};

class BlockControl
{
public:
	virtual void undo() = 0;
	virtual void redo() = 0;
	virtual void clearUndoStack() = 0;

};
