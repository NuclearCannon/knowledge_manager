#include <QMessageBox>

#include "../MainWindow/KMMainWindow.h"
#include "../EntryWidget/EntryWidget.h"

// 槽：点击添加文本块时，添加一个文本块
void KMMainWindow::actAddTextBlock()
{
	EntryWidget* entry_widget = static_cast<EntryWidget*>(ui.tab_widget->currentWidget());
	if (entry_widget == nullptr)
	{
		QMessageBox::warning(this, "错误", "当前没有打开的词条！");
		return;
	}

	EntryArea* entry_area = entry_widget->getEntryArea();

	if (entry_area == nullptr)
	{
		QMessageBox::warning(this, "错误", "词条缺少编辑区域！非法词条！");
		return;
	}

	// 添加一个文本块
	entry_area->appendBlock(BlockType::Text);
}

// 槽：点击添加代码块时，添加一个代码块
void KMMainWindow::actAddCodeBlock()
{
	EntryWidget* entry_widget = static_cast<EntryWidget*>(ui.tab_widget->currentWidget());
	if (entry_widget == nullptr)
	{
		QMessageBox::warning(this, "错误", "当前没有打开的词条！");
		return;
	}

	EntryArea* entry_area = entry_widget->getEntryArea();

	if (entry_area == nullptr)
	{
		QMessageBox::warning(this, "错误", "词条缺少编辑区域！非法词条！");
		return;
	}

	// 添加一个代码块
	entry_area->appendBlock(BlockType::Code);
}

// 槽：添加一个图片块
void KMMainWindow::actAddImageBlock()
{
	EntryWidget* entry_widget = static_cast<EntryWidget*>(ui.tab_widget->currentWidget());
	if (entry_widget == nullptr)
	{
		QMessageBox::warning(this, "错误", "当前没有打开的词条！");
		return;
	}

	EntryArea* entry_area = entry_widget->getEntryArea();

	if (entry_area == nullptr)
	{
		QMessageBox::warning(this, "错误", "词条缺少编辑区域！非法词条！");
		return;
	}

	entry_area->appendBlock(BlockType::Image);
}

// 槽：添加一个标题块
void KMMainWindow::actAddHeaderBlock()
{
	EntryWidget* entry_widget = static_cast<EntryWidget*>(ui.tab_widget->currentWidget());
	if (entry_widget == nullptr)
	{
		QMessageBox::warning(this, "错误", "当前没有打开的词条！");
		return;
	}

	EntryArea* entry_area = entry_widget->getEntryArea();

	if (entry_area == nullptr)
	{
		QMessageBox::warning(this, "错误", "词条缺少编辑区域！非法词条！");
		return;
	}

	entry_area->appendBlock(BlockType::Header);
}
