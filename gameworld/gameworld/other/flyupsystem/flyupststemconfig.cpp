#include "flyupststemconfig.hpp"
#include "item/itempool.h"
#include "monster/monster_group_pool.hpp"
#include "item/itembase.h"
#include "task/taskpool.h"

FlyUpSystemConfig::FlyUpSystemConfig()
{
}

FlyUpSystemConfig::~FlyUpSystemConfig()
{
	std::vector<FlyUpShiLianCfg>().swap(m_shilian_vec);
	std::vector<FlyUpSkillUpLevelCfg>().swap(skill_uplevel_vec);
}

bool FlyUpSystemConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("other", InitOtherCfg);
	LOAD_CONFIG_2("shilian_attributes", InitShiLianAttrCfg);
	LOAD_CONFIG_2("feisheng_attributes", InitFeiShengAttrCfg);
	LOAD_CONFIG_2("skill_lock", InitSkillCfg);
	LOAD_CONFIG_2("skill_up", InitSkillUpCfg);
	LOAD_CONFIG_2("feisheng_task", InitFlyUpTaskCfg);
	LOAD_CONFIG_2("task_reward", InitFlyUpTaskRewardCfg);
	LOAD_CONFIG_2("reward_group", InitFlyUpTaskRewardGroupCfg);
	LOAD_CONFIG_2("last_reward", InitFlyUpTaskLastRewardCfg);

	return true;
}

const FlyUpShiLianCfg * FlyUpSystemConfig::GetShiLianCfg(int stag, int prof, int element)
{
	if (stag < 0 || stag >= FlyUpSystemParam::FLY_UP_STAG_MAX)return NULL;
	if (prof < 0 || prof >= PROFESSION_TYPE_NUM)return NULL;
	if (element < 0 || element >= FlyUpSystemParam::FLY_UP_ORGAN_MAX)return NULL;

	return shilian_list[stag][prof][element];
}

const FlyUpAttrCfg * FlyUpSystemConfig::GetAttrCfg(int stag)
{
	if (stag < 0 || stag >= FlyUpSystemParam::FLY_UP_STAG_MAX)return NULL;
	return &fly_up_attr_cfg[stag];
}

const FlyUpSkillCfg * FlyUpSystemConfig::GetSkillCfg(int prof)
{
	if (prof < 0 || prof >= PROFESSION_TYPE_NUM)return NULL;
	return &skill_list[prof];
}

const FlyUpSkillUpLevelCfg * FlyUpSystemConfig::GetSkillUplevelCfg(int skill_level)
{
	for (std::vector<FlyUpSkillUpLevelCfg>::iterator it = skill_uplevel_vec.begin(); it != skill_uplevel_vec.end(); it++)
	{
		if (it->skill_level == skill_level)
		{
			return &(*it);
		}
	}
	return NULL;
}

const FlyUpTaskCfg * FlyUpSystemConfig::RandTask(int stag, int is_shilian, int pre_task_id)
{
	if (stag < 0 || stag >= FlyUpSystemParam::FLY_UP_STAG_MAX)return NULL;
	if (is_shilian < 0 || is_shilian >= FlyUpSystemParam::FLY_UP_SHILIAN_MAX)return NULL;
	FlyUpTaskListCfg &task_list = fly_task_list[stag][is_shilian];

	if (task_list.weigth > 0)
	{
		std::vector<FlyUpTaskCfg*> rand_vec;
		int rand_weight = task_list.weigth;
		//rand_vec.assign(task_list.task_vec.begin(), task_list.task_vec.end());
		for (std::vector<FlyUpTaskCfg>::iterator it = task_list.task_vec.begin(); it != task_list.task_vec.end(); it++)
		{
			rand_vec.push_back(&(*it));
		}

		if (pre_task_id > 0)//跳过前一个任务
		{
			for (std::vector<FlyUpTaskCfg*>::iterator it = rand_vec.begin(); it != rand_vec.end(); it++)
			{
				if ((*it)->task_id == pre_task_id)
				{
					rand_weight -= (*it)->rate;
					rand_vec.erase(it);
					break;
				}
			}
		}
		int rand_weight2 = rand() % rand_weight;
		for (std::vector<FlyUpTaskCfg*>::iterator it2 = rand_vec.begin(); it2 != rand_vec.end(); it2++)
		{
			if ((*it2)->rate >= rand_weight2)
			{
				return *it2;
			}
			rand_weight2 -= (*it2)->rate;
		}
	}

	return NULL;
}

