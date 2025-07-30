#include "teammanager.hpp"
#include "team.hpp"
#include "obj/character/role.h"
#include "protocol/msgteam.h"
#include "engineadapter.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/rolecross/rolecross.hpp"
#include "other/rolemodulemanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "internalcomm.h"
#include "global/team/teammatch.hpp"
#include "chat/chatmanager.h"
#include "obj/character/robot.h"
#include "config/logicconfigmanager.hpp"
#include "teamconfig.hpp"
#include "gamelog.h"
#include "global/usercache/usercache.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activityguildfight.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "other/salary/salary.hpp"
#include "global/jieyi/jieyizumanager.h"
#include "global/qingyuan/qingyuanmanager.h"
#include "global/team/teamrecruit.hpp"

bool TeamManager::TeamGotoScene(Role * role, int target_scene_id, int target_scene_key, Posi target_pos, bool is_set_target_pos)
{
	return false;
}

void TeamManager::TeamGotoScene(Team * team, int target_scene_id, int target_scene_key, Posi target_pos, bool is_set_target_pos, TeamMemberGoToFunction func, void * func_param)
{
	if (team == NULL)
	{
		return;
	}

	Role * leader = team->GetLeaderRole();
	if (leader == NULL)
	{
		return;
	}

	if (!leader->GetRoleStatusManager()->CanTransportTo(target_scene_id))
	{
		return;
	}

	std::vector<Robot *> robot_list;

	int leader_scene_type = leader->GetScene()->GetSceneType();
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * member_info = team->GetMemberInfo(i);
		if (member_info == NULL)
		{
			continue;
		}

		if (member_info->IsRobot())
		{
			Robot * robot = team->GetMemberRobotByIndex(i);
			if (NULL == robot) continue;

			robot_list.push_back(robot);
		}
		else
		{
			int role_id = member_info->uid;
			Role * member_role = team->GetMemberRoleByIndex(i);
			if (member_role == NULL)
			{
				if (func != NULL)
				{
					func(role_id, target_scene_id, target_scene_key, false, func_param);
				}

				continue;
			}

			if (member_role->GetScene()->GetSceneType() != leader_scene_type)
			{
				if (func != NULL)
				{
					func(role_id, target_scene_id, target_scene_key, false, func_param);
				}

				continue;
			}

			bool go_ret = World::GetInstWorld()->GetSceneManager()->GoTo(member_role, target_scene_id, target_scene_key, target_pos, is_set_target_pos);

			if (func != NULL)
			{
				func(role_id, target_scene_id, target_scene_key, go_ret, func_param);
			}
		}
	}

	for (int i = 0; i < (int)robot_list.size(); ++i)
	{
		Robot * robot = robot_list[i];
		World::GetInstWorld()->GetSceneManager()->GoTo(robot, target_scene_id, target_scene_key, target_pos);
	}
}

void TeamManager::TeamGotoSceneByTeamIndex(int team_index, int target_scene_id, int target_scene_key, Posi target_pos, bool is_set_target_pos, TeamMemberGoToFunction func, void * func_param)
{
	Team * team = GetTeamByTeamIndex(team_index);
	this->TeamGotoScene(team, target_scene_id, target_scene_key, target_pos, is_set_target_pos, func, func_param);
}

void TeamManager::SyncCrossTeamInfo(Team* team, int sync_reason)
{
	if (NULL == team) return;

	static crossgameprotocal::HiddenGameSyncCrossTeamInfo msg;
	msg.notify_reason = sync_reason;
	msg.team_count = 1;
	msg.team_list[0].Reset();
	if (crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_REMOVE == sync_reason)
	{
		msg.team_list[0].team_index = team->GetTeamIndex();
		msg.team_list[0].create_team_timestamp = team->GetTeamCreateTime();
	}
	else
	{
		team->GetTeamItemInfo(&msg.team_list[0]);
	}
	
	int send_len = sizeof(msg) - sizeof(msg.team_list) + msg.team_count * sizeof(msg.team_list[0]);
	InternalComm::Instance().SendToGameThroughCross(0, 0, (const char*)&msg, send_len);
}

TeamManager::TeamManager() : m_team_index(0)
{
}

TeamManager::~TeamManager()
{
	for (TeamList::iterator i = m_team_list.begin(); i != m_team_list.end(); ++i)
	{
		delete i->second;
	}

	m_team_list.clear();
}

void TeamManager::OnTeamCreate(Team* team, bool is_start_match)
{
	team->OnTeamCreate();

	if (is_start_match)
	{
		TeamMatch::Instance().OnTeamStartMatch(team);
	}


	Role* leader = team->GetLeaderRole();
	if (NULL != leader && leader->GetScene()->IsInStaticScene())
	{
		this->GetTeamChannel()->OnTeamCreate(team->GetTeamIndex(), team->GetIsCross(), team->GetTeamCreateTime(), team);
		if (CrossConfig::Instance().IsHiddenServer())
		{
			TeamManager::Instance().SyncCrossTeamInfo(team, crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_ADD);
		}
	}
}

void TeamManager::OnTeamDismiss(Team* team)
{
	this->GetTeamChannel()->OnTeamDismiss(team->GetTeamIndex(), team->GetIsCross(), team->GetTeamCreateTime(), team);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		TeamManager::Instance().SyncCrossTeamInfo(team, crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_REMOVE);
	}
}

void TeamManager::SendErrorNumToOriginServer(int origin_uid, int origin_plat_type, int origin_server_id, int errnum)
{
	static Protocol::SCNoticeNum msg;
	msg.notice_num = errnum;

	InternalComm::Instance().SendToGameThroughCross(origin_plat_type, origin_server_id, (const char*)&msg, sizeof(msg), origin_uid);
}

TeamManager* gTeamMgr = NULL;
TeamManager & TeamManager::Instance()
{
	if (NULL == gTeamMgr)
	{
		gTeamMgr = new TeamManager();
	}
	return *gTeamMgr;
}

void TeamManager::Update(unsigned long interval, time_t now_second)
{
	for (TeamListIt iter = m_delay_delete_list.begin(); iter != m_delay_delete_list.end(); ++iter)
	{
#ifdef _DEBUG
		//gamelog::g_log_world.printf(LL_MAINTANCE, "TeamManager::Update delay delete team_idx[%d] team_addr[%d]", iter->second->GetTeamIndex(), iter->second);
#endif

		delete iter->second;
	}
	m_delay_delete_list.clear();

	for (TeamListIt iter = m_team_list.begin(); iter != m_team_list.end(); ++iter)
	{
#ifdef _DEBUG
		//gamelog::g_log_world.printf(LL_MAINTANCE, "TeamManager::Update m_team_index[%d] team_idx[%d] team_addr[%d]", m_team_index, iter->second->GetTeamIndex(), iter->second);
#endif

		iter->second->Update(interval, now_second);
	}
}

Team * TeamManager::CreateTeam(Role * leader, short limit_level_low /* = 0 */, short limit_level_high /* = MAX_ROLE_LEVEL */, int team_type /* = TEAM_TYPE_DEFAULT */, bool notice_flag /* = true */, bool is_match /* = false */, bool allow_red_join /* = true */)
{
	if (NULL == leader) return NULL;
	if (!leader->CanJoinTeam() || !leader->GetScene()->GetSpecialLogic()->CanEnterTeam(leader, NULL, true))
	{
		//leader->NoticeNum(errornum::EN_NOW_STATUS_UN_JOIN_TEAM);
		return NULL;
	}
	if (leader->GetRoleStatusManager()->IsInBattleStatus())
	{
		leader->NoticeNum(errornum::EN_ROLE_IN_BATTLE);
		return NULL;
	}
	if (NULL != leader->GetMyTeam())
	{
		leader->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return NULL;
	}
	Scene* leader_scene = leader->GetScene();
	if (NULL != leader_scene)
	{
		if (leader_scene->GetSceneType() == Scene::SCENE_TYPE_GUILD_FIGHT)
		{
			if (team_type != 165)
			{
				leader->NoticeNum(errornum::EN_TEAM_CREATE_SCENE_ERR);
				return NULL;
			}
		}
	}

	const TeamTypeCfg* team_type_cfg = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeCfg(team_type);
	if (NULL == team_type_cfg)
	{
		leader->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return NULL;
	}
	if (team_type != TEAM_TYPE_DEFAULT &&
		(limit_level_low < team_type_cfg->low_level_limit || 
		limit_level_low > team_type_cfg->high_level_limit ||
		limit_level_high < team_type_cfg->low_level_limit || 
		limit_level_high > team_type_cfg->high_level_limit))
	{
		leader->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return NULL;
	}

	if (team_type == TEAM_TYPE_GUILD_FIGHT)
	{
		if (leader->GetGuild() == NULL)
		{
			leader->NoticeNum(errornum::EN_GUILD_NOT_IN_GUILD);
			return NULL;
		}
	}

	int count = 0;
	do
	{
		if (++m_team_index >= TEAM_INDEX_MAX) { m_team_index = 1; }
		if (++count > 100) return NULL;

	} while (m_team_list.end() != m_team_list.find(m_team_index));


	Team *team = new Team();
	m_team_list[m_team_index] = team;

	team->SetTeamIndex(m_team_index);
	team->SetTeamLimit(limit_level_low, limit_level_high);
	team->SetTeamType(team_type);
	team->SetTeamAllowRedJoin(allow_red_join);
	team->SetIsCross(CrossConfig::Instance().IsHiddenServer());
	team->SetNotice(notice_flag, notice_flag ? 0 : Team::AUTO_JOIN_LIMIT_TIME);

	team->AddMember(leader, true);
	team->AutoSetLeader(true);
	team->FillPosList();

	team->SendTeamInfo();

	this->OnTeamCreate(team, is_match);

#ifdef _DEBUG
	gamelog::g_log_world.printf(LL_MAINTANCE, "TeamManager::CreateTeam m_team_index[%d] team_idx[%d] team_addr[%p]", m_team_index, team->GetTeamIndex(), team);
#endif
	return team;
}

int TeamManager::DismissTeam(Role * user)
{
	if (NULL == user) return TOR_UNKNOWN_ERROR;

	Team* team = this->GetTeam(user);
	if (NULL == team)
	{
		return TOR_TEAM_NOT_EXIST;
	}

	// 只有队长能解散队伍
	if (!team->IsLeader(user)) return TOR_TEAM_NOT_LEADER;

	team->DismissTeamHelper();
	return 0;
}

