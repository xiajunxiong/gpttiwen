#include "shituconfig.hpp"

ShiTuConfig::ShiTuConfig()
{
}

ShiTuConfig::~ShiTuConfig()
{
	std::vector< ShiTuRewardConfig>().swap(m_reward_cfg) ;
	std::vector<ShiTuFbMonsterCfg>().swap(m_monster_cfg);
	std::vector<ShiTuFbRewardCfg>().swap(m_fb_reward_cfg);
	std::vector<ShiTuMileCfg>().swap(m_mile_cfg);
	std::vector<ShiTuChuanGongExpCfg>().swap(m_chuan_gong_exp_vec);
	ShiTuChuanGongGradeVec().swap(m_chuan_gong_grade_vec);
}

bool ShiTuConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("mentor_reward", InitLevelRewardCfg);
	LOAD_CONFIG("mentor_fb", InitFbMonsterCfg);
	LOAD_CONFIG("mentor_fb_reward", InitFbRewardCfg);

	LOAD_CONFIG("milepost", InitMileCfg);

	LOAD_CONFIG("other", InitChuanGongOtherCfg);
	LOAD_CONFIG("Passmerit_exp", InitChuanGongExpCfg);
	LOAD_CONFIG("game_reward", InitChuanGongGradeCfg);


	return true;
}

const ShiTuOtherCfg & ShiTuConfig::GetOtherCfg()
{
	return m_other_cfg;
}

const std::vector<ShiTuRewardConfig> & ShiTuConfig::GetLevelRewardItem()
{
	return m_reward_cfg;
}

std::vector<ShiTuFbMonsterItemCfg> ShiTuConfig::GetRandMonsterList(int role_level, int num)
{
	std::vector<ShiTuFbMonsterItemCfg> ret;

	int index = -1;
	for (int i = 0; i < (int)m_monster_cfg.size(); ++i)
	{
		if (m_monster_cfg[i].min_level <= role_level && role_level <= m_monster_cfg[i].max_level)
		{
			index = i;
			break;
		}
	}

	if (index == -1)
	{
		return ret;
	}

	ShiTuFbMonsterCfg & monster_group_cfg = m_monster_cfg[index];

	std::vector<int> rand_vec;
	for (std::map<int, std::vector<ShiTuFbMonsterItemCfg> >::iterator iter = monster_group_cfg.monster_cfg.begin(); iter != monster_group_cfg.monster_cfg.end(); ++iter)
	{
		rand_vec.push_back(iter->first);
	}

	std::random_shuffle(rand_vec.begin(), rand_vec.end());

	for (int i = 0; i < num && i < (int)rand_vec.size(); ++i)
	{
		int group = rand_vec[i];
		ret.insert(ret.end(), monster_group_cfg.monster_cfg[group].begin(), monster_group_cfg.monster_cfg[group].end());
	}

	return ret;
}

const ShiTuFbRewardCfg * ShiTuConfig::GetRewardCfg(int num)
{
	if (num < 0 || num >= (int)m_fb_reward_cfg.size())
	{
		return NULL;
	}

	return &m_fb_reward_cfg[num];
}

const ShiTuMileCfg * ShiTuConfig::GetShiTuMileCfg(int index) const
{
	if (0 > index
		|| (int)m_mile_cfg.size() <= index)
	{
		return NULL;
	}

	return &m_mile_cfg[index];
}

const ShiTuChuanGongOtherCfg & ShiTuConfig::GetChuanGongOtherCfg() const
{
	return m_chuan_gong_other;
}

const ShiTuChuanGongExpCfg * ShiTuConfig::GetChuanGongExpCfg(int role_level) const
{
	if (0 >= role_level
		|| (int)m_chuan_gong_exp_vec.size() <= role_level)
	{
		return NULL;
	}
	return &m_chuan_gong_exp_vec[role_level];
}

const ShiTuChuanGongGradeCfg * ShiTuConfig::GetChuanGongGradeCfg(int grade) const
{
	if (0 > grade
		|| (int)m_chuan_gong_grade_vec.size() <= grade)
	{
		return NULL;
	}

	return &m_chuan_gong_grade_vec[grade];
}

int ShiTuConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "graduate_days", m_other_cfg.graduation_day))
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "graduate_level", m_other_cfg.graduation_level))
	{
		return -111;
	}

	if (!GetSubNodeValue(root_element, "mentor_fb_scene", m_other_cfg.shi_tu_fb_scene_id))
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "min_team_num", m_other_cfg.shi_tu_fb_role_num))
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "mentor_fb_time", m_other_cfg.shi_tu_fb_limit_time))
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "verse_group_num", m_other_cfg.shi_tu_fb_create_monster_num))
	{
		return -5;
	}

	if (!GetSubNodeValue(root_element, "not_online_time", m_other_cfg.force_break_relation_offline_time))
	{
		return -6;
	}

	if (!GetSubNodeValue(root_element, "consume_amount", m_other_cfg.force_break_relation_consume_gold))
	{
		return -7;
	}

	if (!GetSubNodeValue(root_element, "chill_time", m_other_cfg.force_break_relation_time))
	{
		return -8;
	}

	return 0;
}

int ShiTuConfig::InitLevelRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	
	while (NULL != data_element)
	{
		ShiTuRewardConfig cfg;
		if (!GetSubNodeValue(data_element, "seq", cfg.seq) || (int)m_reward_cfg.size() != cfg.seq || cfg.seq >= BitCount<int>())
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "days", cfg.need_day))
		{
			return -100;
		}

		TiXmlElement * master_reward = data_element->FirstChildElement("reward_item1");
		if (!cfg.master_reward.ReadConfig(master_reward))
		{
			return -200;
		}

		TiXmlElement * apprentice_reward = data_element->FirstChildElement("reward_item2");
		if (!cfg.apprentice_reward.ReadConfig(apprentice_reward))
		{
			return -201;
		}

		m_reward_cfg.push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ShiTuConfig::InitFbMonsterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -__LINE__;
	}

	int last_max_role_level = 0;
	while (NULL != data_element)
	{
		int min_level = 0;
		if (!GetSubNodeValue(data_element, "min_level", min_level) || min_level <= 0)
		{
			return -__LINE__;
		}


		int max_level = 0;
		if (!GetSubNodeValue(data_element, "max_level", max_level) || max_level < min_level)
		{
			return -__LINE__;
		}

		if (m_monster_cfg.empty())
		{
			m_monster_cfg.push_back(ShiTuFbMonsterCfg());
		}

		if (last_max_role_level != 0 && last_max_role_level != max_level)
		{
			if (min_level != last_max_role_level + 1)
			{
				return -__LINE__;
			}
			m_monster_cfg.push_back(ShiTuFbMonsterCfg());
		}

		ShiTuFbMonsterCfg & cfg = m_monster_cfg.back();
		cfg.min_level = min_level;
		cfg.max_level = max_level;
		last_max_role_level = max_level;

		ShiTuFbMonsterItemCfg item_cfg;

		if (!GetSubNodeValue(data_element, "verse_group", item_cfg.group))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "is_down", item_cfg.index))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "is_down", item_cfg.index) || item_cfg.index != (int)cfg.monster_cfg[item_cfg.group].size())
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "npc_id", item_cfg.monster_id))
		{
			return -__LINE__;
		}

		cfg.monster_cfg[item_cfg.group].push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ShiTuConfig::InitFbRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -__LINE__;
	}


	while (NULL != data_element)
	{
		ShiTuFbRewardCfg cfg;

		if (!GetSubNodeValue(data_element, "master_coin", cfg.master_coin) || cfg.master_coin < 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "master_chivalrous", cfg.master_xia_yi_zhi) || cfg.master_xia_yi_zhi < 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "apprentice_coin", cfg.tu_id_coin) || cfg.tu_id_coin < 0)
		{
			return -__LINE__;
		}

		{
			TiXmlElement * reward_list_element = data_element->FirstChildElement("master_reward_list");
			if (reward_list_element != NULL)
			{
				TiXmlElement * reward_element = reward_list_element->FirstChildElement("master_reward");

				while (reward_element)
				{
					ItemConfigData reward;

					if (!reward.ReadConfig(reward_element))
					{
						return -__LINE__;
					}

					cfg.master_reward.push_back(reward);
					reward_element = reward_element->NextSiblingElement();
				}
			}
		}

		{
			TiXmlElement * reward_list_element = data_element->FirstChildElement("apprentice_reward_list");
			if (reward_list_element != NULL)
			{
				TiXmlElement * reward_element = reward_list_element->FirstChildElement("apprentice_reward");

				while (reward_element)
				{
					ItemConfigData reward;

					if (!reward.ReadConfig(reward_element))
					{
						return -__LINE__;
					}

					cfg.tu_di_reward.push_back(reward);
					reward_element = reward_element->NextSiblingElement();
				}
			}
		}

		m_fb_reward_cfg.push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ShiTuConfig::InitMileCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		int index = 0;
		if (!GetSubNodeValue(data_element, "seq", index)
			|| (int)m_mile_cfg.size() != index
			|| 60 < index)
		{
			return -1;
		}

		m_mile_cfg.push_back(ShiTuMileCfg());
		ShiTuMileCfg &cfg_info = m_mile_cfg[index];
		cfg_info.index = index;

		if (!GetSubNodeValue(data_element, "type", cfg_info.type)
			|| 0 >= cfg_info.type
			|| SHI_TU_MILE_TYPE_MAX <= cfg_info.type)
		{
			return -2;
		}
		if (!GetSubNodeValue(data_element, "param", cfg_info.param)
			|| 0 > cfg_info.param)
		{
			return -3;
		}
	
		LOAD_ITEM_VETOR_NODE("apprentice_reward_list", "apprentice_reward", cfg_info.apprentice_reward);
		LOAD_ITEM_VETOR_NODE("master_reward_list", "master_reward", cfg_info.master_reward);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ShiTuConfig::InitChuanGongOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "passmerit_scene", m_chuan_gong_other.scene_id))
	{
		return -1;
	}


	if (!GetSubNodeValue(root_element, "passmerit_coordinate_x", m_chuan_gong_other.born_p.x))
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "passmerit_coordinate_y", m_chuan_gong_other.born_p.y))
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "passmerit_ready", m_chuan_gong_other.game_ready_time))
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "passmerit_game_time", m_chuan_gong_other.game_time))
	{
		return -5;
	}

	if (!GetSubNodeValue(root_element, "passmerit_exp_frequency", m_chuan_gong_other.week_apprentice_exp_times))
	{
		return -6;
	}

	if (!GetSubNodeValue(root_element, "milepost_limit", m_chuan_gong_other.day_mile_fetch_num))
	{
		return -7;
	}
	
	return 0;
}

