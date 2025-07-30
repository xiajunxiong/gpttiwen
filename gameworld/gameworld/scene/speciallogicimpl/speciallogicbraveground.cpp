#include "servercommon/activitydef.hpp"
#include "config/logicconfigmanager.hpp"
#include "scene/scene.h"
#include "obj/otherobj/specialtransport.h"
#include "obj/character/role.h"
#include "global/team/team.hpp"
#include "world.h"
#include "global/worldstatus/worldstatus.hpp"
#include "obj/character/npc_monster.h"
#include "npc/npcmanager.h"
#include "battle/battle_manager_local.hpp"
#include "global/activity/activitymanager.hpp"
#include "scene/fbmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "obj/otherobj/npcother.h"
#include "servercommon/errornum.h"
#include "other/partner/partner.hpp"
#include "other/pet/pet.hpp"
#include "global/team/teammanager.hpp"
#include "drop/droppool.hpp"
#include "item/expense/norexitem.hpp"
#include "item/knapsack.h"
#include "servercommon/logdef.h"
#include "servercommon/commonsavedef.h"
#include "protocol/msgbraveground.h"
#include "global/activity/activityimpl/activitybraveground.hpp"
#include "config/activityconfig/braveground/activitybravegroundconfig.hpp"
#include "speciallogicbraveground.hpp"
#include "other/event/eventhandler.hpp"

SpecialLogicBraveGround::SpecialLogicBraveGround(Scene * scene) : SpecialLogic(scene)
{
	m_is_first_enter_flag = false;
	m_is_update_record = false;
	m_team_index = -1;
	m_record_key = 0;
	m_can_select_level_max = 0;
	m_role_level = 1;
	m_is_init = false;
	m_curr_level = 1;
	m_boss_obj_id = INVALID_OBJ_ID;
	m_transport_pos = Posi(0, 0);
	m_buff_id = 0;
}

SpecialLogicBraveGround::~SpecialLogicBraveGround()
{
}

void SpecialLogicBraveGround::Update(unsigned long interval, time_t now_second)
{
	if (m_team_index != -1 && m_is_update_record)
	{
		m_is_update_record = false;

		int finis_level = m_boss_obj_id == INVALID_OBJ_ID ? m_curr_level : m_curr_level - 1;
		this->UpdateTeamRecord(finis_level);
	}
}

void SpecialLogicBraveGround::OnRoleEnterScene(Role * role)
{
	role->SetEncounterMonsterFlag(true);

	if (m_is_init == false)
	{
		m_is_init = true;

		this->CreateMonster();
		this->CreateTransport();
		this->CreateNpc();
	}

	Team * team = role->GetMyTeam();
	if (team == NULL)
	{
		DelayKickOutRole(role);
		return;
	}

	if (team->GetTeamIndex() != m_team_index)
	{
		DelayKickOutRole(role);
		return;
	}


	if (m_is_first_enter_flag)
	{
		role->GetRoleModuleManager()->GetCommonData().enter_brave_ground_times++;
		EventHandler::Instance().OnParticipateLimitActivity(role, ACTIVITY_TYPE_BRAVE_GROUND, __FUNCTION__, role->GetRoleModuleManager()->GetCommonData().enter_brave_ground_times);
	}

	m_is_update_record = true;

	this->SendLayerInfo(role);
}

void SpecialLogicBraveGround::OnRoleLeaveScene(Role * role, bool is_logout)
{
	role->RestoreLastEncounterMonsterFlag();
}

void SpecialLogicBraveGround::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (BATTLE_MODE_BRAVE_GROUND == ack->battle_mode)
	{
		m_buff_id = 0;

		if (RESULT_TYPE_ATTACKER_WIN == ack->battle_result)
		{
			int npc_seq_id = ack->business_data.monster_gen_id;
			NPCMonster* npc_monster = NpcManager::Instance().GetNPCMonster(npc_seq_id);
			if (NULL != npc_monster && npc_monster->GetScene()->GetSceneID() == m_scene->GetSceneID() && npc_monster->GetScene()->GetSceneKey() == m_scene->GetSceneKey())
			{
				if (m_monster_objid.find(npc_monster->GetId()) != m_monster_objid.end())
				{
					m_monster_objid.erase(npc_monster->GetId());
					m_buff_id = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetRandBuffId();

					this->DropRewardKillMonster(ack);
				}

				if (m_boss_obj_id == npc_monster->GetId())
				{
					m_boss_obj_id = INVALID_OBJ_ID;
					this->CreateTransport(true);

					this->UpdateTeamRecord(m_curr_level);
				}

				m_scene->DeleteObj(npc_monster->GetId());
			}

			this->SendLayerInfo();
		}
	}
}

