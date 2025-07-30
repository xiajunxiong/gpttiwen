#include "courseofgrowthconfig.hpp"
#include "servercommon/courseofgrowthdef.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/prestige/prestigeconfig.hpp"
#include "other/treasure_map/treasuremapconfig.h"
#include "other/rolecrystalfb/rolecrystalfbconfig.hpp"
#include "scene/fbmanager.hpp"
#include "item/itembase.h"
#include "item/expense/norexitem.hpp"
#include "task/taskpool.h"
#include "monster/monster_group_pool.hpp"
#include "other/train/trainconfig.hpp"
#include "other/pet/petconfig.hpp"
#include "servercommon/facescoredef.h"
#include "servercommon/struct/moneyotherdef.h"
#include "gameworld/checkresourcecenter.hpp"
#include "servercommon/miyidef.hpp"
#include "global/shanhaibossmanager/shanhaibossconfig.hpp"

CourseOfGrowthConfig::CourseOfGrowthConfig(): max_cog_task_num(0)
{
}

CourseOfGrowthConfig::~CourseOfGrowthConfig()
{
	for (std::map<int, std::vector<CourseEntryCfg> >::iterator it = m_course_entry_list.begin(); it != m_course_entry_list.end(); it++)
	{
		std::vector<CourseEntryCfg>().swap(it->second);
	}
}

bool CourseOfGrowthConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG

	LOAD_CONFIG_2("accomplishment", InitCOGTasksList);
	LOAD_CONFIG("stage_group", InitCOGStarAward);
	LOAD_CONFIG("promotion_challenge", InitCOGBossGroup);
	LOAD_CONFIG_2("course", InitCourseEntry);
	LOAD_CONFIG_2("course_reward", InitCourseLevel);

	return true;
}

const COGTaskCon * CourseOfGrowthConfig::GetCOGTaskInfo(int index)
{
	std::map<int, COGTaskCon>::iterator find_it = m_cog_task_list.find(index);
	if (m_cog_task_list.end() != find_it) return & find_it->second;

	return NULL;
}

const COGTaskCon * CourseOfGrowthConfig::GetCOGTaskInfo(int prog, int star, int sort)
{
	if (prog > GROWTH_TASK_NUM_PER_TYPE_STAGE || prog > max_cog_task_num)
	{
		return NULL;
	}

	if (star > GROWTH_TASK_NUM_PER_TYPE_STAR)
	{
		return NULL;
	}

	if (sort >= GROWTH_TASK_NUM_PER_TYPE_SEQ)
	{
		return NULL;
	}

	return &m_cog_task_map[prog][star][sort];
}

const COGAwardCon * CourseOfGrowthConfig::GetStarAward(int prog, int star)
{
	std::map<int, std::map<int, COGAwardCon> >::iterator prog_it = m_cog_promote_award.find(prog);
	if (m_cog_promote_award.end() == prog_it) return NULL;

	std::map<int, COGAwardCon>::iterator star_it = prog_it->second.find(star);
	if (prog_it->second.end() != star_it) return & star_it->second;

	return NULL;
}

int CourseOfGrowthConfig::GetBossGroupID(int prog, int star)
{
	std::map<int, std::map<int, int> >::iterator prog_it = m_boss_group_map.find(prog);
	if (m_boss_group_map.end() == prog_it) return 0;

	std::map<int, int>::iterator star_it = prog_it->second.find(star);
	if (prog_it->second.end() != star_it) return star_it->second;

	return 0;
}

void CourseOfGrowthConfig::SetNextProgAndStar(UInt16 * out_prog, UInt16 * out_star)
{
	if (NULL == out_prog || NULL == out_star) return;
	if (*out_prog > max_cog_task_num)
	{
		return;
	}

	if (*out_star >= GROWTH_TASK_NUM_PER_TYPE_STAR)
	{
		++*out_prog;
		*out_star = GROWTH_STAR_BEGINNING;
	}
	else
	{
		++*out_star;
	}
	return;
}

int CourseOfGrowthConfig::GetMaxCourseSeq()
{
	if (m_course_entry_map.empty())
	{
		return 0;
	}
	return m_course_entry_map.rbegin()->first + 1;
}

const std::vector<CourseEntryCfg>* CourseOfGrowthConfig::GetCourseEntryCfg(int condition_type)
{
	std::map<int, std::vector<CourseEntryCfg> >::const_iterator condition_it = m_course_entry_list.find(condition_type);
	if (condition_it == m_course_entry_list.end() || condition_it->second.empty())
	{
		return NULL;
	}

	return &condition_it->second;
}

