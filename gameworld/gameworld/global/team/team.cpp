#include "obj/character/role.h"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "world.h"
#include "servercommon/crossdef.hpp"
#include "teammanager.hpp"
#include "protocol/msgteam.h"
#include "servercommon/errornum.h"
#include "other/partner/partner.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "scene/scene.h"
#include <algorithm>
#include "scene/fbmanager.hpp"
#include "global/team/teammatch.hpp"
#include "servercommon/noticenum.h"
#include "servercommon/string/gameworldstr.h"
#include "chat/chatmanager.h"
#include "internalcomm.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/rolecross/rolecross.hpp"
#include "other/salary/salary.hpp"
#include "protocol/msgscene.h"
#include "battle/battle_manager_local.hpp"
#include "obj/character/robot.h"
#include "other/robot/robotmanager.hpp"
#include "other/pet/pet.hpp"
#include "global/activity/activitymanager.hpp"
#include "item/knapsack.h"
#include "scene/scenemanager.hpp"
#include "team.hpp"
#include "other/formation/formation.hpp"
#include "protocol/msgother.h"
#include "protocol/msgformation.h"
#include "gamelog.h"
#include "temporary_chat_group.hpp"
#include "scene/teamkeyregister/teamkeyregister.hpp"
#include "protocol/msghuanjiezhanchang.h"
#include "global/activity/activityimpl/activityhuanjiezhanchang.hpp"
#include "other/duel/duel.hpp"
#include "global/activity/activityimpl/activityreliclooting.hpp"
#include "other/cycleactivity/lanternriddie/lanternriddie.hpp"
#include "other/cycleactivity/kejuexam/kejuexam.hpp"
#include "global/team/teamconfig.hpp"
#include "task/taskrecorder.h"

Team::Team() : m_limit_level_low(0), m_limit_level_high(MAX_ROLE_LEVEL), m_team_index(0), m_team_scene_key_inc(0),
	m_leader_index(0), m_cur_member_num(0), m_team_type(TEAM_TYPE_DEFAULT),
	m_auto_join(true), m_is_cross(false), m_check_type(TEAM_CHECK_TYPE_AUTO_JOIN),
	m_join_req_count(0), m_agree_enter_end_time(0u), m_enter_type(0),
	m_enter_type_param(0), m_enter_param1(0), m_enter_param2(0), 
	m_enter_param3(0), m_last_recruit_timestamp(0u), m_team_create_time(0u),
	leader_npc_seq_count(0), m_is_notice(true), m_auto_join_type(0), m_auto_join_type_param1(0),
	m_auto_join_p1(0), m_auto_join_time(0u), m_notice_toggle_time(0u), m_temp_group(NULL)
{
	memset(m_team_notice, 0, sizeof(m_team_notice));
	memset(m_enter_param, 0, sizeof(m_enter_param));
	this->ResetEnterFbInfo(false);

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		m_pos_list[i].Reset();
	}

	m_team_vote.SetTeam(this);

	this->ResetAutoJoinInfo();

	m_team_create_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	m_temp_group = new TemporaryChatGroup();
	m_temp_group->Init(this);

	m_team_allow_red_join = true;
	m_team_is_red_name = false;
	memset(m_member_kill_value, 0, sizeof(m_member_kill_value));
}

Team::Team(Team & team)
{
	m_limit_level_low = team.m_limit_level_low;
	m_limit_level_high = team.m_limit_level_high;
	//m_team_index = team->m_team_index;
	m_team_scene_key_inc = team.m_team_scene_key_inc;
	m_leader_index = team.m_leader_index;
	m_cur_member_num = team.m_cur_member_num;
	m_team_type = team.m_team_type;
	m_auto_join = team.m_auto_join;
	m_is_cross = team.m_is_cross;
	m_check_type = team.m_check_type;
	m_join_req_count = team.m_join_req_count;
	m_agree_enter_end_time = team.m_agree_enter_end_time;
	m_enter_type = team.m_enter_type;
	m_enter_type_param = team.m_enter_type_param;
	m_enter_param1 = team.m_enter_param1;
	m_enter_param2 = team.m_enter_param2;
	m_enter_param3 = team.m_enter_param3;
	m_last_recruit_timestamp = team.m_last_recruit_timestamp;
	m_team_create_time = team.m_team_create_time;
	leader_npc_seq_count = team.leader_npc_seq_count;
	m_is_notice = team.m_is_notice;
	m_auto_join_type = team.m_auto_join_type;
	m_auto_join_type_param1 = team.m_auto_join_type_param1;
	m_auto_join_p1 = team.m_auto_join_p1;
	m_auto_join_time = team.m_auto_join_time;
	m_notice_toggle_time = team.m_notice_toggle_time;
	m_leader_index = team.m_leader_index;
	m_cur_member_num = team.m_cur_member_num;
	memcpy(m_member_list, team.m_member_list, sizeof(m_member_list));
	memcpy(m_team_notice, team.m_team_notice, sizeof(m_team_notice));
	memcpy(m_enter_param, team.m_team_notice, sizeof(m_enter_param));
	this->ResetAutoJoinInfo();
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		m_pos_list[i] = team.m_pos_list[i];
	}

	m_team_vote.SetTeam(this);

	m_temp_group = new TemporaryChatGroup();
	m_temp_group->Init(this);

	m_team_allow_red_join = team.m_team_allow_red_join;
	m_team_is_red_name = team.m_team_is_red_name;
	memcpy(m_member_kill_value, team.m_member_kill_value, sizeof(m_member_kill_value));
}

Team::~Team()
{
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
	{
		if (m_member_list[i].Invalid()) continue;

#ifdef _DEBUG
		assert(0);
#endif
		/*
		if (!m_member_list[i].IsRobot())
		{
			Role* role = this->GetMemberRoleByIndex(i);
			if (NULL != role)
			{
				role->SetTeam(NULL);
			}
		}
		else
		{
			Robot* robot = this->GetMemberRobotByIndex(i);
			if (NULL != robot)
			{
				robot->SetTeam(NULL);
				robot->SetDelayDelete(); // 所有机器人从队伍中退出，都要回收掉
			}
		}	
		*/
	}

	if (NULL != m_temp_group)
	{
		delete m_temp_group;
		m_temp_group = NULL;
	}
}

int Team::UserLogin(Role * user)
{
	Role * leader_role = this->GetLeaderRole();
	if (NULL == leader_role) this->AutoSetLeader();

	int member_index = -1;
	TeamMember* member = this->GetMemberInfoByUID(user->GetUID(), &member_index);
	if (NULL != member)
	{
		member->is_online = true;
		//member->role = user;
		member->is_follow = member->is_follow_before_logout;
		user->SetTeam(this);

		leader_role = this->GetLeaderRole();
		if (member->is_follow && leader_role != NULL && leader_role->GetUID() != user->GetUID())
		{
			// 尝试传送到队长身边
			bool is_goto_leader_scene = true;
			Scene* scene = leader_role->GetScene();
			if (NULL != scene && scene->CanEnter(user, false))
			{
				if (user->GetScene()->GetIndex() != scene->GetIndex())
				{
					if ((!scene->IsInStaticScene() && !scene->GetSpecialLogic()->CanTeamMemberGotoLeaderFromOtherScene(user, leader_role))
						|| !user->GetScene()->IsInStaticScene())
					{
						// 当不在同一个场景中，且任意一方场景是个副本，则不能传送
						is_goto_leader_scene = false;
					}
				}
			}
			else
			{
				is_goto_leader_scene = false;
			}

			if (is_goto_leader_scene)
			{
				member->is_follow = true;
				bool ret = this->TryFollowRole(user, leader_role, true);
				if (!ret)
				{
					user->NoticeNum(errornum::EN_TEAM_MEMBER_MUST_TEMP_APART);
					member->is_follow = false;
					this->SendStepOutInfoToMember(user);
				}
				else
				{
					Role* new_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
					if (NULL != new_role)
					{
						EventHandler::Instance().OnRoleRejoinTeam(new_role, this);
					}
				}
			}
			else
			{
				user->NoticeNum(errornum::EN_TEAM_MEMBER_MUST_TEMP_APART);
				member->is_follow = false;
				this->SendStepOutInfoToMember(user);
			}
		}
		else if (leader_role != NULL && leader_role->GetUID() == user->GetUID())
		{
			member->is_follow = true;
			this->SendUpdateFollowListMsg(user, user);
		}

		if (member->IsFollow())
		{	
			// 维护一下PosList
			int empty_pos = this->GetOnePosForMember();
			if (empty_pos != -1)
			{
				this->FillOnePosWithMember(empty_pos, member_index);
			}
		}
	}

	this->OnSendTeamLeaderInfo(Protocol::TEAM_LEADER_NPC_REQ_TYPE_LOAD, user);
	this->SendTeamInfo();

	if (NULL != m_temp_group)
	{
		m_temp_group->UserJoinGroup(user);
		m_temp_group->OnUserLogin(user);
	}
	return 0;
}

int Team::UserLogout(Role * user)
{
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
	{
		if (m_member_list[i].Invalid()) continue;

		if (user->GetUID() == m_member_list[i].uid)
		{
			m_member_list[i].is_follow_before_logout = m_member_list[i].is_follow;
			m_member_list[i].is_online = false;
			m_member_list[i].is_follow = false;
			//m_member_list[i].role = NULL;

			this->OnStopFollow(user);

			{
				// 维护一下PosList
				for (int idx = 0; idx < MAX_TEAM_MEMBER_NUM; ++idx)
				{
					if (m_pos_list[idx].IsValid() && m_pos_list[idx].id == user->GetUID())
					{
						m_pos_list[idx].Reset();
						break;
					}
				}
			}

			break;
		}
	}

	// 如果队长下线了就将队长给一个在线的玩家
	if (user->GetUID() == this->GetLeaderUID())
	{
		bool is_user_in_battle = user->GetRoleStatusManager()->IsInBattleStatus();
		bool ret = this->AutoSetLeader(false, is_user_in_battle);
		if (!ret)
		{
			if (is_user_in_battle)
			{
				ret = this->AutoSetLeader(false, false);
			}

			if (!ret)
			{
				//记录队伍--只记录单人的
				if (this->GetMemberCount() == 1)
				{
					int scene_id = user->GetScene()->GetSceneID();
					int scene_key = user->GetScene()->GetSceneKey();
					TeamKeyRegister::Instance().RegisterTeam(this, scene_id, scene_key);
					this->DismissTeamHelper();
					return 0;
				}
			}
		}
	}
	SendStepOutInfoToMember(user);
	this->OnEnterFbRet(user, false);
	this->SendTeamInfo();

	if (NULL != m_temp_group)
	{
		m_temp_group->OnUserLogout(user);
	}
	return 0;
}

int Team::UserLevelUp(Role * user)
{
	TeamMember * member = this->GetMemberInfo(user);
	if (NULL != member)
	{
		member->level = user->GetLevel();
		this->SendTeamMemberBaseInfoChangeNotice(user);
	}
	return 0;
}

void Team::Update(unsigned long interval, time_t now_second)
{
	this->NoticeToggleUpdate(now_second);
	this->AutoJoinUpdate(now_second);

	if (m_enter_type != AGREE_BEFORE_ENTER_TYPE_INVALID)
	{
		bool all_member_agree = true;		//回复拒绝的时候已经重置了，所以这里只有 ”同意“或者 “没应答”
		for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
		{
			if (!m_member_list[i].Invalid() && m_member_determine_flag[i] != ENTER_FB_DETERMINE_TYPE_YES)
			{
				all_member_agree = false;
				break;
			}
		}

		if (all_member_agree)
		{
			switch (m_enter_type)
			{
			case AGREE_BEFORE_ENTER_TYPE_FB:
				{
					FBManager::Instance().EnterFB(this->GetLeaderRole(), m_enter_type_param, m_enter_param1, m_enter_param2, false);
				}
				break;
			case AGREE_BEFORE_ENTER_TYPE_BATTLE:
				{
					BattleManagerLocal::GetInstance().StartBattleModeFightReq(this->GetLeaderRole(), m_enter_type_param, m_enter_param1, m_enter_param2, m_enter_param3, true);
				}
				break;
				case AGREE_BEFORE_ENTER_TYPE_TASK_BATTLE:
				{
					BattleManagerLocal::GetInstance().StartMonsterFightReq(this->GetLeaderRole(), m_enter_param1, m_enter_type_param, 0);
				}
				break;
			}
			this->ResetEnterFbInfo();
		}
		else
		{
			if (now_second >= m_agree_enter_end_time)//超时依旧进入
			{
				//秘境与远征默认倒计时结束自动进入
				bool is_notice = true;
				switch (m_enter_type)
				{
					case AGREE_BEFORE_ENTER_TYPE_FB:
					{
						if(FBManager::FB_TYPE_CRYSTAL_FB == m_enter_type_param || FBManager::FB_TYPE_MIYI == m_enter_type_param)
						{
							FBManager::Instance().EnterFB(this->GetLeaderRole(), m_enter_type_param, m_enter_param1, m_enter_param2, false);
							is_notice = false;
						}
					}
					break;
				default:
					break;
				}
				if(is_notice)
				{
					this->NoticeAllMember(errornum::EN_TEAM_NOT_READY_ERR);
				}
				this->ResetEnterFbInfo();
			/*	if (AGREE_BEFORE_ENTER_TYPE_FB == m_enter_type && FBManager::FB_TYPE_CRYSTAL_FB == m_enter_type_param)
				{
					FBManager::Instance().EnterFB(this->GetLeaderRole(), m_enter_type_param, m_enter_param1, m_enter_param2, false);
				}
				else
				{
					this->NoticeAllMember(errornum::EN_TEAM_NOT_READY_ERR);
				}
				this->ResetEnterFbInfo();*/
			}
		}		
	}

	m_team_vote.Update(interval, now_second);
}

void Team::EjectWindosControl(Role * role, int windows_type)
{
	if (!this->IsLeader(role))
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return;
	}

	Protocol::SCTeamLeaderEjectWindowsInfo msg;
	msg.windows_type = windows_type;
	this->SendToMember((const char*)&msg, sizeof(msg), false);
}

