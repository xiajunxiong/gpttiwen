#include "speciallogicadvancefb.hpp"
#include "world.h"
#include "obj/character/role.h"
#include "obj/character/npc_monster.h"
#include "obj/otherobj/specialtransport.h"
#include "config/logicconfigmanager.hpp"
#include "other/advancefb/advancefbconfig.hpp"
#include "other/event/eventhandler.hpp"
#include "npc/npcmanager.h"
#include "scene/scene.h"
#include "global/team/team.hpp"
#include "scene/fbmanager.hpp"
#include "obj/otherobj/npcother.h"
#include "other/rolemodulemanager.hpp"
#include "battle/battle_manager_local.hpp"
#include "servercommon/errornum.h"
#include "task/taskmanager.h"
#include "task/taskrecorder.h"
#include "item/itempool.h"
#include "item/knapsack.h"
#include "drop/droppool.hpp"
#include "gameworld/protocol/msgcrystalfb.h"
#include "servercommon/string/gameworldstr.h"
#include "other/pet/pet.hpp"
#include "other/partner/partner.hpp"
#include "gameworld/gamelog.h"
#include "task/taskmanager.h"

SpecialLogicAdvanceFb::SpecialLogicAdvanceFb(Scene * scene) : SpecialLogic(scene), m_adv(0), m_play_type(0), m_sp_param(0), m_layer_num(0), m_boss_num(0), m_npc_num(0), m_boss_id(INVALID_OBJ_ID), m_boss_id2(INVALID_OBJ_ID)
{
}

SpecialLogicAdvanceFb::~SpecialLogicAdvanceFb()
{
}

const AttributeList & SpecialLogicAdvanceFb::ReCalcAttrPer(AttributeList & base_add, const AttributeList & cpy_attr_list)
{
	m_attr_inc.Reset();
	base_add.Add(m_attr_inc.m_attrs);
	return m_attr_inc;
}

void SpecialLogicAdvanceFb::OnRoleEnterScene(Role * role)
{
	if (NULL == role) return;

	role->SetEncounterMonsterFlag(true);
	this->SendAllObjInfo();
	this->SendSpecialTaskInfo();
}

void SpecialLogicAdvanceFb::OnRoleLeaveScene(Role * role, bool is_logout)
{
	if (NULL == role) return;

	role->RestoreLastEncounterMonsterFlag();
}

