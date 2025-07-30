#include "world.h"
#include "scene/scene.h"
#include "scene/fbmanager.hpp"
#include "obj/otherobj/specialtransport.h"
#include "task/taskmanager.h"

#include "global/team/team.hpp"

#include "npc/npcmanager.h"
#include "obj/character/npc_monster.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "config/logicconfigmanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/pet/pet.hpp"
#include "other/partner/partner.hpp"
#include "servercommon/string/gameworldstr.h"

#include "protocol/msgmedal.h"
#include "other/medal/medalfbconfig.hpp"
#include "speciallogicmedal.hpp"

#include "other/event/eventhandler.hpp"

SpecialLogicMedal::SpecialLogicMedal(Scene * scene) : SpecialLogic(scene)
{
	m_fb_seq = 0;
	m_curr_layer = 1;
	m_is_init = false;
	m_is_finish = false;
	m_recory_npc_objid = INVALID_OBJ_ID;
}

SpecialLogicMedal::~SpecialLogicMedal()
{

}

void SpecialLogicMedal::OnRoleEnterScene(Role * role)
{
	if (!m_is_init)
	{
		m_is_init = true;
		this->CreateMonster();
		this->CreateNpc();
	}	 

	role->SetEncounterMonsterFlag(true);
	this->SendSceneInfo(role);
}

void SpecialLogicMedal::OnRoleLeaveScene(Role * role, bool is_logout)
{
	role->RestoreLastEncounterMonsterFlag();
}

void SpecialLogicMedal::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
	    BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}

	if (BATTLE_MODE_MEDAL_FB == ack->battle_mode && RESULT_TYPE_ATTACKER_WIN == ack->battle_result)
	{
		int npc_seq_id = ack->business_data.monster_gen_id;
		NPC* npc = NpcManager::Instance().GetServerNPC(npc_seq_id);
		if (NULL != npc && Obj::OBJ_TYPE_NPC_MONSTER == npc->GetObjType())
		{
			NPCMonster * npc_monster = (NPCMonster*)npc;
			if (npc->GetScene()->GetSceneID() != m_scene->GetSceneID() ||
			    npc->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
			{
				return;
			}

			if (m_monster_obj.find(npc_monster->GetId()) != m_monster_obj.end())
			{
				this->ClearMonster();
				this->CreateTransport();

				if (this->IsLastLayer())
				{
					for (int i = 0; i < ack->attacker_list.role_count; ++i)
					{
						Role * temp_role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
						if (NULL != temp_role)
						{
							temp_role->GetRoleModuleManager()->GetTaskManager()->ExecuteATask(TASK_NEED_PASS_DESIGNATED_MEDAL_FB, m_fb_seq, 0);
							EventHandler::Instance().OnPassMedalEctype(temp_role);
						}
					}
				}

				this->SendKillBoss();
			}
		}
	}
}

void SpecialLogicMedal::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	if (BATTLE_MODE_MEDAL_FB == ack->battle_mode &&ack->special_business_data.data_length >= (int)sizeof(BattleFBData) && BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}
}

void SpecialLogicMedal::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack)
{
	if (BATTLE_MODE_MEDAL_FB == ack->battle_mode &&ack->special_business_data.data_length >= (int)sizeof(BattleFBData) && BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}
}

void SpecialLogicMedal::OnRoleLeaveTeam(Role * role)
{
	const bool is_single = LOGIC_CONFIG->GetMedalFbConfig()->IsSingleFb(m_fb_seq);
	if (is_single)
	{
		return;
	}

	this->DelayKickOutRole(role);
}

void SpecialLogicMedal::Update(unsigned long interval, time_t now_second)
{
	SpecialLogic::Update(interval, now_second);
}

bool SpecialLogicMedal::CanInvitationRole(Role * role, int uid)
{
	return false;
}

void SpecialLogicMedal::OnTransport(Role * role, SpecialTransport * transport_obj)
{
	if (NULL == role || NULL == transport_obj)
	{
		return;
	}

	if (!this->IsLastLayer())
	{
		this->TransportNextLayer(role, transport_obj);
	}
	else
	{
		Team * team = role->GetMyTeam();
		if (NULL != team && !team->IsLeader(role))		//如果在队伍里判断是否队长操作
		{
			return;
		}

		FBManager::Instance().LeaveFB(role);
	}
}

