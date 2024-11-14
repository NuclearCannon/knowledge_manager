#include "MetaData.h"
#include <cassert>
#include <queue>
#include <QTextStream>
#include <sstream>
#include <set>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

Tag::Tag(int id, const QColor& color, const QString& name) :
	m_id(id),
	m_color(color),
	m_name(name)
{

};

Tag::~Tag() = default;

inline int Tag::id() const
{
	return m_id;
}

inline const QColor& Tag::color() const
{
	return m_color;
}

inline const QString& Tag::name() const
{
	return m_name;
}


EntryMeta::EntryMeta(int id, const QString& title) : m_id(id), m_title(title) {};
EntryMeta::~EntryMeta() = default;

inline int EntryMeta::id() const
{
	return m_id;
}

inline const QString& EntryMeta::title() const
{
	return m_title;
}

const std::set<int>& EntryMeta::getTags() const
{
	return tagIds;
}

MetaData::MetaData() = default;
MetaData::~MetaData()
{
	clear();
}


void MetaData::clear()
{
	for (const auto& tag_pair : tags)delete tag_pair.second;
	tags.clear();
	for (const auto& entry_pair : entrys)delete entry_pair.second;
	entrys.clear();
}

const Tag* MetaData::getTag(int tag_id) const
{
	auto iter = tags.find(tag_id);
	if (iter == tags.end())return nullptr;
	return iter->second;
}

const EntryMeta* MetaData::getEntry(int entry_id) const
{
	auto iter = entrys.find(entry_id);
	if (iter == entrys.end())return nullptr;
	return iter->second;
}

int MetaData::addTag(const QColor& color, const QString& name)
{
	for (const auto& pair : tags)
	{
		if (pair.second->name() == name)
		{
			return -1;
		}
	}
	// else: 无重名
	// 分配一个新的tag_id
	int new_id = 0;
	while (tags.find(new_id) != tags.end())new_id++;
	tags[new_id] = new Tag(new_id, color, name);
	return new_id;
}

int MetaData::addEntry(const QString& title)
{
	for (const auto& pair : entrys)
	{
		if (pair.second->title() == title)return -1;
	}
	// else: 无重名
	// 分配一个新的entry_id
	int new_id = 0;
	while (entrys.find(new_id) != entrys.end())new_id++;
	entrys[new_id] = new EntryMeta(new_id, title);
	return new_id;
}

int MetaData::addTagToEntry(int tag_id, int entry_id)
{
	if (!hasTagId(tag_id))return -2;
	auto dest_iter = entrys.find(entry_id);
	if (dest_iter == entrys.end())return -3;
	EntryMeta* dest = dest_iter->second;
	if (dest->tagIds.find(tag_id) != dest->tagIds.end())return -1;
	dest->tagIds.insert(tag_id);
	return 0;
}

int MetaData::removeTag(int tag_id)
{
	auto dest_iter = tags.find(tag_id);
	if (dest_iter == tags.end())return -1;

	Tag* dest_ptr = dest_iter->second;// 已经找到这个标签
	// 从每个词条中删除它
	for (const auto& pair : entrys)
	{
		std::set<int>& entry_tag_id_set = pair.second->tagIds;
		if (entry_tag_id_set.find(tag_id) != entry_tag_id_set.end())entry_tag_id_set.erase(tag_id);
	}
	// 从全局上删除它
	delete dest_ptr;
	tags.erase(tag_id);

	return 0;
}

int MetaData::removeEntry(int entry_id)
{
	auto dest_iter = entrys.find(entry_id);
	if (dest_iter == entrys.end())return -1;
	delete dest_iter->second;
	entrys.erase(entry_id);
	return 0;
}

int MetaData::removeTagFromEntry(int tag_id, int entry_id)
{
	if (!hasTagId(tag_id))return -2;
	auto dest_iter = entrys.find(entry_id);
	if (dest_iter == entrys.end())return -3;
	EntryMeta* dest = dest_iter->second;
	if (dest->tagIds.find(tag_id) == dest->tagIds.end())return -1;
	dest->tagIds.erase(tag_id);
	return 0;
}

inline bool MetaData::hasTagId(int tagId)
{
	return tags.find(tagId) != tags.end();
}

inline bool MetaData::hasEntryId(int entry_id)
{
	return entrys.find(entry_id) != entrys.end();
}

