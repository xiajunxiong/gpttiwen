#include "scene/scene.h"
#include "obj/character/npc_monster.h"
#include "obj/character/robot.h"
#include "npc/npcmanager.h"
#include "other/rolescenestatus.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "global/team/team.hpp"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "gameworld/gameworld/item/itempool.h"
#include "item/expense/norexitem.hpp"
#include "obj/otherobj/specialtransport.h"
#include "obj/otherobj/treasurechest.h"
#include "battle/battle_manager_local.hpp"
#include "drop/droppool.hpp"
#include "monster/monster_group_pool.hpp"

#include "other/rolecrystalfb/rolecrystalfb.h"
#include "protocol/msgcrystalfb.h"
#include "servercommon/rolecrystalfbdef.h"
#include "speciallogiccrystalfb.hpp"
#include "other/event/eventhandler.hpp"
#include "scene/fbmanager.hpp"
#include "gameworld/protocol/msgother.h"
#include "obj/character/npc_chosed.h"
#include "gamelog.h"
#include "obj/character/npc_gather.h"
#include "servercommon/noticenum.h"
#include "gameworld/other/buffmanager/buffmanager.hpp"
#include "other/buffmanager/buffbase.hpp"
#include "gameworld/engineadapter.h"
#include "servercommon/string/gameworldstr.h"
#include "other/pet/pet.hpp"
#include "other/partner/partner.hpp"
#include "global/team/teammanager.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitydropdoubleconfig.hpp"
#include "other/prestige/prestige.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitytujiantreasure.hpp"
#include "config/otherconfig/knapsack_config.hpp"
#include "other/roledrop/roledrop.hpp"
#include "other/workshop/workshop.hpp"
#include "other/robot/robotmanager.hpp"
#include "global/drawmanager/drawmanager.hpp"
#include "protocol/msgdraw.h"
#include "world.h"
#include "other/levelcomplement/levelcomplement.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "other/activesystem/activesystem.hpp"

SpecialLogicCrystalFB::SpecialLogicCrystalFB(Scene* scene) : SpecialLogic(scene), m_is_init(false), m_layer(false), m_level_seq(0), m_monster_num(0),
	m_box_num(0), m_npc_num(0), m_talk_npc_num(0), m_boss_obj_id(INVALID_OBJ_ID), m_gouhuo_num(0), m_coin_num(0), m_gather_num(0), m_monster_num_max(0),
	m_box_num_max(0), m_npc_num_max(0), m_gouhuo_num_max(0), m_coin_num_max(0), m_gather_num_max(0), m_open_box_consume_uid(0), m_team_max_level(0),
	m_battle_mode(BATTLE_MODE_CRYSTAL_FB), m_normal_monster_battle_mode(BATTLE_MODE_CRYSTAL_FB)
{
	consumable_player = 0;
	m_transport_pos = Posi();
	m_consume_uid_list.clear();
	m_already_rand_scene_id.insert(m_scene->GetSceneID());
}

SpecialLogicCrystalFB::~SpecialLogicCrystalFB()
{

}

void SpecialLogicCrystalFB::Update(unsigned long interval, time_t now_second)
{

}

void SpecialLogicCrystalFB::OnRoleEnterScene(Role *role)
{
	if (NULL == role) return;

	role->SetEncounterMonsterFlag(true);

	this->SendSceneInfo(role);
	this->SendAllSceneObjInfo(role);
	this->SendUseItemInfo(role);

	EventHandler::Instance().OnRoleEnterFB(role, FBManager::FB_TYPE_CRYSTAL_FB, m_level_seq);
}

void SpecialLogicCrystalFB::OnRoleLeaveScene(Role *role, bool is_logout)
{
	role->RestoreLastEncounterMonsterFlag();
}

