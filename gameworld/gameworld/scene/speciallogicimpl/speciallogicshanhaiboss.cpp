#include "speciallogicshanhaiboss.hpp"

#include "scene/scene.h"
#include "obj/character/npc.h"
#include "obj/character/role.h"
#include "obj/character/robot.h"

#include "obj/character/npc_monster.h"

#include "global/team/team.hpp"
#include "global/team/teammanager.hpp"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"

#include "gameworld/world.h"
#include "gameworld/gamelog.h"
#include "gameworld/npc/npcmanager.h"
#include "gameworld/drop/droppool.hpp"
#include "gameworld/protocol/msgshanhaiboss.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "global/drawmanager/drawmanager.hpp"
#include "global/usercache/usercache.hpp"
#include "other/robot/robotmanager.hpp"

SpecialLogicShanHaiBoss::SpecialLogicShanHaiBoss(Scene * scene) : SpecialLogic(scene), m_is_init(false), m_boss_id(0), m_boss_seq_id(0)
{
	m_draw_group_id = 0;
	m_draw_group_timeout = 0;
}

SpecialLogicShanHaiBoss::~SpecialLogicShanHaiBoss()
{
}

void SpecialLogicShanHaiBoss::Update(unsigned long interval, time_t now_second)
{
}

void SpecialLogicShanHaiBoss::OnRoleEnterScene(Role * role)
{
	if (NULL == role) return;

	if (!m_is_init)
	{
		m_is_init = true;
	/*	for (int i = 0; i < SIDE_MAX_ROLE_NUM; ++i)
		{
			poker_list[i].Reset();
		}*/

		this->CreateMonster();
	}
}

void SpecialLogicShanHaiBoss::OnRoleLeaveScene(Role * role, bool is_logout)
{
	//随机出翻牌奖励后没翻牌就离线的玩家
// 	for(int i = 0; i < SIDE_MAX_ROLE_NUM; ++i)
// 	{
// 		if(poker_list[i].role_uid != role->GetUID()) continue;
// 
// 		if(1 == poker_list[i].is_can_turn && 0 == poker_list[i].is_turned)
// 		{
// 			const ItemBase* item_base = ITEMPOOL->GetItem(poker_list[i].item_id);
// 			if (NULL == item_base) return;
// 
// 			ItemDataWrapper item;
// 			item.item_id = poker_list[i].item_id;
// 			item.is_bind = item_base->IsBind();
// 			item.num = poker_list[i].item_num;
// 			memcpy(item.param_data, poker_list[i].param_data, sizeof(item.param_data));
// 
// 			poker_list[i].is_can_turn = 0;
// 			poker_list[i].is_turned = 1;
// 
// 			static MailContentParam contentparam;
// 			contentparam.Reset();
// 
// 			int length_1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_shanhaiboss_poker_title);
// 			int length_2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_shanhaiboss_poker_reward_content);
// 			contentparam.item_list[0] = ItemDataWrapper(item);
// 
// 			if (length_1 > 0 && length_2 > 0)
// 			{
// 				MailRoute::MailToUser(IntToUid(poker_list[i].role_uid), MAIL_REASON_DEFAULT, contentparam);
// 
// 				gamelog::g_log_shan_hai_boss.printf(LL_INFO, "SpecialLogicShanHaiBoss::OnRoleLeaveScene Leave_Patch_Poker user[%d, %s], level[%d], poker_item_id[%d], poker_item_num[%d], is_bind[%d]",
// 					role->GetUID(), role->GetName(), role->GetLevel(), item.item_id, item.num, item.is_bind ? 1 : 0);
// 			}
// 			break;
// 		}
// 	}
}

