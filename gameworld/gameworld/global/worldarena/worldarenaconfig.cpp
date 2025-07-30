#include "worldarenaconfig.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "checkresourcecenter.hpp"

WorldArenaConfig::WorldArenaConfig() : m_robot_attr_adjust_cfg_num(0)
{

}

WorldArenaConfig::~WorldArenaConfig()
{
	WinBoxRewardList().swap(m_win_box_reward_list);
	SeasonRankRewardList().swap(m_season_rank_reward_list);
}

bool WorldArenaConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("other", InitOtherCfg);
	LOAD_CONFIG_2("victory_times_reward", InitWinBoxRewardList);
	LOAD_CONFIG_2("season_rank_reward", InitSeasonRankRewardList);

	LOAD_CONFIG("attr_adjust_group", InitRobotAttrAdjustCfg);
	LOAD_CONFIG("robot_cfg", InitRobotCfg);

	return true;
}

const WorldArenaOtherCfg& WorldArenaConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

const WorldArenaWinBoxRewardCfg* WorldArenaConfig::GetWinBoxRewardCfg(int sn)
{
	for (size_t i = 0; i < m_win_box_reward_list.size(); ++i)
	{
		if (m_win_box_reward_list[i].sn == sn)
		{
			return &m_win_box_reward_list[i];
		}
	}

	return NULL;
}

const WorldArenaSeasonRankRewardCfg* WorldArenaConfig::GetSeasonRankRewardCfg(int rank)
{
	for (size_t i = 0; i < m_season_rank_reward_list.size(); ++i)
	{
		if (m_season_rank_reward_list[i].rank == rank)
		{
			return &m_season_rank_reward_list[i];
		}
	}

	return NULL;
}

const WorldArenaRobotCfg* WorldArenaConfig::GetRobotCfgByProfLevel(int prof_base, int level)
{
	ProfLevelKey key(prof_base, level);
	ProfLevelRobotMap::iterator it = m_prof_level_robot_map.find(key);
	if (it != m_prof_level_robot_map.end())
	{
		return &it->second;
	}

	return NULL;
}

bool BinarySearchByGroupId(const WorldArenaRobotAttrAdjustCfg* cfg_list, int cfg_list_size, int group_id, ARG_OUT int* out_index)
{
	if (NULL == cfg_list) return false;

	int low_index = 0;
	int high_index = cfg_list_size - 1;

	while (low_index <= high_index)
	{
		int mid_index = (low_index + high_index) / 2;
		if (group_id == cfg_list[mid_index].group_id)
		{
			if (NULL != out_index)
			{
				*out_index = mid_index;
				return true;
			}
		}
		else if (group_id < cfg_list[mid_index].group_id)
		{
			high_index = mid_index - 1;
		}
		else
		{
			low_index = mid_index + 1;
		}
	}

	return false;
}


const WorldArenaRobotAttrAdjustCfg* WorldArenaConfig::GetRobotAttrAdjustCfg(int group_id)
{
	int index = 0;
	if (BinarySearchByGroupId(m_robot_attr_adjust_list, m_robot_attr_adjust_cfg_num, group_id, &index))
	{
		return &m_robot_attr_adjust_list[index];
	}

	return NULL;
}

