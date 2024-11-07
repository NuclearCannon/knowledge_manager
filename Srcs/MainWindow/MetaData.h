#pragma once
#include <string>
#include <list>
#include <map>
#include <set>
#include <QString>
#include <QColor>
#include <vector>
#include <algorithm>
#include <QFile>
#include "../EntryWidget/pugixml/pugixml.hpp"

class Tag
{
private:
	int m_id;
	QColor m_color;
	QString m_name;
	Tag(int id, const QColor& color, const QString& name);
	~Tag();  // 是的，Tag的析构函数是私有的，这样可以避免除了MetaData以外的类和函数删除它
	friend class MetaData;  // MetaData是自己的友元，允许它修改私有成员
public:
	// 对于MetaData类以外的类，Tag都是只读的
	Tag(const Tag&) = delete;
	inline int id() const;
	inline const QColor& color() const;
	inline const QString& name() const;
};

class EntryMeta
{
private:
	int m_id;
	QString m_title;
	std::set<int> tagIds;
	std::set<int> in, out;// 指向
	EntryMeta(int id, const QString& title);
	~EntryMeta();
	friend class MetaData;

public:
	inline int id() const;

	inline const QString& title() const;

};


class MetaData
{
private:
	std::map<int,Tag*> tags;
	std::map<int,EntryMeta*> entrys;  // 词条集合
	std::set<int> anchors;  // 锚点
	std::set<int> empty_set;  // 用于返回空集
	int loadFromPugiDoc(pugi::xml_document& doc);
	void dumpToPugiDoc(pugi::xml_document& doc);
public:
	MetaData();
	~MetaData();
	// 加载方法，成功return 0
	int load(QFile&);
	// 保存方法
	void dump(QFile&);

	void clear();

	// 通过TagId获得Tag*
	const Tag* getTag(int tag_id) const;

	// 通过EntryId获得EntryMeta*
	const EntryMeta* getEntry(int entry_id) const;

	// 添加一个新的Tag，如果成功返回对应的TagId，
	// 如果失败（重名）return -1
	int addTag(const QColor& color, const QString& name);

	// 添加一个新的EntryMeta，如果成功返回对应的EntryId，
	// 如果失败（重名）return -1
	int addEntry(const QString& title);

	// 向词条的标签集中加入一个标签。
	// 如果目标已有标签，return -1
	// 如果标签不存在，return -2
	// 如果词条不存在，return -3
	// 一切正常，return 0
	int addTagToEntry(int tag_id, int entry_id);

	// 在全局层面上删除一个标签
	// 如果标签原本就不存在，return -1
	// 一切正常，return 0
	int removeTag(int tag_id);

	// 删除一个词条元数据
	// 如果词条原本就不存在，return -1
	// 一切正常，return 0
	int removeEntry(int entry_id);

	// 从词条的标签集中移除一个标签。
	// 如果目标没有该标签，return -1
	// 如果标签不存在，return -2
	// 如果词条不存在，return -3
	// 一切正常，return 0
	int removeTagFromEntry(int tag_id, int entry_id);

	inline bool hasTagId(int tagId);

	inline bool hasEntryId(int entry_id);

	// 修改一个Tag的名
	// 如果Tag本不存在，return -1
	// 如果修改导致重名，return -2
	// 一切正常（包括新名和原名一样），return 0
	int modifyTagName(int tag_id, const QString& new_name);


	// 修改一个标签的颜色
	// 如果Tag本不存在，return -1
	// 一切正常（包括新名和原名一样），return 0
	int modifyTagColor(int tag_id, const QColor& new_color);


	// 修改一个Tag的名和颜色
	// 如果Tag本不存在，return -1
	// 如果修改导致重名，return -2
	// 一切正常（包括新名和原名一样），return 0
	int modifyTag(int tag_id, const QString& new_name, const QColor& new_color);

	// 修改一个Entry的标题
	// 如果Entry本不存在，return -1
	// 如果修改导致重名，return -2
	// 一切正常（包括新名和原名一样），return 0
	int modifyEntryTitle(int entry_id, const QString& new_title);


	// 获得标签集（所有的）
	std::vector<const Tag*> getTags() const;

	// 获得词条集（所有的）
	std::vector<const EntryMeta*> getEntries() const;
	
	// 获得一个词条的标签集
	const std::set<int>& getTagsOfEntry(int entry_id) const;

	// 插入一条指向关系
	// from未找到：return -1
	// to未找到：return -2
	// 这条关系已经存在了：return -3
	// 一切正常：return 0
	int insertLinkRelationship(int from, int to);

	// 删除一条指向关系
	// from未找到：return -1
	// to未找到：return -2
	// 这条关系原本就不存在了：return -3
	// 一切正常：return 0
	int removeLinkRelationship(int from, int to);

	// 查找一条指向关系
	// from未找到：return -1
	// to未找到：return -2
	// 这条关系已经存在了：return 1
	// 这条关系不存在：return 0
	int hasLinkRelationship(int from, int to) const;

	// 查找一个Entry的所有指向的目的地。如果找不到这个entry, return NULL
	const std::set<int>* getOut(int entry_id) const;

	// 查找一个Entry的所有被指向的源头。如果找不到这个entry, return NULL
	const std::set<int>* getIn(int entry_id) const;

	// 添加锚点
	// 锚点已存在：return -1
	// 词条不存在：return -2
	// 一切正常：return 0
	int addAnchor(int entry_id);

	// 移除锚点
	// 锚点不存在：return -1
	// 词条不存在：return -2
	// 一切正常：return 0
	int removeAnchor(int entry_id);

	// 查询锚点
	// 锚点存在：return 1
	// 锚点不存在：return 0
	// 词条不存在：return -1
	int hasAnchor(int entry_id);
	// 取锚点集合
	const std::set<int>& getAnchors() const;
};

