#include <memory.h>

#include "taskpool.h"
#include "world.h"
#include "engineadapter.h"

#include "globalconfig/globalconfig.h"

#include "servercommon/servercommon.h"
#include "servercommon/prestigedef.hpp"

#include "obj/character/role.h"
#include "item/itempool.h"
#include "drop/droppool.hpp"
#include "scene/fbmanager.hpp"
#include "monster/monsterpool.hpp"
#include "npc/npcpool.h"
#include "scene/scenemanager.hpp"
#include "other/funcguide/funcguide.hpp"
#include "other/rolemodulemanager.hpp"
#include "checkresourcecenter.hpp"
#include "servercommon/anecdotetaskdef.hpp"
#include "servercommon/prestigetaskdef.hpp"

TaskPool::TaskPool() : m_record_task(0)
{
	memset(m_task_list, 0, sizeof(m_task_list));
	memset(m_first_bounty_task_list, 0, sizeof(m_first_bounty_task_list));
	memset(m_acpb_task, 0, sizeof(m_acpb_task));
	memset(m_prof_task, 0, sizeof(m_prof_task));
	memset(m_task_type_list, 0, sizeof(m_task_type_list));
	memset(m_level_adv_task_list, 0, sizeof(m_level_adv_task_list));

	m_robot_mission_begin_task = 0;
}

TaskPool::~TaskPool()
{
	for (int i = 0; i < MAX_UINT16; i++)
	{
		if (NULL != m_task_list[i])
		{
			delete m_task_list[i];
		}
	}
	memset(m_task_list, 0, sizeof(m_task_list));

	for (int i = 0; i < MAX_ROLE_LEVEL + 1; ++i)
	{
		if (NULL != m_acpb_task[i])
		{
			delete m_acpb_task[i];
		}
	}
	memset(m_acpb_task, 0, sizeof(m_acpb_task));

	for (int i = 0; i < PROFESSION_TYPE_NUM; ++i)
	{
		if (NULL != m_prof_task[i])
		{
			delete m_prof_task[i];
		}
	}
	memset(m_prof_task, 0, sizeof(m_prof_task));

	for (int i = 0; i < TASK_TYPE_MAX; ++i)
	{
		if (NULL != m_task_type_list[i])
		{
			delete m_task_type_list[i];
		}
	}
	memset(m_task_type_list, 0, sizeof(m_task_type_list));

	memset(m_level_adv_task_list, 0, sizeof(m_level_adv_task_list));
}

TaskPool * gs_task_pool_cfg = NULL;

TaskPool * TaskPool::Instance()
{
	if (NULL == gs_task_pool_cfg)
	{
		gs_task_pool_cfg = new TaskPool();
	}

	return gs_task_pool_cfg;
}

bool TaskPool::Init(const std::string &dir, std::string *err)
{
	PRE_LOAD_MULTI_LIST;

	{
		READ_NEW_LIST("tasklist.xml");			// 主线、支线任务表

		LOAD_CONFIG("drop_group", InitDefeatsGroupCfg);					// 击杀组配置
		LOAD_CONFIG("monsters_group", InitMonsterGroupCfg);				// 怪物组配置
		LOAD_CONFIG("item_group", InitItemOpeGroupCfg);					// 道具组配置
		LOAD_CONFIG("task_award", InitCommonRewardCfg);					// 通常任务报酬配置 - 奖励必须先于任务读取
		LOAD_CONFIG("task_list", InitAllTaskInforCfg);					// 任务信息列表配置
	
		//LOAD_CONFIG("monster_grow", InitBossChapterBossCfg);			// 怪物成长之路
		//LOAD_CONFIG("chapter_award", InitBossChapterChapterCfg);		// 怪物成长之路章节奖励
	
		LOAD_CONFIG("other", InitOtherCfg);
		LOAD_CONFIG("task_to_item_type_3", InitConditionItemCfg);
		LOAD_CONFIG("special_rewards", InitSpecialRewardsCfg);

		if (!this->CheckMainTask(*err))
		{
			return false;
		}
	}

	{
		READ_NEW_LIST("challengetask_list.xml");		// 挑战任务表

		LOAD_CONFIG("group_monster", InitMonsterGroupCfg);				// 怪物组配置
		LOAD_CONFIG("item_group", InitItemOpeGroupCfg);					// 道具组配置
		LOAD_CONFIG("task_award", InitCommonRewardCfg);					// 通常任务报酬配置 - 奖励必须先于任务读取
		LOAD_CONFIG("task_list", InitAllTaskInforCfg);					// 任务信息列表配置
	}

	{
		READ_NEW_LIST("huanshi_challenge.xml");	// 奇闻任务
		LOAD_CONFIG("task_list", InitAllTaskInforCfg);
		LOAD_CONFIG("team_challenge_reward", InitTeamChallengeRewardCfg);					// 组队挑战任务奖励
	}
	
	{
		READ_NEW_LIST("medaltask_list.xml");		// 勋章任务表

		LOAD_CONFIG("group_monster", InitMonsterGroupCfg);
		LOAD_CONFIG("item_group", InitItemOpeGroupCfg);
		LOAD_CONFIG("task_award", InitCommonRewardCfg);
		LOAD_CONFIG("task_list", InitAllTaskInforCfg);
	}

	{
		READ_NEW_LIST("advanced_tasks.xml");			// 进阶任务表

		LOAD_CONFIG("drop_group", InitDefeatsGroupCfg);					// 击杀组配置
		LOAD_CONFIG("monsters_group", InitMonsterGroupCfg);				// 怪物组配置
		LOAD_CONFIG("item_group", InitItemOpeGroupCfg);					// 道具组配置
		LOAD_CONFIG("task_award", InitCommonRewardCfg);					// 通常任务报酬配置 - 奖励必须先于任务读取
		LOAD_CONFIG("task_list", InitAllTaskInforCfg);					// 任务信息列表配置
	}
	
	{
		READ_NEW_LIST("strange_tasklist.xml");			// 奇遇任务表

		LOAD_CONFIG("drop_group", InitDefeatsGroupCfg);					// 击杀组配置
		LOAD_CONFIG("monsters_group", InitMonsterGroupCfg);				// 怪物组配置
		LOAD_CONFIG("item_group", InitItemOpeGroupCfg);					// 道具组配置
		LOAD_CONFIG("task_award", InitCommonRewardCfg);					// 通常任务报酬配置 - 奖励必须先于任务读取
		LOAD_CONFIG("task_list", InitAllTaskInforCfg);					// 任务信息列表配置
		LOAD_CONFIG("activation_type", InitAdventureGetCfg);			// 奇遇任务激活方式配置
		LOAD_CONFIG("other", InitAdvTaskOtherCfg);						// 奇遇任务 其它 配置
		LOAD_CONFIG("mission_group", InitAdvTaskIDGroupCfg);			// 奇遇任务组
	}

	{
		READ_NEW_LIST("feisheng_tasklist.xml");			// 飞升任务表

		LOAD_CONFIG("drop_group", InitDefeatsGroupCfg);					// 击杀组配置
		LOAD_CONFIG("task_award", InitCommonRewardCfg);					// 通常任务报酬配置 - 奖励必须先于任务读取
		LOAD_CONFIG("task_list", InitAllTaskInforCfg);					// 任务信息列表配置
	}


	{
		READ_NEW_LIST("pet_fs_tasklist.xml");			// 宠物飞升任务表
		LOAD_CONFIG("task_list", InitAllTaskInforCfg);					// 任务信息列表配置
	}

	{
		READ_NEW_LIST("partner_fs_tasklist.xml");			// 飞升任务表

		LOAD_CONFIG("drop_group", InitDefeatsGroupCfg);					// 击杀组配置
		LOAD_CONFIG("task_award", InitCommonRewardCfg);					// 通常任务报酬配置 - 奖励必须先于任务读取
		LOAD_CONFIG("task_list", InitAllTaskInforCfg);					// 任务信息列表配置
		LOAD_CONFIG("monsters_group", InitMonsterGroupCfg);				// 怪物组配置
		LOAD_CONFIG("item_group", InitItemOpeGroupCfg);					// 道具组配置
	}

	{
		READ_NEW_LIST("bounty_tasklist.xml");		// 赏金任务表

		LOAD_CONFIG("monsters_group", InitMonsterGroupCfg);
		LOAD_CONFIG("drop_group", InitDefeatsGroupCfg);
		LOAD_CONFIG("item_group", InitItemOpeGroupCfg);
		LOAD_CONFIG("task_award", InitCommonRewardCfg);
		LOAD_CONFIG("task_list", InitAllTaskInforCfg);
		LOAD_CONFIG("other", InitBountyTaskOtherCfg);
		LOAD_CONFIG("reward_group", InitBountyTaskTurnTableCfg);
		LOAD_CONFIG("turn_table", InitBountyTaskTurnTableGroupCfg);
		LOAD_CONFIG("task_level", InitBountyTaskLevelLimitCfg);
		LOAD_CONFIG("challenge_go_on", InitNextBountyTaskCfg);
		LOAD_CONFIG("job_relevant", InitFirstBountyTaskCfg);
		LOAD_CONFIG("reward_find", InitBountyTaskFindCfg);
		LOAD_CONFIG("reward_find_group", InitBountyTaskFindRewardCfg);
		LOAD_CONFIG("first_task", InitBountyFirstTaskCfg);
		LOAD_CONFIG("finish_reward", InitBountyTaskFinishRewardCfg);
	}

	{
		READ_NEW_LIST("logicconfig/transaction_task.xml");		// 购买任务 - 丝绸之路

		LOAD_CONFIG("task_list", InitAllTaskInforCfg);
	}

	{
		READ_NEW_LIST("anecdotes_tasklist.xml");	// 奇闻任务
		LOAD_CONFIG("item_group", InitItemOpeGroupCfg);					// 道具组配置
		LOAD_CONFIG("task_list", InitAllTaskInforCfg);
		LOAD_CONFIG("task_award", InitCommonRewardCfg);
		LOAD_CONFIG("task_list", InitAnecdoteTaskCfg);
		LOAD_CONFIG("anecdotes_award", InitAnecdoteTaskFinalRewardCfg);
		LOAD_CONFIG("task_type_monster", InitAnecdoteTaskMonsterGroupCfg);
		LOAD_CONFIG("other", InitAnecdoteOtherCfg);
		LOAD_CONFIG("event_reward", InitAnecdoteBigEventRwardCfg);
		LOAD_CONFIG("challenge_anecdote", InitAnecdoteSpecialCfg);
	}

	{
		READ_NEW_LIST("logicconfig/prestige_task.xml");
		LOAD_CONFIG("task", InitPrestigeTaskVillageCfg);
		LOAD_CONFIG("task_line", InitAllTaskInforCfg);
		LOAD_CONFIG("task_line", InitPrestigeTaskCfg);
		LOAD_CONFIG("star_reward", InitPrestigeTaskStarCfg);
		LOAD_CONFIG("other", InitPrestigeTaskOtherCfg);

		{
			TiXmlElement *root_element = RootElement->FirstChildElement("reward_group"); 
			if (NULL == root_element)
			{
				*err = configname + ": no [reward_group].";
				return false; 
			}
			iRet = this->InitPrestigeTaskPrestigeGroupCfg(root_element, m_prestige_group_cfg_map);
			if (iRet)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: %s failed %d", configname.c_str(), "InitPrestigeTaskPrestigeGroupCfg(reward_group)", iRet);
				*err = errinfo;
				return false;
			}
		}

		{
			TiXmlElement *root_element = RootElement->FirstChildElement("end_reward_group");
			if (NULL == root_element)
			{
				*err = configname + ": no [end_reward_group].";
				return false;
			}
			iRet = this->InitPrestigeTaskPrestigeGroupCfg(root_element, m_end_prestige_group_cfg_map);
			if (iRet)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: %s failed %d", configname.c_str(), "InitPrestigeTaskPrestigeGroupCfg(end_reward_group)", iRet);
				*err = errinfo;
				return false;
			}
		}
	}

	{
		READ_NEW_LIST("logicconfig/family_daily_mission.xml");			// 角色家族任务
		LOAD_CONFIG("mission_list", InitAllTaskInforCfg);
	}

	int ret = this->CheckPrevNextTask();
	if (ret < 0)
	{
		*err += "CheckPrevNextTask Failed";
		return false;
	}

	if (m_record_task > MAX_TASK_RECORD_NUM)
	{
		*err = STRING_SPRINTF("Total task num[%d] exceed record limit[%d]", m_record_task, MAX_TASK_RECORD_NUM);
		return false;
	}

	this->SetPetFeiShengFirstTask();

	return true;
}