int WorldArenaConfig::InitOtherCfg(TiXmlElement *RootElement, std::string& err)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "prepare_scene", m_other_cfg.standby_scene_id))
	{
		return -111;
	}
	CheckResourceCenter::Instance().GetSceneCheck()->Add(m_other_cfg.standby_scene_id, __FUNCTION__);

	if (!GetSubNodeValue(dataElement, "duration", m_other_cfg.wait_enter_scene_minute) || m_other_cfg.wait_enter_scene_minute < 5)
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "winner_score", m_other_cfg.win_score) || m_other_cfg.win_score <= 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(dataElement, "loser_score", m_other_cfg.lose_score) || m_other_cfg.lose_score >= m_other_cfg.win_score)
	{
		return -3;
	}

	if (!GetSubNodeValue(dataElement, "tie_score", m_other_cfg.draw_score) || m_other_cfg.draw_score > m_other_cfg.win_score)
	{
		return -4;
	}

	if (!GetSubNodeValue(dataElement, "max_numbers", m_other_cfg.max_limit_round) || m_other_cfg.max_limit_round < 5)
	{
		return -5;
	}

	if (!GetSubNodeValue(dataElement, "give_up_score", m_other_cfg.give_up_score) || m_other_cfg.give_up_score >= m_other_cfg.lose_score)
	{
		return -6;
	}

	if (!GetSubNodeValue(dataElement, "attack_x", m_other_cfg.attacker_posi.x) || m_other_cfg.attacker_posi.x <= 0)
	{
		return -7;
	}

	if (!GetSubNodeValue(dataElement, "attack_y", m_other_cfg.attacker_posi.y) || m_other_cfg.attacker_posi.y <= 0)
	{
		return -8;
	}

	if (!GetSubNodeValue(dataElement, "def_x", m_other_cfg.defender_posi.x) || m_other_cfg.defender_posi.x <= 0)
	{
		return -9;
	}

	if (!GetSubNodeValue(dataElement, "def_y", m_other_cfg.defender_posi.y) || m_other_cfg.defender_posi.y <= 0)
	{
		return -10;
	}

	{
		TiXmlElement *list_element = dataElement->FirstChildElement("day_reward_list");
		if (NULL == list_element)
		{
			return -1000;
		}

		TiXmlElement *item_element = list_element->FirstChildElement("day_reward");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
			{
				return -1002;
			}
			m_other_cfg.day_reward_list.push_back(item);
			item_element = item_element->NextSiblingElement();
		}
	}
	
	{
		TiXmlElement *list_element = dataElement->FirstChildElement("report_reward_list");
		if (NULL == list_element)
		{
			return -2000;
		}

		TiXmlElement *item_element = list_element->FirstChildElement("report_reward");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
			{
				return -2002;
			}
			m_other_cfg.signup_reward_list.push_back(item);
			item_element = item_element->NextSiblingElement();
		}
	}
	

	return 0;
}