void SpecialLogicCrystalFB::OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack)
{
	if (IsBattleModeBelongToCrystalFB(ack->battle_mode) && 
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}

	if (IsBattleModeBelongToCrystalFB(ack->battle_mode) && RESULT_TYPE_ATTACKER_WIN == ack->battle_result)
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

			const RoleCrystalFbLevelCfg  * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(m_level_seq);
			int monster_type = Protocol::SCCrystalFbObjInfo::CRYSTAL_FB_OBJ_TYPE_MONSTER;
			bool is_boss = npc_monster->GetId() == m_boss_obj_id;
			if (cfg != NULL)
			{
				DrawGroupInfo draw_group_info;
				int tmp_role_count = 0;
				Role* tmp_role_list[SIDE_MAX_ROLE_NUM] = { 0 };
				Protocol::SCDrawPokerInfo info;
				DrawCrystalPoker crystal_poker_info[SIDE_MAX_ROLE_NUM];
				if (is_boss)
				{
					for (int ind = 0; ind < SIDE_MAX_ROLE_NUM && ind < DRAW_GROUP_MAX_ITEM_NUM; ind++)
					{
						draw_group_info.draw_count++;
						//无人获得的奖励 拥有者为0
						int role_uid = 0;

						if (ind < ack->attacker_list.role_count && 0 == ack->attacker_list.role_list[ind].is_flyaway && 0 == ack->attacker_list.role_list[ind].is_runaway)
						{
							role_uid = ack->attacker_list.role_list[ind].uid;
						}

						DrawGroupInfo::DrawItem &reward_info = draw_group_info.draw_info_list[ind];
						reward_info.role_uid = role_uid;

						GameName &role_name = crystal_poker_info[ind].role_name;
						Role * role = m_scene->GetRoleByUID(role_uid);
						if (NULL == role)
						{
							const ItemConfigData * item_data = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandNpcReward(cfg->poker_reward_group_vec[0]);
							if (NULL != item_data)
							{
								DrawManager::Instance().PutRewardItem(reward_info, 1, item_data);
							}

							if (!reward_info.item_list.empty())
							{
								DrawManager::Instance().GetItemInfoType(crystal_poker_info[ind].reward_info, reward_info.item_list[0]);
							}

							//普通副本没有高额次数
							/*	if (role_uid > 0 && !RobotManager::IsRobot(role_uid) && ROLE_CRYSTAL_FB_TYPE_NORMAL == cfg->type)
								{
									OfflineRegisterManager::Instance().OnNormalCrystalFBGetBossReward(role_uid, cfg->seq);
								}*/

							//机器人
							Robot *robot = RobotManager::Instance().GetRobot(role_uid);
							if (NULL != robot)
							{
								robot->GetName(role_name);
							}
							else
							{
								memset(role_name, 0, sizeof(role_name));
							}
							reward_info.role_uid = 0;
						}
						else
						{
							//角色名
							role->GetName(role_name);
							if (cfg->type == ROLE_CRYSTAL_FB_TYPE_XI_YOU && !role->GetRoleModuleManager()->GetRoleCrystalFb()->HasUseTimes())
							{
								//不能获得翻牌奖励
								//移除uid,清空玩家名字
								reward_info.role_uid = 0;
								memset(role_name, 0, sizeof(role_name));
							}
							if (cfg->type == ROLE_CRYSTAL_FB_TYPE_JING_YING && !this->IsHasConsume(role->GetUID()))
							{
								//精英副本没有消耗封印石不能获得翻牌奖励
								reward_info.role_uid = 0;
								memset(role_name, 0, sizeof(role_name));
								role->NoticeNum(errornum::EN_CRYSTAL_PASS_LEVEL_NOT_HAS_CONSUME);
							}
							tmp_role_list[tmp_role_count] = role;
							tmp_role_count++;

							std::vector<ItemConfigData> reward_list;
							const ItemConfigData * item_data = NULL;
							//活动 - 掉落翻倍
							int drop_double_num = 1;
							RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
							if (NULL != ramgr)
							{
								if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_DROP_DOUBLE))
								{
									const RandActivityDropDoubleConfig * config = static_cast<const RandActivityDropDoubleConfig *>
										(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_DROP_DOUBLE));
									if (NULL != config)
									{
										const DropDoubleCfg * drop_double_cfg = config->GetDropPeomoteCfg(ramgr, cfg->seq);
										if (NULL != drop_double_cfg)
										{
											drop_double_num = drop_double_cfg->drop_promote;
										}
									}
								}
							}
							//检测首杀
							if (role->GetRoleModuleManager()->GetRoleCrystalFb()->HasFristPassCrystalFb(m_level_seq))
							{
								int index = 0;
								if (ROLE_CRYSTAL_FB_TYPE_NORMAL == cfg->type)index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(0) - 1;
								
								if (ROLE_CRYSTAL_FB_TYPE_JING_YING == cfg->type)
								{
									//目前队员队长,只要消耗了封印石头,就不再衰减奖励
									//int temp_play_times = role->GetUID() == consumable_player ? 0 : role->GetRoleModuleManager()->GetRoleCrystalFb()->GetJingYingPlayTimes();
									index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(0) - 1;
								}
								if (index < 0 || index >= (int)cfg->poker_reward_group_vec.size())
								{
									continue;
								}
								item_data = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandNpcReward(cfg->poker_reward_group_vec[index]);
							}
							else
							{
								item_data = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandNpcReward(cfg->first_poker_reward_group_id);
							}
							if (NULL != item_data)
							{
								reward_list.push_back(*item_data);
								if (drop_double_num > 1)
								{
									reward_list[(int)reward_list.size() - 1].num *= drop_double_num;
								}
								DrawManager::Instance().PutRewardItem(reward_info, (int)reward_list.size(), &reward_list[0], role->GetAppearanceProf());
							}

							if (!reward_info.item_list.empty())
							{
								DrawManager::Instance().GetItemInfoType(crystal_poker_info[ind].reward_info, reward_info.item_list[0]);
							}

							role->GetRoleModuleManager()->GetRoleCrystalFb()->OnRoleGetFBBossReward(cfg->seq);
						}
						//协议信息
						crystal_poker_info[ind].index = reward_info.draw_index;
						crystal_poker_info[ind].role_uid = reward_info.role_uid;
					}
				}
				for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
				{	
					Role * temp_role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
					if (NULL == temp_role)
					{
						continue;
					}
					bool is_has_activity_reward = true;
					int village_id = 0;
					int prestige_num = 0;
					int add_exp = 0;
					std::vector<ItemConfigData> must_has_reward;	
					std::vector<int> drop_id_list = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetMonsterDropId(npc_monster->GetNpcId(), 0, &village_id, &prestige_num, &add_exp, &must_has_reward);
					if (cfg->type == ROLE_CRYSTAL_FB_TYPE_NORMAL)
					{
						drop_id_list = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetMonsterDropId(npc_monster->GetNpcId(), 0);
					}
					else if (cfg->type == ROLE_CRYSTAL_FB_TYPE_JING_YING)
					{
						Team * role_team = temp_role->GetMyTeam();
						int times = 0;
						if (NULL != role_team && !this->IsHasConsume(temp_role->GetUID()))	//没有消耗封印石头,没有奖励(宝箱不会进入这里)
						{
							times = -1;
							drop_id_list.clear();
							village_id = 0;
							prestige_num = 0;
							add_exp = 0;
							is_has_activity_reward = false;
						}
						/*int times = temp_role->GetUID() == consumable_player ? 0 : temp_role->GetRoleModuleManager()->GetRoleCrystalFb()->GetJingYingPlayTimes();*/
						if (times >= 0)
						{
							drop_id_list = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetMonsterDropId(npc_monster->GetNpcId(), times, &village_id, &prestige_num, &add_exp, &must_has_reward);
						}
					/*	is_has_activity_reward = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(temp_role->GetRoleModuleManager()->GetRoleCrystalFb()->GetJingYingPlayTimes()) == 1 ? true : false;*/
					}
					else if (cfg->type == ROLE_CRYSTAL_FB_TYPE_XI_YOU)
					{
						drop_id_list.clear();//先清除
						if (temp_role->GetRoleModuleManager()->GetRoleCrystalFb()->HasUseTimes())
						{
							drop_id_list = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetMonsterDropId(npc_monster->GetNpcId(), 0);
							temp_role->GetRoleModuleManager()->GetRoleCrystalFb()->UseXiYouRewardPlayTimes();
						}
						else
						{
							is_has_activity_reward = false;
						}
						temp_role->GetRoleModuleManager()->GetRoleCrystalFb()->UseXiYouPlayTimes();
					}
					if (ROLE_CRYSTAL_FB_TYPE_NORMAL == cfg->type)
					{
						add_exp = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetExpCfg(temp_role->GetLevel()) * LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg().use_power;
					}

					if (village_id != 0 && prestige_num > 0)temp_role->GetRoleModuleManager()->GetPrestige()->AddPrestige(village_id, prestige_num, __FUNCTION__);
					//是否首通
					if (is_boss)
					{
						this->CheckFristPassFb(temp_role, &drop_id_list);
						if (cfg->type == ROLE_CRYSTAL_FB_TYPE_XI_YOU)temp_role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_CRYSTAL_XIYOU, 1, __FUNCTION__);
					}

					//活动 - 掉落翻倍
					int drop_double_num = 1;
					RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(temp_role->GetUniqueServerID());
					if (NULL != ramgr)
					{
						if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_DROP_DOUBLE))
						{
							const RandActivityDropDoubleConfig * config = static_cast<const RandActivityDropDoubleConfig *>
								(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_DROP_DOUBLE));
							if (NULL != config)
							{
								const DropDoubleCfg * drop_double_cfg = config->GetDropPeomoteCfg(ramgr, cfg->seq);
								if (NULL != drop_double_cfg)
								{
									drop_double_num = drop_double_cfg->drop_promote;
								}
							}
						}
					}

					std::vector<ItemConfigData> item_list;
					if (is_boss)
					{
						EventHandler::Instance().OnKillCrystalBoss(temp_role, npc_monster->GetNpcId(), &item_list, is_has_activity_reward);
						EventHandler::Instance().OnPassCrystalEctype(temp_role, cfg->type, npc_monster->GetNpcId());
						EventHandler::Instance().OnRolePassCrystalFB(temp_role, cfg->seq);
						EventHandler::Instance().OnRolePassCrystalFB2(temp_role, cfg->seq, cfg->type, cfg->fb_index);
						for (std::vector<ItemConfigData>::const_iterator it = item_list.begin(); it != item_list.end(); ++it)
						{
							this->ItemStack(temp_role, &(*it));
						}
					}
					if (add_exp > 0)
					{
						temp_role->AddExp(add_exp, ADD_EXP_TYPE_CRYSTAL_FB, __FUNCTION__);
						temp_role->GetRoleModuleManager()->GetLevelComplement()->OnAddExp(add_exp);
						EventHandler::Instance().OnFinishLevelComplementRelatedFun(temp_role, add_exp, __FUNCTION__);
					}
					if (must_has_reward.size() > 0)
					{
						item_list.insert(item_list.end(), must_has_reward.begin(), must_has_reward.end());
					}
					DropPool::Instance()->DropListToRole(temp_role, drop_id_list, ack->monster_group_id, true, npc_monster->GetId() == m_boss_obj_id ?
						GET_REASON_CRYSTAL_FB_BOSS_REWARD : GET_REASON_INVALID, drop_double_num, &item_list, PUT_REASON_CRYSTAL_FB, add_exp);

					if (is_boss)
					{
						//图鉴夺宝活动 给完掉落之后检测活动
						if (NULL != ramgr && ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE))
						{
							RoleActivityTuJianTreasure * role_activity = static_cast<RoleActivityTuJianTreasure *>
								(temp_role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE));
							if (NULL != role_activity)
							{
								if (role_activity->IsRoleTuJianTreasureOpen())
								{
									role_activity->OnTuJianTreasureCheckReward(TU_JIAN_TREASURE_TYPE_FB, cfg->seq, m_item_list[temp_role->GetUID()]);
								}
							}

							std::map<int, std::vector<ItemConfigData> >::iterator i_it = m_item_list.find(temp_role->GetUID());
							if (i_it != m_item_list.end())
							{
								m_item_list.erase(i_it);
							}
						}

						const RoleCrystalFbLevelCfg * role_cry_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(m_level_seq);
						if (NULL != role_cry_cfg)
						{
							const std::vector<WorkshopItemConfig> * workshop_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetWorkshopItemCfg(role_cry_cfg->formula_drop_group);
							if (NULL != workshop_cfg)
							{
								int rand_weight = rand() % WorkshopItemConfig::MAX_RAND_WEIGHT_NUM;
								int weight_count = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetWorkshopItemWeightCfg(role_cry_cfg->formula_drop_group);
								if (rand_weight <= weight_count)
								{
									for (std::vector<WorkshopItemConfig>::const_iterator it = workshop_cfg->begin(); it != workshop_cfg->end(); it++)
									{
										if (it->rate >= rand_weight)//得到奖励
										{
											for (int prof_index = 0; prof_index < PROFESSION_TYPE_NUM; prof_index++)
											{
												if(PROF_TYPE_INVALID == prof_index) continue;

												int new_item = LOGIC_CONFIG->GetKnapsackConfig()->GetItemIdConvertByJob(it->item_id, prof_index);
												const ItemBase * gift_itembase = ITEMPOOL->GetItem(new_item);
												if (gift_itembase != NULL)
												{
													if (gift_itembase->GetItemType() == I_TYPE_EXPENSE)
													{
														const NorexItem * norex_item = (NorexItem *)gift_itembase;
														if (norex_item->GetUseType() == NorexItem::I_NOREX_ITEM_WORKSHOP_RECIPE)
														{
															//检查是否已经获得这个奖励了
															if (!temp_role->GetRoleModuleManager()->GetWorkshop()->RecipeIsActive(norex_item->GetParam1()))
															{
																if (!temp_role->GetRoleModuleManager()->GetRoleDrop()->IsLockReceipeDrop(gift_itembase->GetItemId()))
																{
																	bool is_notice = temp_role->GetAppearanceProf() == prof_index ? true : false;
																	//自己职业的才提示
																	temp_role->GetRoleModuleManager()->GetKnapsack()->Put(ItemConfigData(gift_itembase->GetItemId(), 1, 1), PUT_REASON_CRYSTAL_FB, 0, 0, 0, 0, is_notice);
																	if (is_notice)
																	{
																		Protocol::SCSystemMsg sm;
																		int sendlen = 0;
																		Team* temp_team = temp_role->GetMyTeam();
																		if (temp_team != NULL)
																		{
																			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_crytals_reward_content, temp_role->GetName(), new_item);
																			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
																			{
																				for (int i = 0; i < temp_team->GetMemberCount(); ++i)
																				{
																					Role* member_role = temp_team->GetMemberRoleByIndex(i);
																					if (NULL == member_role || member_role->GetUID() == temp_role->GetUID()) continue;
																					EngineAdapter::Instance().NetSend(member_role->GetNetId(), (const char*)&sm, sendlen);
																				}
																			}
																		}
																		//获得者
																		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_crytals_reward2_content, new_item);
																		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
																		{
																			EngineAdapter::Instance().NetSend(temp_role->GetNetId(), (const char*)&sm, sendlen);
																		}
																	}
																}
															}
														}
													}
												}
											}
											break;
										}
										rand_weight -= it->rate;
									}
								}
							}
						}
					}
				}
				if (is_boss)	
				{
					DrawManager::Instance().AddDrawGroup(draw_group_info, DRAW_GROUP_TYPE_CRYSTAL);
					info.draw_group_id = draw_group_info.draw_group_id;
					info.timeout = draw_group_info.out_time;
					info.draw_group_type = DRAW_GROUP_TYPE_CRYSTAL;
					for (int k = 0; k < SIDE_MAX_ROLE_NUM; k++)
					{
						memcpy(info.draw_poker[k].data, &crystal_poker_info[k], sizeof(DrawCrystalPoker));
					}

					for (int i = 0; i < tmp_role_count && i < SIDE_MAX_ROLE_NUM; i++)
					{
						if (NULL == tmp_role_list[i])
						{
							continue;
						}
						EngineAdapter::Instance().NetSendSerializeMsg2(tmp_role_list[i]->GetNetId(), &info);
					}
				}
			}
			if (is_boss)
			{
				m_boss_obj_id = INVALID_OBJ_ID;
				monster_type = Protocol::SCCrystalFbObjInfo::CRYSTAL_FB_OBJ_TYPE_BOSS;
				this->CreateLeaveTransport(npc_monster->GetPos());
			}

			std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.find(monster_type);
			if (it != m_obj_info_list.end())
			{
				for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
				{
					if (it2->obj_id == npc->GetId())
					{
						//删除allobj列表
						for (std::vector<SpecialLogicCrystalObjInfo>::iterator all_it = all_npc_list.begin(); all_it != all_npc_list.end(); all_it++)
						{
							if (all_it->obj_id == it2->obj_id)
							{
								all_it->obj_id = INVALID_OBJ_ID;
								break;
							}
						}
						it->second.erase(it2);
						break;
					}
				}
			}

			m_scene->DeleteObj(npc->GetId());
			if (monster_type != Protocol::SCCrystalFbObjInfo::CRYSTAL_FB_OBJ_TYPE_BOSS)
			{
				m_monster_num -= 1;
			}
			this->SendSceneObjInfo();
			this->SendSceneInfo();
			this->SendAllSceneObjInfo();
		}
		else if (NULL != npc && Obj::OBJ_TYPE_NPC_CHOSED == npc->GetObjType())
		{
			NPCChosed * npc_chose = (NPCChosed*)npc;
			if (npc->GetScene()->GetSceneID() != m_scene->GetSceneID() ||
				npc->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
			{
				return;
			}

			const CrystalFbNPCRewardCfg * npc_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetNpcRewardCfg(npc_chose->GetNpcId(), ack->business_data.param1);
			if (NULL != npc_cfg)
			{
				for (int i = 0; i < ack->attacker_list.role_count; ++i)
				{
					Role * temp_role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
					if (NULL != temp_role)
					{
						this->RoleAloneGetRewardByCfg(temp_role, npc_cfg);
					}

				}

				if (npc_cfg->is_delete == 1)
				{
					std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.find(Protocol::SCCrystalFbObjInfo::CRYSTAL_FB_OBJ_TYPE_NPC);
					if (it != m_obj_info_list.end())
					{
						for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
						{
							if (it2->obj_id == npc->GetId())
							{
								//删除allobj列表
								for (std::vector<SpecialLogicCrystalObjInfo>::iterator all_it = all_npc_list.begin(); all_it != all_npc_list.end(); all_it++)
								{
									if (all_it->obj_id == it2->obj_id)
									{
										all_it->obj_id = INVALID_OBJ_ID;
										break;
									}
								}
								it->second.erase(it2);
								break;
							}
						}
					}
					m_scene->DeleteObj(npc->GetId());
					m_npc_num -= 1;
					this->SendSceneObjInfo();
					this->SendAllSceneObjInfo();
				}
			}
			this->SendSceneInfo();
			this->SendAllSceneObjInfo();
		}
		else if(0 != m_open_box_consume_uid)
		{
			const MonsterGroupCfg* monster_cfg = MonsterGroupPool::GetInstance().GetMonsterGroupCfg(ack->business_data.monster_group_id);
			const RoleCrystalFbLevelCfg * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(m_level_seq);
			if (NULL == monster_cfg || NULL == cfg || cfg->type != ROLE_CRYSTAL_FB_TYPE_JING_YING)
			{
				m_open_box_consume_uid = 0;
				return;
			}
			for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
			{
				Role * temp_role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
				if (NULL == temp_role)
				{
					continue;
				}

				std::vector<UInt16> drop_id_list;
				Team * role_team = temp_role->GetMyTeam();
				if (NULL != role_team)
				{	
					if (m_open_box_consume_uid == temp_role->GetUID())	//开宝箱拿的是队长奖励
					{
						drop_id_list = monster_cfg->captain_drop_list;
					}
					else
					{
						if (this->IsHasConsume(temp_role->GetUID()))
						{
							drop_id_list = monster_cfg->dropid_list;
						}
						else
						{
							drop_id_list = monster_cfg->decay_drop_list;
						}
					}
				}
				else
				{
					drop_id_list = monster_cfg->captain_drop_list;
				}
				bool drop_succ = false;
				for (size_t i = 0; i < drop_id_list.size(); ++i)
				{
					if (DropPool::Instance()->DropToRole(temp_role, drop_id_list[i], monster_cfg->monster_group_id, true))
					{
						drop_succ = true;
					}
				}
				if (drop_succ && -1 == monster_cfg->drop_item_id_checker) // 只要掉落成功，不管掉落什么都算一次
				{
					temp_role->GetRoleModuleManager()->GetRoleDrop()->AddMonsterGroupDrop(monster_cfg->monster_group_id);
				}
			}
		}
	}
	m_open_box_consume_uid = 0;
}

