#include "dayrewardconfig.hpp"
#include "checkresourcecenter.hpp"
#include "servercommon/commondata.hpp"
#include "other/pet/petconfig.hpp"

DayRewardConfig::DayRewardConfig() : m_max_online_gift_count(0)
{
}

DayRewardConfig::~DayRewardConfig()
{
	for (std::map<int, std::vector<SignReward> >::iterator it = m_sign_reward_map.begin(); it != m_sign_reward_map.end(); it++)
	{
		std::vector<SignReward>().swap(it->second);
	}

	for (std::map<int, std::vector<DayRewardFindPriceCfg> >::iterator it = m_find_reward_price_map.begin(); it != m_find_reward_price_map.end(); it++)
	{
		std::vector<DayRewardFindPriceCfg>().swap(it->second);
	}

	std::vector<DayRewardDayFoodCfg>().swap(m_day_food_vec);
	std::vector<PetStrengthenWelfareCfg>().swap(m_pet_strengthen_welfare_cfg);
}

bool DayRewardConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("online_reward", InitOnlineRewardCfg);
	LOAD_CONFIG("sign_reward", InitSignRewardCfg);
	LOAD_CONFIG("level_reward", InitLevelRewardCfg);
	LOAD_CONFIG("find_price", InitFindRewardPriceCfg);			//找回价格读取需要放在活动找回读取前,用于检测活动找回是否缺失价格配置
	LOAD_CONFIG("find_reward", InitFindRewardCfg);
	LOAD_CONFIG("day_food", InitDayFoodCfg);
	LOAD_CONFIG("reward_find", InitOtherCfg);
	LOAD_CONFIG("pet_strengthen", InitPetStrengthenCfg);
	LOAD_CONFIG("online_gift", InitOnlineGiftCfg);

	return true;
}

const OnlineReward * DayRewardConfig::GetOnlineReward(int index)
{
	std::map<int, OnlineReward>::const_iterator it = m_online_reward_map.find(index);
	if (it == m_online_reward_map.end()) return NULL;
	return &it->second;
}

const SignReward * DayRewardConfig::GetSignReward(int month, int day, int sever_open_day)
{
	int date_key = 1000 * month + day;
	std::map<int, std::vector<SignReward> >::const_iterator it = m_sign_reward_map.find(date_key);
	if (it == m_sign_reward_map.end()) return NULL;

	for (int i = 0; i < (int)it->second.size(); i++)
	{
		if (it->second[i].section_start <= sever_open_day && it->second[i].section_end >= sever_open_day)
		{
			return &it->second[i];
		}
	}

	return NULL;
}

const LevelReward * DayRewardConfig::GetLevelReward(int seq)
{
	std::map<int, LevelReward>::const_iterator it = m_level_reward_map.find(seq);
	if (it == m_level_reward_map.end()) return NULL;
	return &it->second;
}

const DayRewardFindCfg * DayRewardConfig::GetFindReward(int type)
{
	std::map<int, DayRewardFindCfg>::const_iterator it = m_find_reward_map.find(type);
	if (it == m_find_reward_map.end()) return NULL;
	return &it->second;
}

const DayRewardFindPriceCfg * DayRewardConfig::GetFindRewardPriceCfg(int type, int role_level)
{
	if(role_level <= 0 || role_level > MAX_ROLE_LEVEL) return NULL;

	std::map<int, std::vector<DayRewardFindPriceCfg> >::iterator it = m_find_reward_price_map.find(type);
	if(it == m_find_reward_price_map.end()) return NULL;

	for (int i = 0; i < (int)it->second.size(); i++)
	{
		if (it->second[i].min_level <= role_level && it->second[i].max_level >= role_level)
		{
			return &it->second[i];
		}
	}

	return NULL;
}

const DayRewardDayFoodCfg * DayRewardConfig::GetDayFoodCfg(int seq)
{
	if(seq < 0 || seq >= (int)m_day_food_vec.size()) return NULL;

	return &m_day_food_vec[seq];
}

const PetStrengthenWelfareCfg * DayRewardConfig::GetPetStrengthenWelfareCfg(int level_or_seq, bool is_seq)
{
	if (level_or_seq <= 0 || level_or_seq > PET_STRENGTHEN_MAX_LEVEL) return NULL;

	for (int i = 0; i < (int)m_pet_strengthen_welfare_cfg.size(); ++i)
	{
		if (is_seq)
		{
			if (level_or_seq == m_pet_strengthen_welfare_cfg[i].seq)
			{
				return &m_pet_strengthen_welfare_cfg[i];
			}
			continue;
		}

		if (level_or_seq == m_pet_strengthen_welfare_cfg[i].pet_strengthen_lv)
		{
			return &m_pet_strengthen_welfare_cfg[i];
		}
	}

	return NULL;
}

