#include "treasuretask.hpp"

#include "config/logicconfigmanager.hpp"
#include "treasuretaskconfig.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "world.h"
#include "item/knapsack.h"
#include "servercommon/errornum.h"
#include "other/event/eventhandler.hpp"
#include "gameworld/gamelog.h"
#include "protocol/msgtask.h"
#include "servercommon/string/gameworldstr.h"
#include "other/funopen/funopen.hpp"
#include "other/rolejoinactivityreward/rolejoinactivityreward.hpp"
#include "other/activesystem/activesystem.hpp"

TreasureTask::TreasureTask()
{
}

TreasureTask::~TreasureTask()
{
}

void TreasureTask::Init(RoleModuleManager * mgr, const TreasureTaskParam & param)
{
	m_role_module_mgr = mgr;
	m_param = param;
}

void TreasureTask::GetInitParam(TreasureTaskParam * param)
{
	*param = m_param;
}

void TreasureTask::OnDayChange()
{
	m_param.Reset();

	this->SendInfo();
}

void TreasureTask::OnReq(void * msg)
{
	if (NULL == msg)
		return;

	Protocol::CSTreasureTaskReq * req = (Protocol::CSTreasureTaskReq*)msg;

	switch (req->type)
	{
	case 0:
		{
			this->SendInfo();
		}
		break;
	case 1:
		{
			this->OnAccept();
		}
		break;
	case 2:
		{
			this->OnCommit();
		}
		break;
	}
}

void TreasureTask::OnAccept()
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_JIUZHOU_MIBAO, true)) return;

// 	if (m_role_module_mgr->GetRole()->GetLevel() < LOGIC_CONFIG->GetTreasureTaskConfig()->GetOtherCfg().level)
// 	{
// 		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
// 		return;
// 	}

	if (m_param.order_id != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TASK_ALREADY_ACCEPTED);
		return;
	}

	m_param.order_id = 1;
	int route_num = LOGIC_CONFIG->GetTreasureTaskConfig()->GetRouteNum();
	m_param.route = RandomNum(1, route_num + 1);

	m_param.level = m_role_module_mgr->GetRole()->GetLevel();
	m_param.pos_id = LOGIC_CONFIG->GetTreasureTaskConfig()->GetCoordomate(m_param.route, m_param.order_id);

	this->RandPool();
	this->SendInfo();
}