void SpecialLogicCrystalFB::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack)
{
	if (IsBattleModeBelongToCrystalFB(ack->battle_mode) &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}
}

void SpecialLogicCrystalFB::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack)
{
	if (BATTLE_MODE_CRYSTAL_FB == ack->battle_mode &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}
}

bool SpecialLogicCrystalFB::IsForceEncounterMineMonster()
{
	return !this->IsLastLayer();
}

bool SpecialLogicCrystalFB::CanEncounterMineMonster(Role * role)
{
	return !this->IsLastLayer();
}

bool SpecialLogicCrystalFB::CanUseItem(Role * role, ItemID item_id, int num)
{
	const ItemBase *item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base)
	{
		return false;
	}

	if (item_base->GetItemType() == I_TYPE_EXPENSE)
	{
		NorexItem * norex_item = (NorexItem *)item_base;
		int use_type = norex_item->GetUseType();

		std::map<int, SpecialLogicCrystalUseItemInfo>::iterator iter = m_role_id_to_use_info.find(role->GetUID());
		if (iter != m_role_id_to_use_info.end())
		{
			const RoleCrystalFbLevelCfg  * level_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(m_level_seq);
			std::map<int, int>::iterator use_times_iter = iter->second.use_type_to_use_times.find(use_type);
			if (level_cfg != NULL && use_times_iter != iter->second.use_type_to_use_times.end())
			{
				return use_times_iter->second + num <= LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetUseItemLimitTimes(level_cfg->use_item_limit_group_id, use_type);
			}
		}
	}

	return true;
}

void SpecialLogicCrystalFB::OnUseItem(Role * role, ItemID item_id, int num)
{
	const ItemBase *item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base)
	{
		return;
	}

	if (item_base->GetItemType() == I_TYPE_EXPENSE)
	{
		NorexItem * norex_item = (NorexItem *)item_base;
		int use_type = norex_item->GetUseType();

		const RoleCrystalFbLevelCfg  * level_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(m_level_seq);
		if (level_cfg != NULL && LOGIC_CONFIG->GetRoleCrystalFbConfig()->IsLimitTimesUseType(level_cfg->use_item_limit_group_id, use_type))
		{
			m_role_id_to_use_info[role->GetUID()].use_type_to_use_times[use_type] += num;
		}
	}

	this->SendUseItemInfo(role);
}

void SpecialLogicCrystalFB::OnTransport(Role * role, SpecialTransport * transport_obj)
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

		this->DelayKickOutAllRole();
	}
}

