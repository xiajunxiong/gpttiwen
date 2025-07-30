#include "config/otherconfig/sundryconfig.hpp"
#include "item/itempool.h"
#include "servercommon/salarydef.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include "equipment/jewelry.h"
#include "task/taskpool.h"
#include "monster/monster_group_pool.hpp"
#include "gamelog.h"
#include "servercommon/formationdef.hpp"
#include "checkresourcecenter.hpp"
#include "scene/fbmanager.hpp"
#include "other/rolecrystalfb/rolecrystalfbconfig.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"

SundryConfig::SundryConfig() : m_chivalrous_achieve_cfg_num(0), m_fang_fraud_reward_count(0), m_original_fake_announcement_times_count(0), m_hidden_fake_announcement_times_count(0)
{
	memset(m_element_item_list, -1, sizeof(m_element_item_list));
}

SundryConfig::~SundryConfig()
{
	std::vector<GiveFlowerCfg>().swap(m_give_flower_cfg);
	std::vector<bool>().swap(m_fang_fraud_cfg);
	std::vector<FakeAnnouncementCfg>().swap(m_original_fake_announcement_list);
	std::vector<FakeAnnouncementCfg>().swap(m_hidden_fake_announcement_list);
}

bool SundryConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("constant", InitConstantCfg);
	LOAD_CONFIG("flower", InitGiveFlowerCfg);
	LOAD_CONFIG("captain_salary", InitCaptainSalaryCfg);
	LOAD_CONFIG("chivalrous_value", InitChivalrousSalaryCfg);
	LOAD_CONFIG("element", InitElementCfg);
	LOAD_CONFIG("patrol_task", InitPatrolTaskCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("chivalrous_value_cfg", InitMonsterGroupLinkageTaskCfg);
	LOAD_CONFIG("chivalrous_monster_group", InitChivalrousMonsterCfg);		//需要放在侠义值读取之后
	LOAD_CONFIG("chivalrous_achievement", InitChivalrousAchieveCfg);
	LOAD_CONFIG("question_configuration", InitFangFraudCfg);
	LOAD_CONFIG("gift_configuration", InitFangFraudRewardCfg);
	LOAD_CONFIG("fake_announcement", InitFakeAnnouncementCfg);
	LOAD_CONFIG_2("support_universal_card_activity", InitSupportUniversalCardCfg);
	LOAD_CONFIG_2("universal_card_value", InitUniversalCardValueCfg);
	LOAD_CONFIG_2("guaji_max", InitOnHookExpCfg);
	LOAD_CONFIG_2("fatigue", InitFatigueCfg);

	return true;
}

const GiveFlowerCfg * SundryConfig::GetGiveFlowerCfg(int seq)
{
	if (seq < 0 || seq >= (int)m_give_flower_cfg.size())
	{
		return NULL;
	}

	return &m_give_flower_cfg[seq];
}

void SundryConfig::GetFakeAnnouncement(int & times_count, std::vector<FakeAnnouncementCfg>& out_list)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		times_count = m_hidden_fake_announcement_times_count;
		out_list.assign(m_hidden_fake_announcement_list.begin(), m_hidden_fake_announcement_list.end());
	}
	else
	{
		times_count = m_original_fake_announcement_times_count;
		out_list.assign(m_original_fake_announcement_list.begin(), m_original_fake_announcement_list.end());
	}
}

bool SundryConfig::IsCanUseBuyGiftByType(int buy_gift_type) const
{
	return m_buy_gift_cmd_type_set.end() != m_buy_gift_cmd_type_set.find(buy_gift_type);
}

int SundryConfig::GetChongZhiNumByItemId(int item_id) const
{
	std::map<int, int>::const_iterator it = m_buy_gift_item_map.find(item_id);
	if (it == m_buy_gift_item_map.end())
	{
		return 0;
	}

	return it->second;
}

int SundryConfig::GetItemIdByChongZhiNum(int chong_zhi_num) const
{
	std::map<int, int>::const_iterator it = m_buy_gift_item_cfg.find(chong_zhi_num);
	if (it == m_buy_gift_item_cfg.end())
	{
		return 0;
	}

	return it->second;
}

