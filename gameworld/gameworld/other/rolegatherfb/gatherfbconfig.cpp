#include <set>

#include "checkresourcecenter.hpp"
#include "other/rolegatherfb/gatherfbconfig.hpp"

RoleGatherFbConfig::RoleGatherFbConfig()
{
}

RoleGatherFbConfig::~RoleGatherFbConfig()
{
	std::vector<RoleGatherFbLevelCfg>().swap(m_level_cfg);
	for (std::map<int, std::vector<GatherFbRewardGroupCfg> >::iterator it = m_gather_reward_group_cfg.begin(); it != m_gather_reward_group_cfg.end(); it++)
	{
		std::vector<GatherFbRewardGroupCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<GatherFbSpecialRewardGroupCfg> >::iterator it = m_special_reward_group_cfg.begin(); it != m_special_reward_group_cfg.end(); it++)
	{
		std::vector<GatherFbSpecialRewardGroupCfg>().swap(it->second);
	}
}

bool RoleGatherFbConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("reward_group", InitRewardGroupCfg);	//需要放在场景配置前
	LOAD_CONFIG("gather_scene", InitLevelCfg);
	LOAD_CONFIG("gather_pos", InitPosCfg);
	LOAD_CONFIG("field_collect", InitfieldCfg);
	LOAD_CONFIG_2("gather_item_group", InitSpecialRewardGroupCfg);
	LOAD_CONFIG_2("skill_level_and_gather", InitSkillLevelCfg);		//技能等级关联配置读取需放在特殊奖励组配置读取之后

	return true;
}

const RoleGatherFbLevelCfg * RoleGatherFbConfig::GetLevelCfg(int seq)
{
	if (seq < 0 || seq >= (int)m_level_cfg.size())
	{
		return NULL;
	}

	return &m_level_cfg[seq];
}

const RoleGatherFbLevelCfg * RoleGatherFbConfig::GetLevelCfgBySceneId(int scene_id)
{
	for (int i = 0; i < (int)m_level_cfg.size(); ++i)
	{
		if (scene_id == m_level_cfg[i].scene_id)
		{
			return &m_level_cfg[i];
		}
	}

	return NULL;
}

const RoleGatherFbPosCfg * RoleGatherFbConfig::GetPosCfg(int pos_group_id)
{
	std::map<int, RoleGatherFbPosCfg>::iterator iter = m_pos_cfg.find(pos_group_id);

	if (iter == m_pos_cfg.end())
	{
		return NULL;
	}

	return &iter->second;
}

const RoleGatherFbOtherCfg & RoleGatherFbConfig::GetOtherCfg()
{
	return m_other_cfg;
}