int WorldArenaConfig::InitWinBoxRewardList(TiXmlElement *RootElement, std::string& err)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	int last_sn = 0;
	int last_win_num = 0;

	while (dataElement)
	{
		WorldArenaWinBoxRewardCfg cfg;
		if (!GetSubNodeValue(dataElement, "sn", cfg.sn) || last_sn + 1 != cfg.sn)
		{
			err = STRING_SPRINTF("InitWinBoxRewardList [sn] not found or invalid, sn[%d]", cfg.sn);
			return -1;
		}
		last_sn = cfg.sn;

		if (!GetSubNodeValue(dataElement, "victory_times", cfg.win_num) || last_win_num >= cfg.win_num)
		{
			err = STRING_SPRINTF("InitWinBoxRewardList [victory_times] not found or invalid, victory_times[%d]", cfg.win_num);
			return -2;
		}
		last_win_num = cfg.win_num;

		TiXmlElement *list_element = dataElement->FirstChildElement("win_times_list");
		if (NULL == list_element)
		{
			return -1000;
		}

		TiXmlElement *item_element = list_element->FirstChildElement("win_times");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
			{
				return -1002;
			}
			cfg.reward_list.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		m_win_box_reward_list.push_back(cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int WorldArenaConfig::InitSeasonRankRewardList(TiXmlElement* RootElement, std::string& err)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	int last_sn = 0;
	int last_rank = 0;

	while (dataElement)
	{
		WorldArenaSeasonRankRewardCfg cfg;
		if (!GetSubNodeValue(dataElement, "sn", cfg.sn) || last_sn + 1 != cfg.sn)
		{
			err = STRING_SPRINTF("InitSeasonRankRewardList [sn] not found or invalid, sn[%d]", cfg.sn);
			return -1;
		}
		last_sn = cfg.sn;

		if (!GetSubNodeValue(dataElement, "score_rank", cfg.rank) || last_rank >= cfg.rank)
		{
			err = STRING_SPRINTF("InitSeasonRankRewardList [score_rank] not found or invalid, score_rank[%d]", cfg.rank);
			return -2;
		}
		last_rank = cfg.rank;

		TiXmlElement *list_element = dataElement->FirstChildElement("rank_reward_list");
		if (NULL == list_element)
		{
			return -1000;
		}

		TiXmlElement *item_element = list_element->FirstChildElement("rank_reward");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
			{
				return -1002;
			}
			cfg.reward_list.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		m_season_rank_reward_list.push_back(cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int WorldArenaConfig::InitRobotCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	while (dataElement)
	{
		WorldArenaRobotCfg cfg;
		if (!GetSubNodeValue(dataElement, "rob_pro", cfg.prof_base))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "level_start", cfg.level) || cfg.level < 30 || cfg.level > MAX_ROLE_LEVEL)
		{
			return -3;
		}
		
		if (!GetSubNodeValue(dataElement, "attr_adjust_group_1", cfg.attr_adjust_group)
			|| NULL == this->GetRobotAttrAdjustCfg(cfg.attr_adjust_group))
		{
			return -5;
		}
	
		if (!GetSubNodeValue(dataElement, "pets_num", cfg.pet_helper_num) || cfg.pet_helper_num < 0 || cfg.pet_helper_num > 4)
		{
			return -6;
		}

		ProfLevelKey key(cfg.prof_base, cfg.level);
		if (m_prof_level_robot_map.find(key) != m_prof_level_robot_map.end())
		{
			return -7;
		}

		m_prof_level_robot_map.insert(ProfLevelRobotMap::value_type(key, cfg));

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int WorldArenaConfig::InitRobotAttrAdjustCfg(TiXmlElement* RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	int last_group_id = 0;
	m_robot_attr_adjust_cfg_num = 0;
	while (dataElement)
	{
		// 数量超上限
		if (m_robot_attr_adjust_cfg_num >= ARRAY_LENGTH(m_robot_attr_adjust_list))
		{
			return -888;
		}

		int group_id = 0;
		if (!GetSubNodeValue(dataElement, "attr_adjust_group", group_id) || group_id < 0 || group_id != last_group_id + 1)
		{
			return -1;
		}
		last_group_id = group_id;

		WorldArenaRobotAttrAdjustCfg& cfg = m_robot_attr_adjust_list[m_robot_attr_adjust_cfg_num];
		cfg.group_id = group_id;

		static int ADJUST_MIN = -10 * WORLD_ARENA_ROBOT_ATTR_ADJUST_PERCENT;
		static int ADJUST_MAX = 10 * WORLD_ARENA_ROBOT_ATTR_ADJUST_PERCENT;

		if (!GetSubNodeValue(dataElement, "role_gongji_adjust", cfg.role_attack_adjust) || cfg.role_attack_adjust <= ADJUST_MIN || cfg.role_attack_adjust > ADJUST_MAX)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "role_jingshen_adjust", cfg.role_mental_adjust) || cfg.role_mental_adjust <= ADJUST_MIN || cfg.role_mental_adjust > ADJUST_MAX)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "role_huifu_adjust", cfg.role_recover_adjust) || cfg.role_recover_adjust <= ADJUST_MIN || cfg.role_recover_adjust > ADJUST_MAX)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "role_maxhp_adjust", cfg.role_maxhp_adjust) || cfg.role_maxhp_adjust <= ADJUST_MIN || cfg.role_maxhp_adjust > ADJUST_MAX)
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "pet_gongji_adjust", cfg.pet_attack_adjust) || cfg.pet_attack_adjust <= ADJUST_MIN || cfg.pet_attack_adjust > ADJUST_MAX)
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "pet_jingshen_adjust", cfg.pet_mental_adjust) || cfg.pet_mental_adjust <= ADJUST_MIN || cfg.pet_mental_adjust > ADJUST_MAX)
		{
			return -7;
		}

		if (!GetSubNodeValue(dataElement, "pet_huifu_adjust", cfg.pet_recover_adjust) || cfg.pet_recover_adjust <= ADJUST_MIN || cfg.pet_recover_adjust > ADJUST_MAX)
		{
			return -8;
		}

		if (!GetSubNodeValue(dataElement, "pet_maxhp_adjust", cfg.pet_maxhp_adjust) || cfg.pet_maxhp_adjust <= ADJUST_MIN || cfg.pet_maxhp_adjust > ADJUST_MAX)
		{
			return -9;
		}

		if (!GetSubNodeValue(dataElement, "partner_gongji_adjust", cfg.partner_attack_adjust) || cfg.partner_attack_adjust <= ADJUST_MIN || cfg.partner_attack_adjust > ADJUST_MAX)
		{
			return -10;
		}

		if (!GetSubNodeValue(dataElement, "partner_jingshen_adjust", cfg.partner_mental_adjust) || cfg.partner_mental_adjust <= ADJUST_MIN || cfg.partner_mental_adjust > ADJUST_MAX)
		{
			return -11;
		}

		if (!GetSubNodeValue(dataElement, "partner_huifu_adjust", cfg.partner_recover_adjust) || cfg.partner_recover_adjust <= ADJUST_MIN || cfg.partner_recover_adjust > ADJUST_MAX)
		{
			return -12;
		}

		if (!GetSubNodeValue(dataElement, "partner_maxhp_adjust", cfg.partner_maxhp_adjust) || cfg.partner_maxhp_adjust <= ADJUST_MIN || cfg.partner_maxhp_adjust > ADJUST_MAX)
		{
			return -13;
		}

		m_robot_attr_adjust_cfg_num += 1;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