void SpecialLogicAdvanceFb::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (BATTLE_MODE_ADVANCE_FB == ack->battle_mode &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}

	if (m_play_type == ADV_TYPE_CAN_COMMIT_TASK || m_play_type == ADV_TYPE_TASK_PASS)//处理怪物击杀特殊掉落
	{
		this->OnkillMonster(&ack->statistic_data);
	}
	int npc_seq_id = ack->business_data.monster_gen_id;
	NPCMonster* npc_monster = NpcManager::Instance().GetNPCMonster(npc_seq_id);
	if (NULL == npc_monster) return;	// 如果打的不是明雷，不用管，直接返回；

	if (npc_monster->GetScene()->GetSceneID() != m_scene->GetSceneID() ||
		npc_monster->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
	{
		return;
	}

	if (RESULT_TYPE_ATTACKER_WIN != ack->battle_result) return;

	// 击杀名怪奖励，检测
	const AdvanceDropCfg * drop_cfg = LOGIC_CONFIG->GetAdvanceFbConfig()->GetDropByNPCidCfg(npc_monster->GetNpcId());
	if (drop_cfg != NULL)
	{
		for (int i = 0; i < ack->attacker_list.role_count; ++i)
		{
			Role * temp_role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
			if (NULL == temp_role)
			{
				continue;
			}

			bool check_has_task = false;
			for (size_t k = 0; k < drop_cfg->task_id_list.size(); ++k)
			{
				if (temp_role->GetRoleModuleManager()->GetTaskManager()->CheckRoleHasTask(drop_cfg->task_id_list[k]))
				{
					check_has_task = true;
					break;
				}
			}
			if (!check_has_task)
			{
				continue;
			}

			for(std::vector<ItemConfigData>::const_iterator it = drop_cfg->item_vec.begin();it != drop_cfg->item_vec.end();it++)
			{
				if (temp_role->GetRoleModuleManager()->GetKnapsack()->HasItem(it->item_id, 1))continue;
				if (temp_role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(it->item_id, it->num))
				{
					temp_role->GetRoleModuleManager()->GetKnapsack()->Put(*it, PUT_REASON_ADVANCE_REWARD);
					//记录奖励-通关副本后给予
					/*m_reward_vec[temp_role->GetUID()].push_back(drop_cfg->item);
					this->SendRoleDropInfo(temp_role);*/
				}
				else
				{
					gamelog::g_log_knapsack.printf(LL_INFO, "SpecialLogicAdvanceFb::OnNPCChosedReward role[%s,%d] knapsack full no put item[id:%d,num:%d,is_bind:%d]",
						temp_role->GetName(), temp_role->GetUID(), it->item_id, it->num, it->is_bind ? 1 : 0);
				}
			}
		}
	}

	ObjID obj_id = npc_monster->GetId();
	for (std::vector<SpecialObjType>::iterator it = m_obj_vec.begin();it != m_obj_vec.end();it++)
	{
		if (it->obj_id == obj_id)
		{
			m_obj_vec.erase(it);
			this->SendAllObjInfo();
			break;
		}
	}
	if (obj_id == m_boss_id && obj_id != INVALID_OBJ_ID)
	{
		m_boss_num -= 1;
		const AdvFbInfoCfg  * list_cfg = LOGIC_CONFIG->GetAdvanceFbConfig()->GetAdvFbInfoCfg(m_adv, m_layer_num);
		if (m_boss_id2 == INVALID_OBJ_ID && list_cfg != NULL && list_cfg->boss_npc_id2 != 0)//是否有第二个boss
		{
			const ServerNPCInfoCfg * npc_cfg = NPCPOOL->GetServerNPCInfo(list_cfg->boss_npc_id2);
			if (npc_cfg != NULL)
			{
				if (npc_cfg->server_npc_type == SERVER_NPC_TYPE_CHOSED)
				{
					NPCChosed*  monster2 = NpcManager::Instance().CreateNPCChosed(list_cfg->boss_npc_id2, m_scene, list_cfg->boss_pos2, BATTLE_MODE_ADVANCE_FB, &list_cfg->dir_y2);
					if (NULL != monster2)
					{
						m_boss_id2 = monster2->GetId();
						m_boss_num += 1;
						SpecialObjType node;
						node.obj_id = monster2->GetId();
						node.obj_type = SPECIALLOGIC_OBJ_TYPE_BOSS;
						node.obj_pos.x = list_cfg->boss_pos2.x;
						node.obj_pos.y = list_cfg->boss_pos2.y;
						m_obj_vec.push_back(node);
					}
				}
				else if (npc_cfg->server_npc_type == SERVER_NPC_TYPE_MONSTER)
				{
					NPCMonster * monster2 = NpcManager::Instance().CreateNPCMonster(list_cfg->boss_npc_id2, m_scene, list_cfg->boss_pos2, BATTLE_MODE_ADVANCE_FB, &list_cfg->dir_y2);
					if (NULL != monster2)
					{
						m_boss_id2 = monster2->GetId();
						m_boss_num += 1;
						SpecialObjType node;
						node.obj_id = monster2->GetId();
						node.obj_type = SPECIALLOGIC_OBJ_TYPE_BOSS;
						node.obj_pos.x = list_cfg->boss_pos2.x;
						node.obj_pos.y = list_cfg->boss_pos2.y;
						m_obj_vec.push_back(node);
					}
				}
			}			
		}
	}
	else if (obj_id == m_boss_id2 && obj_id != INVALID_OBJ_ID)
	{
		m_boss_num -= 1;
	}

	UNSTD_STATIC_CHECK(4 == ADV_TYPE_MAX)
	switch (m_play_type)
	{
	case ADV_TYPE_PASS_AGAIN:	// 如果 NPC 明雷被清理完了，则出现传送门；
		if (m_boss_num <= 0) this->CreateTransToNextLayer();
		break;
	case ADV_TYPE_TASK_PASS:	// 如果 NPC 明雷被清理完了，则出现传送门；
		if (m_boss_num <= 0) this->CreateTransToNextLayer();
		break;
	default:
		break;
	}
	m_scene->DeleteObj(obj_id);		// 删掉怪物
	this->SendAllObjInfo();
}

void SpecialLogicAdvanceFb::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack)
{
	if (BATTLE_MODE_ADVANCE_FB == ack->battle_mode &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}
}

void SpecialLogicAdvanceFb::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack)
{
	if (BATTLE_MODE_ADVANCE_FB == ack->battle_mode &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}
}