void Team::SetEnterFbInfo(Role * role, int enter_type, int enter_type_param, int param1, int param2, int param3)
{
	if (role == NULL)
	{
		return;
	}

	if (!this->IsLeader(role))
	{
		return;
	}

	//判断暂离
	if(!this->IsAllMemberFollow())
	{
		return;
	}
	
	m_enter_type = enter_type;
	m_enter_type_param = enter_type_param;
	m_enter_param1 = param1;
	m_enter_param2 = param2;
	m_enter_param3 = param3;
	m_agree_enter_end_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 10;

	if (AGREE_BEFORE_ENTER_TYPE_TASK_BATTLE == m_enter_type)
	{
		for (int i = 0; i < m_cur_member_num && i < MAX_TEAM_MEMBER_NUM; i++)
		{
			Role* role = this->GetMemberRoleByIndex(i);
			if (NULL != role)
			{
				m_enter_param[i] = role->GetRoleModuleManager()->GetSalary()->CalcTeamUserCurBattleGetChivSalary(m_enter_param1);
			}
		}
	}

	this->SendEnterInfo();

	this->RobotAgreeEnterFB();
}

void Team::OnEnterFbRet(Role * role, bool is_agree)
{
	if (role == NULL || m_enter_type == AGREE_BEFORE_ENTER_TYPE_INVALID)
	{
		return;
	}

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		if (m_member_list[i].uid == role->GetUID())
		{
			m_member_determine_flag[i] = is_agree ? ENTER_FB_DETERMINE_TYPE_YES : ENTER_FB_DETERMINE_TYPE_NO;

			Protocol::SCTeamEnterFbInfo protocol;
			protocol.member_index = i;
			protocol.is_determine = is_agree ? 1 : 0;

			this->SendToMember((char *)&protocol, sizeof(protocol));

			if (!is_agree)
			{
				this->ResetEnterFbInfo(false);
				for (int k = 0; k < m_cur_member_num; ++k)//飘字提示
				{
					Role* temp_role = this->GetMemberRoleByIndex(k);
					if (NULL != temp_role && temp_role->GetUID() != role->GetUID())
					{
						temp_role->GetRoleModuleManager()->NoticeNum(errornum::EN_TEAM_NOT_READY_ERR);
					}
				}
			}

			break;
		}
	}
}

void Team::RobotAgreeEnterFB()
{
	for (int i = 0; i < m_cur_member_num; ++i)
	{
		TeamMember* member = this->GetMemberInfoByIndex(i);
		if (NULL != member)
		{
			if (member->IsRobot() ||
				(TEAM_TYPE_ONE_DRAGON == m_team_type &&
					member->is_follow))
			{
				m_member_determine_flag[i] = ENTER_FB_DETERMINE_TYPE_YES;

				Protocol::SCTeamEnterFbInfo protocol;
				protocol.member_index = i;
				protocol.is_determine = 1;

				this->SendToMember((char *)&protocol, sizeof(protocol));
			}
		}
	}
}

void Team::ResetEnterFbInfo(bool is_notice)
{
	memset(m_member_determine_flag, ENTER_FB_DETERMINE_TYPE_PENDING, sizeof(m_member_determine_flag));
	m_agree_enter_end_time = 0;
	m_enter_type = AGREE_BEFORE_ENTER_TYPE_INVALID;
	m_enter_type_param = 0;
	m_enter_param1 = 0;
	m_enter_param2 = 0;
	m_enter_param3 = 0;
	memset(m_enter_param, 0, sizeof(m_enter_param));

	if (is_notice)
	{
		this->SendEnterInfo();
	}

}

void Team::SendEnterInfo()
{
	Protocol::SCTeamEnterRoute protocol;
	protocol.enter_type = m_enter_type;
	protocol.enter_type_param = m_enter_type_param;
	protocol.agree_enter_end_time =(unsigned int) m_agree_enter_end_time;
	protocol.param1 = m_enter_param1;
	protocol.param2 = m_enter_param2;
	protocol.param3 = m_enter_param3;

	if (AGREE_BEFORE_ENTER_TYPE_TASK_BATTLE == protocol.enter_type)
	{	
		for (int i = 0; i < m_cur_member_num && i < MAX_TEAM_MEMBER_NUM; ++i)
		{
			Role* role = this->GetMemberRoleByIndex(i);
			if (NULL != role)
			{
				protocol.param3 = m_enter_param[i];
				EngineAdapter::Instance().NetSend(role->GetNetId(), (const void *)&protocol, sizeof(protocol));
			}
		}
	}
	else
	{
		this->SendToMember((const void *)&protocol, sizeof(protocol));
	}
}

void Team::SetAutoInfoGotoHidden(int auto_join_type, int join_type_param1, int p1, const std::map<int, int>& all_member_roleid)
{
	m_auto_join_type = auto_join_type;
	m_auto_join_type_param1 = join_type_param1;
	m_auto_join_p1 = p1;
	m_need_wait_role_id_list = all_member_roleid;
	m_auto_join_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
}

void Team::AutoJoinUpdate(time_t now_second)
{
	if (m_auto_join_type == CROSS_AUTO_JION_TYPE_INVALID)
	{
		return;
	}

	std::map<int, int> tmp = m_need_wait_role_id_list;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		tmp.erase(m_member_list[i].uid);
	}

	if (tmp.empty())
	{
		if (m_auto_join_type == CROSS_AUTO_JION_TYPE_ACTIVITY)
		{
			this->AutoJoinActivityAfterTeamLoginHidden(m_auto_join_type_param1, m_auto_join_p1, 0);
		}
		
		this->ResetAutoJoinInfo();
	}

	if (m_auto_join_time != 0 && now_second > m_auto_join_time + AUTO_JOIN_LIMIT_TIME)
	{
		this->ResetAutoJoinInfo();

		Role * leader = this->GetLeaderRole();
		if (leader != NULL)
		{
			leader->NoticeNum(errornum::EN_TEAM_AUTO_JOIN_FAIL);
		}
	}
}

void Team::ResetAutoJoinInfo()
{
	m_auto_join_type = CROSS_AUTO_JION_TYPE_INVALID;
	m_auto_join_type_param1 = 0;
	m_auto_join_p1 = 0;
	m_need_wait_role_id_list = std::map<int, int>();
	m_auto_join_time = 0;
}

bool Team::AutoJoinActivityAfterTeamLoginHidden(int activity_type, int p1, int p2)
{
	switch (activity_type)
	{
		case ACTIVITY_TYPE_MAZE_RACER:
			{
				return FBManager::Instance().EnterFB(this->GetLeaderRole(), FBManager::FB_TYPE_MAZE_RACER, p1, p2);
			}
			break;
		case ACTIVITY_TYPE_BRAVE_GROUND:
			{
				return FBManager::Instance().EnterFB(this->GetLeaderRole(), FBManager::FB_TYPE_BRAVE_GROUND, p1, p2);
			}
			break;
		case ACTIVITY_TYPE_GUILD_ANSWER:
		case ACTIVITY_TYPE_GUILD_FIGHT:
		case ACTIVITY_TYPE_TEAM_FIGHT:
			{
				Role * leader = this->GetLeaderRole();
				if (leader != NULL)
				{
					leader->GetRoleModuleManager()->GetRoleActivityManager()->OnActivityEnterReq2(activity_type);
					return true;
				}				
			}
			break;
		case ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG:
			{
				Role * leader = this->GetLeaderRole();
				ActivityHuanJieZhanChang * activity = (ActivityHuanJieZhanChang *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG);
				if (activity != NULL && leader != NULL)
				{
					activity->OnRoleMatchReq(leader);
					return true;
	}
			}
			break;
	}

	return false;
}

void Team::NoticeToggleUpdate(time_t now_second)
{
	if (m_notice_toggle_time != 0 && static_cast<unsigned int>(now_second) > m_notice_toggle_time)
	{
		m_notice_toggle_time = 0;
		this->SetNotice(!m_is_notice);
	}
}

void Team::SetNotice(bool flag, time_t t)
{
	m_is_notice = flag;
	m_notice_toggle_time = 0;

	if (t != 0)
	{
		m_notice_toggle_time = static_cast<unsigned int>(EngineAdapter::Instance().Time() + t);
	}	
}

bool Team::IsNotice()
{
	return m_is_notice;
}

void Team::NoticeAllMember(int errornum_id)
{
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)//飘字提示
	{
		Role* temp_role = this->GetMemberRoleByIndex(i);
		if (NULL != temp_role)
		{
			temp_role->GetRoleModuleManager()->NoticeNum(errornum_id);
		}
	}
}

bool Team::IsInFb()
{
	Role* leader_role = this->GetLeaderRole();
	if (NULL == leader_role) return false;

	Scene* scene = leader_role->GetScene();
	if (NULL == scene) return false;

	return !scene->IsInStaticScene();
}

void Team::CheckMemberItem(Role * leader, int item_id, int num)
{
	if (leader == NULL)return;

	if (this->GetLeaderUID() != leader->GetUID())
	{
		leader->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return;
	}

	Protocol::SCLeaderCheckMemberItemRet info;
	info.check_num = 0;
	for (int i = 0; i < m_cur_member_num; ++i)//飘字提示
	{
		Role* temp_role = this->GetMemberRoleByIndex(i);
		if (NULL != temp_role)
		{
			info.member_info[info.check_num].item_num = temp_role->GetRoleModuleManager()->GetKnapsack()->Count(item_id);
			info.member_info[info.check_num++].member_idx = i;
		}
	}
	if (info.check_num > 0)
	{
		EngineAdapter::Instance().NetSend(leader->GetNetId(), (const char*)&info, sizeof(info));
		//leader->GetRoleModuleManager()->NetSend((const char*)&info, sizeof(info) - ((MAX_TEAM_MEMBER_NUM - info.check_num) * sizeof(info.member_info)));
	}
}

int Team::GetLeaderLevel()
{
	TeamMember* leader = this->GetMemberInfoByIndex(m_leader_index);
	if (NULL == leader) return 0;

	return leader->level;
}

void Team::SetTeamIndex(int team_index)
{
	m_team_index = team_index;
}

void Team::SetTeamLimit(short limit_level_low, short limit_level_high)
{
	m_limit_level_low = limit_level_low;
	m_limit_level_high = limit_level_high;
}

void Team::SetTeamNotice(TeamNotice team_notice)
{
	memcpy(m_team_notice, team_notice, sizeof(GameName));
}

void Team::SetLeader(int index)
{
	if (index < 0 || index >= MAX_TEAM_MEMBER_NUM) return;

	std::swap(m_member_list[index], m_member_list[0]);

	this->OnLeaderChangeUpdateFollow(this->GetLeaderRole());

	EventHandler::Instance().OnRoleBecomeLeader(this->GetLeaderRole(), this);
	EventHandler::Instance().OnTeamChangeLeader(this->GetLeaderRole(), m_member_list[index].uid, this);

	this->SendApplyListToLeader();
}

bool Team::SetTeamType(int type)
{
	if (type == m_team_type) return false;

	m_team_type = type;

	return true;
}

bool Team::SetTeamAllowRedJoin(bool team_allow_red_join)
{
	if(m_team_allow_red_join == team_allow_red_join) return false;

	m_team_allow_red_join = team_allow_red_join;
	return true;
}

void Team::SetTeamCheckType(int check_type)
{
	if (check_type < 0 || check_type >= TEAM_CHECK_TYPE_MAX) return;

	m_check_type = check_type;
}

bool Team::AutoSetLeader(bool is_from_create /* = false */, bool find_not_in_battle /* = false */)
{
	bool is_auto_set_succ = false;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
	{
		Role* member_role = NULL;
		if (!m_member_list[i].Invalid() &&
			m_member_list[i].is_follow &&
			m_member_list[i].is_online &&
			!m_member_list[i].IsRobot() &&
			NULL != (member_role = this->GetMemberRoleByIndex(i)))
		{
			if (find_not_in_battle)
			{
				if (!member_role->GetRoleStatusManager()->IsInBattleStatus())
				{
					continue;
				}
			}
			is_auto_set_succ = true;
			std::swap(m_member_list[i], m_member_list[0]);
			this->OnLeaderChangeUpdateFollow(member_role);
			break;
		}
	}

	if (!is_from_create && is_auto_set_succ)
	{
		this->SendApplyListToLeader();
		this->OnTeamChangeLeader();
		TeamManager::Instance().OnTeamInfoChange(this);
	}

	if (is_auto_set_succ)
	{
		Role* leader = this->GetLeaderRole();
		if (NULL != leader)
		{
			if (this->IsNotice())
			{
				leader->NoticeNum(noticenum::NT_TEAM_ROLE_BECOME_LEADER);
			}

			EventHandler::Instance().OnRoleBecomeLeader(leader, this);
			EventHandler::Instance().OnTeamChangeLeader(leader, 0, this);
		}
	}

	return is_auto_set_succ;
}

