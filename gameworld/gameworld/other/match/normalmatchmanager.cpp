#include "normalmatchmanager.hpp"
#include "servercommon/matchmanager/matchmanager.hpp"
#include "servercommon/servercommon.h"
#include "global/team/team.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/capability/capability.hpp"
#include "protocol/msgmatch.h"
#include "servercommon/matchmanager/matchdef.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "servercommon/matchmanager/matchengine.hpp"

NormalMatchMgr::NormalMatchMgr() 
{

}

NormalMatchMgr::~NormalMatchMgr()
{

}

void NormalMatchMgr::Init(int team_member_num, IMatchLogic* logic, int business_type)
{
	MatchManagerInitParam init_param;
	init_param.full_team_match_interval_ms = 3000;
	init_param.unfull_team_match_interval_ms = 3000;
	init_param.business_type = business_type;
	init_param.team_member_num = team_member_num;
	init_param.logic_ptr = logic;

	this->MatchManager::Init(new MatchEngine(), init_param);
}

void NormalMatchMgr::RoleStartMatchReq(Role* role)
{
	static MatchUser* mul_list[MAX_TEAM_MEMBER_NUM];
	int match_member_num = 0;

	Team* team = role->GetMyTeam();
	if (NULL == team)
	{
		mul_list[0] = MatchUser::Create();
		mul_list[0]->uid = role->GetUID();
		mul_list[0]->profession = role->GetProfession();
		mul_list[0]->level = role->GetLevel();
		mul_list[0]->avatar_type = role->GetAvatarType();
		mul_list[0]->headshot_id = role->GetHeadshotID();
		F_STRNCPY(mul_list[0]->user_name, role->GetName(), sizeof(GameName));

		mul_list[0]->user_info.mulb.level = role->GetLevel();
		mul_list[0]->user_info.mulb.capability = role->GetRoleModuleManager()->GetCapability()->GetRoleCapability();
		
		match_member_num = 1;
	}
	else
	{
		for (int i = 0; i < team->GetMemberCount(); ++i)
		{
			TeamMember* member = team->GetMemberInfoByIndex(i);
			if (NULL == member || !member->is_follow || member->IsRobot()) continue;

			Role* member_role = team->GetMemberRoleByIndex(i);
			if (NULL == member_role) continue;

			mul_list[match_member_num] = MatchUser::Create();
			mul_list[match_member_num]->uid = member_role->GetUID();
			mul_list[match_member_num]->profession = member_role->GetProfession();
			mul_list[match_member_num]->level = member_role->GetLevel();
			mul_list[match_member_num]->avatar_type = member_role->GetAvatarType();
			mul_list[match_member_num]->headshot_id = member_role->GetHeadshotID();
			F_STRNCPY(mul_list[match_member_num]->user_name, member_role->GetName(), sizeof(GameName));

			mul_list[match_member_num]->user_info.mulb.level = member_role->GetLevel();
			mul_list[match_member_num]->user_info.mulb.capability = member_role->GetRoleModuleManager()->GetCapability()->GetRoleCapability();

			match_member_num += 1;
		}
	}

	if (match_member_num > 0)
	{
		this->MatchManager::OnUserStartMatchReq(match_member_num, 0, mul_list);
	}
}

void NormalMatchMgr::RoleStopMatchReq(Role* role)
{
	static int uid_list[MAX_TEAM_MEMBER_NUM];
	int match_member_num = 0;

	Team* team = role->GetMyTeam();
	if (NULL == team)
	{
		uid_list[0] = role->GetUID();
		match_member_num = 1;
	}
	else
	{
		for (int i = 0; i < team->GetMemberCount(); ++i)
		{
			TeamMember* member = team->GetMemberInfoByIndex(i);
			if (NULL == member || member->IsRobot()) continue;
			/*Role* role = team->GetMemberRoleByIndex(i);
			if (NULL == role) continue;*/
			uid_list[match_member_num++] = member->uid;
		}
	}

	if (match_member_num > 0)
	{
		this->MatchManager::OnUserStopMatchReq(match_member_num, uid_list);
	}
}

