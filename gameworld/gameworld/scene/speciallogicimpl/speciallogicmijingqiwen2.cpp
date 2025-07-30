#include "speciallogicmijingqiwen2.hpp"
#include "obj/character/role.h"
#include "global/team/team.hpp"
#include "obj/obj.h"
#include "obj/character/npc_chosed.h"
#include "npc/npcmanager.h"
#include "servercommon/errornum.h"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "other/wst_question/wstquestionconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "gameworld/protocol/msgother.h"
#include "protocol/msgcrystalfb.h"
#include "npc/npcmanager.h"
#include "obj/character/npc_monster.h"
#include "other/rolemodulemanager.hpp"
#include "other/prestige/prestige.hpp"
#include "drop/droppool.hpp"
#include "obj/otherobj/treasurechest.h"
#include "battle/battle_manager_local.hpp"
#include "gamelog.h"
#include "obj/character/npc_gather.h"
#include "obj/otherobj/specialtransport.h"
#include "world.h"
#include "other/pet/pet.hpp"
#include "other/partner/partner.hpp"
#include "servercommon/string/gameworldstr.h"
#include "gameworld/other/buffmanager/buffmanager.hpp"
#include "other/buffmanager/buffbase.hpp"
#include "servercommon/noticenum.h"
#include "global/team/teammanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "servercommon/errornum.h"
#include "other/roleactivity/roleactivity.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"

SpecialLogicMiJingQiWenFB2::SpecialLogicMiJingQiWenFB2(Scene * scene) : SpecialLogic(scene), m_is_init(false), m_layer(false), m_level_seq(0), m_monster_num(0),
m_box_num(0), m_npc_num(0), m_talk_npc_num(0), m_boss_obj_id(INVALID_OBJ_ID), m_gouhuo_num(0), m_coin_num(0), m_gather_num(0), m_monster_num_max(0),
m_box_num_max(0), m_npc_num_max(0), m_gouhuo_num_max(0), m_coin_num_max(0), m_gather_num_max(0)
{
	m_transport_pos = Posi();
	m_already_rand_scene_id.insert(m_scene->GetSceneID());
}

SpecialLogicMiJingQiWenFB2::~SpecialLogicMiJingQiWenFB2()
{
}

void SpecialLogicMiJingQiWenFB2::Update(unsigned long interval, time_t now_second)
{
	for (std::map<int, time_t>::iterator it = kick_out_tamp.begin();it != kick_out_tamp.end(); it++)
	{
		if (it->second > 0 && it->second < now_second)
		{
			Role * role = m_scene->GetRoleByUID(it->first);
			if (role != NULL)this->DelayKickOutRole(role);
			kick_out_tamp.erase(it);
			break;
		}
	}
}

void SpecialLogicMiJingQiWenFB2::OnRoleEnterScene(Role *role)
{
	if (NULL == role) return;

	/*if (!m_is_init)
	{
	m_is_init = true;
	this->CreateTransport();
	this->CreateMonster();
	this->CreateNpc();
	this->CreateBox();

	m_monster_num_max = m_monster_num;
	m_box_num_max = m_box_num;
	m_npc_num_max = m_npc_num;
	m_gouhuo_num_max = m_gouhuo_num;
	m_coin_num_max = m_coin_num;
	m_gather_num_max = m_gather_num;
	}*/

	role->SetEncounterMonsterFlag(true);

	this->SendSceneInfo(role);
}

void SpecialLogicMiJingQiWenFB2::OnRoleLeaveScene(Role *role, bool is_logout)
{
	role->RestoreLastEncounterMonsterFlag();
}

void SpecialLogicMiJingQiWenFB2::OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack)
{
	if (BATTLE_MODE_RA_MIJINGQIWEN_2 == ack->battle_mode)
	{
		const RandActivityMiJingQiWenConfig2 * config = static_cast<const RandActivityMiJingQiWenConfig2 *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANECDOTE_2));
		if (NULL == config)
		{
			return;
		}
		if (BATTLE_MODE_RA_MIJINGQIWEN_2 == ack->battle_mode && RESULT_TYPE_ATTACKER_WIN == ack->battle_result)
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

				const RoleCrystalFbLevelCfg  * cfg = config->GetRoleCrystalFbLevelCfg(m_level_seq);
				if (NULL == cfg)return;

				int monster_type = Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_MONSTER;
				bool is_boss = npc_monster->GetId() == m_boss_obj_id;

				if (!is_boss)
				{
					const MiJingQiWenFbNPCRewardCfg2 * npc_cfg = config->GetNpcRewardCfg(npc_monster->GetNpcId(), ack->business_data.param1);
					if (NULL != npc_cfg)
					{
						if (npc_cfg->is_delete == 1)
						{
							std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.find(Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_MONSTER);
							if (it != m_obj_info_list.end())
							{
								for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
								{
									if (it2->obj_id == npc->GetId())
									{
										it->second.erase(it2);
										break;
									}
								}
							}
							m_npc_num -= 1;
						}
					}
					std::vector<int> buff_id_list = config->GetMonsterDropId(npc_monster->GetNpcId(), 0);//赢了填0
					for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
					{
						Role * temp_role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
						if (NULL == temp_role)
						{
							continue;
						}

						if ((int)buff_id_list.size() > 0)
						{
							for (int i = 0; i < (int)buff_id_list.size(); i++)
							{
								const BuffListConfig*  buff_cfg = config->GetBuffId(buff_id_list[i]);
								if (buff_cfg != NULL)
								{
									this->AddRoleBuff(temp_role, buff_cfg);
								}
							}
						}
					}
				}
				else
				{
					std::vector<int> drop_id_list = config->GetMonsterDropId(npc_monster->GetNpcId(), 0);
					for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
					{
						Role * temp_role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
						if (NULL == temp_role)
						{
							continue;
						}
						DropPool::Instance()->DropListToRole(temp_role, drop_id_list, ack->monster_group_id, true, npc_monster->GetId() == m_boss_obj_id ?
							GET_REASON_MIJIQIWEN : GET_REASON_INVALID, 1, NULL, 0, npc_monster->GetNpcId(), 0, RAND_ACTIVITY_TYPE_ANECDOTE_2);
						time_t & node = kick_out_tamp[temp_role->GetUID()];
						node = EngineAdapter::Instance().Time() + 10;
					}
					m_boss_obj_id = INVALID_OBJ_ID;
					monster_type = Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_BOSS;
					this->CreateLeaveTransport(npc_monster->GetPos());
				}

				std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.find(monster_type);
				if (it != m_obj_info_list.end())
				{
					for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
					{
						if (it2->obj_id == npc->GetId())
						{
							it->second.erase(it2);
							break;
						}
					}
				}

				m_scene->DeleteObj(npc->GetId());
				if (monster_type != Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_BOSS)
				{
					m_monster_num -= 1;
				}
				this->SendSceneObjInfo();
				this->SendSceneInfo();
			}
			else if (NULL != npc && Obj::OBJ_TYPE_NPC_CHOSED == npc->GetObjType())
			{
				NPCChosed * npc_chose = (NPCChosed*)npc;
				if (npc->GetScene()->GetSceneID() != m_scene->GetSceneID() ||
					npc->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
				{
					return;
				}

				int monster_type = Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_NPC;

				std::vector<int> buff_id_list = config->GetMonsterDropId(npc_chose->GetNpcId(), 0);//赢了填0
				for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
				{
					Role * temp_role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
					if (NULL == temp_role)
					{
						continue;
					}

					if ((int)buff_id_list.size() > 0)
					{
						for (int i = 0; i < (int)buff_id_list.size(); i++)
						{
							const BuffListConfig*  buff_cfg = config->GetBuffId(buff_id_list[i]);
							if (buff_cfg != NULL)
							{
								this->AddRoleBuff(temp_role, buff_cfg);
							}
						}
					}
				}

				std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.find(monster_type);
				if (it != m_obj_info_list.end())
				{
					for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
					{
						if (it2->obj_id == npc->GetId())
						{
							it->second.erase(it2);
							break;
						}
					}
				}

				m_scene->DeleteObj(npc->GetId());
				if (monster_type != Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_BOSS)
				{
					m_npc_num -= 1;
				}
				this->SendSceneObjInfo();
				this->SendSceneInfo();
			}
		}
		else if (BATTLE_MODE_RA_MIJINGQIWEN_2 == ack->battle_mode && RESULT_TYPE_DEFENDER_WIN == ack->battle_result)
		{
			//怪物消失，并且赋予玩家负面buff
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

				int monster_type = Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_MONSTER;
				bool is_boss = npc_monster->GetId() == m_boss_obj_id;

				if (!is_boss)
				{
					std::vector<int> buff_id_list = config->GetMonsterDropId(npc_monster->GetNpcId(), 1);//输了填1
					for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
					{
						Role * temp_role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
						if (NULL == temp_role)
						{
							continue;
						}

						if ((int)buff_id_list.size() > 0)
						{
							for (int i = 0; i < (int)buff_id_list.size(); i++)
							{
								const BuffListConfig*  buff_cfg = config->GetBuffId(buff_id_list[i]);
								if (buff_cfg != NULL)
								{
									this->AddRoleBuff(temp_role, buff_cfg);
								}
							}
						}
					}
				}
				else
				{
					std::vector<int> drop_id_list = config->GetMonsterDropId(npc_monster->GetNpcId(), 1);
					for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
					{
						Role * temp_role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
						if (NULL == temp_role)
						{
							continue;
						}
						DropPool::Instance()->DropListToRole(temp_role, drop_id_list, ack->monster_group_id, true, npc_monster->GetId() == m_boss_obj_id ?
							GET_REASON_MIJIQIWEN : GET_REASON_INVALID, 1, NULL, 0, npc_monster->GetNpcId(), 0, RAND_ACTIVITY_TYPE_ANECDOTE_2, false);
						time_t & node = kick_out_tamp[temp_role->GetUID()];
						node = EngineAdapter::Instance().Time() + 10;
					}
					m_boss_obj_id = INVALID_OBJ_ID;
					monster_type = Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_BOSS;
					this->CreateLeaveTransport(npc_monster->GetPos());
				}

				std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.find(monster_type);
				if (it != m_obj_info_list.end())
				{
					for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
					{
						if (it2->obj_id == npc->GetId())
						{
							it->second.erase(it2);
							break;
						}
					}
				}

				m_scene->DeleteObj(npc->GetId());
				if (monster_type != Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_BOSS)
				{
					m_monster_num -= 1;
				}
				this->SendSceneObjInfo();
				this->SendSceneInfo();
			}
			else if (NULL != npc && Obj::OBJ_TYPE_NPC_CHOSED == npc->GetObjType())
			{
				NPCChosed * npc_chose = (NPCChosed*)npc;
				if (npc->GetScene()->GetSceneID() != m_scene->GetSceneID() ||
					npc->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
				{
					return;
				}

				std::vector<int> buff_id_list = config->GetMonsterDropId(npc_chose->GetNpcId(), 1);//输了填1
				for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
				{
					Role * temp_role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
					if (NULL == temp_role)
					{
						continue;
					}

					if ((int)buff_id_list.size() > 0)
					{
						for (int i = 0; i < (int)buff_id_list.size(); i++)
						{
							const BuffListConfig*  buff_cfg = config->GetBuffId(buff_id_list[i]);
							if (buff_cfg != NULL)
							{
								this->AddRoleBuff(temp_role, buff_cfg);
							}
						}
					}
				}

				int monster_type = Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_NPC;

				const MiJingQiWenFbNPCRewardCfg2 * npc_cfg = config->GetNpcRewardCfg(npc_chose->GetNpcId(), ack->business_data.param3);
				if (NULL != npc_cfg)
				{
					if (npc_cfg->is_delete == 1)
					{
						std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.find(Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_NPC);
						if (it != m_obj_info_list.end())
						{
							for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
							{
								if (it2->obj_id == npc->GetId())
								{
									it->second.erase(it2);
									break;
								}
							}
						}
						m_npc_num -= 1;
					}
				}

				std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.find(monster_type);
				if (it != m_obj_info_list.end())
				{
					for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
					{
						if (it2->obj_id == npc->GetId())
						{
							it->second.erase(it2);
							break;
						}
					}
				}
				m_scene->DeleteObj(npc->GetId());
				if (monster_type != Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_BOSS)
				{
					m_npc_num -= 1;
				}
				this->SendSceneObjInfo();
				this->SendSceneInfo();
			}
		}
	}
}

