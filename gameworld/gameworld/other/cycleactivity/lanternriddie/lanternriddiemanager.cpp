#include "lanternriddiemanager.hpp"
#include "gameworld/obj/character/role.h"
#include "servercommon/errornum.h"
#include "global/team/team.hpp"
#include "world.h"
#include "other/cycleactivity/lanternriddie/lanternriddieconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/cycleactivity/lanternriddie/lanternriddie.hpp"

LanernRiddieManager::LanernRiddieManager()
{
}

LanernRiddieManager::~LanernRiddieManager()
{
}

LanernRiddieManager & LanernRiddieManager::Instance()
{
	static LanernRiddieManager lr_manager;
	return lr_manager;
}

void LanernRiddieManager::CreateNPCQuestion(Role* role)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	if (!role->IsLeader())
	{
		return;
	}

	if (role->GetRoleModuleManager()->GetLanternRiddie()->GetAnswerCount() >= LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetOtherCfg().question_num)
	{
		return;
	}

	std::set<int> used_questions;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)		//统计队伍里用过的题
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) return;

		role_member->GetRoleModuleManager()->GetLanternRiddie()->GetUsedQuestion(used_questions);
	}

	int npc_seq = LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetRandNPCCfg(role->GetRoleModuleManager()->GetLanternRiddie()->GetTeamNpc());
	int question_id = LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->RandQuestionCfg(used_questions);

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetLanternRiddie()->SendNpcInfo(npc_seq, question_id);
	}
}

void LanernRiddieManager::StartAnswering(Role * role)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	if (!role->IsLeader())
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return;
	}

	bool is_all_follow = true;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		if(role_member->IsLeader()) continue;

		is_all_follow = is_all_follow && role_member->IsFollow();
	}
	if (!is_all_follow)
	{
		role->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
		return;
	}

	unsigned int answer_timestamp = (unsigned int)EngineAdapter::Instance().Time() + LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetOtherCfg().answer_time;

	//先清掉先前的数据然后发协议
	this->ClearMemberPastPraiseInfo(role);

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetLanternRiddie()->SendTimeOutInfo(answer_timestamp);
	}

	this->SendPraiseToTeamMember(role);		//客户端要求先发时间后发点赞信息
}

void LanernRiddieManager::Answer(RoleModuleManager * m_module_mgr, int question_id, int answer_id)
{
	if (NULL == m_module_mgr) return;

	if (m_module_mgr->GetLanternRiddie()->GetAnswerCount() >= LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetOtherCfg().question_num)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_NOT_LANTERN_IRDDIE_ANSWER_REWARD_COUNT);
	}

	if (m_module_mgr->GetLanternRiddie()->GetErrorAnswer() == answer_id)
	{
		m_module_mgr->NoticeNum(errornum::EN_NOT_LANTERN_IRDDIE_ERROR_ANSWER);
		return;
	}

	int result = CALRQuestionResultCfg::WRONG;
	const CALRQuestionResultCfg * reward_cfg = LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetAnswerResultCfg(question_id, answer_id, result);
	if (NULL == reward_cfg)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	Team* team = m_module_mgr->GetRole()->GetMyTeam();
	if (NULL == team) return;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetLanternRiddie()->AnswerReward((*reward_cfg), m_module_mgr->GetUid(), answer_id, result);
	}

}

void LanernRiddieManager::SynCardUse(Role * role)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetLanternRiddie()->SetSpecialCard();
	}
}

void LanernRiddieManager::SynQuestion(Role * role, int question_id)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetLanternRiddie()->SetQuestion(question_id);
	}
}

void LanernRiddieManager::SynTimeOut(Role * role, unsigned int timeout_timestamp)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetLanternRiddie()->SendTimeOutInfo(timeout_timestamp);
	}
}

void LanernRiddieManager::SynErrorAnswer(Role * role, int error_answer)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetLanternRiddie()->SetErrorAnswer(error_answer);
	}
}

void LanernRiddieManager::ClearMemberPastPraiseInfo(Role * role)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetLanternRiddie()->ClearPastQuestionInfo();
	}
}

void LanernRiddieManager::SendInfoToTeamMember(Role * role, const char * info, int len)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		EngineAdapter::Instance().NetSend(role_member->GetNetId(), info, len);
	}
}

void LanernRiddieManager::SendPraiseToTeamMember(Role * role)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	Protocol::SCLanternRiddiePraiseInfo praise_info;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetLanternRiddie()->AssemblePraiseInfo(praise_info.user_info[i]);
	}

	this->SendInfoToTeamMember(role, (const char*)&praise_info, sizeof(praise_info));
}

void LanernRiddieManager::SendLastInfo(Role * role)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	Protocol::SCLanternRiddieLastResultInfo result_info;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		result_info.last_result[i].Reset();

		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetLanternRiddie()->AssembleLastResultInfo(result_info.last_result[i]);
	}

	this->SendInfoToTeamMember(role, (const char*)&result_info, sizeof(result_info));
}


