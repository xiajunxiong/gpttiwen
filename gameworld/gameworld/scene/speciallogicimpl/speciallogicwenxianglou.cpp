#include "speciallogicwenxianglou.hpp"
#include "world.h"
#include "obj/character/role.h"
#include "obj/character/npc_monster.h"
#include "config/logicconfigmanager.hpp"
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
#include "gameworld/gamelog.h"
#include "task/taskmanager.h"
#include "other/wenxianglou/wenxiangloufbconfig.hpp"
#include "other/event/eventhandler.hpp"
#include "servercommon/anecdotetaskdef.hpp"
#include "other/anecdotetask/anecdotetask.hpp"

SpecialLogicWenXiangLouFb::SpecialLogicWenXiangLouFb(Scene * scene) : SpecialLogic(scene)
{
	check_enter_posi_tamp = 0;
	role_enter_posi = false;
	role_enter_posi2 = false;

	load_status = WXL_LOAD_TYPE_0;
	memset(npc_sucspicion_list, 0, sizeof(npc_sucspicion_list));
	memset(npc_talk_list, 0, sizeof(npc_talk_list));
	memset(npc_identity_list, 0, sizeof(npc_identity_list));
	memset(npc_check_list, 0, sizeof(npc_check_list));

	has_find_identity = false;
	has_finish_fb = false;

	reward_count = 0;
	memset(reward_list, 0, sizeof(reward_list));
}

SpecialLogicWenXiangLouFb::~SpecialLogicWenXiangLouFb()
{
}

void SpecialLogicWenXiangLouFb::Update(unsigned long interval, time_t now_second)
{
	if (check_enter_posi_tamp >= 0 && check_enter_posi_tamp <= now_second)
	{
		check_enter_posi_tamp = static_cast<unsigned int>(now_second + 1);
		this->CheckObjEnterPosi();
	}
}

void SpecialLogicWenXiangLouFb::OnRoleEnterScene(Role * role)
{
	if (NULL == role) return;
	this->SendNPCInfo(role);
	this->SendSceneInfo(role);
	const EventConfig * event_cfg = LOGIC_CONFIG->GetWenXiangLouFbConfig()->GetEventCfg(load_status);
	if (event_cfg == NULL)return;
	//瞬移玩家到指定位置
	wenxianglou_has_enter = 1;
	role->ResetPos(event_cfg->x, event_cfg->y);
}

void SpecialLogicWenXiangLouFb::OnRoleLeaveScene(Role * role, bool is_logout)
{
	if (NULL == role) return;
	RoleModuleManager* m_role_manager = role->GetRoleModuleManager();
	TaskWenXiangLouFbParam & param = m_role_manager->GetTaskManager()->GetWenXiangLouParam();

	param.role_enter_posi = role_enter_posi == 1 ? 1 : 0;
	param.has_find_identity = has_find_identity == 1 ? 1 : 0;
	param.has_finish_fb = has_finish_fb == 1 ? 1 : 0;
	param.load_status = load_status;
	param.wenxianglou_has_enter = 1;
	for (int i = 0; i < MAX_NPC_NUM_WXL; i++)
	{
		param.npc_sucspicion_list[i] = npc_sucspicion_list[i];
		param.npc_talk_list[i] = npc_talk_list[i];
		param.npc_identity_list[i] = npc_identity_list[i];
		param.npc_check_list[i] = npc_check_list[i];
	}
	param.reward_count = 0;
	for (int i = 0; i < reward_count && i < MAX_SPECIAL_ITEM_NUM; i++)
	{
		param.reward_count++;
		param.reward_list[i] = reward_list[i];
	}
}

void SpecialLogicWenXiangLouFb::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	//this->SyncHpMp(ack->special_business_data.data);
	
	if (RESULT_TYPE_ATTACKER_WIN != ack->battle_result) return;

	if (load_status <= WXL_LOAD_TYPE_2)
	{
		if (has_find_identity)
		{
			load_status = WXL_LOAD_TYPE_3;
		}
	}
	else if (load_status == WXL_LOAD_TYPE_3)
	{
		has_finish_fb = true;
		this->OnFbFinish();
	}
	this->SendNPCInfo();
	this->SendSceneInfo();
}

void SpecialLogicWenXiangLouFb::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack)
{
	if (BATTLE_MODE_ADVANCE_FB == ack->battle_mode &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}
}

void SpecialLogicWenXiangLouFb::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack)
{
	if (BATTLE_MODE_ADVANCE_FB == ack->battle_mode &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}
}

bool SpecialLogicWenXiangLouFb::CanEnterTeam(Role * role, Team * team, bool is_from_create)
{
	if (role->GetMyTeam() != NULL)return false;
	if (team != NULL)return false;
	if (is_from_create)return false;
	return true;
}

