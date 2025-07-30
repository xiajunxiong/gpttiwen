#include "roleactivitybozhongyouli.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitybozhongyouliconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityyuanqijishibu.hpp"

RoleActivityBoZhongYouLi::RoleActivityBoZhongYouLi(int type)
	:RoleActivity(type)
{
}

RoleActivityBoZhongYouLi::~RoleActivityBoZhongYouLi()
{
}
void RoleActivityBoZhongYouLi::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RABoZhongYouLiParam));
}

void RoleActivityBoZhongYouLi::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RABoZhongYouLiParam));
}

void RoleActivityBoZhongYouLi::OnLogin()
{
	this->RASendInfo();
}

void RoleActivityBoZhongYouLi::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	const RandActivityBoZhongYouLiConfig * config = static_cast<const RandActivityBoZhongYouLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_BO_ZHONG_YOU_LI))
	{
		return;
	}

	if (config->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_BO_ZHONG_YOU_LI) == 1)
	{
		return;
	}

	m_param.sowing_count += config->GetOtherCfg().sowing_limit;

	this->RASendInfo();
}

void RoleActivityBoZhongYouLi::CheckInitRandActivity(int rand_act_type, bool is_force)
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

		const RandActivityBoZhongYouLiConfig * config = static_cast<const RandActivityBoZhongYouLiConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
		if (NULL == config) return;

		m_param.sowing_count += config->GetOtherCfg().sowing_limit;

		this->SetRoleDataChange();
	}

}

void RoleActivityBoZhongYouLi::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
	case 1:		// 播种
		{
			this->Sowing();
		}
		break;
	case 2:		// 灌溉
		{
			this->Watering();	
		}
		break;
	case 3:		// 领取成熟奖励 p1-seq
		{
			this->ReceiveReward(raor->param_1);
		}
		break;
	}
}

void RoleActivityBoZhongYouLi::Sowing()
{	
	const RandActivityBoZhongYouLiConfig * config = static_cast<const RandActivityBoZhongYouLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	if (m_param.sowing_count  <= 0)		// 播种次数不足
	{
		return;
	}

	// 标记种植时间戳
	bool is_planting = false;
	for (int i = 0; i < BO_ZHONG_YOU_LI_MAX_PLANT_NUM; ++i)
	{
		if(m_param.planting_timestamp[i] != 0) continue;

		m_param.planting_timestamp[i] = EngineAdapter::Instance().Time();
		is_planting = true;
		break;
	}
	if (!is_planting)	// 种满了
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BO_ZHONG_YOU_LI_PLANT_LIMIT);
		return;
	}
	
	m_param.sowing_count -= 1;

	this->RASendInfo();
}

void RoleActivityBoZhongYouLi::Watering()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityBoZhongYouLiConfig * config = static_cast<const RandActivityBoZhongYouLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	if (EngineAdapter::Instance().Time() < m_param.next_refresh_timestamp)
	{
		return;
	}

	bool is_can_watering = false;
	for (int i = 0; i < BO_ZHONG_YOU_LI_MAX_PLANT_NUM; i++)
	{
		if (EngineAdapter::Instance().Time() < (m_param.planting_timestamp[i] + config->GetOtherCfg().mature_time))
		{
			is_can_watering = true;
			break;
		}
	}
	if (!is_can_watering)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BO_ZHONG_YOU_LI_NO_PLANTS);
		return;
	}

	const ItemConfigData * reward_cfg = config->GetRandRewardCfg(ramgr);
	if (NULL == reward_cfg) return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPut2(*reward_cfg, true, false))
	{
		m_role_module_mgr->GetKnapsack()->Put(*reward_cfg, PUT_REASON_RA_BO_ZHONG_YOU_LI);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(reward_cfg, 1, SEND_MAIL_TYPE_BO_ZHONG_YOU_LI);
	}

	m_param.next_refresh_timestamp = EngineAdapter::Instance().Time() + (config->GetOtherCfg().watering_limit);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] reward[%d, %d] next_timestamp[%d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), reward_cfg->item_id, reward_cfg->num, m_param.next_refresh_timestamp);

	this->RASendInfo();
}

void RoleActivityBoZhongYouLi::ReceiveReward(int seq)
{
	if (seq < 0 || seq >= BO_ZHONG_YOU_LI_MAX_PLANT_NUM) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityBoZhongYouLiConfig * config = static_cast<const RandActivityBoZhongYouLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	if (m_param.planting_timestamp[seq] == 0)
	{
		return;
	}

	if (EngineAdapter::Instance().Time() < (m_param.planting_timestamp[seq] + (config->GetOtherCfg().mature_time)))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BO_ZHONG_YOU_LI_PLANTS_IMMATURE);
		return;
	}

	const std::vector<ItemConfigData>* reward_list = config->GetBaseRewardCfg(ramgr);
	if (NULL == reward_list) return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(*reward_list, false))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(*reward_list, PUT_REASON_RA_BO_ZHONG_YOU_LI);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&(*reward_list)[0], (int)reward_list->size(), SEND_MAIL_TYPE_BO_ZHONG_YOU_LI);
	}

	std::string log_str;
	for (std::vector<ItemConfigData>::const_iterator it = reward_list->begin(); it != reward_list->end(); it++)
	{	
		log_str += STRING_SPRINTF(" [%d, %d]", it->item_id, it->num);
	}

	m_param.planting_timestamp[seq] = 0;

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] plant_seq[%d] reward[%s] ", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), seq, log_str.c_str());

	this->RASendInfo();

	this->OnJoinActivity();
}

void RoleActivityBoZhongYouLi::RASendInfo()
{
	Protocol::SCRABoZhongYouLiInfo msg;

	msg.next_refresh_timestamp = m_param.next_refresh_timestamp;
	msg.sowing_count = m_param.sowing_count;
	memcpy(msg.planting_timestamp, m_param.planting_timestamp, sizeof(msg.planting_timestamp));

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void RoleActivityBoZhongYouLi::OnJoinActivity()
{
	{
		RoleActivityYuanQiJiShiBu * role_activity = static_cast<RoleActivityYuanQiJiShiBu *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU));
		if (NULL != role_activity)
		{
			role_activity->OnCompleteTask(RA_YUAN_QI_JISHIBU_TASK_TYPE_BOZHONG_YOULI);
		}
	}
}
