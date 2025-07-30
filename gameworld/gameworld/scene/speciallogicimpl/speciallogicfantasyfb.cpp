#include "speciallogicfantasyfb.hpp"

#include "scene/scene.h"
#include "scene/fbmanager.hpp"
#include "obj/character/npc.h"
#include "obj/character/role.h"
#include "obj/character/npc_monster.h"
#include "obj/character/npc_gather.h"

#include "global/team/team.hpp"
#include "global/team/teammanager.hpp"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/string/gameworldstr.h"

#include "gameworld/world.h"
#include "gameworld/gamelog.h"
#include "gameworld/npc/npcmanager.h"
#include "gameworld/drop/droppool.hpp"
#include "gameworld/protocol/msgother.h"

#include "config/logicconfigmanager.hpp"

#include "obj/otherobj/treasurechest.h"
#include "obj/otherobj/specialtransport.h"

#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/route/mailroute.hpp"
#include "other/skill/roleskill.hpp"
#include "other/rolegatherfb/gatherfbconfig.hpp"
#include "other/fantasyfb/fantasyfbconfig.hpp"
#include "other/dayreward/dayrewardconfig.hpp"
#include "other/levelcomplement/levelcomplement.h"
#include "other/rolelifeskill/rolenewlifeskill.hpp"

SpecialLogicFantasyFb::SpecialLogicFantasyFb(Scene * scene):SpecialLogic(scene), m_is_init(false), m_layer(0),m_boss_id(0), kill_boss_flag(0),
	m_box_num(0), m_gather_num(0), m_box_num_max(0), m_gather_num_max(0)
{
	m_transport_pos = Posi();
}

SpecialLogicFantasyFb::~SpecialLogicFantasyFb()
{
}

void SpecialLogicFantasyFb::OnRoleEnterScene(Role * role)
{
	if(NULL == role) return;

	if (!m_is_init)
	{
		m_is_init = true;

		this->CreateMonster();
		this->CreateNpcOrBox();
	}
	m_box_num_max = m_box_num;
	m_gather_num_max = m_gather_num;

	role->SetEncounterMonsterFlag(true);

	this->SendSceneInfo(role);
	this->SendSceneObjInfo(role);
	this->SendRewardInfo(role);
}

void SpecialLogicFantasyFb::OnRoleLeaveScene(Role * role, bool is_logout)
{
	role->RestoreLastEncounterMonsterFlag();
}

void SpecialLogicFantasyFb::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (BATTLE_MODE_FANTASY_FB == ack->battle_mode &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		//this->SyncHpMp(ack->special_business_data.data);
	}

	if (BATTLE_MODE_FANTASY_FB == ack->battle_mode && RESULT_TYPE_ATTACKER_WIN == ack->battle_result)
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
			
			const FantasyFBSceneCfg * cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetSceneCfgBySeq(m_level_seq[m_layer]);
			int monster_type = Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_1;
			if (cfg != NULL)
			{
				if (npc_monster->GetNpcId() == m_boss_id)
				{
					m_boss_id = INVALID_OBJ_ID;
					kill_boss_flag = 1;
					monster_type = Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_3;
					this->CreateTransport(m_transport_pos);
					this->ClearSceneBox();

					for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
					{
						Role * mem_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(ack->attacker_list.role_list[i].uid));
						if (NULL != mem_role)
						{
							//是否首通
							this->CheckFirstPassFB(mem_role);
							if (this->IsLastLayer())
							{
								this->SendNotifyGetItemList(mem_role);	
								EventHandler::Instance().OnPassFantasyEctype(mem_role);
							}
							else
							{
								this->SendNotifyGetItemList(mem_role);
							}
						}
					}
				}
			}
			std::map<int, std::vector<SpecialLogicFantasyObjInfo> >::iterator it = m_obj_info_list.find(monster_type);
			if (it != m_obj_info_list.end())
			{
				for (std::vector<SpecialLogicFantasyObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
				{
					if (it2->obj_id == npc->GetId())
					{
						it->second.erase(it2);
						break;
					}
				}
			}
			m_scene->DeleteObj(npc->GetId());
			this->SendSceneObjInfo();
			this->SendSceneInfo();
		}
	}
}

