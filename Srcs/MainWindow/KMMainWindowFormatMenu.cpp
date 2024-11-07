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

}
void KMMainWindow::actStrike()
{

}
void KMMainWindow::actUnderline()
{

}
void KMMainWindow::actSetTypeCode()
{

}
void KMMainWindow::actSetTypeLink()
{

}
void KMMainWindow::actSetTypeNormal()
{

}