void SpecialLogicBraveGround::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	if (BATTLE_MODE_BRAVE_GROUND == ack->battle_mode && 
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		BattleFBData* data = (BattleFBData*)ack->special_business_data.data;
		for (int i = 0; i < data->role_count; ++i)
		{
			TeamManager::Instance().ExitTeam2(data->role_list[i].role_id);
		}
	}
}

void SpecialLogicBraveGround::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack)
{
}

void SpecialLogicBraveGround::OnRoleLeaveTeam(Role * role)
{
	this->SendNotify(role);
	this->DelayKickOutRole(role);
}

bool SpecialLogicBraveGround::CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader)
{
	return true;
}

bool SpecialLogicBraveGround::CanInvitationRole(Role * role, int uid)
{
	return false;
}


void SpecialLogicBraveGround::OnTransport(Role * role, SpecialTransport * transport_obj)
{
	if (NULL == role || NULL == transport_obj)
	{
		return;
	}

	Team * team = TeamManager::Instance().GetTeamByTeamIndex(m_team_index);
	if (NULL == team)
	{
		return;
	}

	if (!team->IsLeader(role))
	{
		return;
	}

	if (m_boss_obj_id != INVALID_OBJ_ID)		//存在boss 要击杀才能进入下一层
	{
		return;
	}

	this->TransportHelp(m_curr_level + 1);
}

void SpecialLogicBraveGround::OnActivityBraveGroundClose()
{
	for (int index = 0; index < (int)m_scene->RoleNum(); ++index)
	{
		Role *temp_role = m_scene->GetRoleByIndex(index);
		if (NULL != temp_role)
		{
			BattleManagerLocal::GetInstance().ForceFinishFightReq2(temp_role);
			this->SendNotify(temp_role);
		}
	}

	this->DelayKickOutAllRole();
}

void SpecialLogicBraveGround::SendLayerInfo(Role * role)
{
	Protocol::SCBraveGroundSceneInfo scene_info;
	scene_info.curr_level = m_curr_level;
	scene_info.buff_id = m_buff_id;
	scene_info.is_kill_boss = m_boss_obj_id == INVALID_OBJ_ID;

	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&scene_info, sizeof(scene_info));
	}
	else
	{
		m_scene->SendToRole((const char*)&scene_info, sizeof(scene_info));
	}
}

void SpecialLogicBraveGround::SendLevelRewardInfo(int start_level, int end_level, Role * role)
{

}

void SpecialLogicBraveGround::SendNotify(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	Protocol::SCBraveGroundNotify protocol;
	protocol.p1 = m_curr_level - 1;

	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&protocol, sizeof(protocol));
}

