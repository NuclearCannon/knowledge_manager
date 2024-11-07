#include <QMessageBox>

#include "../MainWindow/KMMainWindow.h"
#include "../EntryWidget/EntryWidget.h"
#include "../Search/search_entry_widget.h"

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

// 槽：添加一条指出
void KMMainWindow::addPointOut()
{
	int from, to;
	auto current = getCurrentEntryWidget();
	if (!current)
	{
		QMessageBox::warning(this, "Warning", QStringLiteral("当前没有打开词条，不能添加指出关联。"));
		return;
	}
	from = current->getEntryId();
	to = getIdBySearchEntryDialog(meta_data);
	if (to < 0)
	{
		qWarning() << "KMMainWindow::addPointOut(): to<0";
		return;
	}
	// 添加一条关联
	int rtn = meta_data.insertLinkRelationship(from, to);
	if (rtn == -3)
	{
		QMessageBox::warning(this, "Warning", QStringLiteral("这条关联已经存在！"));
		return;
	}

	// 如果当前处于“关联”页，还要刷新
	if (ui.left_tab_widget->currentIndex() == 1)
	{
		relatedEntriedButtonClicked();
	}

	handleKLChanged();
}