const CourseLevelRewardCfg * CourseOfGrowthConfig::GetCourseLevelRewardCfg(int level)
{
	std::map<int, CourseLevelRewardCfg>::const_iterator it = m_course_level_reward_cfg.find(level);
	if (it == m_course_level_reward_cfg.end())
	{
		return NULL;
	}
	return &it->second;
}

void CourseOfGrowthConfig::CalcLevelByCourseFraction(int course_fraction, int * out_level, int * out_cur_fraction)
{
	if(NULL == out_level || NULL == out_cur_fraction) return;
	int last_level = 0;
	for (std::map<int, CourseLevelRewardCfg>::const_iterator it = m_course_level_reward_cfg.begin(); it != m_course_level_reward_cfg.end(); ++it)
	{
		if (course_fraction < it->second.need_fraction)
		{
			*out_level = last_level;
			*out_cur_fraction = course_fraction;	
			return;
		}
		course_fraction -= it->second.need_fraction;
		last_level = it->first;
	}
	*out_cur_fraction = course_fraction;	
	*out_level = m_course_level_reward_cfg.rbegin()->first;
}

int CourseOfGrowthConfig::InitCOGTasksList(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	std::map<int, std::map<int, int> > check_map;
	max_cog_task_num = 0;
	while (NULL != root_element)
	{
		COGTaskCon tmp_con;
		if (!GetSubNodeValue(root_element, "sn", tmp_con.cog_id) || m_cog_task_list.end() != m_cog_task_list.find(tmp_con.cog_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "stage", tmp_con.prog) || tmp_con.prog < GROWTH_PROG_BEGINNING || tmp_con.prog > GROWTH_TASK_NUM_PER_TYPE_STAGE)
		{
			return -2;
		}
		if (max_cog_task_num != tmp_con.prog && max_cog_task_num + 1 != tmp_con.prog)
		{
			return -999;
		}
		max_cog_task_num = tmp_con.prog;
		if (!GetSubNodeValue(root_element, "star", tmp_con.star) || tmp_con.star < GROWTH_STAR_BEGINNING || tmp_con.star > GROWTH_TASK_NUM_PER_TYPE_STAR)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "seq", tmp_con.seq) || tmp_con.seq < 0 || tmp_con.seq >= GROWTH_TASK_NUM_PER_TYPE_SEQ)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "cond_type", tmp_con.condtion) || tmp_con.condtion < 0 || tmp_con.condtion >= COG_CON_TYPE_MAX)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "cond_param1", tmp_con.param1))
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "cond_param2", tmp_con.param2))
		{
			return -7;
		}
		
		if (!this->CheckCondition(tmp_con.seq, tmp_con.condtion, tmp_con.param1, tmp_con.param2, errormsg))
		{
			return -8;
		}
		
		if (!GetSubNodeValue(root_element, "reward_exp", tmp_con.award_con.reward_exp) || tmp_con.award_con.reward_exp < 0)
		{
			return -9;
		}

		if (!GetSubNodeValue(root_element, "reward_coin", tmp_con.award_con.reward_coin) || tmp_con.award_con.reward_coin < 0)
		{
			return -10;
		}

		TiXmlElement * list_node = root_element->FirstChildElement("reward_item_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("reward_item");
			if (NULL != item_node)		// 允许奖励道具为空
			{
				if (tmp_con.award_con.reward_num >= MAX_COG_AWARD_NUM) return -20;
				if (!tmp_con.award_con.reward_list[tmp_con.award_con.reward_num++].ReadConfig(item_node))
				{
					return -111;
				}

				item_node = item_node->NextSiblingElement();
			}
		}

		m_cog_task_list[tmp_con.cog_id] = tmp_con;
		m_cog_task_map[tmp_con.prog][tmp_con.star][tmp_con.seq] = tmp_con;

		check_map[tmp_con.condtion * GROWTH_KEY_MULT + tmp_con.param1][tmp_con.condtion] = tmp_con.param1;
		root_element = root_element->NextSiblingElement();
	}
	
	for (std::map<int, std::map<int, int> >::const_iterator key_it = check_map.begin(); key_it != check_map.end(); ++key_it)
	{
		if ((int)key_it->second.size() >= 2)
		{
			return -5555;
		}
	}

	return 0;
}

