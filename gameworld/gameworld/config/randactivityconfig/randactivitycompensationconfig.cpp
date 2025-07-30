#include "randactivitycompensationconfig.hpp"
#include "servercommon/configcommon.h"

RACompensationConfig::RACompensationConfig()
{

}

RACompensationConfig::~RACompensationConfig()
{
	ServerCfgVec().swap(m_server_cfg_list);
	ItemGroupCfgVec().swap(m_item_group_cfg_list);
	for (UserSeqListMap::iterator it = m_user_seq_list_map.begin(); it != m_user_seq_list_map.end(); it++)
	{
		SeqList().swap(it->second);
	}
	for (UserSeqListMapOnErrorLog::iterator it = m_user_seq_list_map_on_error_log.begin(); it != m_user_seq_list_map_on_error_log.end(); it++)
	{
		SeqList().swap(it->second);
	}
}

bool RACompensationConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("gift_configure", InitItemGroupCfg);
	LOAD_CONFIG("server", InitServerCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("role", InitUserListCfg);

	return true;
}

const RACompensationServerCfg* RACompensationConfig::GetServerCfg(const std::string& spid_name, int server_id)
{
	for (size_t i = 0; i < m_server_cfg_list.size(); ++i)
	{
		if (server_id == m_server_cfg_list[i].server_id && spid_name == m_server_cfg_list[i].spid_name)
		{
			return &m_server_cfg_list[i];
		}
	}

	return NULL;
}

const RACompensationItemGroupCfg* RACompensationConfig::GetItemGroupCfg(int item_group)
{
	return this->MutableItemGroupCfg(item_group);
}

const RACompensationConfig::SeqList* RACompensationConfig::GetUserSeqList(int role_id, const std::string& role_name)
{
	return this->MutableUserSeqList(role_id, role_name);
}

const RACompensationConfig::SeqList* RACompensationConfig::GetUserSeqListOnErrorLog(int role_id)
{
	return this->MutableUserSeqListOnErrorLog(role_id);
}

bool RACompensationConfig::IsNoCompensate(const std::string& spid_name, int server_id)
{
	const RACompensationServerCfg* cfg = this->GetServerCfg(spid_name, server_id);
	if (NULL == cfg)
	{
		return true; // 找不到配置当然也不补偿了
	}

	return cfg->open_day <= m_other_cfg.no_compensate_day;
}

const RACompensationItemGroupCfg* RACompensationConfig::GetItemGroupCfgByServerId(const std::string& spid_name, int server_id)
{
	const RACompensationServerCfg* cfg = this->GetServerCfg(spid_name, server_id);
	if (NULL == cfg)
	{
		return false;
	}

	const RACompensationItemGroupCfg* item_group = this->GetItemGroupCfg(cfg->item_group);
	return item_group;
}

int RACompensationConfig::InitOtherCfg(TiXmlElement* RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -999;
	}

	if (!GetSubNodeValue(dataElement, "reset_flag", m_other_cfg.reset_flag) || m_other_cfg.reset_flag <= 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "no_compensation", m_other_cfg.no_compensate_day))
	{
		return -2;
	}

	return 0;
}