bool TaskPool::Reload(const std::string & config_dir, std::string * err)
{
	TaskPool *temp = new TaskPool();

	if (!temp->Init(config_dir, err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != gs_task_pool_cfg) delete gs_task_pool_cfg;

	gs_task_pool_cfg = temp;
	return true;
}

bool TaskPool::IsMonsterGroupsMember(int monster_group, int monster_id)
{
	std::map<int, std::vector<int> >::iterator m_it = m_monster_group.find(monster_group);
	if (m_monster_group.end() == m_it) return false;

	for (std::vector<int>::iterator v_it = m_it->second.begin(); v_it != m_it->second.end(); ++v_it)
	{
		if (monster_id == *v_it) return true;
	}
	
	return false;
}

const std::vector<int>* TaskPool::GetMonsterGroupMonsterList(int monster_group)
{
	std::map<int, std::vector<int> >::iterator m_it = m_monster_group.find(monster_group);
	if (m_monster_group.end() == m_it) return NULL;

	return &m_it->second;
}

bool TaskPool::IsItemGroupsMember(int item_group, int item_id)
{
	std::map<int, std::set<ItemID> >::iterator m_it = m_item_group.find(item_group);
	if (m_item_group.end() == m_it || m_it->second.end() == m_it->second.find(item_id)) return false;

	return true;
}

const std::set<ItemID>* TaskPool::GetItemGroupList(int item_group)
{
	std::map<int, std::set<ItemID> >::const_iterator it = m_item_group.find(item_group);
	if (it != m_item_group.end())
	{
		return &it->second;
	}
	return NULL;
}

bool TaskPool::IsInRightOrder(int monster_group, int monster_id, int order)
{
	std::map<int, std::vector<int> >::iterator m_it = m_monster_group.find(monster_group);
	if (m_monster_group.end() == m_it || order < 0 || order >= (int)m_it->second.size()) return false;

	return monster_id == m_it->second[order];
}

void TaskPool::GetPrevTaskList(TaskID task_id, std::set<TaskID> & result_list)
{
	result_list.clear();

	std::map<TaskID, std::set<TaskID> >::iterator s_it = m_prev_task.find(task_id);
	if (m_prev_task.end() != s_it) result_list = s_it->second;
}

void TaskPool::GetNextTaskList(TaskID task_id, std::set<TaskID> & result_list)
{
	result_list.clear();	// 确保当没有后续任务时，传出的容器为空

	std::map<TaskID, std::set<TaskID> >::iterator s_it = m_next_task.find(task_id);
	if (m_next_task.end() != s_it) result_list = s_it->second;
}

const std::set<TaskID>* TaskPool::GetTaskListAtLv(UInt16 min_lv)
{
	if (min_lv < 0 || min_lv > MAX_ROLE_LEVEL) return NULL;

	return m_acpb_task[min_lv];
}

const std::set<TaskID>* TaskPool::GetTaskListWithBaseProf(int base_prof_type)
{
	if (base_prof_type < 0 || base_prof_type >= PROFESSION_TYPE_NUM) return NULL;

	return m_prof_task[base_prof_type];
}

const std::set<TaskID>* TaskPool::GetTaskListAtTaskType(int task_type)
{
	if(task_type < 0 || task_type >= TASK_TYPE_MAX) return NULL;

	return m_task_type_list[task_type];
}

const TaskInfomaConfig * TaskPool::GetTaskCfgByID(int p_task_id)
{
	if(p_task_id >= MAX_UINT16 || p_task_id < 0) return NULL;

	return m_task_list[p_task_id];
}

const CommonTaskReward * TaskPool::GetCommonTaskReward(int p_task_id, Role * p_role)
{
	if (NULL == p_role) return NULL;

	std::map<TaskID, std::vector<CommonTaskReward> >::iterator m_f_it = m_common_task_reward_cfg.find(p_task_id);
	if (m_common_task_reward_cfg.end() == m_f_it) return NULL;		// 如果没有奖励，直接返回

	const TaskInfomaConfig* task_cfg = this->GetTaskCfgByID(p_task_id);
	if (NULL == task_cfg) return NULL;

	if (task_cfg->task_id == 0) return NULL;

	int param = 0;
	UNSTD_STATIC_CHECK(4 == ALLOT_TYPE_MAX);
	switch (task_cfg->reward_allot)
	{
	case ALLOT_TYPE_COMMON:
		{
			if (m_f_it->second.empty()) return NULL;

			return & *m_f_it->second.begin();
		}
		break;

	case ALLOT_TYPE_ATPROF:
		param = p_role->GetProfession();
		break;

	case ALLOT_TYPE_ATLEVE:
		param = p_role->GetLevel();
		break;

	case ALLOT_TYPE_PET_CHOICE:
		param = p_role->GetRoleModuleManager()->GetFuncGuide()->GetPetChoice();
		break;

	default:
		{
			return NULL;
		}
		break;
	}

	for (std::vector<CommonTaskReward>::iterator m_v_it = m_f_it->second.begin(); m_v_it != m_f_it->second.end(); ++m_v_it)
	{
		if (param >= m_v_it->allot_min && param <= m_v_it->allot_max) return & *m_v_it;
	}

	return NULL;
}

const DefeatMonsGroups * TaskPool::GetDefeatMonsGroups(int group_id)
{
	std::map<int, DefeatMonsGroups>::iterator find_it = m_defeat_mon_group.find(group_id);
	if (m_defeat_mon_group.end() != find_it) return & find_it->second;

	return NULL;
}

int TaskPool::RandAdventureTask(int level, std::set<int>* unlocked_partner_set /* = NULL */)
{
	//下面是根据伙伴情况重算概率
	int real_prob = RAND_UNIVERSAL_RATIO_BASE_NUM;		//重组后概率
	std::vector<AdvTaskActivates> real_lv_map;			//重组后任务组表
	if (unlocked_partner_set != NULL)
	{
		std::vector<AdvTaskActivates>* adv_task_list = m_level_adv_task_list[level];
		if (NULL != adv_task_list)
		{
			real_lv_map.reserve(adv_task_list->size());
			for (std::vector<AdvTaskActivates>::iterator task_it = adv_task_list->begin(); task_it != adv_task_list->end(); ++task_it)
			{
				static AdvTaskActivates node;
				node.mission_group = task_it->mission_group;
				node.prob = task_it->prob;
				node.partner_id = task_it->partner_id;
				node.partner_pro = task_it->partner_pro;

				if (unlocked_partner_set->find(task_it->partner_id) != unlocked_partner_set->end())
				{
					node.prob += task_it->partner_pro;
					real_prob += task_it->partner_pro;
				}

				real_lv_map.push_back(node);
			}
		}
	}
	else
	{
		std::vector<AdvTaskActivates>* adv_task_list = m_level_adv_task_list[level];
		if (NULL != adv_task_list)
		{
			real_lv_map.assign(adv_task_list->begin(), adv_task_list->end());
		}
	}
	if (real_prob <= 0)return 0;

	int rate = rand() % real_prob;

	for (std::vector<AdvTaskActivates>::iterator task_it = real_lv_map.begin(); task_it != real_lv_map.end(); ++task_it)
	{
		if (rate < task_it->prob) return task_it->mission_group;
		rate -= task_it->prob;
	}

	return 0;
}

int TaskPool::GetAdventureTaskByUsingItem(int item_id, int level)
{
	std::map<ItemID, std::map<ParamSection, TaskID> >::iterator item_it = m_item_act_task_map.find(item_id);
	if (m_item_act_task_map.end() == item_it) return 0;
	
	for (std::map<ParamSection, TaskID>::iterator lv_it = item_it->second.begin(); lv_it != item_it->second.end(); ++lv_it)
	{
		if (level >= lv_it->first.section_min && level <= lv_it->first.section_max) return lv_it->second;
	}

	return 0;
}

int TaskPool::GetAdventureTaskTimeLimit(int task_id)
{
	std::map<TaskID, int>::iterator task_it = m_adv_task_time_limit.find(task_id);
	if (m_adv_task_time_limit.end() != task_it) return task_it->second;

	return 0;
}

int TaskPool::GetAdventureTaskByGroupID(int group_id)
{
	std::map<int, AdvTaskGroupList>::iterator it = m_adv_task_group_map.find(group_id);
	if (it != m_adv_task_group_map.end())
	{
		int dis_count = rand() % it->second.weight;
		for (std::vector<AdvTaskGroup>::iterator it2 = it->second.m_task_list.begin(); it2 != it->second.m_task_list.end();it2++)
		{
			if (dis_count < it2->active_probability_10000)
			{
				return it2->activation_task_id;
			}
			dis_count -= it2->active_probability_10000;
		}
	}
	return 0;
}

TaskID TaskPool::RandomBountyTask(int role_lv, int accepted_task_num, ARG_IN TaskID* accepted_task_list)
{
	std::vector<int> task_list;
	task_list.reserve(m_all_bounty_task_list.size());
	for (size_t i = 0; i < m_all_bounty_task_list.size(); ++i)
	{
		// 首任务不再随机
		if (m_all_bounty_task_list[i].is_first_task)
		{
			continue;
		}

		if (role_lv >= m_all_bounty_task_list[i].min_level &&
			role_lv <= m_all_bounty_task_list[i].max_level)
		{
			bool is_found = false;
			for (int k = 0; k < accepted_task_num; ++k)
			{
				if (accepted_task_list[k] == m_all_bounty_task_list[i].task_id)
				{
					is_found = true;
					break;
				}
			}
			if (is_found)
			{
				continue;
			}
			
			task_list.push_back(m_all_bounty_task_list[i].task_id);
		}
	}
	
	if (task_list.size() > 0)
	{
		int idx = rand() % task_list.size();
		return task_list[idx];
	}

	return 0;
}

const BossChapterBossCfg* TaskPool::GetBossChapterBossCfg(int monster_group_id)
{
	BossChapterBossCfgMap::iterator it = m_boss_chapter_boss_cfg_map.find(monster_group_id);
	if (it == m_boss_chapter_boss_cfg_map.end()) return NULL;

	return &it->second;
}

const TaskPool::ChapterBossList* TaskPool::GetChapterBossList(int chapter)
{
	if (chapter <= 0 || chapter >= MAX_BOSS_CHAPTER) return NULL;

	return &m_boss_chapter_chapter_boss_list[chapter];
}

const BossChapterChapterRewardCfg* TaskPool::GetBossChapterChapterRewardCfg(int chapter)
{
	if (chapter <= 0 || chapter >= MAX_BOSS_CHAPTER) return NULL;

	return &m_boss_chapter_chapter_reward_cfg_list[chapter];
}

int TaskPool::RandomBountyTaskTurnTableReward(int role_level, bool is_first)
{
	const BountyTaskTurnTableCfg* cfg = this->GetBountyTaskTurnTableCfg(role_level);
	if (NULL == cfg) return -1;

	//if (is_first)
	//{
	//	return cfg->first_reward_index;
	//}

	int r = rand() % cfg->total_rate;
	for (int i = 0; i < cfg->reward_num && i < BountyTaskTurnTableCfg::TABLE_REWARD_NUM; ++i)
	{
		if (r < cfg->rate_list[i])
		{
			return i;
		}
	}
	return -1;
}

const BountyTaskTurnTableCfg* TaskPool::GetBountyTaskTurnTableCfg(int role_level)
{
	for (size_t i = 0; i < m_bounty_turn_table_cfg_list.size(); ++i)
	{
		if (role_level >= m_bounty_turn_table_cfg_list[i].min_level &&
			role_level <= m_bounty_turn_table_cfg_list[i].max_level)
		{
			return &m_bounty_turn_table_cfg_list[i];
		}
	}
	return NULL;
}

int TaskPool::GetBountyTaskGroupId(int role_level)
{
	for (size_t i = 0; i < m_bounty_turn_table_cfg_list.size(); ++i)
	{
		if (role_level >= m_bounty_turn_table_cfg_list[i].min_level &&
			role_level <= m_bounty_turn_table_cfg_list[i].max_level)
		{
			return m_bounty_turn_table_cfg_list[i].reward_group_id;
		}
	}
	return 0;
}

const BountyTaskTurnTableCfg* TaskPool::GetBountyTaskTurnTableCfgByGroupId(int group_id)
{
	for (size_t i = 0; i < m_bounty_turn_table_cfg_list.size(); ++i)
	{
		if (m_bounty_turn_table_cfg_list[i].reward_group_id == group_id)
		{
			return &m_bounty_turn_table_cfg_list[i];
		}
	}
	return NULL;
}

TaskID TaskPool::GetNextBountyTaskID(TaskID task_id)
{
	NextBountyTaskMap::iterator it = m_next_bounty_task_map.find(task_id);
	if (it != m_next_bounty_task_map.end())
	{
		if (task_id == it->second)
		{
			return 0;
		}
		else
		{
			return it->second;
		}
	}

	return 0;
}

TaskID TaskPool::GetFirstBountyTaskID(int prof_base_type)
{
	if (prof_base_type <= 0 || prof_base_type >= PROFESSION_TYPE_NUM) return 0;

	return m_first_bounty_task_list[prof_base_type];
}

const BountyTaskFindRewardCfg * TaskPool::GetBountyTaskFindRewardCfgByGroupId(int group_id)
{
	std::map<int, std::vector<BountyTaskFindRewardCfg> >::iterator it = m_bounty_task_find_reward_cfg.find(group_id);
	if(it == m_bounty_task_find_reward_cfg.end()) return NULL;

	int rand_num = rand() % RATE_COUNT_MAX_NUM;
	int count = 0;
	for(int i = 0; i < (int)it->second.size(); i++)
	{
		count += it->second[i].rate;
		if (count >= rand_num)
		{
			return &it->second[i];
		}
	}
	return NULL;
}

const BountyTaskFindCfg * TaskPool::GetBountyTaskFindCfg(int level, int find_type)
{
	for (std::map<int, std::map<int, BountyTaskFindCfg> >::iterator it = m_bounty_task_find_cfg.begin(); it != m_bounty_task_find_cfg.end(); it++)
	{
		if(it->first / 1000 <= level && it->first % 1000 >= level)
		{
			std::map<int, BountyTaskFindCfg>::iterator find_it = it->second.find(find_type);
			if (find_it != it->second.end())
			{
				return &find_it->second;
			}
		}
	}

	return NULL;
}

const int TaskPool::GetFindType(int _role_level)
{
	/*for (std::map<int, BountyTaskFindCfg>::iterator it = m_seq_to_btfcfg.begin(); it != m_seq_to_btfcfg.end(); ++it)
	{
		if (_role_level < it->second.min_level || _role_level > it->second.max_level) continue;
		return &it->second;
	}*/

	for (std::vector<BountyTaskFindCfg>::iterator it = m_btfcfg_v.begin(); it != m_btfcfg_v.end(); ++it)
	{
		if (_role_level < it->min_level || _role_level > it->max_level) continue;
		return it->find_type;
	}

	return -1;
}

int TaskPool::GetFirstTaskCfg(int seq) const
{
	std::map<int, int>::const_iterator it = m_bounty_first_task_cfg.find(seq);
	if (it != m_bounty_first_task_cfg.end()) return it->second;

	return 0;
}

const std::vector<ItemConfigData>* TaskPool::GetBountyTaskFinishRewardCfg(int role_level) const
{
	if (role_level <= 0 || role_level > MAX_ROLE_LEVEL) return NULL;

	for (size_t i = m_bounty_task_finish_reward_cfg.size() - 1; i >= 0 ; i--)
	{
		if (m_bounty_task_finish_reward_cfg[i].min_level <= role_level && role_level <= m_bounty_task_finish_reward_cfg[i].max_level)
		{
			return &m_bounty_task_finish_reward_cfg[i].reward_list;
		}
	}
	return NULL;
}

TaskID TaskPool::GetLimitDropItemTask(ItemID item_id)
{
	LimitDropItemMap::iterator it = m_limit_drop_item_task_hash.find(item_id);
	if (it == m_limit_drop_item_task_hash.end()) return INVALID_TASK_ID;

	return it->second;
}

int TaskPool::GetAnecdoteTaskType(int task_id)
{
	std::map<int, int>::iterator it = m_task_anecdote_type_hash.find(task_id);
	if (it != m_task_anecdote_type_hash.end())
	{
		return it->second;
	}
	return ANECDOTE_TASK_TYPE_INVALID;
}

const AnecdoteInitialTaskCfg* TaskPool::GetAnecdoteTaskInitialCfg(int anecdote_task_type)
{
	if (anecdote_task_type <= ANECDOTE_TASK_TYPE_INVALID || anecdote_task_type >= ANECDOTE_TASK_TYPE_MAX)
	{
		return NULL;
	}

	return &m_anecdote_initial_task_cfg_list[anecdote_task_type];
}

const AnecdoteRewardCfg* TaskPool::GetAnecdoteFinalReward(int anecdote_task_type)
{
	if (anecdote_task_type <= ANECDOTE_TASK_TYPE_INVALID || anecdote_task_type >= ANECDOTE_TASK_TYPE_MAX)
	{
		return NULL;
	}

	return &m_anecdote_reward_cfg[anecdote_task_type];
}

const AnecdoteMonsterGroupCfg* TaskPool::GetAnecdoteMonsterGroupCfg(int monster_group_id)
{
	AnecdoteMonsterGroupCfgMap::iterator it = m_anecdote_monster_group_cfg_map.find(monster_group_id);
	if (it != m_anecdote_monster_group_cfg_map.end())
	{
		return &it->second;
	}

	return NULL;
}

bool TaskPool::IsConditionNeedItem(ItemID item_id) const
{
	return m_condition_item_set.find(item_id) != m_condition_item_set.end();
}

const PrestigeTaskCfg* TaskPool::GetPrestigeTaskCfg(int task_id)
{
	PrestigeTaskCfgMap::iterator it = m_prestige_task_cfg_map.find(task_id);
	if (it == m_prestige_task_cfg_map.end()) return NULL;

	return &it->second;
}

const PrestigeTaskVillageCfg* TaskPool::GetPrestigeTaskVillageCfg(int village_id)
{
	if (village_id < 0 || village_id >= ARRAY_ITEM_COUNT(m_prestige_task_village_cfg_list))
	{
		return NULL;
	}

	if (!m_prestige_task_village_cfg_list[village_id].IsValid())
	{
		return NULL;
	}

	return &m_prestige_task_village_cfg_list[village_id];
}

int TaskPool::RandPrestigeTaskGroup(int village_id)
{
	if (village_id < 0 || village_id >= ARRAY_ITEM_COUNT(m_prestige_task_group_vec))
	{
		return 0;
	}

	if (m_prestige_task_group_vec[village_id].empty())
	{
		return 0;
	}

	int idx = rand() % static_cast<int>(m_prestige_task_group_vec[village_id].size());
	return m_prestige_task_group_vec[village_id][idx];
}

int TaskPool::FillPrestigeTaskList(int village_id, int task_group, int task_list_size, ARG_OUT UInt16* task_list)
{
	std::vector<UInt16> candidate_list;
	candidate_list.reserve(task_list_size);

	PrestigeTaskCfgMap::iterator it = m_prestige_task_cfg_map.begin();
	for (; it != m_prestige_task_cfg_map.end(); ++it)
	{
		if (village_id != it->second.village_id || task_group != it->second.task_group)
		{
			continue;
		}
		candidate_list.push_back(it->second.task_id);
	}

	std::sort(candidate_list.begin(), candidate_list.end());
	
	int ret_num = 0;
	for (size_t i = 0; i < candidate_list.size() && ret_num < task_list_size; ++i)
	{
		task_list[ret_num++] = candidate_list[i];
	}

	return ret_num;

}

int TaskPool::GetPrestigeTaskReward(int role_level, int reward_group)
{
	const PrestigeTaskPrestigeGroupCfg* group_cfg = this->GetPrestigeRewardGroupCfg(reward_group, m_prestige_group_cfg_map);
	if (NULL == group_cfg) return 0;

	for (int i = (int)group_cfg->level_prestige_vec.size() - 1; i >= 0 ; --i)
	{
		if (role_level >= group_cfg->level_prestige_vec[i].role_level)
		{
			return group_cfg->level_prestige_vec[i].add_prestige_num;
		}
	}

	return 0;
}

int TaskPool::GetPrestigeTaskEndReward(int role_level, int reward_group)
{
	const PrestigeTaskPrestigeGroupCfg* group_cfg = this->GetPrestigeRewardGroupCfg(reward_group, m_end_prestige_group_cfg_map);
	if (NULL == group_cfg) return 0;

	for (int i = (int)group_cfg->level_prestige_vec.size() - 1; i >= 0; --i)
	{
		if (role_level >= group_cfg->level_prestige_vec[i].role_level)
		{
			return group_cfg->level_prestige_vec[i].add_prestige_num;
		}
	}

	return 0;
}

bool TaskPool::IsPrestigeTaskAddActiveDegree(int task_id)
{
	const PrestigeTaskCfg* cfg = this->GetPrestigeTaskCfg(task_id);
	if (NULL == cfg) return false;

	return cfg->is_activedegree;
}

const PrestigeTaskStarCfg * TaskPool::GetPrestigeTaskStarCfg(int star)
{
	if (star <= 0 || star > (int)m_prestige_task_star_cfg.size())
		return NULL;

	return &m_prestige_task_star_cfg[star - 1];
}

int TaskPool::GetPrestigeTaskStar(bool is_rate_2)
{
	int rate = RandomNum(10000);
	if (is_rate_2)
	{		
		std::map<int, int>::iterator it = m_prestige_task_star_rate_2_map.upper_bound(rate);
		return it->second;
	}
	else
	{
		std::map<int, int>::iterator it = m_prestige_task_star_rate_1_map.upper_bound(rate);
		return it->second;
	}
}

const AnecdoteBigEventRewardCfg* TaskPool::GetAnecdoteBigEventRewardCfg(int seq, int version) const
{
	const std::vector<AnecdoteBigEventRewardCfg>* vec = this->GetAnecdoteBigEventRewardCfgVec(version);
	if (NULL == vec) return NULL;

	for (size_t i = 0; i < vec->size(); ++i)
	{
		if (seq == (*vec)[i].seq)
		{
			return &(*vec)[i];
		}
	}

	return NULL;
}

const std::vector<AnecdoteBigEventRewardCfg>* TaskPool::GetAnecdoteBigEventRewardCfgVec(int version) const
{
	if (version < 0 || version >= ARRAY_ITEM_COUNT(m_anecdote_big_event_reward_cfg_vec)) return NULL;

	return &m_anecdote_big_event_reward_cfg_vec[version];
}

const std::vector<ItemConfigData>* TaskPool::GetSpecialRewardCfg(int job) const
{
	std::map<int, std::vector<ItemConfigData> >::const_iterator it = m_special_rewards_cfg.find(job);
	if (it != m_special_rewards_cfg.end()) return &it->second;

	return NULL;
}

const AnecdoteSpecialCfg * TaskPool::GetAnecdoteSpecialCfg(int task_type, int monster_seq)
{
	for (std::vector<AnecdoteSpecialCfg>::iterator it = special_anecdote_cfg.begin(); it != special_anecdote_cfg.end(); it++)
	{
		if (it->special_type == task_type && it->monster_seq == monster_seq)
		{
			return &(*it);
		}
	}
	return NULL;
}

const TeamChallengeRewardCfg * TaskPool::GetTeamChallengeRewardCfg(int task_id)
{
	std::map<int, TeamChallengeRewardCfg>::iterator it = team_challenge_reward_map.find(task_id);
	if (it != team_challenge_reward_map.end())
	{
		return &it->second;
	}
	return NULL;
}

int TaskPool::InitDefeatsGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return 0;		// 可以为空

	while (NULL != dataElement)
	{
		int group_id = 0; DefeatMonsGroups tmp_con;
		if (!GetSubNodeValue(dataElement, "group", group_id) || m_defeat_mon_group.end() != m_defeat_mon_group.find(group_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "item_id", tmp_con.need_item) || (tmp_con.need_item > 0 && NULL == ITEMPOOL->GetItem(tmp_con.need_item)))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "drop_id", tmp_con.drop_id) || (tmp_con.drop_id > 0 && NULL == DROPPOOL->GetDropConfig(tmp_con.drop_id)))
		{
			return -3;
		}

		TiXmlElement * monster_list = dataElement->FirstChildElement("drop_monster_list");
		if (NULL != monster_list)
		{
			TiXmlElement * drop_monster = monster_list->FirstChildElement("drop_monster");
			while (NULL != drop_monster)
			{
				int monster_id = 0;
				if (!GetNodeValue(drop_monster, monster_id) || !MonsterPool::GetInstance().IsMonsterExist(monster_id))
				{
					return -500;
				}

				tmp_con.drop_monster_list.insert(monster_id);
				drop_monster = drop_monster->NextSiblingElement();
			}
		}
		else
		{
			monster_list = dataElement->FirstChildElement("drop_monster");
			if (NULL == monster_list) return -200;
			{
				TiXmlElement * monster_node = monster_list->FirstChildElement("node");
				while (NULL != monster_node)
				{
					int monster_id = 0;
					if (!GetNodeValue(monster_node, monster_id) || (monster_id > 0 && !MonsterPool::GetInstance().IsMonsterExist(monster_id)))
					{
						return -500;
					}

					tmp_con.drop_monster_list.insert(monster_id);
					monster_node = monster_node->NextSiblingElement();
				}
			}
		}

		m_defeat_mon_group[group_id] = tmp_con;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitMonsterGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return 0;		// 可以为空

	while (NULL != dataElement)
	{
		int t_group_h = 0;
		if (!GetSubNodeValue(dataElement, "group", t_group_h) || m_monster_group.end() != m_monster_group.find(t_group_h))
		{
			return -1;
		}

		std::vector<int> t_i_id_list;
		TiXmlElement * monster_id_list = dataElement->FirstChildElement("group_monster_id_list");
		if (NULL != monster_id_list)
		{
			TiXmlElement * cfg_monster_id = monster_id_list->FirstChildElement("group_monster_id");
			while (NULL != cfg_monster_id)
			{
				int t_monster_id = 0;
				if (!GetNodeValue(cfg_monster_id, t_monster_id) || !MonsterPool::GetInstance().IsMonsterExist(t_monster_id))
				{
					return -4;
				}

				t_i_id_list.push_back(t_monster_id);
				cfg_monster_id = cfg_monster_id->NextSiblingElement();
			}
		}
		else
		{
			monster_id_list = dataElement->FirstChildElement("group_monster_id");
			if (NULL == monster_id_list) return -200;

			TiXmlElement * cfg_monster_id = monster_id_list->FirstChildElement("node");
			while (NULL != cfg_monster_id)
			{
				int t_monster_id = 0;
				if (!GetNodeValue(cfg_monster_id, t_monster_id) || !MonsterPool::GetInstance().IsMonsterExist(t_monster_id))
				{
					return -4;
				}

				t_i_id_list.push_back(t_monster_id);
				cfg_monster_id = cfg_monster_id->NextSiblingElement();
			}
		}

		m_monster_group.insert(std::pair<int, std::vector<int> >(t_group_h, t_i_id_list));
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitItemOpeGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return 0;		// 可以为空

	while (NULL != dataElement)
	{
		int t_group_h = 0;
		if (!GetSubNodeValue(dataElement, "group", t_group_h) || m_item_group.end() != m_item_group.find(t_group_h))
		{
			return -1;
		}

		std::set<ItemID> t_i_id_list;
		TiXmlElement * item_list = dataElement->FirstChildElement("item_id_list");
		if (NULL != item_list)
		{
			TiXmlElement * cfg_item_id = item_list->FirstChildElement("item_id");
			while (NULL != cfg_item_id)
			{
				ItemID t_item_id = 0;
				if (!GetNodeValue(cfg_item_id, t_item_id) || NULL == ITEMPOOL->GetItem(t_item_id))
				{
					return -4;
				}

				t_i_id_list.insert(t_item_id);
				cfg_item_id = cfg_item_id->NextSiblingElement();
			}
		}
		else
		{
			item_list = dataElement->FirstChildElement("item_id");
			if (NULL == item_list) return -200;

			TiXmlElement * cfg_item_id = item_list->FirstChildElement("node");
			while (NULL != cfg_item_id)
			{
				ItemID t_item_id = 0;
				if (!GetNodeValue(cfg_item_id, t_item_id) || NULL == ITEMPOOL->GetItem(t_item_id))
				{
					return -4;
				}

				t_i_id_list.insert(t_item_id);
				cfg_item_id = cfg_item_id->NextSiblingElement();
			}
		}

		m_item_group.insert(std::pair<int, std::set<ItemID> >(t_group_h, t_i_id_list));
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitCommonRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return 0;		// 可以为空

	while (NULL != dataElement)
	{
		TaskID t_tid = 0; CommonTaskReward t_trc;
		if (!GetSubNodeValue(dataElement, "task_id", t_tid))
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "left_value", t_trc.allot_min) || t_trc.allot_min < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "right_value", t_trc.allot_max) || t_trc.allot_max < t_trc.allot_min)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "coin_bind", t_trc.coin) || t_trc.coin < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "immortal", t_trc.immortal) || t_trc.immortal < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "gold", t_trc.gold) || t_trc.gold < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "exp", t_trc.exp) || t_trc.exp < 0)
		{
			return -7;
		}

		if (!GetSubNodeValue(dataElement, "pet_exp", t_trc.pet_exp) || t_trc.pet_exp < 0)
		{
			return -20;
		}

		if (!GetSubNodeValue(dataElement, "reputation_city", t_trc.repu_city) || t_trc.repu_city < 0 || t_trc.repu_city >= PRESTIGE_VILLAGE_MAX_NUM)
		{
			return -8;
		}

		if (!GetSubNodeValue(dataElement, "reputation_num", t_trc.repu_numb) || t_trc.repu_numb < 0)
		{
			return -9;
		}

		if (!GetSubNodeValue(dataElement, "power", t_trc.power) || t_trc.power < 0)
		{
			return -333;
		}

		t_trc.reward_count = 0;
		TiXmlElement * item_list = dataElement->FirstChildElement("item_list_list");
		if (NULL != item_list)	// 允许道具奖励为空
		{
			TiXmlElement * item_obj = item_list->FirstChildElement("item_list");
			while (NULL != item_obj)
			{
				if (t_trc.reward_count >= TASK_REWARD_LIST_COUNT_MAX) return -10;

				if (!t_trc.reward_list[t_trc.reward_count].ReadConfig(item_obj))
				{
					return -(11 + t_trc.reward_count);
				}

				++t_trc.reward_count;
				item_obj = item_obj->NextSiblingElement();
			}
		}

		std::map<TaskID, std::vector<CommonTaskReward> >::iterator t_it = m_common_task_reward_cfg.find(t_tid);
		if (t_it != m_common_task_reward_cfg.end())	// 如果找到
		{
			if (t_trc.allot_min <= t_it->second.rbegin()->allot_max) return -400;
			t_it->second.push_back(t_trc);
		}
		else		// 如果没找到
		{
			std::vector<CommonTaskReward> t_v_trc; t_v_trc.push_back(t_trc);
			m_common_task_reward_cfg.insert(std::pair<TaskID, std::vector<CommonTaskReward> >(t_tid, t_v_trc));
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitAllTaskInforCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -100;

	while (NULL != dataElement)
	{
		TaskInfomaConfig* t_tci = new TaskInfomaConfig();
		if (!GetSubNodeValue(dataElement, "task_id", t_tci->task_id))
		{
			delete t_tci;
			return -1;
		}

		if (NULL != m_task_list[t_tci->task_id])
		{
			delete t_tci;
			return -2;
		}
		
		if (!GetSubNodeValue(dataElement, "task_type", t_tci->task_type) || t_tci->task_type >= TASK_TYPE_MAX || t_tci->task_type < 0)
		{
			delete t_tci;
			return -3;
		}

		if (t_tci->task_type == TASK_TYPE_PET_FEI_SHENG_TASK)
		{
			if (!GetSubNodeValue(dataElement, "t_param1", t_tci->task_param1))
			{
				delete t_tci;
				return -31;
			}

			if (!GetSubNodeValue(dataElement, "t_param2", t_tci->task_param2))
			{
				delete t_tci;
				return -32;
			}

			if (!GetSubNodeValue(dataElement, "t_param3", t_tci->task_param3))
			{
				delete t_tci;
				return -33;
			}		
		}

		/*
		if (!GetSubNodeValue(dataElement, "ver", t_tci.task_ver) || t_tci.task_ver < 0)
		{
			return -4;
		}
		*/
		if (!GetSubNodeValue(dataElement, "min_level", t_tci->level_min) || t_tci->level_min < 0)
		{
			delete t_tci;
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "max_level", t_tci->level_max) || t_tci->level_max < t_tci->level_min)
		{
			delete t_tci;
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "pretaskid1", t_tci->pre_task_id))
		{
			t_tci->pre_task_id = 0;
		}

		if (t_tci->pre_task_id != 0 && NULL == m_task_list[t_tci->pre_task_id])
		{
			printf("-7, task_id[%d] pre_task_id[%d]\n", t_tci->task_id, t_tci->pre_task_id);
			return -7;
		}

		if (t_tci->pre_task_id == t_tci->task_id || (0 != t_tci->pre_task_id && t_tci->pre_task_id != m_task_list[t_tci->pre_task_id]->task_id))
		{
			return -8;
		}

		TiXmlElement * pre_task_cfg = dataElement->FirstChildElement("pretaskid2");
		if (NULL != pre_task_cfg)
		{
			if (!GetSubNodeValue(dataElement, "pretaskid2", t_tci->pre_task_id2))
			{
				t_tci->pre_task_id2 = 0;
			}

			if (t_tci->pre_task_id2 > 0)
			{
				TaskInfomaConfig* prev_task = m_task_list[t_tci->pre_task_id2];
				if (NULL != prev_task)
				{
					if (prev_task->nex_task_id2 > 0)
					{
						printf("%s t_tci->task_id[%d] pre_task_id2[%d]", __FUNCTION__, t_tci->task_id, t_tci->pre_task_id2);
						return -444; // 不允许有多个后置任务
					}
					prev_task->nex_task_id2 = t_tci->task_id;
				}
				else
				{
					printf("%s t_tci->task_id[%d] pre_task_id2[%d]", __FUNCTION__, t_tci->task_id, t_tci->pre_task_id2);
					return -555; //前置任务比当前任务的任务ID更后？
				}
			}
		}

		if (!GetSubNodeValue(dataElement, "camp_left_value", t_tci->prof_limit_min) || t_tci->prof_limit_min < PROF_TYPE_PROF_NO_LIMIT || t_tci->prof_limit_min > PROF_TYPE_MAX)
		{
			return -9;
		}

		if (!GetSubNodeValue(dataElement, "camp_right_value", t_tci->prof_limit_max) || t_tci->prof_limit_max < t_tci->prof_limit_min || t_tci->prof_limit_max > PROF_TYPE_MAX)
		{
			return -10;
		}

		if (PROF_TYPE_PROF_NO_LIMIT == t_tci->prof_limit_min && PROF_TYPE_PROF_NO_LIMIT != t_tci->prof_limit_max)
		{
			return -11;
		}

		if (!GetSubNodeValue(dataElement, "group_limited", t_tci->team_limit_type) || t_tci->team_limit_type < TASK_TEAM_UNCONFINED || t_tci->team_limit_type >= TASK_TEAM_TYPE_MAX)
		{
			return -12;
		}

		if (!GetSubNodeValue(dataElement, "accept_npc", t_tci->accept_npc) || (0 != t_tci->accept_npc && NULL == NPCPOOL->GetNPCInformaCfg(t_tci->accept_npc)))
		{
			return -13;
		}

		int open_server_days = 0;
		if (GetSubNodeValue(dataElement, "time_day", open_server_days))
		{
			t_tci->open_server_day = open_server_days;
		}

		if (!GetSubNodeValue(dataElement, "commit_npc", t_tci->commit_npc) || (0 != t_tci->commit_npc && NULL == NPCPOOL->GetNPCInformaCfg(t_tci->commit_npc)))
		{
			return -14;
		}

		int is_auto_commit = 0;
		if (!GetSubNodeValue(dataElement, "is_auto_commit", is_auto_commit))
		{
			return -144;
		}
		t_tci->can_auto_commit = (is_auto_commit > 0);

		int is_auto_catch = 0;
		if (!GetSubNodeValue(dataElement, "is_auto_catch", is_auto_catch))
		{
			return -14;
		}
		t_tci->can_auto_catch = (is_auto_catch > 0);

		if (!GetSubNodeValue(dataElement, "is_share", t_tci->is_share) || (TASK_IS_SHARE_NONE != t_tci->is_share && TASK_IS_SHARE_TRUE != t_tci->is_share))
		{
			return -15;
		}

		if (!GetSubNodeValue(dataElement, "time_limit_second", t_tci->timer_limit) || t_tci->timer_limit < 0)
		{
			return -16;
		}

		if (!GetSubNodeValue(dataElement, "condition", t_tci->task_condition) || t_tci->task_condition < 0 || t_tci->task_condition >= TASK_NEED_MAX)
		{
			return -17;
		}

		if (!GetSubNodeValue(dataElement, "c_param1", t_tci->param1) || t_tci->param1 < 0)
		{
			return -18;
		}

		if (!GetSubNodeValue(dataElement, "c_param2", t_tci->param2) || t_tci->param2 < 0)
		{
			return -19;
		}

		if (!GetSubNodeValue(dataElement, "reward_type", t_tci->reward_allot) || t_tci->reward_allot < 0 || t_tci->reward_allot >= ALLOT_TYPE_MAX)
		{
			return -20;
		}

		if (!GetSubNodeValue(dataElement, "sever_fight_type", t_tci->battle_mode) || t_tci->battle_mode < BATTLE_MODE_INVALID || t_tci->battle_mode >= BATTLE_MODE_MAX)
		{
			return -__LINE__;
		}
		int is_skip = 0;
		if (!GetSubNodeValue(dataElement, "is_skip", is_skip))
		{
			return -__LINE__;
		}
		t_tci->is_skip = is_skip == 1 ? true : false;
		if (!GetSubNodeValue(dataElement, "skip_cost_type", t_tci->skip_cost_type) || t_tci->skip_cost_type < MONEY_TYPE_GOLD || t_tci->skip_cost_type >= MONEY_TYPE_MAX)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "skip_cost_num", t_tci->skip_cost_num) || t_tci->skip_cost_num < 0)
		{
			return -__LINE__;
		}

		t_tci->is_add_robot = false;
		GetSubNodeValue(dataElement, "is_add_robot", t_tci->is_add_robot);

		t_tci->can_skip_battle = false;
		GetSubNodeValue(dataElement, "can_skip_battle", t_tci->can_skip_battle);
		if (t_tci->can_skip_battle && t_tci->task_condition != TASK_NEED_BEAT_MONGROUP)
		{
			return -21;
		}

		t_tci->skip_battle_cost_coin = 0;
		if (GetSubNodeValue(dataElement, "skip_battle_payment", t_tci->skip_battle_cost_coin))
		{
			if (t_tci->can_skip_battle && t_tci->skip_battle_cost_coin <= 0)
			{
				return -22;
			}
		}

		t_tci->limit_drop_count = 0;
		TiXmlElement* limit_drop_element = dataElement->FirstChildElement("task_limit_drop");
		if (NULL == limit_drop_element)
		{
			limit_drop_element = dataElement->FirstChildElement("task_limit_drop_list");
		}
		if (NULL != limit_drop_element)
		{
			TiXmlElement* limit_drop_node = limit_drop_element->FirstChildElement("node");
			while (limit_drop_node)
			{
				if (t_tci->limit_drop_count >= TASK_LIMIT_DROP_ITEM_NUM_MAX)
				{
					return -200;
				}

				ItemID item_id = 0;
				if (GetNodeValue(limit_drop_node, item_id))
				{
					if (NULL == ITEMPOOL->GetItem(item_id))
					{
						return -201;
					}

					if (m_limit_drop_item_task_hash.find(item_id) != m_limit_drop_item_task_hash.end())
					{
						//return -202;	// 一个物品对应一个任务
					}
					m_limit_drop_item_task_hash.insert(LimitDropItemMap::value_type(item_id, t_tci->task_id));

					t_tci->limit_drop_item[t_tci->limit_drop_count++] = item_id;
				}

				limit_drop_node = limit_drop_node->NextSiblingElement();
			}
		}
		
		UNSTD_STATIC_CHECK(46 == TASK_NEED_MAX)
		switch (t_tci->task_condition)
		{
		case TASK_NEED_PLAY_GAME:
			{
				if (t_tci->param1 < 0 || t_tci->param1 >= TASK_GAME_PLAY_TYPE_MAX || t_tci->param2 <= 0)
				{
					return -29;
				}
			}
			break;
		case TASK_NEED_TO_DO_NOTHING:	// 啥也不判断，直接通过检测
		case TASK_NEED_COLLECTION_TU_JIAN_ACTIVE_NUM:	
		case TASK_NEED_ONE_FA_BAO_UP_LEVEL:					
			break;

		case TASK_NEED_TALK_WITH_NPC:	// 判断 NPC 是否存在
//#ifndef _MSC_VER // TODO 用作奇闻任务本机测试 因策划配置未有NPC
			if (NULL == NPCPOOL->GetNPCInformaCfg(t_tci->param1))
			{
				return -30;
			}
			break;
//#endif
		case TASK_NEED_KILL_MONGROUP:	// 判断怪物组是否存在
//#ifndef _MSC_VER // TODO 用作奇闻任务本机测试 因策划配置未有NPC
			if (m_monster_group.end() == m_monster_group.find(t_tci->param1))
			{
				return -40;
			}
			break;
//#endif
		case TASK_NEED_INTO_SCENARIO:	// 判断场景是否存在
//#ifndef _MSC_VER // TODO 用作奇闻任务本机测试 因策划配置未有NPC
			if (!World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(t_tci->param1))
			{
				return -31;
			}
//#endif
			break;

		case TASK_NEED_GIVE_PROGROUP:	// 判断道具组是否存在
		case TASK_NEED_USES_PROGROUP:
		case TASK_NEED_HAVE_PROPERTY_LIST:
			{
				const std::set<ItemID> *  need_item_list = this->GetItemGroupList(t_tci->param1);
				if (need_item_list == NULL)
				{
					return -41;
				}

				for (std::set<ItemID>::iterator it = need_item_list->begin(); it != need_item_list->end(); ++it)
				{
					const ItemBase* item_base = ITEMPOOL->GetItem(*it);
					if (NULL == item_base)
					{
						return -411;
					}
						
					if (item_base->GetItemColumn() != ItemNamespace::ITEM_COLUMN_TYPE_TASK_MATERIAL)
					{	
						CheckResourceCenter::Instance().GetTaskConditionItemCheck()->Add(item_base->GetItemId(), __FUNCTION__);	
					}
				}
			}
			break;

		case TASK_NEED_BUY_PROPERTY:	// 判断道具是否存在
		case TASK_NEED_GIVE_PROPERTY:
		case TASK_NEED_USES_PROPERTY:
		case TASK_NEED_HAVE_PROPERTY:
		case TASK_NEED_COMMIT_PROPERTY:
			{
				const ItemBase* item_base = ITEMPOOL->GetItem(t_tci->param1);
				if (NULL == item_base)
				{
					return -42;
				}
				if (item_base->GetItemColumn() != ItemNamespace::ITEM_COLUMN_TYPE_TASK_MATERIAL)
				{
					CheckResourceCenter::Instance().GetTaskConditionItemCheck()->Add(item_base->GetItemId(), __FUNCTION__);
				}
			}
			break;
		case TASK_NEED_KILL_MONSTERS:	// 判断怪物是否存在
			if (!MonsterPool::GetInstance().IsMonsterExist(t_tci->param1)) return -43;
			break;

		case TASK_NEED_MEET_REQUIRES:	// 判断 参数1 是否合法
			if (t_tci->param1 >= TASK_MEET_TYPE_MAX) return -44;
			break;

		case TASK_NEED_GAIN_TASKPROP:	// 判断击杀组是否存在
			if (m_defeat_mon_group.end() == m_defeat_mon_group.find(t_tci->param1)) return -45;
			break;

		case TASK_NEED_ENTER_FB:
			{
				if (t_tci->param1 == FBManager::FB_TYPE_CRYSTAL_FB)
				{
					if(t_tci->param2 < 0 || t_tci->param2 >= (int)(CRYSTAL_FB_FIRST_FLAG * sizeof(int) * 8)) return -48;
				}
			}
			break;
		case TASK_NEED_ROLE_BEAT_MONGROUP:	// 延迟判断怪物组是否存在
		case TASK_NEED_BEAT_MONGROUP:	// 延迟判断怪物组是否存在
			CheckResourceCenter::Instance().GetMonsterGroupCheck()->Add(t_tci->param1, __FUNCTION__);
			break;
		case TASK_NEED_MATERIAL:
			{
				if(t_tci->param1 <= 0 || t_tci->param1 > MAX_MATERIAL_CHAPTER_NUM || t_tci->param2 <= 0 || t_tci->param2 > MAX_MATERIAL_CHECKPOINT_NUM) return -46;
			}
			break;
		case TASK_NEED_MI_YI:
			{
				if(t_tci->param1 < 0 || t_tci->param1 >= MI_YI_MAX_LEVEL || t_tci->param2 < 0 || t_tci->param2 >= MI_YI_MAX_MONSTER_NUM) return -47;
			}
			break;
		case TASK_NEED_TRAP_CREATURE:	// 依赖于逻辑配置读取，后于任务读取，不判断
		case TASK_NEED_PASS_TRANSCRI:
		case TASK_NEED_LOAD_PROGRESS:
		case TASK_NEED_PLAY_CUTSCENE:
		case TASK_NEED_KILL_IN_ORDER:
		
		case TASK_NEED_ESCORT:
		case TASK_NEED_ACCEPT_ESCORT:
		case TASK_NEED_PASS_DESIGNATED_MEDAL_FB:
		case TASK_NEED_PASS_CRYSTAL_FB_SEQ:
		case TASK_NEED_COURAGE_CHALLENGE:
		case TASK_NEED_COMMIT_PET:
			break;

		case TASK_NEED_MEET_ANECDOTE_REQUIRES:
			{
				if (t_tci->param1 <= ANECDOTE_TASK_TYPE_INVALID || t_tci->param1 >= ANECDOTE_TASK_TYPE_MAX || t_tci->param2 <= 0)
				{
					return -(1000000 + t_tci->task_id);
				}
			}
			break;
		case TASK_NEED_VILLAGE_LEVEL_ACHIVE:
			{
				if (t_tci->param1 < 0 || t_tci->param1 >= PRESTIGE_VILLAGE_MAX_NUM)
				{
					return -(1000000 + t_tci->task_id);
				}
			}
			break;
		case TASK_NEED_OPEN_YIN_YANG_EYE:
			{
				if (t_tci->param1 < 0 || t_tci->param2 < 0 || t_tci->param2 > 1)
				{
					return -(1000000 + t_tci->task_id);
				}
			}
			break;
		case TASK_NEED_DREAM_NOTES_CHALLENGE:
			{
				if (t_tci->param1 <= 0)
				{
					return -(1000000 + t_tci->task_id);
				}
			}
			break;
		case TASK_NEED_PARTNER_STRENGTHEN:
			{
				if (t_tci->param2 <= 0)
				{
					return -(1000000 + t_tci->task_id);
				}
				CheckResourceCenter::Instance().GetPartnerCheck()->Add(t_tci->param1, __FUNCTION__);
			}
			break;
		case TASK_NEED_PARTNER_BREAK:
			{
				if (t_tci->param2 <= 0)
				{
					return -(1000000 + t_tci->task_id);
				}
				CheckResourceCenter::Instance().GetPartnerCheck()->Add(t_tci->param1, __FUNCTION__);
			}
			break;
		default:
			break;
		}

		int gu_type = 0;
		TiXmlElement * giveup_list = dataElement->FirstChildElement("is_giveup");
		if (NULL != giveup_list)
		{
			TiXmlElement * cfg_give_up = giveup_list->FirstChildElement("node");
			while (NULL != cfg_give_up)
			{
				if (gu_type >= GIVEUP_TYPE_MAX) return -20;

//#ifndef _MSC_VER // TODO 用作奇闻任务本机测试 因策划配置未有NPC
				if (!GetNodeValue(cfg_give_up, t_tci->giveup[gu_type]) || (TASK_GIVEUP_TYPE_NONE != t_tci->giveup[gu_type] && TASK_GIVEUP_TYPE_TRUE != t_tci->giveup[gu_type]))
				{
					return -(21 + gu_type);
				}
//#endif

				++gu_type;
				cfg_give_up = cfg_give_up->NextSiblingElement();
			}
		}

		m_task_list[t_tci->task_id] = t_tci;
		if (0 != t_tci->pre_task_id)
		{
			TaskInfomaConfig* prev_task = m_task_list[t_tci->pre_task_id];
			if (NULL != prev_task)
			{
				if (prev_task->nex_task_id > 0)
				{
					return -777; // 不允许有多个后置任务
				}
				prev_task->nex_task_id = t_tci->task_id;
			}
			else
			{
				return -666; //前置任务比当前任务的任务ID更后？
			}
			this->InsertNextTask(t_tci->pre_task_id, t_tci->task_id);
			this->InsertPrevTask(t_tci->task_id, t_tci->pre_task_id);
		}
		this->InsertCuLvTask(t_tci->level_min, t_tci->task_id);
		this->InsertTaskType(t_tci->task_type, t_tci->task_id);
		if (PROF_TYPE_PROF_NO_LIMIT != t_tci->prof_limit_min)
		{
			int base_prof_type = GetBaseProf(t_tci->prof_limit_min);
			this->InsertProfTask(base_prof_type, t_tci->task_id);
		}

		++m_record_task;
		dataElement = dataElement->NextSiblingElement();
	}

	//直接取第一个主线任务的id为主线机器人初始任务
	for (int i = 0; i < MAX_UINT16; i++)
	{
		if (NULL == m_task_list[i])
		{
			continue;
		}
		if (TASK_TYPE_MAIN_QUEST == m_task_list[i]->task_type)
		{
			m_robot_mission_begin_task = m_task_list[i]->task_id;
			break;
		}
	}
	if (0 == m_robot_mission_begin_task)
	{
		return -400;
	}

	return 0;
}

