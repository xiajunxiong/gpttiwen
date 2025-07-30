#include "speciallogicmazeracer.hpp"
#include "mazeracermanager.hpp"
#include "engineadapter.h"
#include "world.h"
#include "gamelog.h"
#include "scene/scene.h"
#include "npc/npcpool.h"
#include "obj/character/role.h"
#include "obj/character/npc.h"
#include "obj/otherobj/specialtransport.h"
#include "obj/otherobj/npcother.h"
#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/mazeracer/activitymazeracerconfig.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "global/team/teammanager.hpp"
#include "global/team/team.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activitymazeracer.hpp"
#include "other/robot/robotmanager.hpp"
#include "other/roleactivity/roleactivity.hpp"


SpecialLogicMazeRacer::SpecialLogicMazeRacer(Scene * scene) : SpecialLogic(scene), m_is_finish(false),m_copy_id(0), m_maze_id(0), m_layer(0), m_next_refresh_rank_timestamp(0),m_need_refresh_rank(false)
{
	m_start_time = 0;
}


SpecialLogicMazeRacer::~SpecialLogicMazeRacer()
{
}

bool SpecialLogicMazeRacer::CanDestroy()
{
	return 0 != m_is_finish;
}

void SpecialLogicMazeRacer::Update(unsigned long interval, time_t now_second)
{
	if (m_need_refresh_rank && now_second >= m_next_refresh_rank_timestamp)	// 发送排名
	{
		m_next_refresh_rank_timestamp = now_second + 5;		// 两个刷新间隔不能少于 5 秒
		m_need_refresh_rank = false;
		MazeRacerManager::Instance().SendEctypeRankInfo(m_copy_id);
	}
}

void SpecialLogicMazeRacer::OnRoleEnterScene(Role * role)
{
	Team * team = TeamManager::Instance().GetTeamByUid(role->GetUID());
	if (team == NULL)
	{
		this->DelayKickOutRole(role);
		return;
	}
	
	if (role->GetUID() == team->GetLeaderUID())
	{
		int team_index = team->GetTeamIndex();
		m_team_id_vec.push_back(team_index);	// 将队伍插入队伍列表内；

		if (1 == m_layer)		// 如果是第一层，刷新队伍状态，发送双信息
		{
			MazeRacerManager().Instance().RefreshTeamStatus(team_index);
			MazeRacerManager().Instance().SendAmuletNumToTeam(team_index);
			MazeRacerManager().Instance().AmuletEffectMessage(team_index);
			for (int i = 0; i < team->GetMemberCount() && i < MAX_TEAM_MEMBER_NUM; i++)
			{
				Role * member = team->GetMemberRoleByIndex(i);
				if (NULL != member)
				{
					EventHandler::Instance().OnParticipateLimitActivity(member, ACTIVITY_TYPE_MAZE_RACER, __FUNCTION__, team_index);
				}
			}
		}

		MazeRacerManager().Instance().OnTeamEnter(m_copy_id, m_layer, team_index);
		m_need_refresh_rank = true;

		this->SendAmuletInfo(role);
	}

	this->SendSceneInfo(role);	
	role->SetEncounterMonsterFlag(true);
}

void SpecialLogicMazeRacer::OnRoleLeaveScene(Role * role, bool is_logout)
{
	role->RestoreLastEncounterMonsterFlag();
	
	Team * team = role->GetMyTeam();		// 如果是带着队伍走的，删除场景内的队伍id
	if (NULL != team && role->GetUID() == team->GetLeaderUID())
	{
		for (std::vector<int>::iterator loop = m_team_id_vec.begin(); loop != m_team_id_vec.end(); ++loop)
		{
			if (team->GetTeamIndex() == *loop)
			{
				m_team_id_vec.erase(loop);
				break;
			}
		}

		for (std::set<int>::iterator it = m_pass_team_index.begin();it !=  m_pass_team_index.end(); ++it)
		{
			if (team->GetTeamIndex() == *it)
			{
				m_pass_team_index.erase(it++);
				break;
			}
		}
	}
}

void SpecialLogicMazeRacer::OnRoleLeaveTeam(Role * role)
{
	if (NULL == role) return;
	
	this->DelayKickOutRole(role);
}