int TeamManager::ExitTeam(Role * user)
{
	if (NULL == user) return TOR_UNKNOWN_ERROR;
	
	Team* team = this->GetTeam(user);
	if (NULL == team)
	{
		return TOR_TEAM_NOT_EXIST;
	}

	TeamMember* member = team->GetMemberInfoByUID(user->GetUID());
	if (NULL == member)
	{
		return TOR_TEAM_NOT_MEMBER;
	}

	// 如果是最后一个人离队了 或者没有跟随的人...清除队伍信息 否则发送一次队伍信息
	if (team->GetMemberCount() <= 1 || (team->GetFollowMemberCount() <= 1 && member->IsFollow()))
	{
		team->DismissTeamHelper();

		static Protocol::SCTeamLeaderChangeNoticeArea msg;
		msg.change_objid = user->GetId();
		msg.is_leader = 0;
		user->GetScene()->GetZoneMatrix()->NotifyAreaMsg(user->GetObserHandle(), &msg, sizeof(msg));
	}
	else
	{
		// 离队
		team->LeaveTeam(user->GetUserId());
	}

	return 0;
}

void TeamManager::ExitTeam2(int uid)
{
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	if (role != NULL)
	{
		this->ExitTeam(role);
		return;
	}

	Team * team = this->GetTeamByUid(uid);
	if (team != NULL)
	{
		team->LeaveTeamByUid(uid);
	}
}

int TeamManager::KickOutMember(Role * user, int kick_index)
{
	if (NULL == user) return TOR_UNKNOWN_ERROR;

	Team* team = this->GetTeam(user);
	if (NULL == team)
	{
		return TOR_TEAM_NOT_EXIST;
	}

	if (!team->IsLeader(user)) return TOR_TEAM_NOT_LEADER;

	TeamMember* kick_member = team->GetMemberInfoByIndex(kick_index);
	if (NULL == kick_member) return TOR_TEAM_NOT_MEMBER;
	if (kick_member->uid == user->GetUID())
	{
		this->ExitTeam(user);
		return 0;
	}

	{
		Role* kick_role = World::GetInstWorld()->GetSceneManager()->GetRole(kick_member->uid);
		if (NULL != kick_role)
		{
			//发被踢通知
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_team_be_kicked);
			if (length > 0)
			{
				static Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT)))
				{	
					EngineAdapter::Instance().NetSend(kick_role->GetNetId(), (const char*)&sm, sendlen);			
				}
			}
		}
	}

	team->LeaveTeam(kick_index);
	return 0;
}

int TeamManager::JoinTeam(Role * user, int team_index, bool is_apply_cross)
{
	if (NULL == user) return TOR_TEAM_NOT_MEMBER;
	if (!user->CanJoinTeam())
	{
		//user->NoticeNum(errornum::EN_NOW_STATUS_UN_JOIN_TEAM);
		return TOR_TEAM_NOT_MEMBER;
	}

	Team * m_team = this->GetTeamByUid(user->GetUID());
	if (is_apply_cross && !CrossConfig::Instance().IsHiddenServer())
	{
		CrossTeamInfoMap::iterator it = m_cross_team_info_map.find(team_index);
		//if (it == m_cross_team_info_map.end()) 
		//{
		//	user->NoticeNum(errornum::EN_SORRY_OTHER_SIDE_TEAM_DISMISS);
		//	return TOR_TEAM_NOT_EXIST;
		//}
		if (it != m_cross_team_info_map.end())
		{
			int team_type = it->second.team_type;
			int max_team_member_num = MAX_TEAM_MEMBER_NUM;
			if (TEAM_TYPE_QINGYUAN_DUIDUIPENG == team_type ||
				TEAM_TYPE_TU_ZI_KUAI_PAO == team_type ||
				TEAM_TYPE_CONCENTTRIC_KONT_1 == team_type ||
				TEAM_TYPE_CONCENTTRIC_KONT_2 == team_type ||
				TEAM_TYPE_CONCENTTRIC_KONT_3 == team_type)
			{
				max_team_member_num = 2;
			}

			int tmp_max_member_num = 0;
			if (LOGIC_CONFIG->GetTeamConfig()->IsTeamTypeLimitMaxMember(team_type, &tmp_max_member_num))
			{
				max_team_member_num = tmp_max_member_num;
			}

			if (it->second.cur_member_num >= max_team_member_num)		// 满员时
			{
				user->NoticeNum(errornum::EN_SORRY_OTHER_SIDE_TEAM_FULL);
				return TOR_TEAM_IS_FULL;
			}
			if (user->GetLevel() < it->second.limit_level_low || user->GetLevel() > it->second.limit_level_high)
			{
				user->NoticeNum(errornum::EN_TEAM_LEVEL_LIMIT);
				return TOR_LEVEL_LIMIT;
			}
		}

		if (NULL != m_team)
		{
			if (this->ExitTeam(user))
			{
				return TOR_UNKNOWN_ERROR;
			}
		}
		
		static crossgameprotocal::GameHiddenUserApplyJoinCrossTeam apply;
		apply.origin_plat_type = LocalConfig::Instance().GetPlatType();
		apply.origin_server_id = LocalConfig::Instance().GetMergeServerId();
		apply.origin_role_id = user->GetUID();
		apply.cross_team_index = team_index;

		apply.level = user->GetLevel();
		apply.profession = user->GetProfession();
		apply.avatar_timestamp = user->GetAvatarTimestamp();
		F_STRNCPY(apply.name, user->GetName(), sizeof(GameName));
		apply.avatar_type = user->GetAvatarType();
		apply.headshot_id = user->GetHeadshotID();
		apply.guild_id = user->GetGuildID();

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&apply, sizeof(apply));
		return TOR_SUC;
	}

	TeamListIt team_it = m_team_list.find(team_index);
	if (team_it == m_team_list.end())
	{
		user->NoticeNum(errornum::EN_SORRY_OTHER_SIDE_TEAM_DISMISS);
		return TOR_TEAM_NOT_EXIST;
	}
	
	Team* team = team_it->second;
	if (NULL == team)
	{
		m_team_list.erase(team_it);
		return TOR_TEAM_ERROR;
	}

	if (team->IsReachLevel(user->GetLevel()))
	{
		bool is_has_exit_team = false;
		if (NULL != m_team)
		{
			if (m_team->GetTeamIndex() != team_index)
			{
				is_has_exit_team = true;
			}
			else
			{
				user->NoticeNum(errornum::EN_ROLE_IN_TEAM);
				return TOR_JOINED_TEAM;
			}
		}

		int team_type = team->GetTeamType();
		int max_team_member_num = MAX_TEAM_MEMBER_NUM;
		if (TEAM_TYPE_QINGYUAN_DUIDUIPENG == team_type ||
			TEAM_TYPE_TU_ZI_KUAI_PAO == team_type ||
			TEAM_TYPE_CONCENTTRIC_KONT_1 == team_type ||
			TEAM_TYPE_CONCENTTRIC_KONT_2 == team_type ||
			TEAM_TYPE_CONCENTTRIC_KONT_3 == team_type)
		{
			max_team_member_num = 2;
		}

		int tmp_max_member_num = 0;
		if (LOGIC_CONFIG->GetTeamConfig()->IsTeamTypeLimitMaxMember(team_type, &tmp_max_member_num))
		{
			max_team_member_num = tmp_max_member_num;
		}

		if (team->CanAutoJoin())
		{
			int robot_index = -1;
			if (team->GetMemberCount() >= max_team_member_num && !team->HasRobotMember(&robot_index))		// 满员时
			{
				user->NoticeNum(errornum::EN_SORRY_OTHER_SIDE_TEAM_FULL);
				return TOR_TEAM_IS_FULL;
			}
			if (is_has_exit_team)
			{
				if (this->ExitTeam(user))
				{
					return TOR_UNKNOWN_ERROR;
				}
			}

			if (robot_index >= 0)
			{
				team->LeaveTeam(robot_index);
			}

			if (team->AddMember(user))
			{
				this->OnTeamInfoChange(team);
				//team->SendTeamInfo();
				if (team->IsNotice())
				{
					user->NoticeNum(noticenum::NT_TEAM_JOIN_SUCC);
				}
			}
		}
		else
		{
			if (team->GetMemberCount() >= max_team_member_num)		// 满员时
			{
				user->NoticeNum(errornum::EN_SORRY_OTHER_SIDE_TEAM_FULL);
				return TOR_TEAM_IS_FULL;
			}

			if (is_has_exit_team)
			{
				if (this->ExitTeam(user))
				{
					return TOR_UNKNOWN_ERROR;
				}
			}

			if (!team->CanApplyJoin())
			{
				user->NoticeNum(errornum::EN_TEAM_CAN_NOT_APPLY_JOIN);
			}
			else if (team->JoinReq(user) < 0)
			{
				user->NoticeNum(errornum::EN_HAS_BEEN_SEND_JOIN_REQ);
			}
			else
			{
				user->NoticeNum(noticenum::NT_TEAM_JOIN_REQ_SUCC);
			}
		}
	}
	else
	{
		user->NoticeNum(errornum::EN_NOT_MEET_JOIN_CONDITIONS);
	}

	return 0;
}

int TeamManager::JoinReqRet(Role * user, int uid, bool agree)
{
	if (NULL == user) return TOR_UNKNOWN_ERROR;

	Team* team = this->GetTeam(user);
	if (NULL == team)
	{
		return TOR_TEAM_NOT_EXIST;
	}

	if (!team->IsLeader(user)) return TOR_TEAM_NOT_LEADER;

	int ret = team->JoinReqRet(uid, agree);
	if (0 == ret)
	{
	}
	return 0;
}

int TeamManager::InviteRet(Role * role, int uid, bool agree)
{
	if (NULL == role) return TOR_UNKNOWN_ERROR;
	
	Team* team = this->GetTeamByUid(uid);
	if (NULL == team)
	{
		return TOR_TEAM_NOT_EXIST;
	}

	if (agree)		// 接受邀请
	{
		if (!role->CanJoinTeam())
		{
			role->NoticeNum(errornum::EN_NOW_STATUS_UN_JOIN_TEAM);
			return TOR_TEAM_NOT_MEMBER;
		}

		int robot_index = -1;
		if (team->GetMemberCount() >= MAX_TEAM_MEMBER_NUM && !team->HasRobotMember(&robot_index))		// 满员时
		{
			return TOR_TEAM_IS_FULL;
		}
		else
		{
			if (robot_index >= 0)
			{
				team->LeaveTeam(robot_index);
			}

			if (team->AddMember(role))
			{
				this->OnTeamInfoChange(team);
				//team->SendTeamInfo();
			}
		}
	}
	else				// 拒绝邀请
	{
		Role* leader_role = team->GetLeaderRole();
		if (NULL != leader_role)
		{
			leader_role->NoticeNum(noticenum::NT_TEAM_INVITE_BE_REFUSED);
		}
	}

	return 0;
}

