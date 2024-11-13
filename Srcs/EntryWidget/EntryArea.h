#pragma once
#include <QScrollArea>
#include <QBoxLayout>
#include "BlockWidget/BlockWidgets.h"
#include <list>
#include <QDir>
#include <QPixmap>
#include <QFileDialog>
#include <QSet>
#include <QFile>
class EntryArea : public QScrollArea
{
    Q_OBJECT
private:
    QVBoxLayout* layout;
    BlockWidget* head, * tail;
    QDir root_dir, attachment_dir;
    QSet<BlockWidget*> blocks;
    BlockWidget* focus;
    bool saved;
    // 构造函数私有：不允许手动构造，必须使用open或者initialize构造
    EntryArea(QWidget* parent, const QDir& root);
    int insert(int index, BlockWidget* ptr);// 在下标处插入一个块
    int find(BlockWidget* block_widget);  // 定位一个块，返回其下标
    
    QFile entry_file;
    int load();
    int dump();  // 将当前内容同步到文件

public:
    
    int loadQtXml();
    int dumpQtXml();

    ~EntryArea();
    struct OutlineItem
    {
        QString text;
        int level;
        int position;

        OutlineItem(QString text, int level, int position);
    };
    std::list<OutlineItem> getOutline() const;
    
    bool has(BlockWidget* block) const;  // 检查是否有该块
    int rollTo(int position);// 滚动到指定位置，如果滚动失败返回0，滚动成功返回1
    int length() const;  // 返回块列表的长度
    BlockWidget* getWidgetByIndex(int index);// 根据下标取一个块指针，如果越界返回nullptr
    BlockWidget* newBlockWidgetByType(BlockType type);
    void insertBlockAbove(BlockWidget* block_widget, BlockType type);
    void insertBlockAbove(BlockWidget* block_widget, BlockWidget* new_block);
    int insertBlockAbove(BlockType type);
    void insertBlockBelow(BlockWidget* block_widget, BlockType type);
    void insertBlockBelow(BlockWidget* block_widget, BlockWidget* new_block);
    int insertBlockBelow(BlockType type);
    void appendBlock(BlockType type);
    void appendBlock(BlockWidget* widget);
    int deleteCurrentBlock();
    static EntryArea* open(QWidget* parent, const QDir& dir);
    static EntryArea* initialize(QWidget* parent, const QDir& dir);
    QDir getRootDir() const;  
    // 响应文本块要求
    void emitLinkClicked(const QUrl& url);
signals:

    void contentChange();
signals:
    void titleChange();
signals:
    void linkClicked(QUrl url);
    
public slots:
    void contentChangeSlot();
    void titleChangeSlot();

    
};