void SpecialLogicShanHaiBoss::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	/*if (BATTLE_MODE_SHAN_HAI_BOSS == ack->battle_mode &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}

	if (BATTLE_MODE_SHAN_HAI_BOSS == ack->battle_mode && RESULT_TYPE_ATTACKER_WIN == ack->battle_result)
	{
		int npc_seq_id = ack->business_data.monster_gen_id;
		if (ack->business_data.monster_gen_id != m_boss_seq_id)
		{
			return;
		}
		NPCMonster* npc_monster = NpcManager::Instance().GetNPCMonster(npc_seq_id);
		if (NULL == npc_monster) return;

		if (npc_monster->GetScene()->GetSceneID() != m_scene->GetSceneID() || npc_monster->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
		{
			return;
		}

		if (npc_monster->GetNpcId() == m_boss_id)
		{
			m_boss_id = INVALID_OBJ_ID;
		}

		//本身就是随机奖励不需要洗牌
		DrawGroupInfo draw_group_info;

		for (int i = 0; i < ack->attacker_list.role_count; ++i)
		{
			draw_group_info.draw_count++;

			//无人获得的奖励 拥有者为0
			int role_uid = 0;
			if (i < ack->attacker_list.role_count && 0 == ack->attacker_list.role_list[i].is_flyaway && 0 == ack->attacker_list.role_list[i].is_runaway)
			{
				role_uid = ack->attacker_list.role_list[i].uid;
			}

			//翻牌奖励
			ShanHaiBossPoker node; node.Reset();
			node.role_uid = role_uid;

			DrawGroupInfo::DrawItem &reward_info = draw_group_info.draw_info_list[i];
			reward_info.role_uid = role_uid;

			// 角色名
			GameName &role_name = node.role_name;

			Role * role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
			if (NULL != role)
			{
				const ShanHaiBossMonsterCfg* monster_cfg = LOGIC_CONFIG->GetShanHaiBossConfig()->GetMonsterCfg(npc_monster->GetNpcId());
				if (NULL == monster_cfg) continue;
				role->GetRoleModuleManager()->OnKillShanHaiMonster(monster_cfg->star, monster_cfg->npc_type);

				int add_coin = 0;
				std::vector<ItemConfigData> rewards;
				LOGIC_CONFIG->GetShanHaiBossConfig()->GetNpcNormalRewardCfg(npc_monster->GetNpcId(), &add_coin, &rewards);

				// 发放正常奖励
				if (add_coin > 0)
				{
					role->GetRoleModuleManager()->GetMoney()->AddCoinBind(add_coin, __FUNCTION__);
				}
				if (rewards.size() > 0)
				{
					role->GetRoleModuleManager()->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_SHAN_HAI_BOSS);
					role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)rewards.size(), &rewards[0], GET_REASON_SHAN_HAI_BOSS);
				}

				role->GetName(role_name);

				//翻牌奖励
				ItemConfigData poker_reward;
				LOGIC_CONFIG->GetShanHaiBossConfig()->GetPokerRewardCfg(npc_monster->GetNpcId(), &poker_reward);
				// 翻牌奖励设置
				DrawManager::Instance().PutRewardItem(reward_info, 1, &poker_reward, role->GetAppearanceProf());

				gamelog::g_log_shan_hai_boss.printf(LL_INFO, "SpecialLogicShanHaiBoss::OnFightFinish user[%d, %s], level[%d], poker_item_id[%d], poker_item_num[%d], is_bind[%d]",
					role->GetUID(), role->GetName(), role->GetLevel(), poker_reward.item_id, poker_reward.num, poker_reward.is_bind ? 1 : 0);
			}
			else               //随机翻牌奖励前就离线的玩家
			{
				//翻牌奖励
				ItemConfigData poker_reward;
				LOGIC_CONFIG->GetShanHaiBossConfig()->GetPokerRewardCfg(npc_monster->GetNpcId(), &poker_reward);
				// 翻牌奖励设置
				DrawManager::Instance().PutRewardItem(reward_info, 1, &poker_reward);

				gamelog::g_log_shan_hai_boss.printf(LL_INFO, "SpecialLogicShanHaiBoss::OnFightFinish OFFLINE Mail_Send_Poker user_uid[%d], poker_item_id[%d], poker_item_num[%d], is_bind[%d]",
					ack->attacker_list.role_list[i].uid, poker_reward.item_id, poker_reward.num, poker_reward.is_bind ? 1 : 0);	
		
				//角色名
				DrawManager::Instance().GetRoleOrRobotName(role_uid, role_name);

			}

			poker_list[i] = node;
	
		}

		DrawManager::Instance().AddDrawGroup(draw_group_info, DRAW_GROUP_TYPE_SHAN_HAI);
		m_draw_group_id = draw_group_info.draw_group_id;
		m_draw_group_timeout = draw_group_info.out_time;

		this->SendPokerRewardInfo();
		m_scene->DeleteObj(npc_monster->GetId());
	}*/
}