int RACompensationConfig::InitServerCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");

	while (dataElement)
	{
		RACompensationServerCfg cfg;
		if (!GetSubNodeValue(dataElement, "platform", cfg.spid_name) || cfg.spid_name.size() != 3)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "server", cfg.server_id) || cfg.server_id <= 0 || cfg.server_id >= 2046)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "open_day", cfg.open_day) || cfg.open_day < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "item_group", cfg.item_group) || NULL == this->GetItemGroupCfg(cfg.item_group))
		{
			return -4;
		}

		m_server_cfg_list.push_back(cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int RACompensationConfig::InitItemGroupCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");

	int last_item_group = 0;
	int last_seq = -1;
	while (dataElement)
	{
		int item_group = 0;
		if (!GetSubNodeValue(dataElement, "item_group", item_group) || (last_item_group != item_group && last_item_group + 1 != item_group))
		{
			return -1;
		}

		if (last_item_group + 1 == item_group)
		{
			last_item_group = item_group;
			last_seq = -1;
			if (NULL != this->GetItemGroupCfg(item_group))
			{
				return -111;
			}

			RACompensationItemGroupCfg cfg;
			cfg.item_group = item_group;
			m_item_group_cfg_list.push_back(cfg);
		}

		RACompensationItemGroupCfg* cfg = this->MutableItemGroupCfg(item_group);
		if (NULL == cfg)
		{
			return -112;
		}

		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq != last_seq + 1)
		{
			return -2;
		}

		last_seq = seq;
		RACompensationSeqCfg seq_cfg;

		TiXmlElement * reward_list_element = dataElement->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element)
		{
			return -4;
		}
			
		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element)
		{
			return -401;
		}
			
		while (NULL != reward_element)
		{
			if (seq_cfg.item_list.size() >= 5)
			{
				return -402;
			}

			ItemConfigData item;
			if (!item.ReadConfig(reward_element))
			{
				return -403;
			}
			seq_cfg.item_list.push_back(item);

			reward_element = reward_element->NextSiblingElement();
		}
		cfg->seq_list.push_back(seq_cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int RACompensationConfig::InitUserListCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");

	while (dataElement)
	{
		int role_id = 0;
		if (!GetSubNodeValue(dataElement, "role_id", role_id) || role_id <= 0)
		{
			return -1;
		}

		std::string role_name;
		if (!GetSubNodeValue(dataElement, "name", role_name) || role_name.empty())
		{
			return -2;
		}

		RACompensationUser user;
		user.role_id = role_id;
		user.role_name = role_name;		
		SeqList* seq_list = this->MutableUserSeqList(role_id, role_name);
		if (NULL == seq_list)
		{
			m_user_seq_list_map.insert(UserSeqListMap::value_type(user, SeqList()));
			seq_list = this->MutableUserSeqList(role_id, role_name);
			if (NULL == seq_list)
			{
				return -33;
			}
		}

		SeqList* seq_list_on_error_log = this->MutableUserSeqListOnErrorLog(role_id);
		if (NULL == seq_list_on_error_log)
		{
			m_user_seq_list_map_on_error_log.insert(UserSeqListMapOnErrorLog::value_type(role_id, SeqList()));
			seq_list_on_error_log = this->MutableUserSeqListOnErrorLog(role_id);
			if (NULL == seq_list_on_error_log)
			{
				return -44;
			}
		}

		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq < 0)
		{
			return -5;
		}

		for (size_t i = 0; i < seq_list->size(); ++i)
		{
			if (seq == seq_list->at(i))
			{
				// 重复的seq
				return -55;
			}
		}
		seq_list->push_back(seq);

		for (size_t i = 0; i < seq_list_on_error_log->size(); ++i)
		{
			if (seq == seq_list_on_error_log->at(i))
			{
				// role_id下有重复的seq, 可能是不同渠道的相同role_id，这种情况请运营手动发补偿给这个玩家并从配置中剔除该玩家条目
				printf("RACompensationConfig::InitUserListCfg role_id[%d] role_name[%s] seq[%d]", role_id, role_name.c_str(), seq);
				return -66;
			}
		}
		seq_list_on_error_log->push_back(seq);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

RACompensationItemGroupCfg* RACompensationConfig::MutableItemGroupCfg(int item_group)
{
	for (size_t i = 0; i < m_item_group_cfg_list.size(); ++i)
	{
		if (item_group == m_item_group_cfg_list[i].item_group)
		{
			return &m_item_group_cfg_list[i];
		}
	}

	return NULL;
}

RACompensationConfig::SeqList* RACompensationConfig::MutableUserSeqList(int role_id, const std::string& role_name)
{
	RACompensationUser user;
	user.role_id = role_id;
	user.role_name = role_name;

	UserSeqListMap::iterator it = m_user_seq_list_map.find(user);
	if (it != m_user_seq_list_map.end())
	{
		return &it->second;
	}

	return NULL;
}

RACompensationConfig::SeqList* RACompensationConfig::MutableUserSeqListOnErrorLog(int role_id)
{
	UserSeqListMapOnErrorLog::iterator it = m_user_seq_list_map_on_error_log.find(role_id);
	if (it != m_user_seq_list_map_on_error_log.end())
	{
		return &it->second;
	}

	return NULL;
}

const RACompensationSeqCfg* RACompensationItemGroupCfg::GetSeqCfg(int seq) const
{
	if (seq < 0 || seq >= seq_list.size()) return NULL;

	return &seq_list[seq];
}
