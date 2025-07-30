#include "roleactivityduanyangxiangnang.hpp"
#include "config/randactivityconfig/impl/randactivityduanyangxiangnangconfig.hpp"
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

RoleActivityDuanYangXiangNang::RoleActivityDuanYangXiangNang(int type)
	:RoleActivity(type), m_online_task_timestamp(0)
{
}

RoleActivityDuanYangXiangNang::~RoleActivityDuanYangXiangNang()
{
}

void RoleActivityDuanYangXiangNang::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RADuanYangXiangNangParam));
}

void RoleActivityDuanYangXiangNang::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RADuanYangXiangNangParam));
}

void RoleActivityDuanYangXiangNang::CheckInitRandActivity(int rand_act_type, bool is_force)
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

void RoleActivityDuanYangXiangNang::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
	case 1:		// 领取任务奖励,p1-seq
		{
			this->ReceiveReward(raor->param_1);
		}
		break;
	case 2:		//制作香囊
		{
			this->MakeSachet();
		}
		break;
	default:
		break;
	}
}

void RoleActivityDuanYangXiangNang::OnLogin()
{
	m_online_task_timestamp = EngineAdapter::Instance().Time() + 60;

	this->RASendInfo();
}

void RoleActivityDuanYangXiangNang::Update(time_t now_second)
{
	if (m_online_task_timestamp != 0 && now_second >= m_online_task_timestamp)
	{
		m_online_task_timestamp = now_second + 60;
		this->AddTaskParam(DYXNBasicCfg::TASK_TYPE_ONLINE_TIME);
	}
}

void RoleActivityDuanYangXiangNang::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_online_task_timestamp = EngineAdapter::Instance().Time() + 60;
	m_param.DayReset();

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityDuanYangXiangNang::AddTaskParam(int task_type, int task_param)
{
	if (task_type < 0 || task_type >= DYXNBasicCfg::TASK_TYPE_MAX || task_param <= 0) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityDuanYangXiangNangConfig * config = static_cast<const RandActivityDuanYangXiangNangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	int task_num = config->GetTaskNumCfg(ramgr);
	if (task_num <= 0) return;

	for (int i = 0; i < task_num; ++i)
	{
		const DYXNBasicCfg* task_cfg = config->GetTaskCfg(ramgr, i);
		if (NULL == task_cfg) continue;

		if (task_cfg->task_type == task_type)
		{
			if (!m_param.today_task_finish.IsBitSet(i))
			{
				m_param.today_task_count[i] += task_param;

				if (m_param.today_task_count[i] >= task_cfg->param)
				{
					m_param.today_task_count[i] = task_cfg->param;
					m_param.today_task_finish.SetBit(i);

					gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] task_finish[seq:%d, task_type:%d]", __FUNCTION__,
						m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), i, task_type);
				}
			}

			if (task_type == DYXNBasicCfg::TASK_TYPE_ONLINE_TIME && m_param.today_task_finish.IsBitSet(i))
			{
				m_online_task_timestamp = 0;
			}
		}
	}

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityDuanYangXiangNang::ReceiveReward(int seq)
{
	if (seq < 0 || seq >= DUAN_WU_ACTIVITY_MAX_BITMAP_NUM) return;
	if (!m_param.today_task_finish.IsBitSet(seq) || m_param.is_receive_reward.IsBitSet(seq)) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityDuanYangXiangNangConfig * config = static_cast<const RandActivityDuanYangXiangNangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	const DYXNBasicCfg* task_cfg = config->GetTaskCfg(ramgr, seq);
	if (NULL == task_cfg) return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(task_cfg->reward_list))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(task_cfg->reward_list, PUT_REASON_RA_DUAN_YANG_XIANG_NANG);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&task_cfg->reward_list[0], (int)task_cfg->reward_list.size(), SEND_MAIL_TYPE_DUAN_YANG_XIANG_NANG);
	}

	{	//  记录日志
		std::string log_str;
		for (int i = 0; i < (int)task_cfg->reward_list.size(); i++)
		{
			log_str += STRING_SPRINTF("[%d, %d] ", task_cfg->reward_list[i].item_id, task_cfg->reward_list[i].num);
		}

		gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] receive_reward[seq:%d] reward[%s]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), seq, log_str.c_str());
	}

	m_param.is_receive_reward.SetBit(seq);

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityDuanYangXiangNang::MakeSachet()
{
	const RandActivityDuanYangXiangNangConfig * config = static_cast<const RandActivityDuanYangXiangNangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	const std::vector<std::pair<ItemID, int> >& make_cfg = config->GetMakeCfg();

	for (size_t i = 0; i < make_cfg.size(); i++)
	{
		if (!m_role_module_mgr->GetKnapsack()->HasItem(make_cfg[i].first, make_cfg[i].second, false))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
			return;
		}
	}

	std::string consume_str;
	for (size_t i = 0; i < make_cfg.size(); i++)
	{
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(make_cfg[i].first, make_cfg[i].second, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}

		consume_str += STRING_SPRINTF("[%d, %d] ", make_cfg[i].first, make_cfg[i].second);
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const DYXNRewardCfg* reward_cfg = config->RandRewardCfg(ramgr);
	if (NULL == reward_cfg) return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(reward_cfg->reward_list))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(reward_cfg->reward_list, PUT_REASON_NO_NOTICE);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward_list[0], (int)reward_cfg->reward_list.size(), SEND_MAIL_TYPE_DUAN_YANG_XIANG_NANG);
	}
	m_role_module_mgr->GetKnapsack()->NotifyGetItemListVec(reward_cfg->reward_list, GET_REASON_DUAN_YANG_XIANG_NANG);

	{	//  记录日志
		std::string log_str;
		for (int i = 0; i < (int)reward_cfg->reward_list.size(); i++)
		{
			log_str += STRING_SPRINTF("[%d, %d] ", reward_cfg->reward_list[i].item_id, reward_cfg->reward_list[i].num);
		}

		gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] consume_item[%s] reward[%s]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), consume_str.c_str(), log_str.c_str());
	}
}

void RoleActivityDuanYangXiangNang::RASendInfo()
{
	Protocol::SCRADuanYangXiangNang msg;
	msg.is_receive_reward = m_param.is_receive_reward;
	msg.today_task_finish = m_param.today_task_finish;
	memcpy(msg.today_task_count, m_param.today_task_count, sizeof(msg.today_task_count));

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}