void SpecialLogicMazeRacer::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (BATTLE_MODE_MAZE_RACER != ack->battle_mode) return;

	Team * team = NULL;
	for (int i = 0; i < ack->attacker_list.role_count; ++i)
	{
		if (RobotManager::IsRobot(ack->attacker_list.role_list[i].uid)) continue;

		team = TeamManager::Instance().GetTeamByUid(ack->attacker_list.role_list[i].uid);
		if (NULL != team) break;
	}
	if (NULL == team) return;
	
	int team_index = team->GetTeamIndex();
	if (RESULT_TYPE_ATTACKER_WIN == ack->battle_result)
	{
		int amulet_id = LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetARandAmulet();
		if (amulet_id >= 0 && amulet_id < ACTIVITY_MAZE_RACER_AMULET_MAX_TYPE)
		{
			// 迷宫竞速符文掉落 广播
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_maze_racer_amulet_drop, amulet_id);
			if (length > 0)
			{
				static Protocol::SCSystemMsg sm;
				int sendlen = 0;

				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
				{
					// 只广播给队长
					Role * leader = team->GetLeaderRole();
					if (NULL != leader) EngineAdapter::Instance().NetSend(leader->GetNetId(), (const char*)&sm, sendlen);
				}
			}

			MazeRacerManager::Instance().AddAmuletToTeam(team_index, amulet_id, 1);
		}
	}

	MazeRacerManager::Instance().OnEncounterMonster(team_index);
}

void SpecialLogicMazeRacer::OnTeamDismiss(Role * leader_role, Team * team)
{
	if (NULL == leader_role || NULL == team || leader_role->GetUID() != team->GetLeaderUID()) return;
	
	int team_index = team->GetTeamIndex();
	for (std::vector<int>::iterator loop = m_team_id_vec.begin(); loop != m_team_id_vec.end(); ++loop)
	{
		if (team_index == *loop)
		{
			m_team_id_vec.erase(loop);
			break;
		}
	}

	MazeRacerManager::Instance().RefreshTeamStatus(team_index);
	m_need_refresh_rank = true;
}

void SpecialLogicMazeRacer::OnTransport(Role * role, SpecialTransport * transport_obj)
{
	if (NULL == role || NULL == transport_obj) return;

	Team * team = role->GetMyTeam();
	if (NULL == team || role->GetUID() != team->GetLeaderUID()) return;
	int mem_count = team->GetMemberCount();

	const SpecialTransportExtraInfo & extra_info = transport_obj->GetExtraInfo();
	if (0 != extra_info.param1)		// 传送到下一层
	{
		SpecialLogicMazeRacer * sp_mr = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicMazeRacer(extra_info.param1, m_copy_id);
		if (NULL == sp_mr) return;

		Posi target_posi = sp_mr->GetScene()->GetARandPosi();
		TeamManager::Instance().TeamGotoScene(team, extra_info.param1, m_copy_id, target_posi);
	}
	else		// 通关，下发协议
	{
		int team_index = team->GetTeamIndex();
		if (m_pass_team_index.find(team_index) == m_pass_team_index.end())
		{
			MazeRacerManager::Instance().OnTeamPass(m_copy_id, m_maze_id, team);
			m_pass_team_index.insert(team_index);
			for (int mem_idx = 0; mem_idx < mem_count; ++mem_idx)		// 事件触控
			{
				Role * mem_role = team->GetMemberRoleByIndex(mem_idx);
				if (NULL != mem_role) EventHandler::Instance().OnPassMazeRacerEctype(mem_role, m_maze_id);
			}
		}
	}
}

void SpecialLogicMazeRacer::OnAOIEnterRole(Role * role, Role * target_role)
{
	this->SendAmuletInfo(role, target_role);
}

bool SpecialLogicMazeRacer::CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader)
{
	return false;
}