void SpecialLogicMedal::SetInfo(int fb_seq, int layer)
{
	this->m_fb_seq = fb_seq;
	this->m_curr_layer = layer;
}

void SpecialLogicMedal::SendSceneInfo(Role * role)
{
	Protocol::SCMedalFbSceneInfo scene_info;
	scene_info.level_seq = m_fb_seq;
	scene_info.layer = m_curr_layer;
	scene_info.is_has_monster = !m_monster_obj.empty();

	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&scene_info, sizeof(scene_info));
	}
	else
	{
		m_scene->SendToRole((const char*)&scene_info, sizeof(scene_info));
	}
}

void SpecialLogicMedal::SendKillBoss()
{
	Protocol::SCMedalFbKillBoss msg;
	msg.scene_id = m_scene->GetSceneID();
	m_scene->SendToRole((const char *)&msg, sizeof(msg));
}


void SpecialLogicMedal::OnNPCChosedReward(Role * role, ObjID npc_objid, int option_index)
{
	if (m_recory_npc_objid == INVALID_OBJ_ID || npc_objid != m_recory_npc_objid)
	{
		return;
	}

	Obj * npc_obj = m_scene->GetObj(npc_objid);
	if (npc_obj == NULL || Obj::OBJ_TYPE_NPC_CHOSED != npc_obj->GetObjType())return;

	NPCChosed * npc_chose = (NPCChosed *)npc_obj;

	const MedalFbLayerCfg * layer_cfg = LOGIC_CONFIG->GetMedalFbConfig()->GetMedalFbLayerCfg(m_fb_seq, m_curr_layer);
	if (layer_cfg == NULL)
	{
		return;
	}

	int cure_present = layer_cfg->recover_num;

	Team * team = role->GetMyTeam();
	if (team != NULL)
	{
		if (!role->IsLeader())
		{
			return;
		}

		for (int i = 0; i < team->GetMemberCount() && i < MAX_TEAM_MEMBER_NUM; i++)
		{
			Role * temp_role = team->GetMemberRoleByIndex(i);
			if (temp_role == NULL || !temp_role->IsFollow())continue;
			this->RecoryHelp(temp_role, cure_present);
		}
	}
	else
	{
		this->RecoryHelp(role, cure_present);
	}
}

void SpecialLogicMedal::RecoryHelp(Role * role, int cure_present)
{
	BattleAttr max_hp = role->GetRoleModuleManager()->GetAttrByType(BATTLE_ATTR_MAX_HP);
	BattleAttr max_mp = role->GetRoleModuleManager()->GetAttrByType(BATTLE_ATTR_MAX_MP);

	BattleAttr cur_hp = role->GetRoleModuleManager()->GetAttrByType(BATTLE_ATTR_CUR_HP);
	BattleAttr cur_mp = role->GetRoleModuleManager()->GetAttrByType(BATTLE_ATTR_CUR_MP);

	cur_hp = cur_hp + (max_hp * cure_present / 100);
	cur_mp = cur_mp + (max_mp * cure_present / 100);

	if (cur_hp > max_hp)cur_hp = max_hp;
	if (cur_mp > max_mp)cur_mp = max_mp;

	role->GetRoleModuleManager()->SetCurHpMp(cur_hp, cur_mp);

	Pet * role_pet = role->GetRoleModuleManager()->GetPet();
	int pet_fight_index = role_pet->GetFightIndex();
	BattleAttr role_attr_hp = role_pet->GetPetAttr(pet_fight_index, BATTLE_ATTR_MAX_HP);
	BattleAttr add_hp = role_attr_hp * cure_present / 100;
	BattleAttr role_attr_mp = role_pet->GetPetAttr(pet_fight_index, BATTLE_ATTR_MAX_MP);
	BattleAttr add_mp = role_attr_mp * cure_present / 100;
	role_pet->ChangeHP(pet_fight_index, add_hp);
	role_pet->ChangeMp(pet_fight_index, add_mp);

	Partner * role_partner = role->GetRoleModuleManager()->GetPartner();
	role_partner->AddFightPartnerHpMp(((double)cure_present / 100.0));

	char info[20];
	memset(info, 0, sizeof(info));
	SNPRINTF(info, sizeof(info), "%d%s", cure_present, "%");
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_buff_effect_hpmp_message, info);
	if (length > 0)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT)))
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&sm, sendlen);
		}
	}
}