void NormalMatchMgr::OnTeamStartMatch(MatchTeam* mt)
{
#ifdef TEST_MODE
	printf("###############################\n");
	for (size_t i = 0; i < mt->user_list.size(); ++i)
	{
		printf("OnTeamStartMatch team_id[%lld] uid[%d]\n", mt->team_id, mt->user_list[i]->uid);
	}
	printf("--------------------------------\n\n");
#endif

	this->SendTeamStartMatchNotice(mt);
}

void NormalMatchMgr::OnTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB)
{
#ifdef TEST_MODE
	printf("###############################\n");
	printf("OnTeamMatchTeam  teamA[%lld]  teamB[%lld]\n", mtA->team_id, mtB->team_id);
	for (size_t i = 0; i < mtA->user_list.size(); ++i)
	{
		printf("AAAA: uid[%d]\n", mtA->user_list[i]->uid);
	}
	printf("---\n");
	for (size_t i = 0; i < mtB->user_list.size(); ++i)
	{
		printf("BBBB: uid[%d]\n", mtB->user_list[i]->uid);
	}
	printf("--------------------------------\n\n");
#endif

	this->SendTeamMatchTeamNotice(mtA, mtB);
}

void NormalMatchMgr::OnTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB)
{
#ifdef TEST_MODE
	printf("###############################\n");
	printf("OnTeamMatchEnemy  teamA[%lld]  teamB[%lld]\n", mtA->team_id, mtB->team_id);
	for (size_t i = 0; i < mtA->user_list.size(); ++i)
	{
		printf("AAAA: uid[%d]\n", mtA->user_list[i]->uid);
	}
	printf("---\n");
	for (size_t i = 0; i < mtB->user_list.size(); ++i)
	{
		printf("BBBB: uid[%d]\n", mtB->user_list[i]->uid);
	}
	printf("--------------------------------\n\n");
#endif

	this->SendTeamMatchEnemyNotice(mtA, mtB);
}

void NormalMatchMgr::OnUserLeaveTeam(MatchTeam* mt, int uid)
{
#ifdef TEST_MODE
	printf("###############################\n");
	printf("OnUserLeaveTeam team_id[%lld] leave_uid[%d]\n", mt->team_id, uid);
	for (size_t i = 0; i < mt->user_list.size(); ++i)
	{
		printf("remain uid[%d]\n", mt->user_list[i]->uid);
	}
	printf("--------------------------------\n\n");
#endif

	this->SendUserLeaveTeamNotice(mt, uid);
}

void NormalMatchMgr::SendTeamStartMatchNotice(MatchTeam* mt)
{
	Protocol::SCStartMatch msg;
	msg.user_num = 0;
	msg.business_type = m_engine->GetMatchLogicType();

	for (size_t i = 0; i < mt->user_list.size(); ++i)
	{
		msg.user_list[msg.user_num].uid = mt->user_list[i]->uid;
		msg.user_list[msg.user_num].profession = mt->user_list[i]->profession;
		msg.user_list[msg.user_num].level = mt->user_list[i]->level;
		msg.user_list[msg.user_num].avatar_type = mt->user_list[i]->avatar_type;
		msg.user_list[msg.user_num].headshot_id = mt->user_list[i]->headshot_id;
		msg.user_list[msg.user_num].reserve_sh = 0;
		F_STRNCPY(msg.user_list[msg.user_num].name, mt->user_list[i]->user_name, sizeof(GameName));

		msg.user_num += 1;
	}

	int send_len = sizeof(msg) - sizeof(msg.user_list) + msg.user_num * sizeof(msg.user_list[0]);
	for (size_t i = 0; i < mt->user_list.size(); ++i)
	{
		World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mt->user_list[i]->uid), (const char*)&msg, send_len);
	}
}