int TaskPool::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "monster_group", m_task_other_cfg.realive_guide_monster_group))
	{
		return -1;
	}
	CheckResourceCenter::Instance().GetMonsterGroupCheck()->Add(m_task_other_cfg.realive_guide_monster_group, __FUNCTION__);

	if (!GetSubNodeValue(dataElement, "anecdotes_recommend_task_id", m_task_other_cfg.anecdotes_recommend_task_id))
	{
		return -2;
	}
	CheckResourceCenter::Instance().GetTaskCheck()->Add(m_task_other_cfg.anecdotes_recommend_task_id, __FUNCTION__);

	/*if (!GetSubNodeValue(dataElement, "task_id", m_task_other_cfg.award_surface_point_task_id))
	{
		return -3;
	}
	CheckResourceCenter::Instance().GetTaskCheck()->Add(m_task_other_cfg.award_surface_point_task_id, __FUNCTION__);

	if (!GetSubNodeValue(dataElement, "change_seq", m_task_other_cfg.award_surface_seq) || (m_task_other_cfg.award_surface_point_task_id > 0 && 
		m_task_other_cfg.award_surface_seq <= 0))
	{
		return -4;
	}*/

	if (!GetSubNodeValue(dataElement, "career_rewards", m_task_other_cfg.career_rewards) || m_task_other_cfg.career_rewards < 0) return -5;

	if (!GetSubNodeValue(dataElement, "partner_upgrade_task", m_task_other_cfg.partner_upgrade_task_id))
	{
		return -6;
	}
	CheckResourceCenter::Instance().GetTaskCheck()->Add(m_task_other_cfg.partner_upgrade_task_id, __FUNCTION__);


	return 0;
}