const FlyUpTaskRewardCfg * FlyUpSystemConfig::GetTaskRewardCfg(int stag, int is_shilian, int task_idx)
{
	if (stag < 0 || stag >= FlyUpSystemParam::FLY_UP_STAG_MAX)return NULL;
	if (is_shilian < 0 || is_shilian >= FlyUpSystemParam::FLY_UP_SHILIAN_MAX)return NULL;
	if (task_idx < 0 || task_idx >= MAX_FLY_UP_TASK_IDX)return NULL;

	return &fly_task_reward_list[stag][is_shilian][task_idx];
}

const FlyUpTaskRewardGroupCfg * FlyUpSystemConfig::RandTaskRewardCfg(int reward_id)
{
	std::map<int, FlyUpTaskRewardGroupListCfg>::iterator it = m_task_reward_map.find(reward_id);
	if (it != m_task_reward_map.end())
	{
		if ((int)it->second.m_reward_vec.size() > 0 && it->second.weight > 0)
		{
			int rand_num = rand() % it->second.weight;
			for (std::vector<FlyUpTaskRewardGroupCfg>::iterator it2 = it->second.m_reward_vec.begin(); it2 != it->second.m_reward_vec.end(); it2++)
			{
				if (it2->rate >= rand_num)
				{
					return &(*it2);
				}
				rand_num -= it2->rate;
			}
		}
	}
	return NULL;
}

const FlyUpFinishRewardCfg * FlyUpSystemConfig::GetTaskFinishRewardCfg(int stag, int is_shilian)
{
	if (stag < 0 || stag >= FlyUpSystemParam::FLY_UP_STAG_MAX)return NULL;
	if (is_shilian < 0 || is_shilian >= FlyUpSystemParam::FLY_UP_SHILIAN_MAX)return NULL;

	return &finish_reward_list[stag][is_shilian];
}