bool SpecialLogicCrystalFB::OnOpenTreasureChest(Role * role, TreasureChest * treasure_chest_obj)
{
	if (treasure_chest_obj == NULL)
	{
		return false;
	}

	const RoleCrystalFbLevelCfg  * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(m_level_seq);
	if (cfg == NULL)
	{
		return false;
	}

	//精英和稀有模式（组队模式）只能由队长来开
	if (cfg->type == ROLE_CRYSTAL_FB_TYPE_JING_YING || cfg->type == ROLE_CRYSTAL_FB_TYPE_XI_YOU)
	{
		Team * team = role->GetMyTeam();
		if (team == NULL)
		{
			return false;
		}

		if (!team->IsLeader(role))
		{
			return false;
		}
	}

	if (LOGIC_CONFIG->GetRoleCrystalFbConfig()->IsCrystalFbBoxGather(treasure_chest_obj->GetTreasureChestid()))
	{
		int box_group_id = 0;
		int box_seq = 0;
		treasure_chest_obj->GetParam(&box_group_id, &box_seq, NULL);
		const CrystalFbBoxCfg::ItemCfg *  box_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetBoxCfg(box_group_id, box_seq);

		if (box_cfg == NULL)
		{
			return false;
		}

		if (box_cfg->encounter_monster_group_id != 0)
		{
			const CrystalFbMonsterCfg::ItemCfg * monster_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbMonsterCfg(box_cfg->encounter_monster_group_id);
			if (monster_cfg == NULL)
			{
				return false;
			}

			int monster_id = NpcManager::Instance().GetMonsterIdByNpcId(monster_cfg->monster_npc_id);
			BattleManagerLocal::GetInstance().StartBattleModeFightReq(role, m_normal_monster_battle_mode, monster_id, 0, m_level_seq, true);
			std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.find(Protocol::SCCrystalFbObjInfo::CRYSTAL_FB_OBJ_TYPE_BOX);
			if (it != m_obj_info_list.end())
			{
				for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
				{
					if (it2->obj_id == treasure_chest_obj->GetId())
					{
						//删除allobj列表
						for (std::vector<SpecialLogicCrystalObjInfo>::iterator all_it = all_npc_list.begin(); all_it != all_npc_list.end(); all_it++)
						{
							if (all_it->obj_id == it2->obj_id)
							{
								all_it->obj_id = INVALID_OBJ_ID;
								break;
							}
						}
						it->second.erase(it2);
						break;
					}
				}
			}
			m_open_box_consume_uid = role->GetUID();
			m_box_num -= 1;
			this->SendSceneInfo();
			this->SendAllSceneObjInfo();
			return true;
		}
		Protocol::SCTreasureChestRewardList msg;
		msg.obj_id = treasure_chest_obj->GetId();
		Team * team = role->GetMyTeam();
		if (team != NULL)
		{
			for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
			{
				Role * member_role = team->GetMemberRoleByIndex(i);
				if (member_role != NULL && member_role->IsFollow())
				{
					msg.reward_count = 0;
					msg.add_coin = 0;
					int index = 0;
					if (ROLE_CRYSTAL_FB_TYPE_NORMAL == cfg->type)index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(0) - 1;
					
					if (ROLE_CRYSTAL_FB_TYPE_JING_YING == cfg->type)
					{
						/*int times = member_role->GetUID() == consumable_player ? 0 : member_role->GetRoleModuleManager()->GetRoleCrystalFb()->GetJingYingPlayTimes();
						index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(times) - 1;*/
						if (!this->IsHasConsume(member_role->GetUID()) && team->GetLeaderUID() != member_role->GetUID())
						{
							index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetReawrdAttColumn() - 1;
						}
						else
						{
							index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(0) - 1;
						}
					}
					if (0 <= index && index < (int)box_cfg->reward_column.size())
					{
						std::vector<ItemConfigData> reward;
						if (team->IsLeader(member_role))
						{
							reward = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandBoxReward(box_cfg->captain_reward_column[index]);
						}
						else
						{
							reward = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandBoxReward(box_cfg->reward_column[index]);
						}
						 
						if (reward.size() > 0)
						{
							member_role->GetRoleModuleManager()->GetKnapsack()->PutList((short)reward.size(), &reward[0], PUT_REASON_CRYSTAL_FB);
						}
						for (std::vector<ItemConfigData>::iterator it = reward.begin(); it!=reward.end();it++)
						{
							msg.reward_list[msg.reward_count].is_bind = it->is_bind ? 1 : 0;
							msg.reward_list[msg.reward_count].item_id = it->item_id;
							msg.reward_list[msg.reward_count].num = it->num;
							msg.reward_count += 1;

							this->ItemStack(member_role, &(*it));
						}
					}

					if (0 <= index && index < (int)box_cfg->add_coin.size())
					{
						int addordown = rand() % 2;
						int real_coin = 0;
						if (team->IsLeader(member_role))
						{
							real_coin = box_cfg->captain_coin[index];
						}
						else
						{
							real_coin = box_cfg->add_coin[index];
						}
						UNSTD_STATIC_CHECK(Money::RAND_MONEY_MAX > 0);
						if (addordown == 1)
						{
							real_coin += rand() % (Money::RAND_MONEY_MAX + 1);
						}
						else
						{
							real_coin -= rand() % (Money::RAND_MONEY_MAX + 1);
						}
						msg.add_coin = real_coin;
						member_role->GetRoleModuleManager()->GetMoney()->AddCoinBind(real_coin, __FUNCTION__);
					}
					EngineAdapter::Instance().NetSend(member_role->GetNetId(), (const char *)&msg, sizeof(msg) - (sizeof(msg.reward_list[0])*(Protocol::SCTreasureChestRewardList::MAX_TREASURE_CHEST_COUNT - msg.reward_count)));
					EventHandler::Instance().OnOpenTreasureChest(member_role);
				}
			}
		}
		else
		{
			msg.reward_count = 0;
			msg.add_coin = 0;
			int index = 0;
			if (ROLE_CRYSTAL_FB_TYPE_NORMAL == cfg->type)index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(0) - 1;
			if (ROLE_CRYSTAL_FB_TYPE_JING_YING == cfg->type)index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(0) - 1;
			if (0 <= index && index < (int)box_cfg->reward_column.size())
			{
				std::vector<ItemConfigData> reward = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandBoxReward(box_cfg->reward_column[index]);
				if (reward.size() > 0)
				{
					role->GetRoleModuleManager()->GetKnapsack()->PutList((short)reward.size(), &reward[0], PUT_REASON_CRYSTAL_FB);
				}
				for (std::vector<ItemConfigData>::iterator it = reward.begin(); it != reward.end(); it++)
				{
					msg.reward_list[msg.reward_count].is_bind = it->is_bind ? 1 : 0;
					msg.reward_list[msg.reward_count].item_id = it->item_id;
					msg.reward_list[msg.reward_count].num = it->num;
					msg.reward_count += 1;

					this->ItemStack(role, &(*it));
				}
			}

			if (0 <= index && index < (int)box_cfg->add_coin.size())
			{
				int addordown = rand() % 2;
				int real_coin = box_cfg->add_coin[index];
				UNSTD_STATIC_CHECK(Money::RAND_MONEY_MAX > 0);
				if (addordown == 1)
				{
					real_coin += rand() % (Money::RAND_MONEY_MAX + 1);
				}
				else
				{
					real_coin -= rand() % (Money::RAND_MONEY_MAX + 1);
				}
				msg.add_coin = real_coin;
				role->GetRoleModuleManager()->GetMoney()->AddCoinBind(real_coin, __FUNCTION__);
			}
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&msg, sizeof(msg) - (sizeof(msg.reward_list[0])*(Protocol::SCTreasureChestRewardList::MAX_TREASURE_CHEST_COUNT - msg.reward_count)));
		}
		std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.find(Protocol::SCCrystalFbObjInfo::CRYSTAL_FB_OBJ_TYPE_BOX);
		if (it != m_obj_info_list.end())
		{
			for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin();it2 != it->second.end();it2++)
			{
				if (it2->obj_id == treasure_chest_obj->GetId())
				{
					//删除allobj列表
					for (std::vector<SpecialLogicCrystalObjInfo>::iterator all_it = all_npc_list.begin(); all_it != all_npc_list.end(); all_it++)
					{
						if (all_it->obj_id == it2->obj_id)
						{
							all_it->obj_id = INVALID_OBJ_ID;
							break;
						}
					}
					it->second.erase(it2);
					break;
				}
			}
		}
		//m_scene->DeleteObj(treasure_chest_obj->GetId());
		m_box_num -= 1;
		this->SendSceneInfo();
		this->SendSceneObjInfo();
		this->SendAllSceneObjInfo();
	}

	return true;
}

void SpecialLogicCrystalFB::OnRoleLeaveTeam(Role * role)
{
	this->DelayKickOutRole(role);
}