int TeamManager::OnChangeLeader(Role * user, int member_index)
{
	if (NULL == user) return TOR_TEAM_NOT_MEMBER;

	Team* team = this->GetTeam(user);
	if (NULL == team)
	{
		return TOR_TEAM_NOT_EXIST;
	}

	if (!team->IsLeader(user)) return TOR_TEAM_NOT_LEADER;

	Role* member_role = team->GetMemberRoleByIndex(member_index);
	if (NULL == member_role)
	{
		user->NoticeNum(errornum::EN_MEMBER_NOT_ONLINE_LEADER_NO_CHANGE);
		return TOR_TEAM_NOT_MEMBER;
	}

	TeamMember* member = team->GetMemberInfoByIndex(member_index);
	if (NULL == member || !member->is_follow)
	{
		return TOR_TEAM_NOT_MEMBER;
	}

	if (!user->GetScene()->GetSpecialLogic()->CanChangeTeamLeader(user,team))
	{
		user->NoticeNum(errornum::EN_TEAM_CAN_NOT_CHANGE_LEADER);
		return TOR_CAN_NOT_CHANGE;
	}

	{
		static Protocol::SCTeamLeaderChangeNoticeArea msg;
		msg.change_objid = user->GetId();
		msg.is_leader = 0;
		user->GetScene()->GetZoneMatrix()->NotifyAreaMsg(user->GetObserHandle(), &msg, sizeof(msg));
	}

	team->SetLeader(member_index);
	team->OnTeamChangeLeader();
	team->SendTeamInfo();
	this->OnTeamInfoChange(team);	

	EventHandler::Instance().OnRoleRelieveLeader(user, team);

	return 0;
}

int TeamManager::OnChangeTeamCheckType(Role* user, int check_type)
{
	if (check_type < 0 || check_type >= TEAM_CHECK_TYPE_MAX) return TOR_PARAM_ERROR;

	Team* team = this->GetTeam(user);
	if (NULL == team)
	{
		return TOR_TEAM_NOT_EXIST;
	}

	if (!team->IsLeader(user)) return TOR_TEAM_NOT_LEADER;

	team->SetTeamCheckType(check_type);
	if (TEAM_CHECK_TYPE_REFUSE_ALL == check_type)
	{
		TeamMatch::Instance().OnTeamStopMatch(team);
	}
	this->GetTeamChannel()->OnTeamInfoChange(team);
	team->SendTeamSimpleInfoChangeNotice(user);
	return 0;
}

int TeamManager::OnTemporarilyPart(Role * user)
{
	if (NULL == user) return TOR_TEAM_NOT_MEMBER;

	Team* team = this->GetTeam(user);
	if (NULL == team)
	{
		return TOR_TEAM_NOT_EXIST;
	}

	if (user->GetRoleStatusManager()->IsInBattleStatus())
	{
		user->NoticeNum(errornum::EN_IN_BATTLE_CANNOT_TEMPORARILY_PART);
		return TOR_UNKNOWN_ERROR;
	}

	return team->OnTemporarilyPart(user);
}

int TeamManager::OnInvitationReq(Role * user, int uid, int invite_type)
{
	if (NULL == user) return TOR_TEAM_NOT_MEMBER;

	if (user->GetUID() == uid)
	{
		user->NoticeNum(errornum::EN_CAN_NOT_INVITE_YOURSELF);
		return TOR_PARAM_ERROR;
	}

	if (invite_type == 1 && JieYiZuManager::Instance().HasJieYiZu(uid))
	{
		user->NoticeNum(errornum::EN_TARGET_HAS_JIE_YIE_ZU);
		return TOR_PARAM_ERROR;
	}

	if (invite_type == 2 && (QingYuanManager::Instance().HasQingYuan(uid) || QingYuanManager::Instance().HasQingYuan(user->GetUID())))
	{
		user->NoticeNum(errornum::EN_HAS_QING_YUAN);
		return TOR_PARAM_ERROR;
	}

	Scene * user_scene = user->GetScene();
	if (NULL != user_scene)
	{
		if (NULL != user_scene->GetSpecialLogic())
		{
			bool can_inv = user_scene->GetSpecialLogic()->CanInvitationRole(user, uid);
			if (!can_inv)
			{
				user->NoticeNum(errornum::EN_CAN_NOT_NVITATE);
				return TOR_NOT_ALLOW_APPLY;
			}
		}
	}

	Role* target_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(uid));
	if (NULL == target_role)
	{
		// 在原服，则从原服拉过来
		int online_stat = UserCacheManager::Instance().GetUserOnlineStatus(uid);
		if (UserStatusNode::STATUS_TYPE_ONLINE == online_stat && CrossConfig::Instance().IsHiddenServer())
		{
			Team* user_team = this->GetTeamByUid(user->GetUID());
			if (NULL == user_team)
			{
				// 先自行创建队伍
				int create_team_type = TEAM_TYPE_DEFAULT;
				int team_level_min = 1;
				int team_level_max = MAX_ROLE_LEVEL;
				if (NULL != user_scene && NULL != user_scene->GetSpecialLogic())
				{
					create_team_type = user_scene->GetSpecialLogic()->CreateTeamType();//获取特殊副本队伍类型
					const TeamTypeCfg* team_type_cfg = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeCfg(create_team_type);
					if (NULL != team_type_cfg)
					{
						team_level_min = team_type_cfg->low_level_limit;
						team_level_max = team_type_cfg->high_level_limit;
					}
				}

				user_team = this->CreateTeam(user, team_level_min, team_level_max, create_team_type);
				if (NULL == user_team) return TOR_NO_TEAM;
			}

			crossgameprotocal::HiddenGameInviteJoinTeam invitation;
			invitation.cross_team_index = user_team->GetTeamIndex();
			invitation.team_type = user_team->GetTeamType();
			invitation.team_level_limit_low = user_team->GetLimitLevelLow();
			invitation.team_level_limit_high = user_team->GetLimitLevelHigh();
			invitation.team_member_num = user_team->GetMemberCount();
			invitation.inviter_info = user->GetMsgRoleBaseInfo();
			user_team->GetTeamItemInfo(&invitation.team_item);
			invitation.target_uid = uid;
			invitation.invite_type = invite_type;
			
			InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), 
				IntToUid(uid).db_index, (const char*)&invitation, sizeof(invitation));
		}
		else if (UserStatusNode::STATUS_TYPE_CROSS == online_stat && !CrossConfig::Instance().IsHiddenServer())
		{
			Team* team = user->GetMyTeam();
			if (NULL != team)
			{
				int db_index = IntToUid(uid).db_index;
				if (!LocalConfig::Instance().IsAllowedServerId(db_index))
				{
					// 不是本服的玩家，无法从跨服邀请到本服
					user->NoticeNum(errornum::EN_TEAM_INVITE_CROSS_USER_SERVER_NOT_ALLOW);
					return TOR_NOT_ALLOW_INVITE;
				}
				else
				{
					// 邀请对方返回本服并加入玩家的队伍
					crossgameprotocal::GameHiddenInviteJoinTeamInOrigin invitation;
					invitation.team_index = team->GetTeamIndex();
					invitation.team_type = team->GetTeamType();
					invitation.team_level_limit_low = team->GetLimitLevelLow();
					invitation.team_level_limit_high = team->GetLimitLevelHigh();
					invitation.team_member_num = team->GetMemberCount();
					invitation.inviter_info = user->GetMsgRoleBaseInfo();
					team->GetTeamItemInfo(&invitation.team_item);
					invitation.target_uid = uid;
					invitation.invite_type = invite_type;

					InternalComm::Instance().SendToHiddenThroughCross((const char*)&invitation, sizeof(invitation));
				}
			}
			else
			{
				//如果没有队伍，则飞到跨服后继续邀请逻辑
				CrossData_Business business_data;
				business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_ORIGIN_INVITE_CROSS;
				business_data.team_invite_target_uid = uid;
				business_data.team_invite_type = invite_type;
				user->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business_data);
			}	

			return TOR_SUC;
		}
		else
		{
			user->NoticeNum(errornum::EN_TARGET_NOT_ONLINE);
		}
			
		return TOR_NO_ONLINE;
	}

	Team* user_team = this->GetTeamByUid(user->GetUID());
	Team* target_user_team = this->GetTeamByUid(uid);
	if (NULL == user_team)
	{
		// 如果对方也没队伍 就自己创建一个邀请对方
		if (NULL == target_user_team)
		{
			// 创建队伍
			int create_team_type = TEAM_TYPE_DEFAULT;
			int team_level_min = 1;
			int team_level_max = MAX_ROLE_LEVEL;
			if (NULL != user_scene && NULL != user_scene->GetSpecialLogic())
			{
				create_team_type = user_scene->GetSpecialLogic()->CreateTeamType();//获取特殊副本队伍类型
				const TeamTypeCfg* team_type_cfg = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeCfg(create_team_type);
				if (NULL != team_type_cfg)
				{
					team_level_min = team_type_cfg->low_level_limit;
					team_level_max = team_type_cfg->high_level_limit;
				}
			}
			Team* team = this->CreateTeam(user, team_level_min, team_level_max, create_team_type);
			if (NULL == team) return TOR_NO_TEAM;

			user->NoticeNum(noticenum::NT_TEAM_INVITE_SUCC);

			//  邀请对方
			TeamItem team_item;
			team->GetTeamItemInfo(&team_item);

			Protocol::SCTeamInvitationNotice notice;
			notice.notice_type = Protocol::TEAM_INVITATION_NOTICE_TYPE_DEFAULT;
			notice.is_from_cross = CrossConfig::Instance().IsHiddenServer() ? 1 : 0;
			notice.invite_type = invite_type;
			notice.xia_yi_zhi = target_role->GetRoleModuleManager()->GetSalary()->CalculateChivalrousByTeamType(team->GetTeamType(), team_item);
			notice.team_index= team->GetTeamIndex();
			notice.team_type = team->GetTeamType();
			notice.team_member_num = team->GetMemberCount();

			notice.inviter_info = user->GetMsgRoleBaseInfo();
			EngineAdapter::Instance().NetSend(target_role->GetNetId(), (const char*)&notice, sizeof(notice));
		}
		// 如果对方有队伍就加入对方的队伍
		else
		{
			//user->NoticeNum(errornum::EN_TARGET_HAVE_TEAM);
			if (target_user_team->GetTeamType() == 165)
			{
				if (user->GetGuildID() <= 0)
				{
					user->NoticeNum(errornum::EN_TEAMMANAGER_INV_ERR_NO_GUILD);
					return TOR_NOT_ALLOW_APPLY;
				}
				Role * leader_role = target_user_team->GetLeaderRole();
				if (NULL != leader_role)
				{
					if (leader_role->GetGuildID() != user->GetGuildID())
					{
						user->NoticeNum(errornum::EN_TEAMMANAGER_INV_ERR_DIF_GUILD);
						return TOR_NOT_ALLOW_APPLY;
					}
				}
				else
				{
					return TOR_TEAM_NOT_MEMBER;
				}
			}
			this->JoinTeam(user, target_user_team->GetTeamIndex(), false);
		}
	}
	else
	{
		// 如果对方也有队伍就报错
		if (target_user_team != NULL)
		{
			user->NoticeNum(errornum::EN_TARGET_HAVE_TEAM);
			return TOR_JOINED_TEAM;
		}

		if (target_role->GetLevel() < user_team->GetLimitLevelLow() ||
			target_role->GetLevel() > user_team->GetLimitLevelHigh())
		{
			user->NoticeNum(errornum::EN_TEAM_INVITE_LEVEL_LIMIT);
			return TOR_JOINED_TEAM;
		}

		if (user_team->GetTeamType() == 165)
		{
			if (NULL == target_role->GetGuild())
			{
				user->NoticeNum(errornum::EN_TEAMMANAGER_INV_ERR_TAR_NO_GUILD);
				return TOR_NOT_ALLOW_APPLY;
			}
			Role * leader_role = user_team->GetLeaderRole();
			if (NULL != leader_role)
			{
				if (leader_role->GetGuildID() != target_role->GetGuildID())
				{
					user->NoticeNum(errornum::EN_TEAMMANAGER_INV_ERR_DIF_GUILD);
					return TOR_NOT_ALLOW_APPLY;
				}
			}
			else
			{
				return TOR_TEAM_NOT_MEMBER;
			}
		}

		user->NoticeNum(noticenum::NT_TEAM_INVITE_SUCC);

		//  邀请对方
		TeamItem team_item;
		user_team->GetTeamItemInfo(&team_item);

		Protocol::SCTeamInvitationNotice notice;
		notice.notice_type = Protocol::TEAM_INVITATION_NOTICE_TYPE_DEFAULT;
		notice.is_from_cross = CrossConfig::Instance().IsHiddenServer();
		notice.invite_type = invite_type;
		notice.xia_yi_zhi = target_role->GetRoleModuleManager()->GetSalary()->CalculateChivalrousByTeamType(user_team->GetTeamType(), team_item);
		notice.team_index = user_team->GetTeamIndex();
		notice.team_type = user_team->GetTeamType();
		notice.team_member_num = user_team->GetMemberCount();
		notice.inviter_info = user->GetMsgRoleBaseInfo();
		EngineAdapter::Instance().NetSend(target_role->GetNetId(), (const char*)&notice, sizeof(notice));
	}

	return 0;
}