int CourseOfGrowthConfig::InitCOGStarAward(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		int prog = 0, star = 0; COGAwardCon tmp_award;
		if (!GetSubNodeValue(root_element, "stage_id", prog) || prog < GROWTH_PROG_BEGINNING)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "star_id", star) || star < GROWTH_STAR_BEGINNING)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "reward_exp", tmp_award.reward_exp) || tmp_award.reward_exp < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "reward_coin", tmp_award.reward_coin) || tmp_award.reward_coin < 0)
		{
			return -4;
		}

		TiXmlElement * list_node = root_element->FirstChildElement("reward_item_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("reward_item");
			while (NULL != item_node)
			{
				if (tmp_award.reward_num >= MAX_COG_AWARD_NUM) return -10;
				if (!tmp_award.reward_list[tmp_award.reward_num++].ReadConfig(item_node))
				{
					return -5;
				}

				item_node = item_node->NextSiblingElement();
			}
		}

		m_cog_promote_award[prog][star] = tmp_award;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int CourseOfGrowthConfig::InitCOGBossGroup(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		int prog = 0, star = 0, group_id = 0;
		if (!GetSubNodeValue(root_element, "stage", prog) || prog < GROWTH_PROG_BEGINNING)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "star", star) || star < GROWTH_STAR_BEGINNING)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "monster_group", group_id) || NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(group_id))
		{
			return -3;
		}

		m_boss_group_map[prog][star] = group_id;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int CourseOfGrowthConfig::InitCourseEntry(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		CourseEntryCfg node_cfg;
		if (!GetSubNodeValue(root_element, "sn", node_cfg.seq) || node_cfg.seq < 0 || node_cfg.seq >= MAX_COURSE_ENTRY_LIMIT_NUM) 
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is sn(%d) < 0 || >= %d?", node_cfg.seq, node_cfg.seq, MAX_COURSE_ENTRY_LIMIT_NUM);
			return -1;
		}
		if (m_course_entry_map.end() != m_course_entry_map.find(node_cfg.seq))
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, not repeat?", node_cfg.seq);
			return -2;
		}

		if (!GetSubNodeValue(root_element, "cond_param1", node_cfg.param1)) 
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "cond_param2", node_cfg.param2))
		{
			return -4;
		}
		if (!GetSubNodeValue(root_element, "course_reward", node_cfg.course_reward) || node_cfg.course_reward <= 0)
		{
			errormsg = STRING_SPRINTF("[sn=%d] course_reward error, is course_reward(%d) <= 0?", node_cfg.seq, node_cfg.course_reward);
			return -5;
		}
		if (!GetSubNodeValue(root_element, "cond_type", node_cfg.condition_type) || node_cfg.condition_type < 0 || node_cfg.condition_type >= COG_CON_TYPE_MAX) 
		{
			errormsg = STRING_SPRINTF("[sn=%d] condition_type error, is condition_type(%d) < 0 || >= %d?", node_cfg.seq, node_cfg.condition_type, COG_CON_TYPE_MAX);
			return -6;
		}

		if (!this->CheckCondition(node_cfg.seq, node_cfg.condition_type, node_cfg.param1, node_cfg.param2, errormsg))
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "reset", node_cfg.version) || node_cfg.version < 0 || node_cfg.version >= MAX_UINT8)
		{
			errormsg = STRING_SPRINTF("[sn=%d] reset error, is reset(%d) < 0 || >= %d?", node_cfg.seq, node_cfg.version, MAX_UINT8);
			return -8;
		}

		m_course_entry_map[node_cfg.seq] = node_cfg;
		m_course_entry_list[node_cfg.condition_type].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int CourseOfGrowthConfig::InitCourseLevel(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	int last_level = 0;
	while (NULL != root_element)
	{
		int level = 0;
		if (!GetSubNodeValue(root_element, "course_level", level) || level <= 0 || last_level + 1 != level || level > MAX_COURSE_LEVEL_NUM)
		{
			errormsg = STRING_SPRINTF("[course_level=%d] error, is course_level[%d] <= 0 || != last_level[%d] + 1 || > %d?", level, level, last_level, MAX_COURSE_LEVEL_NUM);
			return -1;
		}
		last_level = level;

		CourseLevelRewardCfg node_cfg;
		if (!GetSubNodeValue(root_element, "need_fraction", node_cfg.need_fraction) || node_cfg.need_fraction < 0)
		{
			errormsg = STRING_SPRINTF("[course_level=%d] error, is need_fraction[%d] < 0?", level, node_cfg.need_fraction);
			return -2;
		}

		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp) || node_cfg.exp < 0)
		{
			errormsg = STRING_SPRINTF("[course_level=%d] error, is exp[%d] < 0?", level, node_cfg.exp);
			return -3;
		}

		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin) || node_cfg.coin < 0)
		{
			errormsg = STRING_SPRINTF("[course_level=%d] error, is coin[%d] < 0?", level, node_cfg.coin);
			return -4;
		}

		if (!GetSubNodeValue(root_element, "reset", node_cfg.version) || node_cfg.version < 0 || node_cfg.version >= MAX_UINT8)
		{
			errormsg = STRING_SPRINTF("[course_level=%d] reset error, is reset(%d) < 0 || >= %d?", level, node_cfg.version, MAX_UINT8);
			return -5;
		}

		TiXmlElement * list_node = root_element->FirstChildElement("reward_item_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("reward_item");
			while (NULL != item_node)		// 允许奖励道具为空
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_node))
				{
					errormsg = STRING_SPRINTF("[course_level=%d] error, is reward[%d, %d] < 0?", level, item.item_id, item.num);
					return -111;
				}

				node_cfg.reward_list.push_back(item);
				item_node = item_node->NextSiblingElement();
			}
		}

		m_course_level_reward_cfg[level] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