const std::pair<int, int>* SundryConfig::GetFatigueCfg(int fatigue_number) const
{
	if (fatigue_number < 0) return NULL;

	for (int i = (int)m_fatigue_cfg.size() - 1; i >= 0; i--)
	{
		if (fatigue_number >= m_fatigue_cfg[i].first)
		{
			return &m_fatigue_cfg[i];
		}
	}
	return NULL;
}

const std::pair<double, int>* SundryConfig::GetMaxGuaJiExpCfg(int level) const
{
	std::map<int, std::pair<double, int> >::const_iterator it = m_max_guaji_exp_cfg.find(level);
	if (it != m_max_guaji_exp_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}

const CaptainSalaryCfg* SundryConfig::GetCaptainSalaryCfg(const SalaryCfgKey& key)
{
	CaptainSalaryCfgMap::iterator it = m_captain_salary_cfg_map.find(key);
	if (it != m_captain_salary_cfg_map.end())
	{
		return &it->second;
	}

	return NULL;
}

const CaptainSalaryCfg* SundryConfig::GetCaptainSalaryCfg(int reward_type, int param, int param2 /* = 0 */)
{
	if (SALARY_REWARD_TYPE_KILL_TASK_MONSTER_GROUP == reward_type)
	{
		param = 0;
	}
	return this->GetCaptainSalaryCfg(SalaryCfgKey(reward_type, param, param2));
}

const ChivalrousSalaryCfg* SundryConfig::GetChivalrousSalaryCfg(int reward_type, int param, int param2 /* = 0 */)
{
	if (SALARY_REWARD_TYPE_KILL_TASK_MONSTER_GROUP == reward_type)
	{
		param = 0;
	}
	return this->GetChivalrousSalaryCfg(SalaryCfgKey(reward_type, param, param2));
}

const ChivalrousSalaryCfg * SundryConfig::GetChivalrousSalaryCfgByMonsterGroup(int monster_group_id)
{
	std::map<int, ChivalrousSalaryCfg>::iterator it = m_chivalrous_salary_by_monster_group_map.find(monster_group_id);
	if(it == m_chivalrous_salary_by_monster_group_map.end()) return NULL;
	
	return &it->second;
}

const MonsterGroupLinkageTaskCfg * SundryConfig::GetMonsterGroupLinkageTaskCfg(int monster_group_id)
{
	std::map<int, MonsterGroupLinkageTaskCfg>::iterator it = m_monster_group_linage_task.find(monster_group_id);
	if(it != m_monster_group_linage_task.end()) return &it->second;

	return NULL;
}

const ChivalrousSalaryCfg* SundryConfig::GetChivalrousSalaryCfg(const SalaryCfgKey& key)
{
	ChivarousSalaryCfgMap::iterator it = m_chivalrous_salary_cfg_map.find(key);
	if (it != m_chivalrous_salary_cfg_map.end())
	{
		return &it->second;
	}

	return NULL;
}

ItemID SundryConfig::GetElementItem(int element_type)
{
	if (element_type > ELEMENT_TYPE_INVALID && element_type < ELEMENT_TYPE_MAX)
	{
		return m_element_item_list[element_type];
	}

	return 0;
}

const int SundryConfig::GetCoinByLevel(int _role_level)
{
	std::map<int, int>::iterator it = m_level_to_coin.find(_role_level);
	if (it != m_level_to_coin.end()) return it->second;
	return 0;
}

const SundryOtherCfg & SundryConfig::GetOtherCfg()
{
	return m_other_cfg;
}

const ChivalrousAchievementCfg* SundryConfig::GetChivalrousAchieveCfg(int seq)
{
	for (int i = 0; i < m_chivalrous_achieve_cfg_num; ++i)
	{
		if (seq == m_chivalrous_achieve_cfg[i].seq)
		{
			return &m_chivalrous_achieve_cfg[i];
		}
	}

	return NULL;
}

const ChivalrousTeamInfoCfg * SundryConfig::GetChivalrousMonsterGroupByTeamType(int team_type)
{
	std::map<int, ChivalrousTeamInfoCfg>::iterator it = m_teamtype_monstergroup_hash.find(team_type);
	if (it == m_teamtype_monstergroup_hash.end())
	{
		return NULL;
	}

	return &it->second;
}

bool SundryConfig::IsChivalrousTeamType(int team_type)
{
	return this->GetChivalrousMonsterGroupByTeamType(team_type) != NULL;
}

const SalaryCfgKey * SundryConfig::GetSalaryCfgKeyByMonsterGroupID(int monster_group_id)
{
	std::map<int, SalaryCfgKey>::iterator it = m_monstergroup_key_hash.find(monster_group_id);
	if(it == m_monstergroup_key_hash.end()) return NULL;

	return &it->second;
}

bool SundryConfig::IsFangFraudSucc(int index)
{
	if (index <= 0 || index > (int)m_fang_fraud_cfg.size())
		return false;

	return m_fang_fraud_cfg[index - 1];
}

int SundryConfig::InitConstantCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		if (!GetSubNodeValue(dataElement, "max_distance", m_constant_cfg.max_npc_touch_distance) || m_constant_cfg.max_npc_touch_distance < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "drop_max_num", m_constant_cfg.drop_card_max_num) || m_constant_cfg.drop_card_max_num < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "name_change_item", m_constant_cfg.name_change_item) || NULL == ITEMPOOL->GetItem(m_constant_cfg.name_change_item))
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "name_change_num", m_constant_cfg.name_change_num) || m_constant_cfg.name_change_num < 0)
		{
			return -5;
		}
		if (!GetSubNodeValue(dataElement, "move_speed", m_constant_cfg.move_speed) || m_constant_cfg.move_speed <= 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "crystal", m_constant_cfg.init_crystal) || NULL == ITEMPOOL->GetItem(m_constant_cfg.init_crystal))
		{
			return -7;
		}

		if (!GetSubNodeValue(dataElement, "max_qinmi", m_constant_cfg.max_battle_add_intimacy))
		{
			return -8;
		}

		if (!GetSubNodeValue(dataElement, "level_differ", m_constant_cfg.chivarous_salary_need_level_diff) || m_constant_cfg.chivarous_salary_need_level_diff < 0)
		{
			return -9;
		}
			
		if (!GetSubNodeValue(dataElement, "xiayi_level", m_constant_cfg.chivarous_salary_need_level) 
			|| m_constant_cfg.chivarous_salary_need_level < 0 
			|| m_constant_cfg.chivarous_salary_need_level > MAX_ROLE_LEVEL)
		{
			return -10;
		}

		if (!GetSubNodeValue(dataElement, "partner_robot_partner_id", m_constant_cfg.partner_robot_partner_id))
		{
			return -13;
		}

		if (!GetSubNodeValue(dataElement, "partner_robot_skill_id", m_constant_cfg.partner_robot_skill_id))
		{
			return -14;
		}
		CheckResourceCenter::Instance().GetSkillCheckStruct()->Add(m_constant_cfg.partner_robot_skill_id, __FUNCTION__);	
		
		if (!GetSubNodeValue(dataElement, "partner_robot_skill_id_1", m_constant_cfg.partner_robot_skill_id_1))
		{
			return -15;
		}
		CheckResourceCenter::Instance().GetSkillCheckStruct()->Add(m_constant_cfg.partner_robot_skill_id_1, __FUNCTION__);

		if (!GetSubNodeValue(dataElement, "change_appearance_item", m_constant_cfg.change_appearance_item) || m_constant_cfg.change_appearance_item <= 0 || m_constant_cfg.change_appearance_item > 65535)
		{
			return -16;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int SundryConfig::InitGiveFlowerCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	int next_seq = 0;
	while (NULL != dataElement)
	{
		GiveFlowerCfg cfg;

		if (!GetSubNodeValue(dataElement, "seq", cfg.seq) || cfg.seq != next_seq)
		{
			return -1;
		}
		next_seq += 1;

		if (!GetSubNodeValue(dataElement, "item_id", cfg.consume_item_id) || ITEMPOOL->GetItem(cfg.consume_item_id) == NULL)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "qinmi", cfg.add_value))
		{
			return -3;
		}

		m_give_flower_cfg.push_back(cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int SundryConfig::InitCaptainSalaryCfg(TiXmlElement* RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int reward_type = 0;
		if (!GetSubNodeValue(dataElement, "reward_type", reward_type) || reward_type < 0 || reward_type >= SALARY_REWARD_TYPE_MAX)
		{
			return -1;
		}

		int param = 0;
		if (!GetSubNodeValue(dataElement, "param", param) || param < 0)
		{
			return -2;
		}

		int param2 = 0;
		if (!GetSubNodeValue(dataElement, "param_2", param2) || param2 < 0)
		{
			return -22;
		}

		SalaryCfgKey key(reward_type, param, param2);

		CaptainSalaryCfgMap::iterator it = m_captain_salary_cfg_map.find(key);
		if (it != m_captain_salary_cfg_map.end())
		{
			return -100;
		}

		CaptainSalaryCfg cfg;
		cfg.reward_type = reward_type;
		cfg.param = param;

		if (!GetSubNodeValue(dataElement, "week_limit", cfg.week_limit_times) || cfg.week_limit_times < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "reward", cfg.reward_coin) || cfg.reward_coin < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "price_type", cfg.reward_coin_type))
		{
			return -41;
		}

		if (MONEY_TYPE_GOLD == cfg.reward_coin_type || MONEY_TYPE_COIN_BIND == cfg.reward_coin_type || MONEY_TYPE_IMMORTAL_COIN == cfg.reward_coin_type)
		{
			return -42;
		}

		if (!GetSubNodeValue(dataElement, "fuben_id", cfg.seq) || cfg.seq < 0 || cfg.seq >= MAX_SALARY_INFO_NUM)
		{
			return -5;
		}

		m_captain_salary_cfg_map.insert(CaptainSalaryCfgMap::value_type(key, cfg));
	
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int SundryConfig::InitChivalrousSalaryCfg(TiXmlElement* RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int reward_type = 0;
		if (!GetSubNodeValue(dataElement, "reward_type", reward_type) || reward_type < 0 || reward_type >= SALARY_REWARD_TYPE_MAX)
		{
			return -1;
		}

		int param = 0;
		if (!GetSubNodeValue(dataElement, "param", param) || param < 0)
		{
			return -2;
		}

		int param2 = 0;
		if (!GetSubNodeValue(dataElement, "param_2", param2) || param2 < 0)
		{
			return -22;
		}

		int version = 0;
		if (!GetSubNodeValue(dataElement, "reset", version))
		{
			return -2;
		}

		switch (reward_type)
		{
			case SALARY_REWARD_TYPE_KILL_CHALLENGE_TASK_MONSTER_SUCC:
			{
				if (param != TASK_TYPE_CHALLENGES || param != TASK_TYPE_TEAM_CHALLENGES)
				{
					return -666;
				}
			}
			break;
			case SALARY_REWARD_TYPE_KILL_BRANCH_TASK_MONSTER_SUCC:
			{
				if (param != TASK_TYPE_SIDE_QUEST)
				{
					return -777;
				}
			}
			break;
			case SALARY_REWARD_TYPE_KILL_ADVANCED_TASK_MONSTER_SUCC:
			{
				if (param != TASK_TYPE_STAGE_TASK && param != TASK_TYPE_STAGE_TASK_1)
				{
					return -888;
				}
			}
			break;
			default:
			break;
		}

		SalaryCfgKey key(reward_type, param, param2);

		ChivarousSalaryCfgMap::iterator it = m_chivalrous_salary_cfg_map.find(key);
		if (it != m_chivalrous_salary_cfg_map.end())
		{
			return -100;
		}

		ChivalrousSalaryCfg cfg;
		cfg.reward_type = reward_type;
		cfg.param = param;
		cfg.param2 = param2;
		cfg.version = version;

		if (!GetSubNodeValue(dataElement, "week_limit", cfg.week_limit_times) || cfg.week_limit_times < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "reward", cfg.reward_chivarous) || cfg.reward_chivarous < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "fuben_id", cfg.seq) || cfg.seq < 0 || cfg.seq >= MAX_SALARY_INFO_NUM)
		{
			return -5;
		}

		m_chivalrous_salary_cfg_map.insert(ChivarousSalaryCfgMap::value_type(key, cfg));

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int SundryConfig::InitElementCfg(TiXmlElement* RootElement)
{
	bool flag[ELEMENT_TYPE_MAX] = { false };

	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int et = 0;
		if (!GetSubNodeValue(dataElement, "element_type", et))
		{
			return -1;
		}

		if (et != ELEMENT_TYPE_WATER && et != ELEMENT_TYPE_FIRE && et != ELEMENT_TYPE_EARTH && et != ELEMENT_TYPE_WIND)
		{
			return -2;
		}

		if (flag[et])
		{
			return -22;
		}

		ItemID item = 0;
		if (!GetSubNodeValue(dataElement, "item_id", item) || NULL == ITEMPOOL->GetItem(item))
		{
			return -3;
		}
		flag[et] = true;

		m_element_item_list[et] = item;

		dataElement = dataElement->NextSiblingElement();
	}

	for (int i = ELEMENT_TYPE_INVALID + 1; i < ELEMENT_TYPE_MAX; ++i)
	{
		if (i != ELEMENT_TYPE_WATER && i != ELEMENT_TYPE_FIRE && i != ELEMENT_TYPE_EARTH && i != ELEMENT_TYPE_WIND)
		{
			continue;
		}

		if (!flag[i])
		{
			return -10;
		}
	}

	return 0;
}