void SpecialLogicFantasyFb::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	if (BATTLE_MODE_FANTASY_FB == ack->battle_mode &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		//this->SyncHpMp(ack->special_business_data.data);
	}
}


void SpecialLogicFantasyFb::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack)
{
	if (BATTLE_MODE_FANTASY_FB == ack->battle_mode &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		//this->SyncHpMp(ack->special_business_data.data);
	}
}

void SpecialLogicFantasyFb::OnRoleLeaveTeam(Role * role)
{
	this->DelayKickOutRole(role);
}

void SpecialLogicFantasyFb::OnGather(Role * role, int gather_id, int seq_id, const Posi & pos)
{
	if(NULL == role) return;

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
	
	Team *team = role->GetMyTeam();
	if (NULL != team)
	{
		int member_count = team->GetMemberCount();
		for (int i = 0; i < member_count && i < MAX_TEAM_MEMBER_NUM; i++)
		{
			Role * member_role = team->GetMemberRoleByIndex(i);
			if (member_role != NULL && member_role->IsFollow())
			{
				if (!member_role->GetRoleModuleManager()->GetCommonData().fantasy_fb_flag.IsBitSet(m_layer)) continue;

				const FantasyFBGatherRewardCfg *  gather_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetGatherRewardCfg(member_role->GetLevel(), gather_id);
				if (NULL != gather_cfg)
				{
					switch (gather_cfg->reward_type)
					{
					case GatherNpcConfig::REWARD_TYPE_0:
					{
					}
					break;
					case GatherNpcConfig::REWARD_TYPE_1:
					{
						const FantasyFBRewardGroupCfg * reward_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetRewardGroupCfg(gather_cfg->reward_id);
						if (reward_cfg != NULL)
						{
							const ItemBase * item_base = ItemPool::Instance()->GetItem(reward_cfg->reward.item_id);
							if(NULL == item_base) continue;

							ItemConfigData reward(reward_cfg->reward.item_id, reward_cfg->reward.is_bind, reward_cfg->reward.num);
							int new_life_skill_type = 0, new_life_skill_level = 0;;
							reward.num *= member_role->GetRoleModuleManager()->GetRoleNewLifeSkill()->GetMultipleByItemSubType(item_base->GetItemSubType(), &new_life_skill_type, &new_life_skill_level);
							const GatherFbSkillLevelCfg * skill_level_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetSkillLevelCfg(this->GetScene()->GetSceneID(), new_life_skill_level);
							if (NULL != skill_level_cfg)
							{
								if (skill_level_cfg->multiple > 0)
								{
									reward.num *= skill_level_cfg->multiple;
								}
								const GatherFbSpecialRewardGroupCfg * reward_group_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetRandSpecialRewardCfg(skill_level_cfg->special_reward_group);
								if (NULL != reward_group_cfg)
								{
									this->ItemStack(member_role, &reward_group_cfg->reward);
								}
							}

							this->ItemStack(member_role, &reward);
							this->SendRewardInfo(member_role);

							/*if (member_role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(reward_cfg->reward.item_id, reward_cfg->reward.num))
							{
							member_role->GetRoleModuleManager()->GetKnapsack()->Put(reward_cfg->reward, PUT_REASON_FANTASY_FB);
							}
							else
							{
							gamelog::g_log_knapsack.printf(LL_INFO, "SpecialLogicFantasyFb::OnGather role[%s,%d] knapsack full no put item[id:%d,num:%d,is_bind:%d]",
							member_role->GetName(), member_role->GetUID(), reward_cfg->reward.item_id, reward_cfg->reward.num, reward_cfg->reward.is_bind ? 1 : 0);
							}*/
						}
					}
					break;
					case GatherNpcConfig::REWARD_TYPE_2:
					{
					}
					break;
					default:
						break;
					}
				}
			}
		}
	}
	else
	{
		if (!role->GetRoleModuleManager()->GetCommonData().fantasy_fb_flag.IsBitSet(m_layer))
		{
			const FantasyFBGatherRewardCfg *  gather_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetGatherRewardCfg(role->GetLevel(), gather_id);
			if (NULL != gather_cfg)
			{
				switch (gather_cfg->reward_type)
				{
				case GatherNpcConfig::REWARD_TYPE_0:
				{
				}
				break;
				case GatherNpcConfig::REWARD_TYPE_1:
				{
					const FantasyFBRewardGroupCfg * reward_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetRewardGroupCfg(gather_cfg->reward_id);
					if (reward_cfg != NULL)
					{
						this->ItemStack(role, &reward_cfg->reward);
						this->SendRewardInfo(role);

						/*if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(reward_cfg->reward.item_id, reward_cfg->reward.num))
						{
							role->GetRoleModuleManager()->GetKnapsack()->Put(reward_cfg->reward, PUT_REASON_FANTASY_FB);
						}
						else
						{
							gamelog::g_log_knapsack.printf(LL_INFO, "SpecialLogicFantasyFb::OnGather role[%s,%d] knapsack full no put item[id:%d,num:%d,is_bind:%d]",
								role->GetName(), role->GetUID(), reward_cfg->reward.item_id, reward_cfg->reward.num, reward_cfg->reward.is_bind ? 1 : 0);
						}*/
					}
				}
				break;
				case GatherNpcConfig::REWARD_TYPE_2:
				{
				}
				break;
				default:
					break;
				}
			}
		}
	}
	const ServerNPCInfoCfg *server_npc_cfg = NPCPOOL->GetServerNPCInfo(gather_id);
	if (server_npc_cfg != NULL)
	{
		for (std::map<int, std::vector<SpecialLogicFantasyObjInfo> >::iterator it = m_obj_info_list.begin(); it != m_obj_info_list.end(); it++)
		{
			for (std::vector<SpecialLogicFantasyObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
			{
				if (it2->obj_id == npc->GetId() && it2->npc_id == gather_id)
				{
					if (it2->obj_type == Event_NPC_cfg::CRYSTALFB_NPC_TYPE_6)
					{
						m_gather_num -= 1;
					}
					else if (it2->obj_type == Event_NPC_cfg::CRYSTALFB_NPC_TYPE_8)
					{
						m_gather_num -= 1;
					}
					else if (it2->obj_type == Event_NPC_cfg::CRYSTALFB_NPC_TYPE_7)
					{
						m_gather_num -= 1;
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

void SpecialLogicFantasyFb::OnLeaveFBSituation(Role * role, int param1)
{
	if(NULL == role) return;

	if (this->IsLastLayer())
	{
		switch (param1)
		{
			case 0:
			case 1:
			{	
				if (role->IsLeader())
				{
					FBManager::Instance().LeaveFB(role);
				}
				/*else
				{
					TeamManager::Instance().ExitTeam(role);
					this->DelayKickOutRole(role);
				}*/
			}
			break;
			case 2:
			{
				if (role->IsLeader())
				{
					FBManager::Instance().LeaveFB(role);
				}
			}
			break;
		}
	}
}

void SpecialLogicFantasyFb::Update(unsigned long interval, time_t now_second)
{
}

void SpecialLogicFantasyFb::OnTransport(Role * role, SpecialTransport * transport_obj)
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

bool SpecialLogicFantasyFb::IsForceEncounterMineMonster()
{
	return true;
}

bool SpecialLogicFantasyFb::CanEncounterMineMonster(Role * role)
{
	return true;
}

bool SpecialLogicFantasyFb::OnOpenTreasureChest(Role * role, TreasureChest * treasure_chest_obj)
{
	if (NULL == treasure_chest_obj || NULL == role)
	{
		return false;
	}

	Team *team = role->GetMyTeam();

	Protocol::SCTreasureChestRewardList msg;
	msg.obj_id = treasure_chest_obj->GetId();
	if (team != NULL)
	{
		int team_count = team->GetMemberCount();
		for (int i = 0; i < MAX_TEAM_MEMBER_NUM && i < team_count; ++i)
		{
			Role * member_role = team->GetMemberRoleByIndex(i);
			if (member_role != NULL && member_role->IsFollow())
			{
				const FantasyFBGatherRewardCfg * box_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetGatherRewardCfg(member_role->GetLevel(), treasure_chest_obj->GetTreasureChestid());
				if (NULL == box_cfg) continue;

				msg.reward_count = 0;
				if (!member_role->GetRoleModuleManager()->GetCommonData().fantasy_fb_flag.IsBitSet(m_layer))
				{
					const FantasyFBRewardGroupCfg * reward_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetRewardGroupCfg(box_cfg->reward_id);
					if (reward_cfg == NULL) return false;
					//member_role->GetRoleModuleManager()->GetKnapsack()->Put(reward_cfg->reward, PUT_REASON_FANTASY_FB);
					this->ItemStack(member_role, &reward_cfg->reward);
					this->SendRewardInfo(member_role);

					msg.reward_list[msg.reward_count].is_bind = reward_cfg->reward.is_bind ? 1 : 0;
					msg.reward_list[msg.reward_count].item_id = reward_cfg->reward.item_id;
					msg.reward_list[msg.reward_count].num = reward_cfg->reward.num;
					msg.reward_count++;
				}

				EngineAdapter::Instance().NetSend(member_role->GetNetId(), (const char *)&msg, sizeof(msg) - (sizeof(msg.reward_list[0])*(Protocol::SCTreasureChestRewardList::MAX_TREASURE_CHEST_COUNT - msg.reward_count)));
				EventHandler::Instance().OnOpenTreasureChest(member_role);
			}
		}
	}
	else
	{
		const FantasyFBGatherRewardCfg * box_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetGatherRewardCfg(role->GetLevel(), treasure_chest_obj->GetTreasureChestid());
		if (NULL == box_cfg) return false;

		msg.reward_count = 0;
		if (!role->GetRoleModuleManager()->GetCommonData().fantasy_fb_flag.IsBitSet(m_layer))
		{
			const FantasyFBRewardGroupCfg * reward_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetRewardGroupCfg(box_cfg->reward_id);
			if (reward_cfg == NULL) return false;

			//role->GetRoleModuleManager()->GetKnapsack()->Put(reward_cfg->reward, PUT_REASON_FANTASY_FB);
			this->ItemStack(role, &reward_cfg->reward);
			this->SendRewardInfo(role);

			msg.reward_list[msg.reward_count].is_bind = reward_cfg->reward.is_bind ? 1 : 0;
			msg.reward_list[msg.reward_count].item_id = reward_cfg->reward.item_id;
			msg.reward_list[msg.reward_count].num = reward_cfg->reward.num;
			msg.reward_count++;

		}
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&msg, sizeof(msg) - (sizeof(msg.reward_list[0])*(Protocol::SCTreasureChestRewardList::MAX_TREASURE_CHEST_COUNT - msg.reward_count)));
		EventHandler::Instance().OnOpenTreasureChest(role);
	}

	std::map<int, std::vector<SpecialLogicFantasyObjInfo> >::iterator it = m_obj_info_list.find(Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_9);
	if (it != m_obj_info_list.end())
	{
		for (std::vector<SpecialLogicFantasyObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			if (it2->obj_id == treasure_chest_obj->GetId())
			{
				it->second.erase(it2);
				break;
			}
		}
	}
	//m_scene->DeleteObj(treasure_chest_obj->GetId());
	m_box_num--;
	this->SendSceneInfo();
	this->SendSceneObjInfo();

	return true;
}

void SpecialLogicFantasyFb::SetInfo(int layer, Posi transport_pos, std::map<int, int>* level_seq, std::map<int, std::vector<ItemConfigData> >* item_list, std::vector<Event_NPC_FB_Cfg> *has_create_npc_vec)
{
	if (layer <= 0)
	{
		return;
	}
	m_layer = layer;
	m_transport_pos = transport_pos;
	if (NULL != level_seq)
	{
		m_level_seq.swap(*level_seq);
	}
	if (NULL != item_list)
	{
		m_reward_item_list.swap(*item_list);
	}
	if (NULL != has_create_npc_vec)
	{
		m_has_create_npc_vec.assign(has_create_npc_vec->begin(), has_create_npc_vec->end());
	}
}

void SpecialLogicFantasyFb::SendSceneInfo(Role * role)
{
	Protocol::SCFantasyFbSceneInfo info;
	info.level_seq = m_level_seq[m_layer];
	info.layer = m_layer;
	info.kill_boss_flag = kill_boss_flag;
	info.box_num = m_box_num;
	info.m_box_num_max = m_box_num_max;
	info.gather_num = m_gather_num;
	info.m_gather_num_max = m_gather_num_max;

	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, sizeof(info));
	}
	else
	{
		m_scene->SendToRole((const char*)&info, sizeof(info));
	}

}

void SpecialLogicFantasyFb::SendSceneObjInfo(Role * role)
{
	Protocol::SCFantasyFbObjInfo info;
	info.obj_count = 0;
	for (std::map<int, std::vector<SpecialLogicFantasyObjInfo> >::iterator it = m_obj_info_list.begin(); it != m_obj_info_list.end(); it++)
	{
		if ((int)it->second.size() > 0 && info.obj_count < Protocol::SCFantasyFbObjInfo::MAX_OBJ_LIST_COUNT)
		{
			for (std::vector<SpecialLogicFantasyObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
			{
				info.obj_list[info.obj_count].npc_id = it2->npc_id;
				info.obj_list[info.obj_count].obj_type = (short)it->first;
				info.obj_list[info.obj_count].pos_x = it2->pos_x;
				info.obj_list[info.obj_count].pos_y = it2->pos_y;
				info.obj_count += 1;
			}
		}
	}

	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, sizeof(info) - ((Protocol::SCCrystalFbObjInfo::MAX_OBJ_LIST_COUNT - info.obj_count) * sizeof(info.obj_list[0])));
	}
	else
	{
		m_scene->SendToRole((const char*)&info, sizeof(info));
	}
}

void SpecialLogicFantasyFb::SendRewardInfo(Role * role)
{
	if(role == NULL) return;

	Protocol::SCFantasyFBRewardInfo info;
	info.op_type = Protocol::SCFantasyFBRewardInfo::FANTASY_FB_TYPE_REWARD;
	info.fantasy_finish_flag = role->GetRoleModuleManager()->GetCommonData().fantasy_fb_flag;
	info.reward_count = 0;

	std::map<int, std::vector<ItemConfigData> >::iterator it = m_reward_item_list.find(role->GetUID());
	if (it != m_reward_item_list.end())
	{
		if ((int)it->second.size() > 0)
		{
			for (std::vector<ItemConfigData>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
			{
				info.reward_list[info.reward_count].item_id = it2->item_id;
				info.reward_list[info.reward_count].is_bind = it2->is_bind;
				info.reward_list[info.reward_count].num = it2->num;
				info.reward_count++;
			}
		}
	}

	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, sizeof(info) - ((Protocol::SCFantasyFBRewardInfo::MAX_ITEM_DATA_NUM - info.reward_count) * sizeof(info.reward_list[0])));
}

void SpecialLogicFantasyFb::CheckFirstPassFB(Role * role)
{
	if(NULL == role) return;

	if(!kill_boss_flag || role->GetRoleModuleManager()->GetCommonData().fantasy_fb_flag.IsBitSet(m_layer)) return;

	const FantasyFbKillBossRewardCfg * kill_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetRewardCfg(role->GetLevel());
	if(NULL == kill_cfg || m_layer > kill_cfg->count) return;

	const FantasyFBRewardCfg & reward_cfg = kill_cfg->node[m_layer - 1];
	for (int i = 0; i < (int)reward_cfg.reward.size(); i++)
	{
		this->ItemStack(role, &reward_cfg.reward[i]);
	}
	
	role->GetRoleModuleManager()->GetMoney()->AddCoinBind(reward_cfg.bind_coin, __FUNCTION__);
	long long real_exp = role->AddExp(reward_cfg.reward_exp, ADD_EXP_TYPE_FANTASY_FB);
	role->GetRoleModuleManager()->GetLevelComplement()->OnAddExp(real_exp);
	EventHandler::Instance().OnFinishLevelComplementRelatedFun(role, reward_cfg.reward_exp, __FUNCTION__);
	role->GetRoleModuleManager()->GetCommonData().fantasy_fb_flag.SetBit(m_layer);
	EventHandler::Instance().OnFantasyFBReferReward(role);
}

void SpecialLogicFantasyFb::SendNotifyGetItemList(Role * role, int notice)
{
	if(NULL == role || !kill_boss_flag) return;

	bool flag = false;
	int count = 0;
	ItemDataWrapper item_info[MAX_ATTACHMENT_ITEM_NUM];
	std::map<int, std::vector<ItemConfigData> >::iterator it = m_reward_item_list.find(role->GetUID());
	if (it != m_reward_item_list.end())
	{
		for (std::vector<ItemConfigData>::iterator item_it = it->second.begin(); item_it != it->second.end(); item_it++)
		{
			bool is_flag = false;
			if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(item_it->item_id, item_it->num))
			{
				if (!role->GetRoleModuleManager()->GetKnapsack()->Put(*item_it, PUT_REASON_FANTASY_FB))
				{
					is_flag = true;
				}
			}
			else
			{
				is_flag = true;
			}
			if (is_flag)
			{
				item_info[count].item_id = item_it->item_id;
				item_info[count].is_bind = item_it->is_bind;
				item_info[count].num = item_it->num;
				flag = true;
				count++;
			}
			if (count == MAX_ATTACHMENT_ITEM_NUM)
			{
				this->SendMail(role, item_info, count);
				count = 0;
			}		
		}
		role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)m_reward_item_list[role->GetUID()].size(), m_reward_item_list[role->GetUID()].empty() ? NULL : &m_reward_item_list[role->GetUID()][0], notice);
		m_reward_item_list.erase(it);
	}
	else
	{
		role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)m_reward_item_list[role->GetUID()].size(), m_reward_item_list[role->GetUID()].empty() ? NULL : &m_reward_item_list[role->GetUID()][0], notice);
	}

	if (count != 0)
	{
		this->SendMail(role, item_info, count);
	}

	if (flag)
	{
		role->NoticeNum(noticenum::NT_KNAPSACKFULL_SEND_MAIL);
	}
	this->SendRewardInfo(role);
}

