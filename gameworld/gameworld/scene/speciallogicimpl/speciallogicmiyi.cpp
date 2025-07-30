#include "speciallogicmiyi.hpp"
#include "npc/npcmanager.h"
#include "obj/character/npc_monster.h"
#include "scene/scene.h"
#include "other/miyi/miyiconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/miyi/rolemiyi.hpp"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "other/event/eventhandler.hpp"
#include "other/route/mailroute.hpp"
#include "protocol/msgmiyi.h"
#include "obj/character/role.h"
#include "engineadapter.h"
#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "global/offlineregister/offlineregister.hpp"
#include "global/usercache/usercache.hpp"
#include "other/robot/robotmanager.hpp"
#include "other/poker/pokermodulemanager.hpp"
#include "global/drawmanager/drawmanager.hpp"
#include "protocol/msgdraw.h"


SpecialLogicMiYi::SpecialLogicMiYi(Scene* scene) : SpecialLogic(scene), m_refreshed_monster(false), m_is_finish(false),m_npc_monster_count(0), m_kill_flag(0)
{

}

SpecialLogicMiYi::~SpecialLogicMiYi()
{

}

void SpecialLogicMiYi::OnRoleEnterScene(Role *role)
{
	if (NULL == role) return;

	if (!m_refreshed_monster)
	{
		this->RefreshNPCMonster();
	}
	this->SendSceneInfo(role);
}

void SpecialLogicMiYi::Update(unsigned long interval, time_t now_second)
{
	if (m_is_finish) return;

	SpecialLogic::Update(interval, now_second);

	if (!m_refreshed_monster) return;

	this->CheckSceneMonsterStatus(interval, now_second);

}

