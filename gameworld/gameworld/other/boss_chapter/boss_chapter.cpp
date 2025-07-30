#include "boss_chapter.hpp"
#include "protocol/msgbosschapter.h"
#include "other/rolemodulemanager.hpp"
#include "task/taskpool.h"
#include "item/knapsack.h"
#include "servercommon/errornum.h"
#include "task/taskmanager.h"
#include "gamelog.h"
#include "obj/character/role.h"
#include "battle/battle_manager_local.hpp"
#include "task/taskrecorder.h"

BossChapter::BossChapter() : m_mgr(NULL), m_chapter_reward_flag(0LL)
{

}

BossChapter::~BossChapter()
{

}

void BossChapter::Init(RoleModuleManager* mgr, const BossChapterParam& param)
{
	m_mgr = mgr;
	for (int i = 0; i < param.info_num; ++i)
	{
		m_info_map.insert(BossChapterInfoMap::value_type(param.info_list[i].monster_group_id, param.info_list[i]));
	}
	m_chapter_reward_flag = param.chapter_reward_flag;
}

void BossChapter::GetInitParam(BossChapterParam* param)
{
	param->info_num = 0;

	BossChapterInfoMap::iterator it = m_info_map.begin();
	for (; it != m_info_map.end() && param->info_num < BOSS_CHAPTER_MAX_BOSS_NUM; ++it)
	{
		param->info_list[param->info_num++] = it->second;
	}

	param->chapter_reward_flag = m_chapter_reward_flag;
}

