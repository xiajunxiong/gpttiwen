#include "npc/npcmanager.h"
#include "config/logicconfigmanager.hpp"
#include "scene/scene.h"
#include "obj/character/npc_gather.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"

#include "other/roleactivity/roleactivity.hpp"
#include "other/rolegatherfb/rolegatherfb.hpp"
#include "other/rolegatherfb/gatherfbconfig.hpp"
#include "protocol/msggatherfb.h"
#include "speciallogicgather.hpp"

SpecialLogicGatherFB::SpecialLogicGatherFB(Scene * scene) :SpecialLogic(scene), m_is_init(false), m_begin_timestamp(0)
{
}

SpecialLogicGatherFB::~SpecialLogicGatherFB()
{
}

void SpecialLogicGatherFB::Update(unsigned long interval, time_t now_second)
{
	for (int i = 0; i < (int)m_gather_info.size(); i++)
	{
		GatherInfo & gather_info = m_gather_info[i];
		if (gather_info.gather_obj_id == INVALID_OBJ_ID && now_second >= gather_info.next_create_time)
		{
			NPCGather * gather = NpcManager::Instance().CreateNPCGather(gather_info.gather_id, m_scene, gather_info.pos);
			if (gather == NULL)
			{
				continue;
			}

			gather_info.next_create_time = 0;
			gather_info.gather_obj_id = gather->GetId();

			//printf("Update seq_id[%d] obj_id[%d] pos[%d,%d]\n", gather->GetNpcSeq(), gather->GetId(), gather->GetPos().x, gather->GetPos().y);
		}
	}
}

void SpecialLogicGatherFB::OnRoleEnterScene(Role * role)
{
	if (m_is_init == false)
	{
		m_is_init = true;

		this->CreateGather();
	}
}

void SpecialLogicGatherFB::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (BATTLE_MODE_DUEL == ack->battle_mode)
	{
		this->DuelOnFightFinish(ack);
	}
}

bool SpecialLogicGatherFB::CanGather(Role * role, NPCGather * gather)
{
	const RoleGatherFbLevelCfg * level_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetLevelCfgBySceneId(m_scene->GetSceneID());
	if (NULL == level_cfg)
	{
		return false;
	}
	if (role->GetRoleModuleManager()->GetRoleGatherFb()->GetGatherPower() < level_cfg->expend_active)
	{
		return false;
	}

	return true;
}

void SpecialLogicGatherFB::OnGather(Role * role, int gather_id, int seq_id, const Posi & pos)
{
	//×îÉÙÏûºÄ1
	int consume = 0;
	const RoleGatherFbLevelCfg * level_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetLevelCfgBySceneId(m_scene->GetSceneID());
	if (NULL != level_cfg)
	{
		consume = level_cfg->expend_active;
	}
	if (consume <= 0)
	{
		consume = 1;
	}
	role->GetRoleModuleManager()->GetRoleGatherFb()->AddGatherPower(-consume, true);
}

void SpecialLogicGatherFB::OnRemoveObj(Obj * obj)
{
	const RoleGatherFbOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetOtherCfg();	
	for (int i = 0; i < (int)m_gather_info.size(); ++i)
	{
		if (m_gather_info[i].gather_obj_id == obj->GetId())
		{
			m_gather_info[i].gather_obj_id = INVALID_OBJ_ID;
			m_gather_info[i].next_create_time = EngineAdapter::Instance().Time() + other_cfg.create_interval;
			break;
		}
	}
}

void SpecialLogicGatherFB::OnGetItemByGather(Role * role, ItemID item_id, int num)
{
	if (NULL == role || num <= 0) return;
	
	m_gather_list[role->GetUID()][item_id] += num;
	this->SendGatherListInfo(role);
}

void SpecialLogicGatherFB::CreateGather()
{
	const RoleGatherFbLevelCfg *  level_cfg= LOGIC_CONFIG->GetRoleGatherFbConfig()->GetLevelCfgBySceneId(m_scene->GetSceneID());
	if (level_cfg == NULL)
	{
		return;
	}

	const RoleGatherFbPosCfg * pos_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetPosCfg(level_cfg->pos_group_id);
	if (pos_cfg == NULL)
	{
		return;
	}

	GatherInfo gather_info;
	gather_info.gather_id = level_cfg->gather_id;
	for (int i = 0; i < (int)pos_cfg->create_pos.size(); ++i)
	{		
		gather_info.pos = pos_cfg->create_pos[i];
		NPCGather * gather = NpcManager::Instance().CreateNPCGather(gather_info.gather_id, m_scene, gather_info.pos);
		if (gather == NULL)
		{
			continue;
		}

		gather_info.next_create_time = 0;
		gather_info.gather_obj_id = gather->GetId();

		m_gather_info.push_back(gather_info);

		//printf("Update seq_id[%d] obj_id[%d] pos[%d,%d]\n", gather->GetNpcSeq(), gather->GetId(), gather->GetPos().x, gather->GetPos().y);
	}
}

void SpecialLogicGatherFB::SendGatherListInfo(Role * role)
{
	if (NULL == role) return;

	Protocol::SCGatherFbGatherListInfo gfgli;
	gfgli.list_num = 0;
	std::map<int, std::map<ItemID, int> >::const_iterator user_it = m_gather_list.find(role->GetUID());
	if (user_it != m_gather_list.end())
	{
		for (std::map<ItemID, int>::const_iterator loop = user_it->second.begin(); loop != user_it->second.end(); ++loop)
		{
			if (gfgli.list_num >= GATHER_LIST_ITEM_TYPE_MAX) break;
			gfgli.gather_list[gfgli.list_num++] = ItemPairCon(loop->first, loop->second);
		}
	}

	int send_len = sizeof(gfgli) - (GATHER_LIST_ITEM_TYPE_MAX - gfgli.list_num) * sizeof(gfgli.gather_list[0]);
	if (send_len > 0) EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&gfgli, send_len);
}