const DayRewardOnlineGiftCfg * DayRewardConfig::GetOnlineGiftCfg(int seq) const
{
	if (seq < 0 || seq >= m_max_online_gift_count || seq >= ARRAY_ITEM_COUNT(m_online_gift_list))
	{
		return NULL;
	}

	return &m_online_gift_list[seq];
}

int DayRewardConfig::InitOnlineRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		OnlineReward node_cfg;
		if (!GetSubNodeValue(root_element, "seq", node_cfg.seq) || node_cfg.seq < 0 || node_cfg.seq >= (int)(sizeof(short) * 8))return -1;
		if (!GetSubNodeValue(root_element, "time", node_cfg.time) || node_cfg.time < 0)return -2;
		TiXmlElement *reward_list_element = root_element->FirstChildElement("online_reward_list");
		if (NULL == reward_list_element) return -999;
		TiXmlElement *reward_element = reward_list_element->FirstChildElement("online_reward");
		node_cfg.reward_count = 0;
		while (NULL != reward_element)
		{
			if (!node_cfg.reward[node_cfg.reward_count].ReadConfigNoCheck(reward_element))return -3;
			CheckResourceCenter::Instance().GetItemValidCheck()->Add(node_cfg.reward[node_cfg.reward_count].item_id, node_cfg.reward[node_cfg.reward_count].item_id, node_cfg.reward[node_cfg.reward_count].num, __FUNCTION__);

			node_cfg.reward_count++;
			reward_element = reward_element->NextSiblingElement();
		}

		if (!GetSubNodeValue(root_element, "bind_coin", node_cfg.bind_coin) || node_cfg.bind_coin < 0)return -4;
		if (0 == node_cfg.bind_coin && 0 == node_cfg.reward_count) return -5;

		if (!GetSubNodeValue(root_element, "section_start", node_cfg.section_start) || node_cfg.section_start <= 0)return -6;
		if (!GetSubNodeValue(root_element, "section_end", node_cfg.section_end) || node_cfg.section_end < 0)return -7;
		if (0 == node_cfg.section_end)
		{
			node_cfg.section_end = INT_MAX;
		}
		if (node_cfg.section_start > node_cfg.section_end)
		{
			return -8;
		}
		if (!GetSubNodeValue(root_element, "index", node_cfg.index) || node_cfg.index < 0)return -9;

		if (m_online_reward_map.find(node_cfg.index) != m_online_reward_map.end())return -10;
	
		m_online_reward_map[node_cfg.index] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int DayRewardConfig::InitSignRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		SignReward node_cfg;
		if (!GetSubNodeValue(root_element, "month", node_cfg.month) || node_cfg.month <= 0 || node_cfg.month > 12)return -1;
		if (!GetSubNodeValue(root_element, "day", node_cfg.day) || node_cfg.day <= 0 || node_cfg.day > 31)return -2;
		TiXmlElement *reward_element = root_element->FirstChildElement("online_reward");
		node_cfg.reward.ReadConfigNoCheck(reward_element);

		if (!GetSubNodeValue(root_element, "bind_coin", node_cfg.bind_coin) || node_cfg.bind_coin < 0)return -4;
		if (0 == node_cfg.reward.item_id && 0 == node_cfg.bind_coin)return -5;

		if (node_cfg.reward.item_id > 0)
		{
			CheckResourceCenter::Instance().GetItemValidCheck()->Add(node_cfg.reward.item_id, node_cfg.reward.item_id, node_cfg.reward.num, __FUNCTION__);
		}
		if (!GetSubNodeValue(root_element, "section_start", node_cfg.section_start) || node_cfg.section_start <= 0) return -6;
		if (!GetSubNodeValue(root_element, "section_end", node_cfg.section_end) || node_cfg.section_end < 0)return -7;
		if (0 == node_cfg.section_end)
		{
			node_cfg.section_end = INT_MAX;
		}
		if (node_cfg.section_start > node_cfg.section_end)
		{
			return -8;
		}

		int date_key = 1000 * node_cfg.month + node_cfg.day;
		std::map<int, std::vector<SignReward> >::iterator it = m_sign_reward_map.find(date_key);
		if (it != m_sign_reward_map.end() && !it->second.empty())
		{
			for (int i = 0; i < (int)it->second.size(); i++)
			{
				if (it->second[i].section_start == node_cfg.section_start && it->second[i].section_end == node_cfg.section_end)
				{
					return -9;
				}
			}
		}

		m_sign_reward_map[date_key].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int DayRewardConfig::InitLevelRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		LevelReward node_cfg;
		if (!GetSubNodeValue(root_element, "seq", node_cfg.seq) || node_cfg.seq <= 0)return -1;
		if (!GetSubNodeValue(root_element, "level", node_cfg.level) || node_cfg.level <= 0)return -1;
		TiXmlElement *reward_list_element = root_element->FirstChildElement("online_reward_list");
		if (NULL == reward_list_element) return -999;
		TiXmlElement *reward_element = reward_list_element->FirstChildElement("online_reward");
		node_cfg.reward_count = 0;
		while (NULL != reward_element)
		{
			if (!node_cfg.reward[node_cfg.reward_count].ReadConfigNoCheck(reward_element))return -3;
			CheckResourceCenter::Instance().GetItemValidCheck()->Add(node_cfg.reward[node_cfg.reward_count].item_id, node_cfg.reward[node_cfg.reward_count].item_id, node_cfg.reward[node_cfg.reward_count].num, __FUNCTION__);

			node_cfg.reward_count++;
			reward_element = reward_element->NextSiblingElement();
		}

		if (!GetSubNodeValue(root_element, "bind_coin", node_cfg.bind_coin) || node_cfg.bind_coin < 0)return -4;
		if (0 == node_cfg.bind_coin && 0 == node_cfg.reward_count) return -5;

		m_level_reward_map[node_cfg.seq] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int DayRewardConfig::InitFindRewardPriceCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int find_type = 0;
		if (!GetSubNodeValue(root_element, "find_type", find_type) || find_type <= DAY_ACTIVITY_TYPE_MIN || find_type >= DAY_ACTIVITY_TYPE_MAX)return -1;

		DayRewardFindPriceCfg node_cfg;
		if (!GetSubNodeValue(root_element, "min_level", node_cfg.min_level) || node_cfg.min_level <= 0 || node_cfg.min_level > MAX_ROLE_LEVEL) return -2;
		if (!GetSubNodeValue(root_element, "max_level", node_cfg.max_level) || node_cfg.max_level < node_cfg.min_level || node_cfg.max_level > MAX_ROLE_LEVEL) return -3;
		if (!GetSubNodeValue(root_element, "coin_scale", node_cfg.coin_scale) || node_cfg.coin_scale < 0) return -4;
		if (!GetSubNodeValue(root_element, "coin_price", node_cfg.coin_price) || node_cfg.coin_price < 0) return -5;

		m_find_reward_price_map[find_type].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int DayRewardConfig::InitFindRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		DayRewardFindCfg node_cfg;
		if (!GetSubNodeValue(root_element, "find_type", node_cfg.find_type) || node_cfg.find_type <= DAY_ACTIVITY_TYPE_MIN || node_cfg.find_type >= DAY_ACTIVITY_TYPE_MAX)return -1;

		if (m_find_reward_price_map.end() == m_find_reward_price_map.find(node_cfg.find_type))
		{
			return -33;
		}
					
		//if (!GetSubNodeValue(root_element, "coin_scale", node_cfg.coin_scale)) return -2;
		//if (!GetSubNodeValue(root_element, "coin_price", node_cfg.coin_price)) return -3;
		//if (!GetSubNodeValue(root_element, "gold_scale", node_cfg.gold_scale)) return -4;
		//if (!GetSubNodeValue(root_element, "gold_price", node_cfg.gold_price)) return -5;
		if (!GetSubNodeValue(root_element, "find_level", node_cfg.find_level)) return -6;

		m_find_reward_map[node_cfg.find_type] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int DayRewardConfig::InitDayFoodCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	int next_seq = 0;
	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq != next_seq) return -1;
		next_seq++;

		DayRewardDayFoodCfg node_cfg;
		if (!GetSubNodeValue(root_element, "time_start", node_cfg.start_time) || node_cfg.start_time < 0) return -2;
		if (!GetSubNodeValue(root_element, "time_end", node_cfg.end_time) || node_cfg.start_time >= node_cfg.end_time) return -3;
		if (!GetSubNodeValue(root_element, "food_num", node_cfg.food_num) || node_cfg.food_num <= 0) return -4;

		int is_make = 0;
		if (!GetSubNodeValue(root_element, "is_make", is_make) || is_make < 0) return -5;
		node_cfg.is_make = (0 != is_make) ? true : false;

		if (!GetSubNodeValue(root_element, "make_time_end", node_cfg.make_time_end) || node_cfg.make_time_end <= node_cfg.end_time || node_cfg.make_time_end > 2400) return -6;
		if (!GetSubNodeValue(root_element, "currency_type", node_cfg.currency_type) || node_cfg.currency_type < 0 || node_cfg.currency_type >= DAY_REWARD_DAY_FOOD_CURRENCY_TYPE_MAX) return -7;
		if (!GetSubNodeValue(root_element, "currency_num", node_cfg.currency_num) || node_cfg.currency_num <= 0) return -8;

		m_day_food_vec.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int DayRewardConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -999;

	if (!GetSubNodeValue(root_element, "copper_coin_price", m_other_cfg.copper_coin_price) || m_other_cfg.copper_coin_price < 0) return -__LINE__;
	if (!GetSubNodeValue(root_element, "max_level", m_other_cfg.max_level) || m_other_cfg.max_level <= 0 || m_other_cfg.max_level > PET_STRENGTHEN_MAX_LEVEL) return -__LINE__;

	TiXmlElement * pet_strengthen_element = root_element->FirstChildElement("pet_strengthen");
	if (NULL == pet_strengthen_element) return -998;

	TiXmlElement *node_element = pet_strengthen_element->FirstChildElement("node");
	while (NULL != node_element)
	{
		int node = 0;
		if (!GetNodeValue(node_element, node) || node <= PET_QUALITY_INVALID || node >= PET_QUALITY_MAX)  return -__LINE__;

		m_other_cfg.pet_strengthen.insert(node);
		node_element = node_element->NextSiblingElement();
	}

	if (!GetSubNodeValue(root_element, "online_gift_open", m_other_cfg.online_gift_open_level) || m_other_cfg.online_gift_open_level <= 0 || 
		m_other_cfg.online_gift_open_level > MAX_ROLE_LEVEL) 
	{
		return -__LINE__;
	}

	return 0;
}