bool Team::AddMember(Role* user, bool is_from_create /* = false */)
{
	if (NULL == user || m_cur_member_num >= MAX_TEAM_MEMBER_NUM) return false;

	if (!user->GetScene()->GetSpecialLogic()->CanEnterTeam(user, this, is_from_create))
	{
		user->NoticeNum(errornum::EN_NOW_STATUS_UN_JOIN_TEAM);
		return false;
	}

	if (NULL != user->GetMyTeam())
	{
		user->NoticeNum(errornum::EN_ROLE_IN_TEAM);

		gamelog::g_log_serious_error.printf(LL_WARNING, "Team::AddMember possible double team, role[%d,%s] backstack[%s]", user->GetUID(), user->GetName(),
			PrintStackBackTrace(false).c_str());

#ifdef _DEBUG
		assert(0);
#endif

		return false;
	}

	Role * leader = this->GetLeaderRole();
	if (!is_from_create && NULL == leader) return false;

	if (!m_team_allow_red_join && user->GetRoleModuleManager()->GetDuel()->IsRedName())
	{
		user->NoticeNum(errornum::EN_DUEL_TEAM_NOT_ALLOW_RED_JOIN);
		return false;
	}

	if (CrossConfig::Instance().IsHiddenServer() && !is_from_create && leader != NULL)
	{
		ActivityHuanJieZhanChang * activity = (ActivityHuanJieZhanChang *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG);
		if (NULL != activity && activity->GetRoleMatchState(leader->GetUID()) != ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_NONE)
		{
			user->NoticeNum(errornum::EN_TEAM_IN_HUAN_JIE_MATCHING_CANN_ADD);
			return false;
		}

		if (NULL != activity && activity->GetRoleMatchState(user->GetUID()) != ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_NONE)
		{
			user->NoticeNum(errornum::EN_ROLE_IN_HUAN_JIE_MATCHING);
			return false;
		}
	}

	if (CrossConfig::Instance().IsHiddenServer() && !is_from_create && leader != NULL)
	{
		ActivityRelicLooting * activity = (ActivityRelicLooting *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_RELIC_LOOTING);
		if (NULL != activity && activity->IsMatch(leader->GetUID()))
		{
			user->NoticeNum(errornum::EN_RELICLOOTING_TEAM_ADD_ERR);
			return false;
		}

		if (NULL != activity && activity->IsMatch(user->GetUID()))
		{
			user->NoticeNum(errornum::EN_RELICLOOTING_IN_MATCH_ERR);
			return false;
		}
	}

	if (NULL != leader && leader->GetScene()->GetIndex() != user->GetScene()->GetIndex() &&
		!leader->GetScene()->GetSpecialLogic()->CanEnterTeam(user, this, is_from_create))
	{
		user->NoticeNum(errornum::EN_TARGET_TEAM_NOW_NOT_AVALIABLE);
		return false;
	}

	if (!is_from_create && leader != NULL && this->GetTeamType() == TEAM_TYPE_SHAN_HAI_DENG_HUI)		//山海灯会判断
	{
		if (leader->GetRoleModuleManager()->GetLanternRiddie()->IsAnswering())
		{
			user->NoticeNum(errornum::EN_TEAM_CAN_NOT_JOIN_SHAN_HAI_DENG_HUI);
			leader->NoticeNum(errornum::EN_TEAM_CAN_NOT_JOIN_SHAN_HAI_DENG_HUI);
			return false;
		}
		else
		{
			if (leader->GetRoleModuleManager()->GetLanternRiddie()->CanSynTeam())		//同步题目信息
			{
				user->GetRoleModuleManager()->GetLanternRiddie()->SendNpcInfo(leader->GetRoleModuleManager()->GetLanternRiddie()->GetTeamNpc(),
					leader->GetRoleModuleManager()->GetLanternRiddie()->GetTeamQuestion());
			}
		}
	}

	if (!is_from_create && leader != NULL && this->GetTeamType() == TEAM_TYPE_KE_JU_EXAM)		//山海灯会判断
	{
		if (leader->GetRoleModuleManager()->GetKeJuExam()->IsAnswering())
		{
			user->NoticeNum(errornum::EN_TEAM_CAN_NOT_JOIN_KE_JU_EXAM);
			leader->NoticeNum(errornum::EN_TEAM_CAN_NOT_JOIN_KE_JU_EXAM);
			return false;
		}
		else
		{
			if (leader->GetRoleModuleManager()->GetKeJuExam()->CanSynTeam())		//同步题目信息
			{
				user->GetRoleModuleManager()->GetKeJuExam()->SetNpcQuestion(leader->GetRoleModuleManager()->GetKeJuExam()->GetCurTeamNpc(),
					leader->GetRoleModuleManager()->GetKeJuExam()->GetTeamQuestion());
			}
		}
	}

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)		// 是否已经在队伍里
	{
		if (user->GetUID() == m_member_list[i].uid)
		{
			this->SendTeamInfo();
			return true;
		}
	}

	int member_index = m_cur_member_num++;
	TeamMember & member_con = m_member_list[member_index];
	member_con.uid = user->GetUID();
	member_con.level = user->GetLevel();
	member_con.profession = user->GetProfession();
	member_con.avatar_type = user->GetAvatarType();
	member_con.headshot_id = user->GetHeadshotID();
	user->GetName(member_con.member_name);
	member_con.is_follow = true;
	member_con.is_online = true;
	member_con.appearance = user->GetRoleModuleManager()->GetAppearance();
	//member_con.role = user;
	member_con.notice_on_double_speed = true;

	this->SetMemberKillValue(member_index, user->GetRoleModuleManager()->GetDuel()->GetKillValue());

	user->SetTeam(this);
	TeamManager::Instance().OnRoleEnterTeam(user->GetUID(), this->GetTeamIndex());
	EventHandler::Instance().OnRoleJoinTeam(user, this);
	this->OnEnterFbRet(user, false);

	long long origin_uuid = user->GetUUID();
	int user_uid = user->GetUID();

	if (NULL != leader)
	{
		// 尝试传送到队长身边
		if (leader->GetUID() != user->GetUID())
		{
			bool is_goto_leader_scene = true;
			Scene* scene = leader->GetScene();
			if (user->GetRoleStatusManager()->IsInBattleStatus())
			{
				// 在战斗中加入队伍要暂离
				is_goto_leader_scene = false;
			}
			else if (NULL != scene && scene->CanEnter(user, false))
			{
				if (user->GetScene()->GetIndex() != scene->GetIndex())
				{
					if ((!scene->IsInStaticScene() && !scene->GetSpecialLogic()->CanTeamMemberGotoLeaderFromOtherScene(user, leader))
						|| !user->GetScene()->IsInStaticScene()) 
					{
						// 当不在同一个场景中，且任意一方场景是个副本，则不能传送
						is_goto_leader_scene = false;
					}
				}
			}
			else
			{
				is_goto_leader_scene = false;
			}

			if (is_goto_leader_scene)
			{
				bool ret = this->TryFollowRole(user, leader);
				if (!ret)
				{
					user->NoticeNum(errornum::EN_TEAM_MEMBER_MUST_TEMP_APART);
					m_member_list[member_index].is_follow = false;
					SendStepOutInfoToMember(user);
				}
			}
			else
			{
				user->NoticeNum(errornum::EN_TEAM_MEMBER_MUST_TEMP_APART);
				m_member_list[member_index].is_follow = false;
				SendStepOutInfoToMember(user);
			}
		}
	}
	if (!is_from_create)
	{
		int empty_pos = this->GetOnePosForMember();
		if (empty_pos != -1)
		{
			this->FillOnePosWithMember(empty_pos, member_index);
		}
	}

	this->RemoveApplyInfo(user_uid);
	if (CrossConfig::Instance().IsHiddenServer())
	{
		int role_id = UidToInt(LongLongToUniqueUserID(origin_uuid).user_id);
		this->RemoveApplyInfo(role_id);
	}
	this->OnSendTeamLeaderInfo(Protocol::TEAM_LEADER_NPC_REQ_TYPE_LOAD, user);
	if (!is_from_create)
	{
		this->NoticeAddMember(user->GetName());
		this->SendTeamInfo();
	}

	if (NULL != m_temp_group)
	{
		m_temp_group->UserJoinGroup(user);
	}
	
	return true;
}

void Team::DismissTeamHelper()
{
	TeamMatch::Instance().OnTeamStopMatch(this);
	TeamManager::Instance().TeamDismiss(this);
	// modify by chenyulin ： 不再使用delete this 而是采用延迟delete
	//delete this;
}

int Team::OnTemporarilyPart(Role * user)
{
	if (NULL == user) return -1;
	int user_index = -1;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
	{
		if (m_member_list[i].uid == user->GetUID())
		{
			user_index = i;
			break;
		}
	}

	if (!user->GetScene()->IsInStaticScene()  
		&& user->GetScene()->GetSceneType() != Scene::SCENE_TYPE_RELIC_LOOTING  //遗迹夺宝允许暂离
		&& user->GetScene()->GetSceneType() != Scene::SCENE_TYPE_QINGYUAN_DUIDUIPENG
		&& user->GetScene()->GetSceneType() != Scene::SCENE_TYPE_TU_ZI_KUAI_PAO
		)
	{
		user->NoticeNum(errornum::EN_CAN_NOT_TEMP_APART_IN_FB);
		return -1;
	}

	if (user_index < 0) return -1;
	if (m_member_list[user_index].is_follow)
	{
		m_member_list[user_index].is_follow = false;

		this->OnStopFollow(user);
		SendStepOutInfoToMember(user);

		/* 暂离不改变队员的站位
		{
			// 维护一下PosList
			bool has_check_1 = false;
			for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
			{
				if (!has_check_1 && m_pos_list[i].IsValid() && !m_pos_list[i].is_partner && m_pos_list[i].member_index == user_index)
				{
					m_pos_list[i].Reset();
					this->FillOnePosWithPartner(i);
					has_check_1 = true;
				}
			}
		}
		*/
	}
	else
	{
		m_member_list[user_index].is_follow = true;

		// 尝试传送到队长身边
		Role* leader_role = this->GetLeaderRole();
		if (leader_role != NULL && leader_role->GetUID() != user->GetUID())
		{
			bool is_goto_leader_scene = true;
			Scene* scene = leader_role->GetScene();
			if (NULL != scene && scene->CanEnter(user, false))
			{
				if (user->GetScene()->GetIndex() != scene->GetIndex())
				{
					if ((!scene->IsInStaticScene() && !scene->GetSpecialLogic()->CanTeamMemberGotoLeaderFromOtherScene(user, leader_role))
						|| !user->GetScene()->IsInStaticScene())
					{
						// 当不在同一个场景中，且任意一方场景是个副本，则不能传送
						is_goto_leader_scene = false;
					}
				}
			}
			else
			{
				is_goto_leader_scene = false;
			}

			if (is_goto_leader_scene)
			{
				bool ret = this->TryFollowRole(user, leader_role);
				if (!ret)
				{
					user->NoticeNum(errornum::EN_TEAM_MEMBER_MUST_TEMP_APART);
					m_member_list[user_index].is_follow = false;
					SendStepOutInfoToMember(user);
				}
				else
				{
					Role* new_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(m_member_list[user_index].uid));
					if (NULL != new_role)
					{
						EventHandler::Instance().OnRoleRejoinTeam(new_role, this);
					}
				}
			}
			else
			{
				user->NoticeNum(errornum::EN_TEAM_MEMBER_MUST_TEMP_APART);
				m_member_list[user_index].is_follow = false;
				SendStepOutInfoToMember(user);
			}
		}

		{
			// 维护一下PosList
			int empty_pos = this->GetOnePosForMember();
			if (empty_pos != -1)
			{
				this->FillOnePosWithMember(empty_pos, user_index);
			}
		}
	}

	this->SendTeamInfo();
	return 0;
}

void Team::AddRobot()
{
	//!< \TODO 去掉这个方法，改为加真机器人

	Role* role = this->GetLeaderRole();
	if (NULL == role) return;

	static int robot_id = 1;
	robot_id < 10000 ? robot_id++ : robot_id = 1;

	m_member_list[m_cur_member_num].Reset();
	m_member_list[m_cur_member_num].uid = robot_id;
	m_member_list[m_cur_member_num].level = role->GetLevel();
	m_member_list[m_cur_member_num].profession = role->GetProfession();
	m_member_list[m_cur_member_num].avatar_type = role->GetAvatarType();
	m_member_list[m_cur_member_num].headshot_id = 0;

	SNPRINTF(m_member_list[m_cur_member_num].member_name, sizeof(GameName), "Robot_%d", robot_id);

	m_member_list[m_cur_member_num].is_follow = true;
	m_member_list[m_cur_member_num].is_online = true;
	m_member_list[m_cur_member_num].appearance = role->GetRoleModuleManager()->GetAppearance();
	//m_member_list[m_cur_member_num].role = NULL;

	m_cur_member_num++;


	int empty_pos = this->GetOnePosForMember();
	if (empty_pos != -1)
	{
		this->FillOnePosWithMember(empty_pos, m_cur_member_num - 1);
	}

	TeamManager::Instance().OnTeamInfoChange(this);

	this->SendTeamInfo();

	this->NoticeAddMember(m_member_list[m_cur_member_num].member_name);
}

void Team::AddRobot(Robot* robot)
{
	m_member_list[m_cur_member_num].Reset();
	m_member_list[m_cur_member_num].uid = robot->GetRobotID();
	m_member_list[m_cur_member_num].level = robot->GetLevel();
	m_member_list[m_cur_member_num].profession = robot->GetProfession();
	m_member_list[m_cur_member_num].avatar_type = robot->GetAvatarType();
	m_member_list[m_cur_member_num].headshot_id = 0;
	F_STRNCPY(m_member_list[m_cur_member_num].member_name, robot->GetName(), sizeof(GameName));
	
	m_member_list[m_cur_member_num].is_follow = true;
	m_member_list[m_cur_member_num].is_online = true;
	m_member_list[m_cur_member_num].appearance = robot->GetAppearance();
	//m_member_list[m_cur_member_num].role = NULL;
	//m_member_list[m_cur_member_num].robot = robot;

	m_cur_member_num++;

	robot->SetTeam(this);

	int empty_pos = this->GetOnePosForMember();
	if (empty_pos != -1)
	{
		this->FillOnePosWithMember(empty_pos, m_cur_member_num - 1);
	}

	TeamManager::Instance().OnTeamInfoChange(this);

	this->SendTeamInfo();
	this->TryFollowRole(robot, this->GetLeaderRole());

	this->NoticeAddMember(robot->GetName());
}

void Team::LeaveTeamByUid(int role_id, bool is_notice_team /* = true */)
{
	int leave_index = -1;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
	{
		if (m_member_list[i].uid == role_id)
		{
			leave_index = i;
			break;
		}
	}
	if (leave_index < 0) return;

	this->LeaveTeam(leave_index, is_notice_team);
}

void Team::LeaveTeam(const UserID& user_id, bool is_notice_team /* = true */)
{
	int leave_index = -1;
	int uid = UidToInt(user_id);
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
	{
		if (m_member_list[i].uid == uid)
		{
			leave_index = i;
			break;
		}
	}
	if (leave_index < 0) return;

	this->LeaveTeam(leave_index);
}