int SundryConfig::InitPatrolTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	
	//if (!GetSubNodeValue(dataElement, "kill_num", patrol_task_cfg.kill_num) || patrol_task_cfg.kill_num > 0x7fff)//定义进度最大是short
	//{
	//	return -1;
	//}

	//if (!GetSubNodeValue(dataElement, "cost_type", patrol_task_cfg.cost_type) || BIND_COIN < patrol_task_cfg.cost_type || GOLD > patrol_task_cfg.cost_type)
	//{
	//	return -2;
	//}

	//if (!GetSubNodeValue(dataElement, "reward", patrol_task_cfg.reward) || patrol_task_cfg.reward <= 0)
	//{
	//	return -3;
	//}

	while (NULL != dataElement)
	{
		if (!GetSubNodeValue(dataElement, "level", patrol_task_cfg.level) || patrol_task_cfg.level > 120 || patrol_task_cfg.level < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "kill_num", patrol_task_cfg.kill_num) || patrol_task_cfg.kill_num > 0x7fff)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "coin", patrol_task_cfg.coin) || patrol_task_cfg.coin < 0)
		{
			return -3;
		}

		m_level_to_coin[patrol_task_cfg.level] = patrol_task_cfg.coin;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int SundryConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	if (!GetSubNodeValue(dataElement, "recovery_need_gold", m_other_cfg.recovery_need_gold) || m_other_cfg.recovery_need_gold <= 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "treasure_map_notice_item", m_other_cfg.treasure_map_notice_item) || ITEMPOOL->GetItem(m_other_cfg.treasure_map_notice_item) == NULL)
	{
		return -2;
	}

	if (!GetSubNodeValue(dataElement, "patrol_open_level", m_other_cfg.patrol_open_level) || m_other_cfg.patrol_open_level < 0 || m_other_cfg.patrol_open_level > MAX_ROLE_LEVEL)
	{
		return -3;
	}

	if (!GetSubNodeValue(dataElement, "star_team", m_other_cfg.formation_start_num) || m_other_cfg.formation_start_num <= 0 || m_other_cfg.formation_start_num > MAX_FORMATION_NUM)
	{
		return -4;
	}

	TiXmlElement *list_element = dataElement->FirstChildElement("novice_reward_item_list");
	if (list_element)
	{
		TiXmlElement *item_element = list_element->FirstChildElement("novice_reward_item");

		if (item_element)
		{
			if (!m_other_cfg.create_role_reward.ReadConfig(item_element)) return -5;
		}
		
	}
	if (!GetSubNodeValue(dataElement, "server_check", m_other_cfg.server_check) || m_other_cfg.server_check < 0)
	{
		return -6;
	}

	if (!GetSubNodeValue(dataElement, "auto_level_difference", m_other_cfg.auto_level_difference) || m_other_cfg.auto_level_difference < 0)
	{
		return -7;
	}

	TiXmlElement *download_reward_element = dataElement->FirstChildElement("download_reward_list");
	if (NULL != download_reward_element)
	{
		TiXmlElement *item_element = download_reward_element->FirstChildElement("download_reward");
		int count = 0;
		while (NULL != item_element)
		{
			if (count >= sizeof(m_other_cfg.download_reward) / sizeof(ItemConfigData))
				return -8;

			if (!m_other_cfg.download_reward[count].ReadConfig(item_element))
				return -9;

			count++;
			item_element = item_element->NextSiblingElement();
		}

		m_other_cfg.download_reward_count = count;
	}

	TiXmlElement * pingjia_reward_element = dataElement->FirstChildElement("reward_item");
	if (NULL == pingjia_reward_element)
		return -10;

	if (!m_other_cfg.pinjia_reward.ReadConfig(pingjia_reward_element))
		return -11;

	if (!GetSubNodeValue(dataElement, "level", m_other_cfg.pingjia_level) || m_other_cfg.pingjia_level <= 0)
		return -12;

	int is_add_day_chong = 0;
	if (!GetSubNodeValue(dataElement, "is_open", is_add_day_chong))
	{
		return -13;
	}
	m_other_cfg.is_add_day_chong = (0 != is_add_day_chong);

	return 0;
}

int SundryConfig::InitMonsterGroupLinkageTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;
	
	while (NULL != dataElement)
	{
		int monster_group_id = 0;
		if (!GetSubNodeValue(dataElement, "monster_group_id", monster_group_id) || MonsterGroupPool::GetInstance().GetMonsterGroupCfg(monster_group_id) == NULL)
		{
			return -1;
		}

		MonsterGroupLinkageTaskCfg cfg;
		if (!GetSubNodeValue(dataElement, "task_id", cfg.task_id) || TASKPOOL->GetTaskCfgByID(cfg.task_id) == NULL)
		{
			return -2;
		}
		if (!GetSubNodeValue(dataElement, "reward_type", cfg.reward_type) || cfg.reward_type < 0 || cfg.reward_type >= SALARY_REWARD_TYPE_MAX)
		{
			return -3;
		}

		const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(cfg.task_id);
		switch (cfg.reward_type)
		{
		case SALARY_REWARD_TYPE_KILL_CHALLENGE_TASK_MONSTER_SUCC:
		{
			if (task_cfg->task_type != TASK_TYPE_CHALLENGES || task_cfg->task_type != TASK_TYPE_TEAM_CHALLENGES)
			{
				return -666;
			}
		}
		break;
		case SALARY_REWARD_TYPE_KILL_BRANCH_TASK_MONSTER_SUCC:
		{
			if (task_cfg->task_type != TASK_TYPE_SIDE_QUEST)
			{
				return -777;
			}
		}
		break;
		case SALARY_REWARD_TYPE_KILL_ADVANCED_TASK_MONSTER_SUCC:
		{
			if (task_cfg->task_type != TASK_TYPE_STAGE_TASK && task_cfg->task_type != TASK_TYPE_STAGE_TASK_1)
			{
				return -888;
			}
		}
		break;
		default:
			return -999;
		}

		m_monster_group_linage_task[monster_group_id] = cfg;
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int SundryConfig::InitChivalrousMonsterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int monster_group_id = 0;
		if (!GetSubNodeValue(dataElement, "monster_group_id", monster_group_id) || MonsterGroupPool::GetInstance().GetMonsterGroupCfg(monster_group_id) == NULL)
		{
			return -1;
		}
		if (m_chivalrous_salary_by_monster_group_map.find(monster_group_id) != m_chivalrous_salary_by_monster_group_map.end())
		{
			return -2;
		}

		int type = 0;
		if (!GetSubNodeValue(dataElement, "reward_type", type))
		{
			return -3;
		}
		int param = 0;
		if (!GetSubNodeValue(dataElement, "param", param))
		{
			return -4;
		}
		int param2 = 0;
		if (!GetSubNodeValue(dataElement, "param_2", param2))
		{
			return -5;
		}
		m_monstergroup_key_hash[monster_group_id] = SalaryCfgKey(type, param, param2);	//某些奖励类型参数在此参数2有特殊作用

		ChivalrousTeamInfoCfg team_cfg;
		team_cfg.monster_group_id = monster_group_id;
		if (!GetSubNodeValue(dataElement, "item_id", team_cfg.need_item_id) || team_cfg.need_item_id < 0)
		{
			return -7;
		}

		//新增或减少时请修改一下Salary::OnBattleStartStatSalaryInfo逻辑
		switch (type)
		{
		case SALARY_REWARD_TYPE_WIN_BATTLE:
			{
				param2 = 0;
			}
			break;

		case SALARY_REWARD_TYPE_KILL_CHALLENGE_TASK_MONSTER_SUCC:
		case SALARY_REWARD_TYPE_KILL_BRANCH_TASK_MONSTER_SUCC:
		case SALARY_REWARD_TYPE_KILL_ADVANCED_TASK_MONSTER_SUCC:
			{

			}
			break;

		case SALARY_REWARD_TYPE_CLEAR_FB:
			{
				if (FBManager::FB_TYPE_CRYSTAL_FB == param && ROLE_CRYSTAL_FB_TYPE_JING_YING == param2)
				{
					if(NULL == ITEMPOOL->GetItem(team_cfg.need_item_id)) return -9;
				}
			}	
			break;
		
		default:
			return -10;
			break;
		}

		SalaryCfgKey key(type, param, param2);
		const ChivalrousSalaryCfg* chivalrous_salary_cfg = this->GetChivalrousSalaryCfg(key);
		if (NULL == chivalrous_salary_cfg) return -6;

		int team_type = 0;
		if (!GetSubNodeValue(dataElement, "target_id", team_type) || team_type <= 0)
		{
			return -8;
		}

		m_chivalrous_salary_by_monster_group_map[monster_group_id] = *chivalrous_salary_cfg;
		m_teamtype_monstergroup_hash[team_type] = team_cfg;
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int SundryConfig::InitChivalrousAchieveCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	m_chivalrous_achieve_cfg_num = 0;
	while (NULL != dataElement)
	{	
		if (m_chivalrous_achieve_cfg_num >= ARRAY_ITEM_COUNT(m_chivalrous_achieve_cfg))
		{
			return -1;
		}

		ChivalrousAchievementCfg& cfg = m_chivalrous_achieve_cfg[m_chivalrous_achieve_cfg_num];
		if (!GetSubNodeValue(dataElement, "seq", cfg.seq))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "chivalrous_value", cfg.need_chivalrous_value))
		{
			return -3;
		}

		TiXmlElement* list_element = dataElement->FirstChildElement("achievement_reward_list");
		if (list_element)
		{
			TiXmlElement* item_element = list_element->FirstChildElement("achievement_reward");

			cfg.reward_num = 0;
			while (item_element)
			{
				if (!cfg.item_list[cfg.reward_num].ReadConfig(item_element)) return -4;

				cfg.reward_num += 1;
				item_element = item_element->NextSiblingElement();
			}
		}

		m_chivalrous_achieve_cfg_num += 1;
		
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int SundryConfig::InitFangFraudCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int correct = 0;
		if (!GetSubNodeValue(dataElement, "is_correct", correct) || (0 != correct && 1 != correct))
			return -1;

		bool is_correct = (0 != correct);
		m_fang_fraud_cfg.push_back(is_correct);

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int SundryConfig::InitFangFraudRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	TiXmlElement *reward_element = dataElement->FirstChildElement("reward_item_list");
	if (NULL != reward_element)
	{
		TiXmlElement *item_element = reward_element->FirstChildElement("reward_item");
		int count = 0;
		while (NULL != item_element)
		{
			if (count >= sizeof(m_fang_fraud_reward) / sizeof(ItemConfigData))
				return -8;

			if (!m_fang_fraud_reward[count].ReadConfig(item_element))
				return -9;

			count++;
			item_element = item_element->NextSiblingElement();
		}

		m_fang_fraud_reward_count = count;
	}

	return 0;
}