int TaskPool::InitSpecialRewardsCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -544;

	while (dataElement)
	{
		int job = 0;
		std::vector<ItemConfigData> reward_list;

		if (!GetSubNodeValue(dataElement, "job", job) || job < 0) return -1;

		TiXmlElement* reward_list_element = dataElement->FirstChildElement("item_list_list");
		if (NULL == reward_list_element) return -10;

		TiXmlElement* reward_element = reward_list_element->FirstChildElement("item_list");
		if (NULL == reward_element) return -20;

		while (reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element)) return -2;

			reward_list.push_back(item);

			reward_element = reward_element->NextSiblingElement();
		}

		m_special_rewards_cfg[job] = reward_list;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitTeamChallengeRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -544;

	while (dataElement)
	{
		int task_id = 0;
		if (!GetSubNodeValue(dataElement, "task_id", task_id)) return -1;

		TeamChallengeRewardCfg &cfg = team_challenge_reward_map[task_id];
		cfg.task_id = task_id;

		if (!GetSubNodeValue(dataElement, "coin_bind", cfg.coin_bind)) return -1;

		if (!GetSubNodeValue(dataElement, "exp", cfg.exp)) return -1;

		if (!GetSubNodeValue(dataElement, "pet_exp", cfg.pet_exp)) return -1;

		TiXmlElement* reward_list_element = dataElement->FirstChildElement("item_list_list");
		if (NULL != reward_list_element)
		{
			TiXmlElement* reward_element = reward_list_element->FirstChildElement("item_list");
			if (NULL == reward_element) return -20;

			while (reward_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(reward_element)) return -2;

				cfg.reward_vec.push_back(item);

				reward_element = reward_element->NextSiblingElement();
			}
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitAdventureGetCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -100;

	while (NULL != dataElement)
	{
		ParamSection lv_sec; AdvTaskActivates tmp_con;
		if (!GetSubNodeValue(dataElement, "min_level", lv_sec.section_min) || lv_sec.section_min < 0 || lv_sec.section_min > MAX_ROLE_LEVEL)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "max_level", lv_sec.section_max) || lv_sec.section_max < lv_sec.section_min || lv_sec.section_max > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "active_probability_10000", tmp_con.prob) || tmp_con.prob < 0 || tmp_con.prob > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "mission_group", tmp_con.mission_group) || tmp_con.mission_group < 0)
		{
			return -4;
		}

		int act_item = 0;
		if (!GetSubNodeValue(dataElement, "or_activation_item", act_item) || NULL == ITEMPOOL->GetItem(act_item))
		{
			return -5;
		}

		int time_limit = 0;
		if (!GetSubNodeValue(dataElement, "extinction_time_minute", time_limit))
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "partner_id", tmp_con.partner_id) || tmp_con.partner_id < 0)
		{
			return -7;
		}

		if (!GetSubNodeValue(dataElement, "partner_pro", tmp_con.partner_pro) || tmp_con.partner_pro < 0)
		{
			return -8;
		}

		std::map<ParamSection, std::vector<AdvTaskActivates> >::iterator it = m_adv_task_active_map.find(lv_sec);
		if (m_adv_task_active_map.end() != it)
		{
			it->second.push_back(tmp_con);
		}
		else
		{
			std::vector<AdvTaskActivates> tmp_list; tmp_list.push_back(tmp_con);
			m_adv_task_active_map[lv_sec] = tmp_list;
		}

		m_item_act_task_map[act_item][lv_sec] = tmp_con.mission_group;
		if (m_adv_task_time_limit.find(act_item) != m_adv_task_time_limit.end() && m_adv_task_time_limit[act_item] != time_limit)
		{
			return -9;
		}
		m_adv_task_time_limit[act_item] = time_limit;
		dataElement = dataElement->NextSiblingElement();
	}

	for (std::map<ParamSection, std::vector<AdvTaskActivates> >::iterator adv_it = m_adv_task_active_map.begin(); adv_it != m_adv_task_active_map.end(); ++adv_it)
	{
		int total_prob = 0;
		for (std::vector<AdvTaskActivates>::iterator prob_it = adv_it->second.begin(); prob_it != adv_it->second.end(); ++prob_it)
		{
			total_prob += prob_it->prob;
		}

		for (int lv = adv_it->first.section_min; lv <= adv_it->first.section_max; ++lv)
		{
			m_level_adv_task_list[lv] = &adv_it->second;
		}

		if (total_prob != RAND_UNIVERSAL_RATIO_BASE_NUM) return -500;
	}

	return 0;
}

