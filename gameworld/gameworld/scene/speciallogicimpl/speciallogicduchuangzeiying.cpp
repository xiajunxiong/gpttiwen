#include "speciallogicduchuangzeiying.hpp"

#include "scene/scene.h"
#include "world.h"
#include "obj/character/npc.h"
#include "obj/character/role.h"
#include "obj/character/npc_monster.h"
#include "obj/otherobj/specialtransport.h"

#include "servercommon/errornum.h"

#include "npc/npcmanager.h"
#include "other/rolemodulemanager.hpp"
#include "other/anecdotetask/anecdotetask.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/duchuangzeiyingconfig.hpp"

#include "global/team/team.hpp"

SpecialLogicDuChuangZeiYing::SpecialLogicDuChuangZeiYing(Scene * scene)
	:SpecialLogic(scene), m_is_init(false), m_boss_is_create(false), m_layer(0)
{
	m_transport_pos = Posi(0, 0);
	m_obj_info_list.clear();
}

void SpecialLogicDuChuangZeiYing::Update(unsigned long interval, time_t now_second)
{
}

void SpecialLogicDuChuangZeiYing::OnRoleEnterScene(Role * role)
{
	if(NULL == role) return;
	
	if (!m_is_init)
	{
		m_is_init = true;

		this->CreateNpcOrMonster();
		this->CreateTransport();
	}
	if (!this->IsLastLayer())
	{
		this->SendSceneInfo(role);
	}
}

void SpecialLogicDuChuangZeiYing::OnRoleLeaveScene(Role * role, bool is_logout)
{
}

void SpecialLogicDuChuangZeiYing::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (BATTLE_MODE_DU_CHUANG_ZEI_YING == ack->battle_mode)
	{
		if (RESULT_TYPE_ATTACKER_WIN == ack->battle_result)
		{	
			if (!this->IsLastLayer())
			{
				int npc_id = ack->business_data.monster_gen_id;
				for (std::vector<SpecialLogicDuChuangZeiYingObjInfo>::iterator it = m_obj_info_list.begin(); it != m_obj_info_list.end(); it++)
				{
					if (it->npc_id == npc_id)
					{
						it->status = 0;
						break;
					}
				}
				this->SendSceneInfo();
			}
			else
			{
				int npc_seq_id = ack->business_data.monster_gen_id;
				NPC* npc = NpcManager::Instance().GetServerNPC(npc_seq_id);
				if (NULL != npc)
				{
					if (Obj::OBJ_TYPE_NPC_CHOSED == npc->GetObjType())
					{
						NPCChosed * npc_chosed = (NPCChosed*)npc;
						if (npc->GetScene()->GetSceneID() != m_scene->GetSceneID() ||
							npc->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
						{
							return;
						}
					}
				}
				for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
				{
					Role * mem_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(ack->attacker_list.role_list[i].uid));
					if (NULL == mem_role) continue;

					mem_role->GetRoleModuleManager()->GetAnecdoteTask()->OnFinishTask(ANECDOTE_TASK_TYPE_THIEF_CAMP_RESCUE);
				}
				this->DelayKickOutAllRole();
			}
		}
		else
		{
			this->DelayKickOutAllRole();
		}
	}
}

void SpecialLogicDuChuangZeiYing::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	this->DelayKickOutAllRole();
}

void SpecialLogicDuChuangZeiYing::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{
	this->DelayKickOutAllRole();
}

void SpecialLogicDuChuangZeiYing::OnTransport(Role * role, SpecialTransport * transport_obj)
{
	if (NULL == role || NULL == transport_obj) return;

	if (this->IsLastLayer())
	{
		this->DelayKickOutAllRole();
	}
	else
	{
		this->TransportNextLayer(role, transport_obj);
	}
}

void SpecialLogicDuChuangZeiYing::OnNPCChosedReward(Role * role, ObjID npc_objid, int option_index)
{
	Obj * npc_obj = m_scene->GetObj(npc_objid);
	if (npc_obj != NULL && Obj::OBJ_TYPE_NPC_CHOSED == npc_obj->GetObjType())
	{	
		if (!m_boss_is_create)
		{
			NPCChosed * npc_chosed = (NPCChosed*)npc_obj;
			if (npc_chosed->GetScene()->GetSceneID() == m_scene->GetSceneID() &&
				npc_chosed->GetScene()->GetSceneKey() == m_scene->GetSceneKey())
			{
				npc_chosed->SetSendOption(false);
			}
			this->CreateBossNpc(role);
		}
	}
}

void SpecialLogicDuChuangZeiYing::SetInfo(int layer, const Posi & transport_pos)
{
	m_layer = layer;
	m_transport_pos = transport_pos;
}

