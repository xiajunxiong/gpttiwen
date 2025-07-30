#include "roleactivityluckyturntable.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityluckyturntableconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/schooltask/schooltask.h"
#include "other/researchtask/researchtask.h"
#include "other/monster_wave/monsterwave.h"
#include "other/treasure_map/treasuremap.h"
#include "other/bountytask/bountytask.hpp"
#include "other/transactiontask/transactiontask.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

RoleActivityLuckyTurnTable::RoleActivityLuckyTurnTable(int type)
	:RoleActivity(type)
{

}

RoleActivityLuckyTurnTable::~RoleActivityLuckyTurnTable()
{

}

void RoleActivityLuckyTurnTable::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RALuckyTurnTableParam));
}

void RoleActivityLuckyTurnTable::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RALuckyTurnTableParam));
}

void RoleActivityLuckyTurnTable::OnLoginSendInfo()
{
	this->SendRALuckyTurntableInfo();
}

void RoleActivityLuckyTurnTable::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE != rand_act_type)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity *activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_lucky_turntable_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_lucky_turntable_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivityLuckyTurnTable::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	//幸运转盘每日重置
	memset(m_param.ra_lucky_turntable_schedule, 0, sizeof(m_param.ra_lucky_turntable_schedule));
	m_param.ra_lucky_turntable_flag.Reset();
	this->SendRALuckyTurntableInfo();

	this->SetRoleDataChange();
}

void RoleActivityLuckyTurnTable::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	if (NULL == raor)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE);

	switch (raor->opera_type)
	{
	case Protocol::RA_LUCKY_TURNTABLE_REQ_TYPE_INFO:
		{
			this->SendRALuckyTurntableInfo();
		}
		break;
	case Protocol::RA_LUCKY_TURNTABLE_REQ_TYPE_START:
		{
			this->OnRAStartLuckyTurntable();				//启动转盘
		}
		break;
	case Protocol::RA_LUCKY_TURNTABLE_REQ_TYPE_GIVE:
		{
			this->OnRAGetLuckyTurntableReward(raor->param_1);//转动结束客户端回调请求领取奖励 param1 - seq
		}
		break;
	case Protocol::RA_LUCKY_TURNTABLE_REQ_TYPE_SKIP:
		{
			this->OnRAStartLuckyTurntableSkip();			//启动转盘抽取后直接给奖励(勾选了跳过动画)
		}
		break;
	}
}

void RoleActivityLuckyTurnTable::OnRALuckyTurntableFinishType(int type, int num)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE))
	{
		//m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE);

	const RandActivityLuckyTurnTableConfig * config = static_cast<const RandActivityLuckyTurnTableConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const LuckyTurntableParamCfg * param_cfg = config->GetParamCfg(type);
	if (NULL == param_cfg) return;

	int count = 0;
	bool is_add = false;
	switch (type)
	{
	case RA_LUCKY_TURNTABLE_TYPE_SCHOOL_TASK:
		{
			count = m_role_module_mgr->GetSchoolTask()->GetFinishCount();
		}
		break;
	case RA_LUCKY_TURNTABLE_TYPE_ZHEN_ZHONG:			// 2  真中的研究(三界除魔)
		{
			count = m_role_module_mgr->GetResearchTask()->GetTodayFinishCount();
		}
		break;
	case RA_LUCKY_TURNTABLE_TYPE_DEMON_CAVE:			// 3  魔窟（万妖谷)
		{
			count = m_role_module_mgr->GetMonsterWave()->GetPlayTimes();
		}
		break;
	case RA_LUCKY_TURNTABLE_TYPEARRESTING_THIEVES:		// 4  追缉盗贼
		{
			count = m_role_module_mgr->GetTreasureMap()->GetTodayFinish();
		}
		break;
	case RA_LUCKY_TURNTABLE_TYPE_WST_QUESTION:			// 5  万事通的考验 / 神算子的考验
		{
			count = m_role_module_mgr->GetCommonData().wst_answer_num;
		}
		break;
	case RA_LUCKY_TURNTABLE_TYPE_ESCORT:				// 6  运镖
		{
			count = m_role_module_mgr->GetCommonData().escort_today_count;
		}
		break;
	case RA_LUCKY_TURNTABLE_TYPE_BOUNTY_TASK:		// 7  赏金任务
		{
			count = m_role_module_mgr->GetBountyTask()->GetTodayFinish();
		}
		break;
	case RA_LUCKY_TURNTABLE_TYPE_TRANSACTION_TASK:	// 8  购买任务 - 丝绸之路
		{
			count = m_role_module_mgr->GetTransactionTask()->GetTodayFinish();
		}
		break;
	case RA_LUCKY_TURNTABLE_TYPE_JINGJICHANG:		// 9  参与竞技宝库
	case RA_LUCKY_TURNTABLE_TYPE_DREAM_NORES_CHALLENGE:		// 10 通关/扫荡梦渊笔录
	case RA_LUCKY_TURNTABLE_TYPE_REALM_BREAK_SOUL_FIGHT:	// 11 境界-心灵之境通关
	case RA_LUCKY_TURNTABLE_TYPE_CRYSTAL_JING_YING:			// 12 精英试炼通关
		{
			is_add = true;
			count = num;
		}
		break;
	default:
		return;
	}

	if (count <= 0) return;
	int type_seq = type - 1;

	if (m_param.ra_lucky_turntable_flag.IsBitSet(type_seq))	return;

	if (is_add)
	{
		m_param.ra_lucky_turntable_schedule[type_seq] += count;
	}
	else
	{
		m_param.ra_lucky_turntable_schedule[type_seq] = count;
	}
	this->SetRoleDataChange();

	if (param_cfg->param > m_param.ra_lucky_turntable_schedule[type_seq]) 
	{
		this->SendRALuckyTurntableInfo();
		return;
	}
	m_param.ra_lucky_turntable_last_num += param_cfg->frequency;
	m_param.ra_lucky_turntable_flag.SetBit(type_seq);
	this->SendRALuckyTurntableInfo();
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], type[%d] finish_type[%d] ", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), 
		m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE, type);
}