void NormalMatchMgr::SendTeamMatchTeamNotice(MatchTeam* mtA, MatchTeam* mtB)
{
	Protocol::SCMatchInfoUpdate msg;
#ifdef _DEBUG
	assert(mtA->user_list.size() + mtB->user_list.size() <= MSG_MATCH_USER_NUM);
#endif

	msg.user_num = 0;
	msg.business_type = m_engine->GetMatchLogicType();

	for (size_t i = 0; i < mtA->user_list.size() && msg.user_num < MSG_MATCH_USER_NUM; ++i)
	{
		msg.user_list[msg.user_num].uid = mtA->user_list[i]->uid;
		msg.user_list[msg.user_num].profession = mtA->user_list[i]->profession;
		msg.user_list[msg.user_num].level = mtA->user_list[i]->level;
		msg.user_list[msg.user_num].avatar_type = mtA->user_list[i]->avatar_type;
		msg.user_list[msg.user_num].headshot_id = mtA->user_list[i]->headshot_id;
		msg.user_list[msg.user_num].reserve_sh = 0;
		F_STRNCPY(msg.user_list[msg.user_num].name, mtA->user_list[i]->user_name, sizeof(GameName));

		msg.user_num += 1;
	}

	for (size_t i = 0; i < mtB->user_list.size() && msg.user_num < MSG_MATCH_USER_NUM; ++i)
	{
		msg.user_list[msg.user_num].uid = mtB->user_list[i]->uid;
		msg.user_list[msg.user_num].profession = mtB->user_list[i]->profession;
		msg.user_list[msg.user_num].level = mtB->user_list[i]->level;
		msg.user_list[msg.user_num].avatar_type = mtB->user_list[i]->avatar_type;
		msg.user_list[msg.user_num].headshot_id = mtB->user_list[i]->headshot_id;
		msg.user_list[msg.user_num].reserve_sh = 0;
		F_STRNCPY(msg.user_list[msg.user_num].name, mtB->user_list[i]->user_name, sizeof(GameName));

		msg.user_num += 1;
	}

	int send_len = sizeof(msg) - sizeof(msg.user_list) + msg.user_num * sizeof(msg.user_list[0]);
	for (size_t i = 0; i < mtA->user_list.size(); ++i)
	{
		World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mtA->user_list[i]->uid), (const char*)&msg, send_len);
	}

	for (size_t i = 0; i < mtB->user_list.size(); ++i)
	{
		World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mtB->user_list[i]->uid), (const char*)&msg, send_len);
	}
}

void NormalMatchMgr::SendTeamMatchEnemyNotice(MatchTeam* mtA, MatchTeam* mtB)
{
	Protocol::SCMatchEnemy msg;
	msg.business_type = m_engine->GetMatchLogicType();
	msg.reserve_sh = 0;
	msg.left_user_num = 0;
	msg.right_user_num = 0;

	for (size_t i = 0; i < mtA->user_list.size(); ++i)
	{
		msg.left_user_list[msg.left_user_num].uid = mtA->user_list[i]->uid;
		msg.left_user_list[msg.left_user_num].profession = mtA->user_list[i]->profession;
		msg.left_user_list[msg.left_user_num].level = mtA->user_list[i]->level;
		msg.left_user_list[msg.left_user_num].avatar_type = mtA->user_list[i]->avatar_type;
		msg.left_user_list[msg.left_user_num].headshot_id = mtA->user_list[i]->headshot_id;
		msg.left_user_list[msg.left_user_num].reserve_sh = 0;
		F_STRNCPY(msg.left_user_list[msg.left_user_num].name, mtA->user_list[i]->user_name, sizeof(GameName));

		msg.left_user_num += 1;
	}

	for (size_t i = 0; i < mtB->user_list.size(); ++i)
	{
		msg.right_user_list[msg.right_user_num].uid = mtB->user_list[i]->uid;
		msg.right_user_list[msg.right_user_num].profession = mtB->user_list[i]->profession;
		msg.right_user_list[msg.right_user_num].level = mtB->user_list[i]->level;
		msg.right_user_list[msg.right_user_num].avatar_type = mtB->user_list[i]->avatar_type;
		msg.right_user_list[msg.right_user_num].headshot_id = mtB->user_list[i]->headshot_id;
		msg.right_user_list[msg.right_user_num].reserve_sh = 0;
		F_STRNCPY(msg.right_user_list[msg.right_user_num].name, mtB->user_list[i]->user_name, sizeof(GameName));

		msg.right_user_num += 1;
	}

	static char buffer[65535];
	int send_len = 0;
	if (!msg.Serialize(buffer, sizeof(buffer), &send_len))
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}
	for (size_t i = 0; i < mtA->user_list.size(); ++i)
	{
		World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mtA->user_list[i]->uid), buffer, send_len);
	}

	for (size_t i = 0; i < mtB->user_list.size(); ++i)
	{
		World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mtB->user_list[i]->uid), buffer, send_len);
	}
}