void SpecialLogicDuChuangZeiYing::SendSceneInfo(Role * role)
{
	SCDuChuangZeiYingSceneInfo info;
	info.count = 0;

	for (int i = 0; i < (int)m_obj_info_list.size() && i < SCDuChuangZeiYingSceneInfo::MAX_SCENE_OBJ_NUM; i++)
	{
		info.obj_info[info.count++] = m_obj_info_list[i];
	}

	int length = sizeof(info) - sizeof(SpecialLogicDuChuangZeiYingObjInfo) * (SCDuChuangZeiYingSceneInfo::MAX_SCENE_OBJ_NUM - info.count);
	if (NULL == role)
	{
		m_scene->SendToRole((const char *)&info, length);
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &info, length);
	}
}

void SpecialLogicDuChuangZeiYing::SendRoleNotice(Role * role, int npc_id)
{
	SCDuChuangZeiYingNotice info;
	info.npc_id = npc_id;

	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
}

void SpecialLogicDuChuangZeiYing::CreateNpcOrMonster()
{
	if (this->IsLastLayer())
	{
		const DuChuangZeiYingSceneCfg * scene_cfg =LOGIC_CONFIG->GetDuChuangZeiYingConfig()->GetSceneCfg(m_layer - 1);
		if(NULL == scene_cfg) return;

		NPCChosed * monster = NpcManager::Instance().CreateNPCChosed(scene_cfg->npc_id, m_scene, scene_cfg->npc_pos, BATTLE_MODE_DU_CHUANG_ZEI_YING, &scene_cfg->npc_dir);
		if (monster == NULL) return;
	}
	else
	{
		LOGIC_CONFIG->GetDuChuangZeiYingConfig()->GetPatrolCfg(&m_obj_info_list);
	}
}

void SpecialLogicDuChuangZeiYing::CreateTransport()
{
	if (this->IsLastLayer())
	{
		return;
	}

	SpecialTransport * transport_obj = new SpecialTransport();
	if (NULL != transport_obj)
	{
		transport_obj->SetPos(gamecommon::GetDisRandPos(m_transport_pos, 5));
		transport_obj->GetExtraInfo().send_param1 = m_layer;
		m_scene->AddObj(transport_obj);
	}
}

void SpecialLogicDuChuangZeiYing::CreateBossNpc(Role * role)
{	
	if (!this->IsLastLayer())
	{
		return;
	}
	const DuChuangZeiYingSceneCfg * scene_cfg = LOGIC_CONFIG->GetDuChuangZeiYingConfig()->GetSceneCfg(m_layer - 1);
	if (NULL == scene_cfg) return;

	NPCChosed * monster = NpcManager::Instance().CreateNPCChosed(scene_cfg->boss_npc_id, m_scene, scene_cfg->boss_npc_pos, BATTLE_MODE_DU_CHUANG_ZEI_YING, &scene_cfg->boss_npc_dir);
	if (monster == NULL) return;

	m_boss_is_create = true;
	if (NULL != role)
	{
		this->SendRoleNotice(role, scene_cfg->boss_npc_id);
	}
}

void SpecialLogicDuChuangZeiYing::TransportNextLayer(Role * role, SpecialTransport * transport_obj)
{
	if (NULL == role || NULL == transport_obj)
	{
		return;
	}
	const DuChuangZeiYingSceneCfg * scene_cfg = LOGIC_CONFIG->GetDuChuangZeiYingConfig()->GetSceneCfg(m_layer - 1);
	if(NULL == scene_cfg) return;

	const DuChuangZeiYingSceneCfg * next_scnen_cfg = LOGIC_CONFIG->GetDuChuangZeiYingConfig()->GetSceneCfg(m_layer);
	if (NULL == next_scnen_cfg || scene_cfg->transport_id != next_scnen_cfg->scene_id) return;

	const DuChuangZeiYingOtherCfg & other_cfg = LOGIC_CONFIG->GetDuChuangZeiYingConfig()->GetOtherCfg();

	int scene_key = m_scene->GetSceneKey();
	SpecialLogicDuChuangZeiYing * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicDuChuangZeiYing(next_scnen_cfg->scene_id, scene_key);
	if (special_lobgic == NULL)
	{
		return;
	}
	special_lobgic->SetInfo(m_layer + 1, next_scnen_cfg->transport_pos);
	Team * team = role->GetMyTeam();
	if (NULL != team)
	{
		if(!team->IsLeader(role) || team->GetMemberCount() > other_cfg.max_team_num)
		{
			return;
		}
	}
	World::GetInstWorld()->GetSceneManager()->GoTo(role, next_scnen_cfg->scene_id, scene_key, next_scnen_cfg->born_pos);
}

bool SpecialLogicDuChuangZeiYing::IsLastLayer()
{
	return m_layer == MAX_SCENE_LAYER_NUM;
}