int SundryConfig::InitFakeAnnouncementCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	m_original_fake_announcement_times_count = 0;
	m_hidden_fake_announcement_times_count = 0;
	while (NULL != dataElement)
	{
		FakeAnnouncementCfg node_cfg;
		if (!GetSubNodeValue(dataElement, "start_level", node_cfg.min_level) || node_cfg.min_level < 0)
		{
			return -1;
		}
		if (!GetSubNodeValue(dataElement, "end_level", node_cfg.max_level) || node_cfg.max_level < node_cfg.min_level || node_cfg.max_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}
		if (!GetSubNodeValue(dataElement, "announcement_type", node_cfg.announcement_type) || node_cfg.announcement_type < 0 
			|| node_cfg.announcement_type >= FAKE_ANNOUNCEMENT_TYPE_MAX)
		{
			return -3;
		}
		int is_corss_service = 0;
		if (!GetSubNodeValue(dataElement, "is_corss_service", is_corss_service))
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "random_times", node_cfg.random_times) || node_cfg.random_times <= 0)
		{
			return -5;
		}
		if (!GetSubNodeValue(dataElement, "descripte_1", node_cfg.descripte_1))
		{
			return -6;
		}
		int is_random_name = 0;
		if (!GetSubNodeValue(dataElement, "is_random_name", is_random_name))
		{
			return -7;
		}
		node_cfg.is_random_name = (0 != is_random_name) ? true : false;

		if (!GetSubNodeValue(dataElement, "descripte_2", node_cfg.descripte_2))
		{
			return -8;
		}

		if (!GetSubNodeValue(dataElement, "item_id", node_cfg.item_id) || NULL == ITEMPOOL->GetItem(node_cfg.item_id))
		{
			return -9;
		}

		if (!GetSubNodeValue(dataElement, "descripte_3", node_cfg.descripte_3))
		{
			return -10;
		}

		if (0 == is_corss_service)
		{
			m_original_fake_announcement_times_count += node_cfg.random_times;
			m_original_fake_announcement_list.push_back(node_cfg);
		}
		else
		{
			m_hidden_fake_announcement_times_count += node_cfg.random_times;
			m_hidden_fake_announcement_list.push_back(node_cfg);
		}
		dataElement = dataElement->NextSiblingElement();
	}

	if (m_original_fake_announcement_times_count >= SECOND_PER_HOUR || m_hidden_fake_announcement_times_count >= SECOND_PER_HOUR)
	{
		return -555;
	}

	return 0;
}