void SpecialLogicMiJingQiWenFB2::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack)
{
	if (BATTLE_MODE_RA_MIJINGQIWEN_2 == ack->battle_mode)
	{
		const RandActivityMiJingQiWenConfig2 * config = static_cast<const RandActivityMiJingQiWenConfig2 *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANECDOTE_2));
		if (NULL == config)
		{
			return;
		}
		if (BATTLE_MODE_RA_MIJINGQIWEN_2 == ack->battle_mode)
		{
			//怪物消失，并且赋予玩家负面buff
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

				int monster_type = Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_MONSTER;
				bool is_boss = npc_monster->GetId() == m_boss_obj_id;
				std::vector<int> buff_id_list = config->GetMonsterDropId(npc_monster->GetNpcId(), 1);//输了填1
				if (!is_boss)
				{
					Role * temp_role = m_scene->GetRoleByUID(ack->uid);
					if (NULL != temp_role)
					{
						if ((int)buff_id_list.size() > 0)
						{
							for (int i = 0; i < (int)buff_id_list.size(); i++)
							{
								const BuffListConfig*  buff_cfg = config->GetBuffId(buff_id_list[i]);
								if (buff_cfg != NULL)
								{
									this->AddRoleBuff(temp_role, buff_cfg);
								}
							}
						}
					}
				}
				else
				{
					Role * temp_role = m_scene->GetRoleByUID(ack->uid);
					if (NULL != temp_role)
					{
						std::vector<int> drop_id_list = config->GetMonsterDropId(npc_monster->GetNpcId(), 1);
						DropPool::Instance()->DropListToRole(temp_role, drop_id_list, ack->business_data.monster_group_id, true, npc_monster->GetId() == m_boss_obj_id ?
							GET_REASON_MIJIQIWEN : GET_REASON_INVALID, 1, NULL, 0, npc_monster->GetNpcId(), 0, RAND_ACTIVITY_TYPE_ANECDOTE_2, false);
						m_boss_obj_id = INVALID_OBJ_ID;
						monster_type = Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_BOSS;
						this->CreateLeaveTransport(npc_monster->GetPos());
						time_t & node = kick_out_tamp[temp_role->GetUID()];
						node = EngineAdapter::Instance().Time() + 10;
					}
				}

				this->SendSceneObjInfo();
				this->SendSceneInfo();
			}
			else if (NULL != npc && Obj::OBJ_TYPE_NPC_CHOSED == npc->GetObjType())
			{
				NPCChosed * npc_chose = (NPCChosed*)npc;
				if (npc->GetScene()->GetSceneID() != m_scene->GetSceneID() ||
					npc->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
				{
					return;
				}

				std::vector<int> buff_id_list = config->GetMonsterDropId(npc_chose->GetNpcId(), 1);//输了填1
				Role * temp_role = m_scene->GetRoleByUID(ack->uid);
				if (NULL != temp_role)
				{
					if ((int)buff_id_list.size() > 0)
					{
						for (int i = 0; i < (int)buff_id_list.size(); i++)
						{
							const BuffListConfig*  buff_cfg = config->GetBuffId(buff_id_list[i]);
							if (buff_cfg != NULL)
							{
								this->AddRoleBuff(temp_role, buff_cfg);
							}
						}
					}
				}
				this->SendSceneObjInfo();
				this->SendSceneInfo();
			}
		}
	}
}

void SpecialLogicMiJingQiWenFB2::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack)
{
	if (BATTLE_MODE_RA_MIJINGQIWEN_2 == ack->battle_mode)
	{
		const RandActivityMiJingQiWenConfig2 * config = static_cast<const RandActivityMiJingQiWenConfig2 *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANECDOTE_2));
		if (NULL == config)
		{
			return;
		}
		if (BATTLE_MODE_RA_MIJINGQIWEN_2 == ack->battle_mode)
		{
			//怪物消失，并且赋予玩家负面buff
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

				int monster_type = Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_MONSTER;
				bool is_boss = npc_monster->GetId() == m_boss_obj_id;

				if (!is_boss)
				{
					std::vector<int> buff_id_list = config->GetMonsterDropId(npc_monster->GetNpcId(), 1);//输了填1
					for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
					{
						Role * temp_role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
						if (NULL == temp_role)
						{
							continue;
						}

						if ((int)buff_id_list.size() > 0)
						{
							for (int i = 0; i < (int)buff_id_list.size(); i++)
							{
								const BuffListConfig*  buff_cfg = config->GetBuffId(buff_id_list[i]);
								if (buff_cfg != NULL)
								{
									this->AddRoleBuff(temp_role, buff_cfg);
								}
							}
						}
					}
				}
				else
				{
					std::vector<int> drop_id_list = config->GetMonsterDropId(npc_monster->GetNpcId(), 1);
					for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
					{
						Role * temp_role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
						if (NULL == temp_role)
						{
							continue;
						}
						DropPool::Instance()->DropListToRole(temp_role, drop_id_list, ack->monster_group_id, true, npc_monster->GetId() == m_boss_obj_id ?
							GET_REASON_MIJIQIWEN : GET_REASON_INVALID, 1, NULL, 0, npc_monster->GetNpcId(), 0, RAND_ACTIVITY_TYPE_ANECDOTE_2, false);
						time_t & node = kick_out_tamp[temp_role->GetUID()];
						node = EngineAdapter::Instance().Time() + 10;
					}
					m_boss_obj_id = INVALID_OBJ_ID;
					monster_type = Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_BOSS;
					this->CreateLeaveTransport(npc_monster->GetPos());
				}

				std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.find(monster_type);
				if (it != m_obj_info_list.end())
				{
					for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
					{
						if (it2->obj_id == npc->GetId())
						{
							it->second.erase(it2);
							break;
						}
					}
				}

				m_scene->DeleteObj(npc->GetId());
				if (monster_type != Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_BOSS)
				{
					m_monster_num -= 1;
				}
				this->SendSceneObjInfo();
				this->SendSceneInfo();
			}
			else if (NULL != npc && Obj::OBJ_TYPE_NPC_CHOSED == npc->GetObjType())
			{
				NPCChosed * npc_chose = (NPCChosed*)npc;
				if (npc->GetScene()->GetSceneID() != m_scene->GetSceneID() ||
					npc->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
				{
					return;
				}

				std::vector<int> buff_id_list = config->GetMonsterDropId(npc_chose->GetNpcId(), 1);//输了填1
				for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
				{
					Role * temp_role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
					if (NULL == temp_role)
					{
						continue;
					}

					if ((int)buff_id_list.size() > 0)
					{
						for (int i = 0; i < (int)buff_id_list.size(); i++)
						{
							const BuffListConfig*  buff_cfg = config->GetBuffId(buff_id_list[i]);
							if (buff_cfg != NULL)
							{
								this->AddRoleBuff(temp_role, buff_cfg);
							}
						}
					}
				}

				int monster_type = Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_NPC;

				const MiJingQiWenFbNPCRewardCfg2 * npc_cfg = config->GetNpcRewardCfg(npc_chose->GetNpcId(), ack->business_data.param3);
				if (NULL != npc_cfg)
				{
					if (npc_cfg->is_delete == 1)
					{
						std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.find(Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_NPC);
						if (it != m_obj_info_list.end())
						{
							for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
							{
								if (it2->obj_id == npc->GetId())
								{
									it->second.erase(it2);
									break;
								}
							}
						}
						m_npc_num -= 1;
					}
				}

				std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.find(monster_type);
				if (it != m_obj_info_list.end())
				{
					for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
					{
						if (it2->obj_id == npc->GetId())
						{
							it->second.erase(it2);
							break;
						}
					}
				}
				m_scene->DeleteObj(npc->GetId());
				if (monster_type != Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_BOSS)
				{
					m_npc_num -= 1;
				}
				this->SendSceneObjInfo();
				this->SendSceneInfo();
			}
		}
	}
}