void SpecialLogicFantasyFb::CreateNpcOrBox()
{
	const FantasyFBSceneCfg * cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetSceneCfgBySeq(m_level_seq[m_layer]);
	if (NULL == cfg) return;

	m_gather_num = 0;
	m_box_num = 0;
	std::vector<Event_NPC_FB_Cfg> npc_node_cfg;
	int num = LOGIC_CONFIG->GetFantasyFbConfig()->GetEventNPCFBCfg(cfg->event_group_id, cfg->event_num, &npc_node_cfg, &m_obj_info_list);
	for (int i = 0; i < num && i < cfg->event_num; i++)
	{
		switch (npc_node_cfg[i].npc_type)
		{
			case Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_6:
			case Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_7:
			case Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_8:
			{
				const ServerNPCInfoCfg *  s_cfg = NPCPOOL->GetServerNPCInfo(npc_node_cfg[i].npc_id);
				if (s_cfg != NULL)
				{
					if (s_cfg->server_npc_type == SERVER_NPC_TYPE_GATHERS)
					{
						NPCGather * gather = NpcManager::Instance().CreateNPCGather(npc_node_cfg[i].npc_id, m_scene, m_scene->GetARandPosi());
						if (gather == NULL)continue;
						SpecialLogicFantasyObjInfo node_info;
						node_info.obj_id = gather->GetId();
						node_info.npc_id = gather->GetNpcId();
						node_info.pos_x = gather->GetPos().x;
						node_info.pos_y = gather->GetPos().y;

						switch (npc_node_cfg[i].npc_type)
						{
						case Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_6:
						{
							node_info.obj_type = Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_6;
							m_obj_info_list[Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_6].push_back(node_info);
							m_gather_num++;
						}
						break;
						case Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_7:
						{
							node_info.obj_type = Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_7;
							m_obj_info_list[Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_7].push_back(node_info);
							m_gather_num++;
						}
						break;
						case Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_8:
						{
							node_info.obj_type = Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_8;
							m_obj_info_list[Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_8].push_back(node_info);
							m_gather_num++;
						}
						break;
						default:
							break;
						}
					}
				}
			}
				break;
			case Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_9:
			{
				const NPCTreasureChestInfoCfg * s_box_cfg = NPCPOOL->GetTreasureChestCfg(npc_node_cfg[i].npc_id);
				if (s_box_cfg != NULL)
				{
					TreasureChest  * treasure_chest_obj = new TreasureChest(npc_node_cfg[i].npc_id);
					treasure_chest_obj->SetPos(m_scene->GetARandPosi());
					m_scene->AddObj(treasure_chest_obj);

					SpecialLogicFantasyObjInfo node_info;
					node_info.npc_id = npc_node_cfg[i].npc_id;
					node_info.obj_id = treasure_chest_obj->GetId();
					node_info.obj_type = Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_9;
					node_info.pos_x = treasure_chest_obj->GetPos().x;
					node_info.pos_y = treasure_chest_obj->GetPos().y;
					m_obj_info_list[Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_9].push_back(node_info);
					m_box_num++;
				}
			}
				break;
			default:
				break;		
		}
	}
}