void SpecialLogicShanHaiBoss::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	if (BATTLE_MODE_SHAN_HAI_BOSS == ack->battle_mode &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}
}

void SpecialLogicShanHaiBoss::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{
	if (BATTLE_MODE_SHAN_HAI_BOSS == ack->battle_mode &&
		ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}
}

void SpecialLogicShanHaiBoss::OnRoleLeaveTeam(Role * role)
{
	this->DelayKickOutRole(role);
}

// void SpecialLogicShanHaiBoss::GetPoker(Role * role, int index)
// {
// 	if (NULL == role) return;
// 
// 	for (int i = 0; i < SIDE_MAX_ROLE_NUM; ++i)
// 	{
// 		if (poker_list[i].is_turned == 1) continue;
// 
// 		if (poker_list[i].role_uid == role->GetUID())
// 		{
// 			const ItemBase *item_base = ITEMPOOL->GetItem(poker_list[i].item_id);
// 			if (NULL == item_base) return;
// 
// 			ItemDataWrapper item;
// 			if (poker_list[i].is_can_turn == 1 && poker_list[i].is_turned == 0)
// 			{
// 				item.item_id = poker_list[i].item_id;
// 				item.is_bind = item_base->IsBind();
// 				item.num = poker_list[i].item_num;
// 				memcpy(item.param_data, poker_list[i].param_data, sizeof(item.param_data));
// 			}
// 
// 			poker_list[i].index = index;
// 			poker_list[i].is_can_turn = 0;
// 			poker_list[i].is_turned = 1;
// 
// 			role->GetRoleModuleManager()->GetKnapsack()->Put(item, PUT_REASON_SHAN_HAI_BOSS);
// 
// 			gamelog::g_log_shan_hai_boss.printf(LL_INFO, "SpecialLogicShanHaiBoss::GetPoker user[%d, %s], level[%d], poker_index[%d], poker_item_id[%d], poker_item_num[%d]",
// 				role->GetUID(), role->GetName(), role->GetLevel(), index, item.item_id, item.num);
// 			break;
// 		}
// 	}
// 	this->SendPokerRewardInfo();
// }

void SpecialLogicShanHaiBoss::SetInfo(int npc_id, int max_level)
{
	m_boss_id = npc_id;
	m_team_max_level = max_level;
}

void SpecialLogicShanHaiBoss::CreateMonster()
{
	/*const ShanHaiBossBossCfg * cfg = LOGIC_CONFIG->GetShanHaiBossConfig()->GetBossFromNpcCfg(m_boss_id, m_team_max_level);
	if (cfg == NULL)
	{
		return;
	}

	NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(cfg->npc_id, m_scene, cfg->boss_pos, BATTLE_MODE_SHAN_HAI_BOSS, &cfg->monster_dir);
	if (NULL != monster)
	{
		m_boss_seq_id = monster->GetNpcSeq();
	}*/
}

void SpecialLogicShanHaiBoss::SendPokerRewardInfo()
{
	/*GSNetID netid_list[SIDE_MAX_ROLE_NUM];

	Protocol::SCShanHaiBossPokerInfo info;
	info.m_draw_group_id = m_draw_group_id;
	info.timeout = m_draw_group_timeout;
	int net_num = 0;
	for (int i = 0; i < SIDE_MAX_ROLE_NUM; ++i)
	{
		memcpy(&info.poker_data[i], &poker_list[i], sizeof(info.poker_data[i]));

		Role *once_role = m_scene->GetRoleByUID(poker_list[i].role_uid);
		if (once_role != NULL)
		{
			netid_list[net_num++] = once_role->GetNetId();
		}
	}

	EngineAdapter::Instance().NetMulSend(netid_list, net_num, (const char *)&info, sizeof(info));*/
}