void SpecialLogicWenXiangLouFb::InitFbScene(Role *role)
{
	if (role == NULL)return;
	//设置

	RoleModuleManager* m_role_manager = role->GetRoleModuleManager();
	TaskWenXiangLouFbParam & param = m_role_manager->GetTaskManager()->GetWenXiangLouParam();

	role_enter_posi = param.role_enter_posi == 1 ? 1 : 0;
	has_find_identity = param.has_find_identity == 1 ? 1 : 0;
	has_finish_fb = param.has_finish_fb == 1 ? 1 : 0;
	load_status = param.load_status;
	wenxianglou_has_enter = param.wenxianglou_has_enter;
	for (int i = 0; i < MAX_NPC_NUM_WXL; i++)
	{
		npc_sucspicion_list[i] = param.npc_sucspicion_list[i];
		npc_talk_list[i] = param.npc_talk_list[i];
		npc_identity_list[i] = param.npc_identity_list[i];
		npc_check_list[i] = param.npc_check_list[i];
	}
	
	for (int i = 0; i < param.reward_count && i < MAX_SPECIAL_ITEM_NUM; i++)
	{
		reward_count++;
		reward_list[i] = param.reward_list[i];
	}
	this->SendNPCInfo();
	this->SendSceneInfo();
}

void SpecialLogicWenXiangLouFb::TalkToNpc(Role * role, int npc_seq)
{
	if (role == NULL)return;
	const RoleClueConfig * npc_cfg = LOGIC_CONFIG->GetWenXiangLouFbConfig()->GetRoleClueCfg(npc_seq);
	if (npc_cfg == NULL)return;
	if (npc_cfg->seq < 0 || npc_cfg->seq >= MAX_NPC_NUM_WXL)
	{
		return;
	}
	if (load_status <= WXL_LOAD_TYPE_1)
	{
		if (npc_talk_list[npc_cfg->seq] == 1)
		{
			return;
		}
		npc_talk_list[npc_cfg->seq] = 1;
	}
	if (load_status >= WXL_LOAD_TYPE_2)
	{
		if (npc_check_list[npc_cfg->seq] == 0)
		{
			npc_check_list[npc_cfg->seq] = 1;
		}
		for (std::vector<int>::const_iterator it = npc_cfg->m_directed_list.begin(); it != npc_cfg->m_directed_list.end(); it++)
		{
			if (*it < 0 || *it >= MAX_NPC_NUM_WXL)
			{
				return;
			}
			npc_sucspicion_list[*it] += npc_cfg->add_suspicion;
		}
	}

	this->SendNPCInfo(role);
}

void SpecialLogicWenXiangLouFb::findItem(Role * role, int item_seq)
{
	if (role == NULL)return;
	const ItemClueConfig * item_cfg = LOGIC_CONFIG->GetWenXiangLouFbConfig()->GetItemClueCfg(item_seq);
	if (item_cfg == NULL)return;
	const OtherConfig * other_cfg = LOGIC_CONFIG->GetWenXiangLouFbConfig()->GetOtherCfg();
	if (other_cfg == NULL)return;

	Posi role_posi = role->GetPos();

	for (int i = 0;i < reward_count && i < MAX_SPECIAL_ITEM_NUM; i++)
	{
		if (reward_list[reward_count] == item_seq)
		{
			return;
		}
	}

	if (reward_count >= MAX_SPECIAL_ITEM_NUM)return;
	reward_list[reward_count] = item_cfg->seq;
	reward_count += 1;
	npc_sucspicion_list[item_cfg->directed] += item_cfg->add_suspicion;

	this->SendNPCInfo(role);
	this->SendSceneInfo(role);
}

void SpecialLogicWenXiangLouFb::IdentificatinNpc(Role * role, int npc_seq)
{
	if (role == NULL)return;
	const RoleClueConfig * npc_cfg = LOGIC_CONFIG->GetWenXiangLouFbConfig()->GetRoleClueCfg(npc_seq);
	if (npc_cfg == NULL)return;
	if (npc_cfg->seq < 0 || npc_cfg->seq >= MAX_NPC_NUM_WXL)
	{
		return;
	}
	if (npc_identity_list[npc_cfg->seq] == 1)
	{
		return;
	}

	npc_identity_list[npc_cfg->seq] = 1;

	if (npc_cfg->identity == WenXiangLouFbConfig::WEN_XIANG_LOU_IDENTITY_3)
	{
		has_find_identity = true;
		load_status = WXL_LOAD_TYPE_2;
	}

	this->SendNPCInfo(role);
	this->SendSceneInfo(role);
}

void SpecialLogicWenXiangLouFb::Finalfight(Role * role)
{
	const OtherConfig * other_cfg = LOGIC_CONFIG->GetWenXiangLouFbConfig()->GetOtherCfg();
	if (other_cfg == NULL)return;
	
	if (load_status == WXL_LOAD_TYPE_2)
	{
		if (has_find_identity)
		{
			BattleManagerLocal::GetInstance().StartMonsterFightReq(role, other_cfg->battle_1, BATTLE_MODE_NORMAL_MONSTER, 0);
		}
	}
	else if (load_status == WXL_LOAD_TYPE_3)
	{
		BattleManagerLocal::GetInstance().StartMonsterFightReq(role, other_cfg->battle_2, BATTLE_MODE_NORMAL_MONSTER, 0);
	}
}

