#include "kejuexammanager.hpp"
#include "gameworld/obj/character/role.h"
#include "servercommon/errornum.h"
#include "global/team/team.hpp"
#include "world.h"
#include "other/cycleactivity/kejuexam/kejuexamconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/cycleactivity/kejuexam/kejuexam.hpp"

KeJuExamManager::KeJuExamManager()
{
}

KeJuExamManager::~KeJuExamManager()
{
}

KeJuExamManager & KeJuExamManager::Instance()
{
	static KeJuExamManager kj_manager;
	return kj_manager;
}

void KeJuExamManager::CreateNPCQuestion(Role * role)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	if (!role->IsLeader()) return;

	if (role->GetRoleModuleManager()->GetKeJuExam()->GetAnswerCount() >= LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetOtherCfg().question_num)
	{
		return;
	}

	std::set<int> used_questions;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)		//统计队伍里用过的题
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetKeJuExam()->GetUsedQuestion(used_questions);
	}

	int npc_seq = LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetRandNPCCfg(role->GetRoleModuleManager()->GetKeJuExam()->GetCurTeamNpc());
	int question_id = LOGIC_CONFIG->GetCycleKeJuExamConfig()->RandQuestionCfg(used_questions);

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetKeJuExam()->SetNpcQuestion(npc_seq, question_id);
	}
}

void KeJuExamManager::StartAnswering(Role * role)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	if (!role->IsLeader())
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return;
	}

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		if (role_member->IsLeader()) continue;

		if (!role_member->IsFollow())
		{
			role->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
			return;
		}
	}

	// 先清掉先前的数据然后发协议
	this->ClearMemberPastPraiseInfo(role);

	// 根据配置选择一个玩家答题
	std::pair<int, int> group_order = role->GetRoleModuleManager()->GetKeJuExam()->GetGroupOrder();
	int role_position = 0;

	int answer_uid = 0;
	while (0 == answer_uid)
	{
		role_position = LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetOrderCfg(group_order.first, group_order.second);

		TeamMember * member = team->GetMemberInfo(role_position - 1);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		answer_uid = role_member->GetUID(); break;
	}

	unsigned int answer_timestamp = (unsigned int)EngineAdapter::Instance().Time() + LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetOtherCfg().answer_time;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetKeJuExam()->SetGroupOrder(group_order);
		role_member->GetRoleModuleManager()->GetKeJuExam()->SetTimeOut(answer_uid, answer_timestamp);
	}

	this->SendPraiseToTeamMember(role);		//客户端要求先发时间后发点赞信息
}

void KeJuExamManager::Answer(RoleModuleManager * m_module_mgr, int question_id, int answer_id)
{
	if (NULL == m_module_mgr) return;

	if (m_module_mgr->GetKeJuExam()->GetAnswerCount() >= LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetOtherCfg().question_num)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_NOT_LANTERN_IRDDIE_ANSWER_REWARD_COUNT);
	}

	if (m_module_mgr->GetKeJuExam()->GetTeamReduceErrorCard() == answer_id)
	{
		m_module_mgr->NoticeNum(errornum::EN_NOT_LANTERN_IRDDIE_ERROR_ANSWER);
		return;
	}

	int result = KeJuExamQuestionResultCfg::WRONG;
	const KeJuExamQuestionResultCfg * reward_cfg = LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetAnswerResultCfg(question_id, answer_id, result);
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

		role_member->GetRoleModuleManager()->GetKeJuExam()->AnswerReward((*reward_cfg), answer_id, result);
	}
}

void KeJuExamManager::SynCardUse(Role * role)
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

		role_member->GetRoleModuleManager()->GetKeJuExam()->SetTeamUsedCard();
	}
}

void KeJuExamManager::SynQuestion(Role * role, int question_id)
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

		role_member->GetRoleModuleManager()->GetKeJuExam()->SetQuestion(question_id);
	}
}

void KeJuExamManager::SynTimeOut(Role * role, unsigned int timeout_timestamp)
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

		role_member->GetRoleModuleManager()->GetKeJuExam()->SetTimeOut(role_member->GetRoleModuleManager()->GetKeJuExam()->GetAnswerUid(), timeout_timestamp);
	}
}

void KeJuExamManager::SynErrorAnswer(Role * role, int error_answer)
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

		role_member->GetRoleModuleManager()->GetKeJuExam()->SetErrorAnswer(error_answer);
	}
}

void KeJuExamManager::ClearMemberPastPraiseInfo(Role * role)
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

		role_member->GetRoleModuleManager()->GetKeJuExam()->ClearPastQuestionInfo();
	}
}

void KeJuExamManager::SendInfoToTeamMember(Role * role, const char * info, int len)
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

void KeJuExamManager::SendPraiseToTeamMember(Role * role)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	Protocol::SCKeJuExamPraiseInfo praise_info;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		praise_info.user_info[i].Reset();

		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetKeJuExam()->AssemblePraiseInfo(praise_info.user_info[i]);
	}

	this->SendInfoToTeamMember(role, (const char*)&praise_info, sizeof(praise_info));
}

void KeJuExamManager::SendPraiseToSpecificMember(Role * role, int specific_uid, int word_id)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	Protocol::SCKeJuExamPraiseInfo praise_info;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		praise_info.user_info[i].Reset();

		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetKeJuExam()->AssemblePraiseInfo(praise_info.user_info[i]);
	}

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		if (role->GetUID() == role_member->GetUID() || role_member->GetUID() == specific_uid)
		{		
			if (role->GetUID() == role_member->GetUID())	// 玩家本身做特殊显示,显示对方头顶也回赞
			{
				Protocol::SCKeJuExamPraiseInfo temp_praise_info = praise_info;

				for (int j = 0; j < ARRAY_LENGTH(temp_praise_info.user_info); j++)
				{
					if (temp_praise_info.user_info[j].uid == specific_uid)
					{
						temp_praise_info.user_info[j].word_id = word_id;
					}
				}

				EngineAdapter::Instance().NetSend(role_member->GetNetId(), (const void *)&temp_praise_info, sizeof(temp_praise_info));
				continue;
			}

			EngineAdapter::Instance().NetSend(role_member->GetNetId(), (const void *)&praise_info, sizeof(praise_info));
		}
	}
}

void KeJuExamManager::SendLastInfo(Role * role)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	Protocol::SCKeJuExamLastResultInfo result_info;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		result_info.last_result[i].Reset();

		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetKeJuExam()->AssembleLastResultInfo(result_info.last_result[i]);
	}

	this->SendInfoToTeamMember(role, (const char*)&result_info, sizeof(result_info));
}