int MetaData::modifyTagName(int tag_id, const QString& new_name)
{
	std::map<int, Tag*>::iterator dest_iter;
	bool found = false;
	for (auto iter = tags.begin(); iter != tags.end(); iter++)
	{
		if (iter->second->id() == tag_id)
		{
			dest_iter = iter;
			found = true;
		}
		else if (iter->second->name() == new_name)
		{
			return -2;// 重名
		}
	}
	if (!found)return -1;
	dest_iter->second->m_name = new_name;
	return 0;
}

int MetaData::modifyTagColor(int tag_id, const QColor& new_color)
{
	std::map<int, Tag*>::iterator dest_iter;
	for (auto iter = tags.begin(); iter != tags.end(); iter++)
	{
		if (iter->second->id() == tag_id)
		{
			iter->second->m_color = new_color;
			return 0;
		}
	}
	return -1;
}

int MetaData::modifyTag(int tag_id, const QString& new_name, const QColor& new_color)
{
	std::map<int, Tag*>::iterator dest_iter;
	bool found = false;
	for (auto iter = tags.begin(); iter != tags.end(); iter++)
	{
		if (iter->second->id() == tag_id)
		{
			dest_iter = iter;
			found = true;
		}
		else if (iter->second->name() == new_name)
		{
			return -2;// 重名
		}
	}
	if (!found)return -1;
	dest_iter->second->m_name = new_name;
	dest_iter->second->m_color = new_color;
	return 0;
}

int MetaData::modifyEntryTitle(int entry_id, const QString& new_title)
{
	std::map<int, EntryMeta*>::iterator dest_iter;
	bool found = false;
	for (auto iter = entrys.begin(); iter != entrys.end(); iter++)
	{
		if (iter->second->id() == entry_id)
		{
			dest_iter = iter;
			found = true;
		}
		else if (iter->second->title() == new_title)
		{
			return -2;// 重名
		}
	}
	if (!found)return -1;
	dest_iter->second->m_title = new_title;
	return 0;
}

std::vector<const Tag*> MetaData::getTags() const
{
	std::vector<const Tag*> result;
	for (const auto& pair : tags)
	{
		result.push_back(pair.second);
	}
	return result;
}

std::vector<const EntryMeta*> MetaData::getEntries() const
{
	std::vector<const EntryMeta*> result;
	for (const auto& pair : entrys)
	{
		result.push_back(pair.second);
	}


	return result;
}



int MetaData::insertLinkRelationship(int from, int to)
{
	if (from == to)return -4;
	auto from_iter = entrys.find(from);
	if (from_iter == entrys.end())return -1;
	auto to_iter = entrys.find(to);
	if (to_iter == entrys.end())return -2;
	auto& out_set = from_iter->second->out;
	if (out_set.find(to) != out_set.end())return -3;
	out_set.insert(to);
	to_iter->second->in.insert(from);
	return 0;
}

int MetaData::removeLinkRelationship(int from, int to)
{
	auto from_iter = entrys.find(from);
	if (from_iter == entrys.end())return -1;
	auto to_iter = entrys.find(to);
	if (to_iter == entrys.end())return -2;

	auto& out_set = from_iter->second->out;
	if (out_set.find(to) == out_set.end())return -3;
	out_set.erase(to);
	to_iter->second->in.erase(from);
	return 0;
}

int MetaData::hasLinkRelationship(int from, int to) const
{
	auto from_iter = entrys.find(from);
	if (from_iter == entrys.end())return -1;
	auto to_iter = entrys.find(to);
	if (to_iter == entrys.end())return -2;
	auto& out_set = from_iter->second->out;
	if (out_set.find(to) != out_set.end())return 1;
	return 0;
}

const std::set<int>* MetaData::getOut(int entry_id) const
{
	auto entry_iter = entrys.find(entry_id);
	if (entry_iter == entrys.end())return NULL;
	return &(entry_iter->second->out);
}

const std::set<int>* MetaData::getIn(int entry_id) const
{
	auto entry_iter = entrys.find(entry_id);
	if (entry_iter == entrys.end())return NULL;
	return &(entry_iter->second->in);
}


int MetaData::addAnchor(int entry_id)
{
	if (!hasEntryId(entry_id))return -2;
	if (anchors.find(entry_id) != anchors.end())return -1;
	anchors.insert(entry_id);
	return 0;
}

int MetaData::removeAnchor(int entry_id)
{
	if (!hasEntryId(entry_id))return -2;
	if (anchors.find(entry_id) == anchors.end())return -1;
	anchors.erase(entry_id);
	return 0;
}