void SpecialLogicAdvanceFb::OnTransport(Role * role, SpecialTransport * transport_obj)
{
	if (NULL == role || NULL == transport_obj) return;

	//if (role->InTeam() && !role->GetMyTeam(false)->IsLeader(role))return;

	int scene_id = transport_obj->GetExtraInfo().param1;
	int scene_key = m_scene->GetSceneKey();

	SpecialLogicAdvanceFb * sp_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicAdvanceFb(scene_id, scene_key);
	if (NULL != sp_logic)	// 如果还有下一层，则进入下一层；
	{
		Posi target_pos = sp_logic->GetScene()->GetARandPosi();
		if (m_play_type == ADV_TYPE_CAN_COMMIT_TASK || m_play_type == ADV_TYPE_TASK_PASS)
		{
			const AdvFbInfoCfg  * list_cfg = LOGIC_CONFIG->GetAdvanceFbConfig()->GetAdvFbInfoCfg(m_adv, m_layer_num);
			if(list_cfg != NULL)
			{
				if (m_sp_task.task_id != 0 && m_sp_task.task_id == list_cfg->task_id && list_cfg->item_num > 0)
				{
					/*if (role->GetRoleModuleManager()->GetKnapsack()->HasItem(list_cfg->item_id, list_cfg->item_num))
					{
						role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(list_cfg->item_id, list_cfg->item_num, "OnFinishAdvanceFb");
					}
					else return;*/
					if (m_sp_task.progress < list_cfg->item_num)
					{
						role->GetRoleModuleManager()->NoticeNum(errornum::EN_SPECIAL_ADVANCE_FB_PASS_ERR);
						return;
					}
					else
					{
						m_sp_task.task_id = 0;
						m_sp_task.progress = 0;
					}
				}
			}
			m_sp_param = m_sp_task.progress;
		}
		sp_logic->InitAdvanceFbScene(m_adv, m_sp_param, m_layer_num + 1);
		const AdvFbInfoCfg  * list_cfg = LOGIC_CONFIG->GetAdvanceFbConfig()->GetAdvFbInfoCfg(m_adv, m_layer_num + 1);
		if (list_cfg != NULL)target_pos = list_cfg->enter_pos;
		Team * team = role->GetMyTeam();
		if (NULL == team)
		{
			if (NULL != role) World::GetInstWorld()->GetSceneManager()->GoTo(role, scene_id, scene_key, target_pos);
		}
		else
		{
			World::GetInstWorld()->GetSceneManager()->TeamGoTo(team, scene_id, scene_key, target_pos);
			/*int mem_count = team->GetMemberCount();
			for (int mem_idx = 0; mem_idx < mem_count; ++mem_idx)
			{
				Role * mem_role = team->GetMemberRoleByIndex(mem_idx);		// 不检测暂离，默认全队传送
				if (NULL != mem_role) World::GetInstWorld()->GetSceneManager()->GoTo(mem_role, scene_id, scene_key, target_pos);
			}*/
		}
	}
	else	// 如果没有下一层，宣布通关
	{
		if (m_play_type == ADV_TYPE_CAN_COMMIT_TASK || m_play_type == ADV_TYPE_TASK_PASS)
		{
			const AdvFbInfoCfg  * list_cfg = LOGIC_CONFIG->GetAdvanceFbConfig()->GetAdvFbInfoCfg(m_adv, m_layer_num);
			if (list_cfg != NULL)
			{
				if (m_sp_task.task_id != 0 && m_sp_task.task_id == list_cfg->task_id && list_cfg->item_num > 0)
				{
					if (m_sp_task.progress < list_cfg->item_num)
					{
						return;
					}
					else
					{
						m_sp_task.task_id = 0;
						m_sp_task.progress = 0;
					}
				}
			}
			/*if (NULL != ITEMPOOL->GetItem(list_cfg->item_id) && list_cfg->item_num > 0)
			{
				if (role->GetRoleModuleManager()->GetKnapsack()->HasItem(list_cfg->item_id, list_cfg->item_num))
				{
					role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(list_cfg->item_id, list_cfg->item_num, "OnFinishAdvanceFb");
				}
				else
				{
					return;
				}
			}*/
			this->OnFinishAdvanceFb();
		}
		else
		{
			this->OnFinishAdvanceFb();
		}
	}
}

