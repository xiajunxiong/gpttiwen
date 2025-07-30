#include "roleactivityyunzeersanshi.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityyunzeersanshiconfig.hpp"

#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitycolorfuldouble.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "gamelog.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/msgitem.h"

#include "other/roleactivity/impl/roleactivitychaozhifenxiang.hpp"
#include "other/roleactivity/impl/roleactivityyunzecutepet.hpp"
#include "other/roleattribute/roleexpconfig.hpp"
#include "other/levelcomplement/levelcomplement.h"
#include "other/event/eventhandler.hpp"

RoleActivityYunZeErSanShi::RoleActivityYunZeErSanShi(int type)
	:RoleActivity(type)
{
}

RoleActivityYunZeErSanShi::~RoleActivityYunZeErSanShi()
{
}

void RoleActivityYunZeErSanShi::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityYunZeErSanShi::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityYunZeErSanShi::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI != rand_act_type)
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

		this->RandTask();

		this->SetRoleDataChange();
		this->RASendInfo();
	}
}

void RoleActivityYunZeErSanShi::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:
		{
			this->SendReward(raor->param_1);
		}
		break;
	case 2:
		{
			this->SendTurntableReward();
		}
		break;
	}
}

void RoleActivityYunZeErSanShi::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI))
	{
		return;
	}

	m_param.DayReset();
	this->RandTask();
	m_param.task_id = m_param.task_list[m_param.times];

	this->SetRoleDataChange();
	this->RASendInfo();
}

void RoleActivityYunZeErSanShi::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI))
		return;
	m_param.task_id = m_param.task_list[m_param.times];

	this->RASendInfo();
}