bool SpecialLogicMazeRacer::GetBattleAnimationSpeedType(Role* role, ARG_OUT int* speed, ARG_OUT bool* is_force_speed)
{
	Team * team = role->GetMyTeam();
	if (NULL == team || role->GetUID() != team->GetLeaderUID())
	{
		*speed = ANIMATION_SPEED_NORMAL;
		*is_force_speed = true;

		return true;
	}

	const TeamAmuletInfo * suffer_amulet = MazeRacerManager::Instance().GetTeamSufferAmulet(team->GetTeamIndex(), MAZE_AMULET_EFFECT_SET_BATTLE_SPEED);
	if (NULL != suffer_amulet && MAZE_AMULET_EFFECT_SET_BATTLE_SPEED == suffer_amulet->effect_type)
	{
		*speed = suffer_amulet->effect_value;
		*is_force_speed = true;

		return true;
	}

	*speed = ANIMATION_SPEED_NORMAL;
	*is_force_speed = true;

	return true;
}

bool SpecialLogicMazeRacer::CanMoveOn(Role * role)
{
	Team * team = role->GetMyTeam();
	if (NULL == team || role->GetUID() != team->GetLeaderUID()) return true;

	const TeamAmuletInfo * suffer_amulet = MazeRacerManager::Instance().GetTeamSufferAmulet(team->GetTeamIndex(), MAZE_AMULET_EFFECT_SET_MOVES_PERMIT);
	if (NULL != suffer_amulet && MAZE_AMULET_EFFECT_SET_MOVES_PERMIT == suffer_amulet->effect_type && 0 == suffer_amulet->effect_value)
	{
		role->NoticeNum(errornum::EN_SPECIAL_CAN_NOT_MOVE_ON);
		return false;
	}

	return true;
}

bool SpecialLogicMazeRacer::CanEncounterMineMonster(Role * role)
{
	Team * team = role->GetMyTeam();
	if (NULL == team || role->GetUID() != team->GetLeaderUID()) return true;
	if (this->IsPassTeam(team->GetTeamIndex()))
	{
		return false;
	}
	const TeamAmuletInfo * suffer_amulet = MazeRacerManager::Instance().GetTeamSufferAmulet(team->GetTeamIndex(), MAZE_AMULET_EFFECT_SET_ENCNT_PERMIT);
	if (NULL != suffer_amulet && MAZE_AMULET_EFFECT_SET_ENCNT_PERMIT == suffer_amulet->effect_type) return 0 != suffer_amulet->effect_value ? true : false;

	return true;
}

long long SpecialLogicMazeRacer::GetMineMonsterEncounterInterval(Role * role)
{
	Team * team = role->GetMyTeam();
	if (NULL == team || role->GetUID() != team->GetLeaderUID()) return m_scene->RandMineMonsterEncounterTimeMS();

	const TeamAmuletInfo * suffer_amulet = MazeRacerManager::Instance().GetTeamSufferAmulet(team->GetTeamIndex(), MAZE_AMULET_EFFECT_SET_WAR_INTERVAL);
	if (NULL != suffer_amulet && MAZE_AMULET_EFFECT_SET_WAR_INTERVAL == suffer_amulet->effect_type) return suffer_amulet->effect_value * 1000;

	return m_scene->RandMineMonsterEncounterTimeMS();
}

void SpecialLogicMazeRacer::InitMazeRacerScene(int copy_id, int maze_id, int layer,time_t start_time)
{
	// 副本参数
	m_copy_id = copy_id;
	m_maze_id = maze_id;
	m_layer = layer;
	m_start_time = start_time;

	// 下一层的传送阵
	SpecialTransport * transport_obj = new SpecialTransport();
	if (transport_obj == NULL) return;

	transport_obj->GetExtraInfo().param1 = MazeRacerManager::Instance().GetNextTiersScene(m_copy_id, m_layer);	// 目标场景有可能会等于 0，在传送时处理

	Posi posi = m_scene->GetARandPosi();
	transport_obj->SetPos(posi);
	m_scene->AddObj(transport_obj);

	// 本层的传送目标点
	trans_posi_list[0] = posi;		// 第一个点默认为传送阵的坐标；
	for (int point_idx = 1; point_idx < MAZE_RACER_TRANSFER_TARGET_POSI_NUM; ++point_idx)
	{
		trans_posi_list[point_idx] = m_scene->GetARandPosi();
	}

	// 传送NPC
	const MazeRacerOthersCfg & mr_other_cfg = LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetOthersConfig();
	for (int npc_cur = 0; npc_cur < MAZE_RACER_TRANSFER_NPC_NUM; ++npc_cur)
	{
		NPCOther * trans_messenger = new NPCOther();
		trans_messenger->SetNpcId(mr_other_cfg.trans_npc_id);
		trans_messenger->SetPos(m_scene->GetARandPosi());
		m_scene->AddObj(trans_messenger);
	}
}

