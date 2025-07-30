#include "roleactivitytigerteasenewyear.hpp"
#include "config/randactivityconfig/impl/randactivitytigerteasenewyearconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "global/randactivity/randactivityimpl/randactivitytigerteasenewyear.hpp"

#include "servercommon/errornum.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/uniqueidgenerator.hpp"

RoleActivityTigerTeaseNewYear::RoleActivityTigerTeaseNewYear(int type)
	:RoleActivity(type)
{

}

RoleActivityTigerTeaseNewYear::~RoleActivityTigerTeaseNewYear()
{

}

void RoleActivityTigerTeaseNewYear::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityTigerTeaseNewYear::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityTigerTeaseNewYear::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR))
	{
		return;
	}

	this->RASendInfo();
}

void RoleActivityTigerTeaseNewYear::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR))
	{
		return;
	}

	m_param.DayReset();

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityTigerTeaseNewYear::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR != rand_act_type)
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

void RoleActivityTigerTeaseNewYear::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();					// 发协议
		}
		break;
	case 1:
		{
			this->Mora();						// 猜拳
		}
		break;
	default:
		break;
	}
}

void RoleActivityTigerTeaseNewYear::RASendInfo(int is_win)
{
	Protocol::SCRATigerTeaseNewYearInfo msg;
	msg.times = m_param.times;
	msg.is_win = is_win;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void RoleActivityTigerTeaseNewYear::Mora()
{
	const RandActivityTigerTeaseNewYearConfig * config = static_cast<const RandActivityTigerTeaseNewYearConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR));
	if (NULL == config) return;

	if (m_param.times >= config->GetOtherCfg().day_num)
	{
		return;
	}
	
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	int total_num = 100;
	int rand_rate = RandomNum(total_num);

	bool is_win = rand_rate >= (total_num / 2) ? true : false;		// 胜负

	std::vector<ItemConfigData> reward_list;

	const RATTNYBasisCfg * basis_cfg = config->GetBasisCfg(ramgr);
	if (NULL == basis_cfg) return;

	if (is_win)
	{
		const RATTNYRewardCfg * reward_cfg = config->GetRandRewardCfg(basis_cfg->win_rate_group);
		if (NULL == reward_cfg) return;

		if (!reward_cfg->reward_list.empty())
		{
			reward_list = reward_cfg->reward_list;
		}

		reward_list.insert(reward_list.end(), basis_cfg->win_reward_list.begin(), basis_cfg->win_reward_list.end());
	}
	else
	{
		const RATTNYRewardCfg * reward_cfg = config->GetRandRewardCfg(basis_cfg->fail_rate_group);
		if (NULL == reward_cfg) return;

		if (!reward_cfg->reward_list.empty())
		{
			reward_list = reward_cfg->reward_list;
		}

		reward_list.insert(reward_list.end(), basis_cfg->fail_reward_list.begin(), basis_cfg->fail_reward_list.end());
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(reward_list))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_RA_TIGER_TEASE_NEW_YEAR, false);
		m_role_module_mgr->GetKnapsack()->NotifyGetItemListVec(reward_list, GET_REASON_ACTVITIY_REWARD, RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (int)reward_list.size(), SEND_MAIL_TYPE_TIGER_TEASE_NEW_YEAR);
	}

	m_param.times++;

	std::string gamelog_str;
	for (int i = 0; i < (int)reward_list.size(); ++i)
	{
		gamelog_str = STRING_SPRINTF("{item[%d, %d]} ", reward_list[i].item_id, reward_list[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] get_reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), gamelog_str.c_str());

	this->RASendInfo(is_win ? 1 : 2);
	this->SetRoleDataChange();
}