int DayRewardConfig::InitPetStrengthenCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -999;

	while (NULL != root_element)
	{
		PetStrengthenWelfareCfg psw_cfg;

		TiXmlElement * quality_element = root_element->FirstChildElement("quality");
		if (NULL == quality_element) return -998;

		TiXmlElement * node_element = quality_element->FirstChildElement("node");
		while (NULL != node_element)
		{
			int node = 0;
			if (!GetNodeValue(node_element, node) || node <= PET_QUALITY_INVALID || node >= PET_QUALITY_MAX) return -__LINE__;

			psw_cfg.quality.insert(node);
			node_element = node_element->NextSiblingElement();
		}

		if (!GetSubNodeValue(root_element, "seq", psw_cfg.seq) || psw_cfg.seq < 0) return -__LINE__;
		if (!GetSubNodeValue(root_element, "pet_strengthen", psw_cfg.pet_strengthen_lv) || psw_cfg.pet_strengthen_lv <= 0 || psw_cfg.pet_strengthen_lv > PET_STRENGTHEN_MAX_LEVEL) return -__LINE__;

		TiXmlElement * reward_list_element = root_element->FirstChildElement("item_list_1_list");
		if (NULL == reward_list_element) return -__LINE__;

		TiXmlElement * reward_element = reward_list_element->FirstChildElement("item_list_1");
		while (NULL != reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element)) return -__LINE__;

			psw_cfg.reward_list.push_back(item);
			reward_element = reward_element->NextSiblingElement();
		}

		m_pet_strengthen_welfare_cfg.push_back(psw_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int DayRewardConfig::InitOnlineGiftCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -999;

	m_max_online_gift_count = 0;
	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0 || seq != m_max_online_gift_count || seq >= ARRAY_ITEM_COUNT(m_online_gift_list)) 
		{
			return -__LINE__;
		}

		DayRewardOnlineGiftCfg node_cfg;
		if (!GetSubNodeValue(root_element, "time", node_cfg.need_time) || node_cfg.need_time <= 0)
		{
			return -__LINE__;
		}

		TiXmlElement * reward_list_element = root_element->FirstChildElement("online_reward_list");
		if (NULL == reward_list_element) return -__LINE__;

		TiXmlElement * reward_element = reward_list_element->FirstChildElement("online_reward");
		if (NULL == reward_element)
		{
			return -__LINE__;
		}
		while (NULL != reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element)) return -__LINE__;

			node_cfg.reward_list.push_back(item);
			reward_element = reward_element->NextSiblingElement();
		}
		if (node_cfg.reward_list.empty())
		{
			return -__LINE__;
		}

		m_max_online_gift_count += 1;
		m_online_gift_list[seq] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