void SpecialLogicBraveGround::UseNpcFunction(Role * role, int obj_id)
{
	if (role == NULL)
	{
		return;
	}

	Obj * obj=m_scene->GetObj(obj_id);
	if (obj == NULL || obj->GetObjType() != Obj::OBJ_TYPE_NPC_FUNC)
	{
		return;
	}

	NPCOther * npc = (NPCOther*)obj;

	const BraveGroundNpcCfg * npc_cfg = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetNpcCfg(npc->GetNpcId());
	if (npc_cfg == NULL)
	{
		return;
	}

	Team * team = role->GetMyTeam();
	if (NULL == team)
	{
		return;
	}

	switch (npc_cfg->npc_fun_type)
	{
		case  BraveGroundNpcCfg::BRAVE_GROUND_NPC_TYPE_TRANSPORT:
			{
				int max_level = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetLastLevel();
				int next_level = m_curr_level + npc_cfg->param1 >= max_level ? max_level : m_curr_level + npc_cfg->param1;
				const BraveGroundLevelCfg * cfg = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetLevelCfg(next_level, true);
				if (cfg == NULL)
				{
					return;
				}

				int scene_id = cfg->scene_id;
				int scene_key = m_record_key;
				Posi target_pos = cfg->birth_pos;

				if (npc_cfg->param3 <= 0 || !role->GetRoleModuleManager()->GetKnapsack()->HasItem(npc_cfg->param2, npc_cfg->param3))
				{
					role->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
					return;
				}

				this->SetTargetLevelInfo(scene_id, scene_key, next_level, cfg->transport_pos);

				if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(npc_cfg->param2, npc_cfg->param3, __FUNCTION__))
				{
					return;
				}

				TeamManager::Instance().TeamGotoScene(team, scene_id, scene_key, target_pos);
			}
			break;
		case BraveGroundNpcCfg::BRAVE_GROUND_NPC_TYPE_RECOVER:
			{
				if (npc->GetNpcExtraInfo().param1 != 0)
				{
					Role * leader = team->GetLeaderRole(); 
					if (leader != NULL)
					{
						leader->NoticeNum(errornum::EN_BRAVE_GROUND_OVER_USE_NPC_RECOVER_TIMES);
					}
					return;
				}

				double add_percent = npc_cfg->param1 / 100.0;
				for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
				{
					Role * member_role = team->GetMemberRoleByIndex(i);
					if (member_role == NULL || !member_role->IsFollow())
					{
						continue;
					}

					member_role->GetRoleModuleManager()->GetPartner()->AddFightPartnerHpMp(add_percent);
					member_role->GetRoleModuleManager()->GetPet()->AllPetRecoverHpMpPercent(add_percent);

					int add_hp = (int)(add_percent * member_role->GetRoleModuleManager()->GetAttrByType(BATTLE_ATTR_MAX_HP));
					int add_mp = (int)(add_percent * member_role->GetRoleModuleManager()->GetAttrByType(BATTLE_ATTR_MAX_MP));
					member_role->GetRoleModuleManager()->ChangeHp(add_hp);
					member_role->GetRoleModuleManager()->ChangeMp(add_mp);
					member_role->NoticeNum(errornum::EN_BRAVE_GROUND_USE_NPC_RECOVER);
				}
			}
			break;
		default:
			break;
	}

	npc->GetNpcExtraInfo().param1 += 1;
	npc->NotifyInfo();
}

void SpecialLogicBraveGround::SelectLevel(Role * role, int level)
{
	if (level == 0)
	{
		level = m_can_select_level_max;
	}

	if (level < m_curr_level || level > m_can_select_level_max)
	{
		return;
	}

	int max_level = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetLastLevel();
	if (level > max_level)
	{
		level = max_level;
	}

	Team * team = TeamManager::Instance().GetTeamByTeamIndex(m_team_index);
	if (NULL == team)
	{
		return;
	}

	if (!team->IsLeader(role))
	{
		return;
	}

	this->TransportHelp(level);
}

void SpecialLogicBraveGround::SendRoute(Role * role, int p1, int p2)
{
	if (role == NULL)
	{
		return;
	}

	Team * team = role->GetMyTeam();
	if (team == NULL)
	{
		return;
	}

	Protocol::SCBraveGroundRoute protocol;
	protocol.param1 = p1;
	protocol.param2 = p2;

	team->SendToMember(&protocol, sizeof(protocol));
}

void SpecialLogicBraveGround::SetInfo(int team_index, int record_key, Posi transport_pos, int can_select_level, int role_level, bool is_first_enter_flag)
{
	m_team_index = team_index;
	m_record_key = record_key;
	m_transport_pos = transport_pos;
	m_can_select_level_max = can_select_level + 1;
	m_is_first_enter_flag = is_first_enter_flag;
	m_role_level = role_level;
}

int SpecialLogicBraveGround::GetBuffId()
{
	return m_buff_id;
}