int TaskPool::InitAdvTaskOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -100;

	if (!GetSubNodeValue(dataElement, "num_every_day", m_adv_task_other.limit_times_daily) || m_adv_task_other.limit_times_daily < 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "odds_100", m_adv_task_other.basic_trigger_rate) || m_adv_task_other.basic_trigger_rate < 0 || m_adv_task_other.basic_trigger_rate > 100)
	{
		return -2;
	}

	if (!GetSubNodeValue(dataElement, "talk_rate", m_adv_task_other.talk_rate) || m_adv_task_other.talk_rate < 0 || m_adv_task_other.talk_rate > 100)
	{
		return -3;
	}

	if (!GetSubNodeValue(dataElement, "level_limit", m_adv_task_other.task_level_limit) || m_adv_task_other.task_level_limit < 0 || m_adv_task_other.task_level_limit > MAX_ROLE_LEVEL)
	{
		return -4;
	}

	return 0;
}

int TaskPool::InitAdvTaskIDGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -100;

	while(dataElement != NULL)
	{
		AdvTaskGroup node_cfg;

		if (!GetSubNodeValue(dataElement, "seq", node_cfg.seq) || node_cfg.seq < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "activation_task_id", node_cfg.activation_task_id) || NULL == this->GetTaskCfgByID(node_cfg.activation_task_id))
		{
			return -2;
		}


		if(m_task_list[node_cfg.activation_task_id]->task_type != TASK_TYPE_ADVENTURES) return -300;

		if (!GetSubNodeValue(dataElement, "active_probability_10000", node_cfg.active_probability_10000) || node_cfg.active_probability_10000 < 0)
		{
			return -3;
		}
		m_adv_task_group_map[node_cfg.seq].weight += node_cfg.active_probability_10000;
		m_adv_task_group_map[node_cfg.seq].m_task_list.push_back(node_cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitBossChapterBossCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int chapter = 0;
		if (!GetSubNodeValue(dataElement, "seq", chapter) || chapter <= 0 || chapter >= MAX_BOSS_CHAPTER)
		{
			return -1;
		}

		/////////////////////////////// 普通BOSS //////////////////////////////////
		{
			int monster_group_id = 0;
			if (!GetSubNodeValue(dataElement, "ordinary_monster", monster_group_id) || monster_group_id <= 0)
			{
				return -2;
			}
			if (m_boss_chapter_boss_cfg_map.find(monster_group_id) != m_boss_chapter_boss_cfg_map.end())
			{
				return -222; // 重复了
			}

			CheckResourceCenter::Instance().GetMonsterGroupCheck()->Add(monster_group_id, __FUNCTION__);

			BossChapterBossCfg cfg;
			cfg.monster_group_id = monster_group_id;
			cfg.is_elite = false;
			cfg.reward_num = 0;
			cfg.related_task_id = 0;

			if (!GetSubNodeValue(dataElement, "task_monster", cfg.related_task_id) || NULL == this->GetTaskCfgByID(cfg.related_task_id))
			{
				return -2222;
			}

			TiXmlElement* rewardListElement = dataElement->FirstChildElement("ordinary_award_list");
			if (NULL != rewardListElement)
			{
				TiXmlElement* rewardElement = rewardListElement->FirstChildElement("ordinary_award");
				if (NULL == rewardElement) return -4;

				while (rewardElement)
				{
					if (cfg.reward_num >= BossChapterBossCfg::REWARD_NUM) return -5;

					if (!cfg.reward_list[cfg.reward_num].ReadConfig(rewardElement)) return -6;

					cfg.reward_num += 1;

					rewardElement = rewardElement->NextSiblingElement();
				}
			}

			m_boss_chapter_boss_cfg_map.insert(BossChapterBossCfgMap::value_type(monster_group_id, cfg));
			m_boss_chapter_chapter_boss_list[chapter].push_back(monster_group_id);
		}

		////////////////////////////////// 精英boss //////////////////////////////////
		{
			int monster_group_id = 0;
			if (!GetSubNodeValue(dataElement, "elite_monster", monster_group_id) || monster_group_id <= 0)
			{
				return -12;
			}
			if (m_boss_chapter_boss_cfg_map.find(monster_group_id) != m_boss_chapter_boss_cfg_map.end())
			{
				return -1222; // 重复了
			}

			CheckResourceCenter::Instance().GetMonsterGroupCheck()->Add(monster_group_id, __FUNCTION__);

			BossChapterBossCfg cfg;
			cfg.monster_group_id = monster_group_id;
			cfg.is_elite = true;
			cfg.reward_num = 0;
			cfg.related_task_id = 0;

			TiXmlElement* rewardListElement = dataElement->FirstChildElement("elite_award_list");
			if (NULL != rewardListElement)
			{
				TiXmlElement* rewardElement = rewardListElement->FirstChildElement("elite_award");
				if (NULL == rewardElement) return -14;

				while (rewardElement)
				{
					if (cfg.reward_num >= BossChapterBossCfg::REWARD_NUM) return -15;

					if (!cfg.reward_list[cfg.reward_num].ReadConfig(rewardElement)) return -16;

					cfg.reward_num += 1;

					rewardElement = rewardElement->NextSiblingElement();
				}
			}

			m_boss_chapter_boss_cfg_map.insert(BossChapterBossCfgMap::value_type(monster_group_id, cfg));
		}
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitBossChapterChapterCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int chapter = 0;
		if (!GetSubNodeValue(dataElement, "seq", chapter) || chapter <= 0 || chapter >= MAX_BOSS_CHAPTER)
		{
			return -1;
		}

		{	
			BossChapterChapterRewardCfg cfg;
			cfg.reward_num = 0;

			TiXmlElement* rewardListElement = dataElement->FirstChildElement("chapter_award_list");
			if (NULL == rewardListElement) return -3;

			TiXmlElement* rewardElement = rewardListElement->FirstChildElement("chapter_award");
			if (NULL == rewardElement) return -4;

			while (rewardElement)
			{
				if (cfg.reward_num >= BossChapterChapterRewardCfg::REWARD_NUM) return -5;

				if (!cfg.reward_list[cfg.reward_num].ReadConfig(rewardElement)) return -6;

				cfg.reward_num += 1;

				rewardElement = rewardElement->NextSiblingElement();
			}

			m_boss_chapter_chapter_reward_cfg_list[chapter] = cfg;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitBountyTaskOtherCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	//if (!GetSubNodeValue(dataElement, "level_limit", m_bounty_task_other_cfg.level_limit) || m_bounty_task_other_cfg.level_limit < 0 || m_bounty_task_other_cfg.level_limit > MAX_ROLE_LEVEL)
	//{
	//	return -1;
	//}

	if (!GetSubNodeValue(dataElement, "tasknum_every_day", m_bounty_task_other_cfg.tasknum_per_day) || m_bounty_task_other_cfg.tasknum_per_day <= 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(dataElement, "level_31", m_bounty_task_other_cfg.skip_first_task_level) || m_bounty_task_other_cfg.skip_first_task_level <= 0 || m_bounty_task_other_cfg.skip_first_task_level > MAX_ROLE_LEVEL)
	{
		return -5;
	}

	if (!GetSubNodeValue(dataElement, "task_694", m_bounty_task_other_cfg.skip_first_task_task_id))
	{
		return -6;
	}
	CheckResourceCenter::Instance().GetTaskCheck()->Add(m_bounty_task_other_cfg.skip_first_task_task_id, __FUNCTION__);

	if (!GetSubNodeValue(dataElement, "first_task_level", m_bounty_task_other_cfg.first_task_level) || m_bounty_task_other_cfg.first_task_level <= 0 || m_bounty_task_other_cfg.first_task_level > MAX_ROLE_LEVEL) return -7;

	return 0;
}

int TaskPool::InitBountyTaskLevelLimitCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int task_id = 0;
		if (!GetSubNodeValue(dataElement, "task_id", task_id))
		{
			return -1;
		}

		int min_level = 0;
		if (!GetSubNodeValue(dataElement, "min_level", min_level) || min_level < 0 || min_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		int max_level = 0;
		if (!GetSubNodeValue(dataElement, "max_level", max_level) || max_level < min_level || max_level > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		short is_first_task;
		if (!GetSubNodeValue(dataElement, "is_first_task", is_first_task) || is_first_task < 0 || is_first_task > 1)
		{
			return -4;
		}

		BountyTaskSimpleCfg cfg;
		cfg.task_id = task_id;
		cfg.min_level = min_level;
		cfg.max_level = max_level;
		cfg.is_first_task = is_first_task > 0;

		m_all_bounty_task_list.push_back(cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitBountyTaskTurnTableCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int group_id = 0;
		if (!GetSubNodeValue(dataElement, "group_id", group_id))
		{
			return -1;
		}

		BountyTaskTurnTableCfg* cfg_ptr = NULL;
		for (size_t i = 0; i < m_bounty_turn_table_cfg_list.size(); ++i)
		{
			if (group_id == m_bounty_turn_table_cfg_list[i].reward_group_id)
			{
				cfg_ptr = &m_bounty_turn_table_cfg_list[i];
				break;
			}
		}
		if (NULL == cfg_ptr)
		{
			BountyTaskTurnTableCfg cfg;
			cfg.reward_group_id = group_id;
			m_bounty_turn_table_cfg_list.push_back(cfg);
			cfg_ptr = &(m_bounty_turn_table_cfg_list[m_bounty_turn_table_cfg_list.size() - 1]);
		}
		BountyTaskTurnTableCfg& cfg = *cfg_ptr;
		
		if (cfg.reward_num >= BountyTaskTurnTableCfg::TABLE_REWARD_NUM)
		{
			return -222;
		}

		int rate = 0;
		if (!GetSubNodeValue(dataElement, "rate", rate) || rate < 0)
		{
			return -2;
		}
		cfg.rate_list[cfg.reward_num] = cfg.total_rate + rate;
		cfg.total_rate += rate;

		int first_hit;
		if (!GetSubNodeValue(dataElement, "first_hit", first_hit))
		{
			return -12;
		}

		if (0 != first_hit)
		{
			cfg.first_reward_index = cfg.reward_num;
		}

		if (!GetSubNodeValue(dataElement, "is_hearsay", cfg.is_hearsay[cfg.reward_num]))
		{
			return -12;
		}

		TiXmlElement* itemElement = dataElement->FirstChildElement("reward");
		if (NULL == itemElement)
		{
			return -3;
		}

		if (!cfg.reward_list[cfg.reward_num].ReadConfig(itemElement))
		{
			return -4;
		}

		cfg.reward_num += 1;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitBountyTaskTurnTableGroupCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int min_level = 0;
		if (!GetSubNodeValue(dataElement, "min_level_turn", min_level) || min_level < 0 || min_level > MAX_ROLE_LEVEL) 
		{
			return -1;
		}

		int max_level = 0;
		if (!GetSubNodeValue(dataElement, "max_level_turn", max_level) || max_level < min_level || max_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		int group_id = 0;
		if (!GetSubNodeValue(dataElement, "reward_group", group_id) || group_id <= 0)
		{
			return -3;
		}

		for (size_t i = 0; i < m_bounty_turn_table_cfg_list.size(); ++i)
		{
			BountyTaskTurnTableCfg& cfg = m_bounty_turn_table_cfg_list[i];
			if (cfg.reward_group_id == group_id)
			{
				cfg.max_level = max_level;
				cfg.min_level = min_level;
			}
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitNextBountyTaskCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		TaskID last_task = 0;
		if (!GetSubNodeValue(dataElement, "last_task", last_task))
		{
			return -1;
		}

		{
			const TaskInfomaConfig* cfg = this->GetTaskCfgByID(last_task);
			if (NULL == cfg || cfg->task_type != TASK_TYPE_BOUNTY_TASK)
			{
				return -2;
			}
		}

		TaskID first_task = 0;
		if (!GetSubNodeValue(dataElement, "word_task", first_task))
		{
			return -3;
		}

		{
			const TaskInfomaConfig* cfg = this->GetTaskCfgByID(first_task);
			if (NULL == cfg || cfg->task_type != TASK_TYPE_BOUNTY_TASK)
			{
				return -4;
			}
		}

		m_next_bounty_task_map[first_task] = last_task;

		dataElement = dataElement->NextSiblingElement();
	}
	
	return 0;
}

int TaskPool::InitFirstBountyTaskCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int job_type = 0;
		if (!GetSubNodeValue(dataElement, "job_type", job_type) || job_type <= 0 || job_type >= PROFESSION_TYPE_NUM)
		{
			return -1;
		}

		TaskID first_task_id = 0;
		if (!GetSubNodeValue(dataElement, "task_id", first_task_id))
		{
			return -2;
		}

		{
			const TaskInfomaConfig* cfg = this->GetTaskCfgByID(first_task_id);
			if (NULL == cfg || cfg->task_type != TASK_TYPE_BOUNTY_TASK)
			{
				return -3;
			}
		}

		m_first_bounty_task_list[job_type] = first_task_id;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitBountyTaskFindCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		BountyTaskFindCfg node_cfg;
		if (!GetSubNodeValue(dataElement, "find_type", node_cfg.find_type) || node_cfg.find_type < RB_TYPE_COIN || node_cfg.find_type > RB_TYPE_GOLD)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "min_level", node_cfg.min_level) || node_cfg.min_level < 0 || node_cfg.min_level >= MAX_ROLE_LEVEL)
		{
			return -2;
		}
		if (!GetSubNodeValue(dataElement, "max_level", node_cfg.max_level) || node_cfg.max_level < 0 || node_cfg.max_level > MAX_ROLE_LEVEL || node_cfg.max_level < node_cfg.min_level)
		{
			return -3;
		}
		if (!GetSubNodeValue(dataElement, "exp", node_cfg.exp) || node_cfg.exp < 0)
		{
			return -4;
		}
		if (!GetSubNodeValue(dataElement, "coin", node_cfg.coin) || node_cfg.coin < 0)
		{
			return -5;
		}
		if (!GetSubNodeValue(dataElement, "family_gongxian", node_cfg.family_gongxian) || node_cfg.family_gongxian < 0)
		{
			return -6;
		}
		if (!GetSubNodeValue(dataElement, "group_id", node_cfg.group_id) || (node_cfg.group_id < 0 && node_cfg.group_id != -1))
		{
			return -7;
		}
		if (!GetSubNodeValue(dataElement, "seq", node_cfg.seq) || node_cfg.seq < 0)
		{
			return -8;
		}

		int level_key = node_cfg.min_level * 1000 + node_cfg.max_level;
		UNSTD_STATIC_CHECK(MAX_ROLE_LEVEL <= 1000);
		
		m_bounty_task_find_cfg[level_key][node_cfg.find_type] = node_cfg;

		m_btfcfg_v.push_back(node_cfg);

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int TaskPool::InitBountyTaskFindRewardCfg(TiXmlElement * RootElement)
{

	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		BountyTaskFindRewardCfg node_cfg;
		if (!GetSubNodeValue(dataElement, "group_id", node_cfg.group_id) || node_cfg.group_id == 0 || (node_cfg.group_id < 0 && node_cfg.group_id != -1))
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "rate", node_cfg.rate) || node_cfg.rate < 0 || node_cfg.rate > RATE_COUNT_MAX_NUM)
		{
			return -2;
		}

		TiXmlElement* item_element = dataElement->FirstChildElement("reward");
		if (NULL != item_element)
		{
			if(!node_cfg.reward.ReadConfig(item_element)) return -100;
		}

		m_bounty_task_find_reward_cfg[node_cfg.group_id].push_back(node_cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitBountyFirstTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -544;

	while (dataElement)
	{
		int seq = 0, task_id = 0;

		if (!GetSubNodeValue(dataElement, "seq", seq) || seq < 0) return -1;
		if (!GetSubNodeValue(dataElement, "task_id", task_id) || task_id <= 0) return -2;

		m_bounty_first_task_cfg[seq] = task_id;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitAnecdoteTaskCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int task_id = 0;
		if (!GetSubNodeValue(dataElement, "task_id", task_id))
		{
			return -1;
		}

		const TaskInfomaConfig* task_cfg = this->GetTaskCfgByID(task_id);
		if (NULL == task_cfg || task_cfg->task_type != TASK_TYPE_ANECDOTE_TASK)
		{
			return -2;
		}

		int anecdote_task_type = 0;
		if (!GetSubNodeValue(dataElement, "stranger_task_type", anecdote_task_type)
			|| anecdote_task_type <= ANECDOTE_TASK_TYPE_INVALID
			|| anecdote_task_type >= ANECDOTE_TASK_TYPE_MAX)
		{
			return -3;
		}

		if (m_task_anecdote_type_hash.find(task_id) != m_task_anecdote_type_hash.end())
		{
			return -4; // 有重复任务ID
		}

		m_task_anecdote_type_hash.insert(std::map<int, int>::value_type(task_id, anecdote_task_type));

		int pre_task_id = 0;
		if (!GetSubNodeValue(dataElement, "pretaskid1", pre_task_id) || pre_task_id < 0)
		{
			return -5;
		}

		int level_min = 0;
		if (!GetSubNodeValue(dataElement, "min_level", level_min) || level_min < 0 || level_min > MAX_ROLE_LEVEL)
		{
			return -6;
		}

		if (0 == pre_task_id)
		{
			static AnecdoteInitialTaskCfg initial_cfg;
			initial_cfg.task_id = task_id;
			initial_cfg.level_limit = level_min;

			m_anecdote_initial_task_cfg_list[anecdote_task_type] = initial_cfg;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitAnecdoteTaskFinalRewardCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int anecdote_type = 0;
		if (!GetSubNodeValue(dataElement, "stranger_task_type", anecdote_type) || anecdote_type <= ANECDOTE_TASK_TYPE_INVALID || anecdote_type >= ANECDOTE_TASK_TYPE_MAX)
		{
			return -1;
		}

		AnecdoteRewardCfg& cfg = m_anecdote_reward_cfg[anecdote_type];
		if (cfg.is_valid)
		{
			return -5; // 重复了
		}

		if (!GetSubNodeValue(dataElement, "coin_bind", cfg.coin_bind) || cfg.coin_bind < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "exp", cfg.exp) || cfg.exp < 0)
		{
			return -7;
		}

		if (!GetSubNodeValue(dataElement, "gold", cfg.gold) || cfg.gold < 0 || cfg.gold > 1000) // 防止策划配错数值
		{
			return -8;
		}

		cfg.reward_count = 0;
		TiXmlElement * item_list = dataElement->FirstChildElement("item_list_list");
		if (NULL != item_list)	// 允许道具奖励为空
		{
			TiXmlElement * item_obj = item_list->FirstChildElement("item_list");
			while (NULL != item_obj)
			{
				if (cfg.reward_count >= TASK_REWARD_LIST_COUNT_MAX) return -10;

				if (!cfg.reward_list[cfg.reward_count].ReadConfig(item_obj))
				{
					return -(100 + cfg.reward_count);
				}

				++cfg.reward_count;
				item_obj = item_obj->NextSiblingElement();
			}
		}

		cfg.is_valid = true;

		dataElement = dataElement->NextSiblingElement();
	}

	//for (int type = ANECDOTE_TASK_TYPE_INVALID + 1; type < ANECDOTE_TASK_TYPE_MAX; ++type)
	//{
	//	if (!m_anecdote_reward_cfg[type].is_valid)
	//	{
	//		return -2345;
	//	}
	//}

	return 0;
}

int TaskPool::InitAnecdoteTaskMonsterGroupCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int anecdote_type = 0;
		if (!GetSubNodeValue(dataElement, "task_type", anecdote_type) || anecdote_type <= ANECDOTE_TASK_TYPE_INVALID || anecdote_type >= ANECDOTE_TASK_TYPE_MAX)
		{
			return -1;
		}

		int npc_id = 0;
		if (!GetSubNodeValue(dataElement, "npc_id", npc_id) || npc_id < 0)
		{
			return -2;
		}

		int min_level = 0;
		if (!GetSubNodeValue(dataElement, "min_level", min_level) || min_level <= 0)
		{
			return -3;
		}

		int max_level = 0;
		if (!GetSubNodeValue(dataElement, "max_level", max_level) || max_level <= 0)
		{
			return -4;
		}
		
		int monster_group_id = 0;
		if (!GetSubNodeValue(dataElement, "monster_group_id", monster_group_id) || monster_group_id <= 0)
		{
			return -5;
		}
		CheckResourceCenter::Instance().GetMonsterGroupCheck()->Add(monster_group_id, __FUNCTION__);

		AnecdoteMonsterGroupCfg cfg;
		cfg.anecdote_task_type = anecdote_type;
		cfg.min_level = min_level;
		cfg.max_level = max_level;
		cfg.npc_id = npc_id;
		cfg.monster_group_id = monster_group_id;

		m_anecdote_monster_group_cfg_map.insert(AnecdoteMonsterGroupCfgMap::value_type(monster_group_id, cfg));
	
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitAnecdoteOtherCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	AnecdoteOtherCfg& cfg = m_anecdote_other_cfg;
	if (!GetSubNodeValue(dataElement, "activity_duration", cfg.big_event_open_duration_hours) ||
		cfg.big_event_open_duration_hours <= 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "event_level", cfg.big_event_open_level) || cfg.big_event_open_level <= 0 || cfg.big_event_open_level > MAX_ROLE_LEVEL)
	{
		return -2;
	}

	if (!GetSubNodeValue(dataElement, "reset", cfg.big_event_version) || cfg.big_event_version <= 0)
	{
		return -3;
	}

	return 0;
}

