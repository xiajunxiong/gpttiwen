#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "npc/npcmanager.h"
#include "obj/character/npc_monster.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "scene/scene.h"
#include "engineadapter.h"
#include "obj/character/role.h"
#include "global/team/teammanager.hpp"
#include "global/team/team.hpp"
#include "item/knapsack.h"
#include "other/mentor/shitu.hpp"
#include "battle/battle_manager_local.hpp"

#include "protocol/msgshitu.h"
#include "other/mentor/shituconfig.hpp"
#include "speciallogicfshitufb.hpp"

SpecialLogicShiTuFb::SpecialLogicShiTuFb(Scene * scene) :SpecialLogic(scene)
{
	m_is_finish = false;
	m_create_role_level = 0;
	m_end_time = 0;
	m_master_uid = 0;
}

SpecialLogicShiTuFb::~SpecialLogicShiTuFb()
{
}

void SpecialLogicShiTuFb::OnRoleEnterScene(Role * role)
{
	this->SendSceneInfo(role);
}

void SpecialLogicShiTuFb::OnRoleLeaveScene(Role * role, bool is_logout)
{
}

void SpecialLogicShiTuFb::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (BATTLE_MODE_SHI_TU_FB == ack->battle_mode)
	{
		int npc_seq_id = ack->business_data.monster_gen_id;
		NPCMonster * npc_monster = NpcManager::Instance().GetNPCMonster(npc_seq_id);
		if (NULL != npc_monster && npc_monster->GetScene()->GetSceneID() == m_scene->GetSceneID() && npc_monster->GetScene()->GetSceneKey() == m_scene->GetSceneKey())
		{
			ObjID obj_id = npc_monster->GetId();
			for (int i = 0; i < (int)m_group_monster_info.monster_info.size(); ++i)
			{
				ShiTuFbMonsterInfoItem & curr = m_group_monster_info.monster_info[i];
				if (curr.obj_id == obj_id)
				{
					if (RESULT_TYPE_ATTACKER_WIN == ack->battle_result)
					{
						curr.is_kill = 1;
						m_group_monster_info.kill_num += 1;
						m_scene->DeleteObj(obj_id);
						this->SendSceneInfo();
					}

					Protocol::SCShiTuFbBattleResult result;
					result.result = ack->battle_result;
					result.group = curr.group;
					result.index = curr.index;
					m_scene->SendToRole((char *)&result, sizeof(result));

					break;
				}
			}
		}
	}
}

void SpecialLogicShiTuFb::OnRoleLeaveTeam(Role * role)
{
	this->DelayKickOutRole(role);
}

void SpecialLogicShiTuFb::Update(unsigned long interval, time_t now_second)
{
	this->CheckFinish(now_second);
}

bool SpecialLogicShiTuFb::CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader)
{
	if (NULL == member || NULL == leader) return false;

	if (leader->GetUID() == member->GetUID()) return false;

	// 如果开启副本的师傅要重新进来，可以进
	if (member->GetUID() == m_master_uid)
	{
		return true;
	}

	// 如果开启副本的师傅的徒弟要进来，可以进
	if (member->GetRoleModuleManager()->GetShiTu()->IsMaster(m_master_uid))
	{
		return true;
	}

	// 其他情况都不能进
	return false;
}

void SpecialLogicShiTuFb::SetInfo(int master_uid, int create_role_level, int team_index)
{
	const ShiTuOtherCfg & other_cfg = LOGIC_CONFIG->GetShiTuConfig()->GetOtherCfg();
	m_create_role_level = create_role_level;
	m_team_index = team_index;
	m_master_uid = master_uid;
	m_end_time = EngineAdapter::Instance().Time() + other_cfg.shi_tu_fb_limit_time;
	this->CreateMonster();
}

void SpecialLogicShiTuFb::SendSceneInfo(Role * role)
{
	Protocol::SCShiTuFbSceneInfo protocol;

	protocol.end_time = (unsigned int)m_end_time;
	protocol.count = (int)m_group_monster_info.monster_info.size();
	for (int i = 0; i < (int)m_group_monster_info.monster_info.size(); ++i)
	{
		protocol.monster_info[i] = m_group_monster_info.monster_info[i];
	}

	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&protocol, sizeof(protocol));
	}
	else
	{
		m_scene->SendToRole((const char *)&protocol, sizeof(protocol));
	}
}

void SpecialLogicShiTuFb::ReqFinish(Role * role)
{
	if (m_is_finish)
	{
		return;
	}

	Team * team = TeamManager::Instance().GetTeamByTeamIndex(m_team_index);;
	if (team == NULL)
	{
		return;
	}

	if (!team->IsLeader(role))
	{
		return;
	}

	m_end_time = EngineAdapter::Instance().Time();
}