bool SpecialLogicMiJingQiWenFB2::IsForceEncounterMineMonster()
{
	return !this->IsLastLayer(m_layer);
}

bool SpecialLogicMiJingQiWenFB2::CanEncounterMineMonster(Role * role)
{
	return !this->IsLastLayer(m_layer);
}

bool SpecialLogicMiJingQiWenFB2::CanUseItem(Role * role, ItemID item_id, int num)
{
	const ItemBase *item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base)
	{
		return false;
	}

	return true;
}

void SpecialLogicMiJingQiWenFB2::OnUseItem(Role * role, ItemID item_id, int num)
{
	const ItemBase *item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base)
	{
		return;
	}
}

void SpecialLogicMiJingQiWenFB2::OnTransport(Role * role, SpecialTransport * transport_obj)
{
	if (NULL == role || NULL == transport_obj)
	{
		return;
	}

	if (!this->IsLastLayer(m_layer))
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

		this->DelayKickOutAllRole();
	}
}

void SpecialLogicMiJingQiWenFB2::OnRoleLeaveTeam(Role * role)
{
	this->DelayKickOutRole(role);
}

void SpecialLogicMiJingQiWenFB2::OnNPCChosedReward(Role * role, ObjID npc_objid, int option_index)
{
	const RandActivityMiJingQiWenConfig2 * config = static_cast<const RandActivityMiJingQiWenConfig2 *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANECDOTE_2));
	if (NULL == config)
	{
		return;
	}
	Obj * npc_obj = m_scene->GetObj(npc_objid);
	if (npc_obj != NULL && Obj::OBJ_TYPE_NPC_CHOSED == npc_obj->GetObjType())
	{
		NPCChosed* npc_chose = (NPCChosed*)npc_obj;

		const MiJingQiWenFbNPCRewardCfg2 * npc_cfg = config->GetNpcRewardCfg(npc_chose->GetNpcId(), option_index);
		if (NULL != npc_cfg)
		{
			this->RoleTeamGetRewardByCfg(role, npc_cfg, &npc_objid);

			if (npc_cfg->is_delete == 1)
			{
				bool is_find = false;
				int npc_type = 0;
				for (std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.begin(); it != m_obj_info_list.end() && !is_find; it++)
				{

					for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end() && !is_find; it2++)
					{
						if (it2->obj_id == npc_objid)
						{
							it->second.erase(it2);
							npc_type = it->first;
							is_find = true;
							break;
						}
					}

				}
				if (is_find)
				{
					m_scene->DeleteObj(npc_objid);
					if (npc_type == new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_2)
					{
						m_npc_num -= 1;
					}
					else if (npc_type == new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_4)
					{
						m_gouhuo_num -= 1;
					}
					this->SendSceneObjInfo();
				}

			}
			this->SendSceneInfo();
		}
	}
}

void SpecialLogicMiJingQiWenFB2::OnGather(Role * role, int gather_id, int seq_id, const Posi & pos)
{
	NPC *npc = NULL;
	if (seq_id < NpcManager::MIN_NPC_SEQ)		// 场景采集物
	{
		return;
	}
	else		// 服务器采集物
	{
		npc = NpcManager::Instance().GetServerNPC(seq_id);
	}
	if (NULL == npc) return;
	const RandActivityMiJingQiWenConfig2 * config = static_cast<const RandActivityMiJingQiWenConfig2 *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANECDOTE_2));
	if (NULL == config)
	{
		return;
	}
	const GatherNpcConfig *  gather_cfg = config->GetGatherNpcCfg(gather_id);
	if (gather_cfg != NULL)
	{
		Team *role_team = role->GetMyTeam();
		if (NULL != role_team)
		{
			int member_count = role_team->GetMemberCount();
			for (int i = 0; i < member_count && i < MAX_TEAM_MEMBER_NUM; i++)
			{
				Role * member_role = role_team->GetMemberRoleByIndex(i);
				if (member_role != NULL && member_role->IsFollow())
				{
					int index = 0;
					if (index < 0 || index >= MAX_ROLE_CRYSTAL_FB_REWARD_COLUMN)continue;
					switch (gather_cfg->reward_type)
					{
					case GatherNpcConfig::REWARD_TYPE_0:
						{
							const BuffListConfig*  buff_cfg = config->GetBuffId(gather_cfg->reward_list[index]);
							if (buff_cfg != NULL)
							{
								this->AddRoleBuff(role, buff_cfg);
							}
						}
						break;
					case GatherNpcConfig::REWARD_TYPE_1:
						{
							const ItemConfigData * reward_cfg = config->GetRandNpcReward(gather_cfg->reward_list[index]);
							if (reward_cfg != NULL)
							{
								ItemConfigData put_reward(*reward_cfg);
								if (!role->OnGiveGatherReward(*reward_cfg, true, &put_reward.num))
								{
									gamelog::g_log_knapsack.printf(LL_INFO, "SpecialLogicMiJingQiWenFB2::OnGather role[%s,%d] knapsack full no put item[id:%d,num:%d,is_bind:%d]",
										member_role->GetName(), member_role->GetUID(), reward_cfg->item_id, reward_cfg->num, reward_cfg->is_bind ? 1 : 0);
								}
								else
								{
									RoleActivity * role_activity = member_role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ANECDOTE_2);
									if (NULL != role_activity)
									{
										std::vector<ItemConfigData> item_list; item_list.push_back(put_reward);
										role_activity->OnSendResultNotice(Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_GET_REWARD, &item_list);
									}
								}
							}
						}
						break;
					case GatherNpcConfig::REWARD_TYPE_2:
						{
							int addordown = rand() % 2;
							int real_coin = gather_cfg->reward_list[index];
							UNSTD_STATIC_CHECK(Money::RAND_MONEY_MAX > 0);
							if (addordown == 1)
							{
								real_coin += rand() % (Money::RAND_MONEY_MAX + 1);
							}
							else
							{
								real_coin -= rand() % (Money::RAND_MONEY_MAX + 1);
							}

							if (!member_role->GetRoleModuleManager()->GetMoney()->AddCoinBind(real_coin, "SpecialLogicMiJingQiWenFB2::OnGather"))
							{
								return;
							}
							else
							{
								RoleActivity * role_activity = member_role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ANECDOTE_2);
								if (NULL != role_activity)
								{
									std::vector<RAMoneyInfo> money_list; money_list.push_back(RAMoneyInfo(MONEY_TYPE_COIN_BIND, real_coin));
									role_activity->OnSendResultNotice(Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_GET_REWARD, NULL, &money_list);
								}
							}
						}
						break;
					default:
						break;
					}
				}
			}

		}
		else
		{
			int index = 0;
			if (index < 0 || index >= MAX_ROLE_CRYSTAL_FB_REWARD_COLUMN)return;
			switch (gather_cfg->reward_type)
			{
			case GatherNpcConfig::REWARD_TYPE_0:
				{
					const BuffListConfig*  buff_cfg = config->GetBuffId(gather_cfg->reward_list[index]);
					if (buff_cfg != NULL)
					{
						this->AddRoleBuff(role, buff_cfg);
					}
				}
				break;
			case GatherNpcConfig::REWARD_TYPE_1:
				{
					const ItemConfigData * reward_cfg = config->GetRandNpcReward(gather_cfg->reward_list[index]);
					if (reward_cfg != NULL)
					{
						ItemConfigData put_reward(*reward_cfg);
						if (!role->OnGiveGatherReward(*reward_cfg, true, &put_reward.num))
						{
							gamelog::g_log_knapsack.printf(LL_INFO, "SpecialLogicMiJingQiWenFB2::OnGather role[%s,%d] knapsack full no put item[id:%d,num:%d,is_bind:%d]",
								role->GetName(), role->GetUID(), reward_cfg->item_id, reward_cfg->num, reward_cfg->is_bind ? 1 : 0);
						}
						else
						{
							RoleActivity * role_activity = role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ANECDOTE_2);
							if (NULL != role_activity)
							{
								std::vector<ItemConfigData> item_list; item_list.push_back(put_reward);
								role_activity->OnSendResultNotice(Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_GET_REWARD, &item_list);
							}
						}
					}
				}
				break;
			case GatherNpcConfig::REWARD_TYPE_2:
				{
					int addordown = rand() % 2;
					int real_coin = gather_cfg->reward_list[index];
					UNSTD_STATIC_CHECK(Money::RAND_MONEY_MAX > 0);
					if (addordown == 1)
					{
						real_coin += rand() % (Money::RAND_MONEY_MAX + 1);
					}
					else
					{
						real_coin -= rand() % (Money::RAND_MONEY_MAX + 1);
					}
					if (!role->GetRoleModuleManager()->GetMoney()->AddCoinBind(real_coin, "SpecialLogicMiJingQiWenFB2::OnGather"))
					{
						return;
					}
					else
					{
						RoleActivity * role_activity = role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ANECDOTE_2);
						if (NULL != role_activity)
						{
							std::vector<RAMoneyInfo> money_list; money_list.push_back(RAMoneyInfo(MONEY_TYPE_COIN_BIND, real_coin));
							role_activity->OnSendResultNotice(Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_GET_REWARD, NULL, &money_list);
						}
					}
				}
				break;
			default:
				break;
			}
		}

		const ServerNPCInfoCfg *server_npc_cfg = NPCPOOL->GetServerNPCInfo(gather_cfg->npc_id);
		if (server_npc_cfg != NULL)
		{
			for (std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.begin(); it != m_obj_info_list.end(); it++)
			{
				for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
				{
					if (it2->obj_id == npc->GetId() && it2->npc_id == gather_cfg->npc_id)
					{
						if (it2->obj_type == new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_6)
						{
							m_gather_num -= 1;
						}
						else if (it2->obj_type == new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_8)
						{
							m_gather_num -= 1;
						}
						else if (it2->obj_type == new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_7)
						{
							m_gather_num -= 1;
						}
						else if (it2->obj_type == new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_5)
						{
							m_coin_num -= 1;
						}

						it->second.erase(it2);
						break;
					}
				}
			}
		}
		this->SendSceneObjInfo();
		this->SendSceneInfo();
	}
}

