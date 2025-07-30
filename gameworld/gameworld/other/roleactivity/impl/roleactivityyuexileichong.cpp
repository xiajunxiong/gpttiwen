#include "roleactivityyuexileichong.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityyuexileichongconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "engineadapter.h"
#include "protocol/randactivity/msgrayuexidengmi.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"

RoleActivityYueXiLeiChong::RoleActivityYueXiLeiChong(int type) :RoleActivity(type)
{
}

RoleActivityYueXiLeiChong::~RoleActivityYueXiLeiChong()
{
}

void RoleActivityYueXiLeiChong::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAYueXiLeiChongParam));
}

void RoleActivityYueXiLeiChong::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAYueXiLeiChongParam));
}

void RoleActivityYueXiLeiChong::OnLoginSendInfo()
{
	this->RASendInfo();
}

void RoleActivityYueXiLeiChong::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
		return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
		return;

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

		this->RASendInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityYueXiLeiChong::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	if (NULL == raor) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
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
	case Protocol::RA_YUE_XI_LEI_CHONG_REQ_TYPE_INFO:
		{
			this->RASendInfo();
		}
		break;
	case Protocol::RA_YUE_XI_LEI_CHONG_REQ_TYPE_RECEIVE_REWARD:
		{
			this->ReceiveReward(raor->param_1);
		}
		break;
	}
}

void RoleActivityYueXiLeiChong::OnAddChongZhi(long long chongzhi)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	m_param.m_total_chongzhi += (int)chongzhi;
	this->RASendInfo();
	this->SetRoleDataChange();
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] add_chongzhi[%lld] m_total_chongzhi[%d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), chongzhi, m_param.m_total_chongzhi);

}

void RoleActivityYueXiLeiChong::OnRAClose()
{
	this->GiveUnFetchReward();
}

void RoleActivityYueXiLeiChong::ReceiveReward(int seq)
{
	if (seq < 0 || seq >= MAX_RA_YUE_XI_LEI_CHONG_BITMAP)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_param.m_receive_reward_flag.IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityYueXiLeiChongConfig * config = static_cast<const RandActivityYueXiLeiChongConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAYueXiLeiChongRewardCfg* reward_cfg = config->GetRewardCfg(ramgr, seq);
	if (NULL == reward_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (reward_cfg->acc_recharge_down > m_param.m_total_chongzhi)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(reward_cfg->reward_list))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_NO_SPACE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->PutVec(reward_cfg->reward_list, PUT_REASON_YUE_XI_LEI_CHONG))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	std::string log_str;
	for (size_t i = 0; i < reward_cfg->reward_list.size(); i++)
	{
		log_str += STRING_SPRINTF("[%d, %d]", reward_cfg->reward_list[i].item_id, reward_cfg->reward_list[i].num);
	}

	m_role_module_mgr->GetMoney()->AddGold(reward_cfg->gold_reward, __FUNCTION__);

	m_param.m_receive_reward_flag.SetBit(seq);
	this->RASendInfo();
	this->SetRoleDataChange();
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] seq[%d] cur_chongzhi[%d] reward_list{%s}", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), seq, m_param.m_total_chongzhi, log_str.c_str());
}

void RoleActivityYueXiLeiChong::GiveUnFetchReward()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityYueXiLeiChongConfig * config = static_cast<const RandActivityYueXiLeiChongConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	int total_gold = 0;
	std::vector<ItemConfigData> all_rewards;
	for (int i = 0; i < MAX_RA_YUE_XI_LEI_CHONG_BITMAP; i++)
	{
		if (m_param.m_receive_reward_flag.IsBitSet(i)) continue;
		
		const RAYueXiLeiChongRewardCfg* reward_cfg = config->GetRewardCfg(ramgr, i);
		if (NULL == reward_cfg) continue;

		if(m_param.m_total_chongzhi < reward_cfg->acc_recharge_down) continue;

		total_gold += reward_cfg->gold_reward;
		for (size_t j = 0; j < reward_cfg->reward_list.size(); j++)
		{
			Knapsack::AddRewards(all_rewards, reward_cfg->reward_list[j]);
		}

		m_param.m_receive_reward_flag.SetBit(i);
	}
	if (all_rewards.empty()) return;

	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		contentparam.reserver_gold = total_gold;
		FillMailContentParam(contentparam, all_rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject_1, m_role_activity_type);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_end_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	this->RASendInfo();
	this->SetRoleDataChange();

	std::string log_str;
	for (size_t i = 0; i < all_rewards.size(); i++)
	{
		log_str += STRING_SPRINTF("[%d, %d]", all_rewards[i].item_id, all_rewards[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] reward_list{%s}", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), log_str.c_str());
}

void RoleActivityYueXiLeiChong::RASendInfo()
{
	Protocol::SCRAYueXiLeiChongInfo msg;
	msg.m_total_chongzhi = m_param.m_total_chongzhi;
	msg.m_receive_reward_flag = m_param.m_receive_reward_flag;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}
