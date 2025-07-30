#include "roleactivityyuanqifanpai.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityyuanqifanpaiconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/string/gameworldstr.h"

RoleActivityYuanQiFanPai::RoleActivityYuanQiFanPai(int type)
	:RoleActivity(type)
{
}

RoleActivityYuanQiFanPai::~RoleActivityYuanQiFanPai()
{
}

void RoleActivityYuanQiFanPai::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAYuanQiFanPaiParam));
}

void RoleActivityYuanQiFanPai::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAYuanQiFanPaiParam));
}

void RoleActivityYuanQiFanPai::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.is_fetch_day_reward = 0;
	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityYuanQiFanPai::OnLoginSendInfo()
{
	this->RASendInfo();
}

void RoleActivityYuanQiFanPai::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivityYuanQiFanPai::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(raor->rand_activity_type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}

	this->CheckInitRandActivity(raor->rand_activity_type);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:		// 翻牌
		{
			this->Lottery();
		}
		break;
	case 2:		// 领取翻牌次数奖励, p1-seq
		{
			this->ReceiveTimes(raor->param_1);
		}
		break;
	case 3:		// 兑换, p1-seq
		{
			this->Exchange(raor->param_1);
		}
		break;
	case 4:		// 领取每日奖励, p1-seq
		{
			this->FetchDayReward();
		}
		break;
	}
}

void RoleActivityYuanQiFanPai::Lottery()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityYuanQiFanPaiConfig * config = static_cast<const RandActivityYuanQiFanPaiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	if (!m_role_module_mgr->GetKnapsack()->HasItem(config->GetOtherCfg().consunme_item.item_id, config->GetOtherCfg().consunme_item.num, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	const YQFPBaseCfg* base_cfg = config->GetRandRewardCfg(ramgr);
	if (NULL == base_cfg) return;

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(config->GetOtherCfg().consunme_item.item_id, config->GetOtherCfg().consunme_item.num, __FUNCTION__))
	{
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPut2(base_cfg->reward, true, false))
	{
		m_role_module_mgr->GetKnapsack()->Put(base_cfg->reward, PUT_REASON_RA_YUAN_QI_FAN_PAI_REWARD);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&base_cfg->reward, 1, SEND_MAIL_TYPE_YUAN_QI_FAN_PAI);
	}
	m_role_module_mgr->GetKnapsack()->NotifyGetItemList(1, &base_cfg->reward, GET_REASON_YUAN_QI_FAN_PAI);

	if (base_cfg->is_hearsay == 1)
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_ra_yuan_qi_fan_pai_notice,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRoleName(), base_cfg->reward.item_id);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}

	m_param.lottery_count += 1;
	
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] consunme_item[%d, %d] reward[%d, %d] ", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), config->GetOtherCfg().consunme_item.item_id, config->GetOtherCfg().consunme_item.num, base_cfg->reward.item_id, base_cfg->reward.num);

	this->RASendInfo();
}

void RoleActivityYuanQiFanPai::ReceiveTimes(int seq)
{
	if (seq < 0) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityYuanQiFanPaiConfig * config = static_cast<const RandActivityYuanQiFanPaiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	const YQFPTimesRewardCfg* times_cfg = config->GetTimesRewardCfg(ramgr, seq);
	if (NULL == times_cfg) return;

	if (m_param.lottery_count < times_cfg->task_limit)
	{
		return;
	}

	if (m_param.is_receive_times.IsBitSet(seq))
	{
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPut2(times_cfg->reward, true, false))
	{
		m_role_module_mgr->GetKnapsack()->Put(times_cfg->reward, PUT_REASON_RA_YUAN_QI_FAN_PAI);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&times_cfg->reward, 1, SEND_MAIL_TYPE_YUAN_QI_FAN_PAI);
	}

	m_param.is_receive_times.SetBit(seq);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] seq[%d] reward[%d, %d] ", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), seq, times_cfg->reward.item_id,times_cfg->reward.num);

	this->RASendInfo();
}

void RoleActivityYuanQiFanPai::Exchange(int seq)
{
	if (seq < 0) return;

	const RandActivityYuanQiFanPaiConfig * config = static_cast<const RandActivityYuanQiFanPaiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	const YQFPGatherCfg* gather_cfg = config->GetGatherCfg(seq);
	if (NULL == gather_cfg) return;

	for (size_t i = 0; i < gather_cfg->consunme_item.size(); i++)
	{
		if (!m_role_module_mgr->GetKnapsack()->HasItem(gather_cfg->consunme_item[i].item_id, gather_cfg->consunme_item[i].num, false))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return;
		}
	}

	for (size_t i = 0; i < gather_cfg->consunme_item.size(); i++)
	{
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(gather_cfg->consunme_item[i].item_id, gather_cfg->consunme_item[i].num, __FUNCTION__))
		{
			gamelog::g_log_rand_activity.printf(LL_INFO, "%s consunme_item[%d, %d] failed!!!!", __FUNCTION__, gather_cfg->consunme_item[i].item_id, gather_cfg->consunme_item[i].num);
			return;
		}
	}

	std::string log_str;
	for (size_t i = 0; i < gather_cfg->consunme_item.size(); i++)
	{
		log_str += STRING_SPRINTF(" [%d, %d]", gather_cfg->consunme_item[i].item_id, gather_cfg->consunme_item[i].num);
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPut2(gather_cfg->reward, true, false))
	{
		m_role_module_mgr->GetKnapsack()->Put(gather_cfg->reward, PUT_REASON_RA_YUAN_QI_FAN_PAI);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&gather_cfg->reward, 1, SEND_MAIL_TYPE_YUAN_QI_FAN_PAI);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] consunme_item[%s] reward[%d, %d] ", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), log_str.c_str(), gather_cfg->reward.item_id, gather_cfg->reward.num);

	this->RASendInfo();
}

void RoleActivityYuanQiFanPai::FetchDayReward()
{
	if (m_param.is_fetch_day_reward)
	{
		return;
	}

	const RandActivityYuanQiFanPaiConfig * config = static_cast<const RandActivityYuanQiFanPaiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const YQFPOtherCfg & other_cfg = config->GetOtherCfg();
	if (!m_role_module_mgr->GetKnapsack()->Put(other_cfg.day_fetch_reward, PUT_REASON_RA_YUAN_QI_FAN_PAI_DAY_FREE_REWARD))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
		return;
	}

	m_param.is_fetch_day_reward = 1;
	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityYuanQiFanPai::RASendInfo()
{
	Protocol::SCRAYuanQiFanPaiInfo msg;
	msg.lottery_count = m_param.lottery_count;
	msg.is_receive_times = m_param.is_receive_times;
	msg.is_fetch_day_reward = m_param.is_fetch_day_reward;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}
