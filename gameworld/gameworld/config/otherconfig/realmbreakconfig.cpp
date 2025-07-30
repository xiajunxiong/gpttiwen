#include "realmbreakconfig.hpp"

#include "monster/monster_group_pool.hpp"
#include "item/itempool.h"

#include "servercommon/pethelperformationdef.hpp"

RealmBreakConfig::RealmBreakConfig()
{
}

RealmBreakConfig::~RealmBreakConfig()
{
	for (std::map<int, std::vector<RealmBreakItemCfg> >::iterator it = m_item_group_cfg.begin(); it!= m_item_group_cfg.end(); it++)
	{
		std::vector<RealmBreakItemCfg>().swap(it->second);
	}

	std::vector<RealmBreakFightCfg>().swap(m_fight_cfg);
}

bool RealmBreakConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("item_group", InitItemGroupCfg);	//需要放在境界基础配置读取之前
	LOAD_CONFIG("break_config", InitBreakCfg);		//需要放在other读取之后
	LOAD_CONFIG("break_item", InitBreakItemCfg);
	LOAD_CONFIG("soul_fight", InitBreakFightCfg);
	LOAD_CONFIG("help_fight", InitHelpFightCfg);
	LOAD_CONFIG("gaiming_item", InitGaiMingItemCfg);

	return true;
}

const RealmBreakCfg * RealmBreakConfig::GetBreakCfg(int seq)
{
	std::map<int, RealmBreakCfg>::iterator it = m_break_cfg.find(seq);
	if(it == m_break_cfg.end()) return NULL;

	return &it->second;
}

int RealmBreakConfig::GetBreakExpByItemId(ItemID item_id)
{
	std::map<ItemID, int>::iterator it = m_item_cfg.find(item_id);
	if(it == m_item_cfg.end()) return 0;

	return it->second;
}

bool RealmBreakConfig::GetRandItem(int item_group_id, int& rand_num, RealmBreakItem * item_list)
{
	if(NULL == item_list || rand_num <= 0) return false;

	std::map<int, std::vector<RealmBreakItemCfg> >::const_iterator it = m_item_group_cfg.find(item_group_id);
	if(it == m_item_group_cfg.end()) return false;

	int item_count = 0;
	for (int i = 0; i < rand_num && i < MAX_REALM_BREAK_ITEM_NUM; i++)
	{
		int rate_num = rand() % RealmBreakItemCfg::RAND_RATE_BASE_NUM;
		for (int k = 0; k < (int)it->second.size(); ++k)
		{
			if (rate_num < it->second[k].rate)
			{
				item_list[item_count].item_id =  it->second[k].item.item_id;
				item_list[item_count].is_bind = it->second[k].item.is_bind ? 1 : 0;
				item_list[item_count].num = it->second[k].item.num;
				item_count++;
				break;
			}
			rate_num -= it->second[k].rate;
		}
	}
	if(item_count <= 0) return false;

	rand_num = item_count;

	return true;
}

const RealmBreakFightCfg * RealmBreakConfig::GetFightCfg(int seq) const
{
	if (0 >= seq || seq > m_fight_cfg.size())
	{
		return NULL;
	}

	return &m_fight_cfg[seq - 1];
}

const RealmBreakFightCfg * RealmBreakConfig::MonsterGroupGetFightCft(int monster_group_id) const
{
	for (std::vector<RealmBreakFightCfg>::const_iterator it = m_fight_cfg.begin(); it != m_fight_cfg.end(); ++it)
	{
		if (it->monster_group_id == monster_group_id)
		{
			return &(*it);
		}
	}
	return NULL;
}

int RealmBreakConfig::GetFightCanPetNum(int create_role_day) const
{
	if (create_role_day <= 0)
	{
		create_role_day = 1;
	}
	
	int pet_num = 0;
	for (std::map<int, int>::const_reverse_iterator it = m_help_fight_map.rbegin(); it != m_help_fight_map.rend(); ++it)
	{
		if (create_role_day >= it->first)
		{
			pet_num = it->second;
			break;
		}
	}

	return pet_num;
}

const std::vector<ItemConfigData>* RealmBreakConfig::GetGaiMingReissueCfg(int layer) const
{
	if (layer <= 0 || layer > (int)m_gaiming_cfg.size()) return NULL;
	return &m_gaiming_cfg[layer - 1];
}

int RealmBreakConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "break_num", m_other_cfg.break_num) || m_other_cfg.break_num < 1) return -1;
	if (!GetSubNodeValue(root_element, "break_max_num", m_other_cfg.break_max_num) || m_other_cfg.break_max_num < m_other_cfg.break_num || m_other_cfg.break_max_num >= MAX_INT16) return -2;
	if (!GetSubNodeValue(root_element, "break_exp_s", m_other_cfg.break_exp_s) || m_other_cfg.break_exp_s <= 0) return -3;
	if (!GetSubNodeValue(root_element, "player_exp_s", m_other_cfg.player_exp_s) || m_other_cfg.player_exp_s <= 0) return -4;
	if (!GetSubNodeValue(root_element, "tuna_time_s", m_other_cfg.tuna_time_s) || m_other_cfg.tuna_time_s <= 1) return -5;	//吐纳时长限制不小于等于1
	if (!GetSubNodeValue(root_element, "tuna_num", m_other_cfg.tuna_num) || m_other_cfg.tuna_num <= 0) return -6;
	int item_min = 0;
	if (!GetSubNodeValue(root_element, "item_min", item_min) || item_min <= 0) return -7;
	m_other_cfg.item_time_s = item_min  * SECOND_PER_MIN;

	if (!GetSubNodeValue(root_element, "gaimingdan_item_s", m_other_cfg.gaimingdan_item_s) || m_other_cfg.gaimingdan_item_s <= 0) return -8;

	return 0;
}

int RealmBreakConfig::InitItemGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int item_group_id = 0;
		if (!GetSubNodeValue(root_element, "reward_group", item_group_id) || item_group_id <= 0) return -1;

		RealmBreakItemCfg node_cfg;
		if (!GetSubNodeValue(root_element, "rate_10000", node_cfg.rate) || node_cfg.rate <= 0) return -2;

		TiXmlElement * item_element = root_element->FirstChildElement("reward");
		if (NULL == item_element)
		{
			return -3;
		}
		if (!node_cfg.item.ReadConfig(item_element))
		{
			return -4;
		}

		m_item_group_cfg[item_group_id].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, std::vector<RealmBreakItemCfg> >::const_iterator it = m_item_group_cfg.begin(); it != m_item_group_cfg.end(); ++it)
	{
		int rate_num = 0;
		for (int i = 0; i < (int)it->second.size(); ++i)
		{
			rate_num += it->second[i].rate;
		}
		if (rate_num > RealmBreakItemCfg::RAND_RATE_BASE_NUM)
		{
			return -10;
		}
	}

	return 0;
}

int RealmBreakConfig::InitBreakCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	int last_seq = 0;
	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq != last_seq + 1 || seq > m_other_cfg.break_max_num) return -1;
		last_seq = seq;

		RealmBreakCfg node_cfg;
		if (!GetSubNodeValue(root_element, "break_exp", node_cfg.break_need_exp) || node_cfg.break_need_exp <= 0) return -2;
		if (!GetSubNodeValue(root_element, "break_exp_time_s", node_cfg.break_exp_time_s) || node_cfg.break_exp_time_s <= 0) return -3;
		if (!GetSubNodeValue(root_element, "player_exp_time_s", node_cfg.player_exp_time_s) || node_cfg.player_exp_time_s <= 0) return -4;
		if (!GetSubNodeValue(root_element, "monster_group", node_cfg.monster_group_id) || ((0 != node_cfg.monster_group_id) && 
			NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(node_cfg.monster_group_id)))
		{
			return -5;
		}
		if (!GetSubNodeValue(root_element, "tuna_exp", node_cfg.tuna_exp) || node_cfg.tuna_exp <= 0) return -6;
		if (!GetSubNodeValue(root_element, "exp_double", node_cfg.exp_double) || node_cfg.exp_double <= 0) return -7;
		if (!GetSubNodeValue(root_element, "exp_treble", node_cfg.exp_treble) || node_cfg.exp_treble <= 0) return -8;
		if (!GetSubNodeValue(root_element, "exp_quintuple", node_cfg.exp_quintuple) || node_cfg.exp_quintuple <= 0) return -9;

		int rate_count = node_cfg.exp_double + node_cfg.exp_quintuple + node_cfg.exp_treble;
		if (rate_count > RealmBreakCfg::MAX_RATE_COUNT_NUM)
		{
			return -10;
		}
		if (!GetSubNodeValue(root_element, "tuna_num", node_cfg.tuna_num) || node_cfg.tuna_num < -1) return -11;
		if (!GetSubNodeValue(root_element, "break_player_exp_award", node_cfg.break_player_exp_award) || node_cfg.break_player_exp_award < 0) return -12;
		if (!GetSubNodeValue(root_element, "break_lose", node_cfg.break_lose) || node_cfg.break_lose < 0 || node_cfg.break_lose > node_cfg.break_need_exp) return -13;
		if (!GetSubNodeValue(root_element, "lingli_lixian_max", node_cfg.break_exp_lixian_max) || node_cfg.break_exp_lixian_max < node_cfg.break_exp_time_s) return -14;
		if (!GetSubNodeValue(root_element, "exp_lixian_max", node_cfg.player_exp_lixian_max) || node_cfg.player_exp_lixian_max < node_cfg.player_exp_time_s) return -15;
		int time_max_hour = 0;
		if (!GetSubNodeValue(root_element, "time_max_hour", time_max_hour) || time_max_hour <= 0) return -16;
		node_cfg.time_max_s = time_max_hour * SECOND_PER_HOUR;
		if (node_cfg.time_max_s < m_other_cfg.player_exp_s || 0 != node_cfg.time_max_s % m_other_cfg.player_exp_s)
		{
			return -17;
		}

		if (node_cfg.time_max_s < m_other_cfg.item_time_s || 0 != node_cfg.time_max_s % m_other_cfg.item_time_s || node_cfg.time_max_s / m_other_cfg.item_time_s > MAX_REALM_BREAK_ITEM_NUM)
		{
			return -18;
		}

		//允许空奖励
		TiXmlElement * break_item_list = root_element->FirstChildElement("break_item_list");
		if (NULL != break_item_list)
		{
			TiXmlElement * break_item_element = break_item_list->FirstChildElement("break_item");
			while (break_item_element != NULL)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(break_item_element))
				{
					return -200;
				}
				node_cfg.reward_list.push_back(reward);
				break_item_element = break_item_element->NextSiblingElement();
			}
		}

		if (!GetSubNodeValue(root_element, "player_coin_time_s", node_cfg.player_coin_time_s) || node_cfg.player_coin_time_s <= 0) return -19;
		if (!GetSubNodeValue(root_element, "lixian_item", node_cfg.item_group_id) || m_item_group_cfg.find(node_cfg.item_group_id) == m_item_group_cfg.end()) return -20;

		if (!GetSubNodeValue(root_element, "lixian_item_hours", node_cfg.time_interval_item_id) || NULL == ITEMPOOL->GetItem(node_cfg.time_interval_item_id)) return -21;
		if (!GetSubNodeValue(root_element, "lixian_item_hours_num", node_cfg.time_interval_item_num) || node_cfg.time_interval_item_num <= 0) return -22;


		m_break_cfg[seq] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}

	if ((int)m_break_cfg.size() != m_other_cfg.break_max_num || m_break_cfg.find(m_other_cfg.break_max_num) == m_break_cfg.end())
	{
		return -888;
	}
	
	return 0;
}