void SpecialLogicWenXiangLouFb::CheckObjEnterPosi()
{
	const OtherConfig * other_cfg = LOGIC_CONFIG->GetWenXiangLouFbConfig()->GetOtherCfg();
	if (other_cfg == NULL)return;
	bool has_change = false;
	if(load_status < WXL_LOAD_TYPE_1 && role_enter_posi == 0)
	{
		const EventConfig * event_cfg = LOGIC_CONFIG->GetWenXiangLouFbConfig()->GetEventCfg(WXL_LOAD_TYPE_1);
		if (event_cfg == NULL)return;
		static ObjID obj_list[256] = { 0 };
		
		int count = m_scene->GetZoneMatrix()->GetObjByCircle(Posi(event_cfg->x, event_cfg->y), other_cfg->radius, obj_list, sizeof(obj_list) / sizeof(obj_list[0]));
		for (int i = 0; i < count; ++i)
		{
			Obj *obj = m_scene->GetObj(obj_list[i]);
			if (NULL != obj)
			{
				if (Obj::OBJ_TYPE_ROLE == obj->GetObjType())
				{
					//Role * role = (Role *)obj;
					role_enter_posi = 1;
					load_status = WXL_LOAD_TYPE_1;
					has_change = true;
					break;
				}
			}
		}
	}
	
	if(load_status < WXL_LOAD_TYPE_2 && role_enter_posi2 == 0)
	{
		const EventConfig * event_cfg = LOGIC_CONFIG->GetWenXiangLouFbConfig()->GetEventCfg(WXL_LOAD_TYPE_2);
		if (event_cfg == NULL)return;
		static ObjID obj_list[256] = { 0 };
		
		int count = m_scene->GetZoneMatrix()->GetObjByCircle(Posi(event_cfg->x, event_cfg->y), other_cfg->radius, obj_list, sizeof(obj_list) / sizeof(obj_list[0]));
		for (int i = 0; i < count; ++i)
		{
			Obj *obj = m_scene->GetObj(obj_list[i]);
			if (NULL != obj)
			{
				if (Obj::OBJ_TYPE_ROLE == obj->GetObjType())
				{
					//Role * role = (Role *)obj;
					role_enter_posi2 = 1;
					load_status = WXL_LOAD_TYPE_2;
					//更换状态后加入初始嫌疑值

					for (int role_index = 0; role_index < MAX_NPC_NUM_WXL; role_index++)
					{
						const RoleClueConfig *clue_cfg = LOGIC_CONFIG->GetWenXiangLouFbConfig()->GetRoleClueCfg(role_index);
						if (clue_cfg != NULL)
						{
							npc_sucspicion_list[role_index] += clue_cfg->suspicion;
						}
					}
					has_change = true;
					break;
				}
			}
		}
	}
	
	if (has_change)
	{
		this->SendSceneInfo();
		this->SendNPCInfo();
	}
}

void SpecialLogicWenXiangLouFb::SendNPCInfo(Role * role)
{
	Protocol::SCWenXiangLouNpcInfo msg;
	msg.load_status = load_status;
	for (int i = 0; i < MAX_NPC_NUM_WXL; i++)
	{
		msg.npc_identity_list[i] = npc_identity_list[i];
		msg.npc_sucspicion_list[i] = npc_sucspicion_list[i];
		msg.npc_talk_list[i] = npc_talk_list[i];
		msg.npc_check_list[i] = npc_check_list[i];
	}
	
	if(NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&msg, sizeof(msg));
	}
	else
	{
		this->GetScene()->SendToRole((const char*)&msg, sizeof(msg));
	}
}

void SpecialLogicWenXiangLouFb::SendSceneInfo(Role * role)
{
	Protocol::SCWenXiangLouReqSceneInfo msg;
	msg.wenxianglou_has_enter = wenxianglou_has_enter;
	msg.load_status = load_status;
	msg.role_enter_posi = role_enter_posi ? 1 : 0;
	msg.has_find_identity = has_find_identity ? 1 : 0;
	msg.has_finish_fb = has_finish_fb ? 1 : 0;
	msg.reward_count = 0;
	for (int i = 0; i < reward_count && i < MAX_SPECIAL_ITEM_NUM; i++)
	{
		msg.reward_list[i] = reward_list[i];
		msg.reward_count += 1;
	}
	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&msg, sizeof(msg));
	}
	else
	{
		this->GetScene()->SendToRole((const char*)&msg, sizeof(msg));
	}
}

void SpecialLogicWenXiangLouFb::OnFbFinish()
{
	int role_num = m_scene->RoleNum();
	for (int role_idx = 0; role_idx < role_num; ++role_idx)
	{
		Role * role = m_scene->GetRoleByIndex(role_idx);
		if (NULL != role)
		{
			EventHandler::Instance().OnRoleClearFB(role, FBManager::FB_TYPE_WEN_XIANG_LOU);
			//role->GetRoleModuleManager()->GetAnecdoteTask()->OnFinishTask(ANECDOTE_TASK_TYPE_BROTHEL_KILLER_EVENT);
		}
	}
	load_status = WXL_LOAD_TYPE_MAX;
// 	this->ClearAllNpcMonster();		// 清理所有明雷；
// 	this->DelayKickOutAllRole();	// 直接踢出副本；
}