int FlyUpSystemConfig::InitOtherCfg(TiXmlElement *RootElement, std::string& errormsg)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)return -999;
	
	if (!GetSubNodeValue(dataElement, "level_limit1", m_other_cfg.level_limit1) || m_other_cfg.level_limit1 < 0 || m_other_cfg.level_limit1 > MAX_ROLE_LEVEL)
	{
		errormsg = STRING_SPRINTF("not data level_limit1 or level_limit1 < 0 or level_limit1 > 120");
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "level_limit2", m_other_cfg.level_limit2) || m_other_cfg.level_limit2 < 0 || m_other_cfg.level_limit2 > MAX_ROLE_LEVEL)
	{
		errormsg = STRING_SPRINTF("not data level_limit2 or level_limit2 < 0 or level_limit2 > 120");
		return -2;
	}

	if (!GetSubNodeValue(dataElement, "use_limit_up", m_other_cfg.use_limit_up) || m_other_cfg.use_limit_up < 0)
	{
		errormsg = STRING_SPRINTF("not data use_limit_up or use_limit_up < 0");
		return -3;
	}

	if (!GetSubNodeValue(dataElement, "feishengtasklist_id", m_other_cfg.feishengtasklist_id) || NULL == TASKPOOL->GetTaskCfgByID(m_other_cfg.feishengtasklist_id))
	{
		errormsg = STRING_SPRINTF("not data feishengtasklist_id or feishengtasklist_id is NULL task_id");
		return -4;
	}

	if (!GetSubNodeValue(dataElement, "feisheng_title1", m_other_cfg.feisheng_title1))
	{
		errormsg = STRING_SPRINTF("not data feisheng_title1");
		return -5;
	}

	if (!GetSubNodeValue(dataElement, "feisheng_title2", m_other_cfg.feisheng_title2))
	{
		errormsg = STRING_SPRINTF("not data feisheng_title2");
		return -6;
	}

	if (!GetSubNodeValue(dataElement, "max_exp1", m_other_cfg.max_exp1))
	{
		errormsg = STRING_SPRINTF("not data max_exp1");
		return -7;
	}

	if (!GetSubNodeValue(dataElement, "max_exp2", m_other_cfg.max_exp2))
	{
		errormsg = STRING_SPRINTF("not data max_exp2");
		return -8;
	}

	if (!GetSubNodeValue(dataElement, "add_exp", m_other_cfg.add_exp) || m_other_cfg.add_exp <= 0)
	{
		errormsg = STRING_SPRINTF("not data add_exp OR add_exp <= 0");
		return -9;
	}

	if (!GetSubNodeValue(dataElement, "score_limit1", m_other_cfg.score_limit1) || m_other_cfg.score_limit1 <= 0)
	{
		errormsg = STRING_SPRINTF("not data score_limit1 OR score_limit1 <= 0");
		return -10;
	}

	if (!GetSubNodeValue(dataElement, "score_limit2", m_other_cfg.score_limit2) || m_other_cfg.score_limit2 <= 0)
	{
		errormsg = STRING_SPRINTF("not data score_limit2 OR score_limit2 <= 0");
		return -11;
	}

	if (!GetSubNodeValue(dataElement, "feishengtasklist_id2", m_other_cfg.feishengtasklist_id2) || m_other_cfg.feishengtasklist_id2 <= 0)
	{
		errormsg = STRING_SPRINTF("not data feishengtasklist_id2 OR feishengtasklist_id2 <= 0");
		return -11;
	}

	if (!GetSubNodeValue(dataElement, "check_feishengtask", m_other_cfg.check_feishengtask) || m_other_cfg.check_feishengtask <= 0)
	{
		errormsg = STRING_SPRINTF("not data feishengtasklist_id2 OR check_feishengtask <= 0");
		return -12;
	}

	if (!GetSubNodeValue(dataElement, "equipmentdh_task", m_other_cfg.equipmentdh_task) || m_other_cfg.equipmentdh_task <= 0)
	{
		errormsg = STRING_SPRINTF("not data equipmentdh_task OR equipmentdh_task <= 0");
		return -13;
	}

	return 0;
}

