#include "roleactivityshanhaiquestion.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "gamelog.h"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"

#include "config/randactivityconfig/impl/randactivityshanhaiquestionconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "protocol/randactivity/msgshanhaiquestion.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/levelcomplement/levelcomplement.h"
#include "other/roleattribute/roleexpconfig.hpp"

#include "global/randactivity/randactivityimpl/randactivityshanhaiquestion.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimet.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimeet.hpp"
#include "other/event/eventhandler.hpp"

RoleActivityShanHaiQuestion::RoleActivityShanHaiQuestion(int type)
	:RoleActivity(type)
{

}

RoleActivityShanHaiQuestion::~RoleActivityShanHaiQuestion()
{

}

void RoleActivityShanHaiQuestion::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAShanHaiQuestionParam));
}

void RoleActivityShanHaiQuestion::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(RAShanHaiQuestionParam) <= sizeof(RoleActivityRoleData));
	memcpy(p->base_data, &m_param, sizeof(RAShanHaiQuestionParam));
}

void RoleActivityShanHaiQuestion::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.times = 0;
	this->RandQuestion();
}



 void RoleActivityShanHaiQuestion::OnLoginSendInfo()
 {
	 RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	 if (NULL == ramgr) return;

	 RandActivityShanHaiQuestion * activity = (RandActivityShanHaiQuestion *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_SHANHAI_QUESTION);
	 if (NULL == activity)
	 {
		 m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		 return;
	 }

	 //this->SendActivityStatusInfo(activity->GetActivityStatus());
	 Protocol::SCRAShanHaiQuestionInfo msg;

	 msg.is_can_answer = activity->GetActivityStatus();
	 msg.question_id = m_param.question_id;
	 msg.reward_seq = m_param.reward_seq;
	 msg.times = m_param.times;

	 m_role_module_mgr->NetSend(&msg, sizeof(msg));
 }

void RoleActivityShanHaiQuestion::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_SHANHAI_QUESTION != rand_act_type)
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

		this->RandQuestion();

		this->SetRoleDataChange();
	}

}

void RoleActivityShanHaiQuestion::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor) 
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SHANHAI_QUESTION))
	{
		return;
	}

	switch (raor->opera_type)
	{
	case 0:
		{
			this->SendInfo();
		}
		break;
	case 1:
		{
			this->DecideReward(raor->param_1, raor->param_2);
		}
		break;
	}
}

void RoleActivityShanHaiQuestion::DecideReward(int question_id, int answer_id)
{
	if (question_id <= 0 || question_id > MAX_QUESTION_NUM || answer_id <= 0 || answer_id > 3)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());	//判断活动是否开启
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SHANHAI_QUESTION))
	{ 
		return;
	}	

	const RandActivityShanHaiQuestionConfig * config = static_cast<const RandActivityShanHaiQuestionConfig *>			//获取配置
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SHANHAI_QUESTION));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	unsigned int cur_hour = static_cast<unsigned int>(EngineAdapter::Instance().LocalTime()->tm_hour);
	if (cur_hour < (config->GetQuertionOtherCfg().start_times / 100) || cur_hour >= (config->GetQuertionOtherCfg().end_times / 100))	//判断是否在答题时间内
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_IN_ACTIVE_TIME);
		return;
	}

	if (m_param.times >= config->GetQuertionOtherCfg().ans_times)			//判断是否剩余可答题次数
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TODAY_JOIN_COUNT_LIMIT);
		return;
	}

	if (config->IsSucAnswer(question_id, answer_id))
	{
		const RewardGroupCfg * t_reward_group = config->GetRewardGroupBySeqCfg(m_param.reward_seq);
		if (NULL == t_reward_group)
		{
			return;
		}

		std::vector<ItemConfigData> v_item;
		v_item = t_reward_group->reward_item;
		if (v_item.empty()) return;

		m_param.times++;
		this->OnDecideReward();

		if (!m_role_module_mgr->GetKnapsack()->PutList((short)v_item.size(),&v_item[0], PUT_REASON_SHAN_HAI_QUESTION_REWARD))
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&v_item[0], (int)v_item.size(), SEND_MAIL_TYPE_SHAN_HAI_QUESTION, true);
		}

		std::string gamelog_str;
		for (int i = 0; i < v_item.size(); ++i)
		{
			gamelog_str += STRING_SPRINTF("{item[%d, %d]} ", v_item[i].item_id, v_item[i].num);
		}

		gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityShanHaiQuestion::%s role[%d, %s] type[%d] reward_item[%s]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(),m_role_module_mgr->GetRole()->GetName(), RAND_ACTIVITY_TYPE_SHANHAI_QUESTION, gamelog_str.c_str());

		if (t_reward_group->exp > 0)
		{
			const RoleLevelCfg* level_cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(m_role_module_mgr->GetRole()->GetLevel());
			if (NULL == level_cfg) return;

			int add_exp = static_cast<int>((level_cfg->exp * t_reward_group->exp) / RewardGroupCfg::EXP_BASE_NUM);

			long long actual_exp = m_role_module_mgr->GetRole()->AddExp(add_exp, ADD_EXP_TYPE_SHAN_HAI_QUESTION, __FUNCTION__);
			m_role_module_mgr->GetLevelComplement()->OnAddExp(actual_exp);
			EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_role_module_mgr->GetRole(), add_exp, __FUNCTION__);

			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityShanHaiQuestion::%s role[%d, %s] role_level[%d] add_exp_num[%d]", __FUNCTION__,
				m_role_module_mgr->GetRoleUID(), m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetRole()->GetLevel(), t_reward_group->exp);
		}

		if (t_reward_group->coin > 0)
		{
			m_role_module_mgr->GetMoney()->AddCoinBind(t_reward_group->coin, __FUNCTION__);
		}
	}
	else
	{
		int rand_open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHANHAI_QUESTION);

		int reward_group = config->GetBasisFaultCfg(rand_open_day);
		if (reward_group <= 0) return;

		const RewardGroupCfg * t_fault_reward = config->GetRewardGroupCfg(reward_group);
		if (NULL == t_fault_reward) return;

		const RewardGroupCfg * t_reward_group = config->GetRewardGroupBySeqCfg(t_fault_reward->seq);
		if (NULL == t_reward_group)
		{
			return;
		}

		m_param.times++;
		this->OnDecideReward();

		const RoleLevelCfg* level_cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(m_role_module_mgr->GetRole()->GetLevel());
		if (NULL == level_cfg) return;

		int add_exp = static_cast<int>((level_cfg->exp * t_reward_group->exp) / RewardGroupCfg::EXP_BASE_NUM);

		long long actual_exp = m_role_module_mgr->GetRole()->AddExp(add_exp, ADD_EXP_TYPE_SHAN_HAI_QUESTION, __FUNCTION__);
		m_role_module_mgr->GetLevelComplement()->OnAddExp(actual_exp);
		EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_role_module_mgr->GetRole(), add_exp, __FUNCTION__);
 
		gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityShanHaiQuestion::%s role[%d, %s] role_level[%d] add_exp_num[%d]", __FUNCTION__,
			m_role_module_mgr->GetRoleUID(), m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetRole()->GetLevel(), t_reward_group->exp);

		m_role_module_mgr->GetMoney()->AddCoinBind(t_reward_group->coin, __FUNCTION__);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityShanHaiQuestion::%s role[%d, %s] times[%d]",__FUNCTION__,
		m_role_module_mgr->GetRoleUID(), m_role_module_mgr->GetRoleName(), m_param.times);

	this->RandQuestion();

	this->OnParticipateRA(__FUNCTION__);
}