int TeamManager::OnInvitationReqFromHidden(const MsgRoleBaseInfo & inviter_info, const TeamItem & team_item, int cross_team_type, int cross_team_index, int team_member_num, int target_uid, int limit_level_low, int limit_level_high, int invite_type)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return TOR_UNKNOWN_ERROR;
	}

	int inviter_uid = inviter_info.uid;

	crossgameprotocal::GameHiddenInvationJoinTeamRet ret;

	Role* target_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(target_uid));
	if (NULL == target_role)
	{
		ret.result = TOR_NO_ONLINE;
		ret.notice_num = errornum::EN_TARGET_NOT_ONLINE;
		ret.inviter_uid = inviter_uid;

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&ret, sizeof(ret));

		return TOR_NO_ONLINE;
	}

	//Team* target_user_team = this->GetTeamByUid(target_uid);
	//if (NULL != target_user_team)
	//{
	//	ret.result = TOR_JOINED_TEAM;
	//	ret.notice_num = errornum::EN_TARGET_HAVE_TEAM;
	//	ret.inviter_uid = inviter_uid;
	//
	//	InternalComm::Instance().SendToHiddenThroughCross((const char*)&ret, sizeof(ret));
	//
	//	return TOR_JOINED_TEAM;
	//}

	if (target_role->GetLevel() < limit_level_low || target_role->GetLevel() > limit_level_high)
	{
		ret.result = TOR_LEVEL_LIMIT;
		ret.notice_num = errornum::EN_TEAM_INVITE_LEVEL_LIMIT;
		ret.inviter_uid = inviter_uid;

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&ret, sizeof(ret));

		return TOR_LEVEL_LIMIT;
	}

	// 成功邀请
	ret.result = TOR_SUC;
	ret.notice_num = noticenum::NT_TEAM_INVITE_SUCC;
	ret.inviter_uid = inviter_uid;
	InternalComm::Instance().SendToHiddenThroughCross((const char*)&ret, sizeof(ret));

	//  邀请对方	
	Protocol::SCTeamInvitationNotice notice;
	notice.is_from_cross = 1;
	notice.invite_type = invite_type;
	notice.xia_yi_zhi = target_role->GetRoleModuleManager()->GetSalary()->CalculateChivalrousByTeamType(cross_team_type, team_item);
	notice.team_index = cross_team_index;
	notice.team_type = cross_team_type;
	notice.team_member_num = team_member_num;
	notice.inviter_info = inviter_info;
	EngineAdapter::Instance().NetSend(target_role->GetNetId(), (const char*)&notice, sizeof(notice));

	return TOR_SUC;
}

int TeamManager::OnInvitationReqFromOrigin(const MsgRoleBaseInfo & inviter_info, const TeamItem & team_item, int cross_team_type, int cross_team_index, int team_member_num, int target_uid, int limit_level_low, int limit_level_high, int invite_type)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return TOR_UNKNOWN_ERROR;
	}

	int inviter_uid = inviter_info.uid;

	crossgameprotocal::HiddenGameInviteJoinTeamInOriginRet ret;

	Role* target_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(target_uid));
	if (NULL == target_role)
	{
		ret.result = TOR_NO_ONLINE;
		ret.notice_num = errornum::EN_TARGET_NOT_ONLINE;
		ret.inviter_uid = inviter_uid;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(inviter_uid).db_index, (const char*)&ret, sizeof(ret));

		return TOR_NO_ONLINE;
	}

	//Team* target_user_team = this->GetTeamByUid(target_uid);
	//if (NULL != target_user_team)
	//{
	//	ret.result = TOR_JOINED_TEAM;
	//	ret.notice_num = errornum::EN_TARGET_HAVE_TEAM;
	//	ret.inviter_uid = inviter_uid;
	//
	//	InternalComm::Instance().SendToHiddenThroughCross((const char*)&ret, sizeof(ret));
	//
	//	return TOR_JOINED_TEAM;
	//}

	if (target_role->GetLevel() < limit_level_low || target_role->GetLevel() > limit_level_high)
	{
		ret.result = TOR_LEVEL_LIMIT;
		ret.notice_num = errornum::EN_TEAM_INVITE_LEVEL_LIMIT;
		ret.inviter_uid = inviter_uid;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(inviter_uid).db_index, (const char*)&ret, sizeof(ret));

		return TOR_LEVEL_LIMIT;
	}

	if (target_role->GetRoleStatusManager()->IsInBattleStatus())
	{
		ret.result = TOR_IN_BATTLE;
		ret.notice_num = errornum::EN_TEAM_INVITE_CROSS_USER_IN_BATTLE;
		ret.inviter_uid = inviter_uid;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(inviter_uid).db_index, (const char*)&ret, sizeof(ret));

		return TOR_IN_BATTLE;
	}

	// 成功邀请
	ret.result = TOR_SUC;
	ret.notice_num = noticenum::NT_TEAM_INVITE_SUCC;
	ret.inviter_uid = inviter_uid;
	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(inviter_uid).db_index, (const char*)&ret, sizeof(ret));

	//  邀请对方	
	Protocol::SCTeamInvitationNotice notice;
	notice.is_from_cross = 0;
	notice.invite_type = invite_type;
	notice.xia_yi_zhi = target_role->GetRoleModuleManager()->GetSalary()->CalculateChivalrousByTeamType(cross_team_type, team_item);
	notice.team_index = cross_team_index;
	notice.team_type = cross_team_type;
	notice.team_member_num = team_member_num;
	notice.inviter_info = inviter_info;
	EngineAdapter::Instance().NetSend(target_role->GetNetId(), (const char*)&notice, sizeof(notice));

	return TOR_SUC;
}