void SpecialLogicMazeRacer::RequestUseAmulet(Role * role, int amulet_id)
{
	if (NULL == role || amulet_id < 0 || amulet_id >= ACTIVITY_MAZE_RACER_AMULET_MAX_TYPE) return;

	Team * team = TeamManager::Instance().GetTeamByUid(role->GetUID());
	if (NULL == team)
	{
		return;
	}

	int team_index = team->GetTeamIndex();
	int & amulet_num = MazeRacerManager::Instance().GetTeamResiAmulet(team_index, amulet_id);
	if (amulet_num <= 0)
	{
		role->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	const MazeRacerAumletCfg * amulet_cfg = LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetAmuletConfig(amulet_id);
	if (NULL == amulet_cfg) return;

	int target_team_id = 0;
	switch (amulet_cfg->target_type)
	{
	case MAZE_AMULET_TARGET_ENTIRE_TEAM:
		{
			if (MazeRacerManager::Instance().TeamHasSufferAmuletBuff(team_index))
			{
				role->NoticeNum(errornum::EN_SELF_HAS_AMULET);
				return;
			}

			target_team_id = team_index;
		}
		break;
	case MAZE_AMULET_TARGET_ANOTHER_TEAM:
		{
			std::random_shuffle(m_team_id_vec.begin(), m_team_id_vec.end());

			for (int i = 0; i < (int)m_team_id_vec.size(); ++i)
			{
				if (MazeRacerManager::Instance().TeamHasSufferAmuletBuff(m_team_id_vec[i]) || m_team_id_vec[i] == team_index)
				{
					continue;
				}

				target_team_id = m_team_id_vec[i];
				break;
			}

			if (target_team_id == 0)
			{
				role->NoticeNum(errornum::EN_NO_LEGITIMATE_TARGET);
				return;
			}	
		}
		break;
	}
	UNSTD_STATIC_CHECK(2 == MAZE_AMULET_TARGET_TYPE_MAX);
	
	--amulet_num;
	MazeRacerManager::Instance().AddAmuletBuffToTeam(team_index, target_team_id, amulet_id);

	role->NoticeNum(errornum::EN_USE_AMULET_SUCC);
}

void SpecialLogicMazeRacer::RequestTransport(Role * role)
{
	if (NULL == role) return;

	Team * team = TeamManager::Instance().GetTeamByUid(role->GetUID());
	if (NULL == team || role->GetUID() != team->GetLeaderUID())
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return;
	}

	int rand_idx = rand() % MAZE_RACER_TRANSFER_TARGET_POSI_NUM;
	Posi target_point = trans_posi_list[rand_idx];

	TeamManager::Instance().TeamGotoScene(team, m_scene->GetSceneID(), m_scene->GetSceneKey(), target_point);
}

void SpecialLogicMazeRacer::GetTeamIndexList(std::vector<int> & out_list)
{
	out_list.clear();
	if (!m_team_id_vec.empty()) out_list.assign(m_team_id_vec.begin(), m_team_id_vec.end());
}

void SpecialLogicMazeRacer::SendEctypeInfoToAllRole(Protocol::SCActivityMazeRacerCopysInfo * msg, int send_len)
{
	if (NULL == msg || send_len <= 0) return;

	for (int i = 0; i < (int)m_team_id_vec.size(); ++i)
	{
		Team * team = TeamManager::Instance().GetTeamByTeamIndex(m_team_id_vec[i]);
		if (NULL != team)
		{
			team->SendToMember((const char *)msg, send_len);
		}
	}
}


void SpecialLogicMazeRacer::OnMRActivityFinish()
{
	m_is_finish = true;
	this->DelayKickOutAllRole();
}