void SpecialLogicMiJingQiWenFB2::OnNPCChosedTalk(Role * role, ObjID npc_objid)
{
	Obj * npc_obj = m_scene->GetObj(npc_objid);
	if (npc_obj != NULL && Obj::OBJ_TYPE_NPC_CHOSED == npc_obj->GetObjType())
	{
		bool not_gouhuo = false;
		for (std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.begin(); it != m_obj_info_list.end(); it++)
		{
			for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
			{
				if (it2->obj_id == npc_objid)
				{
					if (it2->obj_type == new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_4)not_gouhuo = true;
				}
			}
		}
		if (!not_gouhuo)m_talk_npc_num += 1;
	}
	this->SendSceneInfo();
}

void SpecialLogicMiJingQiWenFB2::OnRoleAnswerQuestion(Role * role, ObjID npc_objid, int answer_id)
{
	if (role->InTeam())
	{
		if (!role->GetMyTeam()->IsLeader(role))
		{
			return;//如果组队，只允许队长答题
		}
	}
	Obj * npc_obj = m_scene->GetObj(npc_objid);
	if (npc_obj == NULL || Obj::OBJ_TYPE_NPC_CHOSED != npc_obj->GetObjType())return;
	NPCChosed * npc_chosed = (NPCChosed *)npc_obj;
	const RandActivityMiJingQiWenConfig2 * config = static_cast<const RandActivityMiJingQiWenConfig2 *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANECDOTE_2));
	if (NULL == config)
	{
		return;
	}

	std::map<ObjID, Protocol::MiJingQuestion2>::iterator it = m_question.find(npc_objid);
	if (it == m_question.end())return;
	

	bool answer_ret = false;
	const WSTQuestionBankCfg* question_group_cfg = LOGIC_CONFIG->GetWstQuestionConfig()->GetQuestionBankCfg(it->second.question_id);
	if (question_group_cfg == NULL)return;
	const MiJingQiWenFbNPCRewardCfg2 * npc_cfg = config->GetNpcRewardCfg(npc_chosed->GetNpcId(), 0);
	if (npc_cfg == NULL)return;
	const MiJingQiWenFbNPCRewardListCfg2 *npc_event = config->GetNpcEventCfg(npc_chosed->GetNpcId());
	if (npc_event == NULL)return;
	if (NULL != question_group_cfg)
	{
		if (it->second.answer_index != 0)
		{
			return;
		}
		if (question_group_cfg->answer_id == answer_id)
		{
			answer_ret = true;
			role->GetRoleModuleManager()->NoticeNum(noticenum::NT_RA_MG_ANSWER_SUCC);
		}
		else
		{
			answer_ret = false;
			role->GetRoleModuleManager()->NoticeNum(errornum::EN_MIJINGQIWEN_ANSWER_QUESTION_ERR);
		}
		it->second.answer_index = answer_id;
	}
	it->second.is_ture = answer_ret ? 1 : 2;//1：正确，2：错误
	this->SendQuestionInfo(npc_objid);//先下发结果
	
	if (answer_ret)
	{
		int reward_buff_id = 0;
		if (npc_event->reward_list[0].reward_type == MiJingQiWenFbNPCRewardCfg2::REWARD_TYPE_5)
		{
			reward_buff_id = answer_ret ? npc_event->reward_list[0].reward_list[0] : npc_event->reward_list[0].reward_list[1];
		}
		else if (npc_event->reward_list[1].reward_type == MiJingQiWenFbNPCRewardCfg2::REWARD_TYPE_5)
		{
			reward_buff_id = answer_ret ? npc_event->reward_list[1].reward_list[0] : npc_event->reward_list[0].reward_list[1];
		}

		if (reward_buff_id != 0)
		{
			const BuffListConfig*  buff_cfg = config->GetBuffId(reward_buff_id);
			if (buff_cfg != NULL)
			{
				if (role->InTeam())
				{
					Team *team = role->GetMyTeam();
					for (int i = 0; i < team->GetMemberCount(); ++i)
					{
						Role* member = team->GetMemberRoleByIndex(i);
						if (NULL == member) continue;
						this->AddRoleBuff(member, buff_cfg);
					}
				}
				else
				{
					this->AddRoleBuff(role, buff_cfg);
				}
			}
		}
	}
	else
	{
		int reward_buff_id = 0;
		if (npc_event->reward_list[0].reward_type == MiJingQiWenFbNPCRewardCfg2::REWARD_TYPE_5)
		{
			reward_buff_id = npc_event->reward_list[0].reward_list[1];
		}
		else if (npc_event->reward_list[1].reward_type == MiJingQiWenFbNPCRewardCfg2::REWARD_TYPE_5)
		{
			reward_buff_id = npc_event->reward_list[1].reward_list[1];
		}

		if (reward_buff_id != 0)
		{
			const BuffListConfig*  buff_cfg = config->GetBuffId(reward_buff_id);
			if (buff_cfg != NULL)
			{
				if (role->InTeam())
				{
					Team *team = role->GetMyTeam();
					for (int i = 0; i < team->GetMemberCount(); ++i)
					{
						Role* member = team->GetMemberRoleByIndex(i);
						if (NULL == member) continue;
						this->AddRoleBuff(member, buff_cfg);
					}
				}
				else
				{
					this->AddRoleBuff(role, buff_cfg);
				}
			}
		}
	}
}

void SpecialLogicMiJingQiWenFB2::OnRoleBuyItem(Role * role, ObjID npc_objid, int item_idx)
{
	std::map<int, MiJingItemInfo2>::iterator it = m_shop_info.shop_info.find(item_idx);
	if (it == m_shop_info.shop_info.end())
	{
		role->NoticeNum(errornum::EN_MIJINGQIWEN_BUY_NO_INDEX);
		return;
	}

	//检查次数
	BuyList2 &user_info = scene_user_shop_map[role->GetUID()];

	if (user_info.m_buy_info[item_idx].buy_times >= it->second.buy_times)
	{
		role->NoticeNum(errornum::EN_MIJINGQIWEN_BUY_TIME_LIMIT);
		return;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(it->second.item_id, it->second.num, false))
	{
		return;
	}

	//消耗
	switch (it->second.price_type)
	{
	case MiJingItemInfo2::PRICE_TYPE_0:
		{
			if (!role->GetRoleModuleManager()->GetMoney()->GoldMoreThan(it->second.price))
			{
				role->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}

			if (!role->GetRoleModuleManager()->GetMoney()->UseGold(it->second.price, __FUNCTION__))
			{
				return;
			}
		}
		break;
	case MiJingItemInfo2::PRICE_TYPE_2:
		{
			if (!role->GetRoleModuleManager()->GetMoney()->CoinBindMoreThan(it->second.price))
			{
				role->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}

			if (!role->GetRoleModuleManager()->GetMoney()->UseCoinBind(it->second.price, __FUNCTION__))
			{
				return;
			}
		}
		break;
	default:
		return;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->Put(ItemConfigData(it->second.item_id, it->second.is_bind == 1 ? true : false, it->second.num), PUT_REASON_MI_JING_QI_WEN_FB))
	{
		return;
	}

	user_info.m_buy_info[item_idx].buy_times += 1;
	this->SendShopInfo(role);
}

void SpecialLogicMiJingQiWenFB2::SetInfo(int layer, int level_seq, Posi transport_pos, std::vector<new_Event_NPC_cfg2> * has_create_npc_vec, std::map<int, std::vector<ItemConfigData> >* item_list)
{
	const RandActivityMiJingQiWenConfig2 * config = static_cast<const RandActivityMiJingQiWenConfig2 *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANECDOTE_2));
	if (NULL == config)
	{
		return;
	}
	if (layer <= 0 || layer > config->GetLayerNum(level_seq))
	{
		return;
	}

	m_layer = layer;
	m_level_seq = level_seq;
	m_transport_pos = transport_pos;

	m_monster_num_max = m_monster_num = 0;
	m_box_num_max = m_box_num = 0;
	m_npc_num_max = m_npc_num = 0;
	m_gouhuo_num_max = m_gouhuo_num = 0;
	m_coin_num_max = m_coin_num = 0;
	m_gather_num_max = m_gather_num = 0;

	for (std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.begin(); it != m_obj_info_list.end(); it++)
	{
		for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			m_scene->DeleteObj(it2->obj_id);
		}
	}
	m_obj_info_list.clear();

	m_has_create_npc_vec.clear();

	//this->CreateTransport();// 只有一层不刷传送门
	this->CreateMonster();
	this->CreateNpc();
	this->CreateBox();

	m_monster_num_max = m_monster_num;
	m_box_num_max = m_box_num;
	m_npc_num_max = m_npc_num;
	m_gouhuo_num_max = m_gouhuo_num;
	m_coin_num_max = m_coin_num;
	m_gather_num_max = m_gather_num;

	if (has_create_npc_vec != NULL)
	{
		m_has_create_npc_vec.assign(has_create_npc_vec->begin(), has_create_npc_vec->end());
	}
	if (NULL != item_list)
	{
		m_item_list.swap(*item_list);
	}
}