const FieldGatherCfg * RoleGatherFbConfig::GetFieldGatherCfg(int seq)
{
	std::map<int, FieldGatherCfg>::iterator it = m_field_cfg.find(seq);
	if (it != m_field_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}

const GatherFbRewardGroupCfg * RoleGatherFbConfig::GetRewardGroupRandCfg(int reward_group_id)
{
	std::map<int, std::vector<GatherFbRewardGroupCfg> >::iterator group_it = m_gather_reward_group_cfg.find(reward_group_id);
	if(group_it == m_gather_reward_group_cfg.end() || group_it->second.empty()) return NULL;

	int rate_num = RandomNum(GatherFbRewardGroupCfg::MAX_GATHER_REWARD_RATE_COUNT);
	for (int i = 0; i < (int)group_it->second.size(); i++)
	{	
		if (rate_num < group_it->second[i].rate)
		{
			return &group_it->second[i];
		}
		rate_num -= group_it->second[i].rate;
	}

	return NULL;
}

const GatherFbSkillLevelCfg * RoleGatherFbConfig::GetSkillLevelCfg(int scene_id, int skill_level)
{
	std::map<int, std::map<int, GatherFbSkillLevelCfg> >::const_iterator scene_it = m_skill_level_cfg.find(scene_id);
	if(scene_it == m_skill_level_cfg.end() || scene_it->second.empty()) return NULL;

	for (std::map<int, GatherFbSkillLevelCfg>::const_reverse_iterator level_it = scene_it->second.rbegin(); level_it != scene_it->second.rend(); ++level_it)
	{	
		if (skill_level >= level_it->first)
		{
			return &level_it->second;
		}
	}

	return NULL;
}

const GatherFbSpecialRewardGroupCfg * RoleGatherFbConfig::GetRandSpecialRewardCfg(int special_reward_group_id)
{
	std::map<int, std::vector<GatherFbSpecialRewardGroupCfg> >::iterator group_it = m_special_reward_group_cfg.find(special_reward_group_id);
	if (group_it == m_special_reward_group_cfg.end() || group_it->second.empty()) return NULL;

	int rate_num = RandomNum(GatherFbSpecialRewardGroupCfg::MAX_SPECIAL_REWARD_RATE_COUNT);
	for (int i = 0; i < (int)group_it->second.size(); i++)
	{
		if (rate_num < group_it->second[i].rate)
		{
			return &group_it->second[i];
		}
		rate_num -= group_it->second[i].rate;
	}

	return NULL;
}

int RoleGatherFbConfig::InitLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");

	std::set<int> check_repeat_scene_id;

	int next_seq = 0;
	while (data_element != NULL)
	{
		RoleGatherFbLevelCfg cfg;
		if (!GetSubNodeValue(data_element, "seq", cfg.seq) || cfg.seq != next_seq)
		{
			return -1;
		}
		next_seq += 1;

		if (!GetSubNodeValue(data_element, "scene_id", cfg.scene_id))
		{
			return -2;
		}

		if (check_repeat_scene_id.find(cfg.scene_id) != check_repeat_scene_id.end())
		{
			return -10800;
		}

		check_repeat_scene_id.insert(cfg.scene_id);


		if (!GetSubNodeValue(data_element, "pos_group", cfg.pos_group_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "levels", cfg.need_role_level))
		{
			return -4;
		}

		if (!GetSubNodeValue(data_element, "x", cfg.birth_pos.x) || cfg.birth_pos.x < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(data_element, "y", cfg.birth_pos.y) || cfg.birth_pos.y < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(data_element, "gather_id", cfg.gather_id))
		{
			return -7;
		}

		if (!GetSubNodeValue(data_element, "limit_skill", cfg.need_skill_type))
		{
			return -8;
		}

		if (!GetSubNodeValue(data_element, "limit_skill_level", cfg.need_skill_level))
		{
			return -9;
		}

		if (!GetSubNodeValue(data_element, "reward_group", cfg.reward_group_id) || 
			(0 != cfg.reward_group_id && m_gather_reward_group_cfg.end() == m_gather_reward_group_cfg.find(cfg.reward_group_id)))
		{
			return -10;
		}

		if (!GetSubNodeValue(data_element, "expend_active", cfg.expend_active) || cfg.expend_active <= 0)
		{
			return -11;
		}

		m_level_cfg.push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RoleGatherFbConfig::InitPosCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");

	while (data_element != NULL)
	{
		int group_id = 0;
		if (!GetSubNodeValue(data_element, "pos_group", group_id))
		{
			return -1;
		}
		RoleGatherFbPosCfg & pos_cfg = m_pos_cfg[group_id];

		Posi pos;
		if (!GetSubNodeValue(data_element, "pos_x", pos.x)|| pos.x<0)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "pos_y", pos.y) || pos.y < 0)
		{
			return -3;
		}

		pos_cfg.create_pos.push_back(pos);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RoleGatherFbConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");

	if (!GetSubNodeValue(data_element, "time_second", m_other_cfg.create_interval))
	{
		return -1;
	}

	if (!GetSubNodeValue(data_element, "power_max", m_other_cfg.gather_power_max))
	{
		return -2;
	}

	if (!GetSubNodeValue(data_element, "power_expend", m_other_cfg.gather_comsume_power))
	{
		return -3;
	}

	if (!GetSubNodeValue(data_element, "get_everyday", m_other_cfg.every_add_gather_power_value))
	{
		return -4;
	}

	if (!GetSubNodeValue(data_element, "one_way_time", m_other_cfg.one_way_time) || m_other_cfg.one_way_time <= 0)
	{
		return -5;
	}
	m_other_cfg.one_way_time *= 3;	//钓鱼玩法完成最少所需时间

	return 0;
}

