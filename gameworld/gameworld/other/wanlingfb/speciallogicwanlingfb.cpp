#include "speciallogicwanlingfb.hpp"
#include "wanlingfb.hpp"
#include "wanlingfbconfig.hpp"

#include "world.h"
#include "protocol/msgwanlingfb.h"

#include "npc/npcmanager.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "obj/character/npc_monster.h"
#include "config/logicconfigmanager.hpp"
#include "obj/otherobj/specialtransport.h"
#include "scene/fbmanager.hpp"

#include "global/team/teammatch.hpp"

SpecialLogicWanLingFB::SpecialLogicWanLingFB(Scene * scene)
	:SpecialLogic(scene)
{
	m_fb_index = 0;
	m_is_finish = 0;
	m_task_seq = 0;

	m_can_enter_team = 1;
}

void SpecialLogicWanLingFB::OnRoleEnterScene(Role * role)
{
	if (NULL == role)
	{
		return;
	}

	this->SendSceneInfo(role);

	
}

void SpecialLogicWanLingFB::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (this->GetMineMonsterBattleMode() != ack->battle_mode
		|| ack->special_business_data.data_length < (int)sizeof(BattleFBData)
		|| BATTLE_BUSINESS_DATA_TYPE_FB != ack->special_business_data.data_type)
	{
		return;
	}

	if (RESULT_TYPE_ATTACKER_WIN != ack->battle_result)
	{
		return;
	}

	if (0 != m_is_finish)
	{
		return;
	}

	const WanLingFB::TaskCfg* task_cfg
		= LOGIC_CONFIG->GetWanLingFBConfig()->GetTaskCfg(m_fb_index, m_task_seq);
	if (NULL == task_cfg)
	{
		return;
	}

	switch (task_cfg->task_type)
	{
	case WanLingFB::TASK_TYPE_BATTLE:
		{
			if(ack->monster_group_id != task_cfg->npc_action)
			{
				return;
			}
		}
		break;
	default:
		return;
	}
	
	//奖励结算
	for (int index = 0
		; index < ack->attacker_list.role_count
		&& index < SIDE_MAX_ROLE_NUM
		; ++index)
	{
		Role * role = this->GetScene()->GetRoleByUID(ack->attacker_list.role_list[index].uid);
		if (NULL == role)
		{
			continue;
		}
		role->GetRoleModuleManager()->GetRoleWanLingFB()->FBBattlePass(m_fb_index);
		m_join_set.insert(role->GetUID());
	}

	//任务完成
	//任务seq已改变
	this->OnTaskFinish(false);
}

void SpecialLogicWanLingFB::OnTransport(Role * role, SpecialTransport * transport_obj)
{
	if (NULL == role || NULL == transport_obj)
	{
		return;
	}

	if (role->IsLeader())
	{
		FBManager::Instance().LeaveFB(role);
	}
}

bool SpecialLogicWanLingFB::CanEnterTeam(Role * role, Team * team, bool is_from_create)
{
	return 1 == m_can_enter_team;
}

void SpecialLogicWanLingFB::SetFBInfo(int fb_index)
{
	m_fb_index = fb_index;
	m_task_seq = 1;
	m_is_finish = 0;

	this->CreateNPC();
}

void SpecialLogicWanLingFB::SendSceneInfo(Role * role)
{
	Protocol::SCWanLingFBInfo info;

	info.m_fb_index = m_fb_index;
	info.m_is_finish = m_is_finish;
	info.m_task_seq = m_task_seq;

	if (NULL == role)
	{
		m_scene->SendToRole((const char*)&info, sizeof(info));
	}
	else
	{
		role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	}
}