void Team::LeaveTeam(int member_index, bool is_notice_team /* = true */)
{
	if (member_index < 0 || member_index >= MAX_TEAM_MEMBER_NUM) return;
	if (m_member_list[member_index].Invalid())return;

	GameName leave_member_name;
	F_STRNCPY(leave_member_name, m_member_list[member_index].member_name, sizeof(GameName));

	bool is_robot = false;
	int member_uid = m_member_list[member_index].uid;
	if (!m_member_list[member_index].IsRobot())
	{
		is_robot = false;

		Role *role = this->GetMemberRoleByIndex(member_index);
		if (NULL != role)
		{
			static Protocol::SCOutOfTeam info;
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));

			if (member_index == m_leader_index)
			{
				static Protocol::SCTeamLeaderChangeNoticeArea msg;
				msg.change_objid = role->GetId();
				msg.is_leader = 0;
				role->GetScene()->GetZoneMatrix()->NotifyAreaMsg(role->GetObserHandle(), &msg, sizeof(msg));
			}

			bool is_follow = false;
			if (m_member_list[member_index].is_follow)
			{
				is_follow = true;
				this->OnStopFollow(role);
			}

			// 在副本中退队，直接飞出副本
			if (is_follow && this->IsInFb())
			{
				SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
				if (!scene_manager->CheckIsCommonFBScene(role->GetScene()->GetSceneID()))
				{
					role->GetScene()->GetSpecialLogic()->DelayKickOutRole(role);
				}
			}
		}
	}
	else
	{
		is_robot = true;
	}

	TeamManager::Instance().OnRoleExitTeam(m_member_list[member_index].uid);
	
	int switch_index = m_cur_member_num - 1;
	m_member_list[member_index].Reset();
	m_member_list[member_index] = m_member_list[switch_index];
	m_member_list[switch_index].Reset();
	--m_cur_member_num;

	this->SetMemberKillValue(member_index, 0);

	{
		// 维护一下PosList
		for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
		{
			if (m_pos_list[i].IsValid() && m_pos_list[i].member_index == member_index)
			{
				m_pos_list[i].Reset();
				break;
			}
		}

		for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i) // member_index移位后要更新pos_list中的信息
		{
			if (m_pos_list[i].IsValid() && m_pos_list[i].member_index == switch_index)
			{
				m_pos_list[i].member_index = member_index;
				break;
			}
		}
	}
	
	
	if (member_index == m_leader_index)
	{
		bool ret = this->AutoSetLeader();
		if (!ret)
		{
			this->DismissTeamHelper(); // 找不到继承队长的人，直接解散
			Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(member_uid);
			if (NULL != role)
			{
				EventHandler::Instance().OnRoleLeaveTeam(role, m_team_index, false);
			}
			return;
		}
	}
	else
	{
		this->OnMemberLeaveTeam();
	}

	if (is_notice_team)
	{
		this->NoticeMemberLeave(leave_member_name);
	}

	this->SendTeamInfo();


	if (!is_robot)
	{
		Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(member_uid);
		if (NULL != role)
		{
			role->SetTeam(NULL);
			EventHandler::Instance().OnRoleLeaveTeam(role, m_team_index, false);
		}

		if (NULL != m_temp_group)
		{
			m_temp_group->UserLeaveGroup(member_uid);
		}
	}
	else
	{
		Robot* robot = ROBOTMANAGER.GetRobot(member_uid);
		if (NULL != robot)
		{
			this->OnStopFollow(robot);
			robot->SetTeam(NULL);	 // 解除robot与team的绑定关系
			robot->OnLeaveTeam();
			robot->SetDelayDelete(); // 延迟删除逻辑  防止机器人被踢后不会清除
		}
	}
}

int Team::JoinReq(Role * user)
{
	unsigned int now_time = (unsigned int)EngineAdapter::Instance().Time();
	std::map<int, JoinReqInfo>::iterator it = m_join_req_map.find(user->GetUID());
	if (it != m_join_req_map.end())
	{
		if (it->second.req_tmestamp + 3 > now_time)
		{
			return -1;
		}
	}

	Protocol::SCJoinReq info;
	info.req_index = m_join_req_count++;
	info.uid = user->GetUID();
	info.level = user->GetLevel();
	info.profession = user->GetProfession();
	info.avatar_timestamp = user->GetAvatarTimestamp();
	info.avatar_type = user->GetAvatarType();
	info.headshot_id = user->GetHeadshotID();
	user->GetName(info.name);
	info.req_tmestamp = now_time;

	JoinReqInfo& user_info = m_join_req_map[user->GetUID()];
	user_info.req_index = info.req_index;
	user_info.uid = info.uid;
	user_info.level = info.level;
	user_info.profession = info.profession;
	user_info.avatar_timestamp = info.avatar_timestamp;
	user_info.avatar_type = info.avatar_type;
	user_info.headshot_id = info.headshot_id;
	user_info.req_tmestamp = info.req_tmestamp;
	user->GetName(user_info.name);
	user_info.is_cross_req = false;
	user_info.origin_plat_type = LocalConfig::Instance().GetPlatType();
	user_info.origin_server_id = LocalConfig::Instance().GetMergeServerId();

	Role* leader_role = this->GetLeaderRole();
	if (NULL != leader_role)
	{
		EngineAdapter::Instance().NetSend(leader_role->GetNetId(), (const char*)&info, sizeof(info));
	}

	return 0;
}

int Team::CrossJoinReq(int origin_uid, int origin_plat_type, int origin_server_id, int level, 
	int profession, long long avatar_timestamp, GameName name, short avatar_type, short headshot_id)
{
	unsigned int now_time = (unsigned int)EngineAdapter::Instance().Time();
	std::map<int, JoinReqInfo>::iterator it = m_join_req_map.find(origin_uid);
	if (it != m_join_req_map.end())
	{
		if (it->second.req_tmestamp + 3 > now_time)
		{
			return -1;
		}
	}

	Protocol::SCJoinReq info;
	info.req_index = m_join_req_count++;
	info.uid = origin_uid;
	info.level = level;
	info.profession = profession;
	info.avatar_timestamp = avatar_timestamp;
	info.avatar_type = avatar_type;
	info.headshot_id = headshot_id;
	F_STRNCPY(info.name, name, sizeof(GameName));
	info.req_tmestamp = now_time;

	JoinReqInfo& user_info = m_join_req_map[origin_uid];
	user_info.req_index = info.req_index;
	user_info.uid = info.uid;
	user_info.level = info.level;
	user_info.profession = info.profession;
	user_info.avatar_timestamp = info.avatar_timestamp;
	user_info.avatar_type = info.avatar_type;
	user_info.headshot_id = info.headshot_id;
	user_info.req_tmestamp = info.req_tmestamp;
	F_STRNCPY(user_info.name, info.name, sizeof(GameName));
	user_info.is_cross_req = true;
	user_info.origin_plat_type = origin_plat_type;
	user_info.origin_server_id = origin_server_id;
	
	Role* leader_role = this->GetLeaderRole();
	if (NULL != leader_role)
	{
		EngineAdapter::Instance().NetSend(leader_role->GetNetId(), (const char*)&info, sizeof(info));
	}

	return 0;
}

int Team::JoinReqRet(int uid, bool agree)
{
	Role* leader_role = this->GetLeaderRole();
	if (NULL == leader_role) return -1;

	std::map<int, JoinReqInfo>::iterator it = m_join_req_map.find(uid);
	if (it == m_join_req_map.end()) return -2;

	JoinReqInfo& info = it->second;

	int ret = 0;
	if (agree)		// 同意入队
	{
		int max_team_member_num = MAX_TEAM_MEMBER_NUM;
		if (TEAM_TYPE_QINGYUAN_DUIDUIPENG == m_team_type
			|| TEAM_TYPE_TU_ZI_KUAI_PAO == m_team_type
			|| TEAM_TYPE_CONCENTTRIC_KONT_1 == m_team_type ||
			TEAM_TYPE_CONCENTTRIC_KONT_2 == m_team_type ||
			TEAM_TYPE_CONCENTTRIC_KONT_3 == m_team_type)
		{
			max_team_member_num = 2;
		}

		int tmp_max_member_num = 0;
		if (LOGIC_CONFIG->GetTeamConfig()->IsTeamTypeLimitMaxMember(m_team_type, &tmp_max_member_num))
		{
			max_team_member_num = tmp_max_member_num;
		}

		if (m_cur_member_num >= max_team_member_num)		// 满员时
		{
			int robot_index = -1;
			if (!this->HasRobotMember(&robot_index))
			{
				leader_role->NoticeNum(errornum::EN_TEAM_IS_FULL);
				ret = -3;
			}
			else
			{
				this->LeaveTeam(robot_index);
			}
		}

		if (!info.is_cross_req)
		{
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(uid));
			if (NULL == role)
			{
				leader_role->NoticeNum(errornum::EN_TARGET_NOT_ONLINE);
				return -4;
			}
			else
			{
				if (!role->CanJoinTeam())
				{
					leader_role->NoticeNum(errornum::EN_NOW_STATUS_UN_JOIN_TEAM);
					return -6;
				}
				if (this->AddMember(role))
				{
					TeamManager::Instance().OnTeamInfoChange(this);
					//this->SendTeamInfo();
				}
			}
		}
		else
		{
			// 隐藏服 拉原服的人进队
			static crossgameprotocal::HiddenCrossTeammateJoinCrossTeam hctjct;
			hctjct.origin_team_index = -1;
			hctjct.cross_team_index = m_team_index;
			hctjct.origin_plat_type = info.origin_plat_type;
			hctjct.origin_server_id = info.origin_server_id;
			memset(hctjct.teammate_list, 0, sizeof(hctjct.teammate_list));
			hctjct.teammate_list[0] = info.uid;

			InternalComm::Instance().SendToCross((const char*)&hctjct, sizeof(hctjct));
		}
	}
	else
	{
		ret = -5;
	}

	this->RemoveApplyInfo(uid); // m_join_req_map.erase(it); it可能已被删掉 AddMember现在会删掉申请列表中玩家的申请
	return ret;
}

bool Team::IsLeader(Role * user)
{
	if (m_leader_index < 0 || m_leader_index >= MAX_TEAM_MEMBER_NUM)
	{
		this->AutoSetLeader();
		this->SendTeamInfo();
	}

	if (m_member_list[m_leader_index].uid == user->GetUID())return true;

	return false;
}

bool Team::IsMember(Role * user)
{
	if (NULL == user) return false;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		if (m_member_list[i].uid == user->GetUID())return true;
	}

	return false;
}

bool Team::IsMember(int uid)
{
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		if (m_member_list[i].uid == uid)return true;
	}

	return false;
}

int Team::GetLeaderUID()
{
	if (m_leader_index < 0 || m_leader_index >= MAX_TEAM_MEMBER_NUM)return 0;
	return m_member_list[m_leader_index].uid;
}

const char* Team::GetLeaderName()
{
	if (m_leader_index < 0 || m_leader_index >= MAX_TEAM_MEMBER_NUM) return NULL;
	return m_member_list[m_leader_index].member_name;
}

Role * Team::GetLeaderRole()
{
	Role* role = this->GetMemberRoleByIndex(m_leader_index);

	return role;
}

Role * Team::GetMemberRoleByIndex(int member_index)
{
	if (member_index < 0 || member_index >= MAX_TEAM_MEMBER_NUM) return NULL;
	if (m_member_list[member_index].Invalid() || 
		m_member_list[member_index].IsRobot() || 
		!m_member_list[member_index].is_online
		) return NULL;

	return World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(m_member_list[member_index].uid));

	/*
	if (NULL != m_member_list[member_index].role)
	{
#ifdef _DEBUG
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(m_member_list[member_index].uid));
		assert(role == m_member_list[member_index].role);
#endif
		return m_member_list[member_index].role;
	}
	else
	{
#ifdef _DEBUG
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(m_member_list[member_index].uid));
		assert(role == NULL);
#endif
		return NULL;
	}
	*/
}

Role* Team::GetLastFollowRole()
{
	for (int i = m_cur_member_num - 1; i >= 0; --i)
	{
		TeamMember* member = this->GetMemberInfo(i);
		if (NULL == member) continue;

		if (member->is_follow && member->is_online)
		{
			return this->GetMemberRoleByIndex(i);
		}
	}

	return NULL;
}

Robot* Team::GetMemberRobotByIndex(int member_index)
{
	if (member_index < 0 || member_index >= MAX_TEAM_MEMBER_NUM) return NULL;
	if (m_member_list[member_index].Invalid() || !m_member_list[member_index].IsRobot()) return NULL;

	return ROBOTMANAGER.GetRobot(m_member_list[member_index].uid);

	/*
	if (NULL != m_member_list[member_index].robot)
	{
#ifdef _DEBUG
		Robot* robot = ROBOTMANAGER.GetRobot(m_member_list[member_index].uid);
		assert(robot == m_member_list[member_index].robot);
#endif
		return m_member_list[member_index].robot;
	}
	else
	{
		return NULL;
	}
	*/
}

TeamMember * Team::GetMemberInfoByIndex(int member_index)
{
	if (member_index < 0 || member_index >= MAX_TEAM_MEMBER_NUM) return NULL;
	if (m_member_list[member_index].Invalid()) return NULL;
	return &m_member_list[member_index];
}

TeamMember* Team::GetMemberInfoByUID(int uid, ARG_OUT int* member_index)
{
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		if (m_member_list[i].Invalid()) continue;

		if (m_member_list[i].uid == uid)
		{
			if (NULL != member_index)
			{
				*member_index = i;
			}
			return &m_member_list[i];
		}
	}

	return NULL;
}

int Team::GetMemberCountExcludeRobot()
{
	int count = 0;
	for (int i = 0; i < m_cur_member_num; ++i)
	{
		TeamMember * member = this->GetMemberInfo(i);
		if (NULL == member) continue;

		if (!member->IsRobot())
		{
			++count;
		}
	}

	return count;
}

int Team::GetFollowMemberCount()
{
	int count = 0;
	for (int i = 0; i < m_cur_member_num; ++i)
	{
		TeamMember* member = this->GetMemberInfo(i);
		if (NULL == member) continue;

		if (member->is_follow)
		{
			++count;
		}
	}

	return count;
}

bool Team::GetTeamInfo(int * team_index, short * limit_level_low, short * limit_level_high, short * team_type)
{
	if (m_leader_index < 0 || m_leader_index >= MAX_TEAM_MEMBER_NUM || m_member_list[m_leader_index].Invalid()) return false;

	if (NULL != team_index) *team_index = m_team_index;
	if (NULL != limit_level_low) *limit_level_low = m_limit_level_low;
	if (NULL != limit_level_high) *limit_level_high = m_limit_level_high;
	if (NULL != team_type) *team_type = (short)m_team_type;

	return true;
}

TeamMember * Team::GetMemberInfo(Role * role)
{
	int index = -1;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
	{
		if (m_member_list[i].uid == role->GetUID())
		{
			index = i;
			break;
		}
	}
	if (index < 0)return NULL;

	return this->GetMemberInfo(index);
}

TeamMember * Team::GetMemberInfo(int index)
{
	if (index < 0 || index >= MAX_TEAM_MEMBER_NUM)return NULL;
	if (m_member_list[index].Invalid()) return NULL;
	return &m_member_list[index];
}

int Team::GetMemberIndex(Role * role)
{
	int index = -1;
	if (role)
	{
		index = this->GetMemberIndexByRoleId(role->GetUID());
	}

	return index;
}