void SpecialLogicAdvanceFb::InitAdvanceFbScene(int adv_idx, int param, int layer, std::map<int, std::vector<ItemConfigData> > * item_vec/* = NULL*/)
{
	//设置
	const AdvFbInfoCfg  * list_cfg = LOGIC_CONFIG->GetAdvanceFbConfig()->GetAdvFbInfoCfg(adv_idx, layer);
	if (list_cfg == NULL)return;
	m_adv = adv_idx;
	m_play_type = list_cfg->play_type;
	m_sp_param = param;
	m_layer_num = layer;
	m_boss_num = 0;
	if(item_vec != NULL)
	{
		for (std::map<int, std::vector<ItemConfigData> >::const_iterator it = item_vec->begin();it != item_vec->end();it++)
		{
			m_reward_vec[it->first].assign(it->second.begin(), it->second.end());
		}
	}
	UNSTD_STATIC_CHECK(4 == ADV_TYPE_MAX)
	switch (m_play_type)
	{
	case ADV_TYPE_DEFEAT:
		{
			this->CreateTransToNextLayer();
		}
		break;
	case ADV_TYPE_CAN_COMMIT_TASK://相当于任务
		{
			if (list_cfg->task_id != 0 && m_sp_param == 0)//有任务同时特殊参数为0
			{
				m_sp_task.task_id = list_cfg->task_id;
				m_sp_task.progress = 0;
			}
			else if(list_cfg->task_id != 0 && m_sp_param != 0)//特殊参数不为0
			{
				m_sp_task.task_id = list_cfg->task_id;
				m_sp_task.progress = m_sp_param;
			}
			this->CreateTransToNextLayer();
			
		}
		break;
	case ADV_TYPE_PASS_AGAIN:
	case ADV_TYPE_TASK_PASS://相当于任务
		{
			if (list_cfg->task_id != 0 && m_sp_param == 0)//有任务同时特殊参数为0
			{
				m_sp_task.task_id = list_cfg->task_id;
				m_sp_task.progress = 0;
			}
			else if (list_cfg->task_id != 0 && m_sp_param != 0)//特殊参数不为0
			{
				m_sp_task.task_id = list_cfg->task_id;
				m_sp_task.progress = m_sp_param;
			}
			// 如果是最高层，刷BOSS；
			if (list_cfg->boss_npc_id != 0)
			{
				const ServerNPCInfoCfg * npc_cfg = NPCPOOL->GetServerNPCInfo(list_cfg->boss_npc_id);
				if (NULL != npc_cfg && npc_cfg->server_npc_type == SERVER_NPC_TYPE_CHOSED)
				{
					NPCChosed*  monster = NpcManager::Instance().CreateNPCChosed(list_cfg->boss_npc_id, m_scene, list_cfg->boss_pos, BATTLE_MODE_ADVANCE_FB, &list_cfg->dir_y1);
					if (NULL != monster)
					{
						m_boss_id = monster->GetId();
						m_boss_num += 1;
						SpecialObjType node;
						node.obj_id = monster->GetId();
						node.obj_type = SPECIALLOGIC_OBJ_TYPE_NPC;
						node.obj_pos.x = list_cfg->boss_pos.x;
						node.obj_pos.y = list_cfg->boss_pos.y;
						m_obj_vec.push_back(node);
					}
				}
				else if (NULL != npc_cfg && npc_cfg->server_npc_type == SERVER_NPC_TYPE_MONSTER)
				{
					NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(list_cfg->boss_npc_id, m_scene, list_cfg->boss_pos, BATTLE_MODE_ADVANCE_FB, &list_cfg->dir_y1);
					if (NULL != monster)
					{
						m_boss_id = monster->GetId();
						m_boss_num += 1;
						SpecialObjType node;
						node.obj_id = monster->GetId();
						node.obj_type = SPECIALLOGIC_OBJ_TYPE_BOSS;
						node.obj_pos.x = list_cfg->boss_pos.x;
						node.obj_pos.y = list_cfg->boss_pos.y;
						m_obj_vec.push_back(node);
					}
				}

				if (list_cfg->npc_id != 0)
				{
					NPCChosed * monster = NpcManager::Instance().CreateNPCChosed(list_cfg->npc_id, m_scene, list_cfg->npc_pos, BATTLE_MODE_NPC_FIGHT,&list_cfg->npc_dir);
					if (NULL != monster)
					{
						m_npc_num += 1;
						SpecialObjType node;
						node.obj_id = monster->GetId();
						node.obj_type = SPECIALLOGIC_OBJ_TYPE_NPC;
						node.obj_pos.x = list_cfg->npc_pos.x;
						node.obj_pos.y = list_cfg->npc_pos.y;
						m_obj_vec.push_back(node);
					}
				}
			}
			else	// 否则不是最高层，刷传送门；
			{
				this->CreateTransToNextLayer();
			}
		}
		break;
	}
}

/*
void SpecialLogicAdvanceFb::RequestFightCloneBoss(Role * role)
{
	if (NULL == role) return;

	NPCMonster * monster = m_scene->GetMonsterByObjID(m_boss_id);
	if (NULL == monster)
	{
		role->NoticeNum(errornum::EN_BOSS_ALREADY_DEFEAT);
		return;
	}

	const AdvFbCloneInformCfg & clone_info = LOGIC_CONFIG->GetAdvanceFbConfig()->GetAdvCloneCfg();
	BattleData_Business business; business.monster_gen_id = monster->GetNpcSeq();

	Team * team = role->GetMyTeam();
	if (NULL == team || team->GetMemberCount() <= 1)
	{
		RoleBattleData enemy_data;
		role->GetRoleModuleManager()->CollectRoleBattleData(&enemy_data, true);
		this->ChangeBattleData(&enemy_data, clone_info.clone_attr_change_rate);
		BattleManagerLocal::GetInstance().StartSingleFightShadowReq(role, enemy_data, BATTLE_MODE_ADVANCE_FB, business);
	}
	else
	{
		int team_num = team->GetMemberCount(), true_num = 0;	// 筛选机器人
		Role * attacker_list[SIDE_MAX_ROLE_NUM] = { NULL };
		RoleBattleData enemy_data[SIDE_MAX_ROLE_NUM];
		for (int i = 0; i < team_num && true_num < SIDE_MAX_ROLE_NUM; ++i)
		{
			Role * mem_role = team->GetMemberRoleByIndex(i);
			if (NULL == mem_role) continue;
			
			attacker_list[true_num] = mem_role;
			role->GetRoleModuleManager()->CollectRoleBattleData(&enemy_data[true_num], true);
			this->ChangeBattleData(&enemy_data[true_num], clone_info.clone_attr_change_rate);
			++true_num;
		}

		BattleManagerLocal::GetInstance().StartMultiFightShadowReq(true_num, attacker_list, true_num, enemy_data, BATTLE_MODE_ADVANCE_FB, business);
	}
}*/