void NormalMatchMgr::SendUserLeaveTeamNotice(MatchTeam* mt, int uid)
{
	{
		Protocol::SCMatchInfoUpdate msg;
		msg.business_type = m_engine->GetMatchLogicType();
		msg.user_num = 0;
		MsgMatchUserInfo leave_role_info;
		for (size_t i = 0; i < mt->user_list.size(); ++i)
		{
			if (mt->user_list[i]->uid != uid)
			{
				msg.user_list[msg.user_num].uid = mt->user_list[i]->uid;
				msg.user_list[msg.user_num].profession = mt->user_list[i]->profession;
				msg.user_list[msg.user_num].level = mt->user_list[i]->level;
				msg.user_list[msg.user_num].avatar_type = mt->user_list[i]->avatar_type;
				msg.user_list[msg.user_num].headshot_id = mt->user_list[i]->headshot_id;
				msg.user_list[msg.user_num].reserve_sh = 0;
				F_STRNCPY(msg.user_list[msg.user_num].name, mt->user_list[i]->user_name, sizeof(GameName));

				msg.user_num += 1;
			}
			else
			{
				leave_role_info.uid = mt->user_list[i]->uid;
				leave_role_info.profession = mt->user_list[i]->profession;
				leave_role_info.level = mt->user_list[i]->level;
				leave_role_info.avatar_type = mt->user_list[i]->avatar_type;
				leave_role_info.headshot_id = mt->user_list[i]->headshot_id;
				leave_role_info.reserve_sh = 0;
				F_STRNCPY(leave_role_info.name, mt->user_list[i]->user_name, sizeof(GameName));
			}
		}
		//发给原先仍在匹配的玩家
		int send_len = sizeof(msg) - sizeof(msg.user_list) + msg.user_num * sizeof(msg.user_list[0]);
		for (size_t i = 0; i < mt->user_list.size(); ++i)
		{
			if (mt->user_list[i]->uid != uid)
			{
				World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mt->user_list[i]->uid), (const char*)&msg, send_len);
			}
		}
		//发给已经离开匹配的玩家
		if (leave_role_info.uid == uid)
		{
			memset(msg.user_list, 0, sizeof(msg.user_list));
			msg.user_num = 1;
			msg.user_list[0] = leave_role_info;
			send_len = sizeof(msg) - sizeof(msg.user_list) + msg.user_num * sizeof(msg.user_list[0]);
			World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(leave_role_info.uid), (const char*)&msg, send_len);
		}


	}

	{
		Protocol::SCStopMatch msg;
		msg.business_type = m_engine->GetMatchLogicType();
		msg.reserve_sh = 0;
		World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(uid), (const char*)&msg, sizeof(msg));
	}
}