int RealmBreakConfig::InitBreakItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	
	while (NULL != root_element)
	{
		ItemID item_id = 0;
		int add_break_exp = 0;
		if (!GetSubNodeValue(root_element, "break_item_id", item_id) || NULL == ItemPool::Instance()->GetItem(item_id)) return -1;
		if (!GetSubNodeValue(root_element, "exp", add_break_exp) || add_break_exp <= 0) return -2;

		m_item_cfg[item_id] = add_break_exp;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int RealmBreakConfig::InitBreakFightCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		RealmBreakFightCfg node;

		if (!GetSubNodeValue(root_element, "seq", node.seq) || 0 >= node.seq || node.seq - 1 != m_fight_cfg.size()) return -1;
		if (!GetSubNodeValue(root_element, "monster_group_id", node.monster_group_id) || 0 >= node.monster_group_id) return -2;
		if (!GetSubNodeValue(root_element, "level", node.level) || 0 >= node.level) return -3;
		if (!GetSubNodeValue(root_element, "coin", node.coin) || 0 > node.coin) return -4;
		if (!GetSubNodeValue(root_element, "break_exp", node.break_exp) || 0 > node.break_exp) return -5;
		if (!GetSubNodeValue(root_element, "player_exp", node.player_exp) || 0 > node.player_exp) return -6;

		TiXmlElement * reward_list_element = root_element->FirstChildElement("break_item_list");
		if (reward_list_element != NULL)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("break_item");
			while (reward_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_element))
				{
					return -104;
				}
				node.item.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		m_fight_cfg.push_back(node);
		root_element = root_element->NextSiblingElement();
	}
	if ((int)m_fight_cfg.size() != m_other_cfg.break_max_num)
	{
		return -888;
	}

	return 0;
}

int RealmBreakConfig::InitHelpFightCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int create_role_day = 0, pet_num = 0;
		if (!GetSubNodeValue(root_element, "creat_role_days", create_role_day) || create_role_day < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "pet_num", pet_num) || pet_num < 0 || pet_num >= FORMATION_STANDBY_PET_NUM)
		{
			return -2;
		}

		m_help_fight_map[create_role_day] = pet_num;
		root_element = root_element->NextSiblingElement();
	}

	if (m_help_fight_map.empty())
	{
		return -888;
	}

	return 0;
}

int RealmBreakConfig::InitGaiMingItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -544;

	while (root_element)
	{
		TiXmlElement* reward_list_element = root_element->FirstChildElement("gaiming_item_list");
		if (NULL == reward_list_element) return -1;

		std::vector<ItemConfigData> cfg;
		TiXmlElement* reward_element = reward_list_element->FirstChildElement("gaiming_item");
		while (reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element)) return -2;

			cfg.push_back(item);

			reward_element = reward_element->NextSiblingElement();
		}

		m_gaiming_cfg.push_back(cfg);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