int ShiTuConfig::InitChuanGongExpCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	//²åÈë0¼¶
	m_chuan_gong_exp_vec.push_back(ShiTuChuanGongExpCfg());
	while (NULL != root_element)
	{
		int level = 0;
		if (!GetSubNodeValue(root_element, "level", level)
			|| (int)m_chuan_gong_exp_vec.size() != level)
		{
			return -1;
		}

		ShiTuChuanGongExpCfg cfg_info;
		cfg_info.level = level;

		if (!GetSubNodeValue(root_element, "reward_exp", cfg_info.reward_exp)
			|| 0 > cfg_info.reward_exp)
		{
			return -2;
		}

		m_chuan_gong_exp_vec.push_back(cfg_info);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ShiTuConfig::InitChuanGongGradeCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		ShiTuChuanGongGradeCfg cfg_info;

		if (!GetSubNodeValue(data_element, "grade", cfg_info.grade)
			|| (int)m_chuan_gong_grade_vec.size() != cfg_info.grade)
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "score", cfg_info.score_begin)
			|| 0 > cfg_info.score_begin)
		{
			return -2;
		}

		LOAD_ITEM_VETOR_NODE("master_reward_list", "master_reward", cfg_info.master_reward_vec);
		LOAD_ITEM_VETOR_NODE("apprentice_reward_list", "apprentice_reward", cfg_info.apprentice_reward_vec);

		m_chuan_gong_grade_vec.push_back(cfg_info);

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
