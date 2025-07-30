#include "roleactivityshanhaijubao.hpp"
#include "config/randactivityconfig/impl/rashanhaijubaoconfig.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "obj/character/role.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/route/mailroute.hpp"
#include "other/rolemodulemanager.hpp"
#include "servercommon/string/gameworldstr.h"

#include "gamelog.h"

RoleActivityShanHaiJuBao::RoleActivityShanHaiJuBao(int type)
	: RoleActivity(type)
{
}

RoleActivityShanHaiJuBao::~RoleActivityShanHaiJuBao()
{
}

void RoleActivityShanHaiJuBao::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAShanHaiJuBaoParam));
}

void RoleActivityShanHaiJuBao::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAShanHaiJuBaoParam));
}

void RoleActivityShanHaiJuBao::OnLoginSendInfo()
{
	this->SendRoleInfo();
}

void RoleActivityShanHaiJuBao::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

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

void RoleActivityShanHaiJuBao::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	//if (CrossConfig::Instance().IsHiddenServer())
	//{//跨服禁用
	//	return;
	//}
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	this->CheckInitRandActivity(m_role_activity_type);

	switch (raor->opera_type)
	{
	case Protocol::RA_SHAN_HAI_JU_BAO_REQ_TYPE_DRAW:
		{
			this->Draw(raor->param_1);
		}
		break;
	case Protocol::RA_SHAN_HAI_JU_BAO_REQ_TYPE_FETCH_EXTRA:
		{
			this->FetchExtraReward(raor->param_1, raor->param_2);
		}
		break;
	default:
		return;
	}
}

void RoleActivityShanHaiJuBao::Draw(int phase_seq)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	const RAShanHaiJuBaoConfig * config = static_cast<const RAShanHaiJuBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}
	const RAShanHaiJuBao::DrawItemCfg *reward_cfg = config->GetRandDrawCfg(ramgr, phase_seq);
	if (NULL == reward_cfg)
	{
		return;
	}

	//消耗
	const RAShanHaiJuBao::OtherCfg & other_cfg = config->GetOtherCfg();
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.draw_cost_item_id, other_cfg.cost_num, __FUNCTION__, KNAPSACK_CONSUME_TYPE_CANNOT_BUY, true))
	{
		return;
	}
	//记录
	m_param.total_draw_times++;
	m_param.phase_info_[phase_seq - 1].draw_times++;
	this->SetRoleDataChange();

	//奖励
	if (m_role_module_mgr->GetKnapsack()->CheckForPut2(reward_cfg->draw_item, false, false))
	{
		m_role_module_mgr->GetKnapsack()->Put(reward_cfg->draw_item, PUT_REASON_SHAN_HAI_JU_BAO_DRAW);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, reward_cfg->draw_item);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject),
			gamestring::g_act_full_bag_title,
			m_role_activity_type);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt),
			gamestring::g_act_full_bag_content,
			m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	{// 通知
		static Protocol::SCRAShanHaiJuBaoDrawRet info;
		info.show_type = reward_cfg->show_type;

		m_role_module_mgr->NetSend(&info, sizeof(info));
	}

	// 传闻
	if (0 != reward_cfg->is_brocast)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, 
			sizeof(gamestring::GAMESTRING_BUF), 
			gamestring::g_ra_reward_borcast,
			m_role_module_mgr->GetRole()->GetUID(), 
			m_role_module_mgr->GetRole()->GetName(),
			m_role_activity_type,
			reward_cfg->draw_item.item_id,
			m_role_activity_type);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}


	this->SendRoleInfo();
	this->OnParticipateRA(__FUNCTION__);

	//日志
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] phase_seq:%d total_draw_times:%d phase_draw_times:%d.",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		phase_seq, m_param.total_draw_times, m_param.phase_info_[phase_seq - 1].draw_times);
}

void RoleActivityShanHaiJuBao::FetchExtraReward(int phase_seq, int extra_reward_index)
{
	const RAShanHaiJuBaoConfig * config = static_cast<const RAShanHaiJuBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RAShanHaiJuBao::ExtraRewardCfg *cfg_info = config->GetExtraRewardCfg(phase_seq, extra_reward_index);
	if (NULL == cfg_info)
	{
		return;
	}
	if (cfg_info->reward_item_vec.empty())
	{
		return;
	}
	int phase_index = phase_seq - 1;
	//已领
	UNSTD_STATIC_CHECK(32 >= SHAN_HAI_JU_BAO_EXTRA_REWARD_MAX);
	if (IsSetBit(m_param.phase_info_[phase_index].extra_reward_flag, extra_reward_index))
	{
		return;
	}

	//可领
	if (cfg_info->need_draw_num > m_param.phase_info_[phase_index].draw_times)
	{
		return;
	}

	//记录
	SetBit(m_param.phase_info_[phase_index].extra_reward_flag, extra_reward_index);
	this->SetRoleDataChange();

	//奖励
	if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(cfg_info->reward_item_vec, false))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(cfg_info->reward_item_vec, PUT_REASON_SHAN_HAI_JU_BAO_EXTRA_REWARD);
	}
	else
	{
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, cfg_info->reward_item_vec);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject),
			gamestring::g_act_full_bag_title,
			m_role_activity_type);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt),
			gamestring::g_act_full_bag_content,
			m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	this->SendRoleInfo();
	this->OnParticipateRA(__FUNCTION__);

	//log
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] extra_reward_index:%d phase_extra_reward_flag:%d.",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		extra_reward_index, m_param.phase_info_[phase_index].extra_reward_flag);
}

void RoleActivityShanHaiJuBao::SendRoleInfo()
{
	Protocol::SCRAShanHaiJuBaoInfo info;

	for (int i = 0; i < SHAN_HAI_JU_BAO_EXTRA_PHASE_MAX; i++)
	{
		info.phase_info_[i].draw_times = m_param.phase_info_[i].draw_times;
		info.phase_info_[i].extra_reward_flag = m_param.phase_info_[i].extra_reward_flag;
	}

	m_role_module_mgr->NetSend(&info, sizeof(info));
}