void BossChapter::ClaimReward(int monster_group_id)
{
	const BossChapterBossCfg* boss_cfg = TaskPool::Instance()->GetBossChapterBossCfg(monster_group_id);
	if (NULL == boss_cfg)
	{
		m_mgr->NoticeNum(errornum::EN_BOSS_CHAPTER_BOSS_NOT_FOUND);
		return;
	}

	BossChapterInfo* info = this->GetChapterInfo(monster_group_id);
	if (NULL == info)
	{
		m_mgr->NoticeNum(errornum::EN_BOSS_CHAPTER_BOSS_NOT_KILLED);
		return;
	}

	if (info->reward_status != BOSS_CHAPTER_REWARD_STATUS_CLAIMABLE)
	{
		m_mgr->NoticeNum(errornum::EN_BOSS_CHAPTER_REWARD_CLAIMED);
		return;
	}

	if (0 == boss_cfg->reward_num)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	std::vector<const ItemConfigData*> reward_list;
	if (0 == m_mgr->GetRole()->GetProfession())
	{
		reward_list.reserve(boss_cfg->reward_num);
		for (int i = 0; i < boss_cfg->reward_num; ++i)
		{
			// 当没有职业时，不下发转换物品
			const ItemBase* item_base = ITEMPOOL->GetItem(boss_cfg->reward_list[i].item_id);
			if (NULL == item_base || I_TYPE_CONVERT_ITEM == item_base->GetItemType()) continue;

			reward_list.push_back(&boss_cfg->reward_list[i]);
		}
	}
	else
	{
		reward_list.reserve(boss_cfg->reward_num);
		for (int i = 0; i < boss_cfg->reward_num; ++i)
		{
			reward_list.push_back(&boss_cfg->reward_list[i]);
		}
	}

	if (!m_mgr->GetKnapsack()->CheckForPutList((short)reward_list.size(), reward_list[0]))
	{
		m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (!m_mgr->GetKnapsack()->PutList((short)reward_list.size(), reward_list[0], PUT_REASON_BOSS_CHAPTER_BOSS_REWARD))
	{
		m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	info->reward_status = BOSS_CHAPTER_REWARD_STATUS_CLAIMED;

	this->SendOnInfoChange(info);

	gamelog::g_log_boss_chapter.printf(LL_INFO, "BossChapter::ClaimReward role[%d,%s] claim boss reward, monster_group_id[%d]",
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), monster_group_id);
}

void BossChapter::ClaimChapterReward(int chapter)
{
	const TaskPool::ChapterBossList* list = TASKPOOL->GetChapterBossList(chapter);
	if (NULL == list)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const BossChapterChapterRewardCfg* cfg = TASKPOOL->GetBossChapterChapterRewardCfg(chapter);
	if (NULL == cfg)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if ((m_chapter_reward_flag & (1LL << chapter)) != 0)
	{
		m_mgr->NoticeNum(errornum::EN_BOSS_CHAPTER_REWARD_CLAIMED);
		return; // 已领取
	}

	for (size_t i = 0; i < list->size(); ++i)
	{ 
		int monster_group_id = (*list)[i];
		BossChapterInfo* info = this->GetChapterInfo(monster_group_id);
		if (NULL == info)
		{
			m_mgr->NoticeNum(errornum::EN_BOSS_CHAPTER_CHAPTER_BOSS_NOT_ALL_KILLED);
			return; // 没有打完章节所有BOSS
		}
	}

	std::vector<const ItemConfigData*> reward_list;
	if (0 == m_mgr->GetRole()->GetProfession())
	{
		reward_list.reserve(cfg->reward_num);
		for (int i = 0; i < cfg->reward_num; ++i)
		{
			// 当没有职业时，不下发转换物品
			const ItemBase* item_base = ITEMPOOL->GetItem(cfg->reward_list[i].item_id);
			if (NULL == item_base || I_TYPE_CONVERT_ITEM == item_base->GetItemType()) continue;

			reward_list.push_back(&cfg->reward_list[i]);
		}
	}
	else
	{
		reward_list.reserve(cfg->reward_num);
		for (int i = 0; i < cfg->reward_num; ++i)
		{
			reward_list.push_back(&cfg->reward_list[i]);
		}
	}

	if (!m_mgr->GetKnapsack()->CheckForPutList((short)reward_list.size(), reward_list[0]))
	{
		m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (!m_mgr->GetKnapsack()->PutList((short)reward_list.size(), reward_list[0], PUT_REASON_BOSS_CHAPTER_BOSS_REWARD))
	{
		m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_chapter_reward_flag |= (1LL << chapter);

	this->SendOtherInfoChange();

	gamelog::g_log_boss_chapter.printf(LL_INFO, "BossChapter::ClaimChapterReward role[%d,%s] chapter[%d] reward_flag[%lld]",
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), chapter, m_chapter_reward_flag);
}

void BossChapter::StartBattleReq(int monster_group_id)
{
	const BossChapterBossCfg* boss_cfg = TaskPool::Instance()->GetBossChapterBossCfg(monster_group_id);
	if (NULL == boss_cfg)
	{
		m_mgr->NoticeNum(errornum::EN_BOSS_CHAPTER_BOSS_NOT_FOUND);
		return;
	}

	if (NULL != this->GetChapterInfo(monster_group_id))
	{
		m_mgr->NoticeNum(errornum::EN_BOSS_CHAPTER_BOSS_KILLED);
		return; //已经打过了
	}

	if (boss_cfg->related_task_id != 0 && !m_mgr->GetTaskManager()->CheckRoleHasTask(boss_cfg->related_task_id))
	{
		// 没有接到任务, 不能打
		m_mgr->NoticeNum(errornum::EN_BOSS_CHAPTER_TASK_NOT_ACCEPTED);
		return;
	}
	BattleManagerLocal::GetInstance().StartMonsterFightReq(m_mgr->GetRole(), monster_group_id, BATTLE_MODE_NORMAL_MONSTER, 0);
}

void BossChapter::OnKillMonsterGroup(int monster_group_id)
{	
	const BossChapterBossCfg* boss_cfg = TaskPool::Instance()->GetBossChapterBossCfg(monster_group_id);
	if (NULL == boss_cfg) return;

	if (NULL != this->GetChapterInfo(monster_group_id)) return; //已经打过了
	
	if (boss_cfg->related_task_id != 0 && !m_mgr->GetTaskManager()->CheckRoleHasTask(boss_cfg->related_task_id)
		&& !m_mgr->GetTaskRecorder()->HaveFinished(boss_cfg->related_task_id))
	{
		// 没有接到任务，或者完成任务，打了不算
		return;
	}

	static BossChapterInfo info;
	info.Reset();
	info.chapter = 0;
	info.monster_group_id = monster_group_id;
	info.reserve_ch = 0;
	info.reward_status = BOSS_CHAPTER_REWARD_STATUS_CLAIMABLE;

	m_info_map.insert(BossChapterInfoMap::value_type(monster_group_id, info));

	this->SendOnAdd(&info);

	gamelog::g_log_boss_chapter.printf(LL_INFO, "BossChapter::OnKillMonsterGroup role[%d,%s] monster_group_id[%d]",
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), monster_group_id);
}

void BossChapter::SendAllInfo()
{
	static Protocol::SCBossChapterInfo msg;
	msg.num = 0;
	msg.reason = Protocol::SCBossChapterInfo::REASON_INIT;
	msg.chapter_reward_flag = m_chapter_reward_flag;

	BossChapterInfoMap::iterator it = m_info_map.begin();
	for (; it != m_info_map.end() && msg.num < BOSS_CHAPTER_MAX_BOSS_NUM; ++it)
	{
		msg.info_list[msg.num].monster_group_id = it->second.monster_group_id;
		msg.info_list[msg.num].reward_status = it->second.reward_status;
		msg.info_list[msg.num].reserve_ch = 0;
		msg.info_list[msg.num].reserve_sh = 0;
		msg.num += 1;
	}

	int length = sizeof(msg) - sizeof(msg.info_list) + msg.num * sizeof(msg.info_list[0]);
	m_mgr->NetSend(&msg, length);
}

void BossChapter::SendOnAdd(BossChapterInfo* info)
{
	if (NULL == info) return;

	static Protocol::SCBossChapterInfo msg;
	msg.num = 1;
	msg.reason = Protocol::SCBossChapterInfo::REASON_ADD;
	msg.chapter_reward_flag = m_chapter_reward_flag;

	msg.info_list[0].monster_group_id = info->monster_group_id;
	msg.info_list[0].reward_status = info->reward_status;
	msg.info_list[0].reserve_ch = 0;
	msg.info_list[0].reserve_sh = 0;

	int length = sizeof(msg) - sizeof(msg.info_list) + msg.num * sizeof(msg.info_list[0]);
	m_mgr->NetSend(&msg, length);
}

void BossChapter::SendOnInfoChange(BossChapterInfo* info)
{
	if (NULL == info) return;

	static Protocol::SCBossChapterInfo msg;
	msg.num = 1;
	msg.reason = Protocol::SCBossChapterInfo::REASON_INFO_CHANGE;
	msg.chapter_reward_flag = m_chapter_reward_flag;

	msg.info_list[0].monster_group_id = info->monster_group_id;
	msg.info_list[0].reward_status = info->reward_status;
	msg.info_list[0].reserve_ch = 0;
	msg.info_list[0].reserve_sh = 0;

	int length = sizeof(msg) - sizeof(msg.info_list) + msg.num * sizeof(msg.info_list[0]);
	m_mgr->NetSend(&msg, length);
}

void BossChapter::SendOtherInfoChange()
{
	static Protocol::SCBossChapterInfo msg;
	msg.num = 0;
	msg.reason = Protocol::SCBossChapterInfo::REASON_OTHER_INFO_CHANGE;
	msg.chapter_reward_flag = m_chapter_reward_flag;

	int length = sizeof(msg) - sizeof(msg.info_list) + msg.num * sizeof(msg.info_list[0]);
	m_mgr->NetSend(&msg, length);
}

BossChapterInfo* BossChapter::GetChapterInfo(int monster_group_id)
{
	BossChapterInfoMap::iterator it = m_info_map.find(monster_group_id);
	if (it == m_info_map.end())
	{
		return NULL;
	}

	return &it->second;
}