int TaskPool::InitAnecdoteBigEventRwardCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq < 0 || seq > 30)
		{
			return -1;
		}

		int version = 0;
		if (!GetSubNodeValue(dataElement, "reward_reset", version) || version < 0 || version > ANECDOTE_BIG_EVENT_REWARD_VERSION)
		{
			return -111;
		}
		
		if (NULL != this->GetAnecdoteBigEventRewardCfg(seq, version))
		{
			return -2; // seq重复了
		}

		AnecdoteBigEventRewardCfg cfg;
		cfg.seq = seq;

		if (!GetSubNodeValue(dataElement, "complete_num", cfg.complete_anecdote_num) || cfg.complete_anecdote_num <= 0)
		{
			return -3;
		}

		cfg.item_num = 0;
		TiXmlElement * item_list = dataElement->FirstChildElement("item_list_list");
		if (NULL != item_list)	// 允许道具奖励为空
		{
			TiXmlElement * item_obj = item_list->FirstChildElement("item_list");
			while (NULL != item_obj)
			{
				if (cfg.item_num >= AnecdoteBigEventRewardCfg::MAX_REWARD_NUM)
				{
					return -10;
				}

				if (!cfg.item_list[cfg.item_num].ReadConfig(item_obj))
				{
					return -(1000 + cfg.item_num);
				}

				++cfg.item_num;
				item_obj = item_obj->NextSiblingElement();
			}
		}

		m_anecdote_big_event_reward_cfg_vec[version].push_back(cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitAnecdoteSpecialCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;
	special_anecdote_cfg.clear();
	while (dataElement)
	{
		AnecdoteSpecialCfg cfg;
		
		if (!GetSubNodeValue(dataElement, "special_type", cfg.special_type) || cfg.special_type < 0 || cfg.special_type >= MAX_SPECIAL_ANECDOTE_TASK_TYPE)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "monster_seq", cfg.monster_seq) || cfg.monster_seq < 0 || cfg.monster_seq >= MAX_SPECIAL_ANECDOTE_TASK_LIST)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "npc_id", cfg.npc_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "monster_id", cfg.monster_id))
		{
			return -4;
		}

		TiXmlElement * item_list = dataElement->FirstChildElement("item_list_list");
		if (NULL != item_list)	// 允许道具奖励为空
		{
			TiXmlElement * item_obj = item_list->FirstChildElement("item_list");
			while (NULL != item_obj)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(item_obj))
				{
					return -5;
				}
				cfg.reward_vec.push_back(reward);
				item_obj = item_obj->NextSiblingElement();
			}
		}

		special_anecdote_cfg.push_back(cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitConditionItemCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		ItemID item_id = 0;
		if (!GetSubNodeValue(dataElement, "task_item_type_3", item_id) || NULL == ITEMPOOL->GetItem(item_id))
		{
			return -1;
		}

		m_condition_item_set.insert(item_id);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitBountyTaskFinishRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -544;

	while (dataElement)
	{
		int min_level = 0, max_level = 0;
		if (!GetSubNodeValue(dataElement, "min_level", min_level) || min_level <= 0 || min_level > MAX_ROLE_LEVEL) return -1;
		if (!GetSubNodeValue(dataElement, "max_level", max_level) || max_level <= 0 || max_level > MAX_ROLE_LEVEL) return -2;

		if (min_level > max_level) return -3;

		if (m_bounty_task_finish_reward_cfg.empty())
		{
			m_bounty_task_finish_reward_cfg.push_back(BountyTaskFinishRewardCfg());
		}
		else
		{
			if (m_bounty_task_finish_reward_cfg[m_bounty_task_finish_reward_cfg.size() - 1].max_level >= min_level)
			{
				return -4;
			}
			m_bounty_task_finish_reward_cfg.push_back(BountyTaskFinishRewardCfg());
		}

		BountyTaskFinishRewardCfg& reward_cfg = m_bounty_task_finish_reward_cfg[m_bounty_task_finish_reward_cfg.size() - 1];
		reward_cfg.min_level = min_level;
		reward_cfg.max_level = max_level;

		TiXmlElement* reward_list_element = dataElement->FirstChildElement("reward_list");
		if (NULL != reward_list_element)
		{
			TiXmlElement* reward_element = reward_list_element->FirstChildElement("reward");
			while (reward_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(reward_element)) return -5;

				reward_cfg.reward_list.push_back(item);

				reward_element = reward_element->NextSiblingElement();
			}
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitPrestigeTaskVillageCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	int last_village_id = 0;
	while (dataElement)
	{
		int village_id = 0;
		if (!GetSubNodeValue(dataElement, "village_id", village_id) || village_id != last_village_id + 1)
		{
			return -1;
		}
		last_village_id = village_id;

		if (village_id <= 0 || village_id >= ARRAY_ITEM_COUNT(m_prestige_task_village_cfg_list))
		{
			return -111;
		}

		PrestigeTaskVillageCfg& cfg = m_prestige_task_village_cfg_list[village_id];
		cfg.is_valid = true;

		if (!GetSubNodeValue(dataElement, "unlock_level", cfg.unlock_level) || cfg.unlock_level < 0 || cfg.unlock_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		cfg.reward_num = 0;
		TiXmlElement * item_list = dataElement->FirstChildElement("item_list_list");
		if (NULL != item_list)	// 允许道具奖励为空
		{
			TiXmlElement * item_obj = item_list->FirstChildElement("item_list");
			while (NULL != item_obj)
			{
				if (cfg.reward_num >= TASK_REWARD_LIST_COUNT_MAX) return -10;

				if (!cfg.reward_list[cfg.reward_num].ReadConfig(item_obj))
				{
					return -(1000 + cfg.reward_num);
				}

				++cfg.reward_num;
				item_obj = item_obj->NextSiblingElement();
			}
		}
		else
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "end_reward_group", cfg.finish_all_reward_group))
		{
			return -4;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitPrestigeTaskPrestigeGroupCfg(TiXmlElement* RootElement, PrestigeGroupCfgMap& map)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int prestige_group = 0;
		if (!GetSubNodeValue(dataElement, "prestige_group", prestige_group) || prestige_group < 0)
		{
			return -1;
		}

		int role_level = 0;
		if (!GetSubNodeValue(dataElement, "role_level", role_level) || role_level < 0 || role_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		int prestige_num = 0;
		if (!GetSubNodeValue(dataElement, "prestige_num", prestige_num) || prestige_num <= 0)
		{
			return -3;
		}

		PrestigeTaskPrestigeGroupCfg::Cfg cfg;
		cfg.role_level = role_level;
		cfg.add_prestige_num = prestige_num;
		map[prestige_group].level_prestige_vec.push_back(cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	PrestigeGroupCfgMap::iterator it = map.begin();
	for (; it != map.end(); ++it)
	{
		for (size_t i = 1; i < it->second.level_prestige_vec.size(); ++i)
		{
			if (it->second.level_prestige_vec[i].role_level <= it->second.level_prestige_vec[i - 1].role_level)
			{
				return -100;
			}
		}
	}

	return 0;
}

int TaskPool::InitPrestigeTaskCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int village_id = 0;
		if (!GetSubNodeValue(dataElement, "village_id", village_id) || village_id <= 0 || village_id >= PRESTIGE_VILLAGE_MAX_NUM)
		{
			return -1;
		}

		int task_group = 0;
		if (!GetSubNodeValue(dataElement, "task_group", task_group) || task_group <= 0)
		{
			return -2;
		}

		int task_id = 0;
		if (!GetSubNodeValue(dataElement, "task_id", task_id) || task_id <= 0)
		{
			return -3;
		}

		int prestige_group = 0;
		if (!GetSubNodeValue(dataElement, "prestige_group", prestige_group) || prestige_group < 0)
		{
			return -4;
		}

		int is_activedegree = 0;
		if (!GetSubNodeValue(dataElement, "is_activedegree", is_activedegree) || is_activedegree < 0 || is_activedegree > 1)
		{
			return -5;
		}

		PrestigeTaskCfg cfg;
		cfg.village_id = village_id;
		cfg.task_id = task_id;
		cfg.task_group = task_group;
		cfg.prestige_reward_group = prestige_group;
		cfg.is_activedegree = (is_activedegree > 0);

		m_prestige_task_cfg_map.insert(PrestigeTaskCfgMap::value_type(task_id, cfg));

		if (!this->CheckPrestigeTaskNumInSameGroup(village_id, task_group, MAX_PRESTIGE_TASK_NUM_PER_DAY))
		{
			return -1000;
		}

		{
			bool is_found = false;
			for (int i = 0; i < m_prestige_task_group_vec[village_id].size(); ++i)
			{
				if (m_prestige_task_group_vec[village_id][i] == task_group)
				{
					is_found = true;
					break;
				}
			}
			if (!is_found)
			{
				m_prestige_task_group_vec[village_id].push_back(task_group);
			}
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitPrestigeTaskStarCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	int total_rate_1 = 0, total_rate_2 = 0;
	while (NULL != dataElement)
	{
		PrestigeTaskStarCfg cfg;
		TiXmlElement * list_element = dataElement->FirstChildElement("item_list_list");
		if (NULL == list_element)
			return -1;

		TiXmlElement * item_element = list_element->FirstChildElement("item_list");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
				return -2;

			cfg.reward_item.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		m_prestige_task_star_cfg.push_back(cfg);

		int star = 0;
		if (!GetSubNodeValue(dataElement, "star_level", star) || star <= 0)
			return -3;

		int rate_1 = 0;
		if (!GetSubNodeValue(dataElement, "rate_1", rate_1) || rate_1 <= 0)
			return -4;
		total_rate_1 += rate_1;
		m_prestige_task_star_rate_1_map[total_rate_1] = star;

		int rate_2 = 0;
		if (!GetSubNodeValue(dataElement, "rate_2", rate_2) || rate_2 < 0)
			return -4;

		if (rate_2 > 0)
		{
			total_rate_2 += rate_2;
			m_prestige_task_star_rate_2_map[total_rate_2] = star;
		}	

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TaskPool::InitPrestigeTaskOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "reset_times", m_prestige_task_other_cfg.reset_times) || m_prestige_task_other_cfg.reset_times <= 0)
		return -1;

	if (!GetSubNodeValue(dataElement, "refresh_item", m_prestige_task_other_cfg.use_item) || NULL == ITEMPOOL->GetItem(m_prestige_task_other_cfg.use_item))
		return -2;

	return 0;
}

void TaskPool::InsertPrevTask(TaskID prec_task, TaskID task_id)
{
	std::map<TaskID, std::set<TaskID> >::iterator m_it = m_prev_task.find(prec_task);
	if (m_prev_task.end() != m_it)		// 如果找到
	{
		m_it->second.insert(task_id);
	}
	else
	{
		std::set<TaskID> t_list; t_list.insert(task_id);
		m_prev_task.insert(std::pair<TaskID, std::set<TaskID> >(prec_task, t_list));
	}
}

void TaskPool::InsertNextTask(TaskID prec_task, TaskID task_id)
{
	std::map<TaskID, std::set<TaskID> >::iterator m_it = m_next_task.find(prec_task);
	if (m_next_task.end() != m_it)		// 如果找到
	{
		m_it->second.insert(task_id);
	}
	else
	{
		std::set<TaskID> t_list; t_list.insert(task_id);
		m_next_task.insert(std::pair<TaskID, std::set<TaskID> >(prec_task, t_list));
	}
}

void TaskPool::InsertCuLvTask(UInt16 min_level, TaskID task_id)
{
	if (min_level < 0 || min_level > MAX_ROLE_LEVEL)
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}

	if (NULL != m_acpb_task[min_level])
	{
		m_acpb_task[min_level]->insert(task_id);
	}
	else
	{
		m_acpb_task[min_level] = new std::set<TaskID>();
		m_acpb_task[min_level]->insert(task_id);
	}
}

void TaskPool::InsertProfTask(int base_prof_type, TaskID task_id)
{
	if (base_prof_type < 0 || base_prof_type >= PROFESSION_TYPE_NUM)
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}

	if (NULL != m_prof_task[base_prof_type])
	{
		m_prof_task[base_prof_type]->insert(task_id);
	}
	else
	{
		m_prof_task[base_prof_type] = new std::set<TaskID>();
		m_prof_task[base_prof_type]->insert(task_id);
	}
}

void TaskPool::InsertTaskType(int task_type, TaskID task_id)
{
	if (task_type < 0 || task_type >= TASK_TYPE_MAX)
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}

	if (NULL != m_task_type_list[task_type])
	{
		m_task_type_list[task_type]->insert(task_id);
	}
	else
	{
		m_task_type_list[task_type] = new std::set<TaskID>();
		m_task_type_list[task_type]->insert(task_id);
	}
}