int TeamManager::OninvitationRet(Role* user, int uid, bool accept, bool is_from_cross, int team_index)
{
	bool is_in_same_server = (is_from_cross && CrossConfig::Instance().IsHiddenServer()) || (!is_from_cross && !CrossConfig::Instance().IsHiddenServer());
	if (!accept)
	{
		if (is_in_same_server)
		{
			Role* leader = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(uid));
			if (NULL != leader)
			{
				leader->NoticeNum(errornum::EN_ROLE_DONT_WANT_TO_JOIN);
			}
		}
		else
		{
			Protocol::SCNoticeNum msg;
			msg.notice_num = errornum::EN_ROLE_DONT_WANT_TO_JOIN;

			InternalComm::Instance().SendToHiddenUserThroughCross(uid, (const char*)&msg, sizeof(msg));
		}
		
		return -1;
	}

	if (NULL == user) return TOR_TEAM_NOT_MEMBER;

	if (!user->CanJoinTeam())
	{
		user->NoticeNum(errornum::EN_NOW_STATUS_UN_JOIN_TEAM);
		return TOR_TEAM_NOT_MEMBER;
	}

	if (user->InTeam())
	{
		TeamManager::Instance().ExitTeam(user); // 注意退队后team可能已失效，下面不能再使用team实例
	}

	if (is_in_same_server)
	{
		Team* team = this->GetTeamByUid(uid);
		if (NULL == team)
		{
			user->NoticeNum(errornum::EN_TEAM_NOT_EXIST_2);
			return TOR_TEAM_NOT_EXIST;
		}

		if (team->GetMemberCount() >= MAX_TEAM_MEMBER_NUM)		// 满员时
		{
			int robot_index = -1;
			if (!team->HasRobotMember(&robot_index))
			{
				user->NoticeNum(errornum::EN_TEAM_IS_FULL);
				return TOR_TEAM_IS_FULL;
			}
			else
			{
				team->LeaveTeam(robot_index);
			}
		}
		if (user->InTeam())
		{
			return TOR_JOINED_TEAM;
		}
		if (team->AddMember(user))
		{
			this->OnTeamInfoChange(team);
			//team->SendTeamInfo();
		}
	}
	else
	{
		if (!user->GetScene()->GetSpecialLogic()->CanEnterTeam(user, NULL, !is_in_same_server))
		{
			user->NoticeNum(errornum::EN_NOW_STATUS_UN_JOIN_TEAM);
			return false;
		}

		//如果发出邀请的队伍在跨服，并且被邀请人在本服才自动到跨服
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			CrossData_Business business;
			business.cross_business_type = CrossData_Business::BUSINESS_TYPE_JOIN_CROSS_TEAM;
			business.join_cross_team_id = team_index;
			business.auto_join_type_param1 = 0;
			user->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business);
		}
		else
		{
			ReturnOriginData_Business business;
			business.business_type = ReturnOriginData_Business::BUSINESS_TYPE_JOIN_ORIGIN_TEAM;
			business.join_team_id = team_index;

			crossgameprotocal::HiddenGameRoleReturnOriginServerReq req;
			req.role_id = user->GetUID();
			req.business_data = business;

			InternalComm::Instance().SendToGameThroughCross(
				user->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType(),
				user->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId(),
				(const char*)&req, sizeof(req));
		}
	}
	
	return 0;
}

int TeamManager::OnTeamRecruitReq(Role* user)
{
	if (NULL == user) return -1;

	Team * team = user->GetMyTeam();
	if (team == NULL)
	{
		user->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return -1;
	}

	unsigned int last_recruit_timestamp = team->GetLastRecruitTimestamp();
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (last_recruit_timestamp + 3 > now)
	{
		user->NoticeNum(errornum::EN_OPERATE_TOO_FREQUENT);
		return -1;
	}

	team->SetLastRecruitTimestamp(now);

	int is_cross = 0;
	if (CrossConfig::Instance().IsHiddenServer())
	{
		is_cross = 1;
	}

	//发公告
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
		gamestring::g_team_recruit, team->GetTeamIndex(), team->GetTeamType(), 
		team->GetLimitLevelLow(), team->GetLimitLevelHigh(), is_cross, team->GetMemberCount());
	if (length > 0)
	{
		if (TEAM_TYPE_GUILD_FIGHT == team->GetTeamType())//家族乱斗
		{
			ChatManager::Instance().SystemSendRoleChannelChat(user, chatdef::CHANNEL_TYPE_GUILD, gamestring::GAMESTRING_BUF, length);
				
			return 0;
		}

		if (is_cross)
		{
			ChatManager::Instance().SystemSendRoleChannelChat(user, chatdef::CHANNEL_TYPE_COMPLEX, gamestring::GAMESTRING_BUF, length);
		}
		else
		{
			ChatManager::Instance().SystemSendRoleChannelChat(user, chatdef::CHANNEL_TYPE_WORLD, gamestring::GAMESTRING_BUF, length);
		}
	}

	TeamRecruit::Instance().OnTeamRecruit(team->GetTeamIndex(),
		team->GetTeamType(), team->GetLeaderName(),
		team->GetLimitLevelLow(), team->GetLimitLevelHigh(),
		team->GetMemberCount(), MAX_TEAM_MEMBER_NUM);
	
	if (CrossConfig::Instance().IsHiddenServer())
	{
		TeamRecruit::Instance().SyncTeamRoleRecruitToOrigin(team->GetTeamIndex(),
			team->GetTeamType(), team->GetLeaderName(),
			team->GetLimitLevelLow(), team->GetLimitLevelHigh(),
			team->GetMemberCount(), MAX_TEAM_MEMBER_NUM);
	}

	return 0;
}

int TeamManager::JoinTeamDirectly(Role* user, int team_index)
{
	if (NULL != user->GetMyTeam())
	{
		return TOR_JOINED_TEAM;
	}

	Team* team = this->GetTeamByTeamIndex(team_index);
	if (NULL == team)
	{
		user->NoticeNum(errornum::EN_TEAM_NOT_EXIST_2);
		return TOR_TEAM_NOT_EXIST;
	}

	int team_type = team->GetTeamType();
	int max_team_member_num = MAX_TEAM_MEMBER_NUM;
	if (TEAM_TYPE_QINGYUAN_DUIDUIPENG == team_type ||
		TEAM_TYPE_TU_ZI_KUAI_PAO == team_type ||
		TEAM_TYPE_CONCENTTRIC_KONT_1 == team_type ||
		TEAM_TYPE_CONCENTTRIC_KONT_2 == team_type ||
		TEAM_TYPE_CONCENTTRIC_KONT_3 == team_type)
	{
		max_team_member_num = 2;
	}

	int tmp_max_member_num = 0;
	if (LOGIC_CONFIG->GetTeamConfig()->IsTeamTypeLimitMaxMember(team_type, &tmp_max_member_num))
	{
		max_team_member_num = tmp_max_member_num;
	}

	if (team->GetMemberCount() >= max_team_member_num)		// 满员时
	{
		int robot_index = -1;
		if (!team->HasRobotMember(&robot_index))
		{
			user->NoticeNum(errornum::EN_TEAM_IS_FULL);
			return TOR_TEAM_IS_FULL;
		}
		else
		{
			team->LeaveTeam(robot_index);
		}
	}

	if (team->AddMember(user))
	{
		//team->SendTeamInfo();
		this->OnTeamInfoChange(team);
		if (team->IsNotice())
		{
			user->NoticeNum(noticenum::NT_TEAM_JOIN_SUCC);
		}

		return TOR_SUC;
	}

	return TOR_TEAM_ERROR;
}

int TeamManager::OnTeamSwitchToCrossReq(Role * user)
{
	Team* team = user->GetMyTeam();
	if (NULL == team)
	{
		user->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return TOR_NO_TEAM;
	}

	if (!team->IsLeader(user))
	{
		user->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return TOR_TEAM_NOT_LEADER;
	}
	
	static CrossData_Business business_data;
	business_data.Reset();
	business_data.origin_team_index = team->GetTeamIndex();
	business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_TEAM_SWITCH_TO_CROSS;
	business_data.level_limit_low = team->GetLimitLevelLow();
	business_data.level_limit_high = team->GetLimitLevelHigh();
	business_data.team_type = team->GetTeamType();
	business_data.allown_red_join = team->TeamIsAllowRedNameJoin() ? 1 : 0;
	int other_member_count = 0;
	for (int i = 0; i < team->GetMemberCount() && other_member_count < 4; ++i)
	{
		TeamMember* tmb = team->GetMemberInfoByIndex(i);
		if (NULL == tmb || tmb->IsRobot() || !tmb->IsFollow() || user->GetUID() == tmb->uid) continue;	//剔除队长
		
		business_data.teammate_list[other_member_count++] = tmb->uid;		
	}
	
	if (user->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business_data))
	{
		team->SetNotice(false, Team::AUTO_JOIN_LIMIT_TIME);
	}

	return true;
}

int TeamManager::OnTeamGotoHidden(Role * user, int join_activity_type, int param1, int param2)
{
	Team * team = user->GetMyTeam();
	if (NULL == team)
	{
		user->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return TOR_NO_TEAM;
	}

	if (!team->IsLeader(user))
	{
		user->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return TOR_TEAM_NOT_LEADER;
	}

	team->SetNotice(false, Team::AUTO_JOIN_LIMIT_TIME);
	static CrossData_Business business_data;
	business_data.Reset();
	business_data.origin_team_index = team->GetTeamIndex();
	business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_TEAM_GO_TO_HIDDEN;
	business_data.level_limit_low = team->GetLimitLevelLow();
	business_data.level_limit_high = team->GetLimitLevelHigh();
	business_data.team_type = team->GetTeamType();
	business_data.auto_join_type = CROSS_AUTO_JION_TYPE_ACTIVITY;
	business_data.auto_join_type_param1 = join_activity_type;
	business_data.param1 = param1;
	business_data.param2 = param2;
	int other_member_count = 0;
	for (int i = 0; i < team->GetMemberCount() && other_member_count < 4; ++i)
	{
		Role * member = team->GetMemberRoleByIndex(i);
		if (NULL != member && member->GetUID() != user->GetUID())
		{
			business_data.teammate_list[other_member_count++] = member->GetUID();
		}
	}
	user->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business_data);

	return true;
}

int TeamManager::OnSwitchPositionReq(Role* user, short pos1, short pos2)
{
	Team* team = user->GetMyTeam();
	if (NULL == team)
	{
		user->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return TOR_NO_TEAM;
	}

	if (team->GetLeaderUID() != user->GetUID())
	{
		user->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return TOR_TEAM_NOT_LEADER;
	}

	if (pos1 < 1 || pos1 >= MAX_TEAM_MEMBER_NUM || pos2 < 1 || pos2 >= MAX_TEAM_MEMBER_NUM)
	{
		user->NoticeNum(errornum::EN_TEAM_LEADER_CANT_CHANGE_POSITION);
		return TOR_PARAM_ERROR;
	}

	if (pos1 == pos2)
	{
		return TOR_SUC;
	}

	team->SwitchPosition(user, pos1, pos2);

	return TOR_SUC;
}

int TeamManager::OnSummonMemberReq(Role* user, int summon_role_id)
{
	if (NULL == user)
	{
		return TOR_UNKNOWN_ERROR;
	}

	if (summon_role_id == user->GetUID())
	{
		return TOR_PARAM_ERROR;
	}

	Team* team = user->GetMyTeam();
	if (NULL == team)
	{
		user->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return TOR_NO_TEAM;
	}

	if (team->GetLeaderUID() != user->GetUID())
	{
		user->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return TOR_TEAM_NOT_LEADER;
	}

	TeamMember* member = team->GetMemberInfoByUID(summon_role_id);
	if (NULL == member || member->Invalid())
	{
		return TOR_TEAM_NOT_MEMBER;
	}

	Role* member_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(summon_role_id));
	if (NULL == member_role)
	{
		user->NoticeNum(errornum::EN_TARGET_NOT_ONLINE);
		return TOR_MEMBER_NOT_ONLINE;
	}

	Protocol::SCTeamLeaderSummonMemberNotice notice;
	EngineAdapter::Instance().NetSend(member_role->GetNetId(), (const char*)&notice, sizeof(notice));

	user->NoticeNum(noticenum::NT_TEAM_LEADER_SUMMON_SENT);
	return TOR_SUC;
}