void SpecialLogicCrystalFB::OnNPCChosedReward(Role * role, ObjID npc_objid,int option_index)
{
	Obj * npc_obj = m_scene->GetObj(npc_objid);
	if (npc_obj != NULL && Obj::OBJ_TYPE_NPC_CHOSED == npc_obj->GetObjType())
	{
		NPCChosed* npc_chose = (NPCChosed*)npc_obj;
		const CrystalFbNPCRewardCfg * npc_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetNpcRewardCfg(npc_chose->GetNpcId(),option_index);
		if (NULL != npc_cfg)
		{
			this->RoleTeamGetRewardByCfg(role, npc_cfg);

			if (npc_cfg->is_delete == 1)
			{
				bool is_find = false;
				int npc_type = 0;
				for (std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.begin(); it != m_obj_info_list.end() && !is_find;it++)
				{
					
					for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2!= it->second.end() && !is_find;it2++)
					{
						if (it2->obj_id == npc_objid)
						{
							//删除allobj列表
							for (std::vector<SpecialLogicCrystalObjInfo>::iterator all_it = all_npc_list.begin(); all_it != all_npc_list.end(); all_it++)
							{
								if (all_it->obj_id == it2->obj_id)
								{
									all_it->obj_id = INVALID_OBJ_ID;
									break;
								}
							}
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
					if (npc_type == Event_NPC_cfg::CRYSTALFB_NPC_TYPE_2)
					{
						m_npc_num -= 1;
					}
					else if (npc_type == Event_NPC_cfg::CRYSTALFB_NPC_TYPE_4)
					{
						m_gouhuo_num -= 1;
					}
					this->SendSceneObjInfo();
				}
				
			}
			this->SendSceneInfo();
			this->SendAllSceneObjInfo();
		}
	}
}

void SpecialLogicCrystalFB::OnGather(Role * role, int gather_id, int seq_id, const Posi & pos)
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
	const RoleCrystalFbLevelCfg  * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(m_level_seq);
	if (cfg == NULL)
	{
		return;
	}
	const GatherNpcConfig *  gather_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetGatherNpcCfg(gather_id);
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
					if (ROLE_CRYSTAL_FB_TYPE_NORMAL == cfg->type)index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(0) - 1;
					if (ROLE_CRYSTAL_FB_TYPE_JING_YING == cfg->type)
					{
						if (!this->IsHasConsume(member_role->GetUID()))
						{
							continue;
						}
						else
						{
							index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(0) - 1;
						}
						/*int times = member_role->GetUID() == consumable_player ? 0 : member_role->GetRoleModuleManager()->GetRoleCrystalFb()->GetJingYingPlayTimes();
						index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(times) - 1;*/
					}
					if (index < 0 || index >= MAX_ROLE_CRYSTAL_FB_REWARD_COLUMN)continue;
					switch (gather_cfg->reward_type)
					{
					case GatherNpcConfig::REWARD_TYPE_0:
						{
							const BuffListConfig*  buff_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetBuffId(gather_cfg->reward_list[index]);
							if (buff_cfg != NULL)
							{
								this->AddRoleBuff(role, buff_cfg);
							}
						}
						break;
					case GatherNpcConfig::REWARD_TYPE_1:
						{
							const ItemConfigData * reward_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandNpcReward(gather_cfg->reward_list[index]);
							if (reward_cfg != NULL)
							{
								if (!role->OnGiveGatherReward(*reward_cfg, true))
								{
									gamelog::g_log_knapsack.printf(LL_INFO, "SpecialLogicCrystalFB::OnGather role[%s,%d] knapsack full no put item[id:%d,num:%d,is_bind:%d]",
										member_role->GetName(), member_role->GetUID(), reward_cfg->item_id, reward_cfg->num, reward_cfg->is_bind ? 1 : 0);
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
							
							if (!member_role->GetRoleModuleManager()->GetMoney()->AddCoinBind(real_coin, "SpecialLogicCrystalFB::OnGather"))
							{
								return;
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
			if (ROLE_CRYSTAL_FB_TYPE_NORMAL == cfg->type)index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(0) - 1;
			if (ROLE_CRYSTAL_FB_TYPE_JING_YING == cfg->type)index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(0) - 1;
			if (index < 0 || index >= MAX_ROLE_CRYSTAL_FB_REWARD_COLUMN)return;
			switch (gather_cfg->reward_type)
			{
			case GatherNpcConfig::REWARD_TYPE_0:
				{
					const BuffListConfig*  buff_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetBuffId(gather_cfg->reward_list[index]);
					if (buff_cfg != NULL)
					{
						this->AddRoleBuff(role, buff_cfg);
					}
				}
				break;
			case GatherNpcConfig::REWARD_TYPE_1:
				{
					const ItemConfigData * reward_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandNpcReward(gather_cfg->reward_list[index]);
					if (reward_cfg != NULL)
					{
						if (!role->OnGiveGatherReward(*reward_cfg, true))
						{
							gamelog::g_log_knapsack.printf(LL_INFO, "SpecialLogicCrystalFB::OnGather role[%s,%d] knapsack full no put item[id:%d,num:%d,is_bind:%d]",
								role->GetName(), role->GetUID(), reward_cfg->item_id, reward_cfg->num, reward_cfg->is_bind ? 1 : 0);
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
					if (!role->GetRoleModuleManager()->GetMoney()->AddCoinBind(real_coin, "SpecialLogicCrystalFB::OnGather"))
					{
						return;
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
			for(std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.begin(); it != m_obj_info_list.end(); it++)
			{
				for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
				{
					if (it2->obj_id == npc->GetId() && it2->npc_id == gather_cfg->npc_id)
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
						else if (it2->obj_type == Event_NPC_cfg::CRYSTALFB_NPC_TYPE_5)
						{
							m_coin_num -= 1;
						}
						//删除allobj列表
						for (std::vector<SpecialLogicCrystalObjInfo>::iterator all_it = all_npc_list.begin(); all_it != all_npc_list.end(); all_it++)
						{
							if (all_it->obj_id == it2->obj_id)
							{
								all_it->obj_id = INVALID_OBJ_ID;
								break;
							}
						}
						it->second.erase(it2);
						break;
					}
				}
			}
		}
		this->SendSceneObjInfo();
		this->SendAllSceneObjInfo();
		this->SendSceneInfo();
	}
}

void SpecialLogicCrystalFB::OnNPCChosedTalk(Role * role, ObjID npc_objid)
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
					if (it2->obj_type == Event_NPC_cfg::CRYSTALFB_NPC_TYPE_4)not_gouhuo = true;
				}
			}
		}
		if (!not_gouhuo)m_talk_npc_num += 1;
	}
	this->SendSceneInfo();
}

bool SpecialLogicCrystalFB::IsBattleModeBelongToCrystalFB(int battle_mode)
{
	return BATTLE_MODE_CRYSTAL_FB == battle_mode
		|| BATTLE_MODE_ANCIENT_RUINS == battle_mode
		|| BATTLE_MODE_ELITE_TRAILS == battle_mode
		|| BATTLE_MODE_ELITE_TRAILS_NORMAL_MONSTER == battle_mode
		|| BATTLE_MODE_ANCIENT_RUINS_NORMAL_MONSTER == battle_mode;
}

void SpecialLogicCrystalFB::SetInfo(int layer, int level_seq, Posi transport_pos, int coster, int team_max_level, std::vector<Event_NPC_cfg> * has_create_npc_vec,
	std::map<int, std::vector<ItemConfigData> >* item_list, std::set<int> * consume_uid_list)
{
	if (layer <= 0 || layer > LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetLayerNum(level_seq))
	{
		return;
	}

	m_layer = layer;
	m_level_seq = level_seq;
	m_transport_pos = transport_pos;
	consumable_player = coster;
	m_team_max_level = team_max_level;
	this->SetBattleMode(level_seq);

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
	all_npc_list.clear();						//清除一下信息
	m_has_create_npc_vec.clear();
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

	if (has_create_npc_vec != NULL)
	{
		m_has_create_npc_vec.assign(has_create_npc_vec->begin(),has_create_npc_vec->end());
	}
	if (NULL != item_list)
	{
		m_item_list.swap(*item_list);
	}
	if (NULL != consume_uid_list)
	{
		m_consume_uid_list.swap(*consume_uid_list);
	}
}

void SpecialLogicCrystalFB::OnAddConsumeUid(int uid)
{
	m_consume_uid_list.insert(uid);
}

void SpecialLogicCrystalFB::SendUseItemInfo(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	const SpecialLogicCrystalUseItemInfo & role_use_info = m_role_id_to_use_info[role->GetUID()];

	Protocol::SCCrystalFbUseItemInfo protocol;
	protocol.count = 0;

	for (std::map<int, int>::const_iterator iter = role_use_info.use_type_to_use_times.begin(); iter != role_use_info.use_type_to_use_times.end(); ++iter)
	{
		if (protocol.count >= ARRAY_LENGTH(protocol.use_info))
		{
			break;
		}

		Protocol::SCCrystalFbUseItemInfo::UseItemInfo use_info;
		use_info.use_type = iter->first;
		use_info.use_times = iter->second;
		protocol.use_info[protocol.count++] = use_info;
	}


	int lenght = sizeof(protocol) - (ARRAY_LENGTH(protocol.use_info) - protocol.count) * sizeof(protocol.use_info[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&protocol, lenght);
}

void SpecialLogicCrystalFB::OnReqNextLevel(Role * role) const
{

}

void SpecialLogicCrystalFB::SendSceneInfo(Role * role)
{
	Protocol::SCCrystalFbObjInfo scene_info;
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
		if ((int)it->second.size() > 0 && scene_info.obj_count < Protocol::SCCrystalFbObjInfo::MAX_OBJ_LIST_COUNT)
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
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&scene_info, sizeof(scene_info) - ((Protocol::SCCrystalFbObjInfo::MAX_OBJ_LIST_COUNT - scene_info.obj_count) * sizeof(scene_info.obj_list[0])));
	}
	else
	{
		m_scene->SendToRole((const char*)&scene_info, sizeof(scene_info) - ((Protocol::SCCrystalFbObjInfo::MAX_OBJ_LIST_COUNT - scene_info.obj_count) * sizeof(scene_info.obj_list[0])));
	}
}

void SpecialLogicCrystalFB::SendSceneObjInfo(Role * role)
{
	Protocol::SCCrystalFbObjInfo scene_info;
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
	for (std::map<int, std::vector<SpecialLogicCrystalObjInfo> >::iterator it = m_obj_info_list.begin();it != m_obj_info_list.end();it++)
	{
		if ((int)it->second.size() > 0 && scene_info.obj_count < Protocol::SCCrystalFbObjInfo::MAX_OBJ_LIST_COUNT)
		{
			for (std::vector<SpecialLogicCrystalObjInfo>::iterator it2 = it->second.begin();it2 != it->second.end();it2++)
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
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&scene_info, sizeof(scene_info) - ((Protocol::SCCrystalFbObjInfo::MAX_OBJ_LIST_COUNT - scene_info.obj_count)*sizeof(scene_info.obj_list[0])));
	}
	else
	{
		m_scene->SendToRole((const char*)&scene_info, sizeof(scene_info) - ((Protocol::SCCrystalFbObjInfo::MAX_OBJ_LIST_COUNT - scene_info.obj_count) * sizeof(scene_info.obj_list[0])));
	}
}

void SpecialLogicCrystalFB::SendAllSceneObjInfo(Role * role)
{
	Protocol::CSCrystalFbAllNpcInfo scene_info;
	scene_info.obj_count = 0;
	for (std::vector<SpecialLogicCrystalObjInfo>::iterator it = all_npc_list.begin(); it != all_npc_list.end(); it++)
	{
		if ((int)all_npc_list.size() > 0 && scene_info.obj_count < Protocol::SCCrystalFbObjInfo::MAX_OBJ_LIST_COUNT)
		{
			scene_info.obj_list[scene_info.obj_count].objid = it->obj_id;
			scene_info.obj_list[scene_info.obj_count].npc_id = it->npc_id;
			scene_info.obj_list[scene_info.obj_count].obj_type = (short)it->obj_type;
			scene_info.obj_list[scene_info.obj_count].pos_x = it->pos_x;
			scene_info.obj_list[scene_info.obj_count].pos_y = it->pos_y;
			scene_info.obj_count += 1;

		}
	}
	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&scene_info, sizeof(scene_info) - ((Protocol::SCCrystalFbObjInfo::MAX_OBJ_LIST_COUNT - scene_info.obj_count) * sizeof(scene_info.obj_list[0])));
	}
	else
	{
		m_scene->SendToRole((const char*)&scene_info, sizeof(scene_info) - ((Protocol::SCCrystalFbObjInfo::MAX_OBJ_LIST_COUNT - scene_info.obj_count) * sizeof(scene_info.obj_list[0])));
	}
}