bool RoleActivityLuckyTurnTable::OnUseLuckLotteryTicket(int num)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return false;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_OPEN_ITEM_NOT_USE);
		return false;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE);

	m_param.ra_lucky_turntable_last_num += num;
	this->SendRALuckyTurntableInfo();
	this->SetRoleDataChange();

	return true;
}

void RoleActivityLuckyTurnTable::SendRALuckyTurntableRewardInfo(int type)
{
	Protocol::SCRALuckyTurntableReward info;
	info.index = m_param.ra_lucky_turntable_last_index;
	info.type = type;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityLuckyTurnTable::SendRALuckyTurntableInfo()
{
	Protocol::SCRALuckyTurntableInfo info;
	info.last_num = m_param.ra_lucky_turntable_last_num;
	info.flag = m_param.ra_lucky_turntable_flag;
	memcpy(info.schedule, m_param.ra_lucky_turntable_schedule, sizeof(info.schedule));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityLuckyTurnTable::OnRAStartLuckyTurntable()
{
	if (m_param.ra_lucky_turntable_last_num <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_ENOUGH);
		return;
	}
	if (m_param.ra_lucky_turntable_last_index != -1 && m_param.ra_lucky_turntable_last_index >= 0)
	{
		this->SendRALuckyTurntableRewardInfo();
		this->OnRAGetLuckyTurntableReward(m_param.ra_lucky_turntable_last_index);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityLuckyTurnTableConfig * config = static_cast<const RandActivityLuckyTurnTableConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const LuckyTurntableRewardCfg * reward_cfg = config->GetRandRewardCfg(ramgr);
	if (NULL == reward_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	m_param.ra_lucky_turntable_last_index = reward_cfg->seq;
	this->SendRALuckyTurntableRewardInfo();
	this->SetRoleDataChange();

	this->OnRAGetLuckyTurntableReward(m_param.ra_lucky_turntable_last_index);
}

void RoleActivityLuckyTurnTable::OnRAGetLuckyTurntableReward(int seq)
{
	if (m_param.ra_lucky_turntable_last_num <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_ENOUGH);
		return;
	}
	if (m_param.ra_lucky_turntable_last_index != seq)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const RandActivityLuckyTurnTableConfig * config = static_cast<const RandActivityLuckyTurnTableConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const LuckyTurntableRewardCfg * reward_cfg = config->GetRewardCfgBySeq(seq);
	if (NULL == reward_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(reward_cfg->reward_item.item_id, reward_cfg->reward_item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	if (!m_role_module_mgr->GetKnapsack()->Put(reward_cfg->reward_item, PUT_REASON_LUCKY_TURNTABLE))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	m_param.ra_lucky_turntable_last_num--;
	m_param.ra_lucky_turntable_last_index = -1;

	if (reward_cfg->hearsay != 0 && !CrossConfig::Instance().IsHiddenServer())
	{
		const ItemBase * item_base = ItemPool::Instance()->GetItem(reward_cfg->reward_item.item_id);
		if (NULL != item_base)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_randactivity_lucky_turntable_content,
				m_role_module_mgr->GetRole()->GetName(), item_base->GetItemId(), RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE);
			if (length > 0)
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
				{
					World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
				}
			}
		}
	}
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], type[%d] item[id:%d , num:%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE, reward_cfg->reward_item.item_id, reward_cfg->reward_item.num);
	this->SendRALuckyTurntableInfo();
	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityLuckyTurnTable::OnRAStartLuckyTurntableSkip()
{
	if (m_param.ra_lucky_turntable_last_num <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_ENOUGH);
		return;
	}

	const RandActivityLuckyTurnTableConfig * config = static_cast<const RandActivityLuckyTurnTableConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const LuckyTurntableRewardCfg * reward_cfg = config->GetRandRewardCfg(ramgr);
	if (NULL == reward_cfg) return;

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(reward_cfg->reward_item.item_id, reward_cfg->reward_item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	if (!m_role_module_mgr->GetKnapsack()->Put(reward_cfg->reward_item, PUT_REASON_LUCKY_TURNTABLE))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	m_param.ra_lucky_turntable_last_index = reward_cfg->seq;
	this->SendRALuckyTurntableRewardInfo(Protocol::SCRALuckyTurntableReward::LUCKY_TURN_TABLE_TYPE_SKIP);
	m_param.ra_lucky_turntable_last_num--;
	m_param.ra_lucky_turntable_last_index = -1;

	if (reward_cfg->hearsay != 0 && !CrossConfig::Instance().IsHiddenServer())
	{
		const ItemBase * item_base = ItemPool::Instance()->GetItem(reward_cfg->reward_item.item_id);
		if (NULL != item_base)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_randactivity_lucky_turntable_content,
				m_role_module_mgr->GetRole()->GetName(), item_base->GetItemId(), RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE);
			if (length > 0)
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
				{
					World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
				}
			}
		}
	}
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], type[%d] item[id:%d , num:%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE, reward_cfg->reward_item.item_id, reward_cfg->reward_item.num);
	this->SendRALuckyTurntableInfo();
	this->SetRoleDataChange();
}