void SpecialLogicMiJingQiWenFB2::SendSceneInfo(Role * role)
{
	Protocol::SCMiJingQiWenFbObjInfo2 scene_info;
	scene_info.scene_id = this->GetScene()->GetSceneID();
	scene_info.level_seq = m_level_seq;
	scene_info.layer = m_layer;
	scene_info.box_num = m_box_num;
	scene_info.monster_num = m_monster_num;
	scene_info.npc_num = m_talk_npc_num;
	scene_info.coin_num = m_coin_num;
	scene_info.gouhuo_num = m_gouhuo_num;
	scene_info.gather_num = m_gather_num;
	scene_info.m_box_num_max = m_box_num_max;
	scene_info.m_coin_num_max = m_coin_num_max;
	scene_info.m_gather_num_max = m_gather_num_max;
	scene_info.m_gouhuo_num_max = m_gouhuo_num_max;
	scene_info.m_monster_num_max = m_monster_num_max;
	scene_info.m_npc_num_max = m_npc_num_max;

	scene_info.obj_count = 0;
	for (std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.begin(); it != m_obj_info_list.end(); it++)
	{
		if ((int)it->second.size() > 0 && scene_info.obj_count < Protocol::SCMiJingQiWenFbObjInfo2::MAX_OBJ_LIST_COUNT)
		{
			for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
			{
				scene_info.obj_list[scene_info.obj_count].npc_id = it2->npc_id;
				scene_info.obj_list[scene_info.obj_count].obj_type = (short)it->first;
				scene_info.obj_list[scene_info.obj_count].pos_x = it2->pos_x;
				scene_info.obj_list[scene_info.obj_count].pos_y = it2->pos_y;
				scene_info.obj_count += 1;
			}

		}
	}

	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&scene_info, sizeof(scene_info));
	}
	else
	{
		m_scene->SendToRole((const char*)&scene_info, sizeof(scene_info));
	}
}

void SpecialLogicMiJingQiWenFB2::SendSceneObjInfo(Role * role)
{
	Protocol::SCMiJingQiWenFbObjInfo2 scene_info;
	scene_info.scene_id = this->GetScene()->GetSceneID();
	scene_info.level_seq = m_level_seq;
	scene_info.layer = m_layer;
	scene_info.box_num = m_box_num;
	scene_info.monster_num = m_monster_num;
	scene_info.npc_num = m_talk_npc_num;
	scene_info.coin_num = m_coin_num;
	scene_info.gouhuo_num = m_gouhuo_num;
	scene_info.gather_num = m_gather_num;
	scene_info.m_box_num_max = m_box_num_max;
	scene_info.m_coin_num_max = m_coin_num_max;
	scene_info.m_gather_num_max = m_gather_num_max;
	scene_info.m_gouhuo_num_max = m_gouhuo_num_max;
	scene_info.m_monster_num_max = m_monster_num_max;
	scene_info.m_npc_num_max = m_npc_num_max;
	scene_info.obj_count = 0;
	for (std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.begin(); it != m_obj_info_list.end(); it++)
	{
		if ((int)it->second.size() > 0 && scene_info.obj_count < Protocol::SCMiJingQiWenFbObjInfo2::MAX_OBJ_LIST_COUNT)
		{
			for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
			{
				scene_info.obj_list[scene_info.obj_count].npc_id = it2->npc_id;
				scene_info.obj_list[scene_info.obj_count].obj_type = (short)it->first;
				scene_info.obj_list[scene_info.obj_count].pos_x = it2->pos_x;
				scene_info.obj_list[scene_info.obj_count].pos_y = it2->pos_y;
				scene_info.obj_count += 1;
			}

		}
	}

	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&scene_info, sizeof(scene_info) - ((Protocol::SCMiJingQiWenFbObjInfo2::MAX_OBJ_LIST_COUNT - scene_info.obj_count) * sizeof(scene_info.obj_list[0])));
	}
	else
	{
		m_scene->SendToRole((const char*)&scene_info, sizeof(scene_info));
	}
}

void SpecialLogicMiJingQiWenFB2::SendQuestionInfo(ObjID npc_objid, Role * role)
{
	Protocol::SCRAMiJingQiWenQuestionInfo2 info;
	std::map<ObjID, Protocol::MiJingQuestion2>::iterator it = m_question.find(npc_objid);
	if (it == m_question.end())return;
	info.question_info = it->second;

	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, sizeof(info));
	}
	else
	{
		m_scene->SendToRole((const char*)&info, sizeof(info));
	}
}

void SpecialLogicMiJingQiWenFB2::SendShopInfo(Role * role)
{
	Protocol::SCRAMiJingQiWenShopInfo2 info;
	info.shop_num = 0;
	std::map<UserID, BuyList2>::iterator buy_it = scene_user_shop_map.find(role->GetUID());
	for (int i = 0; i < Protocol::SCRAMiJingQiWenShopInfo2::MAX_SHOP_ITEM_NUM && i < (int)m_shop_info.shop_info.size(); i++)
	{
		std::map<int, MiJingItemInfo2>::iterator it = m_shop_info.shop_info.find(i);
		if (it != m_shop_info.shop_info.end())
		{
			info.shop_list[i].item_id = it->second.item_id;
			info.shop_list[i].num = it->second.num;
			info.shop_list[i].is_bind = it->second.is_bind;
			info.shop_list[i].price_type = it->second.price_type;
			info.shop_list[i].price = it->second.price;
			info.shop_list[i].buy_times = it->second.buy_times;
			info.shop_list[i].buy_item_idx = it->first;
			info.shop_list[i].discount = it->second.discount;
			info.shop_list[i].m_buy_times = 0;//初始化
			if (buy_it != scene_user_shop_map.end())
			{
				if (it->first >= 0 && it->first < Protocol::SCRAMiJingQiWenShopInfo2::MAX_SHOP_ITEM_NUM)
				{
					info.shop_list[i].m_buy_times = buy_it->second.m_buy_info[it->first].buy_times;
				}
			}
			info.shop_num++;
		}
	}

	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, sizeof(info) - ((Protocol::SCRAMiJingQiWenShopInfo2::MAX_SHOP_ITEM_NUM - info.shop_num) * sizeof(Protocol::SCRAMiJingQiWenShopInfo2::ItemInfo)));
}

