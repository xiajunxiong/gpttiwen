#include "roleactivitylantangchumu.hpp"
#include "config/randactivityconfig/impl/randactivitylantangchumuconfig.hpp"
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

RoleActivityLanTangChuMu::RoleActivityLanTangChuMu(int type)
	:RoleActivity(type), m_online_task_timestamp(0)
{
}

RoleActivityLanTangChuMu::~RoleActivityLanTangChuMu()
{
}

void RoleActivityLanTangChuMu::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RALanTangChuMuParam));
}

void RoleActivityLanTangChuMu::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RALanTangChuMuParam));
}

void RoleActivityLanTangChuMu::CheckInitRandActivity(int rand_act_type, bool is_force)
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

void RoleActivityLanTangChuMu::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
	case 2:		//加入材料, p1--seq, p2--数量
		{
			this->AddHerbs(raor->param_1, raor->param_2);
		}
		break;
	case 3:		//开始沐浴
		{
			this->StartBathe();
		}
		break;
	default:
		break;
	}
}

void RoleActivityLanTangChuMu::OnLogin()
{
	m_online_task_timestamp = EngineAdapter::Instance().Time() + 60;

	this->RASendInfo();
}

void RoleActivityLanTangChuMu::Update(time_t now_second)
{
	if (m_online_task_timestamp != 0 && now_second >= m_online_task_timestamp)
	{
		m_online_task_timestamp = now_second + 60;
		this->AddTaskParam(LTCMBasicCfg::TASK_TYPE_ONLINE_TIME);
	}
}

void RoleActivityLanTangChuMu::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_online_task_timestamp = EngineAdapter::Instance().Time() + 60;
	m_param.DayReset();

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityLanTangChuMu::AddTaskParam(int task_type, int task_param)
{
	if (task_type < 0 || task_type >= LTCMBasicCfg::TASK_TYPE_MAX || task_param <= 0) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	const RandActivityLanTangChuMuConfig * config = static_cast<const RandActivityLanTangChuMuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	int task_num = config->GetTaskNumCfg(ramgr);
	if (task_num <= 0) return;

	for (int i = 0; i < task_num; ++i)
	{
		const LTCMBasicCfg* task_cfg = config->GetTaskCfg(ramgr, i);
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

			if (task_type == LTCMBasicCfg::TASK_TYPE_ONLINE_TIME && m_param.today_task_finish.IsBitSet(i))
			{
				m_online_task_timestamp = 0;
			}
		}
	}

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityLanTangChuMu::ReceiveReward(int seq)
{
	if (seq < 0 || seq >= DUAN_WU_ACTIVITY_MAX_BITMAP_NUM) return;
	if (!m_param.today_task_finish.IsBitSet(seq) || m_param.is_receive_reward.IsBitSet(seq)) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityLanTangChuMuConfig * config = static_cast<const RandActivityLanTangChuMuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	const LTCMBasicCfg* task_cfg = config->GetTaskCfg(ramgr, seq);
	if (NULL == task_cfg) return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(task_cfg->reward_list))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(task_cfg->reward_list, PUT_REASON_RA_LAN_TANG_CHU_MU);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&task_cfg->reward_list[0], (int)task_cfg->reward_list.size(), SEND_MAIL_TYPE_LAN_TANG_CHU_MU);
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

void RoleActivityLanTangChuMu::AddHerbs(int seq, int num)
{
	if (m_param.last_reward_flag != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_LAN_TANG_CHU_MU_CAN_NOT_ADD_HERBS);
		return;
	}

	const RandActivityLanTangChuMuConfig * config = static_cast<const RandActivityLanTangChuMuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	const std::vector<std::pair<ItemID, int> >& add_herbs_cfg = config->GetHerbsCfg();

	if (seq < 0 || seq >= (int)add_herbs_cfg.size() || seq >= MAX_HERBS_TYPE_LAN_TANG_CHU_MU || num <= 0) return;

	int need_num = add_herbs_cfg[seq].second - m_param.add_herbs_num[seq];
	if (need_num <= 0) return;

	num = num > need_num ? need_num : num;

	if (!m_role_module_mgr->GetKnapsack()->HasItem(add_herbs_cfg[seq].first, num, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(add_herbs_cfg[seq].first, num, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	m_param.add_herbs_num[seq] += num;

	bool is_put_all_in = true;
	for (int i = 0; i < MAX_HERBS_TYPE_LAN_TANG_CHU_MU; i++)
	{
		is_put_all_in = is_put_all_in && m_param.add_herbs_num[i] >= add_herbs_cfg[i].second;
	}
	if (is_put_all_in)
	{
		m_param.last_reward_flag = 1;
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] consume_item[%d, %d] seq[%d] cur_add_herbs_num[%d, %d, %d, %d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), add_herbs_cfg[seq].first, num, seq, m_param.add_herbs_num[0], m_param.add_herbs_num[1], m_param.add_herbs_num[2], m_param.add_herbs_num[3]);

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityLanTangChuMu::StartBathe()
{
	if (m_param.last_reward_flag != 1) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityLanTangChuMuConfig * config = static_cast<const RandActivityLanTangChuMuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	const std::vector<ItemConfigData>* reward_list = config->RandRewrdCfg(ramgr);
	if (NULL == reward_list) return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(*reward_list))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)reward_list->size(), &(*reward_list)[0], PUT_REASON_NO_NOTICE, false);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(*reward_list, SEND_MAIL_TYPE_LAN_TANG_CHU_MU);
	}
	m_role_module_mgr->GetKnapsack()->NotifyGetItemListVec(*reward_list, GET_REASON_LAN_TANG_CHU_MU);

	m_param.last_reward_flag = 2;

	{	//  记录日志
		std::string log_str;
		for (int i = 0; i < (int)reward_list->size(); i++)
		{
			log_str += STRING_SPRINTF("[%d, %d] ", (*reward_list)[i].item_id, (*reward_list)[i].num);
		}

		gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] last_reward_flag[%d] reward[%s]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.last_reward_flag, log_str.c_str());
	}

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityLanTangChuMu::RASendInfo()
{
	Protocol::SCRALanTangChuMuInfo msg;
	msg.is_receive_reward = m_param.is_receive_reward;
	memcpy(msg.today_task_count, m_param.today_task_count, sizeof(msg.today_task_count));
	msg.last_reward_flag = m_param.last_reward_flag;
	memcpy(msg.add_herbs_num, m_param.add_herbs_num, sizeof(msg.add_herbs_num));

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}
