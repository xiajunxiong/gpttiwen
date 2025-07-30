#include "schoolexercise.hpp"
#include "other/schoolexercise/schoolexerciseconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "global/team/team.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "battle/battle_manager_local.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "item/money.h"
#include "gamelog.h"
#include "protocol/msgschoolexercise.h"
#include "global/worldstatus/worldstatus.hpp"

SchoolExercise::SchoolExercise() : m_mgr(NULL)
{

}

SchoolExercise::~SchoolExercise()
{

}

void SchoolExercise::Init(RoleModuleManager* mgr, const SchoolExerciseParam& param)
{
	m_mgr = mgr;
	m_param = param;
}

void SchoolExercise::GetInitParam(ARG_OUT SchoolExerciseParam* param)
{
	if (NULL == param) return;

	*param = m_param;
}

void SchoolExercise::SendInfo()
{
	Protocol::SCSchoolExerciseInfo msg;
	msg.clear_flag = m_param.clear_flag;
	msg.has_open_ui_ever = (m_param.has_open_ui_ever > 0 ? 1 : 0);

	m_mgr->NetSend(&msg, sizeof(msg));
}

bool SchoolExercise::Challenge(int seq)
{
	if (!m_mgr->GetRole()->GetScene()->GetSpecialLogic()->CanJoinOtherActivity())
	{
		m_mgr->NoticeNum(errornum::EN_PLAY_ACTIVITY_NOW_ERR);
		return false;
	}
	const SchoolExerciseChallengeCfg* cfg = LOGIC_CONFIG->GetSchoolExerciseConfig()->GetChallengeCfg(seq);
	if (NULL == cfg)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (-1 != cfg->pre_seq)
	{
		const SchoolExerciseChallengeCfg* pre_cfg = LOGIC_CONFIG->GetSchoolExerciseConfig()->GetChallengeCfg(cfg->pre_seq);
		if (NULL == pre_cfg)
		{
			m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return false;
		}

		if (!m_param.IsClearSeq(pre_cfg->seq))
		{
			m_mgr->NoticeNum(errornum::EN_SCHOOL_EXERCISE_UNLOCK_PREVIOUS_LIMIT);
			return false;
		}
	}

	Team* team = m_mgr->GetRole()->GetMyTeam();
	if (NULL != team)
	{
		m_mgr->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return false;
	}

	if (!m_mgr->GetRole()->CanRoleStartFight(BATTLE_MODE_SCHOOL_EXERCISE))
	{
		m_mgr->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return false;
	}

	BattleData_Business business;
	business.Reset();
	business.param3 = seq;
	business.is_never_critical = 1;
	WorldStatus::Instance().LogFunctionStats(m_mgr->GetRole(), "SchoolTask");
	return BattleManagerLocal::GetInstance().StartDemoFightReq(m_mgr->GetRole(), cfg->npc_demo_seq, BATTLE_MODE_SCHOOL_EXERCISE, &business);
}

void SchoolExercise::OnChallengeSucc(int seq)
{
	const SchoolExerciseChallengeCfg* cfg = LOGIC_CONFIG->GetSchoolExerciseConfig()->GetChallengeCfg(seq);
	if (NULL == cfg)
	{
		return;
	}

	bool has_already_clear = m_param.IsClearSeq(seq);
	this->SendChallenSuccNotice(seq, has_already_clear ? 0 : 1);

	if (has_already_clear)
	{
		gamelog::g_log_school_exercise.printf(LL_INFO, "SchoolExercise::OnChallengeSucc Seq Clear No Reward, role[%d,%s] seq[%d] flag[%lld]",
			m_mgr->GetRoleUID(), m_mgr->GetRoleName(), seq, m_param.clear_flag);
		return;
	}

	m_param.SetSeqClear(seq);

	if (!m_mgr->GetMoney()->AddCoinBind(cfg->coin_reward, __FUNCTION__))
	{
		gamelog::g_log_school_exercise.printf(LL_WARNING, "SchoolExercise::OnChallengeSucc AddCoinError role[%d,%s] seq[%d] coin_reward[%d]",
			m_mgr->GetRoleUID(), m_mgr->GetRoleName(), seq, cfg->coin_reward);
	}

	if (!m_mgr->GetKnapsack()->PutList(cfg->item_reward_num, &cfg->item_reward_list[0], PUT_REASON_SCHOOL_EXERCISE))
	{
		gamelog::g_log_school_exercise.printf(LL_WARNING, "SchoolExercise::OnChallengeSucc KnapsackFull SendMail role[%d,%s] seq[%d]",
			m_mgr->GetRoleUID(), m_mgr->GetRoleName(), seq);
		m_mgr->GetKnapsack()->SendMail(cfg->item_reward_list, cfg->item_reward_num);
	}

	gamelog::g_log_school_exercise.printf(LL_INFO, "SchoolExercise::OnChallengeSucc role[%d,%s] seq[%d] flag[%lld]",
		m_mgr->GetRoleUID(), m_mgr->GetRoleName(), seq, m_param.clear_flag);

	this->SendInfo();
	
	m_mgr->GetRole()->LogActive(LOG_ACTIVE_TYPE_SCHOOL_EXERCISE);
}

void SchoolExercise::FirstOpenUI()
{
	if (0 == m_param.has_open_ui_ever)
	{
		m_param.has_open_ui_ever = 1;
		this->SendInfo();
	}
}

void SchoolExercise::SendChallenSuccNotice(int seq, int has_reward)
{
	Protocol::SCSchoolExerciseChallengeSucc msg;
	msg.seq = seq;
	msg.has_reward = has_reward;

	m_mgr->NetSend(&msg, sizeof(msg));
}