void TreasureTask::OnCommit()
{
	if (0 == m_param.pos_id)
		return;

	std::map<int, int> pool_map;
	// 生成奖池
	int total_rate = 0;
	const std::vector<TreasureTaskLevelCfg> * level_map = LOGIC_CONFIG->GetTreasureTaskConfig()->GetTaskLevelVec(m_param.level);
	if (NULL == level_map)
		return;

	for (int i = 0; i < TREASURE_TASK_POOL_NNM; i++)
	{
		int seq = m_param.pool[i];
		if(seq <= 0 || seq > (int)level_map->size())
			continue;

		total_rate += (*level_map)[seq - 1].get_rate;
		pool_map[total_rate] = i;
	}

	// 获得奖励
	int rate = RandomNum(total_rate);
	std::map<int, int>::iterator it = pool_map.upper_bound(rate);
	if (it == pool_map.end())
		return;

	int level_seq = m_param.pool[it->second];
	if (level_seq <= 0 || level_seq > (int)level_map->size())
		return;

	int group_id = (*level_map)[level_seq - 1].reward_group;
	const TreasureTaskGroupCfg * reward_cfg = LOGIC_CONFIG->GetTreasureTaskConfig()->GetGroupRewardCfg(group_id);
	if (NULL == reward_cfg)
		return;

	bool is_baodi = false;
	if (m_param.baodi_time >= LOGIC_CONFIG->GetTreasureTaskConfig()->GetOtherCfg().floors) is_baodi = true;

	std::vector<ItemConfigData> reward_list;
	if (it->second == 0 || is_baodi)
	{
		if (reward_cfg->seq == 0 && !is_baodi)  // 0是好事成双 1是满载而归
		{
			while (it->second == 0)
			{
				int rate = RandomNum(total_rate);
				it = pool_map.upper_bound(rate);
				if (it == pool_map.end())
					return;
			}

			int seq = m_param.pool[it->second];
			if (seq <= 0 || seq > (int)level_map->size())
				return;

			int special_group_id = (*level_map)[seq - 1].reward_group;

			for (int i = 0; i < 2; i++)
			{
				const TreasureTaskGroupCfg * special_reward_cfg = LOGIC_CONFIG->GetTreasureTaskConfig()->GetGroupRewardCfg(special_group_id);
				if (NULL == special_reward_cfg)
					continue;

				reward_list.push_back(special_reward_cfg->reward);

				if (m_param.day_total_num < TREASURE_TASK_TOTAL_ITEM_NNM)
				{
					m_param.day_total_item[m_param.day_total_num].item_id = special_reward_cfg->reward.item_id;
					m_param.day_total_item[m_param.day_total_num].num = special_reward_cfg->reward.num;
					m_param.day_total_num++;
				}
			}

			m_role_module_mgr->GetKnapsack()->NotifyGetItemListVec(reward_list, GET_REASON_TREASURE_TASK, 0, it->second);
			m_param.baodi_time++;
		}
		else
		{
			for (int i = 1; i < TREASURE_TASK_POOL_NNM; i++)
			{
				int seq = m_param.pool[i];
				if (seq <= 0 || seq > (int)level_map->size())
					return;

				int special_group_id = (*level_map)[seq - 1].reward_group;
				const TreasureTaskGroupCfg * special_reward_cfg = LOGIC_CONFIG->GetTreasureTaskConfig()->GetGroupRewardCfg(special_group_id);
				if (NULL == special_reward_cfg)
					continue;

				reward_list.push_back(special_reward_cfg->reward);

				if (m_param.day_total_num < TREASURE_TASK_TOTAL_ITEM_NNM)
				{
					m_param.day_total_item[m_param.day_total_num].item_id = special_reward_cfg->reward.item_id;
					m_param.day_total_item[m_param.day_total_num].num = special_reward_cfg->reward.num;
					m_param.day_total_num++;
				}
			}

			m_role_module_mgr->GetKnapsack()->NotifyGetItemListVec(reward_list, GET_REASON_TREASURE_TASK, 0, 0);
			m_param.baodi_time = 0;

			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_treasure_task_notice, 
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName());
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_ROLLING_NOTICE));
				}
			}
		}
	}
	else
	{
		// 发送获奖通知
		m_role_module_mgr->GetKnapsack()->NotifyGetItemList(1, &reward_cfg->reward, GET_REASON_TREASURE_TASK, it->second);
		m_param.baodi_time++;

		reward_list.push_back(reward_cfg->reward);

		if (m_param.day_total_num < TREASURE_TASK_TOTAL_ITEM_NNM)
		{
			m_param.day_total_item[m_param.day_total_num].item_id = reward_cfg->reward.item_id;
			m_param.day_total_item[m_param.day_total_num].num = reward_cfg->reward.num;
			m_param.day_total_num++;
		}	
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)reward_list.size(), &reward_list[0]))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (short)reward_list.size(), SEND_MAIL_TYPE_TREASURE_TASK, true);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutVec(reward_list, PUT_REASON_TREASURE_TASK);
	}

	// log
	gamelog::g_log_treasure_task.printf(LL_INFO, "TreasureTask::OnCommit role[%d,%s] level[%d] route[%d] task_tims[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_param.level, m_param.route, m_param.order_id);

	EventHandler::Instance().OnCommitTreasureTask(m_role_module_mgr->GetRole());

	// 刷新下一个任务
	this->ReshrefTask();
}

void TreasureTask::SendInfo()
{
	Protocol::SCTreasureTaskInfo info;
	info.level = m_param.level;
	info.order_id = m_param.order_id;
	info.route = m_param.route;
	info.pos_id = m_param.pos_id;

	memcpy(info.pool, m_param.pool, sizeof(info.pool));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void TreasureTask::SendTotalItem()
{
	std::vector<ItemConfigData> total_item;

	for (int i = 0; i < m_param.day_total_num && i < TREASURE_TASK_TOTAL_ITEM_NNM; i++)
	{
		ItemConfigData item(m_param.day_total_item[i].item_id, 1, m_param.day_total_item[i].num);

		total_item.push_back(item);
	}

	m_role_module_mgr->GetKnapsack()->NotifyGetItemListVec(total_item, GET_REASON_TREASURE_TASK_TOTAL_ITEM);
}

bool TreasureTask::IsCanOneClick()
{
	return FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_JIUZHOU_MIBAO) && !m_role_module_mgr->GetActiveSystem()->IsStartedActive(ACTIVE_SYSTEM_TYPE_TREASURE_TASK);
}