void SpecialLogicMiYi::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	int npc_seq_id = ack->business_data.monster_gen_id;
	NPCMonster* npc_monster = NpcManager::Instance().GetNPCMonster(npc_seq_id);
	if (NULL == npc_monster) return;

	if (npc_monster->GetScene()->GetSceneID() != m_scene->GetSceneID() ||
		npc_monster->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
	{
		return;
	}

	// 赢了
	if (RESULT_TYPE_ATTACKER_WIN != ack->battle_result)
	{
		return;
	}
	const MiYiLevelCfg * cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiLevelCfgBySceneId(m_scene->GetSceneID());
	if (cfg == NULL)
	{
		return;
	}

	const MiYiMonsterCfg * monster_cfg = cfg->GetMiYiMonsterCfg(npc_monster->GetNpcId());
	if (monster_cfg == NULL)
	{
		return;
	}

	const MiyiOtherCfg& other_cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiOtherCfg();

	UNSTD_STATIC_CHECK(sizeof(unsigned int) * 8 >= MI_YI_MAX_MONSTER_NUM);
	m_kill_flag |= (1 << monster_cfg->monster_seq);
	m_scene->DeleteObj(npc_monster->GetId());
	m_npc_monster_count -= 1;

	//本身就是随机奖励不需要洗牌
	DrawGroupInfo draw_group_info;

	//通知信息
	int tmp_role_count = 0;
	Role* tmp_role_list[SIDE_MAX_ROLE_NUM] = { 0 };
	Protocol::SCDrawPokerInfo info;
	MiYiPoker mypoker[SIDE_MAX_ROLE_NUM];
	// 给奖励
	for (int ind = 0;  ind < SIDE_MAX_ROLE_NUM && ind < DRAW_GROUP_MAX_ITEM_NUM; ++ind)
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

		GameName &role_name = mypoker[ind].role_name;

		Protocol::SCMiYiKillPassInfo kill_pass_info_protocol;
		kill_pass_info_protocol.is_first_kill = 0;
		kill_pass_info_protocol.is_week_kill = 0;
		kill_pass_info_protocol.day_pass_count = 0;

		Role * role = m_scene->GetRoleByUID(role_uid);
		if (NULL == role)
		{
			std::vector<ItemConfigData> rewards_role_level_no = LOGIC_CONFIG->GetMiYiConfig()->GetRewardItem(monster_cfg->role_level_reward_group_id, cfg->need_role_level);
			if (rewards_role_level_no.size() > 0)
			{
				DrawManager::Instance().PutRewardItem(reward_info, (int)rewards_role_level_no.size(), &rewards_role_level_no[0]);
			}
		
			if (!reward_info.item_list.empty())
			{//之前的逻辑就是这样 只通知第一个物品
				DrawManager::Instance().GetItemInfoType(mypoker[ind].poker_info, reward_info.item_list[0]);
			}

			//机器人
			Robot *robot = RobotManager::Instance().GetRobot(role_uid);
			if (NULL != robot)
			{
				robot->GetName(role_name);
			}

			//掉线玩家也没有奖励
			//角色离线
			UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(role_uid);
			if (NULL != user_cache_node)
			{
				reward_info.role_uid = 0;
			}
		}
		else
		{
			tmp_role_list[tmp_role_count] = role;
			tmp_role_count++;

			role->GetRoleModuleManager()->GetMiYi()->SetKill(cfg->seq, monster_cfg->monster_seq);

			bool is_first = role->GetRoleModuleManager()->GetMiYi()->IsFirstKill(cfg->seq, monster_cfg->monster_seq);
			EventHandler::Instance().OnKillMiYiFBBoss(role, cfg->seq, monster_cfg->monster_seq + 1, is_first);

			//角色名
			role->GetName(role_name);
			bool is_has_fixed_reward = false;
			//衰减奖励
			int user_pass_count = role->GetRoleModuleManager()->GetMiYi()->GetDayPassCount();
			if (user_pass_count >= LOGIC_CONFIG->GetMiYiConfig()->GetMaxRewardProportion())
			{
				//不能获得翻牌奖励
				//移除uid
				reward_info.role_uid = 0;
				memset(role_name, 0, sizeof(role_name));
				//即使玩家不能翻牌,也得随机奖励让其他玩家观看 
				user_pass_count = 0;
				//目前衰减后不给固定奖励
				is_has_fixed_reward = false;
			}
			else
			{
				is_has_fixed_reward = true;
			}
			//玩家超过次数不给固定奖励
		/*	if (user_pass_count < other_cfg.weekly_kill_times)
			{
			}*/

			std::vector<ItemConfigData> rewards_role_level = LOGIC_CONFIG->GetMiYiConfig()->GetRewardItem(monster_cfg->role_level_reward_group_id, role->GetLevel(), user_pass_count);	// 周常奖励
			role->GetRoleModuleManager()->GetMiYi()->AddPassCount();
			kill_pass_info_protocol.day_pass_count = user_pass_count + 1;
			EngineAdapter::Instance().NetSend(role->GetNetId(), &kill_pass_info_protocol, sizeof(kill_pass_info_protocol));

			//翻牌随机奖励
			if (rewards_role_level.size() > 0)
			{
				DrawManager::Instance().PutRewardItem(reward_info, (int)rewards_role_level.size(), &rewards_role_level[0], role->GetAppearanceProf());
				//协议信息记录 翻牌随机奖励
				if (!reward_info.item_list.empty())
				{//之前的逻辑就是这样 只通知第一个物品
					DrawManager::Instance().GetItemInfoType(mypoker[ind].poker_info, reward_info.item_list[0]);
				}
			}
			if (is_has_fixed_reward)
			{
				//翻牌固定奖励
				ItemConfigData reward_item = other_cfg.reward;
				DrawManager::Instance().PutRewardItem(reward_info, 1, &reward_item, role->GetAppearanceProf());
				mypoker[ind].fixed_reward.reward_id = other_cfg.reward.item_id;
				mypoker[ind].fixed_reward.num = other_cfg.reward.num;
				mypoker[ind].fixed_reward.is_bind = other_cfg.reward.is_bind;
			}

			if (m_npc_monster_count <= 0)
			{
				EventHandler::Instance().OnPassMiYiFB(role, cfg->seq, other_cfg.monster_num + 1);
			}
			EventHandler::Instance().OnRealKillMiYiFBBoss(role);
		}

		mypoker[ind].index = reward_info.draw_index;
		mypoker[ind].role_uid = reward_info.role_uid;

	}

	DrawManager::Instance().AddDrawGroup(draw_group_info, DRAW_GROUP_TYPE_MI_YI);

	info.draw_group_id = draw_group_info.draw_group_id;
	info.timeout = draw_group_info.out_time;
	info.draw_group_type = DRAW_GROUP_TYPE_MI_YI;
	for (int k = 0; k < SIDE_MAX_ROLE_NUM; k++)
	{
		memcpy(info.draw_poker[k].data, &mypoker[k], sizeof(MiYiPoker));
	}
	UNSTD_STATIC_CHECK(sizeof(MiYiPoker) <= MAX_ITEM_LENGTH_NUM);
	for (int i = 0; i < tmp_role_count && i < SIDE_MAX_ROLE_NUM; i++)
	{
		if (NULL == tmp_role_list[i])
		{
			continue;
		}
		EngineAdapter::Instance().NetSendSerializeMsg2(tmp_role_list[i]->GetNetId(), &info);
	}
	this->SendSceneInfo();
}