void SpecialLogicFantasyFb::CreateMonster()
{
	const FantasyFBSceneCfg * cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetSceneCfgBySeq(m_level_seq[m_layer]);
	if(NULL == cfg) return;

	NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(cfg->boss_id, m_scene, cfg->boss_pos, BATTLE_MODE_FANTASY_FB, &cfg->monster_dir);
	if (NULL != monster)
	{
		SpecialLogicFantasyObjInfo node_info;
		node_info.npc_id = monster->GetNpcId();
		node_info.obj_id = monster->GetId();
		node_info.obj_type = Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_3;
		node_info.pos_x = monster->GetPos().x;
		node_info.pos_y = monster->GetPos().y;
		m_obj_info_list[Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_3].push_back(node_info);
		m_boss_id = cfg->boss_id;
		kill_boss_flag = 0;
	}
}

void SpecialLogicFantasyFb::CreateTransport(const Posi & pos)
{
	SpecialTransport * transport_obj = new SpecialTransport();
	if (transport_obj != NULL)
	{
		transport_obj->SetPos(gamecommon::GetDisRandPos(pos, 5));
		transport_obj->GetExtraInfo().send_param1 = m_layer;
		transport_obj->GetExtraInfo().send_param2 = m_level_seq[m_layer];
		m_scene->AddObj(transport_obj);

		SpecialLogicFantasyObjInfo node_info;
		node_info.npc_id = 0;
		node_info.obj_id = transport_obj->GetId();
		node_info.obj_type = Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_10;
		node_info.pos_x = transport_obj->GetPos().x;
		node_info.pos_y = transport_obj->GetPos().y;
		m_obj_info_list[Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_10].push_back(node_info);
		this->SendSceneObjInfo();
	}
}

