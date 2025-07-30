#include "obj/character/npc_monster.h"
#include "npc/npcmanager.h"
#include "config/logicconfigmanager.hpp"
#include "obj/otherobj/treasurechest.h"
#include "scene/scene.h"
#include "obj/otherobj/specialtransport.h"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "obj/character/npc_monster.h"
#include "task/taskmanager.h"
#include "other/rolemodulemanager.hpp"

#include "other/funcguide/funcguideconfig.h"
#include "other/event/eventhandler.hpp"
#include "protocol/msgguidefb.h"
#include "speciallogicguidefb.hpp"

SpecialLogicGuideFb::SpecialLogicGuideFb(Scene * scene) : SpecialLogic(scene)
{
	m_is_init = false;
	m_layer = 1;
	m_box_num = 0;
	m_box_num = 0;
	m_monster_num = 0;
}

SpecialLogicGuideFb::~SpecialLogicGuideFb()
{
}

void SpecialLogicGuideFb::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (BATTLE_MODE_GUIDE_FB == ack->battle_mode &&
	    ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
	    BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}

	if (BATTLE_MODE_GUIDE_FB == ack->battle_mode && RESULT_TYPE_ATTACKER_WIN == ack->battle_result)
	{
		int npc_seq_id = ack->business_data.monster_gen_id;
		NPCMonster* npc_monster = NpcManager::Instance().GetNPCMonster(npc_seq_id);
		if (NULL == npc_monster) return;

		if (npc_monster->GetScene()->GetSceneID() != m_scene->GetSceneID() ||
		    npc_monster->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
		{
			return;
		}

		if (npc_monster->GetId() == m_boss_obj_id && LOGIC_CONFIG->GetFuncGuideConfig()->IsLastLayer(m_layer))
		{
			this->OnFinish();
		}

		m_monster_num -= 1;
	}

	this->SendSceneInfo();
}

void SpecialLogicGuideFb::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	if (BATTLE_MODE_GUIDE_FB == ack->battle_mode &&
	    ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
	    BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}
}

void SpecialLogicGuideFb::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack)
{
	if (BATTLE_MODE_GUIDE_FB == ack->battle_mode &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}
}

void SpecialLogicGuideFb::OnRoleEnterScene(Role * role)
{
	if (!m_is_init)
	{
		m_is_init = true;
		this->CreateBox();
		this->CreateMonster();
		this->CreateTransport();
	}

	role->SetEncounterMonsterFlag(true);

	this->SendSceneInfo(role);
}

void SpecialLogicGuideFb::OnRoleLeaveScene(Role * role, bool is_logout)
{
	role->RestoreLastEncounterMonsterFlag();
}

void SpecialLogicGuideFb::OnTransport(Role * role, SpecialTransport * transport_obj)
{
	if (NULL == role || NULL == transport_obj)
	{
		return;
	}

	const GuideFBLevelCfg * cfg = LOGIC_CONFIG->GetFuncGuideConfig()->GetGuideFBLevelCfg(transport_obj->GetExtraInfo().param1);
	if (cfg == NULL)
	{
		return;
	}

	int scene_id = cfg->scene_id;
	int scene_key = role->GetUID();
	Posi target_pos = Posi(0, 0);

	SpecialLogicGuideFb * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicGuideFb(scene_id, scene_key);
	if (special_lobgic == NULL)
	{
		return;
	}
	special_lobgic->m_layer = m_layer + 1;
	target_pos = special_lobgic->GetScene()->GetARandPosi();

	World::GetInstWorld()->GetSceneManager()->GoTo(role, scene_id, scene_key, target_pos);
}

bool SpecialLogicGuideFb::OnOpenTreasureChest(Role * role, TreasureChest * transport_obj)
{
	const GuideFBLevelCfg * cfg = LOGIC_CONFIG->GetFuncGuideConfig()->GetGuideFBLevelCfg(m_layer);
	if (cfg == NULL)
	{
		return false;
	}

	if (cfg->box_reward.size() > 0)
	{
		role->GetRoleModuleManager()->GetKnapsack()->PutList((short)cfg->box_reward.size(), &cfg->box_reward[0], PUT_REASON_GUIDE_FB);
	}

	m_box_num -= 1;

	this->SendSceneInfo();

	return true;
}

void SpecialLogicGuideFb::SendSceneInfo(Role * role)
{
	Protocol::SCGuideFbSceneInfo protocol;
	protocol.layer = m_layer;
	protocol.box_num = m_box_num;
	protocol.monster_num = m_monster_num;

	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}
	else
	{
		m_scene->SendToRole((const char *)&protocol, sizeof(protocol));
	}
}

void SpecialLogicGuideFb::OnFinish()
{
	for (int i = 0; i < (int)m_scene->RoleNum(); ++i)
	{
		Role *role = m_scene->GetRoleByIndex(i);
		if (NULL != role)
		{
			const FuncGuideOtherCfg &  other_cfg = LOGIC_CONFIG->GetFuncGuideConfig()->GetOtherCfg();
			role->GetRoleModuleManager()->GetTaskManager()->ExecuteATask(other_cfg.guide_fb_task_id, 1);
			EventHandler::Instance().OnPassGuideEctype(role);
		}
	}

	this->DelayKickOutAllRole();
}

void SpecialLogicGuideFb::CreateMonster()
{
	const GuideFBLevelCfg * cfg = LOGIC_CONFIG->GetFuncGuideConfig()->GetGuideFBLevelCfg(m_layer);
	if (cfg == NULL)
	{
		return;
	}

	if (cfg->monster_npc_id != 0)
	{
		NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(cfg->monster_npc_id, m_scene, cfg->monster_pos);
		if (NULL != monster)
		{
			monster->SetBattleMode(BATTLE_MODE_GUIDE_FB);
		}
		m_monster_num += 1;
	}

	if (cfg->boss_npc_id != 0)
	{
		NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(cfg->boss_npc_id, m_scene, cfg->boss_pos);
		if (NULL != monster)
		{
			monster->SetBattleMode(BATTLE_MODE_GUIDE_FB);
			m_boss_obj_id = monster->GetId();
			m_monster_num += 1;
		}
	}
}

void SpecialLogicGuideFb::CreateBox()
{
	const GuideFBLevelCfg * cfg = LOGIC_CONFIG->GetFuncGuideConfig()->GetGuideFBLevelCfg(m_layer);
	if (cfg == NULL)
	{
		return;
	}

	if (cfg->box_id != 0)
	{
		TreasureChest  * treasure_chest_obj = new TreasureChest(cfg->box_id);
		treasure_chest_obj->SetPos(cfg->box_pos);
		m_scene->AddObj(treasure_chest_obj);
		m_box_num += 1;
	}
}

void SpecialLogicGuideFb::CreateTransport()
{
	const GuideFBLevelCfg * cfg = LOGIC_CONFIG->GetFuncGuideConfig()->GetGuideFBLevelCfg(m_layer + 1);
	if (cfg == NULL)
	{
		return;
	}

	SpecialTransport * transport_obj = new SpecialTransport();
	transport_obj->SetPos(m_scene->GetARandPosi());
	transport_obj->GetExtraInfo().param1 = m_layer + 1;
	transport_obj->GetExtraInfo().send_param1 = m_layer;

	m_scene->AddObj(transport_obj);
}