void TreasureTask::OneClickFinishTask(unsigned int & exp, int & coin, int & jade, std::vector<ItemConfigData>& rewards)
{
	for (int i = 0; i < LOGIC_CONFIG->GetTreasureTaskConfig()->GetOtherCfg().quantity; i++)
	{
		const std::vector<TreasureTaskLevelCfg> * level_map = LOGIC_CONFIG->GetTreasureTaskConfig()->GetTaskLevelVec(m_role_module_mgr->GetRole()->GetLevel());
		if (NULL == level_map) continue;

		std::vector<TreasureTaskLevelCfg> group(level_map->begin(), level_map->end());
		short new_pool[TREASURE_TASK_POOL_NNM] = { 0 };
		new_pool[0] = 1;

		int MAX_RATE = 10000;
		for (int i = 1; i < TREASURE_TASK_POOL_NNM; i++)
		{
			int rand_num = RandomNum(MAX_RATE);
			int rate_num = 0;
			for (int n = 1; n < (int)group.size(); n++)
			{
				rate_num += group[n].rate;
				if (rand_num < rate_num)
				{
					MAX_RATE -= group[n].rate;
					new_pool[i] = group[n].seq;

					group.erase(group.begin() + n);
					break;
				}
			}
		}

		std::map<int, int> pool_map;
		// 生成奖池
		int total_rate = 0;

		for (int i = 0; i < TREASURE_TASK_POOL_NNM; i++)
		{
			int seq = new_pool[i];
			if (seq <= 0 || seq > (int)level_map->size())
				continue;

			total_rate += (*level_map)[seq - 1].get_rate;
			pool_map[total_rate] = i;
		}

		// 获得奖励
		int rate = RandomNum(total_rate);
		std::map<int, int>::iterator it = pool_map.upper_bound(rate);
		if (it == pool_map.end())
			continue;

		int level_seq = new_pool[it->second];
		if (level_seq <= 0 || level_seq > (int)level_map->size())
			continue;

		int group_id = (*level_map)[level_seq - 1].reward_group;
		const TreasureTaskGroupCfg * reward_cfg = LOGIC_CONFIG->GetTreasureTaskConfig()->GetGroupRewardCfg(group_id);
		if (NULL == reward_cfg)
			continue;

		if (it->second != 0)
		{
			RoleJoinActivityReward::AddRewards(rewards, reward_cfg->reward);
		}

		m_param.order_id++;
		EventHandler::Instance().OnCommitTreasureTask(m_role_module_mgr->GetRole());	
	}

	m_param.order_id++;
	EventHandler::Instance().OnFinishTreasureTask(m_role_module_mgr->GetRole());
	this->SendInfo();
}

void TreasureTask::RandPool()
{
	memset(m_param.pool, 0, sizeof(m_param.pool));
	if (m_param.level == 0) m_param.level = 1;

	const std::vector<TreasureTaskLevelCfg> * level_map = LOGIC_CONFIG->GetTreasureTaskConfig()->GetTaskLevelVec(m_param.level);
	if (NULL == level_map)
		return;

	std::vector<TreasureTaskLevelCfg> group(level_map->begin(), level_map->end());
	short new_pool[TREASURE_TASK_POOL_NNM] = { 0 };
	new_pool[0] = 1;

	int MAX_RATE = 10000;
	for (int i = 1; i < TREASURE_TASK_POOL_NNM; i++)
	{
		int rand_num = RandomNum(MAX_RATE);
		int rate_num = 0;
		for (int n = 1; n < (int)group.size(); n++)
		{
			rate_num += group[n].rate;
			if (rand_num < rate_num)
			{
				MAX_RATE -= group[n].rate;
				new_pool[i] = group[n].seq;

				group.erase(group.begin() + n);
				break;
			}
		}
	}
	
	memcpy(m_param.pool, new_pool, sizeof(m_param.pool));
}

void TreasureTask::ReshrefTask()
{
	m_param.order_id += 1;
	m_param.pos_id = 0;

	if (m_param.order_id > LOGIC_CONFIG->GetTreasureTaskConfig()->GetOtherCfg().quantity)
	{
		this->SendInfo();
		this->SendTotalItem();
		EventHandler::Instance().OnFinishTreasureTask(m_role_module_mgr->GetRole());
		return;
	}

	m_param.level = m_role_module_mgr->GetRole()->GetLevel();
	m_param.pos_id = LOGIC_CONFIG->GetTreasureTaskConfig()->GetCoordomate(m_param.route, m_param.order_id);

	this->RandPool();
	this->SendInfo();
}

