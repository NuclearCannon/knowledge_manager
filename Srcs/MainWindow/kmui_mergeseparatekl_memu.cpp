#include "../Search/merge_kl.h"
#include "../MainWindow/KMMainWindow.h"
#include "../Search/search_kl_widget.h"
#include "../Search/separate_kl.h"
#include "ui_merge_kl.h"

void KMMainWindow::actopenmergekl() {
	merge_kl* new_merge_kl = new merge_kl(this);
	new_merge_kl->setWindowFlags(Qt::Window); // 确保它作为一个独立的窗口显示
	new_merge_kl->show();
};
void KMMainWindow::actopenseparatekl() {
	search_kl_widget* search_kl = new search_kl_widget(this, kl_name,1);
	search_kl->setWindowTitle("分库");
	search_kl->setWindowFlags(Qt::Window); // 确保它作为一个独立的窗口显示
	search_kl->show();
};