int FlyUpSystemConfig::InitShiLianAttrCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	memset(shilian_list, NULL, sizeof(shilian_list));
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)return -999;

	while (NULL != dataElement)
	{
		FlyUpShiLianCfg node;

		if (!GetSubNodeValue(dataElement, "seq", node.seq) || node.seq < FlyUpSystemParam::FLY_UP_STAG_1 || node.seq > FlyUpSystemParam::FLY_UP_STAG_2)
		{
			errormsg = STRING_SPRINTF("not data seq OR seq < FLY_UP_STAG_1 OR seq > FLY_UP_STAG_2");
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "type_jobs", node.type_jobs) || node.type_jobs < 0 || node.type_jobs >= PROFESSION_TYPE_NUM)
		{
			errormsg = STRING_SPRINTF("not data type_jobs OR type_jobs < 0 OR type_jobs > %d", PROFESSION_TYPE_NUM);
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "sub_type", node.sub_type) || node.sub_type < 0 || node.sub_type > FlyUpSystemParam::FLY_UP_ORGAN_MAX)
		{
			errormsg = STRING_SPRINTF("not data sub_type OR sub_type < 0 OR sub_type > %d", FlyUpSystemParam::FLY_UP_ORGAN_MAX);
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "stuff_id", node.stuff_id) || NULL == ITEMPOOL->GetItem(node.stuff_id))
		{
			errormsg = STRING_SPRINTF("not data stuff_id OR sub_type is NULL item_id");
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "stuff_num", node.stuff_num) || node.stuff_num <= 0)
		{
			errormsg = STRING_SPRINTF("not data stuff_num OR stuff_num <= 0");
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "attr_min", node.attr_min) || node.attr_min <= 0)
		{
			errormsg = STRING_SPRINTF("not data attr_min OR attr_min <= 0");
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "attr_max", node.attr_max) || node.attr_max <= 0 || node.attr_max < node.attr_min)
		{
			errormsg = STRING_SPRINTF("not data attr_max OR attr_max <= 0 OR attr_max < attr_min");
			return -7;
		}

		if (!GetSubNodeValue(dataElement, "attr_type", node.attr_type) || node.attr_type < 0 || node.attr_type >= BATTLE_ATTR_MAX)
		{
			errormsg = STRING_SPRINTF("not data attr_type OR attr_type <= 0 OR attr_type >= %d", BATTLE_ATTR_MAX);
			return -9;
		}

		if (!GetSubNodeValue(dataElement, "attr_val", node.attr_val) || node.attr_val <= 0)
		{
			errormsg = STRING_SPRINTF("not data attr_val OR attr_val <= 0");
			return -10;
		}

		m_shilian_vec.push_back(node);

		dataElement = dataElement->NextSiblingElement();
	}

	for (std::vector<FlyUpShiLianCfg>::iterator it = m_shilian_vec.begin(); it != m_shilian_vec.end(); it++)
	{
		if (it->seq >= FlyUpSystemParam::FLY_UP_STAG_1 && it->seq <= FlyUpSystemParam::FLY_UP_STAG_2)
		{
			if (it->type_jobs >= 0 && it->type_jobs < PROFESSION_TYPE_NUM)
			{
				if (it->sub_type - 1 >= 0 && it->sub_type - 1 < FlyUpSystemParam::FLY_UP_ORGAN_MAX)
				{
					shilian_list[it->seq][it->type_jobs][it->sub_type - 1] = &(*it);
				}
			}
		}
	}

	return 0;
}

