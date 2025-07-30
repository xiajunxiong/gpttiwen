#include "roleactivitygoodgiftturn.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "protocol/randactivity/msggoodgiftturn.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitygoodgiftturnconfig.hpp"

RoleActivityGoodGiftTurn::RoleActivityGoodGiftTurn(int type)
	:RoleActivity(type)
{

}

RoleActivityGoodGiftTurn::~RoleActivityGoodGiftTurn()
{
}

void RoleActivityGoodGiftTurn::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAGoodGiftTurnParam));
}

void RoleActivityGoodGiftTurn::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAGoodGiftTurnParam));
}

void RoleActivityGoodGiftTurn::Update(time_t now_second)
{
}

void RoleActivityGoodGiftTurn::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if(old_dayid == now_dayid) return;
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN))
	{
		return;
	}

	const RandActivityGoodGiftTurnConfig * config = static_cast<const RandActivityGoodGiftTurnConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN));
	if (NULL == config) return;

	int act_open_day = config->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN);
	int last_add_fetch_num_day = m_param.last_add_fetch_num_day;
	if (m_param.last_add_fetch_num_day == act_open_day)
	{
		return;
	}
	
	int last_num = 0;
	for (int i = 0; i < config->GetOtherCfg().reward_frequency_total; i++)
	{
		if (!(m_param.fetch_flag & (1 << i)))
		{
			last_num++;
		}
	}
	m_param.fetch_num += config->GetOtherCfg().reward_frequency;
	if (m_param.fetch_num > last_num)
	{
		m_param.fetch_num = last_num;
	}

	m_param.last_add_fetch_num_day = act_open_day;
	this->SetRoleDataChange();
	this->SendInfo();
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s open_day[old:%d now:%d] fetch_num[%d] last_num[%d] activity_type[%d]", __FUNCTION__, last_add_fetch_num_day, act_open_day, m_param.fetch_num, last_num,
		RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN);
}

void RoleActivityGoodGiftTurn::OnLoginSendInfo()
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN))
	{
		return;
	}
	this->SendInfo();
}

void RoleActivityGoodGiftTurn::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN != rand_act_type)
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
	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_time);
	if (is_force || !in_activity_time)
	{
		const RandActivityGoodGiftTurnConfig * config = static_cast<const RandActivityGoodGiftTurnConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN));
		if(NULL == config) return;

		m_param.ActReset();
		m_param.ra_begin_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.fetch_num = config->GetOtherCfg().reward_frequency;
		m_param.last_add_fetch_num_day = config->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN);
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s last_add_fetch_num_day[%d] fetch_num[%d] activity_type[%d]", __FUNCTION__, m_param.last_add_fetch_num_day, m_param.fetch_num,
			RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN);
		this->SetRoleDataChange();
	}
}

void RoleActivityGoodGiftTurn::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN);

	switch (raor->opera_type)
	{
		case Protocol::RA_GOOD_GIFT_TURN_REQ_TYPE_INFO:				//下发信息
		{
			this->SendInfo();
		}
		break;
		case Protocol::RA_GOOD_GIFT_TURN_REQ_TYPE_FETCH:			//启动转盘并给予奖励
		{
			this->OnStarTurnTable();
		}
		break;
	default:
		break;
	}

}

void RoleActivityGoodGiftTurn::GmAddTurnTableNum(int num)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN))
	{
		return;
	}

	const RandActivityGoodGiftTurnConfig * config = static_cast<const RandActivityGoodGiftTurnConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN));
	if (NULL == config) return;

	int last_num = 0;
	for (int i = 0; i < config->GetOtherCfg().reward_frequency_total; i++)
	{
		if (!(m_param.fetch_flag & (1 << i)))
		{
			last_num++;
		}
	}
	m_param.fetch_num += num;
	if (m_param.fetch_num > last_num)
	{
		m_param.fetch_num = last_num;
	}
	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityGoodGiftTurn::SendInfo()
{
	Protocol::SCRAGoodGiftTurnInfo info;
	info.fetch_num = m_param.fetch_num;
	info.fetch_flag = m_param.fetch_flag;
	
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityGoodGiftTurn::SendRewardInfo(int index)
{
	Protocol::SCRAGoodGiftTurnRewardInfo info;
	info.index = index;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityGoodGiftTurn::OnStarTurnTable()
{
	if (m_param.fetch_num <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_ENOUGH);
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityGoodGiftTurnConfig * config = static_cast<const RandActivityGoodGiftTurnConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	const RAGoodGiftTurnCfg * reward_cfg = config->GetRandReward(ramgr, m_param.fetch_flag);
	if (NULL == reward_cfg  || m_param.fetch_flag & (1 << reward_cfg->seq1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	this->SendRewardInfo(reward_cfg->seq1);
	if (!reward_cfg->item_list.empty())
	{
		bool is_put_succ = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_cfg->item_list.size(), &reward_cfg->item_list[0]))	//不放临时背包
		{
			if (m_role_module_mgr->GetKnapsack()->PutList((short)reward_cfg->item_list.size(), &reward_cfg->item_list[0], PUT_REASON_GOOD_GIFT_TURN, false))
			{
				is_put_succ = true;
			}
		}
		if(!is_put_succ)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->item_list[0], (int)reward_cfg->item_list.size(), SEND_MAIL_TYPE_GOOD_GIFT_TURN);
		}
	}

	m_param.fetch_num--;
	m_param.fetch_flag |= (1 << reward_cfg->seq1);

	this->SetRoleDataChange();
	this->SendInfo();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], seq1[%d] reward_item_size[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN, reward_cfg->seq1, reward_cfg->item_list.empty() ? 0 : (int)reward_cfg->item_list.size());
}