bool TaskPool::CheckMainTask(std::string& error_msg)
{
	char errmsg[1024] = { 0 };

	int last_main_task_id = 0;
	int first_main_task_id = 0;
	for (int i = 0; i < MAX_UINT16; ++i)
	{
		const TaskInfomaConfig* task = this->GetTaskCfgByID(i);
		if (NULL == task || task->task_type != TASK_TYPE_MAIN_QUEST)
		{
			continue;
		}

		std::set<TaskID> next_task_set;
		this->GetNextTaskList(task->task_id, next_task_set);
		if (next_task_set.size() > 1)
		{
			SNPRINTF(errmsg, sizeof(errmsg), "CheckMainTask task[%d] next_task more than 1", task->task_id);
			error_msg += errmsg;
			return false;
		}
		else if (next_task_set.size() == 0)
		{
			// 没有下一个任务，应该是最后一个任务
			if (last_main_task_id != 0)
			{
				// 如果之前已有一个任务无后置任务，一定是出错了
				SNPRINTF(errmsg, sizeof(errmsg), "CheckMainTask task[%d] task[%d], found two final tasks, should be one!", task->task_id, last_main_task_id);
				error_msg += errmsg;
				return false;
			}
			else
			{
				last_main_task_id = task->task_id;
			}
		}
		
		std::set<TaskID> prev_task_set;
		this->GetPrevTaskList(task->task_id, prev_task_set);
		if (prev_task_set.size() > 1)
		{
			SNPRINTF(errmsg, sizeof(errmsg), "CheckMainTask task[%d] prev_task more than 1", task->task_id);
			error_msg += errmsg;
			return false;
		}
		else if (prev_task_set.size() == 0)
		{
			// 没有前一个任务，应该是第一个任务
			if (first_main_task_id != 0)
			{
				// 如果之前已有一个任务无前置任务，一定是出错了
				SNPRINTF(errmsg, sizeof(errmsg), "CheckMainTask task[%d] task[%d], found two first-tasks, should be one!", task->task_id, first_main_task_id);
				error_msg += errmsg;
				return false;
			}
			else
			{
				first_main_task_id = task->task_id;
			}
		}
	}

	if (0 == first_main_task_id)
	{
		SNPRINTF(errmsg, sizeof(errmsg), "CheckMainTask first task not found !");
		error_msg += errmsg;
		return false;
	}

	if (0 == last_main_task_id)
	{
		SNPRINTF(errmsg, sizeof(errmsg), "CheckMainTask final task not found !");
		error_msg += errmsg;
		return false;
	}

	bool flag[MAX_UINT16] = { 0 };
	int cur_task = last_main_task_id;
	while (cur_task != first_main_task_id)
	{
		const TaskInfomaConfig* task = this->GetTaskCfgByID(cur_task);
		if (NULL == task)
		{
			SNPRINTF(errmsg, sizeof(errmsg), "CheckMainTask task not exist at task[%d]", cur_task);
			error_msg += errmsg;
			return false;
		}

		if (flag[cur_task])
		{
			SNPRINTF(errmsg, sizeof(errmsg), "CheckMainTask task loop found at task[%d]", cur_task);
			error_msg += errmsg;
			return false;
		}
		flag[cur_task] = true;
		
		cur_task = task->pre_task_id;
	}

	return true;
}