// void SpecialLogicMiYi::GetPoker(int role_uid, int poker_index, int reson, int game_mode)
// {
// 	Role *role = m_scene->GetRoleByUID(role_uid);
// 	if (!role) return;
// 
// 	int role_index = 0;
// 	for (int idx = 0; idx < SIDE_MAX_ROLE_NUM; ++idx)
// 	{
// 		if (m_myp[idx].role_uid == role_uid)
// 		{
// 			role_index = idx;
// 			break;
// 		}
// 	}
// 
// 	//MiYiPoker cur_role_poker = role->GetRoleModuleManager()->GetMiYi()->GetPokerReward();
// 	MiYiPoker *cur_role_poker = POKER_MODULE.Get<MiYiPoker,TeamPokerRewardInfoTemp>(role_uid,-1);
// 	if (cur_role_poker == NULL) return;
// 
// 	const ItemBase *item = ITEMPOOL->GetItem(cur_role_poker->reward_id); 
// 	if (!item) return;
// 	const ItemBase *fixed_item = ITEMPOOL->GetItem(cur_role_poker->fixed_reward.reward_id); 
// 	if (!fixed_item) return;
// 	ItemDataWrapper icd;
// 	ItemDataWrapper fixed_icd;
// 
// 	if (cur_role_poker->is_turned == 1) return;
// 	if (cur_role_poker->is_can_turn == 1 && cur_role_poker->is_turned == 0)
// 	{
// 		icd.item_id = item->GetItemId();
// 		icd.is_bind = item->IsBind();
// 		icd.num = cur_role_poker->reward_count;
// 		memcpy(icd.param_data, cur_role_poker->param_data, sizeof(icd.param_data));
// 
// 		fixed_icd.item_id = fixed_item->GetItemId();
// 		fixed_icd.is_bind = fixed_item->IsBind();
// 		fixed_icd.num = cur_role_poker->fixed_reward.num;
// 
// 		// 这里处理都不翻牌的情况
// 		//if (pokker_index[poker_index] != 1)
// 		//{
// 		//	cur_role_poker->index = poker_index;
// 		//	pokker_index[poker_index] = 1;
// 		//}
// 		//else
// 		//{
// 		//	for (int i = 1; i < SIDE_MAX_ROLE_NUM + 1; ++i)
// 		//	{
// 		//		if (pokker_index[i] != 1)
// 		//		{
// 		//			cur_role_poker->index = i;
// 		//			pokker_index[i] = 1;
// 		//			break;
// 		//		}
// 		//	}
// 		//}
// 		POKER_MODULE.PokerConflict(cur_role_poker, poker_index, false);
// 
// 		
// 		role->GetRoleModuleManager()->GetKnapsack()->Put(icd, PUT_REASON_MI_YI);
// 		role->GetRoleModuleManager()->GetKnapsack()->Put(fixed_icd, PUT_REASON_MI_YI);
// 		
// 
// 		cur_role_poker->is_turned = 1;
// 		cur_role_poker->is_can_turn = 0;
// 
// 	}
// 	//memcpy(&m_myp[role_index], &cur_role_poker, sizeof(m_myp[role_index]));
// 	//role->GetRoleModuleManager()->GetMiYi()->SetPokerReward(cur_role_poker);
// 
// 	this->SendPokerRoleLevelReward();
// 
// 	POKER_MODULE.FreeData<MiYiPoker>(cur_role_poker);
// 
// }
		