int TeamManager::OnReplySummon(Role* user, int is_return)
{
	if (NULL == user)
	{
		return TOR_UNKNOWN_ERROR;
	}

	Team* team = user->GetMyTeam();
	if (NULL == team)
	{
		user->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return TOR_NO_TEAM;
	}

	TeamMember* member = team->GetMemberInfoByUID(user->GetUID());
	if (NULL == member || member->IsFollow())
	{
		return TOR_UNKNOWN_ERROR;
	}

	Role* leader = team->GetLeaderRole();
	if (NULL == leader)
	{
		return TOR_UNKNOWN_ERROR;
	}

	if (is_return)
	{
		team->OnTemporarilyPart(user);
	}
	else
	{
		//发公告
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
			gamestring::g_team_member_refuse_summon, user->GetName());
		if (length > 0)
		{
			static Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				EngineAdapter::Instance().NetSend(leader->GetNetId(), (const char*)&sm, sendlen);
			}
		}
	}
	
	return TOR_SUC;
}

int TeamManager::OnLeaderExitTeamWithSomeMembers(Role* leader, int member_count, int* member_uid_list)
{
	if (NULL == leader)
	{
		return TOR_UNKNOWN_ERROR;
	}

	Team* team = leader->GetMyTeam();
	if (NULL == team)
	{
		return TOR_TEAM_NOT_EXIST;
	}

	if (team->GetLeaderUID() != leader->GetUID())
	{
		return TOR_TEAM_NOT_LEADER;
	}

	/*
	bool has_other_role_in_team = false;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember* member = team->GetMemberInfoByIndex(i);
		if (NULL == member) continue;
		if (member->Invalid()) continue;
		if (member->IsRobot()) continue;
		if (!member->IsFollow()) continue;
		if (member->uid == leader->GetUID()) continue;

		bool has_found = false;
		for (int k = 0; k < member_count; ++k)
		{
			if (member->uid == member_uid_list[k])
			{
				has_found = true;
				break;
			}
		}

		if (!has_found)
		{
			has_other_role_in_team = true; // 有其他玩家在队伍里
			break;
		}
	}*/

	//if (!has_other_role_in_team) // 没有其他玩家在队伍里
	//{
	//	team->DismissTeamHelper();
	//}
	//else
	//{

	int team_index = team->GetTeamIndex();
		for (int i = 0; i < member_count; ++i)
		{
			team->LeaveTeamByUid(member_uid_list[i], false);
		}
		team->LeaveTeamByUid(leader->GetUID(), false);

		// 重新获取team实例，以防team已被解散
		team = this->GetTeamByTeamIndex(team_index);
		if (NULL != team)
		{
			team->NoticeAllMember(errornum::EN_CROSS_MEMBER_LEAVE_TEAM);
		}

	//}

		return TOR_SUC;
}

int TeamManager::GetTeamIndex(Role * user)
{
	Team* team = this->GetTeam(user);
	if (NULL == team) return 0;
	
	return team->GetTeamIndex();
}

Team* TeamManager::GetTeam(Role * user)
{
	if (NULL == user) return NULL;
	
	return this->GetTeamByUid(user->GetUID());
}

Team * TeamManager::GetTeamByUid(int uid)
{
	UserTeamIndexMapIt user_it = m_user_teamindex_map.find(uid);
	if (user_it == m_user_teamindex_map.end())
	{
		return NULL;
	}

	TeamListIt team_it = m_team_list.find(user_it->second);
	if (team_it == m_team_list.end())
	{
		m_user_teamindex_map.erase(user_it);
		return NULL;
	}

	Team* team = team_it->second;
	if (NULL == team)
	{
		m_user_teamindex_map.erase(user_it);
		m_team_list.erase(team_it);
		return NULL;
	}

	return team;
}

Team * TeamManager::GetTeamByTeamIndex(int team_index)
{
	TeamListIt team_it = m_team_list.find(team_index);
	if (team_it == m_team_list.end()) return NULL;

	Team* team = team_it->second;
	if (NULL == team)
	{
		m_team_list.erase(team_it);
		return NULL;
	}

	return team;
}

int TeamManager::SetTeamInfo(Role * user, short limit_level_low, short limit_level_high, int team_type, bool allow_red_join)
{
	if (NULL == user) return TOR_TEAM_NOT_MEMBER;
	
	Team* team = this->GetTeam(user);
	if (NULL == team)
	{
		return TOR_TEAM_NOT_EXIST;
	}

	// 只有队长能改设置
	if (!team->IsLeader(user)) return TOR_TEAM_NOT_LEADER;

	for (int i = 0; i < team->GetMemberCount(); ++i)
	{
		TeamMember* member = team->GetMemberInfoByIndex(i);
		if (NULL == member) continue;

		if (member->level > limit_level_high || limit_level_low > member->level)
		{
			user->NoticeNum(errornum::EN_TEAM_MEMBER_LIMIT_LEVEL_NOT_MEET);
			return TOR_LEVEL_LIMIT;
		}

		if (!allow_red_join && team->TeamIsRedName())
		{
			user->NoticeNum(errornum::EN_DUEL_TEAM_HAS_RED);
			return TOR_UNKNOWN_ERROR;
		}
	}
	int old_team_type = team->GetTeamType();
	team->SetTeamLimit(limit_level_low, limit_level_high);
	if (team->SetTeamType(team_type))
	{
		//this->ClearTeamMatch(team);
		TeamMatch::Instance().OnTeamStopMatch(team);
	}
	team->SetTeamAllowRedJoin(allow_red_join);
	this->OnTeamInfoChange(team);

	if (team_type != old_team_type && (TEAM_TYPE_HUAN_JIE_ZHAN_CHANG == team_type || TEAM_TYPE_HUAN_JIE_ZHAN_CHANG == old_team_type))
	{
		// 幻界战场特殊处理
		team->SendTeamInfo();
	}
	else
	{
		team->SendTeamSimpleInfoChangeNotice();
	}

	if(team_type != old_team_type && (TEAM_TYPE_SHAN_HAI_DENG_HUI == team_type || TEAM_TYPE_KE_JU_EXAM == team_type))		// 山海灯会/科举考试踢出机器人
	{
		for (int i = MAX_TEAM_MEMBER_NUM - 1; i >= 0; --i)
		{
			TeamMember* member = team->GetMemberInfo(i);
			if(NULL != member)
			{
				if (member->IsRobot())
				{
					team->LeaveTeam(i);
				}
			}
		}
	}

	return 0;
}

void TeamManager::TeamDismiss(Team* team)
{
	if (NULL == team)return;

	this->OnTeamDismiss(team);

	// 将队员的team index map清空
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
	{
		TeamMember *member = team->GetMemberInfoByIndex(i);
		if (NULL != member)
		{
			if (!member->IsRobot())
			{
				this->OnRoleExitTeam(member->uid);

				Role *role = team->GetMemberRoleByIndex(i);
				if (NULL != role)
				{
					if (team->IsNotice())
					{
						role->NoticeNum(errornum::EN_TEAM_DISMISS);
					}
						
					role->SetTeam(NULL);

					team->OnStopFollow(role);

					static Protocol::SCOutOfTeam info;
					EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));

					EventHandler::Instance().OnRoleLeaveTeam(role, team->GetTeamIndex(), true);
					if (role->GetUID() == team->GetLeaderUID())
					{
						EventHandler::Instance().OnTeamDismiss(role, team);
					}
				}
			}
			else
			{
				Robot* robot = team->GetMemberRobotByIndex(i);
				if (NULL != robot)
				{
					team->OnStopFollow(robot);
					robot->SetTeam(NULL);
					robot->OnLeaveTeam();
					robot->SetDelayDelete(); // 所有机器人从队伍中退出，都要回收掉
				}
			}
			member->Reset();
		}
	}

	// 删除列队信息
	TeamListIt it = m_team_list.find(team->GetTeamIndex());
	if (it != m_team_list.end())
	{
		m_delay_delete_list.insert(TeamList::value_type(it->first, it->second));
		m_team_list.erase(it);
	}
}

void TeamManager::TeamBackFromTeamRegister(Team * team)
{
	int count = 0;
	do
	{
		if (++m_team_index >= TEAM_INDEX_MAX) { m_team_index = 1; }
		if (++count > 100) return;

	} while (m_team_list.end() != m_team_list.find(m_team_index));

	Team *new_team = new Team(*team);
	new_team->SetTeamIndex(m_team_index);
	m_team_list[m_team_index] = new_team;

	for (int i = 0; i < new_team->GetMemberCount(); i++)
	{
		TeamMember* member = new_team->GetMemberInfoByIndex(i);
		if (NULL != member)this->OnRoleEnterTeam(member->uid, m_team_index);
	}
	new_team->AutoSetLeader(true);
	new_team->SendTeamInfo();
	new_team->FillPosList();
}

int TeamManager::UserLogin(const UserID& user_id)
{
	Role* user = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL == user) return TOR_TEAM_NOT_MEMBER;

	Team* team = this->GetTeam(user);
	if (NULL == team)
	{
		//刚进服不应该默认没有队伍吗？感觉没有必要发
		//static Protocol::SCOutOfTeam info;
		//EngineAdapter::Instance().NetSend(user->GetNetId(), (const char*)&info, sizeof(info));

		//检查玩家是否有队伍在跨服


		return TOR_TEAM_NOT_EXIST;
	}

	team->UserLogin(user);
	return 0;
}

int TeamManager::UserLogout(Role * user)
{
	if (NULL == user) return TOR_TEAM_NOT_MEMBER;

	Team* team = this->GetTeam(user);
	if (NULL == team)
	{
		return TOR_NO_TEAM;
	}

	team->UserLogout(user);
	return 0;
}

void TeamManager::ClearTeamMatch(Team * team)
{
	//暂时不使用这个函数
	//TeamMatch::Instance().OnTeamStopMatch(team);
}

