#ifndef TAG_EDIT_DIALOG_H
#define TAG_EDIT_DIALOG_H

#include <QDialog>

#include "ui_tag_edit_dialog.h"

namespace Ui {
class TagEditDialog;
}

class TagEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagEditDialog(QWidget *parent = nullptr);
    ~TagEditDialog();
    void setName(const QString& name)
    {
        ui->name_line_edit->setText(name);
    }

    void setColor(const QString& color)
    {
        ui->color_line_edit->setText(color);
		ui->color_widget->setStyleSheet("background-color: " + color);
    }

    QString getName() const
    {
        return ui->name_line_edit->text();
    }

    QString getColor() const
    {
        return ui->color_line_edit->text();
    }

public slots:
	void colorButtonClicked();
	void colorLineEditChanged(const QString& text);

private:
    Ui::TagEditDialog *ui;
};

#endif // TAG_EDIT_DIALOG_H