// void SpecialLogicMiYi::FightFinishPoker(battlegameprotocol::BattleGameFightResultAck * ack, int index, Role * role, const  void * load_data_arr[LOAD_DATA_MAX], int game_mode)
// {
// 	const MiYiLevelCfg *cfg = (const MiYiLevelCfg *) load_data_arr[LOAD_DATA_MIYI_CONFIG_MiYiLevelCfg];
// 	if (!cfg) return;
// 	const MiYiMonsterCfg *monster_cfg = (const MiYiMonsterCfg *) load_data_arr[LOAD_DATA_MIYI_CONFIG_MiYiMonsterCfg];
// 	if (!monster_cfg) return;
// 
// 	if (NULL == role || ack->attacker_list.role_list[index].is_flyaway > 0)
// 	{
// 		Robot *robot = RobotManager::Instance().GetRobot(ack->attacker_list.role_list[index].uid);
// 
// 		std::vector<ItemConfigData> rewards_role_level_no = LOGIC_CONFIG->GetMiYiConfig()->GetRewardItem(monster_cfg->role_level_reward_group_id, cfg->need_role_level);
// 
// 		if (rewards_role_level_no.size() > 0)
// 		{
// 			//m_myp[index].role_uid = 0;
// 			//m_myp[index].reward_id = rewards_role_level_no[0].item_id;
// 			//m_myp[index].reward_count = rewards_role_level_no[0].num;
// 			//
// 			//// 翻牌奖励净值
// 			//const ItemBase *item = ITEMPOOL->GetItem(m_myp[index].reward_id);
// 			//if (!item) return;
// 			//NetValueItemParam tmp_item_param;
// 			//if (!item->HasRandomNetValue(&tmp_item_param))
// 			//{
// 			//	item->RandomNetValueOnPutItem(NULL, &tmp_item_param, PUT_REASON_MI_YI);
// 			//	item->SetRandomNetValueFlag(&tmp_item_param);
// 			//	m_myp[index].param_len = tmp_item_param.GetDataByItemType(item->GetItemType(), m_myp[index].param_data);
// 			//	m_myp[index].param_len_shengyu = sizeof(m_myp[index].param_data) - m_myp[index].param_len;
// 			//}
// 
// 			POKER_MODULE.RoleRecorder(&m_myp[index],NULL, rewards_role_level_no, PUT_REASON_MI_YI,false,false,true); 
// 		}
// 
// 		//if (robot == NULL) // 掉线队友
// 		//{
// 		//	m_myp[index].is_can_turn = 1;
// 		//	m_myp[index].is_turned = 0;
// 		//	m_myp[index].index = 0;
// 		//} else
// 		//{
// 		//	m_myp[index].is_can_turn = 0;
// 		//	m_myp[index].is_turned = 0;
// 		//	m_myp[index].index = 0;
// 		//}
// 
// 		POKER_MODULE.PermissionsRecorder(&m_myp[index], robot);
// 		return;
// 	} 
// 	else
// 	{
// 		std::vector<ItemConfigData> rewards_first_kill;	// 首杀奖励
// 		std::vector<ItemConfigData> rewards_role_level;	// 周常奖励
// 
// 		rewards_role_level = LOGIC_CONFIG->GetMiYiConfig()->GetRewardItem(monster_cfg->role_level_reward_group_id, role->GetLevel());
// 
// 		
// 		if (rewards_role_level.size() > 0)
// 		{
// 			//m_myp[index].role_uid = role->GetUID();
// 			//STRNCPY(m_myp[index].role_name, role->GetName(), sizeof(m_myp[index].role_name));
// 			//m_myp[index].reward_id = rewards_role_level[0].item_id;
// 			//m_myp[index].reward_count = rewards_role_level[0].num;
// 			//
// 			//// 翻牌奖励净值
// 			//const ItemBase *item = ITEMPOOL->GetItem(m_myp[index].reward_id);
// 			//if (!item) return;
// 			//NetValueItemParam tmp_item_param;
// 			//if (!item->HasRandomNetValue(&tmp_item_param))
// 			//{
// 			//	item->RandomNetValueOnPutItem(NULL, &tmp_item_param, PUT_REASON_MI_YI);
// 			//	item->SetRandomNetValueFlag(&tmp_item_param);
// 			//	m_myp[index].param_len = tmp_item_param.GetDataByItemType(item->GetItemType(), m_myp[index].param_data);
// 			//	m_myp[index].param_len_shengyu = sizeof(m_myp[index].param_data) - m_myp[index].param_len;
// 			//}
// 
// 			POKER_MODULE.RoleRecorder(&m_myp[index], role, rewards_role_level, PUT_REASON_MI_YI, false, true,true);
// 		}
// 
// 
// 		const MiyiOtherCfg* myother_cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiOtherCfg();
// 		if (myother_cfg == NULL) return;;
// 
// 		ItemConfigData reward_item = myother_cfg->reward;
// 		const ItemBase *fixed_item = ITEMPOOL->GetItem(reward_item.item_id);
// 		if (fixed_item == NULL) return;
// 
// 		m_myp[index].fixed_reward.reward_id = fixed_item->GetItemId();
// 		m_myp[index].fixed_reward.num = reward_item.num;
// 		m_myp[index].fixed_reward.is_bind = fixed_item->IsBind();
// 
// 		if (!role->GetRoleModuleManager()->GetMiYi()->IsWeekKilled(cfg->seq, monster_cfg->monster_seq))
// 		{
// 			role->AddExp(monster_cfg->add_exp);
// 			bool is_first = false;
// 
// 			m_myp[index].is_can_turn = 1;
// 			m_myp[index].is_turned = 0;
// 			m_myp[index].index = 0;
// 
// 			if (role->GetRoleModuleManager()->GetMiYi()->IsFirstKIll(cfg->seq, monster_cfg->monster_seq))
// 			{
// 				std::vector<ItemConfigData> temp = LOGIC_CONFIG->GetMiYiConfig()->GetRandRewardItem(monster_cfg->first_pass_reward_id_vec);
// 				rewards_first_kill.insert(rewards_first_kill.end(), temp.begin(), temp.end());
// 				role->GetRoleModuleManager()->GetMoney()->AddCoinBind(monster_cfg->add_coin, __FUNCTION__);
// 				
// 				is_first = true;
// 			}
// 
// 			role->GetRoleModuleManager()->GetMiYi()->SetKill(cfg->seq, monster_cfg->monster_seq);
// 
// 			if (rewards_first_kill.size() > 0)
// 			{
// 				role->GetRoleModuleManager()->GetKnapsack()->PutList((short) rewards_first_kill.size(), &rewards_first_kill[0], PUT_REASON_MI_YI);
// 			}
// 
// 			EventHandler::Instance().OnKillMiYiFBBoss(role, cfg->seq, monster_cfg->monster_seq + 1, is_first);
// 		} else
// 		{
// 			m_myp[index].is_can_turn = 0;
// 			m_myp[index].is_turned = 0;
// 			m_myp[index].index = 0;
// 
// 			role->NoticeNum(errornum::EN_MIYI_HAS_BEEN_POKER_GET);
// 		}
// 
// 		//role->GetRoleModuleManager()->GetMiYi()->SetPokerReward(m_myp[index]);
// 
// 		// 最后一次循环的时候
// 		if (index == (ack->attacker_list.role_count - 1))
// 		{
// 			int residue_arr_index = ack->attacker_list.role_count;
// 			for (int residue_i = residue_arr_index; residue_i < SIDE_MAX_ROLE_NUM; ++residue_i)  // 剩余的结构获得要下发的奖励id
// 			{
// 				
// 				std::vector<ItemConfigData> rewards_role_level_no = LOGIC_CONFIG->GetMiYiConfig()->GetRewardItem(monster_cfg->role_level_reward_group_id, cfg->need_role_level);
// 
// 				//if (m_myp[residue_i].role_uid != 0) m_myp[residue_i].Reset();
// 				//if (rewards_role_level_no.size() > 0)
// 				//{
// 				//	m_myp[residue_i].reward_id = rewards_role_level_no[0].item_id;
// 				//	m_myp[residue_i].reward_count = rewards_role_level_no[0].num;
// 				//
// 				//	// 翻牌奖励净值
// 				//	const ItemBase *item = ITEMPOOL->GetItem(m_myp[residue_i].reward_id);
// 				//	if (!item) continue;
// 				//	NetValueItemParam tmp_item_param;
// 				//	if (!item->HasRandomNetValue(&tmp_item_param))
// 				//	{
// 				//		item->RandomNetValueOnPutItem(NULL, &tmp_item_param, PUT_REASON_MI_YI);
// 				//		item->SetRandomNetValueFlag(&tmp_item_param);
// 				//		m_myp[residue_i].param_len = tmp_item_param.GetDataByItemType(item->GetItemType(), m_myp[residue_i].param_data);
// 				//		m_myp[residue_i].param_len_shengyu = sizeof(m_myp[residue_i].param_data) - m_myp[residue_i].param_len;
// 				//	}
// 				//}
// 
// 				POKER_MODULE.EndLoopRecorder(&m_myp[residue_i], rewards_role_level_no, PUT_REASON_MI_YI,false,true);
// 			}
// 
// 			this->SendPokerRoleLevelReward();
// 		}
// 
// 
// 		if (m_npc_monster_count <= 0)
// 		{
// 			const MiyiOtherCfg * other_cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiOtherCfg();
// 			if (NULL != other_cfg)
// 			{
// 				EventHandler::Instance().OnPassMiYiFB(role, cfg->seq, other_cfg->monster_num + 1);
// 
// 			}
// 		}
// 	}
// }