void SpecialLogicMiJingQiWenFB2::CreateNpc()
{
	const RandActivityMiJingQiWenConfig2 * config = static_cast<const RandActivityMiJingQiWenConfig2 *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANECDOTE_2));
	if (NULL == config)
	{
		return;
	}
	
	const RoleCrystalFbLevelCfg  * cfg = config->GetRoleCrystalFbLevelCfg(m_level_seq);
	if (cfg == NULL)
	{
		return;
	}
	m_npc_num = 0;
	m_gather_num = 0;
	m_coin_num = 0;
	m_gouhuo_num = 0;
	std::vector<new_Event_NPC_cfg2> npc_node_cfg;
	int npc_num = config->GetRandNum(cfg->event_num);
	int real_npc_num = config->GetEventNpcCfg(cfg->event_group_id, npc_num, &npc_node_cfg, &m_has_create_npc_vec);
	for (int i = 0; i < real_npc_num && i < 100; ++i)//避免死循环
	{
		if ((int)npc_node_cfg.size() <= i)continue;

		const ServerNPCInfoCfg *  s_cfg = NPCPOOL->GetServerNPCInfo(npc_node_cfg[i].npc_id);
		if (NULL != s_cfg)
		{
			switch (s_cfg->server_npc_type)
			{
			case SERVER_NPC_TYPE_GATHERS:
				{
					NPCGather * gather = NpcManager::Instance().CreateNPCGather(npc_node_cfg[i].npc_id, m_scene, m_scene->GetARandPosi());
					if (gather == NULL)continue;
					SpecialLogicCrystalObjInfo node_info;
					node_info.obj_id = gather->GetId();
					node_info.npc_id = gather->GetNpcId();
					node_info.pos_x = gather->GetPos().x;
					node_info.pos_y = gather->GetPos().y;

					if (npc_node_cfg[i].npc_type == new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_6)
					{
						node_info.obj_type = Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER0;
						m_obj_info_list[Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER0].push_back(node_info);
						m_gather_num += 1;
					}
					else if (npc_node_cfg[i].npc_type == new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_8)
					{
						node_info.obj_type = Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER2;
						m_obj_info_list[Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER2].push_back(node_info);
						m_gather_num += 1;
					}
					else if (npc_node_cfg[i].npc_type == new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_7)
					{
						node_info.obj_type = Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER1;
						m_obj_info_list[Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER1].push_back(node_info);
						m_gather_num += 1;
					}
					else if (npc_node_cfg[i].npc_type == new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_5)
					{
						node_info.obj_type = Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_COIB;
						m_obj_info_list[Protocol::SCMiJingQiWenFbObjInfo2::MI_JING_QI_WEN_FB_OBJ_TYPE_COIB].push_back(node_info);
						m_coin_num += 1;
					}
				}
				break;
			case SERVER_NPC_TYPE_CHOSED:
				{
					NPCChosed * monster = NpcManager::Instance().CreateNPCChosed(npc_node_cfg[i].npc_id, m_scene, m_scene->GetARandPosi());
					if (monster == NULL)break;
					if (NULL != monster)
					{
						monster->SetBattleMode(BATTLE_MODE_RA_MIJINGQIWEN_2);
					}
					//检查事件
					const MiJingQiWenFbNPCRewardListCfg2 *npc_event = config->GetNpcEventCfg(npc_node_cfg[i].npc_id);
					if (npc_event != NULL)
					{
						m_has_create_npc_vec.push_back(npc_node_cfg[i]);//记录已经创建过的交互NPC
						int shop_id = 0;
						if (npc_event->reward_list[0].reward_type == MiJingQiWenFbNPCRewardCfg2::REWARD_TYPE_2)
						{
							shop_id = npc_event->reward_list[0].reward_list[0];
						}
						else if (npc_event->reward_list[1].reward_type == MiJingQiWenFbNPCRewardCfg2::REWARD_TYPE_2)
						{
							shop_id = npc_event->reward_list[1].reward_list[0];
						}

						if (shop_id != 0)//是商店，导入对应商店信息
						{
							NpcShopItem item_listp[MAX_ROLE_CRYSTALFB_BUY_ITEM_COUNT];
							int real_item_count = config->RandNPCShopItemList(shop_id, item_listp);
							for (int i = 0; i < real_item_count && i < MAX_ROLE_CRYSTALFB_BUY_ITEM_COUNT; i++)
							{
								MiJingItemInfo2 &node = m_shop_info.shop_info[i];
								node.item_id = item_listp[i].item.item_id;
								node.is_bind = item_listp[i].item.is_bind;
								node.num = item_listp[i].item.num;
								node.price_type = item_listp[i].item.money_type;
								node.price = item_listp[i].item.price;
								node.discount = item_listp[i].item.discount;
								node.buy_times = 1;
							}
						}
					}
					SpecialLogicCrystalObjInfo node_info;
					node_info.npc_id = monster->GetNpcId();
					node_info.obj_id = monster->GetId();
					node_info.pos_x = monster->GetPos().x;
					node_info.pos_y = monster->GetPos().y;
					if (npc_node_cfg[i].npc_type == new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_4)
					{
						node_info.obj_type = new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_4;
						m_obj_info_list[new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_4].push_back(node_info);
						m_gouhuo_num += 1;
					}
					else if (npc_node_cfg[i].npc_type == new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_2)
					{
						node_info.obj_type = new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_2;
						m_obj_info_list[new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_2].push_back(node_info);
						m_npc_num += 1;
					}
					else if(npc_node_cfg[i].npc_type == new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_11)
					{
						//随机题目
						const MiJingQiWenOtherCfg2 & other_cfg = config->GetCrystalFbOtherCfg();
						const WSTQuestionBankCfg* question_group_cfg = LOGIC_CONFIG->GetWstQuestionConfig()->GetQuestionBankByGroupCfg(other_cfg.question_id);
						if (NULL != question_group_cfg)
						{
							Protocol::MiJingQuestion2 &node = m_question[node_info.obj_id];
							if (node.question_id == 0)
							{
								node.question_id = question_group_cfg->question_id;
							}
						}
						
						node_info.obj_type = new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_11;
						m_obj_info_list[new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_11].push_back(node_info);
						m_npc_num += 1;
					}
				}
				break;
			default:
				break;
			}
		}
	}

	if (this->IsLastLayer(m_layer))
	{
		const RoleCrystalFbLevelCfg  * cfg = config->GetRoleCrystalFbLevelCfg(m_level_seq);
		if (cfg == NULL)
		{
			return;
		}
		NPCChosed * monster = NpcManager::Instance().CreateNPCChosed(cfg->recover_npc_id, m_scene, cfg->recover_npc_pos, BATTLE_MODE_RA_MIJINGQIWEN_2, &cfg->recover_npc_dir);
		if (monster == NULL)return;
		const ServerNPCInfoCfg *  s_cfg = NPCPOOL->GetServerNPCInfo(cfg->recover_npc_id);
		if (NULL != s_cfg)
		{
			SpecialLogicCrystalObjInfo node_info;
			node_info.npc_id = monster->GetNpcId();
			node_info.obj_id = monster->GetId();
			node_info.pos_x = monster->GetPos().x;
			node_info.pos_y = monster->GetPos().y;
			node_info.obj_type = new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_2;
			m_obj_info_list[new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_2].push_back(node_info);
			m_npc_num += 1;
		}
	}
}

void SpecialLogicMiJingQiWenFB2::CreateMonster()
{
	const RandActivityMiJingQiWenConfig2 * config = static_cast<const RandActivityMiJingQiWenConfig2 *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANECDOTE_2));
	if (NULL == config)
	{
		return;
	}
	const RoleCrystalFbLevelCfg  * cfg = config->GetRoleCrystalFbLevelCfg(m_level_seq);
	if (cfg == NULL)
	{
		return;
	}

	if (this->IsLastLayer(m_layer))		//最后一关刷boss
	{
		NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(cfg->boss_npc_id, m_scene, cfg->boss_pos, BATTLE_MODE_RA_MIJINGQIWEN_2, &cfg->monster_dir);
		if (NULL != monster)
		{
			m_boss_obj_id = monster->GetId();
			SpecialLogicCrystalObjInfo node_info;
			node_info.npc_id = monster->GetNpcId();
			node_info.obj_id = monster->GetId();
			node_info.obj_type = new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_3;
			node_info.pos_x = monster->GetPos().x;
			node_info.pos_y = monster->GetPos().y;
			m_obj_info_list[new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_3].push_back(node_info);
		}
	}
	
	{
		int monster_num = config->GetRandNum(cfg->monster_num_group_id);
		m_monster_num = 0;

		for (int i = 0; i < monster_num && i < 100; ++i)//避免无限判空死循环
		{
			const CrystalFbMonsterCfg::ItemCfg * monster_cfg = config->GetCrystalFbMonsterCfg(cfg->monster_id_group_id);
			if (monster_cfg == NULL)
			{
				continue;
			}

			NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(monster_cfg->monster_npc_id, m_scene, m_scene->GetARandPosi(), BATTLE_MODE_RA_MIJINGQIWEN_2, &cfg->monster_dir);
			if (NULL != monster)
			{
				m_monster_num += 1;
				SpecialLogicCrystalObjInfo node_info;
				node_info.npc_id = monster->GetNpcId();
				node_info.obj_id = monster->GetId();
				node_info.obj_type = new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_1;
				node_info.pos_x = monster->GetPos().x;
				node_info.pos_y = monster->GetPos().y;
				m_obj_info_list[new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_1].push_back(node_info);
			}
		}
	}
}

void SpecialLogicMiJingQiWenFB2::CreateBox()
{
	if (this->IsLastLayer(m_layer))
	{
		return;
	}
	const RandActivityMiJingQiWenConfig2 * config = static_cast<const RandActivityMiJingQiWenConfig2 *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANECDOTE_2));
	if (NULL == config)
	{
		return;
	}

	const RoleCrystalFbLevelCfg  * cfg = config->GetRoleCrystalFbLevelCfg(m_level_seq);
	if (cfg == NULL)
	{
		return;
	}

	int box_num = config->GetRandNum(cfg->box_num_group_id);
	m_box_num = box_num;
	for (int i = 0; i < box_num; ++i)
	{
		const CrystalFbBoxCfg::ItemCfg * box_cfg = config->GetRandBoxCfg(cfg->box_group_id);
		if (box_cfg == NULL)
		{
			continue;
		}

		TreasureChest  * treasure_chest_obj = new TreasureChest(box_cfg->treasure_chest_id);
		treasure_chest_obj->SetPos(m_scene->GetARandPosi());
		treasure_chest_obj->SetParam(&cfg->box_group_id, &box_cfg->seq);
		m_scene->AddObj(treasure_chest_obj);

		SpecialLogicCrystalObjInfo node_info;
		node_info.npc_id = box_cfg->treasure_chest_id;
		node_info.obj_id = treasure_chest_obj->GetId();
		node_info.obj_type = new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_9;
		node_info.pos_x = treasure_chest_obj->GetPos().x;
		node_info.pos_y = treasure_chest_obj->GetPos().y;
		m_obj_info_list[new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_9].push_back(node_info);
	}

}

void SpecialLogicMiJingQiWenFB2::CreateTransport()
{
	if (this->IsLastLayer(m_layer))
	{
		return;
	}

	SpecialTransport * transport_obj = new SpecialTransport();
	if (transport_obj != NULL)
	{
		transport_obj->SetPos(m_transport_pos);
		transport_obj->GetExtraInfo().send_param1 = m_layer;
		transport_obj->GetExtraInfo().send_param2 = m_level_seq;
		m_scene->AddObj(transport_obj);
		SpecialLogicCrystalObjInfo node_info;
		node_info.npc_id = 0;
		node_info.obj_id = transport_obj->GetId();
		node_info.obj_type = new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_10;
		node_info.pos_x = transport_obj->GetPos().x;
		node_info.pos_y = transport_obj->GetPos().y;
		m_obj_info_list[new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_10].push_back(node_info);
	}
}

void SpecialLogicMiJingQiWenFB2::CreateLeaveTransport(const Posi & pos)
{
	SpecialTransport * transport_obj = new SpecialTransport();

	if (transport_obj == NULL)
	{
		return;
	}

	transport_obj->SetPos(gamecommon::GetDisRandPos(pos, 5));
	transport_obj->GetExtraInfo().param1 = 0;
	transport_obj->GetExtraInfo().send_param1 = m_layer;
	transport_obj->GetExtraInfo().send_param2 = m_level_seq;
	m_scene->AddObj(transport_obj);
	SpecialLogicCrystalObjInfo node_info;
	node_info.npc_id = 0;
	node_info.obj_id = transport_obj->GetId();
	node_info.obj_type = new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_10;
	node_info.pos_x = transport_obj->GetPos().x;
	node_info.pos_y = transport_obj->GetPos().y;
	m_obj_info_list[new_Event_NPC_cfg2::CRYSTALFB_NPC_TYPE_10].push_back(node_info);
	this->SendSceneObjInfo();
}

