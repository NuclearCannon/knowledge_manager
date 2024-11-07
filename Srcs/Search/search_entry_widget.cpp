#include <QDir>
#include <QMessageBox>
#include <QButtonGroup>
#include <qoverload.h>
#include <QDebug>
#include "../EntryWidget/EntryWidget.h"
#include "search_entry_widget.h"
#include "ui_search_entry_widget.h"

int getIdBySearchEntryDialog(const MetaData& meta)
{
    search dialog(0, meta);
    dialog.exec();
    return dialog.getResult();
}


search::search(QWidget* parent, const MetaData& meta):
    QDialog(parent),
    entrys_list_widget(),
    button_group(new QButtonGroup(this)),  // 初始化 QButtonGroup
    ui(new Ui::search),
    result(-1),
    meta(meta)
{
    ui->setupUi(this);
    this->setWindowTitle("搜索词条");
    ui->check_box_title->setChecked(true);
    // 初始化 QButtonGroup
    button_group->addButton(ui->check_box_title, 0);
    button_group->addButton(ui->check_box_tag, 1);
    //connect(ui->pushButton, &QPushButton::clicked, this, &search::open_go_back);
    connect(ui->search_text, &QLineEdit::textChanged, this, &search::onSearchTextChanged);//文本框内容改变
    // 连接复选框的状态变化信号到槽函数
    connect(button_group, &QButtonGroup::idClicked, this, &search::onCheckBoxStateChanged);//选择框内容改变
}

search::~search()
{
    //delete ui;
    //ui = nullptr;
}


void search::putAllEntries()
{
    ui->list_widget->clear();
    for (auto entry : meta.getEntries())
    {
        ui->list_widget->addItem(new SeachEntryItem(entry->id(), entry->title()));
    }
}

void search::putEntriesByTitle(const QString& target)
{
    ui->list_widget->clear();
    for (auto entry : meta.getEntries())
    {
        if (entry->title().contains(target))
        {
            ui->list_widget->addItem(new SeachEntryItem(entry->id(), entry->title()));
        }
    }
}

void search::putEntriesByTags(const QString& tag_str)
{
    auto list = tag_str.split(' ');  // 按空格切分
    std::set<int> tag_set;
    for (auto tag : meta.getTags())
    {
        for (const auto& tag_name : list)
        {
            if (tag->name() == tag_name)
            {
                tag_set.insert(tag->id());
                break;
            }
        }
    }
    putEntriesByTags(tag_set);
}
void search::putEntriesByTags(const std::set<int>& tag_set)
{
    ui->list_widget->clear();
    for (auto entry : meta.getEntries())
    {
        // 待实现
        qWarning() << "search::putEntriesByTags待实现";
    }
}

void search::onSearchTextChanged()
{      
    if (ui->search_text->text().isEmpty()) {
        putAllEntries();
    }
    else if (ui->check_box_title->isChecked()) {
        putEntriesByTitle(ui->search_text->text());
    }
    else  if (ui->check_box_tag->isChecked()) {
        putEntriesByTags(ui->search_text->text());
    }
}

void search::onCheckBoxStateChanged(int id)
{
    // 确保只有一个复选框被选中
    for (int i = 0; i < button_group->buttons().size(); ++i) {
        if (i != id) {
            button_group->button(i)->setChecked(false);
        }
    }
    onSearchTextChanged();
}


void search::onListItemClicked(QListWidgetItem* item) {
    SeachEntryItem* entry_item = dynamic_cast<SeachEntryItem*>(item);
    if (entry_item == nullptr)
    {
        result = -1;  // ERROR
        reject();
    }
    else
    {
        result = entry_item->getId();
        accept();
    }
}




int search::getResult() const
{
    return result;
}