void SpecialLogicMiYi::SendPokerRoleLevelReward()
{
	Protocol::SCMiYiPoker miyi_info;

	int net_num = 0;
	for (int ind = 0; ind < SIDE_MAX_ROLE_NUM; ++ind)
	{
		memcpy(&miyi_info.mypoker[ind], &m_myp[ind], sizeof(miyi_info.mypoker[ind]));

		Role *once_role = m_scene->GetRoleByUID(m_myp[ind].role_uid);
		if (once_role != NULL)
		{
			gsnetidlist[net_num++] = once_role->GetNetId();
		}
	}

	EngineAdapter::Instance().NetMulSend(gsnetidlist, net_num, (const char *) &miyi_info, sizeof(miyi_info));
}

void SpecialLogicMiYi::RefreshNPCMonster()
{
	m_refreshed_monster = true;
	m_npc_monster_count = 0;
	const MiYiLevelCfg * cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiLevelCfgBySceneId(m_scene->GetSceneID());
	if (cfg == NULL)
	{
		return;
	}

	for (int i = 0; i < (int) cfg->monsters.size(); ++i)
	{
		NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(cfg->monsters[i].npc_monster_id, m_scene, cfg->monsters[i].npc_monster_pos, 0, &cfg->monsters[i].dir_y);

		if (monster == NULL)
		{
			return;
		}
		m_npc_monster_count += 1;
		monster->SetBattleMode(BATTLE_MODE_MI_YI);
		monster->SetParam1(cfg->monsters[i].monster_level);
	}
}

void SpecialLogicMiYi::SendSceneInfo(Role * role)
{
	Protocol::SCMiYiSceneInfo info;
	info.kill_flag = m_kill_flag;

	if (NULL == role)
	{
		m_scene->SendToRole((const char *)&info, sizeof(info));
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
	}
}