int Team::GetMemberIndexByRoleId(int role_id)
{
	int index = -1;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
	{
		if (m_member_list[i].uid == role_id)
		{
			index = i;
			break;
		}
	}

	return index;
}

bool Team::CanAutoJoin()
{
	return TEAM_CHECK_TYPE_AUTO_JOIN == m_check_type || NULL != TeamMatch::Instance().GetMatchTeamInfo(this->GetTeamIndex());
}

bool Team::CanApplyJoin()
{
	return TEAM_CHECK_TYPE_NEED_CHECK == m_check_type;
}

bool Team::IsReachLevel(int role_level)
{
	return role_level >= m_limit_level_low && role_level <= m_limit_level_high;
}

void Team::SendTeamInfo(Role* user)
{
	Protocol::SCTeamInfo info;
	info.team_id = this->GetTeamIndex();
	info.limit_level_low = m_limit_level_low;
	info.limit_level_high = m_limit_level_high;
	info.team_type = m_team_type;
	info.leader_index = m_leader_index;
	info.check_type = static_cast<short>(m_check_type);
	info.is_cross = m_is_cross ? 1 : 0;
	info.allow_red_join = m_team_allow_red_join ? 1 : 0;
	info.member_count = 0;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
	{
		TeamMember * member = this->GetMemberInfo(i);
		if (NULL != member && !member->Invalid())
		{
			info.member_list[info.member_count].is_partner = 0;
			info.member_list[info.member_count].level = member->level;
			info.member_list[info.member_count].uid = member->uid;
			info.member_list[info.member_count].profession = member->profession;
			info.member_list[info.member_count].real_member_idx = i;
			info.member_list[info.member_count].show_member_idx = -1;
			if (i == m_leader_index)
			{
				info.member_list[info.member_count].is_follow = 0;
				info.member_list[info.member_count].is_online = 1;
			}
			else
			{
				info.member_list[info.member_count].is_follow = member->is_follow ? 1 : 0;
				info.member_list[info.member_count].is_online = member->is_online ? 1 : 0;
			}

			info.member_list[info.member_count].pos_index = -1;
			for (int k = 0; k < MAX_TEAM_MEMBER_NUM; ++k)
			{
				if (m_pos_list[k].IsValid() && m_pos_list[k].id == member->uid)
				{
					info.member_list[info.member_count].pos_index = k;
					info.member_list[info.member_count].show_member_idx = k;
					break;
				}
			}

			memcpy(info.member_list[info.member_count].member_name, member->member_name, sizeof(GameName));
			info.member_list[info.member_count].curr_hp = 0;
			info.member_list[info.member_count].max_hp = 0;
			info.member_list[info.member_count].curr_mp = 0;
			info.member_list[info.member_count].max_mp = 0;
			info.member_list[info.member_count].partner_skin_id = 0;
			info.member_list[info.member_count].pet_id = 0;
			info.member_list[info.member_count].pet_curr_hp = 0;
			info.member_list[info.member_count].pet_max_hp = 0;
			info.member_list[info.member_count].pet_curr_mp = 0;
			info.member_list[info.member_count].pet_max_mp = 0;
			info.member_list[info.member_count].pet_skin_id = 0;

			info.member_list[info.member_count].sub_partner_id = 0;
			info.member_list[info.member_count].sub_partner_cur_hp = 0;
			info.member_list[info.member_count].sub_partner_max_hp = 0;
			info.member_list[info.member_count].sub_partner_cur_mp = 0;
			info.member_list[info.member_count].sub_partner_max_mp = 0;

			info.member_list[info.member_count].appearance = member->appearance;

			if (!member->IsRobot())
			{
				Role * member_role = World::GetInstWorld()->GetSceneManager()->GetRole(member->uid);
				if (NULL != member_role)
				{
					info.member_list[info.member_count].curr_hp = member_role->GetRoleModuleManager()->GetAttrList()->m_attrs[BATTLE_ATTR_CUR_HP];
					info.member_list[info.member_count].max_hp = member_role->GetRoleModuleManager()->GetAttrList()->m_attrs[BATTLE_ATTR_MAX_HP];
					info.member_list[info.member_count].curr_mp = member_role->GetRoleModuleManager()->GetAttrList()->m_attrs[BATTLE_ATTR_CUR_MP];
					info.member_list[info.member_count].max_mp = member_role->GetRoleModuleManager()->GetAttrList()->m_attrs[BATTLE_ATTR_MAX_MP];

					const PetParam *  pet_param = member_role->GetRoleModuleManager()->GetPet()->GetFightPetParam();
					if (pet_param != NULL)
					{
						info.member_list[info.member_count].pet_id = pet_param->pet_id;
						info.member_list[info.member_count].pet_level = pet_param->level;
						info.member_list[info.member_count].pet_str_level = pet_param->str_level;
						info.member_list[info.member_count].pet_curr_hp = pet_param->hp;
						info.member_list[info.member_count].pet_max_hp = pet_param->max_hp;
						info.member_list[info.member_count].pet_curr_mp = pet_param->mp;
						info.member_list[info.member_count].pet_max_mp = pet_param->max_mp;
						info.member_list[info.member_count].pet_skin_id = pet_param->skin_id;
					}
				}
			}
			else
			{
				Robot* robot = RobotManager::Instance().GetRobot(member->uid);
				if (NULL != robot)
				{
					int cur_hp = robot->GetRoleBattleData().role_character_data.attr_list[BATTLE_ATTR_CUR_HP];
					int max_hp = robot->GetRoleBattleData().role_character_data.attr_list[BATTLE_ATTR_MAX_HP];
					int cur_mp = robot->GetRoleBattleData().role_character_data.attr_list[BATTLE_ATTR_CUR_MP];
					int max_mp = robot->GetRoleBattleData().role_character_data.attr_list[BATTLE_ATTR_MAX_MP];
					info.member_list[info.member_count].curr_hp = cur_hp;
					info.member_list[info.member_count].max_hp = max_hp;
					info.member_list[info.member_count].curr_mp = cur_mp;
					info.member_list[info.member_count].max_mp = max_mp;
				}
			}

			info.member_count++; 
		}
	}

	Role* leader_role = this->GetLeaderRole();
	if (NULL != leader_role && m_team_type != TEAM_TYPE_HUAN_JIE_ZHAN_CHANG) // 幻界战场不要伙伴显示
	{
		int partner_list[PARTNER_SYNFIGHT_NUM_MAX] = { 0 };
		int partner_pos_list[PARTNER_SYNFIGHT_NUM_MAX] = { 0 };
		int partner_num = 0;
		RoleFormation* rf = leader_role->GetRoleModuleManager()->GetRoleFormation();
		rf->GetCurFormationPartnerPosList(PARTNER_SYNFIGHT_NUM_MAX, &partner_num, partner_list, partner_pos_list);

		for (int i = 0; i < partner_num && i < PARTNER_SYNFIGHT_NUM_MAX; ++i)
		{
			int partner_id = partner_list[i];
			int pos = partner_pos_list[i];
			if (pos < 0 || pos >= MAX_TEAM_MEMBER_NUM) continue;

			if (info.member_count < MAX_TEAM_MEMBER_NUM && !m_pos_list[pos].IsValid())
			{
				// pos位置为空，则把对应位置的伙伴加进来
				info.member_list[info.member_count].real_member_idx = info.member_count;
				info.member_list[info.member_count].show_member_idx = pos;
				info.member_list[info.member_count].uid = partner_id;
				info.member_list[info.member_count].is_partner = 1;
				info.member_list[info.member_count].pos_index = pos;
				info.member_list[info.member_count].is_follow = 1;
				info.member_list[info.member_count].is_online = 1;

				Partner* partner = leader_role->GetRoleModuleManager()->GetPartner();
				const NewPartnerBaseInfo* pbi = partner->GetPartnerBaseInfoById(partner_id);
				if (NULL != pbi)
				{
					info.member_list[info.member_count].curr_hp = pbi->partner_data.cur_hp;
					info.member_list[info.member_count].max_hp = pbi->partner_data.max_hp;
					info.member_list[info.member_count].curr_mp = pbi->partner_data.cur_mp;
					info.member_list[info.member_count].max_mp = pbi->partner_data.max_mp;
					info.member_list[info.member_count].partner_skin_id = pbi->cur_skin_id;
					info.member_list[info.member_count].level = pbi->level;
				}			

				info.member_list[info.member_count].pet_id = 0;
				info.member_list[info.member_count].pet_curr_hp = 0;
				info.member_list[info.member_count].pet_max_hp = 0;
				info.member_list[info.member_count].pet_curr_mp = 0;
				info.member_list[info.member_count].pet_max_mp = 0;

				info.member_list[info.member_count].sub_partner_id = 0;
				info.member_list[info.member_count].sub_partner_cur_hp = 0;
				info.member_list[info.member_count].sub_partner_max_hp = 0;
				info.member_list[info.member_count].sub_partner_cur_mp = 0;
				info.member_list[info.member_count].sub_partner_max_mp = 0;

				info.member_count++;
			}
		}

		for (int i = 1; i < MAX_TEAM_MEMBER_NUM; ++i)
		{
			// 找到暂离的玩家
			if (0 == info.member_list[i].is_follow && 0 == info.member_list[i].is_partner)
			{
				int pos = info.member_list[i].pos_index;
				for (int k = 0; k < partner_num; ++k)
				{
					if (pos != partner_pos_list[k]) continue;

					int member_index = i;
					int partner_id = partner_list[k];
					info.member_list[member_index].sub_partner_id = partner_id;
					Partner* partner = leader_role->GetRoleModuleManager()->GetPartner();
					const NewPartnerBaseInfo* pbi = partner->GetPartnerBaseInfoById(partner_id);
					if (NULL != pbi)
					{
						info.member_list[member_index].sub_partner_cur_hp = pbi->partner_data.cur_hp;
						info.member_list[member_index].sub_partner_max_hp = pbi->partner_data.max_hp;
						info.member_list[member_index].sub_partner_cur_mp = pbi->partner_data.cur_mp;
						info.member_list[member_index].sub_partner_max_mp = pbi->partner_data.max_mp;
					}

					break;
				}
			}
		}
	}

	for (int i = 1; i < info.member_count; ++i)
	{
		if (-1 == info.member_list[i].show_member_idx)
		{
			//正常来说不会运行到这里，这里只是一道最后的保障
			bool flag[MAX_TEAM_MEMBER_NUM] = { 0 };
			for (int k = 1; k < MAX_TEAM_MEMBER_NUM; ++k)
			{
				if (info.member_list[k].show_member_idx >= 0 && info.member_list[k].show_member_idx < MAX_TEAM_MEMBER_NUM)
				{
					flag[info.member_list[k].show_member_idx] = true; // 标记哪些位置已被占用
				}
			}

			for (int k = 1; k < MAX_TEAM_MEMBER_NUM; ++k)
			{
				if (!flag[k])
				{
					info.member_list[i].show_member_idx = k; // 找到未被占用的并赋值
					break;
				}
			}

			if (-1 == info.member_list[i].show_member_idx)
			{
#ifdef _DEBUG
				assert(0);
#else
				gamelog::g_log_serious_error.printf(LL_ERROR, "Team::SendTeamInfo no place to stand...");
#endif
			}
		}
	}

	if (NULL == user)
	{
		this->SendToMember((const char*)&info, sizeof(info));
	}
	else
	{
		EngineAdapter::Instance().NetSend(user->GetNetId(), (const char*)&info, sizeof(info));
	}

	this->SendTeamInfoHuanJie();
}

void Team::SendTeamSimpleInfoChangeNotice(Role* user /*= NULL*/)
{
	Protocol::SCTeamSimpleInfoChangeNotice info;
	info.limit_level_low = m_limit_level_low;
	info.limit_level_high = m_limit_level_high;
	info.team_type = m_team_type;
	info.check_type = static_cast<short>(m_check_type);
	info.is_cross = m_is_cross ? 1 : 0;
	info.allow_red_join = m_team_allow_red_join ? 1 : 0;

	if (NULL == user)
	{
		this->SendToMember((const char*)&info, sizeof(info));
	}
	else
	{
		EngineAdapter::Instance().NetSend(user->GetNetId(), (const char*)&info, sizeof(info));
	}
}

void Team::SendTeamMemberBaseInfoChangeNotice(Role* member, Role* send_to_role /*= NULL*/)
{
	Protocol::SCTeamMemberBaseInfoChangeNotice info;
	info.member_info.uid = member->GetUID();
	info.member_info.level = member->GetLevel();
	info.member_info.profession = member->GetProfession();
	member->GetName(info.member_info.member_name);
	info.member_info.appearance = member->GetRoleModuleManager()->GetAppearance();

	if (NULL == send_to_role)
	{
		this->SendToMember((const char*)&info, sizeof(info));
	}
	else
	{
		EngineAdapter::Instance().NetSend(send_to_role->GetNetId(), (const char*)&info, sizeof(info));
	}
}

int Team::GetTeamSceneKey()
{
	int ret_scene_key = m_team_index * 10 + m_team_scene_key_inc;
	m_team_scene_key_inc = (m_team_scene_key_inc + 1) % 10;

	return ret_scene_key;
}

int Team::GetAllTeamMemberGuildID()
{
	Role * learder_role = this->GetLeaderRole();
	if (NULL == learder_role)return 0;
	int guild_id = learder_role->GetGuildID();
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
	{
		Role* role = this->GetMemberRoleByIndex(i);
		if (NULL != role)
		{
			if(guild_id != role->GetGuildID())guild_id = 0;//有成员不是同家族
		}
	}
	return guild_id;
}

int Team::GetMemberRoleListByFollow(Role ** list, int max)
{
	if (NULL == list) return 0;

	int count = 0;
	for (int i = 0; i < m_cur_member_num && count < max; ++i)
	{
		TeamMember* member = this->GetMemberInfoByIndex(i);
		if (NULL != member && member->is_follow && member->is_online)
		{
			Role * role = this->GetMemberRoleByIndex(i);
			if (NULL == role) continue;

			list[count++] = role;
		}
	}
	return count;
}

void Team::SendToMemberNotice(int notice_num, bool notice_leader)
{
	Protocol::SCNoticeNum info;
	info.notice_num = notice_num;

	this->SendToMember(&info, sizeof(info), notice_leader);
}

