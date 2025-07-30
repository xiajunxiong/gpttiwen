#include "razaizhanyunzeconfig.hpp"

#include "monster/monster_group_pool.hpp"


RAZaiZhanYunZeConfig::RAZaiZhanYunZeConfig()
{
}

RAZaiZhanYunZeConfig::~RAZaiZhanYunZeConfig()
{
	std::vector<RAZaiZhanYunZe::BossCfg>().swap(m_boss_cfg_vec);
}

bool RAZaiZhanYunZeConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("reward", InitDefGroupIdMap);
	LOAD_RA_CONFIG("reward_probability", InitRandGroupIdMap);
	LOAD_RA_CONFIG("basics", InitBossCfg);

	return true;
}

const RAZaiZhanYunZe::BossCfg * RAZaiZhanYunZeConfig::GetBossCfg(int boss_index) const
{
	if (0 > boss_index
		|| (int)m_boss_cfg_vec.size() <= boss_index)
	{
		return NULL;
	}
	return &m_boss_cfg_vec[boss_index];
}

const RAZaiZhanYunZe::RewardVec * RAZaiZhanYunZeConfig::GetDefRewardGroupCfg(RandActivityManager * ramgr, int def_group_id) const
{
	RAZaiZhanYunZe::DefGroupIdMap::const_iterator group_cit = m_def_group_id_map.find(def_group_id);
	if (m_def_group_id_map.end() == group_cit)
	{
		return NULL;
	}

	const int act_begin_game_open_day
		= this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_ZAI_ZHAN_YUN_ZE);
	RAZaiZhanYunZe::DefSectionDayMap::const_iterator section_cit
		= group_cit->second.section_cfg_map.upper_bound(act_begin_game_open_day);
	if (group_cit->second.section_cfg_map.begin() == section_cit)
	{
		return NULL;
	}
	section_cit--;

	return &section_cit->second;
}

const RAZaiZhanYunZe::RewardVec * RAZaiZhanYunZeConfig::GetRandRewardGroupCfg(RandActivityManager * ramgr, int rand_group_id) const
{
	RAZaiZhanYunZe::RandGroupIdMap::const_iterator group_cit = m_rand_group_id_map.find(rand_group_id);
	if (m_rand_group_id_map.end() == group_cit)
	{
		return NULL;
	}

	const int act_begin_game_open_day
		= this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_ZAI_ZHAN_YUN_ZE);
	RAZaiZhanYunZe::RandSectionDayMap::const_iterator section_cit
		= group_cit->second.section_day_map.upper_bound(act_begin_game_open_day);
	if (group_cit->second.section_day_map.begin() == section_cit)
	{
		return NULL;
	}
	section_cit--;

	const RAZaiZhanYunZe::RandSectionCfg & rand_pool = section_cit->second;
	//随机
	int rand_num = RandomNum(rand_pool.total_power);

	RAZaiZhanYunZe::RandPool::const_iterator item_cit
		= rand_pool.rand_pool.upper_bound(rand_num);
	if (rand_pool.rand_pool.begin() == item_cit)
	{
		return NULL;
	}
	item_cit--;
	return &item_cit->second.reward_vec;
}

int RAZaiZhanYunZeConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -1000;
	}

	if (!GetSubNodeValue(data_element, "cost_item", m_other_cfg.battle_cost_item_id)
		|| NULL == ITEMPOOL->GetItem(m_other_cfg.battle_cost_item_id))
	{
		return -1;
	}

	return 0;
}