void SpecialLogicCrystalFB::CheckFristPassFb(Role *role, std::vector<int> * drop_list)
{
	if (!role->GetRoleModuleManager()->GetRoleCrystalFb()->HasFristPassCrystalFb(m_level_seq))role->GetRoleModuleManager()->GetRoleCrystalFb()->SetFristPassCrystalFb(m_level_seq);
	
	if (!this->IsHasConsume(role->GetUID()))return;//给过首通奖励，直接返回 ,没有消耗封印石头不算

	//奖励
	const RoleCrystalFbLevelCfg  * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(m_level_seq);
	if (cfg == NULL || (int)cfg->drop_list_vec.size() <= 0)
	{
		return;
	}

	drop_list->clear();//只给首杀，原有奖励清除

	for (std::vector<int>::const_iterator it = cfg->drop_list_vec.begin(); it != cfg->drop_list_vec.end(); it++)
	{
		drop_list->push_back(*it);
	}
}

void SpecialLogicCrystalFB::CreateNpc()
{
	if (this->IsLastLayer())
	{
		const RoleCrystalFbLevelCfg  * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(m_level_seq);
		if (cfg == NULL)
		{
			return;
		}
		NPCChosed * monster = NpcManager::Instance().CreateNPCChosed(cfg->recover_npc_id, m_scene, cfg->recover_npc_pos, m_normal_monster_battle_mode, &cfg->recover_npc_dir);
		if (monster == NULL)return;
		const ServerNPCInfoCfg *  s_cfg = NPCPOOL->GetServerNPCInfo(cfg->recover_npc_id);
		if (NULL != s_cfg)
		{
			SpecialLogicCrystalObjInfo node_info;
			node_info.npc_id = monster->GetNpcId();
			node_info.obj_id = monster->GetId();
			node_info.pos_x = monster->GetPos().x;
			node_info.pos_y = monster->GetPos().y;
			node_info.obj_type = Event_NPC_cfg::CRYSTALFB_NPC_TYPE_2;
			m_obj_info_list[Event_NPC_cfg::CRYSTALFB_NPC_TYPE_2].push_back(node_info);
			all_npc_list.push_back(node_info);
			m_npc_num += 1;
		}
		return;
	}
	const RoleCrystalFbLevelCfg  * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(m_level_seq);
	if (cfg == NULL)
	{
		return;
	}
	m_npc_num = 0;
	m_gather_num = 0;
	m_coin_num = 0;
	m_gouhuo_num = 0;
	std::vector<Event_NPC_cfg> npc_node_cfg;
	int npc_num = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandNum(cfg->event_num);
	int real_npc_num = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetEventNpcCfg(cfg->event_group_id, npc_num,&npc_node_cfg,&m_has_create_npc_vec);
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
					
					if (npc_node_cfg[i].npc_type == Event_NPC_cfg::CRYSTALFB_NPC_TYPE_6)
					{
						node_info.obj_type = Protocol::SCCrystalFbObjInfo::CRYSTAL_FB_OBJ_TYPE_GATHER0;
						m_obj_info_list[Protocol::SCCrystalFbObjInfo::CRYSTAL_FB_OBJ_TYPE_GATHER0].push_back(node_info);
						m_gather_num += 1;
					}
					else if (npc_node_cfg[i].npc_type == Event_NPC_cfg::CRYSTALFB_NPC_TYPE_8)
					{
						node_info.obj_type = Protocol::SCCrystalFbObjInfo::CRYSTAL_FB_OBJ_TYPE_GATHER2;
						m_obj_info_list[Protocol::SCCrystalFbObjInfo::CRYSTAL_FB_OBJ_TYPE_GATHER2].push_back(node_info);
						m_gather_num += 1;
					}
					else if (npc_node_cfg[i].npc_type == Event_NPC_cfg::CRYSTALFB_NPC_TYPE_7)
					{
						node_info.obj_type = Protocol::SCCrystalFbObjInfo::CRYSTAL_FB_OBJ_TYPE_GATHER1;
						m_obj_info_list[Protocol::SCCrystalFbObjInfo::CRYSTAL_FB_OBJ_TYPE_GATHER1].push_back(node_info);
						m_gather_num += 1;
					}
					else if (npc_node_cfg[i].npc_type == Event_NPC_cfg::CRYSTALFB_NPC_TYPE_5)
					{
						node_info.obj_type = Protocol::SCCrystalFbObjInfo::CRYSTAL_FB_OBJ_TYPE_COIB;
						m_obj_info_list[Protocol::SCCrystalFbObjInfo::CRYSTAL_FB_OBJ_TYPE_COIB].push_back(node_info);
						m_coin_num += 1;
					}
					all_npc_list.push_back(node_info);
				}
				break;
			case SERVER_NPC_TYPE_CHOSED:
				{
					NPCChosed * monster = NpcManager::Instance().CreateNPCChosed(npc_node_cfg[i].npc_id, m_scene, m_scene->GetARandPosi());
					if (monster == NULL)break;
					if (NULL != monster)
					{
						monster->SetBattleMode(m_normal_monster_battle_mode);
					}
					//检查事件
					const CrystalFbNPCRewardListCfg *npc_event = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetNpcEventCfg(npc_node_cfg[i].npc_id);
					if (npc_event != NULL)
					{
						m_has_create_npc_vec.push_back(npc_node_cfg[i]);//记录已经创建过的交互NPC
						int shop_id = 0;
						if (npc_event->reward_list[0].reward_type == 3)
						{
							shop_id = npc_event->reward_list[0].reward_list[0];
						}
						else if (npc_event->reward_list[1].reward_type == 3)
						{
							shop_id = npc_event->reward_list[1].reward_list[0];
						}

						if (shop_id != 0)//是商店，导入对应商店信息
						{
							NpcShopItem item_listp[MAX_ROLE_CRYSTALFB_BUY_ITEM_COUNT];
							int real_item_count = LOGIC_CONFIG->GetRoleCrystalFbConfig()->RandNPCShopItemList(shop_id, item_listp);
							for (int i = 0; i < real_item_count && i < MAX_ROLE_CRYSTALFB_BUY_ITEM_COUNT; i++)
							{
								monster->AddBuyItem(&item_listp[i]);
							}
						}
					}
					SpecialLogicCrystalObjInfo node_info;
					node_info.npc_id = monster->GetNpcId();
					node_info.obj_id = monster->GetId();
					node_info.pos_x = monster->GetPos().x;
					node_info.pos_y = monster->GetPos().y;
					if (npc_node_cfg[i].npc_type == Event_NPC_cfg::CRYSTALFB_NPC_TYPE_4)
					{
						node_info.obj_type = Event_NPC_cfg::CRYSTALFB_NPC_TYPE_4;
						m_obj_info_list[Event_NPC_cfg::CRYSTALFB_NPC_TYPE_4].push_back(node_info);
						m_gouhuo_num += 1;
					}
					else if (npc_node_cfg[i].npc_type == Event_NPC_cfg::CRYSTALFB_NPC_TYPE_2)
					{
						node_info.obj_type = Event_NPC_cfg::CRYSTALFB_NPC_TYPE_2;
						m_obj_info_list[Event_NPC_cfg::CRYSTALFB_NPC_TYPE_2].push_back(node_info);
						m_npc_num += 1;
					}
					all_npc_list.push_back(node_info);
				}
				break;
			default:
				break;
			}
		}
	}
}

void SpecialLogicCrystalFB::CreateMonster()
{
	const RoleCrystalFbLevelCfg  * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(m_level_seq);
	if (cfg == NULL)
	{
		return;
	}

	if (this->IsLastLayer())		//最后一关刷boss
	{
		NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(cfg->boss_npc_id, m_scene, cfg->boss_pos, m_battle_mode,&cfg->monster_dir);
		if (NULL != monster)
		{
			monster->SetParam1(m_level_seq);
			if (cfg->type == ROLE_CRYSTAL_FB_TYPE_JING_YING) monster->SetLevel(m_team_max_level);

			m_boss_obj_id = monster->GetId();
			SpecialLogicCrystalObjInfo node_info;
			node_info.npc_id = monster->GetNpcId();
			node_info.obj_id = monster->GetId();
			node_info.obj_type = Event_NPC_cfg::CRYSTALFB_NPC_TYPE_3;
			node_info.pos_x = monster->GetPos().x;
			node_info.pos_y = monster->GetPos().y;
			m_obj_info_list[Event_NPC_cfg::CRYSTALFB_NPC_TYPE_3].push_back(node_info);
			all_npc_list.push_back(node_info);
		}
	}
	else
	{
		int monster_num = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandNum(cfg->monster_num_group_id);
		m_monster_num = 0;

		for (int i = 0; i < monster_num  && i < 100; ++i)//避免无限判空死循环
		{
			const CrystalFbMonsterCfg::ItemCfg * monster_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbMonsterCfg(cfg->monster_id_group_id);
			if (monster_cfg == NULL)
			{
				continue;
			}

			NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(monster_cfg->monster_npc_id, m_scene, m_scene->GetARandPosi(), m_normal_monster_battle_mode, &cfg->monster_dir);
			if (NULL != monster)
			{
				m_monster_num += 1;
				SpecialLogicCrystalObjInfo node_info;
				node_info.npc_id = monster->GetNpcId();
				node_info.obj_id = monster->GetId();
				node_info.obj_type = Event_NPC_cfg::CRYSTALFB_NPC_TYPE_1;
				node_info.pos_x = monster->GetPos().x;
				node_info.pos_y = monster->GetPos().y;
				m_obj_info_list[Event_NPC_cfg::CRYSTALFB_NPC_TYPE_1].push_back(node_info);
				all_npc_list.push_back(node_info);
			}
		}
	}
}