int FlyUpSystemConfig::InitFeiShengAttrCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)return -999;

	char attr_name[FlyUpAttrCfg::FLY_UP_ATTR_TYPE_MAX][20] = { "maxhp","maxmp","gongji","fangyu","minjie","jingshen" ,"huifu" };

	while (NULL != dataElement)
	{
		int shilian_type = 0;
		if (!GetSubNodeValue(dataElement, "shilian_type", shilian_type) || shilian_type < FlyUpSystemParam::FLY_UP_STAG_1 || shilian_type > FlyUpSystemParam::FLY_UP_STAG_2)
		{
			errormsg = STRING_SPRINTF("not data shilian_type OR shilian_type < %d OR shilian_type > %d", FlyUpSystemParam::FLY_UP_STAG_1, FlyUpSystemParam::FLY_UP_STAG_2);
			return -1;
		}

		FlyUpAttrCfg &node = fly_up_attr_cfg[shilian_type];
		node.shilian_type = shilian_type;

		if (!GetSubNodeValue(dataElement, "hp_buff", node.hp_buff) || node.hp_buff < 0)
		{
			errormsg = STRING_SPRINTF("not data hp_buff");
			return -2;
		}
		
		for (int i = 0; i < FlyUpAttrCfg::FLY_UP_ATTR_TYPE_MAX; i++)
		{
			int value = 0;
			if (!GetSubNodeValue(dataElement, attr_name[i], value))
			{
				errormsg = STRING_SPRINTF("not data %s", attr_name[i]);
				return -2;
			}
			node.attr_value[i] = value;
		}

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int FlyUpSystemConfig::InitSkillCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)return -999;
	while (NULL != dataElement)
	{
		int type_jobs = 0;
		if (!GetSubNodeValue(dataElement, "type_jobs", type_jobs) || type_jobs < 0 || type_jobs >= PROFESSION_TYPE_NUM)
		{
			errormsg = STRING_SPRINTF("not data type_jobs OR type_jobs < 0 OR type_jobs >= %d", PROFESSION_TYPE_NUM);
			return -1;
		}

		FlyUpSkillCfg &node = skill_list[type_jobs];
		node.type_jobs = type_jobs;

		if (!GetSubNodeValue(dataElement, "ascension_skills", node.ascension_skills) || node.ascension_skills < 0)
		{
			errormsg = STRING_SPRINTF("not data ascension_skills OR ascension_skills < 0");
			return -2;
		}

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int FlyUpSystemConfig::InitSkillUpCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)return -999;
	while (NULL != dataElement)
	{
		FlyUpSkillUpLevelCfg node;
		if (!GetSubNodeValue(dataElement, "skill_level", node.skill_level))
		{
			errormsg = STRING_SPRINTF("not data skill_level");
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "role_level", node.role_level) || node.role_level < 0 || node.role_level > MAX_ROLE_LEVEL)
		{
			errormsg = STRING_SPRINTF("not data role_level OR role_level < 0 || role_level > %d", MAX_ROLE_LEVEL);
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "item_id", node.item_id) || NULL == ITEMPOOL->GetItem(node.item_id))
		{
			errormsg = STRING_SPRINTF("not data item_id OR item_id is NULL item_id");
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "consume", node.consume) || node.consume <= 0)
		{
			errormsg = STRING_SPRINTF("not data consume OR consume < 0");
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "need_exp", node.need_exp) || node.need_exp < 0)
		{
			errormsg = STRING_SPRINTF("not data need_exp OR need_exp < 0");
			return -5;
		}

		skill_uplevel_vec.push_back(node);
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int FlyUpSystemConfig::InitFlyUpTaskCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)return -999;
	while (NULL != dataElement)
	{
		int stag = 0;
		if (!GetSubNodeValue(dataElement, "stag", stag) || stag < 0 || stag >= FlyUpSystemParam::FLY_UP_STAG_MAX)
		{
			errormsg = STRING_SPRINTF("not data stag OR stag < 0 OR stag >= %d", FlyUpSystemParam::FLY_UP_STAG_MAX);
			return -1;
		}
		int is_shilian = 0;
		if (!GetSubNodeValue(dataElement, "is_shilian", is_shilian) || is_shilian < 0 || is_shilian >= FlyUpSystemParam::FLY_UP_SHILIAN_MAX)
		{
			errormsg = STRING_SPRINTF("not data is_shilian OR is_shilian < 0 OR is_shilian >= %d", FlyUpSystemParam::FLY_UP_SHILIAN_MAX);
			return -99;
		}

		FlyUpTaskListCfg &node_list = fly_task_list[stag][is_shilian];
		FlyUpTaskCfg node;
		node.stag = stag;
		node.is_shilian = is_shilian;
		if (!GetSubNodeValue(dataElement, "task_id", node.task_id) || NULL == TASKPOOL->GetTaskCfgByID(node.task_id))
		{
			errormsg = STRING_SPRINTF("not data task_id OR task_id is NULL task_id");
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "rate", node.rate) || node.rate < 0)
		{
			errormsg = STRING_SPRINTF("not data rate OR rate < 0");
			return -3;
		}

		node_list.weigth += node.rate;
		
		node_list.task_vec.push_back(node);
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int FlyUpSystemConfig::InitFlyUpTaskRewardCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)return -999;
	while (NULL != dataElement)
	{
		int feisheng_stage = 0;
		if (!GetSubNodeValue(dataElement, "feisheng_stage", feisheng_stage) || feisheng_stage < 0 || feisheng_stage >= FlyUpSystemParam::FLY_UP_STAG_MAX)
		{
			errormsg = STRING_SPRINTF("not data feisheng_stage OR feisheng_stage < 0 OR feisheng_stage >= %d", FlyUpSystemParam::FLY_UP_STAG_MAX);
			return -1;
		}
		int is_shilian = 0;
		if (!GetSubNodeValue(dataElement, "is_shilian", is_shilian) || is_shilian < 0 || is_shilian >= FlyUpSystemParam::FLY_UP_SHILIAN_MAX)
		{
			errormsg = STRING_SPRINTF("not data is_shilian OR is_shilian < 0 OR is_shilian >= %d", FlyUpSystemParam::FLY_UP_SHILIAN_MAX);
			return -2;
		}
		int task_num = 0;
		if (!GetSubNodeValue(dataElement, "task_num", task_num) || task_num <= 0 || task_num > MAX_FLY_UP_TASK_IDX)
		{
			errormsg = STRING_SPRINTF("not data task_num OR task_num <= 0 OR task_num > %d", MAX_FLY_UP_TASK_IDX);
			return -3;
		}

		FlyUpTaskRewardCfg &node = fly_task_reward_list[feisheng_stage][is_shilian][task_num - 1];

		node.feisheng_stage = feisheng_stage;
		node.is_shilian = is_shilian;
		node.task_num = task_num;

		if (!GetSubNodeValue(dataElement, "reward_group", node.reward_group))
		{
			errormsg = STRING_SPRINTF("not data reward_group");
			return -4;
		}

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int FlyUpSystemConfig::InitFlyUpTaskRewardGroupCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)return -999;
	while (NULL != dataElement)
	{
		int reward_id = 0;
		if (!GetSubNodeValue(dataElement, "reward_id", reward_id))
		{
			errormsg = STRING_SPRINTF("not data reward_id");
			return -1;
		}

		FlyUpTaskRewardGroupListCfg &node_list = m_task_reward_map[reward_id];
		node_list.reward_id = reward_id;
		
		int rate = 0;
		if (!GetSubNodeValue(dataElement, "rate", rate))
		{
			errormsg = STRING_SPRINTF("not data rate");
			return -2;
		}

		FlyUpTaskRewardGroupCfg node;
		node.reward_id = reward_id;
		node.rate = rate;
		node_list.weight += rate;
		TiXmlElement * Reward_Element = dataElement->FirstChildElement("reward_item");
		if (NULL == Reward_Element)
		{
			errormsg = STRING_SPRINTF("not data reward_item");
			return -3;
		}

		if (!node.reward.ReadConfig(Reward_Element))
		{
			errormsg = STRING_SPRINTF("not data ReadErr");
			return -4;
		}
		
		node_list.m_reward_vec.push_back(node);

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int FlyUpSystemConfig::InitFlyUpTaskLastRewardCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)return -999;
	while (NULL != dataElement)
	{
		int feisheng_stage = 0;
		if (!GetSubNodeValue(dataElement, "feisheng_stage", feisheng_stage) || feisheng_stage < 0 || feisheng_stage >= FlyUpSystemParam::FLY_UP_STAG_MAX)
		{
			errormsg = STRING_SPRINTF("not data feisheng_stage OR feisheng_stage < 0 OR feisheng_stage >= %d", FlyUpSystemParam::FLY_UP_STAG_MAX);
			return -1;
		}
		int is_shilian = 0;
		if (!GetSubNodeValue(dataElement, "is_shilian", is_shilian) || is_shilian < 0 || is_shilian >= FlyUpSystemParam::FLY_UP_SHILIAN_MAX)
		{
			errormsg = STRING_SPRINTF("not data is_shilian OR is_shilian < 0 OR is_shilian >= %d", FlyUpSystemParam::FLY_UP_SHILIAN_MAX);
			return -2;
		}

		FlyUpFinishRewardCfg &node = finish_reward_list[feisheng_stage][is_shilian];
		node.feisheng_stage = feisheng_stage;
		node.is_shilian = is_shilian;

		TiXmlElement * Reward_List_Element = dataElement->FirstChildElement("reward_item_list");
		if (NULL != Reward_List_Element)
		{
			TiXmlElement * Reward_Element = Reward_List_Element->FirstChildElement("reward_item");
			while (NULL != Reward_Element)
			{
				ItemConfigData reward_node;
				if (!reward_node.ReadConfig(Reward_Element))
				{
					errormsg = STRING_SPRINTF("not data ReadErr");
					return -4;
				}
				node.reward_vec.push_back(reward_node);
				Reward_Element = Reward_Element->NextSiblingElement();
			}
		}

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}