bool SpecialLogicMiJingQiWenFB2::IsLastLayer(int layer)
{
	return true;//必然是最后一层
}

void SpecialLogicMiJingQiWenFB2::TransportNextLayer(Role * role, SpecialTransport * transport_obj)
{
	if (NULL == role || NULL == transport_obj)
	{
		return;
	}

	const RandActivityMiJingQiWenConfig2 * config = static_cast<const RandActivityMiJingQiWenConfig2 *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANECDOTE_2));
	if (NULL == config)
	{
		return;
	}

	const RoleCrystalFbLevelCfg  * cfg = config->GetRoleCrystalFbLevelCfg(m_level_seq);
	if (cfg == NULL)
	{
		return;
	}

	int scene_id = cfg->last_layer_scene_id;
	int scene_key = m_scene->GetSceneKey();
	Posi target_pos(0, 0);
	Posi transport_pos(0, 0);

	int next_layer = m_layer + 1;
	if (!IsLastLayer(next_layer))
	{
		const RoleCrystalFbLevelCfg::LayerCfg * layer_cfg = cfg->GetLayerCfg(next_layer);
		if (layer_cfg == NULL)
		{
			return;
		}

		const CrystalFbSceneCfg::ItemCfg * scene_cfg = config->GetRandSceneCfg(layer_cfg->scene_id_group, m_already_rand_scene_id);
		if (scene_cfg == NULL)
		{
			return;
		}

		scene_id = scene_cfg->scene_id;
		target_pos = scene_cfg->birth_pos;
		transport_pos = scene_cfg->transport_pos;
	}
	else
	{
		target_pos = config->GetRandPosi(cfg->boss_layer_sceneborn_group_id);
	}

	switch (cfg->type)
	{
	case  ROLE_CRYSTAL_FB_TYPE_XI_YOU:
	case	ROLE_CRYSTAL_FB_TYPE_JING_YING:
		{
			Team * team = role->GetMyTeam();
			if (NULL == team)
			{
				return;
			}

			if (!team->IsLeader(role))
			{
				return;
			}

			//检查非暂离队员能否传送
			for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
			{
				TeamMember *  member_info = team->GetMemberInfoByIndex(i);
				if (member_info == NULL)
				{
					continue;
				}

				Role * member_role = team->GetMemberRoleByIndex(i);
				if (member_role == NULL)
				{
					continue;
				}

				if (!member_role->GetRoleStatusManager()->CanTransportTo(scene_id))
				{
					role->NoticeNum(errornum::EN_TEAM_MEMBER_NOT_TRANSPORT);
					return;
				}
			}
		}
		break;
	}

	SpecialLogicMiJingQiWenFB2 * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicMiJingQiWenFB2(scene_id, scene_key);
	if (special_lobgic == NULL)
	{
		return;
	}
	special_lobgic->SetInfo(m_layer + 1, m_level_seq, transport_pos, &m_has_create_npc_vec, &m_item_list);
	special_lobgic->m_already_rand_scene_id.insert(this->m_already_rand_scene_id.begin(), this->m_already_rand_scene_id.end());

	Team * team = role->GetMyTeam();
	if (team != NULL)
	{
		this->TeamGoNextLayer(team, scene_id, scene_key, target_pos);
	}
	else
	{
		World::GetInstWorld()->GetSceneManager()->GoTo(role, scene_id, scene_key, target_pos);
	}
}

void SpecialLogicMiJingQiWenFB2::RoleAloneGetRewardByCfg(Role * role, const MiJingQiWenFbNPCRewardCfg2 * npc_cfg, ObjID *targer_obj)
{
	if (role == NULL || npc_cfg == NULL)return;
	const RandActivityMiJingQiWenConfig2 * config = static_cast<const RandActivityMiJingQiWenConfig2 *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANECDOTE_2));
	if (NULL == config)
	{
		return;
	}
	int index = 0;
	if (index < 0 || index >= MAX_ROLE_CRYSTAL_FB_REWARD_COLUMN)return;
	switch (npc_cfg->reward_type)
	{
	case MiJingQiWenFbNPCRewardCfg2::REWARD_TYPE_0:
		{
			const BuffListConfig*  buff_cfg = config->GetBuffId(npc_cfg->reward_list[index]);
			if (buff_cfg != NULL)
			{
				this->AddRoleBuff(role, buff_cfg);
			}
		}
		break;
	case MiJingQiWenFbNPCRewardCfg2::REWARD_TYPE_1:
		{
			const ItemConfigData * reward_cfg = config->GetRandNpcReward(npc_cfg->reward_list[index]);
			if (reward_cfg != NULL)
			{
				if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(reward_cfg->item_id, reward_cfg->num))
				{
					role->GetRoleModuleManager()->GetKnapsack()->Put(*reward_cfg, PUT_REASON_MI_JING_QI_WEN_FB);
					RoleActivity * role_activity = role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ANECDOTE_2);
					if (NULL != role_activity)
					{
						std::vector<ItemConfigData> item_list; item_list.push_back(*reward_cfg);
						role_activity->OnSendResultNotice(Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_GET_REWARD, &item_list);
					}
				}
				else
				{
					gamelog::g_log_knapsack.printf(LL_INFO, "SpecialLogicMiJingQiWenFB2::OnNPCChosedReward role[%s,%d] knapsack full no put item[id:%d,num:%d,is_bind:%d]",
						role->GetName(), role->GetUID(), reward_cfg->item_id, reward_cfg->num, reward_cfg->is_bind ? 1 : 0);
				}
			}
		}
		break;
	case MiJingQiWenFbNPCRewardCfg2::REWARD_TYPE_2:
		{
			this->SendShopInfo(role);
		}
		break;
	case MiJingQiWenFbNPCRewardCfg2::REWARD_TYPE_3:
		{
			if (npc_cfg->reward_list[index] > 0)
			{
				if (!role->GetRoleModuleManager()->GetMoney()->AddCoinBind(npc_cfg->reward_list[index], "SpecialLogicMiJingQiWenFB2::OnNPCChosedReward"))
				{
					gamelog::g_log_knapsack.printf(LL_INFO, "SpecialLogicMiJingQiWenFB2::OnNPCChosedReward role[%s,%d] knapsack add bindcoin[%d]",
						role->GetName(), role->GetUID(), npc_cfg->reward_list[index]);
				}
				else
				{
					RoleActivity * role_activity = role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ANECDOTE_2);
					if (NULL != role_activity)
					{
						std::vector<RAMoneyInfo> money_list; money_list.push_back(RAMoneyInfo(MONEY_TYPE_COIN_BIND, npc_cfg->reward_list[index]));
						role_activity->OnSendResultNotice(Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_GET_REWARD, NULL, &money_list);
					}
				}
			}
		}
		break;
	case MiJingQiWenFbNPCRewardCfg2::REWARD_TYPE_5:
		{
			if(targer_obj != NULL)this->SendQuestionInfo(*targer_obj);
		}
		break;
	default:
		break;
	}
}

void SpecialLogicMiJingQiWenFB2::RoleTeamGetRewardByCfg(Role * role, const MiJingQiWenFbNPCRewardCfg2 * npc_cfg, ObjID *targer_obj)
{
	if (role == NULL || npc_cfg == NULL)return;
	if (role->InTeam())
	{
		Team* role_team = role->GetMyTeam();
		if (role_team == NULL || !role_team->IsLeader(role))return;
		for (int i = 0; i < role_team->GetMemberCount() && i < MAX_TEAM_MEMBER_NUM; i++)
		{
			Role *temp_role = role_team->GetMemberRoleByIndex(i);
			if (temp_role == NULL || !temp_role->IsFollow())continue;
			this->RoleAloneGetRewardByCfg(temp_role, npc_cfg, targer_obj);
		}
	}
	else
	{
		this->RoleAloneGetRewardByCfg(role, npc_cfg, targer_obj);
	}
}