void Team::SendToMember(const void * info, int len,bool notie_leader)
{
	for (int i = 0; i < m_cur_member_num; ++i)
	{
		Role* role = this->GetMemberRoleByIndex(i);
		if (NULL != role && (notie_leader || !this->IsLeader(role)))
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), info, len);
		}
	}
}

void Team::SendToFollowMember(const void* info, int len, bool notie_leader /*= true*/)
{
	for (int i = 0; i < m_cur_member_num; ++i)
	{
		TeamMember* member_info = this->GetMemberInfoByIndex(i);
		if (NULL == member_info || (!member_info->is_follow && i != m_leader_index)) continue;

		Role* role = this->GetMemberRoleByIndex(i);
		if (NULL != role && (notie_leader || !this->IsLeader(role)))
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), info, len);
		}
	}
}

void Team::SendStepOutInfoToMember(Role *user)
{
	if (!this->IsNotice())
	{
		return;
	}

	for (int i = 0;i < MAX_TEAM_MEMBER_NUM;i++)
	{
		Role *role = this->GetMemberRoleByIndex(i);	
		if (NULL != role && role->GetUID() != user->GetUID())
		{
			role->NoticeNum(errornum::EN_TEAM_MEMBER_STEP_OUT);
		}
	}
}

bool Team::HasDismissMember()
{
	for (int i = 0; i < m_cur_member_num; ++i)
	{
		if (m_member_list[i].Invalid())
		{
			continue;
		}

		if (!m_member_list[i].is_follow)
		{
			return true;
		}
	}

	return false;
}

bool Team::HasRobotMember(ARG_OUT int* member_index)
{
	for (int i = 0; i < m_cur_member_num; ++i)
	{
		if (m_member_list[i].Invalid())
		{
			continue;
		}

		if (m_member_list[i].IsRobot())
		{
			if (NULL != member_index)
			{
				*member_index = i;
			}
			return true;
		}
	}

	return false;
}

int Team::GetRobotMemberNum()
{
	int num = 0;
	for (int i = 0; i < m_cur_member_num; ++i)
	{
		if (m_member_list[i].Invalid())
		{
			continue;
		}

		if (m_member_list[i].IsRobot())
		{
			++num;
		}
	}

	return num;
}

bool Team::IsAllMemberFollow(bool is_notice)
{
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
	{
		if (m_member_list[i].Invalid())
		{
			continue;
		}

		if (!m_member_list[i].is_follow || !m_member_list[i].is_online)
		{
			if (is_notice)
			{
				Role * learder = this->GetLeaderRole();
				if (learder != NULL)
				{
					learder->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
				}
			}

			return false;
		}
	}

	return true;
}

bool Team::IsAllMemberEnoughLevel(int require_level, bool is_notice)
{
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember *   member = this->GetMemberInfo(i);
		if (member != NULL)
		{
			if (member->level < require_level)
			{
				if (is_notice)
				{
					Role * learder = this->GetLeaderRole();
					if (learder != NULL)
					{
						learder->NoticeNum(errornum::EN_MEMBER_NOT_ENOUGH_LEVEL);
					}
				}

				return false;
			}

		}
	}

	return true;
}

bool Team::IsAllMemberLevelLess(int require_level, bool is_notice)
{
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember *   member = this->GetMemberInfo(i);
		if (member != NULL)
		{
			if (member->level > require_level)
			{
				if (is_notice)
				{
					Role * learder = this->GetLeaderRole();
					if (learder != NULL)
					{
						learder->NoticeNum(errornum::EN_MEMBER_NOT_ENOUGH_LEVEL);
					}
				}

				return false;
			}

		}
	}

	return true;

}

void Team::GetTeamItemInfo(TeamItem* out_ti)
{
	if (NULL == out_ti) return;

	out_ti->Reset();
	if (!this->GetTeamInfo(&out_ti->team_index, &out_ti->limit_level_low, &out_ti->limit_level_high, &out_ti->team_type))
	{
		return;
	}
	out_ti->is_cross = this->GetIsCross();
	out_ti->cur_member_num = this->GetMemberCount();
	out_ti->create_team_timestamp = this->GetTeamCreateTime();
	Role * leader_role = this->GetLeaderRole();
	if (leader_role != NULL)out_ti->guild_id = leader_role->GetGuildID();
	for (int i = 0; i < out_ti->cur_member_num && i < MAX_TEAM_MEMBER_NUM; i++)
	{
		TeamMember *member = this->GetMemberInfoByIndex(i);
		if (NULL != member)
		{
			out_ti->role_info[i].uid = member->uid;
			out_ti->role_info[i].level = member->level;
			out_ti->role_info[i].prof = member->profession;
			out_ti->role_info[i].avatar_type = member->avatar_type;
			out_ti->role_info[i].headshot_id = member->appearance.headshot_id;
			F_STRNCPY(out_ti->role_info[i].name, member->member_name, sizeof(GameName));
		}
	}
}

bool Team::SwitchPosition(Role* leader, short pos1, short pos2)
{
	if (pos1 < 1 || pos1 >= MAX_TEAM_MEMBER_NUM || pos2 < 1 || pos2 >= MAX_TEAM_MEMBER_NUM)
	{
		return false;
	}

	if (pos1 == pos2)
	{
		return true;
	}

	if (!m_pos_list[pos1].IsValid() || !m_pos_list[pos2].IsValid())
	{
		leader->NoticeNum(errornum::EN_TEAM_EMPTY_POSITION_CANT_SWITCH);
		return false;
	}

	std::swap(m_pos_list[pos1], m_pos_list[pos2]);

	this->SendTeamInfo(leader);

	return true;
}

void Team::OnMemberAppearanceChange(Role* role)
{
	for (int i = 0; i < this->GetMemberCount() && i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		if (m_member_list[i].uid == role->GetUID())
		{
			m_member_list[i].appearance = role->GetRoleModuleManager()->GetAppearance();

			static Protocol::SCTeamMemberAppearanceChange msg;
			msg.member_index = i;
			msg.new_appearance = m_member_list[i].appearance;

			this->SendToMember((const char*)&msg, sizeof(msg));

			return;
		}
	}

}

void Team::OnTeamCreate()
{
	Role* leader = this->GetLeaderRole();
	if (NULL != leader)
	{
		if (this->IsNotice())
		{
			leader->NoticeNum(noticenum::NT_TEAM_CREATE_SUCC);
		}

		static Protocol::SCTeamLeaderChangeNoticeArea msg;
		msg.change_objid = leader->GetId();
		msg.is_leader = 1;
		leader->GetScene()->GetZoneMatrix()->NotifyAreaMsg(leader->GetObserHandle(), &msg, sizeof(msg));

		//发聊天
		//int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
		//	gamestring::g_team_create, this->GetTeamIndex(), this->GetTeamType(),
		//	this->GetLimitLevelLow(), this->GetLimitLevelHigh(), 0, this->GetMemberCount());
		//if (length > 0)
		//{
		//	ChatManager::Instance().SystemSendRoleChannelChat(leader, chatdef::CHANNEL_TYPE_WORLD, gamestring::GAMESTRING_BUF, length);
		//}
	}
}

void Team::OnTeamChangeLeader()
{
	this->FillPosList();

	Role* leader = this->GetLeaderRole();
	if (NULL != leader)
	{			
		static Protocol::SCTeamLeaderChangeNoticeArea msg;
		msg.change_objid = leader->GetId();
		msg.is_leader = 1;
		leader->GetScene()->GetZoneMatrix()->NotifyAreaMsg(leader->GetObserHandle(), &msg, sizeof(msg));
		
		leader_npc_seq_count = 0;
		leader_npc_seq_list.clear();
		this->OnSendTeamLeaderInfo(Protocol::TEAM_LEADER_NPC_REQ_TYPE_LOAD, NULL, false, -1, true);
	}

	TeamMatch::Instance().OnTeamStopMatch(this);

	if (m_leader_index >= 0 && m_leader_index < MAX_TEAM_MEMBER_NUM)
	{
		this->NoticeLeaderChange(m_member_list[m_leader_index].member_name);
	}
}

void Team::OnMemberLeaveTeam()
{
	TeamManager::Instance().OnTeamInfoChange(this);
}

void Team::FillPosList()
{
	Role* role = this->GetLeaderRole();
	if (NULL == role) return;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		m_pos_list[i].Reset();
	}

	int pos_num = 0;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember* member = this->GetMemberInfo(i);
		if (NULL == member) continue;

		if (!member->IsFollow()) continue;
		if (!member->is_online) continue;

		m_pos_list[pos_num].id = member->uid;
		m_pos_list[pos_num].member_index = i;
		++pos_num;
	}	
}

/*
void Team::FillOnePosWithPartner(int pos_index)
{
	if (pos_index < 1 || pos_index >= MAX_TEAM_MEMBER_NUM) return;

	Role* role = this->GetLeaderRole();
	if (NULL == role) return;

	int partner_list[PARTNER_SYNFIGHT_NUM_MAX] = { 0 };
	int partner_pos_list[PARTNER_SYNFIGHT_NUM_MAX] = { 0 };
	int partner_num = 0;
	role->GetRoleModuleManager()->GetRoleFormation()->GetCurFormationPartnerPosList(PARTNER_SYNFIGHT_NUM_MAX, &partner_num, partner_list, partner_pos_list);

	for (int i = 0; i < partner_num && i < PARTNER_SYNFIGHT_NUM_MAX; ++i)
	{
		if (partner_pos_list[i] == pos_index)
		{
			m_pos_list[pos_index].id = partner_list[i];
			m_pos_list[pos_index].is_partner = true;
			m_pos_list[pos_index].member_index = -1;
			break;
		}
	}
}
*/

void Team::FillOnePosWithMember(int pos_index, int member_index)
{
	if (pos_index < 0 || pos_index >= MAX_TEAM_MEMBER_NUM) return;
	if (m_pos_list[pos_index].IsValid()) return;
	if (member_index < 0 || member_index >= MAX_TEAM_MEMBER_NUM || member_index >= m_cur_member_num) return;
	if (m_member_list[member_index].Invalid()) return;

	bool is_found = false;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		if (m_pos_list[i].IsValid() && m_pos_list[i].id == m_member_list[member_index].uid)
		{
			is_found = true;
			break;
		}
	}

	if (is_found) return;

	m_pos_list[pos_index].id = m_member_list[member_index].uid;
	m_pos_list[pos_index].member_index = member_index;
}

void Team::ResetOnePosByUid(int uid)
{
	for (int idx = 0; idx < MAX_TEAM_MEMBER_NUM; ++idx)
	{
		if (m_pos_list[idx].IsValid() && m_pos_list[idx].id == uid)
		{
			m_pos_list[idx].Reset();
			break;
		}
	}
}

int Team::GetEmptyPos()
{
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		if (m_pos_list[i].IsValid()) continue;

		return i;
	}

	return -1;
}

int Team::GetOnePosForMember()
{
	int partner_list[PARTNER_SYNFIGHT_NUM_MAX] = { 0 };
	int partner_pos_list[PARTNER_SYNFIGHT_NUM_MAX] = { 0 };
	int partner_num = 0;
	Role* leader = this->GetLeaderRole();
	if (NULL != leader)
	{
		RoleFormation* rf = leader->GetRoleModuleManager()->GetRoleFormation();
		rf->GetCurFormationPartnerPosList(PARTNER_SYNFIGHT_NUM_MAX, &partner_num, partner_list, partner_pos_list);
		
		bool occupied_flag[MAX_TEAM_MEMBER_NUM];
		memset(occupied_flag, 0, sizeof(occupied_flag));
		for (int i = 0; i < partner_num; ++i)
		{
			int pos = partner_pos_list[i];
			if (pos >= 0 && pos < MAX_TEAM_MEMBER_NUM)
			{
				occupied_flag[pos] = true;
			}
		}

		int first_avaliable_pos = -1;
		for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
		{
			if (m_pos_list[i].IsValid())
			{
				continue;
			}

			if (-1 == first_avaliable_pos)
			{
				first_avaliable_pos = i;
			}

			// 有空位就判断是否被伙伴占用
			if (!occupied_flag[i])
			{
				// 未被伙伴占用，就可以使用这个位置
				return i;
			}
		}

		if (-1 != first_avaliable_pos)
		{
			return first_avaliable_pos;
		}
	}
	else
	{
		for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
		{
			if (m_pos_list[i].IsValid())
			{
				continue;
			}

			// 有空位就直接返回
			return i;
		}
	}

	// 无空位就返回-1
	return -1;
}

void Team::SendPosInfo(Role* leader_role)
{
	/*
	if (NULL == leader_role) return;

	static Protocol::SCTeamPosInfo info;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		info.pos_list[i].id = m_pos_list[i].id;
		info.pos_list[i].is_partner = m_pos_list[i].is_partner ? 1 : 0;
		info.pos_list[i].reserve_sh = 0;
	}

	EngineAdapter::Instance().NetSend(leader_role->GetNetId(), (const char*)&info, sizeof(info));
	*/
}

int Team::GetFollowMemberMinLevel()
{
	int min_level = MAX_ROLE_LEVEL;
	for (int i = 0; i < m_cur_member_num && i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		if (m_member_list[i].Invalid() || !m_member_list[i].is_follow || !m_member_list[i].is_online) continue;

		min_level = GetMin(min_level, m_member_list[i].level);
	}

	return min_level;
}
int Team::GetFollowMemberMaxLevel()
{
	int max_level = 0;
	for (int i = 0; i < m_cur_member_num && i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		if (m_member_list[i].Invalid() || !m_member_list[i].is_follow || !m_member_list[i].is_online) continue;

		max_level = GetMax(max_level, m_member_list[i].level);
	}

	return max_level;
}

int Team::GetFollowMemberMaxLevelDiff()
{
	if(m_cur_member_num <= 1) return 0;

	int min_level = MAX_ROLE_LEVEL;
	int max_level = 0;
	for (int i = 0; i < m_cur_member_num && i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		if (m_member_list[i].Invalid() || !m_member_list[i].is_follow || !m_member_list[i].is_online) continue;

		min_level = GetMin(min_level, m_member_list[i].level);
		max_level = GetMax(max_level, m_member_list[i].level);
	}
	return max_level - min_level;
}

