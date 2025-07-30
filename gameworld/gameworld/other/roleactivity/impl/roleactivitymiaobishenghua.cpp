#include "roleactivitymiaobishenghua.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitymiaobishenghuaconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityyuanqijishibu.hpp"

RoleActivityMiaoBiShengHua::RoleActivityMiaoBiShengHua(int type)
	:RoleActivity(type)
{
}

RoleActivityMiaoBiShengHua::~RoleActivityMiaoBiShengHua()
{
}

void RoleActivityMiaoBiShengHua::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAMiaoBiShengHuaParam));
}

void RoleActivityMiaoBiShengHua::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAMiaoBiShengHuaParam));
}

void RoleActivityMiaoBiShengHua::OnLogin()
{
	this->RASendInfo();
}

void RoleActivityMiaoBiShengHua::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.count = 0;
	this->RASendInfo();
}

void RoleActivityMiaoBiShengHua::CheckInitRandActivity(int rand_act_type, bool is_force)
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

void RoleActivityMiaoBiShengHua::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
	case 1:			// 领取奖励
		{
			this->ReceiveReward();
		}
		break;
	}
}

void RoleActivityMiaoBiShengHua::ReceiveReward()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityMiaoBiShengHuaConfig * config = static_cast<const RandActivityMiaoBiShengHuaConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	const std::vector<ItemConfigData>* reward_list = config->GetRewardCfg(ramgr);
	if (NULL == reward_list) return;

	if (!m_role_module_mgr->GetKnapsack()->HasItem(config->GetOtherCfg().consunme_item, config->GetOtherCfg().consunme_num))
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(config->GetOtherCfg().consunme_item, config->GetOtherCfg().consunme_num,__FUNCTION__))
	{
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(*reward_list))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(*reward_list, PUT_REASON_RA_MIAO_BI_SHENG_HUA);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(*reward_list, SEND_MAIL_TYPE_MIAO_BI_SHENG_HUA);
	}

	m_param.count += 1;

	std::string log_str;
	for (size_t i = 0; i < reward_list->size(); i++)
	{
		log_str += STRING_SPRINTF("[%d, %d] ", (*reward_list)[i].item_id, (*reward_list)[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] reward_list[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), log_str.c_str());

	this->OnJoinActivity();
	this->RASendInfo();
}

void RoleActivityMiaoBiShengHua::OnJoinActivity()
{
	{
		RoleActivityYuanQiJiShiBu * role_activity = static_cast<RoleActivityYuanQiJiShiBu *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU));
		if (NULL != role_activity)
		{
			role_activity->OnCompleteTask(RA_YUAN_QI_JISHIBU_TASK_TYPE_MIAOBI_SHENGHUA);
		}
	}
}

void RoleActivityMiaoBiShengHua::RASendInfo()
{
	Protocol::SCRAMiaoBiShengHuaInfo msg;
	msg.count = m_param.count;
	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}
