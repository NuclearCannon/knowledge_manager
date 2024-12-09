#include "KMMainWindow.h"
#include "../EntryWidget/BlockWidget/BlockWidgets.h"

void KMMainWindow::actBold()
{
	TextBlockEdit* browser = qobject_cast<TextBlockEdit*>(focusWidget());
	if (!browser)return;
	browser->setStyleOnSelection(FormatItem::bold, true);
}
void KMMainWindow::actItalic()
{
	TextBlockEdit* browser = qobject_cast<TextBlockEdit*>(focusWidget());
	if (!browser)return;
	browser->setStyleOnSelection(FormatItem::italic, true);
}
void KMMainWindow::actStrike()
{
	TextBlockEdit* browser = qobject_cast<TextBlockEdit*>(focusWidget());
	if (!browser)return;
	browser->setStyleOnSelection(FormatItem::strike, true);
}
void KMMainWindow::actUnderline()
{
	TextBlockEdit* browser = qobject_cast<TextBlockEdit*>(focusWidget());
	if (!browser)return;
	browser->setStyleOnSelection(FormatItem::underline, true);
}
void KMMainWindow::actRemoveBold()
{
	TextBlockEdit* browser = qobject_cast<TextBlockEdit*>(focusWidget());
	if (!browser)return;
	browser->setStyleOnSelection(FormatItem::bold, false);
}
void KMMainWindow::actRemoveItalic()
{
	TextBlockEdit* browser = qobject_cast<TextBlockEdit*>(focusWidget());
	if (!browser)return;
	browser->setStyleOnSelection(FormatItem::italic, false);
}
void KMMainWindow::actRemoveStrike()
{
	TextBlockEdit* browser = qobject_cast<TextBlockEdit*>(focusWidget());
	if (!browser)return;
	browser->setStyleOnSelection(FormatItem::strike, false);
}
void KMMainWindow::actRemoveUnderline()
{
	TextBlockEdit* browser = qobject_cast<TextBlockEdit*>(focusWidget());
	if (!browser)return;
	browser->setStyleOnSelection(FormatItem::underline, false);
}
void KMMainWindow::actSetTypeCode()
{
	TextBlockEdit* browser = qobject_cast<TextBlockEdit*>(focusWidget());
	if (!browser)return;
	browser->setTypeOnSelection(TextType::code);
}
void KMMainWindow::actSetTypeLink()
{
	TextBlockEdit* browser = qobject_cast<TextBlockEdit*>(focusWidget());
	if (!browser)return;
	browser->setTypeOnSelection(TextType::link);
}
void KMMainWindow::actSetTypeNormal()
{
	TextBlockEdit* browser = qobject_cast<TextBlockEdit*>(focusWidget());
	if (!browser)return;
	browser->setTypeOnSelection(TextType::normal);
}