void SpecialLogicMiJingQiWenFB2::AddRoleBuff(Role * role, const BuffListConfig * buff_cfg)
{
	if (role == NULL || buff_cfg == NULL)
	{
		return;
	}
	if (buff_cfg->type == BuffListConfig::REWARDBUFTYPE_1)
	{
		RoleSceneStatus* role_scene_status = role->GetSceneStatus();
		if (role_scene_status != NULL)
		{
			BuffEffect buff_param;
			//role_scene_status->SetSceneBuff();
		}
	}
	else if (buff_cfg->type == BuffListConfig::REWARDBUFTYPE_2)
	{
		Attribute role_attr = role->GetRoleModuleManager()->GetAttrByType(BATTLE_ATTR_MAX_MP);
		int add_mp = (role_attr * buff_cfg->param_1) / 100;
		role->GetRoleModuleManager()->ChangeMp(add_mp);

		Pet* role_pet = role->GetRoleModuleManager()->GetPet();
		int pet_fight_index = role_pet->GetFightIndex();
		role_attr = role_pet->GetPetAttr(pet_fight_index, BATTLE_ATTR_MAX_MP);
		add_mp = (role_attr * buff_cfg->param_1) / 100;
		role_pet->ChangeMp(pet_fight_index, add_mp);

		Partner * role_partner = role->GetRoleModuleManager()->GetPartner();
		int partner_list[PARTNER_SYNFIGHT_NUM_MAX] = { 0 }, fight_num = 0;
		role_partner->GetSynFightPartnerIdList(PARTNER_SYNFIGHT_NUM_MAX, partner_list, &fight_num);
		for (int i = 0; i < fight_num; ++i)
		{
			if (partner_list[i] != 0)
			{
				role_partner->AlterPartnerHPOrMPPerce(partner_list[i], MAX_TYPE_ONLY_CMP, ((double)buff_cfg->param_1 / 100.0));
			}
		}

		this->SendBuffEffectToRole(role, Protocol::SCRoleSceneEffect::ROLE_SCENE_EFFECT_TYPE_1);
		char info[20];
		memset(info, 0, sizeof(info));
		SNPRINTF(info, sizeof(info), "%d%s", buff_cfg->param_1, "%");
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_buff_effect_mp_message, info);
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
	else if (buff_cfg->type == BuffListConfig::REWARDBUFTYPE_3)
	{
		Attribute role_attr = role->GetRoleModuleManager()->GetAttrByType(BATTLE_ATTR_MAX_HP);
		int add_hp = (role_attr * buff_cfg->param_1) / 100;
		role->GetRoleModuleManager()->ChangeHp(add_hp);

		Pet* role_pet = role->GetRoleModuleManager()->GetPet();
		int pet_fight_index = role_pet->GetFightIndex();
		role_attr = role_pet->GetPetAttr(pet_fight_index, BATTLE_ATTR_MAX_HP);
		add_hp = (role_attr * buff_cfg->param_1) / 100;
		role_pet->ChangeHP(pet_fight_index, add_hp);

		Partner * role_partner = role->GetRoleModuleManager()->GetPartner();
		int partner_list[PARTNER_SYNFIGHT_NUM_MAX] = { 0 }, fight_num = 0;
		role_partner->GetSynFightPartnerIdList(PARTNER_SYNFIGHT_NUM_MAX, partner_list, &fight_num);
		for (int i = 0; i < fight_num; ++i)
		{
			if (partner_list[i] != 0)
			{
				role_partner->AlterPartnerHPOrMPPerce(partner_list[i], MAX_TYPE_ONLY_CHP, ((double)buff_cfg->param_1 / 100.0));
			}
		}

		this->SendBuffEffectToRole(role, Protocol::SCRoleSceneEffect::ROLE_SCENE_EFFECT_TYPE_0);
		char info[20];
		memset(info, 0, sizeof(info));
		SNPRINTF(info, sizeof(info), "%d%s", buff_cfg->param_1, "%");
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_buff_effect_hp_message, info);
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
	else if (buff_cfg->type == BuffListConfig::REWARDBUFTYPE_4)
	{
		BuffChangeAttr *buff = new BuffChangeAttr(role->GetRoleModuleManager(), 0, 0);
		buff->AddAttr(buff_cfg->param_1, buff_cfg->param_2);
		buff->SetSave(false);
		buff->SetSceneBuffType(this->GetSceneType());
		role->GetRoleModuleManager()->GetBuffManager()->AddBuff(buff);
		this->SendBuffEffectToRole(role, Protocol::SCRoleSceneEffect::ROLE_SCENE_EFFECT_TYPE_3);
		if (buff_cfg->param_2 > 0)
		{
			role->GetRoleModuleManager()->NoticeNum(noticenum::NT_BUFF_EFFECT_SUCC);
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_buff_effect_attr_message, buff_cfg->param_1);
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
		else
		{
			role->GetRoleModuleManager()->NoticeNum(noticenum::NT_BUFF_DIF_EFFECT_SUCC);
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_buff_bad_effect_attr_message, buff_cfg->param_1);
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

		
	}
	else if (buff_cfg->type == BuffListConfig::REWARDBUFTYPE_5)
	{
		BuffChangeAttr *buff = new BuffChangeAttr(role->GetRoleModuleManager(), 0, 0);
		int num_persent = role->GetRoleModuleManager()->GetAttrByType(buff_cfg->param_1);
		buff->AddAttr(buff_cfg->param_1, (num_persent * buff_cfg->param_2) / 100);
		buff->SetSave(false);
		buff->SetSceneBuffType(this->GetSceneType());
		role->GetRoleModuleManager()->GetBuffManager()->AddBuff(buff);
		this->SendBuffEffectToRole(role, Protocol::SCRoleSceneEffect::ROLE_SCENE_EFFECT_TYPE_3);
		if (buff_cfg->param_2 > 0)
		{
			role->GetRoleModuleManager()->NoticeNum(noticenum::NT_BUFF_EFFECT_SUCC);
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_buff_effect_attr_message, buff_cfg->param_1);
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
		else
		{
			role->GetRoleModuleManager()->NoticeNum(noticenum::NT_BUFF_DIF_EFFECT_SUCC);
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_buff_bad_effect_attr_message, buff_cfg->param_1);
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
	}
	else if (buff_cfg->type == BuffListConfig::REWARDBUFTYPE_6)
	{

		Attribute role_attr = role->GetRoleModuleManager()->GetAttrByType(BATTLE_ATTR_MAX_MP);
		int add_mp = (role_attr * buff_cfg->param_1) / 100;
		role->GetRoleModuleManager()->ChangeMp(add_mp);

		role_attr = role->GetRoleModuleManager()->GetAttrByType(BATTLE_ATTR_MAX_HP);
		int add_hp = (role_attr * buff_cfg->param_1) / 100;
		role->GetRoleModuleManager()->ChangeHp(add_hp);

		Pet* role_pet = role->GetRoleModuleManager()->GetPet();
		int pet_fight_index = role_pet->GetFightIndex();
		role_attr = role_pet->GetPetAttr(pet_fight_index, BATTLE_ATTR_MAX_MP);
		add_mp = (role_attr * buff_cfg->param_1) / 100;
		role_pet->ChangeMp(pet_fight_index, add_mp);

		role_attr = role_pet->GetPetAttr(pet_fight_index, BATTLE_ATTR_MAX_HP);
		add_hp = (role_attr * buff_cfg->param_1) / 100;
		role_pet->ChangeHP(pet_fight_index, add_hp);

		Partner * role_partner = role->GetRoleModuleManager()->GetPartner();
		int partner_list[PARTNER_SYNFIGHT_NUM_MAX] = { 0 }, fight_num = 0;
		role_partner->GetSynFightPartnerIdList(PARTNER_SYNFIGHT_NUM_MAX, partner_list, &fight_num);
		for (int i = 0; i < fight_num; ++i)
		{
			if (partner_list[i] != 0)
			{
				role_partner->AlterPartnerHPOrMPPerce(partner_list[i], MAX_TYPE_ONLY_CMP, ((double)buff_cfg->param_1 / 100.0));
				role_partner->AlterPartnerHPOrMPPerce(partner_list[i], MAX_TYPE_ONLY_CHP, ((double)buff_cfg->param_1 / 100.0));
			}
		}

		this->SendBuffEffectToRole(role, Protocol::SCRoleSceneEffect::ROLE_SCENE_EFFECT_TYPE_2);
		char info[20];
		memset(info, 0, sizeof(info));
		SNPRINTF(info, sizeof(info), "%d%s", buff_cfg->param_1, "%");
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
}

void SpecialLogicMiJingQiWenFB2::SendBuffEffectToRole(Role * buff_geter, int buff_type)
{
	if (buff_geter == NULL)return;
	Protocol::SCRoleSceneEffect my_buff_effect;
	my_buff_effect.buffer_obj = buff_geter->GetId();
	my_buff_effect.effect_type = buff_type;
	buff_geter->GetScene()->SendToRole((const char*)&my_buff_effect, sizeof(my_buff_effect));
}

void SpecialLogicMiJingQiWenFB2::TeamGoNextLayer(Team * team, int target_scene_id, int target_scene_key, const Posi & target_pos)
{
	if (NULL == team) return;
	Role* leader_role = team->GetLeaderRole();
	if (NULL == leader_role) return;

	Role* role_list[MAX_TEAM_MEMBER_NUM] = { NULL };
	int role_num = 0;
	role_list[role_num++] = leader_role;

	Robot* robot_list[MAX_TEAM_MEMBER_NUM] = { NULL };
	int robot_num = 0;

	int member_num = team->GetMemberCount();
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM && i < member_num; i++)
	{
		TeamMember* member = team->GetMemberInfo(i);
		if (NULL == member || member->Invalid()) continue;
		if (member->uid == leader_role->GetUID()) continue;

		if (!member->IsRobot())
		{
			if (!member->is_online) continue;

			Role* role = team->GetMemberRoleByIndex(i);
			if (NULL == role || !role->IsFollow()) continue;

			if (!member->is_follow)
			{
				if (leader_role->GetScene()->GetSceneID() != role->GetScene()->GetSceneID()) continue;

				TeamManager::Instance().OnTemporarilyPart(role);
			}

			role_list[role_num++] = role;
		}
		else
		{
			Robot* robot = team->GetMemberRobotByIndex(i);
			if (NULL == robot) continue;

			robot_list[robot_num++] = robot;
		}
	}
	for (int i = 0; i < role_num && i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		Role* role = role_list[i];
		World::GetInstWorld()->GetSceneManager()->GoTo(role, target_scene_id, target_scene_key, target_pos);
	}

	for (int i = 0; i < robot_num && i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		Robot* robot = robot_list[i];
		World::GetInstWorld()->GetSceneManager()->GoTo(robot, target_scene_id, target_scene_key, target_pos);
	}
}

void SpecialLogicMiJingQiWenFB2::ItemStack(Role * role, const ItemConfigData * item)
{
	if (NULL == role || NULL == item) return;

	std::map<int, std::vector<ItemConfigData> >::iterator it = m_item_list.find(role->GetUID());
	if (it == m_item_list.end())
	{
		m_item_list[role->GetUID()].push_back(*item);
		return;
	}

	for (std::vector<ItemConfigData>::iterator item_it = it->second.begin(); item_it != it->second.end(); item_it++)
	{
		if (item_it->item_id != item->item_id) continue;

		item_it->num += item->num;
		return;
	}

	m_item_list[role->GetUID()].push_back(*item);
	return;
}