void SpecialLogicBraveGround::GmTransport(Role * role, int add_level)
{
	if (add_level <= 0)
	{
		return;
	}

	Team * team = role->GetMyTeam();
	if (NULL == team)
	{
		return;
	}

	int scene_id = 0;
	int scene_key = 0;
	Posi target_pos(0, 0);

	int next_level = m_curr_level + add_level;

	int max_level = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetLastLevel();
	if (next_level > max_level)
	{
		next_level = max_level;
	}

	if (next_level <= max_level)
	{
		const BraveGroundLevelCfg * cfg = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetLevelCfg(next_level, true);
		if (cfg == NULL)
		{
			return;
		}

		scene_id = cfg->scene_id;
		scene_key = m_record_key;
		target_pos = cfg->birth_pos;

		this->SetTargetLevelInfo(scene_id, scene_key, next_level, cfg->transport_pos);
	}
	else
	{
		scene_id = role->GetLastScene(&target_pos);
		scene_key = COMMON_SCENE_KEY;
	}

	TeamManager::Instance().TeamGotoScene(team, scene_id, scene_key, target_pos);
}

void SpecialLogicBraveGround::CreateMonster()
{
	const BraveGroundLevelCfg * cfg = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetLevelCfg(m_curr_level);
	if (cfg == NULL)
	{
		return;
	}

	if (cfg->boss_monster_npc_id != 0)
	{
		Posi boss_pos = cfg->boss_pos;
		NPCMonster * boss_monster = NpcManager::Instance().CreateNPCMonster(cfg->boss_monster_npc_id, m_scene, boss_pos, BATTLE_MODE_BRAVE_GROUND, &cfg->boss_dir);
		if (boss_monster != NULL)
		{
			m_boss_obj_id = boss_monster->GetId();
			boss_monster->SetBattleMode(BATTLE_MODE_BRAVE_GROUND);
			boss_monster->SetLevel(m_role_level);
		}
	}

	int create_num = cfg->GetCreateMonsterRandNum();
	for (int i = 0; i < create_num; ++i)
	{
		int monster_id = cfg->monster_npc_id;

		Posi boss_pos = this->GetRandPosiExcludeTransportPos();
		NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(monster_id, m_scene, boss_pos, BATTLE_MODE_BRAVE_GROUND);
		if (monster != NULL)
		{
			m_monster_objid.insert(monster->GetId());
			monster->SetBattleMode(BATTLE_MODE_BRAVE_GROUND);
			monster->SetLevel(m_role_level);
		}
	}

}

void SpecialLogicBraveGround::CreateTransport(bool is_on_kill_boss)
{
	if (!is_on_kill_boss)
	{
		const BraveGroundLevelCfg * cfg = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetLevelCfg(m_curr_level);
		if (cfg == NULL)
		{
			return;
		}

		//有boss的层数就不创建传送门，要打死boss才创建
		if (cfg->boss_monster_npc_id != 0)
		{
			return;
		}
	}

	SpecialTransport * transport_obj = new SpecialTransport();
	if (transport_obj == NULL)
	{
		return;
	}

	Posi pos = m_transport_pos;
	transport_obj->SetPos(pos);
	transport_obj->GetExtraInfo().send_param1 = m_curr_level;
	m_scene->AddObj(transport_obj);
}

void SpecialLogicBraveGround::CreateNpc()
{
	const BraveGroundLevelCfg * cfg = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetLevelCfg(m_curr_level);
	if (cfg == NULL)
	{
		return;
	}

	const BraveGroundNpcCfg * create_npc_cfg = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetRandNpcCfg(cfg->npc_group_id);
	if (create_npc_cfg==NULL)
	{
		return;
	}

	NPCOther * npc = new NPCOther();
	npc->SetNpcId(create_npc_cfg->npc_id);
	npc->SetPos(this->GetRandPosiExcludeTransportPos());
	npc->GetNpcExtraInfo().info_type = INFO_TYPE_BRAVE_GROUND;

	m_scene->AddObj(npc);	
}

void SpecialLogicBraveGround::SetTargetLevelInfo(int scene_id, int scene_key, int level, Posi pos)
{
	SpecialLogicBraveGround * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicBraveGround(scene_id, scene_key);
	if (special_lobgic == NULL)
	{
		return;
	}
	special_lobgic->m_is_first_enter_flag = false;
	special_lobgic->m_curr_level = level;
	special_lobgic->m_transport_pos = pos;
	special_lobgic->m_buff_id = m_buff_id;
	special_lobgic->m_is_update_record = true;
	special_lobgic->m_team_index = m_team_index;
	special_lobgic->m_record_key = m_record_key;
	special_lobgic->m_can_select_level_max = m_can_select_level_max;
	special_lobgic->m_role_level = m_role_level;
}