int TaskPool::CheckPrevNextTask()
{
	// 确保每个任务最多只有1个后置和1个前置任务
	for (int i = 1; i < MAX_UINT16; ++i)
	{
		const TaskInfomaConfig* cfg = this->GetTaskCfgByID(i);
		if (NULL == cfg) continue;

		std::set<TaskID> next_task;
		this->GetNextTaskList(i, next_task);

		std::set<TaskID> prev_task;
		this->GetPrevTaskList(i, prev_task);

		if (next_task.size() > 1 || prev_task.size() > 1)
		{
			printf("CheckPrevNextTask task[%d] next_size[%d] prev_size[%d]\n",
				i, (int)next_task.size(), (int)prev_task.size());
			return -111;
		}

		if (cfg->nex_task_id > 0)
		{
			if (next_task.find(cfg->nex_task_id) == next_task.end())
			{
				printf("CheckPrevNextTask task[%d] next_task_id[%d] not found in set\n",
					cfg->task_id, cfg->nex_task_id);
				return -222;
			}
		}

		if (cfg->pre_task_id > 0)
		{
			if (prev_task.find(cfg->pre_task_id) == prev_task.end())
			{
				printf("CheckPrevNextTask task[%d] prev_task_id[%d] not found in set\n",
					cfg->task_id, cfg->pre_task_id);
				return -333;
			}
		}
	}

	return 0;
}

void TaskPool::SetPetFeiShengFirstTask()
{	
	//task_param1 飞升阶段
	//task_param2 完成任务后的飞升阶段

	m_pet_fei_sheng_first_task_id.clear();
	for (int i = 1; i < MAX_UINT16; ++i)
	{
		const TaskInfomaConfig * cfg = this->GetTaskCfgByID(i);
		if (NULL == cfg) continue;

		if (cfg->task_type != TASK_TYPE_PET_FEI_SHENG_TASK)
		{
			continue;
		}

		if (cfg->pre_task_id != 0)
		{
			const TaskInfomaConfig * per_cfg = this->GetTaskCfgByID(i);
			if (per_cfg == NULL)
			{
				continue;
			}

			if (per_cfg->task_param1 + 1 != cfg->task_param1 || per_cfg->task_param2 != cfg->task_param1)
			{
				continue;
			}
		}

		MapAdd(m_pet_fei_sheng_first_task_id, cfg->task_param1, cfg->task_id);
		continue;
	}
}

const PrestigeTaskPrestigeGroupCfg* TaskPool::GetPrestigeRewardGroupCfg(int reward_group, const PrestigeGroupCfgMap& map)
{
	PrestigeGroupCfgMap::const_iterator it = map.find(reward_group);
	if (it == map.end()) return NULL;

	return &it->second;
}

bool TaskPool::CheckPrestigeTaskNumInSameGroup(int village, int group, int max_num)
{
	int num = 0;
	PrestigeTaskCfgMap::iterator it = m_prestige_task_cfg_map.begin();
	for (; it != m_prestige_task_cfg_map.end(); ++it)
	{
		if (it->second.village_id != village || it->second.task_group != group)
		{
			continue;
		}
		num += 1;
	}

	return num <= max_num;
}
