#include "roleactivityronglianyouli.hpp"
#include "config/randactivityconfig/impl/randactivityronglianyouliconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "servercommon/errornum.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "other/funopen/funopen.hpp"


RoleActivityRongLianYouLi::RoleActivityRongLianYouLi(int type)
	:RoleActivity(type)
{

}

RoleActivityRongLianYouLi::~RoleActivityRongLianYouLi()
{

}

void RoleActivityRongLianYouLi::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityRongLianYouLi::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityRongLianYouLi::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (m_param.ra_activity_end_stamp != 0 && EngineAdapter::Instance().Time() >= m_param.ra_activity_end_stamp)
	{
		this->ReissueAllReward();
		m_param.ra_activity_end_stamp = 0;
	}
}

void RoleActivityRongLianYouLi::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	this->RASendInfo();
}

void RoleActivityRongLianYouLi::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
	{
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(rand_act_type);
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
		m_param.ra_activity_end_stamp = ramgr->GetRandActivityEndTime(rand_act_type);

		this->RASendInfo();
		this->SetRoleDataChange();
	}

}

void RoleActivityRongLianYouLi::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	if (NULL == raor)
	{
		return;
	}

	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_RA_RIBG_LIAN_YOU_LI))
		return;

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
			this->RASendInfo();			// 发协议
		}
		break;
	case 1:
		{
			this->ReceiveTaskReward(raor->param_1);		// 领取完成任务奖励，p1-seq
		}
		break;
	case 2:
		{
			this->ReceiveTaskNumReward();				// 领取累计完成任务奖励
		}
		break;
	default:
		break;
	}
}

void RoleActivityRongLianYouLi::AddTaskScore(int purple, int orange, int red)
{
	if (purple < 0 || orange < 0 || red < 0)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	const RandActivityRongLianYouLiConfig * config = static_cast<const RandActivityRongLianYouLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	const std::vector<RARongLianYouLiRewardCfg> * reward_cfg = config->GetRewardCfg(ramgr);
	if (NULL == reward_cfg) return;

	for (std::vector<RARongLianYouLiRewardCfg>::const_iterator it = reward_cfg->begin(); it != reward_cfg->end(); ++it)
	{
		if (it->attribute_type == RARongLianYouLi::RONG_LIAN_TYPE_PURPLE && purple > 0)
		{
			m_param.task_score[it->seq] += purple;
		}
		else if (it->attribute_type == RARongLianYouLi::RONG_LIAN_TYPE_ORANGE && orange > 0)
		{
			m_param.task_score[it->seq] += orange;
		}
		else if (it->attribute_type == RARongLianYouLi::RONG_LIAN_TYPE_RED && red > 0)
		{
			m_param.task_score[it->seq] += red;
		}
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] get_attribute[purple[%d], orange[%d], red[%d]]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), purple, orange, red);

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityRongLianYouLi::RASendInfo()
{
	const RandActivityRongLianYouLiConfig * config = static_cast<const RandActivityRongLianYouLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const std::vector<RARongLianYouLiRewardCfg> * reward_cfg = config->GetRewardCfg(ramgr);
	if (NULL == reward_cfg) return;


	Protocol::SCRARongLianYouLiInfo msg;
	msg.has_task_stage_reward = m_param.has_task_stage_reward;
	msg.total_task = (short)reward_cfg->size();
	msg.total_task_finish = m_param.total_task_finish;

	memcpy(msg.task_score,m_param.task_score,sizeof(msg.task_score));
	memcpy(msg.reward_receive_num, m_param.reward_receive_num, sizeof(msg.reward_receive_num));

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void RoleActivityRongLianYouLi::ReceiveTaskReward(int seq)
{
	if (seq < 0) return;

	const RandActivityRongLianYouLiConfig * config = static_cast<const RandActivityRongLianYouLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const std::vector<RARongLianYouLiRewardCfg> * reward_cfg = config->GetRewardCfg(ramgr);
	if (NULL == reward_cfg) return;

	std::vector<RARongLianYouLiRewardCfg>::const_iterator it = reward_cfg->begin();
	for (; it != reward_cfg->end(); ++it)
	{
		if (seq == it->seq)
		{
			if (m_param.reward_receive_num[seq] >= it->reward_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_REWARD_ALL_RECEIVE_ALREADY);
				return;
			}

			if (m_param.task_score[seq] < it->parameter)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_ANECDOTE_TASK_NOT_FINISH);
				return;
			}

			if (m_role_module_mgr->GetKnapsack()->CheckForPut2(it->reward, true, false))
			{
				m_role_module_mgr->GetKnapsack()->Put(it->reward, PUT_REASON_RONG_LIAN_YOU_LI);
			}
			else
			{
				m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return;
			}

			m_param.task_score[seq] -= it->parameter;
			m_param.reward_receive_num[seq] += 1;

			m_param.total_task_finish += 1;
			
			gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] task_seq[%d] get_reward[%d-%d]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), seq, it->reward.item_id, it->reward.num);

			this->RASendInfo();
			this->SetRoleDataChange();
		}
	}
}