int RAZaiZhanYunZeConfig::InitDefGroupIdMap(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -7000;
	}

	while (NULL != data_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(data_element, "item_id", group_id)
			|| 0 >= group_id)
		{
			return -1;
		}
		RAZaiZhanYunZe::DefRewardGroupCfg &group_cfg = m_def_group_id_map[group_id];
		group_cfg.group_id = group_id;

		int section_begin_day = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_begin_day)
			|| 0 >= section_begin_day)
		{
			return -2;
		}

		RAZaiZhanYunZe::RewardVec &reward_vec = group_cfg.section_cfg_map[section_begin_day];
		{
			TiXmlElement * item_list_element = data_element->FirstChildElement("reward_item_list");
			if (NULL == item_list_element)
			{
				return -101;
			}
			TiXmlElement * item_element = item_list_element->FirstChildElement("reward_item");

			while (NULL != item_element)
			{
				ItemConfigData temp_item;
				if (!temp_item.ReadConfig(item_element))
				{
					return -102;
				}

				reward_vec.push_back(temp_item);
				item_element = item_element->NextSiblingElement();
			}
		}

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RAZaiZhanYunZeConfig::InitRandGroupIdMap(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -7000;
	}

	while (NULL != data_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(data_element, "item_id", group_id)
			|| 0 >= group_id)
		{
			return -1;
		}
		RAZaiZhanYunZe::RandRewardGroupCfg &group_cfg = m_rand_group_id_map[group_id];
		group_cfg.group_id = group_id;

		int section_begin_day = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_begin_day)
			|| 0 >= section_begin_day)
		{
			return -2;
		}

		RAZaiZhanYunZe::RandSectionCfg &rand_section_cfg = group_cfg.section_day_map[section_begin_day];
		rand_section_cfg.game_open_day_begin = section_begin_day;

		int power = 0;
		if (!GetSubNodeValue(data_element, "probability", power)
			|| 0 >= power)
		{
			return -4;
		}

		int pool_item_index = 0;
		if (!GetSubNodeValue(data_element, "index", pool_item_index)
			|| (int)rand_section_cfg.rand_pool.size() != pool_item_index)
		{
			return -5;
		}

		RAZaiZhanYunZe::RewardVec &reward_vec = rand_section_cfg.rand_pool[rand_section_cfg.total_power].reward_vec;
		rand_section_cfg.rand_pool[rand_section_cfg.total_power].index = pool_item_index;
		rand_section_cfg.total_power += power;

		{
			TiXmlElement * item_list_element = data_element->FirstChildElement("reward_item_list");
			if (NULL == item_list_element)
			{
				return -101;
			}
			TiXmlElement * item_element = item_list_element->FirstChildElement("reward_item");

			while (NULL != item_element)
			{
				ItemConfigData temp_item;
				if (!temp_item.ReadConfig(item_element))
				{
					return -102;
				}

				reward_vec.push_back(temp_item);
				item_element = item_element->NextSiblingElement();
			}
		}

		data_element = data_element->NextSiblingElement();
	}

	//检查总权重
	for (RAZaiZhanYunZe::RandGroupIdMap::iterator group_it = m_rand_group_id_map.begin()
		; group_it != m_rand_group_id_map.end()
		; group_it++)
	{
		for (RAZaiZhanYunZe::RandSectionDayMap::iterator section_it = group_it->second.section_day_map.begin()
			; section_it != group_it->second.section_day_map.end()
			; section_it++)
		{
			if (10000 != section_it->second.total_power)
			{// 总权重不为 100000     奖励组id                 奖池物品index
				return -100000 - (group_it->first * 100) - section_it->first;
			}
		}
	}
	return 0;
}

int RAZaiZhanYunZeConfig::InitBossCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -7000;
	}

	while (NULL != data_element)
	{
		int boss_index = (int)m_boss_cfg_vec.size();
		m_boss_cfg_vec.push_back(RAZaiZhanYunZe::BossCfg());
		RAZaiZhanYunZe::BossCfg &boss_cfg = m_boss_cfg_vec[boss_index];

		if (!GetSubNodeValue(data_element, "seq", boss_cfg.boss_index)
			|| boss_index != boss_cfg.boss_index
			|| RAZaiZhanYunZe::BOSS_MAX <= boss_index)
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "cost_item", boss_cfg.battle_cost_item_num)
			|| 0 >= boss_cfg.battle_cost_item_num)
		{
			return -700;
		}


		if (!GetSubNodeValue(data_element, "section_start", boss_cfg.act_open_day)
			|| 0 >= boss_cfg.act_open_day)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "monster_group_id", boss_cfg.monster_group_id)
			|| NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(boss_cfg.monster_group_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "reward1", boss_cfg.first_pass_id)
			|| this->CheckDefGroupID(boss_cfg.first_pass_id))
		{
			return -4;
		}

		if (!GetSubNodeValue(data_element, "reward2", boss_cfg.pass_rand_id)
			|| this->CheckRandGroupID(boss_cfg.pass_rand_id))
		{
			return -5;
		}

		for(int i = 0; i < RAZaiZhanYunZe::BOSS_PASS_CFG_MAX; i++)
		{
			int pass_time = 0;
			int pass_time_group_id = 0;

			std::string pass_time_str = STRING_SPRINTF("number%d", i + 1);
			std::string pass_time_group_id_str = STRING_SPRINTF("number_reward%d", i + 1);

			if (!GetSubNodeValue(data_element, pass_time_str, pass_time)
				|| 0 > pass_time)
			{
				return -10;
			}

			if (0 == pass_time)
			{
				break;
			}

			if (!GetSubNodeValue(data_element, pass_time_group_id_str, pass_time_group_id)
				|| this->CheckDefGroupID(pass_time_group_id))
			{
				return -11;
			}

			boss_cfg.pass_num_id_map[pass_time] = pass_time_group_id;

			if (i + 1 != (int)boss_cfg.pass_num_id_map.size())
			{//重复
				return -12;
			}
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

bool RAZaiZhanYunZeConfig::CheckDefGroupID(int group_id) const
{
	return m_def_group_id_map.end() == m_def_group_id_map.find(group_id);
}

bool RAZaiZhanYunZeConfig::CheckRandGroupID(int group_id) const
{
	return m_rand_group_id_map.end() == m_rand_group_id_map.find(group_id);
}