void SpecialLogicAdvanceFb::OnRoleLeaveTeam(Role * role)
{
	if (role->GetUID() == this->GetScene()->GetSceneKey())return;
	this->DelayKickOutRole(role);
}

void SpecialLogicAdvanceFb::OnNPCChosedReward(Role * role, ObjID npc_objid, int option_index)
{
	Obj * npc_obj = m_scene->GetObj(npc_objid);
	if (npc_obj == NULL || Obj::OBJ_TYPE_NPC_CHOSED != npc_obj->GetObjType())return;

	NPCChosed* npc_chose = (NPCChosed*)npc_obj;
	const AdvanceNpcOption * npc_cfg = LOGIC_CONFIG->GetAdvanceFbConfig()->GetNpcOptionCfg(npc_chose->GetNpcId(), option_index);
	if (npc_cfg == NULL)return;

	switch (npc_cfg->operation_type)
	{
	case AdvanceNpcOption::ADVANCE_OPTION_TYPE_0:
		{
			const AdvFbInfoCfg  * list_cfg = LOGIC_CONFIG->GetAdvanceFbConfig()->GetAdvFbInfoCfg(m_adv, m_layer_num);
			if (list_cfg != NULL)
			{
				this->RoleTeamCure(role, list_cfg->recover_num);
			}
		}
		break;
	case AdvanceNpcOption::ADVANCE_OPTION_TYPE_1:
		{
			this->TeamReward(role, npc_cfg);
			const AdvFbInfoCfg  * list_cfg = LOGIC_CONFIG->GetAdvanceFbConfig()->GetAdvFbInfoCfg(m_adv, m_layer_num);
			if (m_boss_id2 == INVALID_OBJ_ID && list_cfg != NULL && list_cfg->boss_npc_id2 != 0)//是否有第二个boss
			{
				const ServerNPCInfoCfg * npc_cfg = NPCPOOL->GetServerNPCInfo(list_cfg->boss_npc_id2);
				if (npc_cfg != NULL)
				{
					if (npc_cfg->server_npc_type == SERVER_NPC_TYPE_CHOSED)
					{
						NPCChosed*  monster2 = NpcManager::Instance().CreateNPCChosed(list_cfg->boss_npc_id2, m_scene, list_cfg->boss_pos2, BATTLE_MODE_ADVANCE_FB, &list_cfg->dir_y2);
						if (NULL != monster2)
						{
							m_boss_id2 = monster2->GetId();
							m_boss_num += 1;
							SpecialObjType node;
							node.obj_id = monster2->GetId();
							node.obj_type = SPECIALLOGIC_OBJ_TYPE_BOSS;
							node.obj_pos.x = list_cfg->boss_pos2.x;
							node.obj_pos.y = list_cfg->boss_pos2.y;
							m_obj_vec.push_back(node);
						}
					}
					else if (npc_cfg->server_npc_type == SERVER_NPC_TYPE_MONSTER)
					{
						NPCMonster * monster2 = NpcManager::Instance().CreateNPCMonster(list_cfg->boss_npc_id2, m_scene, list_cfg->boss_pos2, BATTLE_MODE_ADVANCE_FB, &list_cfg->dir_y2);
						if (NULL != monster2)
						{
							m_boss_id2 = monster2->GetId();
							m_boss_num += 1;
							SpecialObjType node;
							node.obj_id = monster2->GetId();
							node.obj_type = SPECIALLOGIC_OBJ_TYPE_BOSS;
							node.obj_pos.x = list_cfg->boss_pos2.x;
							node.obj_pos.y = list_cfg->boss_pos2.y;
							m_obj_vec.push_back(node);
						}
					}
				}
			}
		}
		break;
	default:
		break;
	}

	if (npc_objid == m_boss_id && npc_objid != INVALID_OBJ_ID && m_boss_id != INVALID_OBJ_ID)
	{
		m_boss_num -= 1;
	}
	else if (npc_objid == m_boss_id2 && npc_objid != INVALID_OBJ_ID && m_boss_id2 != INVALID_OBJ_ID)
	{
		m_boss_num -= 1;
	}
	else if(npc_objid != INVALID_OBJ_ID && m_boss_id2 != INVALID_OBJ_ID)
	{
		m_npc_num -= 1;
	}

	UNSTD_STATIC_CHECK(4 == ADV_TYPE_MAX)
	switch (m_play_type)
	{
	case ADV_TYPE_PASS_AGAIN:	// 如果 NPC 明雷被清理完了，则出现传送门；
		if (m_boss_num <= 0) this->CreateTransToNextLayer();
		break;
	case ADV_TYPE_TASK_PASS:	// 如果 NPC 明雷被清理完了，则出现传送门；
		if (m_boss_num <= 0) this->CreateTransToNextLayer();
		break;
	default:
		break;
	}
	if (npc_cfg->is_delete == 1)
	{
		m_scene->DeleteObj(npc_objid);
		for (std::vector<SpecialObjType>::iterator it = m_obj_vec.begin(); it != m_obj_vec.end(); it++)
		{
			if (it->obj_id == npc_objid)
			{
				m_obj_vec.erase(it);
				this->SendAllObjInfo();
				break;
			}
		}
	}
}