void SpecialLogicMedal::CreateMonster()
{
	const MedalFbLayerCfg * layer_cfg = LOGIC_CONFIG->GetMedalFbConfig()->GetMedalFbLayerCfg(m_fb_seq, m_curr_layer);
	if (layer_cfg == NULL)
	{
		return;
	}

	for (int i = 0; i < (int)layer_cfg->monster_item.size(); ++i)
	{
		const MedalFbLayerCfg::MonsterInfoItem & monster_cfg = layer_cfg->monster_item[i];

		NPCMonster * boss_monster = NpcManager::Instance().CreateNPCMonster(monster_cfg.boss_npc_id, m_scene, monster_cfg.boss_pos, BATTLE_MODE_MEDAL_FB, &monster_cfg.boss_dir);
		if (boss_monster != NULL)
		{
			m_monster_obj.insert(boss_monster->GetId());
		}
	}

	if (m_monster_obj.empty())
	{
		this->CreateTransport();  //这层没有boss ，直接创建传送门
	}
}

void SpecialLogicMedal::CreateTransport()
{
	const MedalFbLayerCfg * layer_cfg = LOGIC_CONFIG->GetMedalFbConfig()->GetMedalFbLayerCfg(m_fb_seq, m_curr_layer);
	if (layer_cfg == NULL)
	{
		return;
	}

	SpecialTransport * transport_obj = new SpecialTransport();
	transport_obj->SetPos(layer_cfg->transport_pos);
	transport_obj->GetExtraInfo().send_param1 = m_curr_layer;

	m_scene->AddObj(transport_obj);
}

void SpecialLogicMedal::CreateNpc()
{
	const MedalFbLayerCfg * layer_cfg = LOGIC_CONFIG->GetMedalFbConfig()->GetMedalFbLayerCfg(m_fb_seq, m_curr_layer);
	if (layer_cfg == NULL)
	{
		return;
	}

	NPCChosed * monster = NpcManager::Instance().CreateNPCChosed(layer_cfg->npc_id, m_scene, layer_cfg->npc_pos, BATTLE_MODE_MEDAL_FB, &layer_cfg->npc_dir);
	if (monster == NULL)
	{
		return;
	}

	m_recory_npc_objid = monster->GetId();
}

void SpecialLogicMedal::ClearMonster()
{
	for (std::set<ObjID>::iterator iter = m_monster_obj.begin(); iter != m_monster_obj.end(); ++iter)
	{
		m_scene->DeleteObj(*iter);
	}

	m_monster_obj.clear();
}

bool SpecialLogicMedal::IsLastLayer(int fb_layer)
{
	if (fb_layer == 0)
	{
		fb_layer = m_curr_layer;
	}

	const MedalFbLevelCfg * level_cfg = LOGIC_CONFIG->GetMedalFbConfig()->GetMedalFbLevelCfg(m_fb_seq);
	if (level_cfg == NULL)
	{
		return true;
	}

	if (fb_layer >= (int)level_cfg->layer_cfg.size())
	{
		return true;
	}

	return false;
}

void SpecialLogicMedal::TransportNextLayer(Role * role, SpecialTransport * transport_obj)
{
	if (NULL == role || NULL == transport_obj)
	{
		return;
	}

	int scene_key = m_scene->GetSceneKey();
	bool is_single = LOGIC_CONFIG->GetMedalFbConfig()->IsSingleFb(m_fb_seq);

	int next_layer = m_curr_layer + 1;
	const MedalFbLayerCfg * layer_cfg = LOGIC_CONFIG->GetMedalFbConfig()->GetMedalFbLayerCfg(m_fb_seq, next_layer);
	if (layer_cfg == NULL)
	{
		return;
	}

	int scene_id = layer_cfg->scene_id;
	Posi target_pos = layer_cfg->birth_pos;


	SpecialLogicMedal * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicMedal(scene_id, scene_key);
	if (special_lobgic == NULL)
	{
		return;
	}

	special_lobgic->SetInfo(m_fb_seq, next_layer);
	
	is_single ? World::GetInstWorld()->GetSceneManager()->GoTo(role, scene_id, scene_key, target_pos) :
		World::GetInstWorld()->GetSceneManager()->TeamGoTo(role->GetMyTeam(), scene_id, scene_key, target_pos);
}









