bool CourseOfGrowthConfig::CheckCondition(int seq, int condition, int param1, int param2, std::string & errormsg)
{
	bool is_param2_jud = false;
	UNSTD_STATIC_CHECK(77 == COG_CON_TYPE_MAX)
	switch (condition)
	{
		case COG_CON_WIN_FIGHT_TIMES_ACC:
		case COG_CON_OBTAIN_COIN_NUMBERS:
		case COG_CON_OBTAIN_TITLE_NUMBER:
		case COG_CON_STUDY_BATTLE_SKILLS:
		case COG_CON_PUT_ON_EQUIPM_TIMES:
		case COG_CON_PET_ALLOT_ATTRPOINT:
		case COG_CON_PET_TRAPS_CREATURES:
		case COG_CON_PET_INTENSIFY_TIMES:
		case COG_CON_PET_REPLACE_TALENTS:
		case COG_CON_ACTIVATE_COLLECTION:
		case COG_CON_TRADEMARKET_PUTAWAY:
		case COG_CON_TRADEMARKET_DEALING:
		case COG_CON_FINISHED_TASK_CHAIN:
		case COG_CON_EXECUTES_SEAL_ORDER:
		case COG_CON_CONSUME_DOUBLE_EXP:
		case COG_CON_PET_SCOUR_TALENTS:
		case COG_CON_BOUNTY_TASK_DONE:
		case COG_CON_EQUIPMENT_SMELT:
		case COG_CON_PASS_CRYSTAL_XI_YOU:
		case COG_CON_FIRST_ADD_GUILD:
		case COG_CON_GUILD_TEAM_FIGHT:
		case COG_CON_FETCH_GUILD_HONGBAO:
		case COG_CON_GUILD_ANSWER:
		case COG_CON_FETCH_FRIEND_GIFT:
		case COG_CON_JIE_YI_DAY_NUM:
		case COG_CON_WIN_CHIEF_ELECTION:
		case COG_CON_CLOUD_ARENA_SCORE:
		case COG_CON_APPRENTICE_COUNT:
		case COG_CON_APPRENTICE_DAY_NUM:
		case COG_CON_YIN_YANG_EYE_REWARD:
		case COG_CON_USE_CRYSTAL_ENERGY:
		case COG_CON_HUAN_JIE_ZHAN_CHANG:
		case COG_CON_CHIVALROUS:
		if (param1 != 0) 
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) != 0?", seq, param1);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_REACHS_PRESTIGE_NUM:
		if (NULL == LOGIC_CONFIG->GetPrestigeConfig()->GetVillageCfg(param1))
		{	
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) not find village?", seq, param1);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_PASS_CRYSTAL_ECTYPE:
		if (param1 < 0 || param1 >= ROLE_CRYSTAL_FB_TYPE_MAX) 
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < 0 || >= %d?", seq, param1, ROLE_CRYSTAL_FB_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_REACHS_FB_TIER_NUMB:
		if (param1 <= 0 || param1 >= FBManager::FB_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) <= 0 || >= %d?", seq, param1, FBManager::FB_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_DEFEAT_RACE_MONSTER:
		case COG_CON_PET_OBTAIN_RACE_NUM:
		if (param1 < RACE_TYPE_MIN || param1 >= RACE_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < %d || >= %d?", seq, param1, RACE_TYPE_MIN, RACE_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_DEFEAT_BOSS_MONSTER:
		if (param1 < 0 || param1 >= COG_BOSS_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < 0 || >= %d?", seq, param1, COG_BOSS_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_SKILLS_REACHS_LEVEL:
		case COG_CON_PET_NUM_REACH_LEVEL:
		case COG_CON_PARTNER_UP_LEVEL:
		if (param1 <= 0 || param2 <= 0)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) <= 0 || param2 <= 0?", seq, param1, param2);
			return false;
		}
		break;
		case COG_CON_ACCOMPLISH_ACTIVITY:
		if (param1 < 0 || param1 >= COG_ACT_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < 0 || >= %d?", seq, param1, COG_ACT_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_USE_APPOINTED_ITEMS:
		if (param1 < 0 || param1 >= I_TYPE_NET_VALUE_ITEM_END)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < 0 || >= %d?", seq, param1, I_TYPE_NET_VALUE_ITEM_END);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_USE_NOREXITEM_TYPES:
		if (param1 <= 0 || param1 >= NorexItem::I_NOREX_ITEM_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) <= 0 || >= %d?", seq, param1, NorexItem::I_NOREX_ITEM_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_REACHS_APPOINTED_LV:
		if (param1 <= 0 || param1 > MAX_ROLE_LEVEL)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) <= 0 || > %d?", seq, param1, MAX_ROLE_LEVEL);
			return false;
		}
		break;
		case COG_CON_APPOINTED_TASK_DONE:
		if (NULL == TASKPOOL->GetTaskCfgByID(param1))
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) not find task?", seq, param1);
			return false;
		}
		break;
		case COG_CON_CARRY_OUT_TYPE_TASK:
		if (param1 < 0 || param1 >= COG_TASK_TYPE_MAX) 
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < 0 || >= %d?", seq, param1, COG_TASK_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_WORKSHOP_CRAFT_ITEM:
		if (param1 < 0 || param1 >= COG_WORKSHOP_MAKE_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < 0 || >= %d?", seq, param1, COG_WORKSHOP_MAKE_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_GATHER_TIMES_ACCUMU:
		if (param1 < 0 || param1 >= COG_GATHER_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < 0 || >= %d?", seq, param1, COG_GATHER_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_PET_PRACTICE_LEVEL:
		CheckResourceCenter::Instance().GetCourseOfGrowthCheck()->Add(param1, __FUNCTION__);
		break;
		case COG_CON_MIYI_KILL_LAYER_NUM:
		if (param1 < 0 || param1 >= MI_YI_MAX_LEVEL)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < 0 || >= %d?", seq, param1, MI_YI_MAX_LEVEL);
			return false;
		}
		if (param2 <= 0 || param2 > MI_YI_MAX_MONSTER_NUM)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param2(%d) <= 0 || > %d?", seq, param2, MI_YI_MAX_MONSTER_NUM);
			return false;
		}
		break;
		case COG_CON_KILL_MONTER_GROUP:
		if (NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(param1))
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) not find monster group?", seq, param1);
			return false;
		}
		if (param2 != 1)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param2(%d) != 1?", seq, param2);
			return false;
		}
		break;
		case COG_CON_PET_MONOPOLY_LEVEL:	//玩家初始就是第一层
		if (param2 <= 1)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) <= 1?", seq, param2);
			return false;
		}
		break;
		case COG_CON_TRAIN_SKILL_LEVEL:
		if (param1 <= 0)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) <= 0?", seq, param1);
			return false;
		}
		if (param2 <= 0 || param2 > TARIN_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param2(%d) <= 0 || > %d?", seq, param2, TARIN_TYPE_MAX);
			return false;
		}
		break;
		case COG_CON_LUN_HUI_WANG_CHUAN:
		case COG_CON_SHAN_HAI_XIANG_YAO:
		is_param2_jud = true;
		break;
		case COG_CON_REALM_BREAK_LEVEL:		//玩家初始境界就是0
		if (param1 <= 0)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) <= 1?", seq, param1);
			return false;
		}
		if (param2 != 1)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param2(%d) != 1?", seq, param2);
			return false;
		}
		break;
		case COG_CON_PET_INTENSIFY_LEVEL:
		if (param1 < 0 || param1 >= MAX_PET_QUALITY_NUM)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < 0 || >= %d?", seq, param1, MAX_PET_QUALITY_NUM);
			return false;
		}
		if (param2 <= 0 || param2 > PET_STRENGTHEN_MAX_LEVEL)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param2(%d) <= 0 || > %d?", seq, param2, PET_STRENGTHEN_MAX_LEVEL);
			return false;
		}
		break;
		case COG_CON_CRYSTAL_FB_XI_YOU:
		case COG_CON_REAML_BREAK_FIGHT:
		if (param1 < 0)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < 0?", seq, param1);
			return false;
		}
		if (param2 != 0)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param2(%d) != 0?", seq, param2);
			return false;
		}
		break;
		case COG_CON_REACH_CAPABILITY:
		if (param1 < 0 || param1 >= COG_CON_REACH_CAPABILITY_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < 0 || >= %d?", seq, param1, COG_CON_REACH_CAPABILITY_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_GET_QUALITY_PET:
		case COG_CON_SCOUR_TALENTS_MUT:
		if (param1 < PET_QUALITY_INVALID || param1 >= PET_QUALITY_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < %d || >= %d?", seq, param1, PET_QUALITY_INVALID, COG_CON_REACH_CAPABILITY_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_HAS_FA_BAO_COUNT:
		if (param1 < ItemBase::I_COLOR_INVALID || param1 > ItemBase::I_COLOR_RED)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < %d || >= %d?", seq, param1, ItemBase::I_COLOR_INVALID, ItemBase::I_COLOR_RED);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_HAS_ACTIVE_FACE:
		if (param1 < -1 || param1 >= FACE_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < -1 || >= %d?", seq, param1, FACE_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_HAS_SURFACE:
		if (param1 < 0 || param1 >= SURFACE_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < 0 || >= %d?", seq, param1, SURFACE_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_GET_MONEY:
		if (param1 < MONEY_TYPE_GOLD || param1 >= MONEY_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < %d || >= %d?", seq, param1, MONEY_TYPE_GOLD, MONEY_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_PARTICIPATE_ACT:
		case COG_CON_ACTIVITY_FIGHT_WIN:
		if (param1 <= ACTIVITY_TYPE_INVALID || param1 >= ACTIVITY_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < %d || >= %d?", seq, param1, ACTIVITY_TYPE_INVALID, ACTIVITY_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_GUILD_FIGHT_RANK:
		case COG_CON_GUILD_HONOR_RANK:
		if (param1 <= 0)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) <= 0?", seq, param1);
			return false;
		}
		if (param2 != 0)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param2(%d) != 0?", seq, param2);
			return false;
		}
		break;
		case COG_CON_CHANNEL_CHAT:
		if (param1 < 0 || param1 >= chatdef::CHANNEL_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < 0 || >= %d?", seq, param1, chatdef::CHANNEL_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_HAS_FRIEND_COUNT:
		if (param1 < 0)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < 0?", seq, param1);
		}
		is_param2_jud = true;
		break;
		case COG_CON_USE_HIGH_TREASURE:
		if (param1 < TREASURE_MAP_ITEM_RANK_TYPE_HIGH || param1 >= TREASURE_MAP_ITEM_RANK_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) < %d || >= %d?", seq, param1, TREASURE_MAP_ITEM_RANK_TYPE_HIGH, TREASURE_MAP_ITEM_RANK_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_SHAN_HAI_BOSS:
		if (param1 <= SHAN_HAI_BOSS_GRADE_TYPE_INVAILD || param1 >= SHAN_HAI_BOSS_GRADE_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) <= %d || >= %d?", seq, param1, SHAN_HAI_BOSS_GRADE_TYPE_INVAILD, SHAN_HAI_BOSS_GRADE_TYPE_MAX);
			return false;
		}
		is_param2_jud = true;
		break;
		case COG_CON_ANECDOTE_TASK:
		if (param1 <= ANECDOTE_TASK_TYPE_INVALID || param1 >= ANECDOTE_TASK_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param1(%d) <= %d || >= %d?", seq, param1, ANECDOTE_TASK_TYPE_INVALID, ANECDOTE_TASK_TYPE_MAX);
			return false;
		}
		if (param2 != 0)
		{
			errormsg = STRING_SPRINTF("[sn=%d] error, is param2(%d) != 0?", seq, param2);
		}
		break;
		default:
		errormsg = STRING_SPRINTF("[sn=%d] error, is condition(%d) < 0 || >= %d?", seq, condition, COG_CON_TYPE_MAX);
		return false;
	}
	if (is_param2_jud && param2 <= 0)
	{
		errormsg = STRING_SPRINTF("[sn=%d] error, is param2(%d) <= 0?", seq, param2);
		return false;
	}

	return true;
}