bool SpecialLogicWanLingFB::OnTaskFinish(bool from_msg)
{
	bool ret = false;
	if (0 != m_is_finish)
	{
		return ret;
	}

	const WanLingFB::FB_Cfg* fb_cfg
		= LOGIC_CONFIG->GetWanLingFBConfig()->GetFBCfg(m_fb_index);
	if (NULL == fb_cfg)
	{
		return ret;
	}

	if (from_msg)
	{
		const WanLingFB::TaskCfg* task_cfg
			= LOGIC_CONFIG->GetWanLingFBConfig()->GetTaskCfg(m_fb_index, m_task_seq);
		if (NULL == task_cfg)
		{
			return ret;
		}

		switch (task_cfg->task_type)
		{
		case WanLingFB::TASK_TYPE_TALK:
			{

			}
			break;
		default:
			return ret;
		}
	}


	m_task_seq++;
	if ((int)fb_cfg->task_vec.size() < m_task_seq)
	{
		m_is_finish = 1;
		ret = true;
		for (std::set<int>::iterator join_it = m_join_set.begin()
			; join_it != m_join_set.end()
			; join_it++)
		{
			Role * role = this->GetScene()->GetRoleByUID(*join_it);
			if (NULL == role)
			{
				continue;
			}

			role->GetRoleModuleManager()->GetRoleWanLingFB()->FBPass(m_fb_index);
		}

		this->CreatLeaveTransport();

	}

	this->SendSceneInfo(NULL);
	return ret;
}

void SpecialLogicWanLingFB::FightStart(Role *role, int monster_group_id)
{//最后一场战斗开始后 禁止进入队伍
	if (NULL == role)
	{
		return;
	}

	const WanLingFB::FB_Cfg* fb_cfg
		= LOGIC_CONFIG->GetWanLingFBConfig()->GetFBCfg(m_fb_index);
	if (NULL == fb_cfg)
	{
		return;
	}

	if (fb_cfg->final_fight_task_seq > m_task_seq)
	{
		return;
	}

	const WanLingFB::TaskCfg* task_cfg
		= LOGIC_CONFIG->GetWanLingFBConfig()->GetTaskCfg(m_fb_index, m_task_seq);
	if (NULL == task_cfg)
	{
		return;
	}

	switch (task_cfg->task_type)
	{
	case WanLingFB::TASK_TYPE_BATTLE:
		{
			if (monster_group_id != task_cfg->npc_action)
			{
				return;
			}

			m_can_enter_team = 0;
			//关闭匹配
			Team * team = role->GetMyTeam();
			TeamMatch::Instance().OnTeamStopMatch(team);
		}
		break;
	default:
		return;
	}

}

void SpecialLogicWanLingFB::CreateNPC()
{
	return;
	const WanLingFB::FB_Cfg* fb_cfg
		= LOGIC_CONFIG->GetWanLingFBConfig()->GetFBCfg(m_fb_index);
	if (NULL == fb_cfg)
	{
		return;
	}
	int npc_num = fb_cfg->creat_npc_vec.size();
	for (int i = 0; i < npc_num ;i++)
	{
		const WanLingFB::CreatNPCCfg & npc_cfg = fb_cfg->creat_npc_vec[i];

		NpcManager::Instance().CreateNPCChosed(
			npc_cfg.npc_id, 
			m_scene, 
			npc_cfg.npc_pos, 
			this->GetMineMonsterBattleMode(), 
			&npc_cfg.npc_dir
		);
	}
}

void SpecialLogicWanLingFB::CreatLeaveTransport()
{
	const WanLingFB::FB_Cfg* fb_cfg
		= LOGIC_CONFIG->GetWanLingFBConfig()->GetFBCfg(m_fb_index);
	if (NULL == fb_cfg)
	{
		return;
	}

	SpecialTransport * transport_obj = new SpecialTransport();
	if (transport_obj == NULL)
	{
		return;
	}
	

	transport_obj->SetPos(fb_cfg->leave_pos);
	transport_obj->GetExtraInfo().param1 = 0;
	transport_obj->GetExtraInfo().send_param1 = 0;
	transport_obj->GetExtraInfo().send_param2 = 0;
	m_scene->AddObj(transport_obj);
}