void SpecialLogicCrystalFB::CreateBox()
{
	if (this->IsLastLayer())
	{
		return;
	}

	const RoleCrystalFbLevelCfg  * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(m_level_seq);
	if (cfg == NULL)
	{
		return;
	}

	int box_num = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandNum(cfg->box_num_group_id);
	m_box_num = box_num;
	for (int i = 0; i < box_num; ++i)
	{
		const CrystalFbBoxCfg::ItemCfg * box_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandBoxCfg(cfg->box_group_id);
		if (box_cfg == NULL)
		{
			continue;
		}
		Posi box_posi(0, 0);
		TreasureChest  * treasure_chest_obj = new TreasureChest(box_cfg->treasure_chest_id);
		if (cfg->type == ROLE_CRYSTAL_FB_TYPE_SPECIAL)
		{
			box_posi = box_cfg->treasure_chest_posi;
		}
		else
		{
			box_posi = m_scene->GetARandPosi();
		}
		treasure_chest_obj->SetPos(box_posi);
		treasure_chest_obj->SetParam(&cfg->box_group_id, &box_cfg->seq);
		m_scene->AddObj(treasure_chest_obj);

		SpecialLogicCrystalObjInfo node_info;
		node_info.npc_id = box_cfg->treasure_chest_id;
		node_info.obj_id = treasure_chest_obj->GetId();
		node_info.obj_type = Event_NPC_cfg::CRYSTALFB_NPC_TYPE_9;
		node_info.pos_x = treasure_chest_obj->GetPos().x;
		node_info.pos_y = treasure_chest_obj->GetPos().y;
		m_obj_info_list[Event_NPC_cfg::CRYSTALFB_NPC_TYPE_9].push_back(node_info);
		all_npc_list.push_back(node_info);
	}

}

void SpecialLogicCrystalFB::CreateTransport()
{
	if (this->IsLastLayer())
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
		node_info.obj_type = Event_NPC_cfg::CRYSTALFB_NPC_TYPE_10;
		node_info.pos_x = transport_obj->GetPos().x;
		node_info.pos_y = transport_obj->GetPos().y;
		m_obj_info_list[Event_NPC_cfg::CRYSTALFB_NPC_TYPE_10].push_back(node_info);
		all_npc_list.push_back(node_info);
	}
}

void SpecialLogicCrystalFB::CreateLeaveTransport(const Posi & pos)
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
	node_info.obj_type = Event_NPC_cfg::CRYSTALFB_NPC_TYPE_10;
	node_info.pos_x = transport_obj->GetPos().x;
	node_info.pos_y = transport_obj->GetPos().y;
	m_obj_info_list[Event_NPC_cfg::CRYSTALFB_NPC_TYPE_10].push_back(node_info);
	all_npc_list.push_back(node_info);
	this->SendSceneObjInfo();
	this->SendAllSceneObjInfo();
}

bool SpecialLogicCrystalFB::IsLastLayer(int layer)
{
	if (layer == 0)
	{
		layer = m_layer;
	}

	return layer >= LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetLayerNum(m_level_seq);
}

void SpecialLogicCrystalFB::TransportNextLayer(Role * role, SpecialTransport * transport_obj)
{
	if (NULL == role || NULL == transport_obj)
	{
		return;
	}

	const RoleCrystalFbLevelCfg  * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(m_level_seq);
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

		const CrystalFbSceneCfg::ItemCfg * scene_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandSceneCfg(layer_cfg->scene_id_group, m_already_rand_scene_id);
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
		target_pos = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandPosi(cfg->boss_layer_sceneborn_group_id);
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

	SpecialLogicCrystalFB * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicCrystalFB(scene_id, scene_key);
	if (special_lobgic == NULL)
	{
		return;
	}
	special_lobgic->SetInfo(m_layer + 1, m_level_seq, transport_pos, consumable_player, m_team_max_level, &m_has_create_npc_vec, &m_item_list, &m_consume_uid_list);
	special_lobgic->m_role_id_to_use_info = this->m_role_id_to_use_info;
	special_lobgic->m_already_rand_scene_id.insert(this->m_already_rand_scene_id.begin(), this->m_already_rand_scene_id.end());

	Team * team = role->GetMyTeam();
	if (team != NULL)
	{
		int member_num = team->GetMemberCount();
		for (int i = 0; i < member_num; ++i)
		{
			Role* member_role = team->GetMemberRoleByIndex(i);
			if (NULL != member_role && member_role->IsFollow())
			{
				EventHandler::Instance().OnClearFBOneLayer(member_role, FBManager::FB_TYPE_CRYSTAL_FB, cfg->type);
			}
		}
		
		this->TeamGoNextLayer(team, scene_id, scene_key, target_pos);
	}
	else
	{
		EventHandler::Instance().OnClearFBOneLayer(role, FBManager::FB_TYPE_CRYSTAL_FB, cfg->type);
		World::GetInstWorld()->GetSceneManager()->GoTo(role, scene_id, scene_key, target_pos);
	}
}

void SpecialLogicCrystalFB::RoleAloneGetRewardByCfg(Role * role, const CrystalFbNPCRewardCfg * npc_cfg)
{
	if (role == NULL || npc_cfg == NULL)return;
	const RoleCrystalFbLevelCfg  * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(m_level_seq);
	if (cfg == NULL)
	{
		return;
	}
	int index = 0;
	if (ROLE_CRYSTAL_FB_TYPE_NORMAL == cfg->type)index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(0) - 1;
	if (ROLE_CRYSTAL_FB_TYPE_JING_YING == cfg->type)
	{
		if (!this->IsHasConsume(role->GetUID()))
		{
			return;
		}
		else
		{
			index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(0) - 1;
		}
		/*int times = role->GetUID() == consumable_player ? 0 : role->GetRoleModuleManager()->GetRoleCrystalFb()->GetJingYingPlayTimes();
		index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(times) - 1;*/
	}
	if (index < 0 || index >= MAX_ROLE_CRYSTAL_FB_REWARD_COLUMN)return;
	switch (npc_cfg->reward_type)
	{
	case CrystalFbNPCRewardCfg::REWARD_TYPE_0:
		{
			const BuffListConfig*  buff_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetBuffId(npc_cfg->reward_list[index]);
			if (buff_cfg != NULL)
			{
				this->AddRoleBuff(role, buff_cfg);
			}
		}
		break;
	case CrystalFbNPCRewardCfg::REWARD_TYPE_1:
		{
			const ItemConfigData * reward_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandNpcReward(npc_cfg->reward_list[index]);
			if (reward_cfg != NULL)
			{
				if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(reward_cfg->item_id, reward_cfg->num))
				{
					role->GetRoleModuleManager()->GetKnapsack()->Put(*reward_cfg, PUT_REASON_CRYSTAL_FB);
				}
				else
				{
					gamelog::g_log_knapsack.printf(LL_INFO, "SpecialLogicCrystalFB::OnNPCChosedReward role[%s,%d] knapsack full no put item[id:%d,num:%d,is_bind:%d]",
						role->GetName(), role->GetUID(), reward_cfg->item_id, reward_cfg->num, reward_cfg->is_bind ? 1 : 0);
				}
			}
		}
		break;
	case CrystalFbNPCRewardCfg::REWARD_TYPE_3:
		{
			if (npc_cfg->reward_list[index] > 0)
			{
				if (!role->GetRoleModuleManager()->GetMoney()->AddCoinBind(npc_cfg->reward_list[index], "SpecialLogicCrystalFB::OnNPCChosedReward"))
				{
					gamelog::g_log_knapsack.printf(LL_INFO, "SpecialLogicCrystalFB::OnNPCChosedReward role[%s,%d] knapsack add bindcoin[%d]",
						role->GetName(), role->GetUID(), npc_cfg->reward_list[index]);
				}
			}
		}
		break;
	default:
		break;
	}
}

void SpecialLogicCrystalFB::RoleTeamGetRewardByCfg(Role * role, const CrystalFbNPCRewardCfg * npc_cfg)
{
	if (role == NULL || npc_cfg == NULL)return;
	if (role->InTeam())
	{
		Team* role_team = role->GetMyTeam();
		if (role_team == NULL)return;
		for (int i = 0; i < role_team->GetMemberCount() && i < MAX_TEAM_MEMBER_NUM; i++)
		{
			Role *temp_role = role_team->GetMemberRoleByIndex(i);
			if (temp_role == NULL || !temp_role->IsFollow())continue;
			this->RoleAloneGetRewardByCfg(temp_role, npc_cfg);
		}
	}
	else
	{
		this->RoleAloneGetRewardByCfg(role, npc_cfg);
	}
}