void TeamManager::ClearUserMatch(Role * user)
{
	//暂时不使用这个函数
	//TeamMatch::Instance().OnUserStopMatch(user->GetUniqueUserID());
}

void TeamManager::ChangePartnerFightState(Role * user)
{
	if (NULL == user) return;

	Team* team = this->GetTeam(user);
	if (NULL == team)
	{
		return;
	}

	if (!team->IsLeader(user)) return  ;

	team->SendTeamInfo();
}

int TeamManager::SendUnfullTeamListToUser(Role * user, int team_type)
{
	if (NULL == user) return - 1;

	if (TEAM_TYPE_INVALID != team_type)
	{
		const TeamTypeCfg* cfg = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeCfg(team_type);
		if (NULL == cfg) return -1;
	}
	
	//// 检测cd
	//{
	//	time_t& next_send_timestamp = m_user_send_cd_map[user->GetUID()];
	//	if (next_send_timestamp > EngineAdapter::Instance().Time())return -3;
	//	next_send_timestamp = EngineAdapter::Instance().Time() + 3;
	//}

	int count = 0;
	static Protocol::SCTeamListAck msg;
	for (TeamListIt it = m_team_list.begin(); it != m_team_list.end(); ++it)
	{
		Team* team = it->second;
		if (NULL == team) continue;
		
		if (team->GetMemberCount() >= MAX_TEAM_MEMBER_NUM) continue;
		
		if (TEAM_TYPE_INVALID != team_type && team_type != team->GetTeamType()) continue;
		
		Role* leader = team->GetLeaderRole();
		if (NULL == leader) continue;
		
		if (TEAM_TYPE_GUILD_FIGHT == team_type)
		{
			if (leader->GetGuildID() != user->GetGuildID())continue;
		}

		if (TEAM_TYPE_MONSTER_WAVE == team_type)
		{
			if (leader->GetRoleStatusManager()->IsInBattleStatus()) continue;
		}

		if (TEAM_CHECK_TYPE_REFUSE_ALL == team->GetCheckType()) continue;

		if (!leader->GetScene()->IsInStaticScene()) continue;

		TeamItem& team_item = msg.team_list[count];
		team->GetTeamItemInfo(&team_item);

		if (0 == team_item.limit_level_low || 0 == team_item.limit_level_high)
		{
			gamelog::g_log_serious_error.printf(LL_WARNING, "SendUnfullTeamListToUser team data invalid, team_index[%d] team_type[%d] low_lv[%d] high_lv[%d]",
				team_item.team_index, team_item.team_type, team_item.limit_level_low, team_item.limit_level_high);
			continue;
		}

		if (++count >= MAX_TEAM_LIST_NUM) break;
	}

	if (count < MAX_TEAM_LIST_NUM)
	{
		CrossTeamInfoMap::iterator it = m_cross_team_info_map.begin();
		for (; it != m_cross_team_info_map.end(); ++it)
		{
			TeamItem& ti = it->second;
			if (ti.cur_member_num >= MAX_TEAM_MEMBER_NUM) continue;
			if (TEAM_TYPE_INVALID != team_type && team_type != ti.team_type) continue;

			if (TEAM_TYPE_GUILD_FIGHT == team_type)
			{
				if (ti.guild_id != user->GetGuildID())continue;
			}

			if (0 == ti.limit_level_low || 0 == ti.limit_level_high)
			{
				gamelog::g_log_serious_error.printf(LL_WARNING, "SendUnfullTeamListToUser \
					team data invalid, team_index[%d] team_type[%d] low_lv[%d] high_lv[%d]",
					ti.team_index, ti.team_type, ti.limit_level_low, ti.limit_level_high);
				continue;
			}

			msg.team_list[count++] = it->second;
		}
	}

	msg.count = count;
	int sendlen = sizeof(Protocol::SCTeamListAck) - (MAX_TEAM_LIST_NUM - msg.count) * sizeof(TeamItem);
	EngineAdapter::Instance().NetSend(user->GetNetId(), (const char*)&msg, sendlen);
	return 0;
}

void TeamManager::QueryUnfullChivalrousTeamList(Role* user)
{
	if(NULL == user) return;

	Team* myteam = user->GetMyTeam();
	int count = 0;
	static Protocol::SCChivalrousTeamList msg;
	for (TeamListIt it = m_team_list.begin(); it != m_team_list.end(); ++it)
	{
		Team* team = it->second;
		if (NULL == team) continue;

		if (NULL != myteam && team->GetTeamIndex() == myteam->GetTeamIndex()) continue;

		if (TEAM_CHECK_TYPE_REFUSE_ALL == team->GetCheckType()) continue;

		if (team->GetMemberCount() >= MAX_TEAM_MEMBER_NUM) continue;

		if (!LOGIC_CONFIG->GetSundryConfig()->IsChivalrousTeamType(team->GetTeamType())) continue; 	

		Role* leader = team->GetLeaderRole();
		if (NULL == leader) continue;

		if (!leader->GetScene()->IsInStaticScene()) continue;	
		//只用配置等级不足,不显示
		const TeamTypeCfg* team_cfg = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeCfg(team->GetTeamType());
		if(NULL == team_cfg || team_cfg->low_level_limit > user->GetLevel()) continue;

		TeamItem& team_item = msg.team_list[count].team_info;
		team->GetTeamItemInfo(&team_item);

		int add_chivalrous = user->GetRoleModuleManager()->GetSalary()->CalculateChivalrousByTeamType(team->GetTeamType(), team_item);
		if (add_chivalrous <= 0) continue;

		msg.team_list[count].add_chivalrous_value = add_chivalrous;

		if (0 == team_item.limit_level_low || 0 == team_item.limit_level_high)
		{
			gamelog::g_log_serious_error.printf(LL_WARNING, "SendUnfullTeamListToUser team data invalid, team_index[%d] team_type[%d] low_lv[%d] high_lv[%d]",
				team_item.team_index, team_item.team_type, team_item.limit_level_low, team_item.limit_level_high);
			continue;
		}

		if (++count >= MAX_TEAM_LIST_NUM) break;
	}

	if (count < MAX_TEAM_LIST_NUM)
	{
		CrossTeamInfoMap::iterator it = m_cross_team_info_map.begin();
		for (; it != m_cross_team_info_map.end(); ++it)
		{
			TeamItem& ti = it->second;
			if (ti.cur_member_num >= MAX_TEAM_MEMBER_NUM) continue;

			if (0 == ti.limit_level_low || 0 == ti.limit_level_high)
			{
				continue;
			}
			
			if (!LOGIC_CONFIG->GetSundryConfig()->IsChivalrousTeamType(ti.team_type)) continue;
			//只用配置等级不足,不显示
			const TeamTypeCfg* team_cfg = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeCfg(ti.team_type);
			if (NULL == team_cfg || team_cfg->low_level_limit > user->GetLevel()) continue;

			int add_chivalrous = user->GetRoleModuleManager()->GetSalary()->CalculateChivalrousByTeamType(ti.team_type, ti);
			if (add_chivalrous <= 0) continue;

			msg.team_list[count].add_chivalrous_value = add_chivalrous;
			msg.team_list[count].team_info = ti;
			
			++count;
		}
	}

	msg.count = count;
	int sendlen = sizeof(Protocol::SCChivalrousTeamList) - (MAX_TEAM_LIST_NUM - msg.count) * sizeof(msg.team_list[0]);
	EngineAdapter::Instance().NetSend(user->GetNetId(), (const char*)&msg, sendlen);
}

void TeamManager::OnTeamInfoChange(Team* team)
{
	this->GetTeamChannel()->OnTeamInfoChange(team);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (TEAM_CHECK_TYPE_REFUSE_ALL != team->GetCheckType())
		{
			TeamManager::Instance().SyncCrossTeamInfo(team, crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_INFO_CHANGE);
		}
		else
		{
			TeamManager::Instance().SyncCrossTeamInfo(team, crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_REMOVE);
		}
	}

	team->OnTeamInfoChangedCheckRealRoleRobotLeave();
}

void TeamManager::SyncAllCrossTeamInfo(int to_plat_type, int to_server_id)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	static crossgameprotocal::HiddenGameSyncCrossTeamInfo msg;
	msg.notify_reason = crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_INIT;
	msg.team_count = 0;
	
	for (TeamListIt it = m_team_list.begin(); it != m_team_list.end(); ++it)
	{
		Team* team = it->second;
		team->GetTeamItemInfo(&msg.team_list[msg.team_count]);
		msg.team_count += 1;

		if (MAX_TEAM_LIST_NUM == msg.team_count)
		{
			int send_len = sizeof(msg) - sizeof(msg.team_list) + msg.team_count * sizeof(msg.team_list[0]);
			InternalComm::Instance().SendToGameThroughCross(to_plat_type, to_server_id, (const char*)&msg, send_len);

			msg.team_count = 0;
			msg.notify_reason = crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_INIT_AGAIN;
		}
	}

	if (msg.team_count > 0)
	{
		int send_len = sizeof(msg) - sizeof(msg.team_list) + msg.team_count * sizeof(msg.team_list[0]);
		InternalComm::Instance().SendToGameThroughCross(to_plat_type, to_server_id, (const char*)&msg, send_len);
	}
}

void TeamManager::OnSyncCrossTeamInfo(crossgameprotocal::HiddenGameSyncCrossTeamInfo* msg)
{
	switch (msg->notify_reason)
	{
	case crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_INIT:
		{
			m_cross_team_info_map.clear();
		}
		//do not break

	case crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_INIT_AGAIN:
	case crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_ADD:
		{
			for (int i = 0; i < msg->team_count; ++i)
			{
				CrossTeamInfoMap::iterator it = m_cross_team_info_map.find(msg->team_list[i].team_index);
				if (it != m_cross_team_info_map.end()) continue;

				m_cross_team_info_map.insert(CrossTeamInfoMap::value_type(msg->team_list[i].team_index, msg->team_list[i]));
				this->GetTeamChannel()->OnTeamCreate(msg->team_list[i].team_index, true, msg->team_list[i].create_team_timestamp, msg->team_list[i]);
			}
		}
		break;

	case crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_INFO_CHANGE:
		{
			for (int i = 0; i < msg->team_count; ++i)
			{
				CrossTeamInfoMap::iterator it = m_cross_team_info_map.find(msg->team_list[i].team_index);
				if (it == m_cross_team_info_map.end()) continue;

				it->second = msg->team_list[i];
				this->GetTeamChannel()->OnTeamInfoChange(msg->team_list[i].team_index, true, msg->team_list[i].create_team_timestamp, msg->team_list[i], TEAM_CHECK_TYPE_NEED_CHECK);
			}	
		}
		break;

	case crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_REMOVE:
		{
			for (int i = 0; i < msg->team_count; ++i)
			{
				CrossTeamInfoMap::iterator it = m_cross_team_info_map.find(msg->team_list[i].team_index);
				if (it == m_cross_team_info_map.end()) continue;

				m_cross_team_info_map.erase(it);
				this->GetTeamChannel()->OnTeamDismiss(msg->team_list[i].team_index, true, msg->team_list[i].create_team_timestamp, msg->team_list[i]);
			}
		}
		break;
	}
}