int SundryConfig::InitSupportUniversalCardCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int buy_gift_cmd_type = 0;
		if (!GetSubNodeValue(dataElement, "buy_type", buy_gift_cmd_type) || buy_gift_cmd_type <= BUY_GIFT_ITEM_TYPE_INVALID || 
			buy_gift_cmd_type >= BUY_GIFT_ITEM_TYPE_MAX || m_buy_gift_cmd_type_set.end() != m_buy_gift_cmd_type_set.find(buy_gift_cmd_type))
		{
			errormsg = STRING_SPRINTF("[buy_type=%d] error, is not find data or <= %d or >= %d or repeat?", buy_gift_cmd_type, 
				BUY_GIFT_ITEM_TYPE_INVALID, BUY_GIFT_ITEM_TYPE_MAX);
			return -1;
		}

		m_buy_gift_cmd_type_set.insert(buy_gift_cmd_type);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int SundryConfig::InitUniversalCardValueCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int item_id = 0;
		if (!GetSubNodeValue(dataElement, "item_id", item_id) || NULL == ITEMPOOL->GetItem(item_id) || m_buy_gift_item_map.end() != m_buy_gift_item_map.find(item_id))
		{
			errormsg = STRING_SPRINTF("[item_id=%d] error, not has item or repeat?", item_id);
			return -1;
		}

		int chong_zhi_num = 0;
		if (!GetSubNodeValue(dataElement, "value_num", chong_zhi_num) || chong_zhi_num < 10 || 0 != (chong_zhi_num % 10))
		{
			errormsg = STRING_SPRINTF("[item_id=%d] error, value_num[%d] < 10 or (% 10) != 0?", item_id, chong_zhi_num);
			return -2;
		}
		if (m_buy_gift_item_cfg.end() != m_buy_gift_item_cfg.find(chong_zhi_num))
		{
			errormsg = STRING_SPRINTF("[item_id=%d] error, value_num[%d] repeat?", item_id, chong_zhi_num);
			return -3;
		}

		m_buy_gift_item_map[item_id] = chong_zhi_num;
		m_buy_gift_item_cfg[chong_zhi_num] = item_id;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int SundryConfig::InitOnHookExpCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -544;

	while (dataElement)
	{
		int level = 0, exp = 0;
		if (!GetSubNodeValue(dataElement, "level", level) || level <= 0 || level > MAX_ROLE_LEVEL) return -1;

		std::pair<int, int> cfg;
		if (!GetSubNodeValue(dataElement, "experience", cfg.first) || cfg.first < 0.0) return -2;
		if (!GetSubNodeValue(dataElement, "fatigue_number", cfg.second) || cfg.second < 0) return -3;

		m_max_guaji_exp_cfg[level] = cfg;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int SundryConfig::InitFatigueCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -544;

	while (dataElement)
	{
		std::pair<int, int> cfg;
		if (!GetSubNodeValue(dataElement, "greater_than_fatigue_number", cfg.first) || cfg.first < 0 || cfg.first > 100) return -1;
		if (!GetSubNodeValue(dataElement, "reduce_proportion", cfg.second) || cfg.second < 0 || cfg.second > 100) return -2;

		m_fatigue_cfg.push_back(cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}