void SpecialLogicCrystalFB::AddRoleBuff(Role * role, const BuffListConfig * buff_cfg)
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
		//只操作队长，方便全员回复（包括NPC）
		if (role->InTeam())
		{
			Team* m_team = role->GetMyTeam();
			if (m_team->GetLeaderUID() != role->GetUID())return;
			int member_count = m_team->GetMemberCount();
			for (int member_index = 0; member_index < member_count; member_index++)
			{
				TeamMember * team_member = m_team->GetMemberInfoByIndex(member_index);
				if (NULL != team_member)
				{
					if (team_member->IsRobot())
					{
						Robot* member_robot = m_team->GetMemberRobotByIndex(member_index);
						if (NULL != member_robot)
						{
							this->RecoveryRobotStatus(member_robot, buff_cfg);
						}
					}
					else
					{
						Role*  member_role = m_team->GetMemberRoleByIndex(member_index);
						if (NULL != member_role)
						{
							this->RecoveryRoleStatus(member_role, buff_cfg);
						}
					}
				}
			}
		}
		else
		{
			this->RecoveryRoleStatus(role, buff_cfg);
		}
	}
	else if (buff_cfg->type == BuffListConfig::REWARDBUFTYPE_3)
	{
		//只操作队长，方便全员回复（包括NPC）
		if (role->InTeam())
		{
			Team* m_team = role->GetMyTeam();
			if (m_team->GetLeaderUID() != role->GetUID())return;
			int member_count = m_team->GetMemberCount();
			for (int member_index = 0; member_index < member_count; member_index++)
			{
				TeamMember * team_member = m_team->GetMemberInfoByIndex(member_index);
				if (NULL != team_member)
				{
					if (team_member->IsRobot())
					{
						Robot* member_robot = m_team->GetMemberRobotByIndex(member_index);
						if (NULL != member_robot)
						{
							this->RecoveryRobotStatus(member_robot, buff_cfg);
						}
					}
					else
					{
						Role*  member_role = m_team->GetMemberRoleByIndex(member_index);
						if (NULL != member_role)
						{
							this->RecoveryRoleStatus(member_role, buff_cfg);
						}
					}
				}
			}
		}
		else
		{
			this->RecoveryRoleStatus(role, buff_cfg);
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
	else if (buff_cfg->type == BuffListConfig::REWARDBUFTYPE_5)
	{
		BuffChangeAttr *buff = new BuffChangeAttr(role->GetRoleModuleManager(), 0, 0);
		int num_persent = role->GetRoleModuleManager()->GetAttrByType(buff_cfg->param_1);
		buff->AddAttr(buff_cfg->param_1, (num_persent * buff_cfg->param_2) / 100);
		buff->SetSave(false);
		buff->SetSceneBuffType(this->GetSceneType());
		role->GetRoleModuleManager()->GetBuffManager()->AddBuff(buff);
		this->SendBuffEffectToRole(role,Protocol::SCRoleSceneEffect::ROLE_SCENE_EFFECT_TYPE_3);
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
	else if (buff_cfg->type == BuffListConfig::REWARDBUFTYPE_6)
	{
		//只操作队长，方便全员回复（包括NPC）
		if (role->InTeam())
		{
			Team* m_team = role->GetMyTeam();
			if (m_team->GetLeaderUID() != role->GetUID())return;
			int member_count = m_team->GetMemberCount();
			for (int member_index = 0; member_index < member_count; member_index++)
			{
				TeamMember * team_member = m_team->GetMemberInfoByIndex(member_index);
				if (NULL != team_member)
				{
					if (team_member->IsRobot())
					{
						Robot* member_robot = m_team->GetMemberRobotByIndex(member_index);
						if (NULL != member_robot)
						{
							this->RecoveryRobotStatus(member_robot, buff_cfg);
						}
					}
					else
					{
						Role*  member_role = m_team->GetMemberRoleByIndex(member_index);
						if (NULL != member_role)
						{
							this->RecoveryRoleStatus(member_role, buff_cfg);
						}
					}
				}
			}
		}
		else
		{
			this->RecoveryRoleStatus(role, buff_cfg);
		}
	}
}

void SpecialLogicCrystalFB::RecoveryRoleStatus(Role * role, const BuffListConfig * buff_cfg)
{
	if (role == NULL || buff_cfg == NULL)
	{
		return;
	}
	switch (buff_cfg->type)
	{
	case BuffListConfig::REWARDBUFTYPE_2:
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
		break;
	case BuffListConfig::REWARDBUFTYPE_3:
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
		break;
	case BuffListConfig::REWARDBUFTYPE_6:
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
		break;
	default:
		break;
	}
}

void SpecialLogicCrystalFB::RecoveryRobotStatus(Robot * robot, const BuffListConfig * buff_cfg)
{
	if (robot == NULL || buff_cfg == NULL)
	{
		return;
	}

	switch (buff_cfg->type)
	{
	case BuffListConfig::REWARDBUFTYPE_2:
		{
			const RoleBattleData& robot_data = robot->GetRoleBattleData();
			BattleAttr robot_attr_hp = robot_data.role_character_data.attr_list[BATTLE_ATTR_MAX_HP];
			BattleAttr robot_attr_mp = robot_data.role_character_data.attr_list[BATTLE_ATTR_MAX_MP];
			int add_mp = (robot_attr_mp * buff_cfg->param_1) / 100;
			robot->UpdateHpMp(robot_attr_hp, add_mp + robot_attr_mp);

			int pet_fight_index = robot_data.fight_pet_idx;
			if (pet_fight_index >= 0 && pet_fight_index < ROLE_BATTLE_PET_NUM)
			{
				BattleAttr robot_pet_attr_hp = robot_data.pet_character_data[pet_fight_index].battle_data.attr_list[BATTLE_ATTR_MAX_HP];
				BattleAttr robot_pet_attr_mp = robot_data.pet_character_data[pet_fight_index].battle_data.attr_list[BATTLE_ATTR_MAX_MP];
				int pet_add_mp = (robot_pet_attr_mp * buff_cfg->param_1) / 100;
				robot->UpdatePetHpMp(robot_data.pet_character_data[pet_fight_index].battle_data.id_param, pet_fight_index, robot_attr_hp, pet_add_mp + robot_attr_mp);
			}
		}
		break;
	case BuffListConfig::REWARDBUFTYPE_3:
		{
			const RoleBattleData& robot_data = robot->GetRoleBattleData();
			BattleAttr robot_attr_hp = robot_data.role_character_data.attr_list[BATTLE_ATTR_MAX_HP];
			BattleAttr robot_attr_mp = robot_data.role_character_data.attr_list[BATTLE_ATTR_MAX_MP];
			int add_hp = (robot_attr_hp * buff_cfg->param_1) / 100;
			robot->UpdateHpMp(add_hp + robot_attr_hp, robot_attr_mp);

			int pet_fight_index = robot_data.fight_pet_idx;
			if (pet_fight_index >= 0 && pet_fight_index < ROLE_BATTLE_PET_NUM)
			{
				BattleAttr robot_pet_attr_hp = robot_data.pet_character_data[pet_fight_index].battle_data.attr_list[BATTLE_ATTR_MAX_HP];
				BattleAttr robot_pet_attr_mp = robot_data.pet_character_data[pet_fight_index].battle_data.attr_list[BATTLE_ATTR_MAX_MP];
				int pet_add_hp = (robot_pet_attr_hp * buff_cfg->param_1) / 100;
				robot->UpdatePetHpMp(robot_data.pet_character_data[pet_fight_index].battle_data.id_param, pet_fight_index, robot_attr_hp + pet_add_hp, robot_attr_mp);
			}
		}
		break;
	case BuffListConfig::REWARDBUFTYPE_6:
		{
			const RoleBattleData& robot_data = robot->GetRoleBattleData();
			BattleAttr robot_attr_hp = robot_data.role_character_data.attr_list[BATTLE_ATTR_MAX_HP];
			BattleAttr robot_attr_mp = robot_data.role_character_data.attr_list[BATTLE_ATTR_MAX_MP];
			int add_hp = (robot_attr_hp * buff_cfg->param_1) / 100;
			int add_mp = (robot_attr_mp * buff_cfg->param_1) / 100;
			robot->UpdateHpMp(add_hp + robot_attr_hp, robot_attr_mp + add_mp);

			int pet_fight_index = robot_data.fight_pet_idx;
			if (pet_fight_index >= 0 && pet_fight_index < ROLE_BATTLE_PET_NUM)
			{
				BattleAttr robot_pet_attr_hp = robot_data.pet_character_data[pet_fight_index].battle_data.attr_list[BATTLE_ATTR_MAX_HP];
				BattleAttr robot_pet_attr_mp = robot_data.pet_character_data[pet_fight_index].battle_data.attr_list[BATTLE_ATTR_MAX_MP];
				int pet_add_hp = (robot_pet_attr_hp * buff_cfg->param_1) / 100;
				int pet_add_mp = (robot_pet_attr_mp * buff_cfg->param_1) / 100;
				robot->UpdatePetHpMp(robot_data.pet_character_data[pet_fight_index].battle_data.id_param, pet_fight_index, robot_attr_hp + pet_add_hp, robot_attr_mp + pet_add_mp);
			}
		}
		break;
	default:
		break;
	}
}

void SpecialLogicCrystalFB::SendBuffEffectToRole(Role * buff_geter, int buff_type)
{
	if (buff_geter == NULL)return;
	Protocol::SCRoleSceneEffect my_buff_effect;
	my_buff_effect.buffer_obj = buff_geter->GetId();
	my_buff_effect.effect_type = buff_type;
	buff_geter->GetScene()->SendToRole((const char*)&my_buff_effect,sizeof(my_buff_effect));
}

void SpecialLogicCrystalFB::TeamGoNextLayer(Team * team, int target_scene_id, int target_scene_key, const Posi & target_pos)
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

void SpecialLogicCrystalFB::SetBattleMode(int level_seq)
{
	m_battle_mode = BATTLE_MODE_CRYSTAL_FB; // 默认值
	m_normal_monster_battle_mode = BATTLE_MODE_CRYSTAL_FB; // 默认值

	const RoleCrystalFbLevelCfg* cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(level_seq);
	if (NULL == cfg) return;
	
	// 为了给客户端区分天雷怪而做的特殊处理
	if (ROLE_CRYSTAL_FB_TYPE_XI_YOU == cfg->type)
	{
		m_battle_mode = BATTLE_MODE_ELITE_TRAILS;
		m_normal_monster_battle_mode = BATTLE_MODE_ELITE_TRAILS;
	}
	else if (ROLE_CRYSTAL_FB_TYPE_JING_YING == cfg->type)
	{
		m_battle_mode = BATTLE_MODE_ANCIENT_RUINS;
		m_normal_monster_battle_mode = BATTLE_MODE_ANCIENT_RUINS;
	}
}

void SpecialLogicCrystalFB::ItemStack(Role * role, const ItemConfigData * item)
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

bool SpecialLogicCrystalFB::IsHasConsume(int uid)
{
	std::set<int>::iterator it = m_consume_uid_list.find(uid);
	if(it != m_consume_uid_list.end()) return true;

	return false;
}


