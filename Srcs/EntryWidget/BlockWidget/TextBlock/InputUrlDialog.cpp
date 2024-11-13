#include "TextBlockWidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

InputUrlDialog::InputUrlDialog(QWidget* parent, const QString& text) : QDialog(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* label = new QLabel(text, this);
    lineEdit = new QLineEdit(this);

    layout->addWidget(label);
    layout->addWidget(lineEdit);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &InputUrlDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &InputUrlDialog::reject);

    layout->addWidget(buttonBox);
}


bool InputUrlDialog::getStringByDialog(const QString& text, QString& dest)
{
    InputUrlDialog dialog(0, text);
    if (dialog.exec() == QDialog::Accepted)
    {
        dest = dialog.lineEdit->text();
        return true;
    }
    else
    {
        return false;
    }
}


void InputUrlDialog::accept()
{
    resultAccepted = true;
    QDialog::accept();
}

void InputUrlDialog::reject()
{
    resultAccepted = false;
    QDialog::reject();
}
