#include <QDir>
#include <QMessageBox>
#include <QButtonGroup>
#include <qoverload.h>
#include <QDebug>
#include "../EntryWidget/EntryWidget.h"
#include "search_entry_widget.h"
#include "../MainWindow/MetaData.h"

int getIdBySearchEntryDialog(const MetaData& meta)
{
    search dialog(0, meta);
    dialog.exec();
    return dialog.getResult();
}

static bool isSubset(const std::set<int> sub_set, const std::set<int> super_set)
{
    for (int i : sub_set)
    {
        auto iter = super_set.lower_bound(i);
        if (iter == super_set.end() || *iter != i)
        {
            return false;
        }
    }
    return true;
}



SeachEntryItem::SeachEntryItem(int id, const QString& title) :
    id(id),
    title(title),
    QListWidgetItem(title)
{

}

SeachEntryItem::~SeachEntryItem() = default;

int SeachEntryItem::getId() const
{
    return id;
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
    connect(ui->search_text, &QLineEdit::textChanged, this, &search::searchTextChanged);//文本框内容改变
    // 连接复选框的状态变化信号到槽函数
    connect(button_group, &QButtonGroup::idClicked, this, &search::checkBoxStateChanged);//选择框内容改变
    connect(ui->list_widget, &QListWidget::itemClicked, this, &search::listItemClicked);
    putAllEntries();
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
    for (const Tag *tag : meta.getTags())
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
        // 判断目标集合是不是entry的集合的子集
        qDebug() << entry->id() << entry->title();
        if (isSubset(tag_set, entry->getTags()))
        {
            ui->list_widget->addItem(new SeachEntryItem(entry->id(), entry->title()));
        }
    }
}

void search::searchTextChanged()
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

void search::checkBoxStateChanged(int id)
{
    // 确保只有一个复选框被选中
    for (int i = 0; i < button_group->buttons().size(); ++i) {
        if (i != id) {
            button_group->button(i)->setChecked(false);
        }
    }
    searchTextChanged();
}


void search::listItemClicked(QListWidgetItem* item) {
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

void search::addItem(int id, const QString& title)
{
    SeachEntryItem* item = new SeachEntryItem(id, title);
    ui->list_widget->addItem(item);
    

}