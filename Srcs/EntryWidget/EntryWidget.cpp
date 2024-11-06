#include <QVBoxLayout>
#include <QTextEdit>

#include "EntryWidget.h"

EntryWidget* EntryWidget::construct(int _entry_id, QWidget* parent, KMMainWindow* _main_window)
{
	const MetaData& _meta_data = _main_window->getMetaData();
	if (parent == nullptr || _main_window == nullptr || _entry_id < 0 || _meta_data.getEntry(_entry_id) == nullptr)
	{
		return nullptr;
	}

	QString entry_path = _main_window->getTempKLPath() + '/' + QString::number(_entry_id);
	QDir entry_dir(entry_path);
	bool new_entry_dir = false;
	if (!entry_dir.exists())
	{
		if (!entry_dir.mkdir(entry_path))
		{
			return nullptr;
		}
		new_entry_dir = true;
	}

	EntryWidget* entry_widget = new EntryWidget(_entry_id, parent, _main_window);
	if (new_entry_dir)
	{
		entry_widget->entry_area = EntryArea::initialize(entry_widget, entry_dir);
	}
	else
	{
		entry_widget->entry_area = EntryArea::open(entry_widget, entry_dir);
	}

	if (entry_widget->entry_area == nullptr)
	{
		delete entry_widget;
		return nullptr;
	}

	connect(entry_widget->entry_area, &EntryArea::contentChange, entry_widget, &EntryWidget::handleContentChange);

	// 设置布局
	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(entry_widget->entry_area);
	layout->setContentsMargins(0, 0, 0, 0);
	entry_widget->setLayout(layout);


	return entry_widget;
}

EntryWidget::EntryWidget(int entry_id, QWidget* parent, KMMainWindow* main_window)
	: QWidget(parent), entry_id(entry_id), is_saved(true), entry_area(nullptr), main_window(main_window)
{

}

EntryWidget::~EntryWidget()
{
	if (entry_area != nullptr)
	{
		delete entry_area;
		entry_area = nullptr;
	}
}

int EntryWidget::getEntryId() const
{
	return entry_id;
}

void EntryWidget::handleContentChange()
{
	//qDebug() << "EntryWidget::handleContentChange()";
	is_saved = false;
}

bool EntryWidget::saveEntry()
{
	if (is_saved)
	{
		return true;
	}

	if (entry_area == nullptr)
	{
		return false;
	}

	try {
		entry_area->dump();
	}
	catch (...)
	{
		return false;
	}
	
	is_saved = true;
	return true;
}