int TeamManager::ApplyJoinCrossTeam(crossgameprotocal::GameHiddenUserApplyJoinCrossTeam* msg)
{
	TeamListIt team_it = m_team_list.find(msg->cross_team_index);
	if (team_it == m_team_list.end())
	{
		this->SendErrorNumToOriginServer(msg->origin_role_id, msg->origin_plat_type, msg->origin_server_id, errornum::EN_TEAM_NOT_EXIST);
		return TOR_TEAM_NOT_EXIST;
	}
	Team* team = team_it->second;
	if (NULL == team)
	{
		m_team_list.erase(team_it);
		this->SendErrorNumToOriginServer(msg->origin_role_id, msg->origin_plat_type, msg->origin_server_id, errornum::EN_TEAM_NOT_EXIST);
		return TOR_TEAM_ERROR;
	}

	if (!team->IsReachLevel(msg->level))
	{
		this->SendErrorNumToOriginServer(msg->origin_role_id, msg->origin_plat_type, msg->origin_server_id, errornum::EN_TEAM_LEVEL_LIMIT);
		return TOR_LEVEL_LIMIT;
	}

	if (team->GetMemberCount() >= MAX_TEAM_MEMBER_NUM)
	{
		this->SendErrorNumToOriginServer(msg->origin_role_id, msg->origin_plat_type, msg->origin_server_id, errornum::EN_TEAM_IS_FULL);
		return TOR_TEAM_IS_FULL;
	}

	Role* leader = team->GetLeaderRole();
	if (NULL == leader)
	{
		if (CrossConfig::Instance().IsHiddenServer())
		{
			TeamManager::Instance().SyncCrossTeamInfo(team, crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_REMOVE);
		}

		this->SendErrorNumToOriginServer(msg->origin_role_id, msg->origin_plat_type, msg->origin_server_id, errornum::EN_UNKNOWN_ERROR);
		return TOR_UNKNOWN_ERROR;
	}
	
	if (Scene::SCENE_TYPE_GUILD_FIGHT == leader->GetScene()->GetSpecialLogic()->GetSceneType())
	{
		Activity* activity = ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_FIGHT);
		if (NULL != activity && activity->IsActivityOpen())
		{
			ActivityGuildFight* agf = (ActivityGuildFight*)activity;
			if (!agf->IsActivityEnterTime())
			{
				this->SendErrorNumToOriginServer(msg->origin_role_id, msg->origin_plat_type, msg->origin_server_id, errornum::EN_JOIN_TEAM_ACTIVITY_LIMIT);
				return TOR_UNKNOWN_ERROR;
			}
		}			

		if (leader->GetGuildID() != msg->guild_id)
		{
			this->SendErrorNumToOriginServer(msg->origin_role_id, msg->origin_plat_type, msg->origin_server_id, errornum::EN_GUILD_FIGHT_NOT_SAME_GUILD_WITH_LEADER);
			return TOR_ACTIVITY_LIMIT;
		}
	}

	if (Scene::SCENE_TYPE_SHI_TU == leader->GetScene()->GetSpecialLogic()->GetSceneType())
	{
		return TOR_UNKNOWN_ERROR;
	}

	if (team->CanAutoJoin())
	{
		// 隐藏服 拉原服的人进队
		static crossgameprotocal::HiddenCrossTeammateJoinCrossTeam hctjct;
		hctjct.origin_team_index = -1;
		hctjct.cross_team_index = team->GetTeamIndex();
		hctjct.origin_plat_type = msg->origin_plat_type;
		hctjct.origin_server_id = msg->origin_server_id;
		memset(hctjct.teammate_list, 0, sizeof(hctjct.teammate_list));
		hctjct.teammate_list[0] = msg->origin_role_id;

		InternalComm::Instance().SendToCross((const char*)&hctjct, sizeof(hctjct));
	}
	else if (team->CanApplyJoin())
	{
		team->CrossJoinReq(msg->origin_role_id, msg->origin_plat_type, msg->origin_server_id, 
			msg->level, msg->profession, msg->avatar_timestamp, msg->name, msg->avatar_type, msg->headshot_id);
	}
	else
	{
		return TOR_NOT_ALLOW_APPLY;
	}

	return TOR_SUC;
}

bool TeamManager::RobotJoinTeam(Role* role, Robot* robot)
{
	if (NULL == role || NULL == robot) return false;

	Team* team = role->GetMyTeam(true);
	if (NULL == team) return false;

	if (team->GetMemberCount() >= MAX_TEAM_MEMBER_NUM) return false;

	team->AddRobot(robot);

	return true;
}

Team* TeamManager::GetJoinableTeamByTeamType(int team_type)
{
	TeamList::iterator it = m_team_list.begin();
	for (; it != m_team_list.end(); ++it)
	{
		Team* team = it->second;
		if (NULL == team ||
			team->GetMemberCount() >= MAX_TEAM_MEMBER_NUM ||
			team_type != team->GetTeamType())
		{
			continue;
		}

		return team;
	}

	return NULL;
}

void TeamManager::OnRoleEnterTeam(int uid, int team_index)
{
	m_user_teamindex_map.insert(UserTeamIndexMap::value_type(uid, team_index));
}

void TeamManager::OnRoleExitTeam(int uid)
{
	m_user_teamindex_map.erase(uid);
}

void TeamManager::OnLeaveFB(Role * role)
{
	Team* team = this->GetTeam(role);
	if (NULL == team)
	{
		return;
	}

	TeamMember* member = team->GetMemberInfoByUID(role->GetUID());
	if (NULL == member)
	{
		return;
	}
	if (team->GetMemberCount() <= 1 || (team->GetFollowMemberCount() <= 1 && member->IsFollow()))
	{
		team->DismissTeamHelper();

		static Protocol::SCTeamLeaderChangeNoticeArea msg;
		msg.change_objid = role->GetId();
		msg.is_leader = 0;
		role->GetScene()->GetZoneMatrix()->NotifyAreaMsg(role->GetObserHandle(), &msg, sizeof(msg));
		return ;
	}

	if (Scene::SCENE_TYPE_WORLD_TEAM_ARENA == role->GetScene()->GetSceneType() && member->is_follow)
	{
		this->OnTemporarilyPart(role);
	}
	else
	{
		// 离队
		team->LeaveTeam(role->GetUserId());
	}
}

void TeamManager::OnDisconnectFromCrossServer()
{
	CrossTeamInfoMap::iterator it = m_cross_team_info_map.begin();
	for (; it != m_cross_team_info_map.end(); ++it)
	{
		this->GetTeamChannel()->OnTeamDismiss(it->second.team_index, true, it->second.create_team_timestamp, it->second);
	}

	m_cross_team_info_map.clear();
}

void TeamManager::OnTeamSwitchToStaticScene(Team* team)
{
	this->GetTeamChannel()->OnTeamCreate(team->GetTeamIndex(), team->GetIsCross(), team->GetTeamCreateTime(), team);
	if (CrossConfig::Instance().IsHiddenServer())
	{
		TeamManager::Instance().SyncCrossTeamInfo(team, crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_ADD);
	}
}

void TeamManager::OnTeamSwitchToSpecialScene(Team* team)
{
	this->GetTeamChannel()->OnTeamDismiss(team->GetTeamIndex(), team->GetIsCross(), team->GetTeamCreateTime(), team);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		TeamManager::Instance().SyncCrossTeamInfo(team, crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_REMOVE);
	}
}

void TeamManager::OnUserLoginOriginGame(int role_id)
{
	Team * team = this->GetTeamByUid(role_id);
	if (team != NULL)
	{
		team->LeaveTeamByUid(role_id);
	}
}

void TeamManager::ReqJoinTeamInfo(Role * role, bool is_team_index_in_cross, int team_index, int inviter_uid)
{
	if ((!is_team_index_in_cross && !CrossConfig::Instance().IsHiddenServer()) || (is_team_index_in_cross && CrossConfig::Instance().IsHiddenServer()))
	{
		Protocol::SCTeamInvitationNotice notice;
		notice.notice_type = Protocol::TEAM_INVITATION_NOTICE_TYPE_NOT_EXIST;
		notice.is_from_cross = is_team_index_in_cross;
		notice.invite_type = 0;
		notice.team_index = team_index;
		Team * team = this->GetTeamByTeamIndex(team_index);
		if (team != NULL && team->IsMember(inviter_uid))
		{
			notice.notice_type = Protocol::TEAM_INVITATION_NOTICE_TYPE_UPDATE;
			TeamItem team_item;
			team->GetTeamItemInfo(&team_item);
			notice.xia_yi_zhi = role->GetRoleModuleManager()->GetSalary()->CalculateChivalrousByTeamType(team->GetTeamType(), team_item);
			notice.team_type = team->GetTeamType();
			notice.team_member_num = team->GetMemberCount();
			notice.inviter_info = team->GetMsgRoleBaseInfo(inviter_uid);
		}

		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&notice, sizeof(notice));
	}

	if (is_team_index_in_cross && !CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenReqJoinTeamInfo req;
		req.cross_team_index = team_index;
		req.inviter_uid = inviter_uid;
		req.target_uid = role->GetUID();
		InternalComm::Instance().SendToHiddenThroughCross((const char *)&req, sizeof(req));
	}

	if (!is_team_index_in_cross && CrossConfig::Instance().IsHiddenServer())
	{
		Protocol::SCTeamInvitationNotice notice;
		notice.notice_type = Protocol::TEAM_INVITATION_NOTICE_TYPE_NOT_EXIST;
		notice.is_from_cross = is_team_index_in_cross;
		notice.team_index = team_index;
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&notice, sizeof(notice));
	}
}