int MetaData::hasAnchor(int entry_id)
{
	if (!hasEntryId(entry_id))return -1;
	if (anchors.find(entry_id) != anchors.end())return 1;
	return 0;
}


const std::set<int>& MetaData::getAnchors() const
{
	return anchors;
}



int MetaData::loadQtXml(QFile& file)
{
	QDomDocument doc;
	doc.setContent(file.readAll());

	QDomElement meta = doc.documentElement();


	clear();

	// 解析tag-set

	QDomElement tag_set_node = meta.firstChildElement("tag-set");
	
	for (auto tag = tag_set_node.firstChildElement("tag"); !tag.isNull(); tag = tag.nextSiblingElement("tag"))
	{
		int id = tag.attribute("id").toInt();
		QColor color(tag.attribute("color"));
		tags[id] = new Tag(id, color, tag.attribute("text"));
	}


	// 解析entrys
	std::queue<std::pair<int, int>> links_queue;


	QDomElement entrys_node = meta.firstChildElement("entrys");
	for (QDomElement entry = entrys_node.firstChildElement("entry"); !entry.isNull(); entry = entry.nextSiblingElement("entry"))
	{

		int id = entry.attribute("id").toInt();

		EntryMeta* entry_meta = new EntryMeta(id, entry.attribute("title"));
		// 解析词条集合
		QDomElement tags = entry.firstChildElement("tags");
		for (QDomElement tag = tags.firstChildElement("tag-usage"); !tag.isNull(); tag = tag.nextSiblingElement("tag-usage"))
		{
			int tag_id = tag.attribute("id").toInt();
			entry_meta->tagIds.insert(tag_id);
		}
		entrys[id] = entry_meta;
		// 解析link
		QDomElement links = entry.firstChildElement("links");
		for (QDomElement link = links.firstChildElement("link"); !link.isNull(); link = link.nextSiblingElement("link"))
		{
			links_queue.push({ id, link.attribute("dest").toInt() });
		}
	}

	// 解析anchors
	QDomElement anchors_node = meta.firstChildElement("anchors");
	for (QDomElement anchor = anchors_node.firstChildElement("anchor"); !anchor.isNull(); anchor = anchor.nextSiblingElement("anchor"))
	{
		anchors.insert(anchor.attribute("id").toInt());
	}

	// 将队列中的链接关系反映到对象
	while (!links_queue.empty())
	{
		auto link = links_queue.front();
		links_queue.pop();
		int from = link.first, to = link.second;
		entrys[from]->out.insert(to);
		entrys[to]->in.insert(from);
	}
	return 0;
}
void MetaData::dumpQtXml(QFile& file)
{
	QDomDocument doc;

	QDomElement meta = doc.createElement("meta");
	doc.appendChild(meta);


	QDomElement tag_set_node = doc.createElement("tag-set");
	meta.appendChild(tag_set_node);
	QDomElement entrys_node = doc.createElement("entrys");
	meta.appendChild(entrys_node);
	QDomElement anchors_node = doc.createElement("anchors");
	meta.appendChild(anchors_node);



	for (const auto& tag_pair : tags)
	{
		Tag* tag_ptr = tag_pair.second;
		QDomElement tag = doc.createElement("tag");
		tag_set_node.appendChild(tag);
		tag.setAttribute("id", tag_ptr->id());
		tag.setAttribute("color", tag_ptr->color().name());
		tag.setAttribute("text", tag_ptr->name());
	}



	for (const auto& entry_pair : entrys)
	{
		EntryMeta* entry_ptr = entry_pair.second;
		QDomElement entry = doc.createElement("entry");
		entrys_node.appendChild(entry);
		entry.setAttribute("id", entry_ptr->id());
		entry.setAttribute("title", entry_ptr->title());

		QDomElement tags = doc.createElement("tags"); 
		entry.appendChild(tags);
		QDomElement links = doc.createElement("links"); 
		entry.appendChild(links);

		for (int tag_id : entry_ptr->tagIds)
		{
			QDomElement usage = doc.createElement("tag-usage");
			tags.appendChild(usage);
			usage.setAttribute("id", tag_id);
		}

		for (int dest : entry_ptr->out)
		{
			QDomElement link = doc.createElement("link");
			links.appendChild(link);
			link.setAttribute("dest", dest);
		}
	}
	for (int anchor_id : anchors)
	{
		QDomElement anchor = doc.createElement("anchor");
		anchors_node.appendChild(anchor);
		anchor.setAttribute("id", anchor_id);

	}
	QTextStream stream(&file);
	stream << doc.toString();
}