#include <QColorDialog>

#include "tag_edit_dialog.h"
#include "ui_tag_edit_dialog.h"

TagEditDialog::TagEditDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TagEditDialog)
{
    ui->setupUi(this);

	connect(ui->color_button, &QPushButton::clicked, this, &TagEditDialog::colorButtonClicked);
	// 动态设置color_line_edit的样式表
	connect(ui->color_line_edit, &QLineEdit::textChanged, this, &TagEditDialog::colorLineEditChanged);

	// 连接按钮的信号到对话框的槽
	connect(ui->ok_button, &QPushButton::clicked, this, &QDialog::accept);
	connect(ui->cancel_button, &QPushButton::clicked, this, &QDialog::reject);
}

TagEditDialog::~TagEditDialog()
{
    delete ui;
}

// 槽：color_line_edit的文本改变时，动态设置color_line_edit的样式表
void TagEditDialog::colorLineEditChanged(const QString& text)
{
	// 检查color合法性，合法则改变颜色预览
	QColor color(text);
	if (color.isValid())
	{
		ui->color_widget->setStyleSheet("background-color: " + color.name());
	}
}

// 槽：点击颜色选择器
void TagEditDialog::colorButtonClicked()
{
	QColor color = QColorDialog::getColor(Qt::white, this);
	if (color.isValid())
	{
		ui->color_widget->setStyleSheet("background-color: " + color.name());
		// 填上color_line_edit为十六进制颜色值
		ui->color_line_edit->setText(color.name());
	}
}

