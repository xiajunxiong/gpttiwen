#include "noticeconfig.hpp"
#include "servercommon/serverdef.h"

NoticeConfig::NoticeConfig()
{

}

NoticeConfig::~NoticeConfig()
{

}

NoticeConfig *g_notice_config = NULL;
NoticeConfig* NoticeConfig::Instance()
{
	if (NULL == g_notice_config)
	{
		g_notice_config = new NoticeConfig();
	}
	return g_notice_config;
}

bool NoticeConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("notice", InitNoticeCfg);

	return true;
}

bool NoticeConfig::Reload(const std::string &configname, std::string *err)
{
	NoticeConfig *temp = new NoticeConfig();

	if (!temp->Init(configname, err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != g_notice_config)
	{
		temp->GetOldConfig(g_notice_config);
		delete g_notice_config;
	}

	g_notice_config = temp;

	return true;
}

void NoticeConfig::GetOldConfig(const NoticeConfig* nc)
{
	m_old_cfg_map = nc->m_cfg_map;
}

std::vector<NoticeCfg*> NoticeConfig::GetChangedNotice()
{
	std::vector<NoticeCfg*> changed_list;

	NoticeCfgMap::iterator it = m_cfg_map.begin();
	for (; it != m_cfg_map.end(); ++it)
	{
		bool is_changed = false;
		int notice_id = it->second.notice_id;
		NoticeCfgMap::iterator it2 = m_old_cfg_map.find(notice_id);
		if (it2 == m_old_cfg_map.end())
		{
			is_changed = true;
		}
		else if (!this->IsSameCfg(&it->second, &it2->second))
		{
			is_changed = true;
		}

		if (is_changed)
		{
			changed_list.push_back(&it->second);
		}
	}

	return changed_list;
}

int NoticeConfig::InitNoticeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	m_old_cfg_map = m_cfg_map;
	m_cfg_map.clear();

	while (dataElement)
	{
		int id = 0;
		if (!GetSubNodeValue(dataElement, "id", id) || m_cfg_map.find(id) != m_cfg_map.end())
		{
			return -1;
		}

		NoticeCfg cfg;
		cfg.notice_id = id;

		if (!GetSubNodeValue(dataElement, "title", cfg.title) || cfg.title.length() >= sizeof(NoticeTitle))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "subject", cfg.subject) || cfg.subject.length() >= sizeof(NoticeSubject))
		{
			return -22;
		}

		if (!GetSubNodeValue(dataElement, "text", cfg.content) || cfg.content.length() >= sizeof(NoticeContent))
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "version", cfg.version) || cfg.version < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "is_show", cfg.is_show) || (cfg.is_show != 0 && cfg.is_show != 1))
		{
			return -5;
		}

		m_cfg_map[id] = cfg;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

bool NoticeConfig::IsSameCfg(NoticeCfg* a, NoticeCfg* b)
{
	if (NULL == a || NULL == b) return false;

	if (a->notice_id != b->notice_id) return false;

	if (a->version != b->version) return false;

	if (a->is_show != b->is_show) return false;

	if (a->title != b->title) return false;

	if (a->subject != b->subject) return false;

	if (a->content != b->content) return false;

	return true;
}