bool Team::IsPartnerInTeam(int partner_id)
{
	Role* leader = this->GetLeaderRole();
	if (NULL == leader)
	{
		return false;
	}
	
	int partner_list[PARTNER_SYNFIGHT_NUM_MAX] = { 0 };
	int partner_pos_list[PARTNER_SYNFIGHT_NUM_MAX] = { 0 };
	int partner_num = 0;
	RoleFormation* rf = leader->GetRoleModuleManager()->GetRoleFormation();
	rf->GetCurFormationPartnerPosList(PARTNER_SYNFIGHT_NUM_MAX, &partner_num, partner_list, partner_pos_list);

	for (int i = 0; i < partner_num; ++i)
	{
		if (partner_id != partner_list[i]) continue;

		int pos = partner_pos_list[i];
		if (pos < 0 || pos >= ARRAY_LENGTH(m_pos_list)) return false;
		
		if (m_pos_list[pos].IsValid()) return false;
	}
	
	return true;
}

void Team::GetPosIdList(int* pos_id_list, int max_list_num, int is_need_partner)
{
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM && i < max_list_num; ++i)
	{
		if (m_pos_list[i].IsValid())
		{
			pos_id_list[i] = m_pos_list[i].id;
		}		
		else
		{
			pos_id_list[i] = 0;
		}
	}

	if (is_need_partner)
	{
		Role* leader = this->GetLeaderRole();
		if (NULL != leader)
		{
			int partner_list[PARTNER_SYNFIGHT_NUM_MAX] = { 0 };
			int partner_pos_list[PARTNER_SYNFIGHT_NUM_MAX] = { 0 };
			int partner_num = 0;
			RoleFormation* rf = leader->GetRoleModuleManager()->GetRoleFormation();
			rf->GetCurFormationPartnerPosList(PARTNER_SYNFIGHT_NUM_MAX, &partner_num, partner_list, partner_pos_list);
		
			for (int i = 0; i < partner_num; ++i)
			{
				int pos = partner_pos_list[i];
				if (pos >= 0 && pos < max_list_num && 0 == pos_id_list[pos])
				{
					pos_id_list[pos] = partner_list[i];
				}
			}
		}
	}
}

void Team::OnLeaderCurrentFormationChange(Role* leader)
{
	if (NULL == leader) return;

	this->SendTeamInfo();
}

void Team::OnRoleRunAwayFromBattle(Role* role, bool is_leave_team)
{
	TeamMember* member = this->GetMemberInfo(role);
	if (NULL == member) return;

	if (is_leave_team)
	{
		TeamManager::Instance().ExitTeam(role);
	}
	else
	{
		this->OnTemporarilyPart(role);
	}
}

void Team::OnRobotRunAwayFromBattle(Robot* robot, bool is_leave_team)
{
	TeamMember* member = this->GetMemberInfoByUID(robot->GetRobotID());
	if (NULL == member) return;

	if (is_leave_team)
	{
		this->LeaveTeam(robot->GetRobotUserID());
	}
}

void Team::OnTeamStartMatch()
{
	if (TEAM_CHECK_TYPE_REFUSE_ALL == m_check_type)
	{
		m_check_type = TEAM_CHECK_TYPE_NEED_CHECK;
		this->SendTeamSimpleInfoChangeNotice(this->GetLeaderRole());
		TeamManager::Instance().GetTeamChannel()->OnTeamInfoChange(this);
	}
}

void Team::OnTeamStopMatch()
{

}

bool Team::TryFollowRole(Character * follower, Role * leader, bool is_from_login/*=false*/)
{
	if (NULL == follower || NULL == leader) return false;

	bool ret = false;
	bool is_same_scene = (follower->GetScene()->GetIndex() == leader->GetScene()->GetIndex());
	if (is_same_scene)
	{
		if (Obj::OBJ_TYPE_ROLE == follower->GetObjType())
		{
			Role* follow_role = (Role*)follower;
			ret = follow_role->ResetPos(leader->GetPos().x, leader->GetPos().y);

			// 判断所有队员是否有这个人的视野，没有的话加上去
			for (int i = 0; i < m_cur_member_num && i < MAX_TEAM_MEMBER_NUM; ++i)
			{
				TeamMember* member = this->GetMemberInfo(i);
				if (NULL == member || member->uid == follow_role->GetUID() || member->IsRobot() || !member->IsFollow()) continue;

				Role* member_role = this->GetMemberRoleByIndex(i);
				if (NULL == member_role) continue;

				if (member_role->GetScene()->GetIndex() != follow_role->GetScene()->GetIndex()) continue;

				if (!member_role->IsRoleInSight(follow_role))
				{
					bool is_pos_in_sight = member_role->GetScene()->GetZoneMatrix()->IsPosInSight(member_role->GetAOIHandle(), follow_role->GetPos());
					if (is_pos_in_sight)
					{
						member_role->OnAOIEnterRole(follow_role);
					}
				}

				if (!follow_role->IsRoleInSight(member_role))
				{
					bool is_pos_in_sight = follow_role->GetScene()->GetZoneMatrix()->IsPosInSight(follow_role->GetAOIHandle(), member_role->GetPos());
					if (is_pos_in_sight)
					{
						follow_role->OnAOIEnterRole(member_role);
					}
				}		
			}
		}
		else if (Obj::OBJ_TYPE_ROBOT == follower->GetObjType())
		{
			Robot* follow_robot = (Robot*)follower;
			ret = follow_robot->ResetPos(leader->GetPos().x, leader->GetPos().y);
		}

		//if (is_from_login)
		//{
		//	this->SendUpdateFollowListMsg(follower, follower);
		//}

		static Protocol::SCAddFollow msg;
		msg.reserve_sh = 0;
		msg.follow_team_id = this->GetTeamIndex();
		msg.add_objid = follower->GetId();

		follower->GetScene()->GetZoneMatrix()->NotifyAreaMsg(follower->GetObserHandle(), &msg, sizeof(msg));
	}
	else
	{
		if (Obj::OBJ_TYPE_ROLE == follower->GetObjType())
		{
			Role* follow_role = (Role*)follower;
			ret = World::GetInstWorld()->GetSceneManager()->GoTo(follow_role, leader->GetScene()->GetSceneID(), leader->GetScene()->GetSceneKey(), leader->GetPos());
		}
	}

	return ret;
}

void Team::OnStopFollow(Character* follower)
{
	if (NULL == follower) return;

	follower->MoveStop(true);

	static Protocol::SCRemoveFollow msg;
	msg.follow_team_id = this->GetTeamIndex();
	msg.is_remove_all = 0;
	msg.remove_objid = follower->GetId();

	follower->GetScene()->GetZoneMatrix()->NotifyAreaMsg(follower->GetObserHandle(), &msg, sizeof(msg));
}

void Team::OnAllStopFollow(Role* leader)
{
	if (NULL == leader) return;

	static Protocol::SCRemoveFollow msg;
	msg.follow_team_id = this->GetTeamIndex();
	msg.is_remove_all = 1;
	msg.remove_objid = INVALID_OBJ_ID;

	leader->GetScene()->GetZoneMatrix()->NotifyAreaMsg(leader->GetObserHandle(), &msg, sizeof(msg));
}

void Team::OnLeaderChangeUpdateFollow(Role* leader)
{
	if (NULL == leader) return;

	this->SendUpdateFollowListMsg(leader);
}

void Team::SendUpdateFollowListMsg(Character* character, Role* send_to_role /* = NULL */)
{
	if (NULL == character) return;

	static Protocol::SCUpdateFollowList msg;
	memset(msg.follow_objid_list, -1, sizeof(msg.follow_objid_list));
	msg.reserve_sh = 0;
	msg.follow_team_id = this->GetTeamIndex();

	int member_count = this->GetMemberCount();
	int cnt = 0;
	for (int i = 0; i < member_count; ++i)
	{
		if (m_member_list[i].Invalid()) continue;
		if (!m_member_list[i].is_follow || !m_member_list[i].is_online) continue;

		if (!m_member_list[i].IsRobot())
		{
			Role* member_role = this->GetMemberRoleByIndex(i);
			if (NULL == member_role || member_role->GetScene()->GetIndex() != character->GetScene()->GetIndex()) continue;
			
			msg.follow_objid_list[cnt++] = member_role->GetId();
		}
		else
		{
			Robot* member_robot = this->GetMemberRobotByIndex(i);
			if (NULL == member_robot || member_robot->GetScene()->GetIndex() != character->GetScene()->GetIndex()) continue;
		
			msg.follow_objid_list[cnt++] = member_robot->GetId();
		}
	}

	if (NULL == send_to_role)
	{
		character->GetScene()->GetZoneMatrix()->NotifyAreaMsg(character->GetObserHandle(), &msg, sizeof(msg));
	}
	else
	{
		EngineAdapter::Instance().NetSend(send_to_role->GetNetId(), &msg, sizeof(msg));
	}
}

void Team::UpdateMemberRolePointer(Role* member_role)
{
	if (NULL == member_role) return;

	/*
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		if (!m_member_list[i].Invalid() && m_member_list[i].uid == member_role->GetUID())
		{
			m_member_list[i].role = member_role;
			return;
		}
	}
	*/
}

void Team::OnRoleLaunchLeaderVote(Role* member_role)
{
	if (NULL == member_role) return;

	if (this->IsLeader(member_role))
	{
		member_role->NoticeNum(errornum::EN_TEAM_VOTE_ROLE_IS_LEADER);
		return;
	}

	if (m_team_vote.IsVoting())
	{
		member_role->NoticeNum(errornum::EN_TEAM_VOTE_IS_ON);
		return;
	}

	m_team_vote.StartVote(member_role);

	member_role->NoticeNum(noticenum::NT_TEAM_VOTE_LAUNCH_SUCC);
}

void Team::OnRoleVote(Role* member_role, bool is_yes)
{
	if (NULL == member_role) return;

	if (!m_team_vote.IsVoting())
	{
		member_role->NoticeNum(errornum::EN_TEAM_VOTE_IS_OFF);
		return;
	}

	m_team_vote.OnRoleVote(member_role, is_yes);
}

int Team::GetAverageLevel()
{
	if (m_cur_member_num == 0)
	{
		return 0;
	}
	int level = 0;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM && i < m_cur_member_num; i++)
	{
		level += m_member_list[i].level;
	}
	return level / m_cur_member_num;
}

void Team::OnTeamLeaderLoadNPC(const char * msg)
{
	Protocol::CSTeamLeaderNpcReq * msg_real = (Protocol::CSTeamLeaderNpcReq*)msg;
	if (NULL == msg_real || msg_real->npc_count <= 0)return;
	leader_npc_seq_count = 0;
	leader_npc_seq_list.clear();
	for (int i= 0; i < Protocol::TEAM_LEADER_NPC_SEQ_COUNT && i < msg_real->npc_count;i++)
	{
		if (msg_real->npc_seq_list[i] == 0)continue;

		leader_npc_seq_list.push_back(msg_real->npc_seq_list[i]);
		leader_npc_seq_count += 1;
	}
	this->OnSendTeamLeaderInfo(Protocol::TEAM_LEADER_NPC_REQ_TYPE_LOAD);
}

void Team::OnTeamLeaderDeleteNPC(const char * msg)
{
	Protocol::CSTeamLeaderNpcReq * msg_real = (Protocol::CSTeamLeaderNpcReq*)msg;
	if (NULL == msg_real || msg_real->npc_count <= 0)return;
	for (std::vector<int>::iterator it = leader_npc_seq_list.begin(); it != leader_npc_seq_list.end(); it++)
	{
		if (*it == msg_real->npc_seq_list[0])
		{
			this->OnSendTeamLeaderInfo(Protocol::TEAM_LEADER_NPC_REQ_TYPE_DELETE, NULL, true, *it);
			leader_npc_seq_list.erase(it);
			break;
		}
	}
	
}

void Team::OnTeamLeaderAddNPC(const char * msg)
{
	Protocol::CSTeamLeaderNpcReq * msg_real = (Protocol::CSTeamLeaderNpcReq*)msg;
	if (NULL == msg_real || msg_real->npc_count <= 0)return;
	leader_npc_seq_list.push_back(msg_real->npc_seq_list[0]);
	leader_npc_seq_count += 1;
	this->OnSendTeamLeaderInfo(Protocol::TEAM_LEADER_NPC_REQ_TYPE_ADD,NULL,true, msg_real->npc_seq_list[0]);
}

void Team::OnSendTeamLeaderInfo(int info_type,Role *member_role /* = NULL */,bool is_one /* = false */,int npc_seq /* = -1 */, bool send_to_leader /* = false */)
{
	static Protocol::SCTeamLeaderNpcInfo info;
	info.type = info_type;
	info.npc_count = 0;
	memset(info.npc_seq_list, 0, sizeof(info.npc_seq_list));
	if (is_one && npc_seq >= 0)
	{
		info.npc_count = 1;
		info.npc_seq_list[0] = npc_seq;
	}
	else
	{
		std::vector<int>::iterator it = leader_npc_seq_list.begin();
		for (; it != leader_npc_seq_list.end() && info.npc_count < Protocol::TEAM_LEADER_NPC_SEQ_COUNT; it++)
		{
			info.npc_seq_list[info.npc_count] = *it;
			info.npc_count += 1;
		}
	}

	if (NULL == member_role)
	{
		this->SendToFollowMember((const char*)&info, (sizeof(info) - ((Protocol::TEAM_LEADER_NPC_SEQ_COUNT - info.npc_count) * sizeof(info.npc_seq_list[0]))), send_to_leader);
	}
	else
	{
		if (!send_to_leader && this->IsLeader(member_role))return;

		EngineAdapter::Instance().NetSend(member_role->GetNetId(), (const char*)&info,
			(sizeof(info) - ((Protocol::TEAM_LEADER_NPC_SEQ_COUNT - info.npc_count) * sizeof(info.npc_seq_list[0]))));
	}
	
}

bool Team::IsRoleMemberFollow(int uid)
{
	TeamMember* member = this->GetMemberInfoByUID(uid);
	if (NULL == member) return false;

	return member->IsFollow();
}