void SpecialLogicMazeRacer::SendAmuletInfo(Role * role, Role * target_role)
{
	if (role == NULL)
	{
		return;
	}

	if (target_role == NULL)
	{
		target_role = role;
	}

	Team * team = target_role->GetMyTeam();
	if (NULL == team || !team->IsLeader(target_role))
	{
		return;
	}

	TeamParamCommonData * suffer_amulet = MazeRacerManager::Instance().GetTeamParamCommonData(team->GetTeamIndex());
	if (NULL == suffer_amulet) return;		// 如果没有受咒，就不发了

	Protocol::SCActivityMazeRacerCurseInfo amrci;
	amrci.obj_id = target_role->GetId();
	amrci.curse_type[0] = suffer_amulet->GetFirstAmuletId();  //目前修改为只能有一个效果

	role->GetRoleModuleManager()->NetSend((const void *)&amrci, sizeof(amrci));
}

void SpecialLogicMazeRacer::SendSceneInfo(Role * role)
{
	Team * team = role->GetMyTeam();
	if (team == NULL)
	{
		return;
	}

	time_t & time = m_team_enter_time[team->GetTeamIndex()];
	if (time == 0)
	{
		time = EngineAdapter::Instance().Time();
	}
	
	Protocol::SCActivityMazeRacerSceneInfo  protocol;
	protocol.time = (unsigned int)time;
	protocol.x = trans_posi_list[0].x;
	protocol.y = trans_posi_list[0].y;
	protocol.layer = m_layer;
	protocol.start_time = (unsigned int)m_start_time;

	role->GetRoleModuleManager()->NetSend((const void *)&protocol, sizeof(protocol));
}

bool SpecialLogicMazeRacer::IsPassTeam(int uid_team)
{
	for (std::set<int>::iterator it = m_pass_team_index.begin(); it != m_pass_team_index.end();it++)
	{
		if (*it == uid_team)
		{
			return true;
		}
	}
	return false;
}

void SpecialLogicMazeRacer::OnTeamChangeLeader(Role * leader, int last_leader_id, Team * team)
{
	Role * last_leader = this->GetScene()->GetRoleByUID(last_leader_id);
	if (last_leader != NULL)
	{
		Protocol::SCActivityMazeRacerCurseInfo amrci;
		amrci.obj_id = last_leader->GetId();
		amrci.curse_type[0] = -1;
		this->GetScene()->GetZoneMatrix()->NotifyAreaMsg(last_leader->GetObserHandle(), (void *)&amrci, sizeof(amrci));

		last_leader->RefreshEncounterInterval();
	}

	if (leader != NULL)
	{
		leader->RefreshEncounterInterval();
	}

	if (team != NULL)
	{
		MazeRacerManager().Instance().AmuletEffectMessage(team->GetTeamIndex());
	}	
}

void SpecialLogicMazeRacer::GmMazeRacerOrder(Role * role, int op_type, int param1, int param2)
{
	enum MazeRacerOrderType
	{
		MAZE_RACER_SUFFER_AMULETS = 0,		// 给本队加咒
		MAZE_RACER_ADD_AMULET_NUM = 1,		// 增加符咒数	符咒ID	增加数量
		MAZE_RACER_SKIP_TO_LAYERS = 2,		// 跳到指定层
		MAZE_RACER_GET_TRANS_POSI = 3,		// 获取传送阵坐标
		MAZE_RACER_GO_TO_TRANSOBJ = 4,		// 传送到传送阵点坐标
	};

	if (NULL == role) return;

	Team * team = role->GetMyTeam();
	if (NULL == team) return;

	switch (op_type)
	{
		case MAZE_RACER_SUFFER_AMULETS:
			{
				int team_index = team->GetTeamIndex();
				MazeRacerManager::Instance().AddAmuletBuffToTeam(team_index, team_index, param1);
			}
			break;
		case MAZE_RACER_ADD_AMULET_NUM:
			MazeRacerManager::Instance().AddAmuletToTeam(team->GetTeamIndex(), param1, param2);
			break;
		case MAZE_RACER_GO_TO_TRANSOBJ:
			TeamManager::Instance().TeamGotoScene(team, m_scene->GetSceneID(), m_scene->GetSceneKey(), trans_posi_list[0]);
			break;
	}
}