bool SpecialLogicFantasyFb::IsLastLayer(int layer)
{
	if (layer == 0)
	{
		layer = m_layer;
	}
	
	return layer >= (int)m_level_seq.size();
}

void SpecialLogicFantasyFb::TransportNextLayer(Role * role, SpecialTransport * transport_obj)
{
	if (NULL == role || NULL == transport_obj)
	{
		return;
	}
	const FantasyFBSceneCfg * scene_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetSceneCfgBySeq(m_level_seq[m_layer]);
	if (NULL == scene_cfg)	return;
	
	int scene_id = scene_cfg->scene_id;
	int scene_key = m_scene->GetSceneKey();
	Posi target_pos(0, 0);
	Posi transport_pos(0, 0);

	int next_layer = m_layer + 1;

	{
		const FantasyFBSceneCfg * next_scene_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetSceneCfgBySeq(m_level_seq[next_layer]);
		if(NULL == next_scene_cfg) return;

		scene_id = next_scene_cfg->scene_id;
		target_pos = next_scene_cfg->birth_pos;
		transport_pos = next_scene_cfg->transport_pos;
	}
	Team * team = role->GetMyTeam();
	if (NULL != team)
	{
		if(!team->IsLeader(role)) return;

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
	SpecialLogicFantasyFb * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicFantasyFb(scene_id, scene_key);
	if (special_lobgic == NULL)
	{
		return;
	}
	special_lobgic->SetInfo(m_layer + 1, transport_pos, &m_level_seq, &m_reward_item_list, &m_has_create_npc_vec);

	if (team != NULL)
	{
		this->TeamGoNextLayer(team, scene_id, scene_key, target_pos);
	}
	else
	{
		World::GetInstWorld()->GetSceneManager()->GoTo(role, scene_id, scene_key, target_pos);
	}
}

void SpecialLogicFantasyFb::SendMail(Role * role, ItemDataWrapper * item_info, int count)
{
	if(role == NULL) return;

	static MailContentParam contentparam; contentparam.Reset();
	for (int i = 0; i < count && i < MAX_ATTACHMENT_ITEM_NUM; i++)
	{
		contentparam.item_list[i] = item_info[i];
	}
	SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_knapsack_full_mail_subject);
	SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_knapsack_full_send_mail);
	MailRoute::MailToUser(role->GetUID(), MAIL_REASON_DEFAULT, contentparam);
}