void Team::SendLeaderBattlePartnerInfo(Role* role)
{
	if (NULL == role) return;

	Role* leader_role = this->GetLeaderRole();
	if (NULL == leader_role)
	{
		return;
	}

	int partner_list[PARTNER_SYNFIGHT_NUM_MAX] = { 0 };
	int partner_pos_list[PARTNER_SYNFIGHT_NUM_MAX] = { 0 };
	int partner_num = 0;
	RoleFormation* rf = leader_role->GetRoleModuleManager()->GetRoleFormation();
	rf->GetCurFormationPartnerPosList(PARTNER_SYNFIGHT_NUM_MAX, &partner_num, partner_list, partner_pos_list);

	Protocol::SCLeaderPartnerInfo msg;
	msg.num = 0;

	for (int i = 0; i < partner_num && i < PARTNER_SYNFIGHT_NUM_MAX && msg.num < ARRAY_ITEM_COUNT(msg.info); ++i)
	{
		int partner_id = partner_list[i];
		int pos = partner_pos_list[i];
		if (pos < 0 || pos >= ARRAY_ITEM_COUNT(m_pos_list)) continue;

		if (!m_pos_list[i].IsValid()) continue;

		Partner* partner = leader_role->GetRoleModuleManager()->GetPartner();
		Attribute cur_hp = partner->GetAttrByPartnerID(partner_id, BATTLE_ATTR_CUR_HP);
		Attribute max_hp = partner->GetAttrByPartnerID(partner_id, BATTLE_ATTR_MAX_HP);
		Attribute cur_mp = partner->GetAttrByPartnerID(partner_id, BATTLE_ATTR_CUR_MP);
		Attribute max_mp = partner->GetAttrByPartnerID(partner_id, BATTLE_ATTR_MAX_MP);

		msg.info[msg.num].curhp = cur_hp;
		msg.info[msg.num].curmp = cur_mp;
		msg.info[msg.num].maxhp = max_hp;
		msg.info[msg.num].maxmp = max_mp;
		msg.info[msg.num].partner_id = m_pos_list[i].id;
		msg.info[msg.num].member_index = m_pos_list[i].member_index;
		msg.num += 1;
	}

	int send_len = sizeof(msg) - sizeof(msg.info) + msg.num * sizeof(msg.info[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), &msg, send_len);
}

void Team::OnRoleResetName(Role* role)
{
	if (NULL == role) return;

	TeamMember* member = this->GetMemberInfoByUID(role->GetUID());
	if (NULL != member)
	{
		F_STRNCPY(member->member_name, role->GetName(), sizeof(GameName));
		this->SendTeamInfo();
	}
}

void Team::SendLeaderIncompatibleNotice(int member_index, int notice_type, int param1, int param2)
{
	Protocol::TeamMemberIncompatibleNotice protocol;
	protocol.notice_type = notice_type;
	protocol.team_member_index = member_index;
	protocol.param1 = param1;
	protocol.param2 = param2;

	Role * leader = this->GetLeaderRole();

	if (leader != NULL)
	{
		EngineAdapter::Instance().NetSend(leader->GetNetId(), &protocol, sizeof(protocol));
	}
}

void Team::SendAllMemberIncompatibleNotice(int member_index, int notice_type, int param1, int param2)
{
	Protocol::TeamMemberIncompatibleNotice protocol;
	protocol.notice_type = notice_type;
	protocol.team_member_index = member_index;
	protocol.param1 = param1;
	protocol.param2 = param2;

	this->SendToMember(&protocol, sizeof(protocol));
}

bool Team::CheckAllTeamMemberItem(int item_id, int num, bool check_dismiss_member)
{
	if (check_dismiss_member && this->HasDismissMember())
	{
		this->NoticeAllMember(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
		return false;
	}

	bool ret = true;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		Role * role = this->GetMemberRoleByIndex(i);
		if (role != NULL)
		{
			if (!role->GetRoleModuleManager()->GetKnapsack()->HasItem(item_id, num))
			{
				this->SendAllMemberIncompatibleNotice(i, Protocol::TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_ITEM, item_id, num);
				ret = false;
			}
		}		
	}

	return ret;
}

bool Team::CheckAllTeamMemberTaskRecord(int task_id, bool check_dismiss_member)
{
	if (check_dismiss_member && this->HasDismissMember())
	{
		this->NoticeAllMember(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
		return false;
	}

	bool ret = true;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		Role * role = this->GetMemberRoleByIndex(i);
		if (role != NULL)
		{
			if (!role->GetRoleModuleManager()->GetTaskRecorder()->HaveFinished(task_id))
			{
				this->SendAllMemberIncompatibleNotice(i, Protocol::TEAM_MEMBER_INCOMPATIBLE_NO_TASK_RECORD, task_id);
				ret = false;
			}
		}
	}

	return ret;
}

bool Team::ConsumeAllTeamMemberItem(int item_id, int num, const char * resone)
{
	if (!this->CheckAllTeamMemberItem(item_id, num))
	{
		return false;
	}

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		Role * role = this->GetMemberRoleByIndex(i);
		if (role != NULL)
		{
			role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(item_id, num, resone);
		}
	}

	return true;
}

void Team::NoticeMemberLevelNotEnough(const char * name, int level)
{
	if (NULL == name || 0 >= level) return;

	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_team_member_level_not_enough, name, level);
	if (length > 0)
	{
		static Protocol::SCSystemMsg sm;
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT)))
		{
			for (int i = 0; i < this->GetMemberCount(); ++i)
			{
				Role* role = this->GetMemberRoleByIndex(i);
				if (NULL != role)
				{
					EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
				}
			}
		}
	}
}

void Team::SaveFormationReq(Protocol::CSFormationSave* req)
{
	bool is_all_member_found = true;
	for (int i = 1; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		if (!m_pos_list[i].IsValid()) continue;

		bool is_found = false;
		for (int k = 0; k < req->team_member_num; ++k)
		{
			if (req->member_list[k].member_pos <= 0 
				|| req->member_list[k].member_pos >= MAX_TEAM_MEMBER_NUM) continue;

			if (req->member_list[k].member_uid != m_pos_list[i].id) continue;

			is_found = true;
			break;
		}

		if (!is_found)
		{
			is_all_member_found = false;
			break;
		}
	}

	if (!is_all_member_found) return;

	MemberPosInfo temp_pos_list[MAX_TEAM_MEMBER_NUM];
	temp_pos_list[0] = m_pos_list[0];

	for (int i = 0; i < req->team_member_num; ++i)
	{
		int pos = req->member_list[i].member_pos;
		int uid = req->member_list[i].member_uid;
		int member_index = -1;
		TeamMember* member = this->GetMemberInfoByUID(uid, &member_index);
		if (NULL == member) continue;

		temp_pos_list[pos].id = uid;
		temp_pos_list[pos].member_index = member_index;
	}

	memcpy(m_pos_list, temp_pos_list, sizeof(m_pos_list));

	this->SendTeamInfo();
}

void Team::OnTempGroupChat(const char* msg, int total_len)
{
	if (NULL != m_temp_group)
	{
		m_temp_group->OnGroupChat(msg, total_len);
	}
}

void Team::TempGroupBecomeOfficialReq(Role* role, GameName group_name)
{
	if (NULL == role) return;
	
	if (NULL != m_temp_group)
	{
		m_temp_group->BecomeOfficialChatGroup(role, group_name);
	}
}

void Team::OnGetTempGroupDetailInfoReq(Role* role)
{
	if (NULL != m_temp_group)
	{
		m_temp_group->GetGroupDetailInfo(role);
	}
}

void Team::LeaderViewHuanJie(int is_open)
{
	m_huan_jie_param.is_open = is_open;

	if (this->GetTeamType() != TEAM_TYPE_HUAN_JIE_ZHAN_CHANG)
	{
		this->SetTeamType(TEAM_TYPE_HUAN_JIE_ZHAN_CHANG);
		this->SendTeamInfo();
	}
	else
	{
		this->SendTeamInfoHuanJie();
	}
	
}

void Team::SendTeamInfoHuanJie()
{
	Protocol::SCHuanJieZhanChangTeamInfo protocol;
	protocol.is_open = m_huan_jie_param.is_open;

	this->SendToMember(&protocol, sizeof(protocol));
}

void Team::SetMemberKillValue(int member_index, int value)
{
	if (0 > member_index || member_index >= MAX_TEAM_MEMBER_NUM) return;
	if (0 > value) return;

	m_member_kill_value[member_index] = value;

	bool old_dirty = m_team_is_red_name;

	m_team_is_red_name = false;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		if (0 < m_member_kill_value[i])
		{
			m_team_is_red_name = true;
			break;
		}
	}

	if (!m_team_is_red_name && old_dirty == m_team_is_red_name)
	{
		//状态没发生改变 且不是红名队伍
		return;
	}

	static Protocol::SCTeamRedNameNotifyArea notice;
	notice.team_id = this->GetTeamIndex();
	notice.is_red = m_team_is_red_name ? 1 : 0;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		Role* temp_role = this->GetMemberRoleByIndex(i);
		if (NULL != temp_role)
		{
			if (m_team_is_red_name)
			{
				temp_role->NoticeNum(errornum::EN_DUEL_TEAM_IS_RED_NAME);
			}

			if (old_dirty != m_team_is_red_name)
			{
				temp_role->GetScene()->GetZoneMatrix()->NotifyAreaMsg(temp_role->GetObserHandle(), &notice, sizeof(notice));
			}
		}
	}
}

MsgRoleBaseInfo Team::GetMsgRoleBaseInfo(int role_id) 
{
	MsgRoleBaseInfo ret;

	TeamMember * team_member = this->GetMemberInfoByUID(role_id); 
	if (team_member != NULL)
	{
		ret.uid = team_member->uid;
		memcpy(ret.name, team_member->member_name, sizeof(ret.name));
		ret.role_level= team_member->level;
		ret.prof = team_member->profession;
		ret.avatar_type = team_member->avatar_type;
		ret.headshot_id = team_member->headshot_id;
	}
		
	return ret;
}

std::vector<MsgRoleBaseInfo> Team::GetAllMsgRoleBaseInfo()
{
	std::vector<MsgRoleBaseInfo> ret;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * team_member = this->GetMemberInfo(i);
		if (team_member != NULL)
		{
			MsgRoleBaseInfo tmp;
			tmp.uid = team_member->uid;
			memcpy(tmp.name, team_member->member_name, sizeof(tmp.name));
			tmp.role_level = team_member->level;
			tmp.prof = team_member->profession;
			tmp.avatar_type = team_member->avatar_type;
			tmp.headshot_id = team_member->headshot_id;

			ret.push_back(tmp);
		}
	}

	return ret;
}

void Team::OnRoleEnterTeamKickRealRoleRobot(int uid)
{
	for (int i = 0; i < this->GetMemberCount(); ++i)
	{
		Robot* robot = this->GetMemberRobotByIndex(i);
		if (NULL != robot && robot->GetRealRoleUID() == uid)
		{
			this->LeaveTeam(robot->GetRobotUserID());
		}
	}
}

bool Team::RemoveApplyInfo(int role_id)
{
	std::map<int, JoinReqInfo>::iterator it = m_join_req_map.find(role_id);
	if (it != m_join_req_map.end())
	{
		m_join_req_map.erase(it);

		static Protocol::SCTeamRemoveApply msg;
		msg.remove_uid = role_id;

		Role* leader = this->GetLeaderRole();
		if (NULL != leader)
		{
			EngineAdapter::Instance().NetSend(leader->GetNetId(), (const char*)&msg, sizeof(msg));
		}
	}
	
	return true;
}

void Team::SendApplyListToLeader()
{
	Role* leader = this->GetLeaderRole();
	if (NULL == leader) return;

	static Protocol::SCApplyList msg;
	msg.count = 0;

	std::map<int, JoinReqInfo>::iterator it = m_join_req_map.begin();
	for (; it != m_join_req_map.end() && msg.count < Protocol::SCApplyList::MAX_APPLY_LIST_SIZE; ++it)
	{
		msg.apply_list[msg.count].req_index = it->second.req_index;
		msg.apply_list[msg.count].uid = it->second.uid;
		msg.apply_list[msg.count].level = it->second.level;
		msg.apply_list[msg.count].profession = it->second.profession;
		msg.apply_list[msg.count].avatar_timestamp = it->second.avatar_timestamp;
		msg.apply_list[msg.count].req_tmestamp = it->second.req_tmestamp;
		F_STRNCPY(msg.apply_list[msg.count].name, it->second.name, sizeof(GameName));
		msg.apply_list[msg.count].avatar_type = it->second.avatar_type;
		msg.apply_list[msg.count].headshot_id = it->second.headshot_id;
		msg.count += 1;
	}

	if (msg.count > 0)
	{
		int send_len = sizeof(msg) - sizeof(msg.apply_list) + msg.count * sizeof(msg.apply_list[0]);
		EngineAdapter::Instance().NetSend(leader->GetNetId(), (const char*)&msg, send_len);
	}
}

void Team::NoticeMemberLeave(const char* name)
{
	if (!this->IsNotice())
	{
		return;
	}

	//发公告
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
		gamestring::g_team_member_leave, name);
	if (length > 0)
	{
		static Protocol::SCSystemMsg sm;
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT)))
		{
			for (int i = 0; i < this->GetMemberCount(); ++i)
			{
				Role* role = this->GetMemberRoleByIndex(i);
				if (NULL != role)
				{
					EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
				}
			}
		}
	}
}

void Team::NoticeAddMember(const char* name)
{
	if (!this->IsNotice())
	{
		return;
	}

	//发公告
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
		gamestring::g_team_add_member, name);
	if (length > 0)
	{
		static Protocol::SCSystemMsg sm;
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT)))
		{
			for (int i = 0; i < this->GetMemberCount(); ++i)
			{
				Role* role = this->GetMemberRoleByIndex(i);
				if (NULL != role)
				{
					EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
				}
			}
		}
	}
}

void Team::NoticeLeaderChange(const char* name)
{
	if (!this->IsNotice())
	{
		return;
	}

	//发公告
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
		gamestring::g_team_become_leader, name);
	if (length > 0)
	{
		static Protocol::SCSystemMsg sm;
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT)))
		{
			for (int i = 0; i < this->GetMemberCount(); ++i)
			{
				Role* role = this->GetMemberRoleByIndex(i);
				if (NULL != role)
				{
					EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
				}
			}
		}
	}
}

void Team::OnTeamInfoChangedCheckRealRoleRobotLeave()
{
	std::vector<UserID> leave_robots;
	for (int i = 0; i < this->GetMemberCount(); ++i)
	{
		Robot* robot = this->GetMemberRobotByIndex(i);
		if (NULL != robot && robot->GetRealRoleUID() > 0 
			&& robot->GetBoundTeamType() != this->GetTeamType())
		{
			leave_robots.push_back(robot->GetRobotUserID());
		}
	}

	for (size_t i = 0; i < leave_robots.size(); ++i)
	{
		this->LeaveTeam(leave_robots[i]);
	}
}

