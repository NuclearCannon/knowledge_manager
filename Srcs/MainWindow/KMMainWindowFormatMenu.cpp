#include "KMMainWindow.h"
#include "../EntryWidget/BlockWidget/BlockWidgets.h"

void KMMainWindow::actBold()
{
	TextBlockBrowser* browser = qobject_cast<TextBlockBrowser*>(focusWidget());
	if (!browser)return;
	browser->setStyleOnSelection(FormatItem::bold, true);
}
void KMMainWindow::actItalic()
{
	TextBlockBrowser* browser = qobject_cast<TextBlockBrowser*>(focusWidget());
	if (!browser)return;
	browser->setStyleOnSelection(FormatItem::italic, true);
}
void KMMainWindow::actStrike()
{
	TextBlockBrowser* browser = qobject_cast<TextBlockBrowser*>(focusWidget());
	if (!browser)return;
	browser->setStyleOnSelection(FormatItem::strike, true);
}
void KMMainWindow::actUnderline()
{
	TextBlockBrowser* browser = qobject_cast<TextBlockBrowser*>(focusWidget());
	if (!browser)return;
	browser->setStyleOnSelection(FormatItem::underline, true);
}
void KMMainWindow::actRemoveBold()
{
	TextBlockBrowser* browser = qobject_cast<TextBlockBrowser*>(focusWidget());
	if (!browser)return;
	browser->setStyleOnSelection(FormatItem::bold, false);
}
void KMMainWindow::actRemoveItalic()
{
	TextBlockBrowser* browser = qobject_cast<TextBlockBrowser*>(focusWidget());
	if (!browser)return;
	browser->setStyleOnSelection(FormatItem::italic, false);
}
void KMMainWindow::actRemoveStrike()
{
	TextBlockBrowser* browser = qobject_cast<TextBlockBrowser*>(focusWidget());
	if (!browser)return;
	browser->setStyleOnSelection(FormatItem::strike, false);
}
void KMMainWindow::actRemoveUnderline()
{
	TextBlockBrowser* browser = qobject_cast<TextBlockBrowser*>(focusWidget());
	if (!browser)return;
	browser->setStyleOnSelection(FormatItem::underline, false);
}
void KMMainWindow::actSetTypeCode()
{
	TextBlockBrowser* browser = qobject_cast<TextBlockBrowser*>(focusWidget());
	if (!browser)return;
	browser->setTypeOnSelection(TextType::code);
}
void KMMainWindow::actSetTypeLink()
{
	TextBlockBrowser* browser = qobject_cast<TextBlockBrowser*>(focusWidget());
	if (!browser)return;
	browser->setTypeOnSelection(TextType::link);
}
void KMMainWindow::actSetTypeNormal()
{
	TextBlockBrowser* browser = qobject_cast<TextBlockBrowser*>(focusWidget());
	if (!browser)return;
	browser->setTypeOnSelection(TextType::normal);
}