void SpecialLogicBraveGround::TransportHelp(int level)
{
	Team * team = TeamManager::Instance().GetTeamByTeamIndex(m_team_index);
	if (NULL == team)
	{
		return;
	}

	int scene_id = 0;
	int scene_key = 0;
	Posi target_pos(0, 0);

	int next_level = level;
	int max_level = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetLastLevel();
	int cfg_max_level = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetCfgMaxLevel();

	if (next_level <= max_level)
	{
		const BraveGroundLevelCfg * cfg = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetLevelCfg(next_level, true);
		if (cfg == NULL)
		{
			return;
		}

		scene_id = cfg->scene_id;
		scene_key = m_record_key;
		target_pos = cfg->birth_pos;

		this->SetTargetLevelInfo(scene_id, scene_key, next_level, cfg->transport_pos);
		TeamManager::Instance().TeamGotoScene(team, scene_id, scene_key, target_pos);
	}
	else
	{
		if (max_level < cfg_max_level)
		{
			//如果因为世界等级不够，不能进入下层，就不自动退出
			const BraveGroundLevelCfg * cfg = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetLevelCfg(next_level, true);
			if (cfg != NULL)
			{
				team->SendLeaderIncompatibleNotice(0, TEAM_MEMBER_INCOMPATIBLE_BRAVE_GROUND_CANT_NEXT, cfg->need_world_level);
			}

			return;
		}

		this->UpdateTeamRecord(max_level);
		this->DelayKickOutAllRole();
	}
}

void SpecialLogicBraveGround::DropRewardKillMonster(battlegameprotocol::BattleGameFightResultAck * ack)
{
	const BraveGroundLevelCfg * cfg = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetLevelCfg(m_curr_level);
	if (cfg != NULL)
	{
		for (int i = 0; i < ack->attacker_list.role_count; ++i)
		{
			Role * temp_role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
			DropPool::Instance()->DropListToRole(temp_role, cfg->drop_id_list, ack->monster_group_id, false, GET_REASON_INVALID);
		}
	}
}

Posi SpecialLogicBraveGround::GetRandPosiExcludeTransportPos()
{
	Posi ret(0, 0);
	for (int i = 0; i < 50; ++i)
	{
		ret = m_scene->GetARandPosi();
		if ((m_transport_pos - ret).IsLongerThan(25))
		{
			break;
		}
	}

	return ret;
}

void SpecialLogicBraveGround::UpdateTeamRecord(int reached_level)
{
	Team * team = TeamManager::Instance().GetTeamByTeamIndex(m_team_index);
	if (team == NULL)
	{
		return;
	}

	ActivityBraveGround * act = static_cast<ActivityBraveGround *>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_BRAVE_GROUND));
	if (NULL == act)
	{
		return;
	}

	CommonSaveBraveGroundTeamReceordData record_data;
	record_data.Reset();
	record_data.reached_level = reached_level;
	record_data.record_key = m_record_key;
	record_data.time = (unsigned int)EngineAdapter::Instance().Time();

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM && record_data.role_info_count < ARRAY_LENGTH(record_data.role_info); ++i)
	{
		TeamMember * member_info = team->GetMemberInfo(i);
		if (member_info == NULL)
		{
			continue;
		}

		int role_id = member_info->uid;
		Role * member_role = m_scene->GetRoleByUID(role_id);
		if (member_role == NULL)
		{
			continue;
		}

		CommonSaveBraveGroundRankRoleInfoItem & role_info_item = record_data.role_info[record_data.role_info_count];
		role_info_item.role_id = role_id;
		member_role->GetName(role_info_item.role_name);
		record_data.role_info_count += 1;

		member_role->GetRoleModuleManager()->SetBraveGroundMaxReachedLevel(reached_level);
	}

	act->UpdateTeamRecord(m_record_key, record_data);
}