int RoleActivityShanHaiQuestion::RandRewardGroup(int reward_group)
{
	if (reward_group <= 0) return -1;

	const RandActivityShanHaiQuestionConfig * config = static_cast<const RandActivityShanHaiQuestionConfig *>			//获取配置
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SHANHAI_QUESTION));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return -2;
	}

	if (NULL == config->GetRewardGroupCfg(reward_group))
	{
		return -3;
	}
	int rand_num = config->GetRewardGroupCfg(reward_group)->seq;


	return rand_num;
}

void RoleActivityShanHaiQuestion::OnDecideReward()
{
	{//山海初遇
		RoleActivityShanHaiMet * role_activity = static_cast<RoleActivityShanHaiMet *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MET));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RAShanHaiMet::TSAK_TYPE_SHANHAI_QUESTION, 1);
		}
	}

	//活动-山海相逢
	RoleActivityShanHaiMeet * shan_hai_meet = static_cast<RoleActivityShanHaiMeet *>
		(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET));
	if (NULL != shan_hai_meet)
	{
		shan_hai_meet->OnFinishType(RA_SHAN_HAI_MEET_ACT_TYPE_3, 1);
	}
}

void RoleActivityShanHaiQuestion::RandQuestion()
{
	const RandActivityShanHaiQuestionConfig * config = static_cast<const RandActivityShanHaiQuestionConfig *>			//获取配置
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SHANHAI_QUESTION));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	
	if (m_param.times > config->GetQuertionOtherCfg().ans_times)			//判断是否剩余可答题次数
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TODAY_JOIN_COUNT_LIMIT);
		return;
	}

	const QuestionBankCfg * t_question = config->GetRandQuestionCfg(RandomNum(config->GetTotalRate()));
	if (NULL == t_question) return;

	int t_reward_seq = this->RandReward();
	if (t_reward_seq <= 0)
	{
		return;
	}

	m_param.reward_seq = t_reward_seq;
	m_param.question_id = t_question->question_id;

	this->SendInfo();
}

void RoleActivityShanHaiQuestion::SendActivityStatusInfo(bool is_open)
{

	Protocol::SCRAShanHaiQuestionStatusInfo msg;

	int open_type = is_open ? 1 : 0;
	msg.is_can_answer = open_type;

	m_role_module_mgr->NetSend(&msg,sizeof(msg));
}

int RoleActivityShanHaiQuestion::RandReward()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());	
	if (NULL == ramgr) return -1;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SHANHAI_QUESTION))
	{
		return -2;
	}

	const RandActivityShanHaiQuestionConfig * config = static_cast<const RandActivityShanHaiQuestionConfig *>			//获取配置
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SHANHAI_QUESTION));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return -3;
	}

	int rand_open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHANHAI_QUESTION);

	int reward_group = config->GetBasisSucCfg(rand_open_day);
	if (reward_group <= 0) return -4;

	int reward_seq = this->RandRewardGroup(reward_group);
	if (reward_seq <= 0) return -5;

	return reward_seq;
}

void RoleActivityShanHaiQuestion::SendInfo()
{
	Protocol::SCRAShanHaiQuestionInfo msg;

	short is_time = 0;
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityShanHaiQuestion * activity = (RandActivityShanHaiQuestion *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_SHANHAI_QUESTION);
	if (NULL == activity) return;
	if (activity->GetActivityStatus())
	{
		is_time = 1;
	}

	msg.is_can_answer = is_time;
	msg.times = m_param.times;
	msg.question_id = m_param.question_id;
	msg.reward_seq = m_param.reward_seq;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}