void SpecialLogicAdvanceFb::RoleTeamCure(Role * role, int cure_present)
{
	if (role == NULL || cure_present <= 0 || cure_present > 100)return;
	if (role->InTeam())
	{
		Team* role_team = role->GetMyTeam();
		if (role_team == NULL)return;
		for (int i = 0; i < role_team->GetMemberCount() && i < MAX_TEAM_MEMBER_NUM; i++)
		{
			Role *temp_role = role_team->GetMemberRoleByIndex(i);
			if (temp_role == NULL || !temp_role->IsFollow() )continue;
			this->AloneCure(temp_role, cure_present);
		}
	}
	else
	{
		this->AloneCure(role, cure_present);
	}
}

void SpecialLogicAdvanceFb::AloneCure(Role * role, int cure_present)
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

	Pet* role_pet = role->GetRoleModuleManager()->GetPet();
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
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
		}
	}
}

void SpecialLogicAdvanceFb::TeamReward(Role * role, const AdvanceNpcOption * reward)
{
	if (role == NULL || reward == NULL )return;
	if (role->InTeam())
	{
		Team* role_team = role->GetMyTeam();
		if (role_team == NULL)return;
		for (int i = 0; i < role_team->GetMemberCount() && i < MAX_TEAM_MEMBER_NUM; i++)
		{
			Role *temp_role = role_team->GetMemberRoleByIndex(i);
			if (temp_role == NULL || !temp_role->IsFollow() )continue;

			bool check_has_task = false;
			for (size_t k = 0; k < reward->task_id_list.size(); ++k)
			{
				if (temp_role->GetRoleModuleManager()->GetTaskManager()->CheckRoleHasTask(reward->task_id_list[k]))
				{
					check_has_task = true;
					break;
				}
			}
			if (!check_has_task)
			{
				continue;
			}
			
			if (temp_role->GetRoleModuleManager()->GetKnapsack()->HasItem(reward->item.item_id, 1))continue;
			if (temp_role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(reward->item.item_id, reward->item.num))
			{
				temp_role->GetRoleModuleManager()->GetKnapsack()->Put(reward->item, PUT_REASON_ADVANCE_REWARD);
				//记录奖励-通关副本后给予
				/*m_reward_vec[temp_role->GetUID()].push_back(reward->item);
				this->SendRoleDropInfo(temp_role);*/
			}
			else
			{
				gamelog::g_log_knapsack.printf(LL_INFO, "SpecialLogicAdvanceFb::OnNPCChosedReward role[%s,%d] knapsack full no put item[id:%d,num:%d,is_bind:%d]",
					temp_role->GetName(), temp_role->GetUID(), reward->item.item_id, reward->item.num, reward->item.is_bind ? 1 : 0);
			}
		}
	}
	else
	{
		if (role->GetRoleModuleManager()->GetKnapsack()->HasItem(reward->item.item_id, 1))return;
		if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(reward->item.item_id, reward->item.num))
		{
			role->GetRoleModuleManager()->GetKnapsack()->Put(reward->item, PUT_REASON_ADVANCE_REWARD);
			//记录奖励-通关副本后给予
			/*m_reward_vec[role->GetUID()].push_back(reward->item);
			this->SendRoleDropInfo(role);*/
		}
		else
		{
			gamelog::g_log_knapsack.printf(LL_INFO, "SpecialLogicAdvanceFb::OnNPCChosedReward role[%s,%d] knapsack full no put item[id:%d,num:%d,is_bind:%d]",
				role->GetName(), role->GetUID(), reward->item.item_id, reward->item.num, reward->item.is_bind ? 1 : 0);
		}
	}
}

