#include "roleactivitychengfengpolang.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitychengfengpolangconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityyuanqijishibu.hpp"

RoleActivityChengFengPoLang::RoleActivityChengFengPoLang(int type) :RoleActivity(type)
{
}

RoleActivityChengFengPoLang::~RoleActivityChengFengPoLang()
{
}

void RoleActivityChengFengPoLang::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAChengFengPoLangParam));
}

void RoleActivityChengFengPoLang::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAChengFengPoLangParam));
}


void RoleActivityChengFengPoLang::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityChengFengPoLang * activity = (RandActivityChengFengPoLang *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity) return;

	const RandActivityChengFengPoLangConfig * config = static_cast<const RandActivityChengFengPoLangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CHENG_FENG_PO_LANG));
	if (NULL == config) return;


	activity->RASendGameInfo(m_role_module_mgr->GetRole());

	if (m_param.cur_question_id == 0 && m_param.answer_times < config->GetOtherCfg().frequency)
	{
		this->RandQuestion();
	}

	this->RASendInfo();
}

void RoleActivityChengFengPoLang::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.DayReset();

	const RandActivityChengFengPoLangConfig * config = static_cast<const RandActivityChengFengPoLangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CHENG_FENG_PO_LANG));
	if (NULL == config) return;

	if (m_param.cur_question_id == 0 && m_param.answer_times < config->GetOtherCfg().frequency)
	{
		this->RandQuestion();
	}
}


void RoleActivityChengFengPoLang::CheckInitRandActivity(int rand_act_type, bool is_force)
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

void RoleActivityChengFengPoLang::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
	case 1:	// 弃用
		{
		}
		break;
	case 2:			// 答题,p1-答案
		{
			this->AnswerReq(raor->param_1);
		}
		break;
	}
}

void RoleActivityChengFengPoLang::RandQuestion()
{	
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityChengFengPoLang * activity = (RandActivityChengFengPoLang *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity) return;

	const RandActivityChengFengPoLangConfig * config = static_cast<const RandActivityChengFengPoLangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CHENG_FENG_PO_LANG));
	if (NULL == config) return;

	if (m_param.answer_times >= config->GetOtherCfg().frequency)
	{
		return;
	}

	std::vector<int> used_questions;
	for (int i = 0; i < MAX_CHENG_FENG_PO_LANG_QUESTION && m_param.used_question[i] != 0; i++)
	{
		used_questions.push_back(m_param.used_question[i]);
	}

	m_param.cur_question_id = config->RandQuestionCfg(used_questions);

	if ((int)used_questions.size() < MAX_CHENG_FENG_PO_LANG_QUESTION)
	{
		m_param.used_question[used_questions.size()] = m_param.cur_question_id;
	}
	else		// 满了清空重算
	{
		memset(m_param.used_question, 0, sizeof(m_param.used_question));
		m_param.used_question[0] = m_param.cur_question_id;
	}

	this->RASendInfo();
}

void RoleActivityChengFengPoLang::AnswerReq(int answer)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityChengFengPoLang * activity = (RandActivityChengFengPoLang *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity) return;

	const RandActivityChengFengPoLangConfig * config = static_cast<const RandActivityChengFengPoLangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CHENG_FENG_PO_LANG));
	if (NULL == config) return;

	if (!activity->GetIsOpen()) return;

	if (m_param.answer_times >= config->GetOtherCfg().frequency) return;

	bool is_true = true;
	const std::vector<ItemConfigData>* reward_cfg = config->GetResultCfg(ramgr, m_param.cur_question_id, answer, is_true);
	if (NULL == reward_cfg && is_true) return;

	if (!is_true)	// 答错直接发新题目
	{
		this->RandQuestion();
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(*reward_cfg))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(*reward_cfg, PUT_REASON_RA_CHENG_FENG_PO_LANG);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&(*reward_cfg)[0], (int)reward_cfg->size(), SEND_MAIL_TYPE_CHENG_FENG_PO_LANG);
	}

	m_param.answer_times += 1;
	m_param.cur_question_id = 0;

	{	//  记录日志
		std::string log_str;
		for (int i = 0; i < (int)reward_cfg->size(); i++)
		{
			log_str += STRING_SPRINTF("[%d, %d] ", (*reward_cfg)[i].item_id, (*reward_cfg)[i].num);
		}

		gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] cur_answer_times[%d] rewards[%s]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.answer_times, log_str.c_str());
	}

	if (m_param.answer_times >= config->GetOtherCfg().frequency)
	{
		this->RASendInfo();
	}
	else
	{
		this->RandQuestion();
	}

	this->OnJoinActivity();
}

void RoleActivityChengFengPoLang::RASendInfo()
{
	Protocol::SCRAChengFengPoLangInfo msg;
	msg.answer_times = m_param.answer_times;
	msg.cur_question_id = m_param.cur_question_id;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void RoleActivityChengFengPoLang::OnJoinActivity()
{
	{
		RoleActivityYuanQiJiShiBu * role_activity = static_cast<RoleActivityYuanQiJiShiBu *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU));
		if (NULL != role_activity)
		{
			role_activity->OnCompleteTask(RA_YUAN_QI_JISHIBU_TASK_TYPE_CHENFENG_POLANG);
		}
	}
}
