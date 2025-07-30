#include "roleactivityqiyuankongmingdeng.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityqiyuankongmingdengconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"

#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityyuanqijishibu.hpp"

RoleActivityQiYuanKongMingDeng::RoleActivityQiYuanKongMingDeng(int type) :RoleActivity(type)
{
}

RoleActivityQiYuanKongMingDeng::~RoleActivityQiYuanKongMingDeng()
{
}

void RoleActivityQiYuanKongMingDeng::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAQiYuanKongMingDengParam));
}

void RoleActivityQiYuanKongMingDeng::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAQiYuanKongMingDengParam));
}

void RoleActivityQiYuanKongMingDeng::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
		return;

	this->RASendInfo();
}

void RoleActivityQiYuanKongMingDeng::CheckInitRandActivity(int rand_act_type, bool is_force)
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

		if (m_param.now_day_id != EngineAdapter::Instance().DayID())
		{
			m_param.now_day_id = EngineAdapter::Instance().DayID();
			this->OnCompleteTask(RA_QIYUAN_KONGMINGDENG_TASK_TYPE_LOGIN);
		}

		this->SetRoleDataChange();
	}
}

void RoleActivityQiYuanKongMingDeng::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
	case 0: // 请求信息
		{
			this->RASendInfo();
		}
		break;
	case 1:	// 点亮火种
		{
			this->RADianDeng();
		}
		break;
	}
}

void RoleActivityQiYuanKongMingDeng::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
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

	m_param.complete_flag = 0;
	memset(m_param.task_list, 0, sizeof(m_param.task_list));

	if (m_param.now_day_id != now_dayid)
	{
		m_param.now_day_id = now_dayid;
		this->OnCompleteTask(RA_QIYUAN_KONGMINGDENG_TASK_TYPE_LOGIN);
	}
	else
	{	// 天数ID相等的时候说明今天已经完成登录任务了，但是上面已经重置过，所以把这个任务在加回来，但是不给奖励
		this->OnCompleteTask(RA_QIYUAN_KONGMINGDENG_TASK_TYPE_LOGIN, 1, false);
	}

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityQiYuanKongMingDeng::OnCompleteTask(int task_type, int add_param, bool is_reward)
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

	const RandActivityQiYuanKongMingDengConfig * config = static_cast<const RandActivityQiYuanKongMingDengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
		return;

	const std::vector<RAQiYuanKongMingDengTaskCfg> * task_map = config->GetTaskListCfg(ramgr);
	if (NULL == task_map)
		return;

	bool is_send = false;
	for (unsigned int i = 0; i < task_map->size(); i++)
	{
		if(task_type != task_map->at(i).task_type)
			continue;

		if (IsSetBit(m_param.complete_flag, i))
			continue;

		m_param.task_list[i] += add_param;
		if (m_param.task_list[i] >= task_map->at(i).param)
		{
			m_param.task_list[i] = task_map->at(i).param;
			SetBit(m_param.complete_flag, i);

			if (is_reward)
			{
				m_param.huozhong += config->GetOtherCfg()->reward_increase;

				//log
				gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] task_index[%d] task_param[%d] huozhong[%d]", __FUNCTION__,
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
					i, m_param.task_list[i], m_param.huozhong);
			}
		}

		is_send = true;
	}

	if (is_send)
	{
		this->RASendInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityQiYuanKongMingDeng::RASendInfo()
{
	Protocol::SCRAQiYuanKongMingDengInfo info;
	info.huozhong = m_param.huozhong;
	info.complete_flag = m_param.complete_flag;

	memcpy(info.task_list, m_param.task_list, sizeof(info.task_list));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityQiYuanKongMingDeng::RADianDeng()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityQiYuanKongMingDengConfig * config = static_cast<const RandActivityQiYuanKongMingDengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
		return;

	// 先看是否在活动时间中
	time_t now = EngineAdapter::Instance().Time();
	unsigned int zero_times = GetZeroTime(now);
	unsigned int zero_to_now_times = now - zero_times;

	if (zero_to_now_times < config->GetOtherCfg()->time_start || zero_to_now_times > config->GetOtherCfg()->time_end)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_IN_ACTIVE_TIME);
		return;
	}

	if (m_param.huozhong < config->GetOtherCfg()->reward_consume)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_QIYUAN_KONGMINGDENG_NOT_ITEM);
		return;
	}

	const RAQiYuanKongMingDengRewardCfg * reward_cfg = config->GetRewardCfg(ramgr);
	if (NULL == reward_cfg)
		return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(reward_cfg->reward, false))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(reward_cfg->reward, PUT_REASON_RA_QIYUAN_KONGMINGDENG);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward[0], (int)reward_cfg->reward.size(), SEND_MAIL_TYPE_TIGER_ACTIVITY, true, m_role_activity_type);
	}

	m_param.huozhong -= config->GetOtherCfg()->reward_consume;

	this->RASendInfo();
	this->SetRoleDataChange();

	// log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] huozhong[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		m_param.huozhong);

	this->OnJoinActivity();
}

void RoleActivityQiYuanKongMingDeng::OnJoinActivity()
{
	{
		RoleActivityYuanQiJiShiBu * role_activity = static_cast<RoleActivityYuanQiJiShiBu *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU));
		if (NULL != role_activity)
		{
			role_activity->OnCompleteTask(RA_YUAN_QI_JISHIBU_TASK_TYPE_VITALITY_HELPS);
		}
	}
}

void RoleActivityQiYuanKongMingDeng::GMAddHuoZhong(int add_huozhong)
{
	m_param.huozhong += add_huozhong;

	this->RASendInfo();
	this->SetRoleDataChange();
}