void SpecialLogicAdvanceFb::SendAllObjInfo()
{
	Protocol::SCAdvanceFbObjInfo scene_info;
	scene_info.obj_count = 0;
	for (std::vector<SpecialObjType>::iterator it = m_obj_vec.begin();it != m_obj_vec.end();it++)
	{
		scene_info.obj_list[scene_info.obj_count].obj_id = it->obj_id;
		scene_info.obj_list[scene_info.obj_count].obj_type = it->obj_type;
		scene_info.obj_list[scene_info.obj_count].obj_pos_x = it->obj_pos.x;
		scene_info.obj_list[scene_info.obj_count].obj_pos_y = it->obj_pos.y;
		scene_info.obj_count += 1;
	}
	for (int index = 0; index < (int)m_scene->RoleNum(); ++index)
	{
		Role *temp_role = m_scene->GetRoleByIndex(index);
		if (NULL != temp_role)
		{
			EngineAdapter::Instance().NetSend(temp_role->GetNetId(),(const void*)&scene_info, sizeof(scene_info) - ((Protocol::SCAdvanceFbObjInfo::MAX_OBJ_LIST_COUNT - scene_info.obj_count) * sizeof(scene_info.obj_list[0])));
		}
	}
	//m_scene->SendToRole((const char*)&scene_info,sizeof(scene_info) - ((Protocol::SCAdvanceFbObjInfo::MAX_OBJ_LIST_COUNT - scene_info.obj_count)*sizeof(scene_info.obj_list[0])));
}

void SpecialLogicAdvanceFb::SendSpecialTaskInfo()
{
	Protocol::SCAdvanceFbTaskInfo task_info;
	task_info.task_id = m_sp_task.task_id;
	task_info.progress = m_sp_task.progress;
	for (int index = 0; index < (int)m_scene->RoleNum(); ++index)
	{
		Role *temp_role = m_scene->GetRoleByIndex(index);
		if (NULL != temp_role)
		{
			EngineAdapter::Instance().NetSend(temp_role->GetNetId(), (const void*)&task_info, sizeof(task_info));
		}
	}
}

void SpecialLogicAdvanceFb::SendRoleDropInfo(Role * role)
{
	if (NULL == role) return;

	Protocol::SCADVdropRewardInfo scene_info;
	scene_info.reward_count = 0;
	for (std::map<int, std::vector<ItemConfigData> >::iterator it = m_reward_vec.begin(); it != m_reward_vec.end(); it++)
	{
		if (role->GetUID() == it->first)
		{
			if ((int)it->second.size() > 0 && scene_info.reward_count < Protocol::SCADVdropRewardInfo::MAX_DROP_DATA_NUM)
			{
				for (std::vector<ItemConfigData>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
				{
					scene_info.reward_list[scene_info.reward_count].item_id = it2->item_id;
					scene_info.reward_list[scene_info.reward_count].is_bind = it2->is_bind ? 1 : 0;
					scene_info.reward_list[scene_info.reward_count].num = it2->num;
					scene_info.reward_count += 1;
				}

			}
		
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&scene_info, sizeof(scene_info) - ((Protocol::SCADVdropRewardInfo::MAX_DROP_DATA_NUM - scene_info.reward_count) * sizeof(scene_info.reward_list[0])));	
		}
	}
}

void SpecialLogicAdvanceFb::CreateTransToNextLayer()
{
	SpecialTransport * transport_obj = new SpecialTransport();
	if (transport_obj == NULL) return;

	UNSTD_STATIC_CHECK(4 == ADV_TYPE_MAX)
	switch (m_play_type)
	{
	case ADV_TYPE_DEFEAT:
		{
			const AdvFbInfoCfg * scene_cfg = LOGIC_CONFIG->GetAdvanceFbConfig()->GetAdvFbInfoCfg(m_adv,m_layer_num);
			if (NULL != scene_cfg) 
			{
				transport_obj->GetExtraInfo().send_param1 = scene_cfg->transport_id;
				transport_obj->GetExtraInfo().param1 = scene_cfg->transport_id;
				transport_obj->SetPos(scene_cfg->transport_pos);
				SpecialObjType node;
				node.obj_id = transport_obj->GetId();
				node.obj_type = SPECIALLOGIC_OBJ_TYPE_TRANSPROT;
				node.obj_pos.x = scene_cfg->transport_pos.x;
				node.obj_pos.y = scene_cfg->transport_pos.y;
				m_obj_vec.push_back(node);
			}
		}
		break;
	case ADV_TYPE_CAN_COMMIT_TASK:
		{
			const AdvFbInfoCfg * scene_cfg = LOGIC_CONFIG->GetAdvanceFbConfig()->GetAdvFbInfoCfg(m_adv, m_layer_num);
			if (NULL != scene_cfg)
			{
				transport_obj->GetExtraInfo().send_param1 = scene_cfg->transport_id;
				transport_obj->GetExtraInfo().param1 = scene_cfg->transport_id;
				transport_obj->SetPos(scene_cfg->transport_pos);
				SpecialObjType node;
				node.obj_id = transport_obj->GetId();
				node.obj_type = SPECIALLOGIC_OBJ_TYPE_TRANSPROT;
				node.obj_pos.x = scene_cfg->transport_pos.x;
				node.obj_pos.y = scene_cfg->transport_pos.y;
				m_obj_vec.push_back(node);
			}
			//else { this->OnFinishAdvanceFb(); }
		}
		break;
	case ADV_TYPE_PASS_AGAIN:
	case ADV_TYPE_TASK_PASS:
		{
			const AdvFbInfoCfg * scene_cfg = LOGIC_CONFIG->GetAdvanceFbConfig()->GetAdvFbInfoCfg(m_adv, m_layer_num);
			if (NULL != scene_cfg)
			{
				transport_obj->GetExtraInfo().send_param1 = scene_cfg->transport_id;
				transport_obj->GetExtraInfo().param1 = scene_cfg->transport_id;
				transport_obj->SetPos(scene_cfg->transport_pos);
				SpecialObjType node;
				node.obj_id = transport_obj->GetId();
				node.obj_type = SPECIALLOGIC_OBJ_TYPE_TRANSPROT;
				node.obj_pos.x = scene_cfg->transport_pos.x;
				node.obj_pos.y = scene_cfg->transport_pos.y;
				m_obj_vec.push_back(node);
			}
			else { this->OnFinishAdvanceFb(); }
		}
		break;
	}
	
	m_scene->AddObj(transport_obj);
	this->SendAllObjInfo();
}