void RoleActivityRongLianYouLi::ReceiveTaskNumReward()
{
	const RandActivityRongLianYouLiConfig * config = static_cast<const RandActivityRongLianYouLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const std::vector<int> task_list_cfg = config->GetTaskNumCfg(ramgr);
	if (task_list_cfg.empty()) return;

	int i = 0;
	for (std::vector<int>::const_iterator it = task_list_cfg.begin(); it < task_list_cfg.end() && i < RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_BITMAP; ++it, ++i)
	{
		if (m_param.total_task_finish >= (*it))
		{
			if (m_param.has_task_stage_reward.IsBitSet(i))
			{
				continue;
			}

			const RARongLianYouLiStageRewardCfg * stage_reward_cfg = config->GetRandStageRewardCfg(ramgr, *it);
			if (NULL == stage_reward_cfg) return;

			if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(stage_reward_cfg->reawrd))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return;
			}
			m_role_module_mgr->GetKnapsack()->PutVec(stage_reward_cfg->reawrd, PUT_REASON_RONG_LIAN_YOU_LI);

			m_param.has_task_stage_reward.SetBit(i);

			std::string gamelog_str;
			for (int j = 0; j < (int)stage_reward_cfg->reawrd.size(); ++j)
			{
				gamelog_str = STRING_SPRINTF("{item[%d, %d]} ", stage_reward_cfg->reawrd[j].item_id, stage_reward_cfg->reawrd[j].num);
			}

			gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] reward_stage[%d] get_reward[%s]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), i+1, gamelog_str.c_str());

			this->RASendInfo();
			this->SetRoleDataChange();

			return;
		}
		else
		{
			return;
		}
	}
}

void RoleActivityRongLianYouLi::ReissueAllReward()
{
	const RandActivityRongLianYouLiConfig * config = static_cast<const RandActivityRongLianYouLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const std::vector<RARongLianYouLiRewardCfg> * reward_cfg = config->GetRewardCfg(ramgr);
	if (NULL == reward_cfg) return;

	std::vector<ItemConfigData> reward_list;

	std::vector<RARongLianYouLiRewardCfg>::const_iterator it = reward_cfg->begin();
	for (int i = 0; i < RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_TASK_NUM && it != reward_cfg->end(); ++i, ++it)
	{
		if (m_param.reward_receive_num[i] < it->reward_times && m_param.task_score[i] >= it->parameter)
		{
			int reward_num = m_param.task_score[i] / it->parameter;

			if (m_param.reward_receive_num[i] + reward_num > it->reward_times)	//超过上限
			{
				reward_num = it->reward_times - m_param.reward_receive_num[i];
			}

			m_param.task_score[i] -= reward_num * it->parameter;
			m_param.reward_receive_num[i] += reward_num;

			reward_list.push_back(ItemConfigData(it->reward.item_id, it->reward.is_bind, it->reward.num * reward_num));
		}
	}

	const std::vector<int> task_list_cfg = config->GetTaskNumCfg(ramgr);
	if (task_list_cfg.empty()) return;

	int reward_bit = 0;
	for (std::vector<int>::const_iterator iter = task_list_cfg.begin(); iter < task_list_cfg.end() && reward_bit < RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_BITMAP; ++iter, ++reward_bit)
	{
		if (m_param.total_task_finish >= *iter)
		{
			if (m_param.has_task_stage_reward.IsBitSet(reward_bit))
			{
				continue;
			}

			const RARongLianYouLiStageRewardCfg * stage_reward_cfg = config->GetRandStageRewardCfg(ramgr, *iter);
			if (NULL == stage_reward_cfg) return;

			reward_list.insert(reward_list.end(), stage_reward_cfg->reawrd.begin(), stage_reward_cfg->reawrd.end());

			m_param.has_task_stage_reward.SetBit(reward_bit);
		}
		else
		{
			break;
		}
	}

	if (reward_list.empty()) return;

	std::vector<std::vector<ItemConfigData> > send_mail_list;
	send_mail_list.push_back(std::vector<ItemConfigData>());

	int send_size = 0;
	std::vector<ItemConfigData>::iterator send_it = reward_list.begin();
	for (; send_it != reward_list.end() ; ++send_it)
	{
		send_mail_list[send_size].push_back(*send_it);

		if (send_mail_list[send_size].size() == 8)
		{
			send_mail_list.push_back(std::vector<ItemConfigData>());
			send_size++;
		}
	}

	for (int j = 0; j < (int)send_mail_list.size(); ++j)
	{
		if (!m_role_module_mgr->GetKnapsack()->SendMail(send_mail_list[j], SEND_MAIL_TYPE_RONG_LIAN_YOU_LI))
		{
			return;
		}
	}


	std::string gamelog_str;
	for (int i = 0; i < (int)send_mail_list.size(); ++i)
	{
		for (int j = 0; j < (int)send_mail_list[i].size(); ++j)
		{
			gamelog_str = STRING_SPRINTF("{item[%d, %d]} ", send_mail_list[i][j].item_id, send_mail_list[i][j].num);
		}		
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] get_reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), gamelog_str.c_str());

	this->RASendInfo();
	this->SetRoleDataChange();
}