void RoleActivityYunZeErSanShi::RASendInfo()
{
	Protocol::SCRAYunZeErSanShiInfo msg;
	msg.times = m_param.times;
	msg.is_turntable_reward = m_param.is_turntable_reward;
	msg.task_id = m_param.task_id;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void RoleActivityYunZeErSanShi::SendReward(int task_id)
{
	if (task_id <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityYunZeErSanShiConfig * config = static_cast<const RandActivityYunZeErSanShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_param.times >= config->GetOtherCfg().frequency)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TODAY_JOIN_COUNT_LIMIT);
		return;
	}

	const RAYZErSanShiRewardCfg * reward_cfg = config->GetRewardCfg(ramgr, task_id);
	if (NULL == reward_cfg) return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0], PUT_REASON_RA_YUNZE_ER_SAN_SHI_REWARD);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward_list[0], (int)reward_cfg->reward_list.size(), SEND_MAIL_TYPE_YUN_ZE_ER_SAN_SHI, true);
	}

	m_param.times++;		// 次数累加
	m_param.task_id = m_param.task_list[m_param.times];		// 切换到新题目

	if (m_param.times == config->GetOtherCfg().frequency)
	{
		const RoleLevelCfg* level_cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(m_role_module_mgr->GetRole()->GetLevel());
		if (NULL == level_cfg) return;

		int add_exp = static_cast<int>((level_cfg->exp * config->GetOtherCfg().exp) / RAYZErSanShiOtherCfg::EXP_BASE_NUM);

		long long actual_exp = m_role_module_mgr->GetRole()->AddExp(add_exp, ADD_EXP_TYPE_YUNZE_ER_SAN_SHI, __FUNCTION__);
		m_role_module_mgr->GetLevelComplement()->OnAddExp(actual_exp);
		EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_role_module_mgr->GetRole(), add_exp, __FUNCTION__);
	}

	{
		RoleActivityColorfulDouble * role_activity = static_cast<RoleActivityColorfulDouble *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RAColorfulDouble::TSAK_TYPE_YUN_ZE_ER_SAN_SHI, 1);
		}
	}

	if (m_param.times == config->GetOtherCfg().frequency)
	{
		m_param.is_turntable_reward = 1;
		m_param.task_id = 0;
	}

	std::string gamelog_str;
	for (int i = 0; i < reward_cfg->reward_list.size(); ++i)
	{
		gamelog_str += STRING_SPRINTF("{item[%d, %d, %d]} ", reward_cfg->reward_list[i].item_id, reward_cfg->reward_list[i].is_bind, reward_cfg->reward_list[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeErSanShi::%s role[%d, %s] reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), gamelog_str.c_str());

	this->SetRoleDataChange();
	this->RASendInfo();

	{
		RoleActivityChaoZhiFenXiang * role_activity = static_cast<RoleActivityChaoZhiFenXiang *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG));
		if (NULL != role_activity)
		{
			role_activity->AddTaskComplete(RA_CHAOZHI_FENXIANG_TASK_TYPE_YUNZE_23);
		}
	}
	{
		RoleActivityYunZeCutePet * role_activity = static_cast<RoleActivityYunZeCutePet *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RAYZCutePet::TASK_TYPE_YZ_ER_SAN_SHI);
		}
	}

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityYunZeErSanShi::SendTurntableReward()
{	
	if (m_param.is_receive_reward == RECEIVE_STATUS_LACK)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_COLLECT_WORD_NOT_FETCH);
		return;
	}

	if (0 == m_param.is_turntable_reward)
	{
		return;
	}

	const RandActivityYunZeErSanShiConfig * config = static_cast<const RandActivityYunZeErSanShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}	

	if (m_param.times != config->GetOtherCfg().frequency)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_UNFINISHED_TASK);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAYZErSanShiTurnTableRewardCfg * turntable_reward = config->GetRandTurntableRewardCfg(ramgr);
	if (NULL == turntable_reward) return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPut(turntable_reward->turntable_reward.item_id, turntable_reward->turntable_reward.num))
	{
		m_role_module_mgr->GetKnapsack()->Put(turntable_reward->turntable_reward, PUT_REASON_RA_YUN_ZE_ER_SAN_SHI, NULL, NULL, 0, 0, false);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&turntable_reward->turntable_reward, 1, SEND_MAIL_TYPE_YUN_ZE_ER_SAN_SHI);
	}

	m_role_module_mgr->GetKnapsack()->NotifyGetItemList(1,&turntable_reward->turntable_reward,GET_REASON_YUNZE_ER_SAN_SHI);

	m_param.is_receive_reward = RECEIVE_STATUS_LACK;
	m_param.is_turntable_reward = 0;

	if (turntable_reward->is_hearsay)
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_yun_ze_er_san_shi_borcast,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_activity_type, turntable_reward->turntable_reward.item_id, m_role_activity_type);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
		}

		int length2 = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_yun_ze_er_san_shi_borcast,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_activity_type, turntable_reward->turntable_reward.item_id);
		if (length2 > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length2, GetMsgType(SYS_MSG_ROLLING_NOTICE));
		}
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeErSanShi::%s role[%d, %s] reward[%d, %d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), turntable_reward->turntable_reward.item_id,turntable_reward->turntable_reward.num);

	Protocol::SCRAYunZeErSanShiTurntableInfo ramsg;
	ramsg.seq = turntable_reward->seq;
	m_role_module_mgr->NetSend(&ramsg, sizeof(ramsg));

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityYunZeErSanShi::RandTask()
{
	const RandActivityYunZeErSanShiConfig * config = static_cast<const RandActivityYunZeErSanShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int task_num = 0;
	while (task_num != config->GetOtherCfg().frequency)
	{
		int t_task_id = config->GetRandTaskCfg(config->GetOtherCfg().frequency);
		if (t_task_id > 0)
		{
			int i = 0;
			for ( ; i < config->GetOtherCfg().frequency;++i)
			{
				if (m_param.task_list[i] == t_task_id)
				{
					break;
				}
			}

			if (config->GetOtherCfg().frequency == i)
			{
				m_param.task_list[task_num] = t_task_id;
				task_num++;
			}

		}
		else
		{
			memset(m_param.task_list, 0, sizeof(m_param.task_list));
			return;
		}
	}
}
