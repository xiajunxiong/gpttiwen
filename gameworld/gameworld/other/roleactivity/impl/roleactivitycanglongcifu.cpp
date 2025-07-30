#include "roleactivitycanglongcifu.hpp"
#include "config/randactivityconfig/impl/randactivitycanglongcifuconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "protocol/randactivity/msgraduanyangxiangnang.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"

RoleActivityCangLongCiFu::RoleActivityCangLongCiFu(int type) :RoleActivity(type)
{
}

RoleActivityCangLongCiFu::~RoleActivityCangLongCiFu()
{
}

void RoleActivityCangLongCiFu::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RACangLongCiFuParam));
}

void RoleActivityCangLongCiFu::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RACangLongCiFuParam));
}

void RoleActivityCangLongCiFu::CheckInitRandActivity(int rand_act_type, bool is_force)
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

		this->RASendInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityCangLongCiFu::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(m_role_activity_type);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:		// 点亮 p1--seq	
		{
			this->LightUp(raor->param_1);
		}
		break;
	case 2:		// 领取全部点亮奖励
		{
			this->ReceiveAllLinghtUp();
		}
		break;
	default:
		break;
	}
}

void RoleActivityCangLongCiFu::OnLoginSendInfo()
{
	this->RASendInfo();
}

void RoleActivityCangLongCiFu::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	this->ReissueReward();
}

void RoleActivityCangLongCiFu::LightUp(int seq)
{
	if (seq < 0 || seq >= CLCF_BITMAP_MAX_VALUE) return;
	if (m_param.light_up_flag.IsBitSet(seq) || m_param.all_light_reward_flag != 0) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityCangLongCiFuConfig * config = static_cast<const RandActivityCangLongCiFuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	int fragments_num = 0;

	const std::pair<ItemConfigData, int>* basic_cfg = config->GetBasicBySeqCfg(ramgr, seq, fragments_num);
	if (NULL == basic_cfg || fragments_num == 0) return;

	if (!m_role_module_mgr->GetKnapsack()->HasItem(basic_cfg->first.item_id, basic_cfg->first.num, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	const std::vector<ItemConfigData>* reward_list = config->GetRewardCfg(basic_cfg->second);
	if (NULL == reward_list) return;

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(basic_cfg->first.item_id, basic_cfg->first.num, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(*reward_list))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(*reward_list, PUT_REASON_RA_CANG_LONG_CI_FU);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(*reward_list, SEND_MAIL_TYPE_CANG_LONG_CI_FU);
	}

	m_param.light_up_flag.SetBit(seq);

	bool is_all_light_up = true;
	for (int i = 0; i < fragments_num; i++)
	{
		is_all_light_up = is_all_light_up && m_param.light_up_flag.IsBitSet(i);
	}

	if (is_all_light_up)
	{
		m_param.all_light_reward_flag = 1;
	}

	{
		std::string log_str;
		for (int i = 0; i < (int)reward_list->size(); i++)
		{
			log_str += STRING_SPRINTF("[%d, %d] ", (*reward_list)[i].item_id, (*reward_list)[i].num);
		}

		gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] light_up_seq[%d] all_linght_flag[%d] reward[%s]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), seq, m_param.all_light_reward_flag, log_str.c_str());
	}

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityCangLongCiFu::ReceiveAllLinghtUp()
{
	if (m_param.all_light_reward_flag != 1) return;

	const RandActivityCangLongCiFuConfig * config = static_cast<const RandActivityCangLongCiFuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPut2(config->GetOtherCfg().last_reward, false, false))
	{
		m_role_module_mgr->GetKnapsack()->Put(config->GetOtherCfg().last_reward, PUT_REASON_RA_CANG_LONG_CI_FU);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&config->GetOtherCfg().last_reward, 1, SEND_MAIL_TYPE_CANG_LONG_CI_FU);
	}

	m_param.all_light_reward_flag = 2;

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] all_linght_flag[%d] reward[%d, %d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.all_light_reward_flag, config->GetOtherCfg().last_reward.item_id, config->GetOtherCfg().last_reward.num);

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityCangLongCiFu::ReissueReward()
{
	if (m_param.all_light_reward_flag != 1) return;

	const RandActivityCangLongCiFuConfig * config = static_cast<const RandActivityCangLongCiFuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	m_role_module_mgr->GetKnapsack()->SendMail(&config->GetOtherCfg().last_reward, 1, SEND_MAIL_TYPE_CANG_LONG_CI_FU);
	m_param.all_light_reward_flag = 2;

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] all_linght_flag[%d] reward[%d, %d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.all_light_reward_flag, config->GetOtherCfg().last_reward.item_id, config->GetOtherCfg().last_reward.num);

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityCangLongCiFu::RASendInfo()
{
	Protocol::SCRACangLongCiFuInfo msg;
	msg.all_light_reward_flag = m_param.all_light_reward_flag;
	msg.light_up_flag = m_param.light_up_flag;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}
