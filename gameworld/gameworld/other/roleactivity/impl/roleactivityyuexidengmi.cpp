#include "roleactivityyuexidengmi.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityyuexidengmiconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "engineadapter.h"
#include "protocol/randactivity/msgrayuexidengmi.hpp"

RoleActivityYueXiDengMi::RoleActivityYueXiDengMi(int type) :RoleActivity(type)
{
}

RoleActivityYueXiDengMi::~RoleActivityYueXiDengMi()
{
}

void RoleActivityYueXiDengMi::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAYueXiDengMiParam));
}

void RoleActivityYueXiDengMi::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAYueXiDengMiParam));
}

void RoleActivityYueXiDengMi::OnLoginSendInfo()
{
	this->RASendInfo();
}

void RoleActivityYueXiDengMi::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.DayReset();
	this->RASendInfo();
}

void RoleActivityYueXiDengMi::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
		return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
		return;

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

void RoleActivityYueXiDengMi::OnRandActivityOpRequest(Protocol::CSRAYueXiDengMiRequest * raor)
{
	if (NULL == raor) return;

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

	switch (raor->op_type)
	{
	case Protocol::CSRAYueXiDengMiRequest::RA_YUE_XI_DENG_MI_REQ_TYPE_INFO:
		{
			this->RASendInfo();
		}
		break;
	case Protocol::CSRAYueXiDengMiRequest::RA_YUE_XI_DENG_MI_REQ_TYPE_START:
		{
			this->RandQuestion();
		}
		break;
	case Protocol::CSRAYueXiDengMiRequest::RA_YUE_XI_DENG_MI_REQ_TYPE_ANSWER:
		{
			this->Answer(raor->user_answer);
		}
		break;
	}
}

void RoleActivityYueXiDengMi::RandQuestion()
{
	const RandActivityYueXiDengMiConfig * config = static_cast<const RandActivityYueXiDengMiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	int new_question_id = config->RandQuestion();
	if (0 == new_question_id) return;

	int loop_count = 0;
	while (loop_count < 100)
	{
		loop_count++;


		new_question_id = config->RandQuestion();
		if (0 == new_question_id) continue;

		bool valid_question = true;
		for (int i = 0; i < ARRAY_LENGTH(m_param.used_question); i++)
		{
			if (new_question_id == m_param.used_question[i])
			{
				valid_question = false;
				break;
			}
		}

		if(valid_question) break;
	}

	// 有理由怀疑问题id表是空的
	if (0 == new_question_id) return;

	m_param.cur_question_id = new_question_id;

	this->RASendInfo();
	this->SetRoleDataChange();
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] Cur_Question[%d]",
		__FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.cur_question_id);
}

void RoleActivityYueXiDengMi::Answer(const char * user_answer)
{
	if (NULL == user_answer) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityYueXiDengMiConfig * config = static_cast<const RandActivityYueXiDengMiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	if (m_param.cur_question_id <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_param.today_answer_count >= config->GetOtherCfg().day_answer_limit)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_CHENG_FENG_PO_LANG_ANSWER_LIMIT);
		return;
	}

	bool is_true = false;
	if (config->IsCorrectAnswer(m_param.cur_question_id, user_answer))
	{
		const std::vector<ItemConfigData>* reward_list = config->GetRewardCfg(ramgr);
		if (NULL != reward_list)
		{
			if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(*reward_list))
			{
				m_role_module_mgr->GetKnapsack()->PutVec(*reward_list, PUT_REASON_YUE_XI_DENG_MI);
			}
			else
			{
				m_role_module_mgr->GetKnapsack()->SendMail(*reward_list);
			}
			m_role_module_mgr->GetKnapsack()->NotifyGetItemListVec(*reward_list, GET_REASON_YUE_XI_DENG_MI);

			std::string log_str;
			for (size_t i = 0; i < reward_list->size(); i++)
			{
				log_str = STRING_SPRINTF("[%d, %d] ", (*reward_list)[i].item_id, (*reward_list)[i].num);
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] Question_id[%d] today_answer_count[%d] reward_list{%s}",
				__FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.cur_question_id, m_param.today_answer_count, log_str.c_str());
		}

		is_true = true;
	}

	{
		Protocol::SCRAYueXiDengMiAnswerResult notice;
		notice.is_true = is_true ? 1 : 0;
		m_role_module_mgr->NetSend(&notice, sizeof(notice));
	}

	m_param.used_question[m_param.today_answer_count++] = m_param.cur_question_id;
	this->RandQuestion();

	this->RASendInfo();
	this->SetRoleDataChange();
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] Cur_Question_id[%d] today_answer_count[%d]",
		__FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.cur_question_id, m_param.today_answer_count);
}

void RoleActivityYueXiDengMi::RASendInfo()
{
	Protocol::SCRAYueXiDengMiInfo msg;
	msg.cur_question_id = m_param.cur_question_id;
	msg.today_answer_count = m_param.today_answer_count;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}