void SpecialLogicShiTuFb::CreateMonster()
{
	const ShiTuOtherCfg & other_cfg = LOGIC_CONFIG->GetShiTuConfig()->GetOtherCfg();
	std::vector<ShiTuFbMonsterItemCfg> monster_list = LOGIC_CONFIG->GetShiTuConfig()->GetRandMonsterList(m_create_role_level, other_cfg.shi_tu_fb_create_monster_num);

	for (int i = 0; i < (int)monster_list.size(); ++i)
	{
		const ShiTuFbMonsterItemCfg & curr = monster_list[i];

		NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(curr.monster_id, m_scene, m_scene->GetARandPosi(), BATTLE_MODE_SHI_TU_FB);
		if (NULL != monster)
		{
			ShiTuFbMonsterInfoItem info;
			info.group = curr.group;
			info.index = curr.index;
			info.obj_id = monster->GetId();
			info.pos_x = monster->GetPos().x;
			info.pos_y = monster->GetPos().y;

			m_group_monster_info.monster_info.push_back(info);
		}
	}
}

void SpecialLogicShiTuFb::CheckFinish(time_t now)
{
	if (m_is_finish)
	{
		return;
	}

	if (now < m_end_time && m_group_monster_info.kill_num < (int)m_group_monster_info.monster_info.size())
	{
		return;
	}

	m_is_finish = true;

	std::set<int> finish_group;
	for (int i = 0; i < (int)m_group_monster_info.monster_info.size(); ++i)
	{
		if (m_group_monster_info.monster_info[i].is_kill == 0)
		{
			continue;
		}

		int group = m_group_monster_info.monster_info[i].group;
		finish_group.insert(group);

		for (int k = 0; k < (int)m_group_monster_info.monster_info.size(); ++k)
		{
			if (group == m_group_monster_info.monster_info[k].group && m_group_monster_info.monster_info[k].is_kill == 0)
			{
				finish_group.erase(group);
				break;
			}
		}
	}

	int finish_group_num = (int)finish_group.size();
	Protocol::SCShiTuFbFinishNotice protocol;
	protocol.finish_group = finish_group_num;
	m_scene->SendToRole((char *)&protocol, sizeof(protocol));

	const ShiTuFbRewardCfg * reward_cfg = LOGIC_CONFIG->GetShiTuConfig()->GetRewardCfg(finish_group_num);
	Team * team = TeamManager::Instance().GetTeamByTeamIndex(m_team_index);
	if (reward_cfg != NULL && team != NULL)
	{
		int mem_count = team->GetMemberCount();
		for (int i = 0; i < mem_count; ++i)
		{
			TeamMember * member = team->GetMemberInfoByIndex(i);
			if (!member->IsFollow())
			{
				continue;
			}

			Role * role = m_scene->GetRoleByUID(member->uid);
			if (role != NULL && role->GetRoleModuleManager()->GetShiTu()->GetShiTuRewardFlag() == 0)
			{
				bool ret = false;
				if (team->IsLeader(role))
				{
					if (!reward_cfg->master_reward.empty())
					{
						ret = role->GetRoleModuleManager()->GetKnapsack()->PutList2((short)reward_cfg->master_reward.size(), &reward_cfg->master_reward[0], PUT_REASON_SHI_TU_FB);
					}

					ret = role->GetRoleModuleManager()->GetMoney()->AddCoinBind(reward_cfg->master_coin, __FUNCTION__) || ret;
					ret = role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(reward_cfg->master_xia_yi_zhi, MONEY_TYPE_CHIVALROUS, __FUNCTION__) || ret;
				}
				else
				{
					if (!reward_cfg->tu_di_reward.empty())
					{
						ret = role->GetRoleModuleManager()->GetKnapsack()->PutList2((short)reward_cfg->tu_di_reward.size(), &reward_cfg->tu_di_reward[0], PUT_REASON_SHI_TU_FB);
					}
					ret = role->GetRoleModuleManager()->GetMoney()->AddCoinBind(reward_cfg->tu_id_coin, __FUNCTION__) || ret;
				}

				if (ret)
				{
					role->GetRoleModuleManager()->GetShiTu()->SetShiTuRewardFlag(true);
				}
			}
		}
	}

	if (team != NULL)
	{
		BattleManagerLocal::GetInstance().ForceFinishFightReq2(team->GetLeaderRole());
	}
	
	this->DelayKickOutAllRole();
}