void SpecialLogicAdvanceFb::OnFinishAdvanceFb()
{
	int role_num = m_scene->RoleNum();
	for (int role_idx = 0; role_idx < role_num; ++role_idx)
	{
		Role * role = m_scene->GetRoleByIndex(role_idx);
		if (NULL != role) EventHandler::Instance().OnRoleClearFB(role, FBManager::FB_TYPE_ADVANCE_FB);
	}

	this->ClearAllNpcMonster();		// 清理所有明雷；
	this->DelayKickOutAllRole();	// 直接踢出副本；
}

void SpecialLogicAdvanceFb::ChangeBattleData(RoleBattleData * out_data, int attr_change_rate)
{
	/*double attr_rate = attr_change_rate / 100.0 + 1;
	// 角色属性变化
	BattleCharacterData & battle_data = out_data->role_character_data;
	for (int attr_type = 0; attr_type < BATTLE_ATTR_MAX; ++attr_type)
	{
		battle_data.attr_list[attr_type] *= attr_rate;
	}

	// 宠物属性变化
	for (int pet_idx = 0; pet_idx < ROLE_BATTLE_PET_NUM; ++pet_idx)
	{
		BattleCharacterData & pet_data = out_data->pet_character_data[pet_idx].battle_data;
		for (int attr_type = 0; attr_type < BATTLE_ATTR_MAX; ++attr_type)
		{
			pet_data.attr_list[attr_type] *= attr_rate;
		}
	}

	// 伙伴属性变化
	int side_max = SIDE_MAX_ROLE_NUM - 1;
	for (int partner_idx = 0; partner_idx < side_max; ++partner_idx)
	{
		BattleCharacterData & partner_data = out_data->partner_character_data[partner_idx].battle_data;
		for (int attr_type = 0; attr_type < BATTLE_ATTR_MAX; ++attr_type)
		{
			partner_data.attr_list[attr_type] *= attr_rate;
		}
	}*/
}

void SpecialLogicAdvanceFb::OnkillMonster(const BattleData_Statistic * data)
{
	//只检查队长，副本归属者
	int ower_id = this->GetScene()->GetSceneKey();//场景key是副本归属者role_id
	Role * role = this->GetScene()->GetRoleByUID(ower_id);
	if (NULL != role)
	{
		if (role->InTeam() && !role->GetMyTeam(false)->IsLeader(role))return;
		const AdvFbInfoCfg * cfg = LOGIC_CONFIG->GetAdvanceFbConfig()->GetAdvFbInfoCfg(m_adv, m_layer_num);
		if (cfg != NULL && m_sp_task.task_id != 0 && (int)cfg->monster_drop_list.size() != 0 && cfg->drop_num > 0)
		{
			bool has_add = false;
			for (int monster_index = 0; monster_index < data->monster_list_size && monster_index < SIDE_CHARACTER_NUM;monster_index++)
			{
				if (this->HasMonsterDropByID(&cfg->monster_drop_list, data->monster_id_list[monster_index]))
				{
					if (m_sp_task.progress < cfg->drop_num)
					{
						/*if (DROPPOOL->DropToRole(role, cfg->drop_id))
						{
							m_sp_task.progress += 1;
						}*/
						m_sp_task.progress += 1;
						has_add = true;
					}
				}
			}
			if (has_add)
			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_advancefb_task_notic,
					m_sp_task.progress);

				if (length > 0)
				{
					static Protocol::SCSystemMsg sm;
					int sendlen = 0;

					if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT)))
					{
						m_scene->SendToRole((const char*)&sm, sendlen);
						//EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
					}
				}
			}
			this->SendSpecialTaskInfo();
		}
	}
}

bool SpecialLogicAdvanceFb::HasMonsterDropByID(const std::vector<int>* monster_drop_list, int monster_id)
{
	if (monster_drop_list == NULL)return false;
	if (monster_id <= 0)return false;
	for (std::vector<int>::const_iterator it = monster_drop_list->begin();it != monster_drop_list->end(); ++it)
	{
		if (*it == monster_id)
		{
			return true;
		}
	}
	return false;
}