int RoleGatherFbConfig::InitfieldCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");

	while (data_element != NULL)
	{
		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq))
		{
			return -1;
		}
		FieldGatherCfg & pos_cfg = m_field_cfg[seq];

		if (!GetSubNodeValue(data_element, "time_second", pos_cfg.time_second) || pos_cfg.time_second < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "min_num", pos_cfg.min_num) || pos_cfg.min_num < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "max_num", pos_cfg.max_num) || pos_cfg.max_num < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(data_element, "scene_id", pos_cfg.scene_id) || pos_cfg.scene_id < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(data_element, "gather_id", pos_cfg.gather_id) || pos_cfg.gather_id < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(data_element, "pos_group", pos_cfg.pos_group) || pos_cfg.pos_group < 0)
		{
			return -7;
		}

		if (!GetSubNodeValue(data_element, "scene_upper_limit", pos_cfg.limit_num) || pos_cfg.limit_num < 0)
		{
			return -8;
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RoleGatherFbConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if(NULL == data_element)
	{
		return -999;
	}

	while (data_element != NULL)
	{
		int reward_group_id = 0;
		if (!GetSubNodeValue(data_element, "reward", reward_group_id)) return -1;

		GatherFbRewardGroupCfg node_cfg;
		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate <= 0 || node_cfg.rate > GatherFbRewardGroupCfg::MAX_GATHER_REWARD_RATE_COUNT) return -2;

		TiXmlElement * item_element = data_element->FirstChildElement("item");
		if (NULL == item_element)
		{
			return -4;
		}
		
		if (!node_cfg.item.ReadConfig(item_element))
		{
			return -5;
		}

		m_gather_reward_group_cfg[reward_group_id].push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}

	for (std::map<int, std::vector<GatherFbRewardGroupCfg> >::iterator group_it = m_gather_reward_group_cfg.begin(); group_it != m_gather_reward_group_cfg.end(); ++group_it)
	{
		int rate_count = 0;
		for (int i = 0; i < (int)group_it->second.size(); i++)
		{
			rate_count += group_it->second[i].rate;
		}
		if (rate_count != GatherFbRewardGroupCfg::MAX_GATHER_REWARD_RATE_COUNT)
		{
			return -66;
		}
	}

	return 0;
}

int RoleGatherFbConfig::InitSkillLevelCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	
	while (NULL != root_element)
	{
		int scene_id = 0, life_skill_level = 0;
		if (!GetSubNodeValue(root_element, "scene_id", scene_id))
		{
			errormsg = STRING_SPRINTF("[scene_id] not find data?");
			return -1;
		}
		CheckResourceCenter::Instance().GetSceneCheck()->Add(scene_id, __FUNCTION__);
		if (!GetSubNodeValue(root_element, "gather_skill_level", life_skill_level) || life_skill_level <= 0)
		{
			errormsg = STRING_SPRINTF("[scene_id:%d] [gather_skill_level:%d] not find data or <= 0?", scene_id, life_skill_level);
			return -2;
		}

		std::map<int, std::map<int, GatherFbSkillLevelCfg> >::iterator it = m_skill_level_cfg.find(scene_id);
		if (it != m_skill_level_cfg.end() && it->second.end() != it->second.find(life_skill_level))
		{
			errormsg = STRING_SPRINTF("[scene_id:%d] [gather_skill_level:%d] repeat?", scene_id, life_skill_level);
			return -3;
		}

		GatherFbSkillLevelCfg node_cfg;
		if (!GetSubNodeValue(root_element, "multiple", node_cfg.multiple) || node_cfg.multiple <= 0)
		{
			errormsg = STRING_SPRINTF("[scene_id:%d] [gather_skill_level:%d], multiple[%d] not find data or <= 0?", scene_id, life_skill_level, node_cfg.multiple);
			return -4;
		}
		if (!GetSubNodeValue(root_element, "spcial_reward_group", node_cfg.special_reward_group) || 
			(0 != node_cfg.special_reward_group && m_special_reward_group_cfg.end() == m_special_reward_group_cfg.find(node_cfg.special_reward_group)))
		{
			errormsg = STRING_SPRINTF("[scene_id:%d] [gather_skill_level:%d], spcial_reward_group[%d] not find data or not [gather_item_group] page find?", scene_id, life_skill_level, node_cfg.special_reward_group);
			return -4;
		}

		m_skill_level_cfg[scene_id][life_skill_level] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleGatherFbConfig::InitSpecialRewardGroupCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (data_element != NULL)
	{
		int reward_group_id = 0;
		if (!GetSubNodeValue(data_element, "reward_group", reward_group_id))
		{
			errormsg = STRING_SPRINTF("[reward_group] not find data?");
			return -1;
		}

		GatherFbSpecialRewardGroupCfg node_cfg;
		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate <= 0 || node_cfg.rate > GatherFbSpecialRewardGroupCfg::MAX_SPECIAL_REWARD_RATE_COUNT) 
		{
			errormsg = STRING_SPRINTF("[reward_group:%d], [rate:%d] not find data or <= 0 or > %d?", reward_group_id, node_cfg.rate, GatherFbSpecialRewardGroupCfg::MAX_SPECIAL_REWARD_RATE_COUNT);
			return -2;
		}
		TiXmlElement * item_element = data_element->FirstChildElement("item");
		if (NULL == item_element)
		{
			errormsg = STRING_SPRINTF("[reward_group:%d], [item] not find data", reward_group_id);
			return -3;
		}

		if (!node_cfg.reward.ReadConfig(item_element))
		{
			errormsg = STRING_SPRINTF("[reward_group:%d, rate:%d] item[%d, %d]?", reward_group_id, node_cfg.rate, node_cfg.reward.item_id, node_cfg.reward.num);
			return -4;
		}

		m_special_reward_group_cfg[reward_group_id].push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