void SpecialLogicFantasyFb::ItemStack(Role * role, const ItemConfigData * item)
{
	if(NULL == role || NULL == item) return;
	
	std::map<int, std::vector<ItemConfigData> >::iterator it= m_reward_item_list.find(role->GetUID());
	if(it == m_reward_item_list.end()) 
	{
		m_reward_item_list[role->GetUID()].push_back(*item);
		return;
	}

	for (std::vector<ItemConfigData>::iterator item_it = it->second.begin(); item_it != it->second.end(); item_it++)
	{
		if (item_it->item_id == item->item_id && item_it->is_bind == item->is_bind)
		{
			item_it->num += item->num;
			return;
		}
	}

	m_reward_item_list[role->GetUID()].push_back(*item);

	return;
}

void SpecialLogicFantasyFb::TeamGoNextLayer(Team * team, int target_scene_id, int target_scene_key, const Posi & target_pos)
{	
	if(NULL == team) return;
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
			if (NULL == role) continue;
			 
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

void SpecialLogicFantasyFb::ClearSceneBox()
{
	std::map<int, std::vector<SpecialLogicFantasyObjInfo> >::iterator it = m_obj_info_list.find(Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_9);
	if (it != m_obj_info_list.end())
	{
		for (std::vector<SpecialLogicFantasyObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end();)
		{
			if (m_scene->DeleteObj(it2->obj_id))
			{
				m_box_num--;
			}
			it2 = it->second.erase(it2);
		}
	}
}

