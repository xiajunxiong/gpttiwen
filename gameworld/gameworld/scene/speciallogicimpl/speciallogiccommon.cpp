#include "speciallogiccommon.hpp"
#include "scene/scene.h"
#include "item/knapsack.h"
#include "engineadapter.h"
#include "npc/npcmanager.h"

#include "obj/character/role.h"
#include "obj/character/npc_monster.h"

#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "other/route/mailroute.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/robot/robotmanager.hpp"
#include "other/event/eventhandler.hpp"

#include "config/logicconfigmanager.hpp"
#include "global/wildbossmanager/wildbossconfig.hpp"
#include "global/wildbossmanager/wildbossmanager.h"
#include "global/activity/activitymanager.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/usercache/usercache.hpp"
#include "servercommon/string/gameworldstr.h"
#include "protocol/msgdraw.h"
#include "protocol/msgwildboss.h"
#include "protocol/msghundredghost.h"
#include "other/roleactivity/impl/roleactivitytujiantreasure.hpp"
#include "global/activity/activityimpl/activityhundredghost.hpp"
#include "global/shanhaibossmanager/shanhaibossconfig.hpp"
#include "global/shanhaibossmanager/shanhaibossmanager.hpp"
#include "global/team/team.hpp"
#include "gamelog.h"

#include "scene/scenemanager.hpp"
#include "gamelog.h"
#include "other/poker/pokermodulemanager.hpp"
#include "global/drawmanager/drawmanager.hpp"
#include "config/activityconfig/hundredghost/hundredghostconfig.hpp"
#include "other/duel/duel.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "other/advancecareer/advancecareer.hpp"

SpecialLogicCommon::SpecialLogicCommon(Scene* scene) : SpecialLogic(scene)
{

}

SpecialLogicCommon::~SpecialLogicCommon()
{

}

void SpecialLogicCommon::Update(unsigned long interval, time_t now_second)
{
}

void SpecialLogicCommon::OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack)
{
	if (BATTLE_MODE_FIELD_BOSS == ack->battle_mode)
	{
		this->FieldBossOnFightFinish(ack);
	}
	else if (BATTLE_MODE_EXPEL_MONSTER == ack->battle_mode ||
		BATTLE_MODE_MO_ZU_ZAI_NA_LI == ack->battle_mode)
	{
		int npc_seq_id = ack->business_data.monster_gen_id;
		NPCMonster* npc_monster = NpcManager::Instance().GetNPCMonster(npc_seq_id);
		if (NULL == npc_monster) return;

		if (npc_monster->GetScene()->GetSceneID() != m_scene->GetSceneID() ||
			npc_monster->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
		{
			return;
		}

		if (RESULT_TYPE_ATTACKER_WIN == ack->battle_result)
		{
			//ɾ��boss
			m_scene->DeleteObj(npc_monster->GetId());
		} else
		{
			//���˾Ͱ�BOSS����ȥ�� ��������Ҳ���Դ�
			npc_monster->SetMonsterLock(false);
		}
	} 
	else if (BATTLE_MODE_MINE_MONSTER == ack->battle_mode)
	{
		if (RESULT_TYPE_ATTACKER_WIN == ack->battle_result)
		{
			for (int i = 0; i < ack->attacker_list.role_count; ++i)
			{
				Role * role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
				if (NULL != role)
				{
					role->GetRoleModuleManager()->GetRoleActivityManager()->OnGiveRoleActivityItem(ACTIVITY_GIVE_ITEM_TYPE_PATROL_MINING);
				}
			}
		}
	}
	else if (BATTLE_MODE_SHAN_HAI_BOSS == ack->battle_mode)
	{
		this->ShanHaiBossOnFightFinish(ack);
	}
	else if (BATTLE_MODE_HUNDRED_GHOST == ack->battle_mode)
	{
		this->HundredGhostOnFightFinish(ack);
	}
	else if (BATTLE_MODE_DUEL == ack->battle_mode)
	{
		this->DuelOnFightFinish(ack);
	}
}

void SpecialLogicCommon::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	if (BATTLE_MODE_SHAN_HAI_BOSS == ack->battle_mode)
	{
		Role* role = m_scene->GetRoleByUID(ack->uid);
		if (NULL != role)
		{
			role->GetRoleModuleManager()->OnShanHaiRunAwayOrForceFinishBattle();
		}
	}
}

void SpecialLogicCommon::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{
	int npc_seq_id = ack->business_data.monster_gen_id;
	NPCMonster* npc_monster = NpcManager::Instance().GetNPCMonster(npc_seq_id);
	if (NULL == npc_monster)
	{
		return;
	}

	if (npc_monster->GetScene()->GetSceneID() != m_scene->GetSceneID() ||
		npc_monster->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
	{
		return;
	}


	if (BATTLE_MODE_SHAN_HAI_BOSS == ack->battle_mode)
	{
		if ((unsigned int)ack->business_data.param3 == ShanHaiBossManager::Instance().GetNextRefreshTime())
		{
			npc_monster->DecBattleTeamNum();
		}

		for (int i = 0; i < SIDE_MAX_ROLE_NUM && i < DRAW_GROUP_MAX_ITEM_NUM; ++i)
		{
			int role_uid = 0;
			if (i < ack->attacker_list.role_count && 0 == ack->attacker_list.role_list[i].is_flyaway && 0 == ack->attacker_list.role_list[i].is_runaway)
			{
				role_uid = ack->attacker_list.role_list[i].uid;
			}

			Role* role = m_scene->GetRoleByUID(role_uid);
			if (NULL != role)
			{
				role->GetRoleModuleManager()->OnShanHaiRunAwayOrForceFinishBattle();
			}
		}
	}
	else if (BATTLE_MODE_HUNDRED_GHOST == ack->battle_mode)
	{
		npc_monster->SetMonsterLock(false);
	}
}

bool SpecialLogicCommon::CreateWildBoss(int npc_monster_id, int cfg_seq, Posi *out_posi, int * monster_group_id)
{
	NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(npc_monster_id, m_scene, m_scene->GetARandPosi());
	if (NULL != monster)
	{
		monster->SetBattleMode(BATTLE_MODE_FIELD_BOSS);
		monster->SetParam1(cfg_seq);
		if (out_posi != NULL)*out_posi = monster->GetPos();
		if (NULL != monster_group_id) *monster_group_id = monster->GetMonsterGroupId();
		return true;
	}

	return false;
}



bool SpecialLogicCommon::CreateShanHaiBoss(int npc_monster_id, int* out_npc_seq, Posi posi)
{
	NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(npc_monster_id, m_scene, posi);
	if (NULL != monster)
	{
		monster->SetBattleMode(BATTLE_MODE_SHAN_HAI_BOSS);
		*out_npc_seq = monster->GetNpcSeq();
		return true;
	}

	return false;
}

bool SpecialLogicCommon::CreateHundredGhostMonster(int npc_monster_id, int * out_npc_seq, Posi in_posi)
{
	NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(npc_monster_id, m_scene, in_posi);
	if (NULL != monster)
	{
		monster->SetBattleMode(BATTLE_MODE_HUNDRED_GHOST);
		*out_npc_seq = monster->GetNpcSeq();
		return true;
	}

	return false;
}

void SpecialLogicCommon::FieldBossOnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (NULL == ack)
	{
		return;
	}

	if (BATTLE_MODE_FIELD_BOSS != ack->battle_mode)
	{
		return;
	}
	
	int npc_seq_id = ack->business_data.monster_gen_id;
	NPCMonster* npc_monster = NpcManager::Instance().GetNPCMonster(npc_seq_id);
	if (NULL == npc_monster) return;

	if (npc_monster->GetScene()->GetSceneID() != m_scene->GetSceneID() ||
		npc_monster->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
	{
		return;
	}

	if (RESULT_TYPE_ATTACKER_WIN != ack->battle_result)
	{
		//���˾Ͱ�BOSS����ȥ�� ��������Ҳ���Դ�
		npc_monster->SetMonsterLock(false);
		return;
	}

	//ʤ��
	WildBossManager::Instance().OnKillMonster(npc_monster->GetParam1());

	//������
	const WildBossOtherCfg & other_cfg = LOGIC_CONFIG->GetWildBossConfig()->GetOtherCfg();

	//������������������Ҫϴ��
	DrawGroupInfo draw_group_info;

	//֪ͨ��Ϣ
	int tmp_role_count = 0;
	Role* tmp_role_list[SIDE_MAX_ROLE_NUM] = { 0 };
	Protocol::SCDrawPokerInfo info;
	WildbossPoker wbpoker[SIDE_MAX_ROLE_NUM];
	for (int ind = 0; ind < SIDE_MAX_ROLE_NUM && ind < DRAW_GROUP_MAX_ITEM_NUM; ++ind)
	{
		//���˻�õĽ��� ӵ����Ϊ0
		int role_uid = 0;

		if (ind < ack->attacker_list.role_count && 0 == ack->attacker_list.role_list[ind].is_flyaway && 0 == ack->attacker_list.role_list[ind].is_runaway)
		{
			role_uid = ack->attacker_list.role_list[ind].uid;
		}

		DrawGroupInfo::DrawItem &reward_info = draw_group_info.draw_info_list[ind];
		reward_info.role_uid = role_uid;

		// ��ɫ��
		GameName &role_name = wbpoker[ind].role_name;

		// ��������
		int coin = 0;
		int luck_coin = 0;
		std::vector<ItemConfigData> rewards;

		// ���ƽ���
		int poker_coin = 0;				// ����֮�⣬�����ô�
		int poker_luck_coin = 0;		// ����֮�⣬�����ô�
		std::vector<ItemConfigData> poker_rewards;

		Role * role = m_scene->GetRoleByUID(role_uid);
		if (NULL == role)
		{// �����˻��߲������ߵĶ���
			LOGIC_CONFIG->GetWildBossConfig()->GetPokerRewardItem(m_scene->GetSceneID(), npc_monster->GetNpcId(), &poker_coin, &poker_luck_coin, poker_rewards);
			// ���ƽ�������
			if (poker_rewards.size() > 0)
			{
				DrawManager::Instance().PutRewardItem(reward_info, (int)poker_rewards.size(), &poker_rewards[0]);
			}

			//������
			Robot *robot = RobotManager::Instance().GetRobot(role_uid);
			if (NULL != robot)
			{
				robot->GetName(role_name);
			}

			//�������Ҳû�н���
			//��ɫ����
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

			role->GetName(role_name);

			// ��������
			int get_reward_times = role->GetWildBossRewardTimes();
			if (get_reward_times >= other_cfg.get_reward_times)
			{
				LOGIC_CONFIG->GetWildBossConfig()->GetRandRewardItem(m_scene->GetSceneID(), npc_monster->GetNpcId(), &coin, &luck_coin, rewards, true);
				LOGIC_CONFIG->GetWildBossConfig()->GetPokerRewardItem(m_scene->GetSceneID(), npc_monster->GetNpcId(), &poker_coin, &poker_luck_coin, poker_rewards, true);
			}
			else
			{
				LOGIC_CONFIG->GetWildBossConfig()->GetRandRewardItem(m_scene->GetSceneID(), npc_monster->GetNpcId(), &coin, &luck_coin, rewards);
				LOGIC_CONFIG->GetWildBossConfig()->GetPokerRewardItem(m_scene->GetSceneID(), npc_monster->GetNpcId(), &poker_coin, &poker_luck_coin, poker_rewards);
			}

			// ������������
			if (rewards.size() > 0)
			{
				role->GetRoleModuleManager()->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_WILD_BOSS);
				role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)rewards.size(), &rewards[0], GET_REASON_WILD_BOSS);
			}
			if (coin > 0)
			{
				role->GetRoleModuleManager()->GetMoney()->AddCoinBind(coin, __FUNCTION__);
				reward_info.money_type[DRAW_GROUP_VIRTUAL_TYPE_LUCK_COIN] = luck_coin;
			}

			// ���ƽ�������
			if (poker_rewards.size() > 0)
			{
				DrawManager::Instance().PutRewardItem(reward_info, (int)poker_rewards.size(), &poker_rewards[0], role->GetAppearanceProf());
			}

			role->GetRoleModuleManager()->GetRoleActivityManager()->OnGiveRoleActivityItem(ACTIVITY_GIVE_ITEM_TYPE_FIELD_BOSS, npc_monster->GetNpcId());
			role->SetWildBossRewardTimes(get_reward_times + 1);
			WildBossManager::Instance().SendInfoToRole(role);

			//ͼ���ᱦ�
			RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
			if (NULL != ramgr && ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE))
			{
				RoleActivityTuJianTreasure *role_activity = static_cast<RoleActivityTuJianTreasure *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE));
				if (NULL != role_activity && role_activity->IsRoleTuJianTreasureOpen())
				{
					role_activity->OnTuJianTreasureCheckReward(TU_JIAN_TREASURE_TYPE_WLID_BOSS, npc_monster->GetNpcId(), rewards);
				}
			}
		}

		draw_group_info.draw_count++;

		wbpoker[ind].index = reward_info.draw_index;
		wbpoker[ind].role_uid = reward_info.role_uid;
		wbpoker[ind].luck_coin = luck_coin;
		if (!reward_info.item_list.empty())
		{//֮ǰ���߼��������� ֻ֪ͨ��һ����Ʒ
			DrawManager::Instance().GetItemInfoType(wbpoker[ind].reward_info, reward_info.item_list[0]);
		}
	}

	DrawManager::Instance().AddDrawGroup(draw_group_info, DRAW_GROUP_TYPE_FIELD_BOSS);

	//��֪ͨ
	info.draw_group_id = draw_group_info.draw_group_id;
	info.timeout = draw_group_info.out_time;
	info.draw_group_type = DRAW_GROUP_TYPE_FIELD_BOSS;

	for (int k = 0; k < SIDE_MAX_ROLE_NUM; k++)
	{
		memcpy(info.draw_poker[k].data, &wbpoker[k], sizeof(WildbossPoker));
	}
	UNSTD_STATIC_CHECK(sizeof(WildbossPoker) <= MAX_ITEM_LENGTH_NUM);

	for (int i = 0; i < tmp_role_count && i < SIDE_MAX_ROLE_NUM; i++)
	{
		if (NULL == tmp_role_list[i])
		{
			continue;
		}
		EngineAdapter::Instance().NetSendSerializeMsg2(tmp_role_list[i]->GetNetId(), &info);
	}

	//ɾ��boss
	m_scene->DeleteObj(npc_monster->GetId());
}

void SpecialLogicCommon::ShanHaiBossOnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (NULL == ack)
	{
		return;
	}

	if (BATTLE_MODE_SHAN_HAI_BOSS != ack->battle_mode)
	{
		return;
	}

	int npc_seq_id = ack->business_data.monster_gen_id;
	int monster_npc_id = ack->business_data.param1;

	bool boss_being_refresh = false;
	if ((unsigned int)ack->business_data.param3 != ShanHaiBossManager::Instance().GetNextRefreshTime())
	{
		boss_being_refresh = true;	//ɽ��������ˢ����NPC�ѱ�delete
	}
	
	NPCMonster* npc_monster = NpcManager::Instance().GetNPCMonster(npc_seq_id);
	if (!boss_being_refresh)
	{
		if (NULL == npc_monster)
		{
			return;
		}

		if (npc_monster->GetScene()->GetSceneID() != m_scene->GetSceneID() ||
			npc_monster->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
		{
			return;
		}

		npc_monster->DecBattleTeamNum();
		monster_npc_id = npc_monster->GetNpcId();
	}


	if (RESULT_TYPE_ATTACKER_WIN != ack->battle_result)
	{//ʧ��
		return;
	}

	if (!boss_being_refresh)
	{
		//����һ����������
		npc_monster->AddDeathNum();
		ShanHaiBossManager::Instance().OnKillMonster(npc_seq_id, monster_npc_id);
	}

	const ShanHaiBossMonsterCfg* monster_cfg = LOGIC_CONFIG->GetShanHaiBossConfig()->GetMonsterCfg(monster_npc_id);
	if (NULL == monster_cfg)
	{
		return;
	}

	//������������������Ҫϴ��
	//DrawGroupInfo draw_group_info;		//��ȡ�����ƣ�

	//֪ͨ��Ϣ
	int tmp_role_count = 0;
	Role* tmp_role_list[SIDE_MAX_ROLE_NUM] = { 0 };
	Protocol::SCDrawPokerInfo info;
	DrawCrystalPoker poker_info[SIDE_MAX_ROLE_NUM];
	for (int ind = 0; ind < ack->attacker_list.role_count  && ind < DRAW_GROUP_MAX_ITEM_NUM; ++ind)
	{
		//���˻�õĽ��� ӵ����Ϊ0
		int role_uid = 0;

		if (ind < ack->attacker_list.role_count && 0 == ack->attacker_list.role_list[ind].is_runaway)
		{
			role_uid = ack->attacker_list.role_list[ind].uid;
		}
		//DrawGroupInfo::DrawItem &reward_info = draw_group_info.draw_info_list[ind];
		//reward_info.role_uid = role_uid;
	
		// ��ɫ��
		GameName &role_name = poker_info[ind].role_name;

		// ��������
		int coin = 0;
		int luck_coin = 0;
		std::vector<ItemConfigData> rewards;

		// ���ƽ���
		int poker_coin = 0;
		int poker_luck_coin = 0;
		std::vector<ItemConfigData> poker_rewards;

		Role * role = m_scene->GetRoleByUID(role_uid);
		if (NULL == role)
		{
			/*LOGIC_CONFIG->GetShanHaiBossConfig()->GetPokerRewardItem(monster_npc_id, &poker_coin, &poker_luck_coin, poker_rewards, true);
			// ���ƽ�������
			if (poker_rewards.size() > 0)
			{
				DrawManager::Instance().PutRewardItem(reward_info, (int)poker_rewards.size(), &poker_rewards[0]);
			}*/

			//������
			Robot *robot = RobotManager::Instance().GetRobot(role_uid);
			if (NULL != robot)
			{
				robot->GetName(role_name);
			}

			//��ɫ����
			UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(role_uid);
			if (NULL != user_cache_node)
			{
				user_cache_node->GetName(role_name);
				//reward_info.role_uid = 0;
			}
		}
		else
		{
			tmp_role_list[tmp_role_count] = role;
			tmp_role_count++;

			role->GetName(role_name);

			role->GetRoleModuleManager()->OnKillShanHaiMonster(monster_npc_id, monster_cfg->seq);
			role->GetRoleModuleManager()->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_SHAN_HAI_BOSS, monster_cfg->grade_type , 0);
			ShanHaiBossManager::Instance().SendRoleInfo(role);
			EventHandler::Instance().OnPassShanHai(role, monster_cfg->grade_type);


			// ��������
			int daily_reward_times = role->GetRoleModuleManager()->GetShanHaiBossChallengeTime();
			if (daily_reward_times > LOGIC_CONFIG->GetShanHaiBossConfig()->GetOtherCfg().day_time)
			{
				LOGIC_CONFIG->GetShanHaiBossConfig()->GetRandRewardItem(monster_npc_id, &coin, &luck_coin, rewards, role->GetLevel(), true);
				//LOGIC_CONFIG->GetShanHaiBossConfig()->GetPokerRewardItem(monster_npc_id, &poker_coin, &poker_luck_coin, poker_rewards, true);
			}
			else
			{
				LOGIC_CONFIG->GetShanHaiBossConfig()->GetRandRewardItem(monster_npc_id, &coin, &luck_coin, rewards, role->GetLevel(), false);
				//LOGIC_CONFIG->GetShanHaiBossConfig()->GetPokerRewardItem(monster_npc_id, &poker_coin, &poker_luck_coin, poker_rewards, false);
			}

			// ������������
			if (coin > 0)
			{
				role->GetRoleModuleManager()->GetMoney()->AddCoinBind(coin, __FUNCTION__);
			}
			if (rewards.size() > 0)
			{
				role->GetRoleModuleManager()->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_SHAN_HAI_BOSS);
				role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)rewards.size(), &rewards[0], GET_REASON_SHAN_HAI_BOSS);

				for (int i = 0; i < (int)rewards.size(); ++i)
				{
					if (LOGIC_CONFIG->GetShanHaiBossConfig()->IsGetItemNotice(rewards[i].item_id))
					{
						int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_shanhaiboss_reward_notice, role->GetName(), rewards[i].item_id);
						if (length > 0)
						{
							World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
								0, 0, 0, 0);
						}
					}
				}
			}

			// ���ƽ�������
			/*if (poker_rewards.size() > 0)
			{
				DrawManager::Instance().PutRewardItem(reward_info, (int)poker_rewards.size(), &poker_rewards[0], role->GetAppearanceProf());
			}*/
		}
	
		/*draw_group_info.draw_count++;

		//Э����Ϣ
		info.poker_data[ind].index = reward_info.draw_index;
		info.poker_data[ind].role_uid = reward_info.role_uid;
		if (!reward_info.item_list.empty())
		{//֮ǰ���߼��������� ֻ֪ͨ��һ����Ʒ
			DrawManager::Instance().GetItemInfoType(info.poker_data[ind].reward_info, reward_info.item_list[0]);
		}*/
	}

	//DrawManager::Instance().AddDrawGroup(draw_group_info, DRAW_GROUP_TYPE_SHAN_HAI);

	//��֪ͨ
	/*info.m_draw_group_id = draw_group_info.draw_group_id;
	info.timeout = draw_group_info.out_time;
	info.draw_group_type = DRAW_GROUP_TYPE_SHAN_HAI;
	for (int k = 0; k < SIDE_MAX_ROLE_NUM; k++)
	{
		memcpy(info.draw_poker[k].data, &poker_info[k], sizeof(DrawCrystalPoker));
	}

	for (int i = 0; i < tmp_role_count && i < SIDE_MAX_ROLE_NUM; i++)
	{
		if (NULL == tmp_role_list[i])
		{
			continue;
		}
		EngineAdapter::Instance().NetSendSerializeMsg2(tmp_role_list[i]->GetNetId(), &info);
	}*/

	if (!boss_being_refresh)
	{
		if (npc_monster->GetDeathNum() >= monster_cfg->fight_times)
		{
			//ɾ��boss
			m_scene->DeleteObj(npc_monster->GetId());
		}
	}
}

void SpecialLogicCommon::HundredGhostOnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (NULL == ack)
	{
		return;
	}

	if (BATTLE_MODE_HUNDRED_GHOST != ack->battle_mode)
	{
		return;
	}

	ActivityHundredGhost* act = static_cast<ActivityHundredGhost*>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_HUNDRED_GHOST));
	if (NULL == act)
	{
		return;
	}

	int npc_seq_id = ack->business_data.monster_gen_id;
	NPCMonster* npc_monster = NpcManager::Instance().GetNPCMonster(npc_seq_id);
	if (NULL == npc_monster) return;

	if (npc_monster->GetScene()->GetSceneID() != m_scene->GetSceneID() ||
		npc_monster->GetScene()->GetSceneKey() != m_scene->GetSceneKey())
	{
		return;
	}

	if (RESULT_TYPE_ATTACKER_WIN != ack->battle_result)
	{
		//���˾Ͱ�BOSS����ȥ�� ��������Ҳ���Դ�
		npc_monster->SetMonsterLock(false);
		return;
	}

	//ʤ��
	act->OnKillMonster(npc_seq_id);

	//������������������Ҫϴ��
	//DrawGroupInfo draw_group_info;		//��ȡ�����ƣ�

	//֪ͨ��Ϣ
	int tmp_role_count = 0;
	Role* tmp_role_list[SIDE_MAX_ROLE_NUM] = { 0 };
	Protocol::SCDrawPokerInfo info;
	DrawCrystalPoker poker_info[SIDE_MAX_ROLE_NUM];
	for (int ind = 0; ind < SIDE_MAX_ROLE_NUM && ind < DRAW_GROUP_MAX_ITEM_NUM; ++ind)
	{
		//���˻�õĽ��� ӵ����Ϊ0
		int role_uid = 0;

		if (ind < ack->attacker_list.role_count && 0 == ack->attacker_list.role_list[ind].is_flyaway && 0 == ack->attacker_list.role_list[ind].is_runaway)
		{
			role_uid = ack->attacker_list.role_list[ind].uid;
		}

		//DrawGroupInfo::DrawItem &reward_info = draw_group_info.draw_info_list[ind];
		//reward_info.role_uid = role_uid;

		// ��ɫ��
		GameName &role_name = poker_info[ind].role_name;

		// ���ƽ���
		int poker_coin = 0;				// ����֮�⣬�����ô�
		int poker_luck_coin = 0;		// ����֮�⣬�����ô�
		std::vector<ItemConfigData> poker_rewards;

		// ��������
		int coin = 0;
		int luck_coin = 0;
		std::vector<ItemConfigData> rewards;

		Role * role = m_scene->GetRoleByUID(role_uid);
		if (NULL == role)
		{
			// �����˻��߲������ߵĶ���
			/*const HundredGhostMonsterCfg* monster_cfg = LOGIC_CONFIG->GetHundredGhostConfig()->GetMonsterCfg(npc_monster->GetNpcId());
			if (NULL != monster_cfg)
			{
				const HundredGhostRewardCfg* reward_cfg = LOGIC_CONFIG->GetHundredGhostConfig()->GetRewradCfg(monster_cfg->poker_reward, 1);
				if (NULL != reward_cfg)
				{
					poker_rewards.push_back(reward_cfg->item);
				}
			}
			// ���ƽ�������
			if (poker_rewards.size() > 0)
			{
				DrawManager::Instance().PutRewardItem(reward_info, (int)poker_rewards.size(), &poker_rewards[0]);
			}*/

			//������
			Robot *robot = RobotManager::Instance().GetRobot(role_uid);
			if (NULL != robot)
			{
				robot->GetName(role_name);
			}

			//�������Ҳû�н���
			//��ɫ����
			UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(role_uid);
			if (NULL != user_cache_node)
			{
				user_cache_node->GetName(role_name);
				//reward_info.role_uid = 0;
			}
		}
		else
		{
			tmp_role_list[tmp_role_count] = role;
			tmp_role_count++;

			role->GetName(role_name);

			/*const HundredGhostMonsterCfg* monster_cfg = LOGIC_CONFIG->GetHundredGhostConfig()->GetMonsterCfg(npc_monster->GetNpcId());
			if (NULL != monster_cfg)
			{
				const HundredGhostRewardCfg* reward_cfg = LOGIC_CONFIG->GetHundredGhostConfig()->GetRewradCfg(monster_cfg->poker_reward, role->GetLevel());
				if (NULL != reward_cfg)
				{
					poker_rewards.push_back(reward_cfg->item);
				}
			}

			// ���ƽ�������
			if (poker_rewards.size() > 0)
			{
				DrawManager::Instance().PutRewardItem(reward_info, (int)poker_rewards.size(), &poker_rewards[0], role->GetAppearanceProf());
			}*/

			LOGIC_CONFIG->GetHundredGhostConfig()->GetRewradCfg(npc_monster->GetNpcId(), role->GetLevel(), &coin, rewards);
			if (coin > 0)
			{
				role->GetRoleModuleManager()->GetMoney()->AddCoinBind(coin, __FUNCTION__, false, true);
			}
			if (rewards.size() > 0)
			{
				role->GetRoleModuleManager()->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_HUNDRED_GHOST);
				role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList3(0, coin, 0, (short)rewards.size(), &rewards[0], GET_REASON_HUNDRED_GHOST);

				for (int i = 0; i < (int)rewards.size(); ++i)
				{
					if (LOGIC_CONFIG->GetHundredGhostConfig()->IsItemNotice(rewards[i].item_id))
					{
						int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_hundred_ghost_poker_reward_notice, role->GetName(), rewards[i].item_id);
						if (length > 0)
						{
							World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
								0, 0, 0, 0);
						}
					}
				}
			}

			EventHandler::Instance().OnParticipateLimitActivity(role, ACTIVITY_TYPE_HUNDRED_GHOST, __FUNCTION__, role->GetGuildID());

		}

		/*draw_group_info.draw_count++;

		//Э����Ϣ
		info.poker_data[ind].index = reward_info.draw_index;
		info.poker_data[ind].role_uid = reward_info.role_uid;
		if (!reward_info.item_list.empty())
		{//֮ǰ���߼��������� ֻ֪ͨ��һ����Ʒ
			DrawManager::Instance().GetItemInfoType(info.poker_data[ind].reward_info, reward_info.item_list[0]);
		}*/
	}

	//DrawManager::Instance().AddDrawGroup(draw_group_info, DRAW_GROUP_TYPE_HUNDRED_GHOST);

	//��֪ͨ
	/*info.draw_group_id = draw_group_info.draw_group_id;
	info.timeout = draw_group_info.out_time;
	info.draw_group_type = DRAW_GROUP_TYPE_HUNDRED_GHOST;
	for (int k = 0; k < SIDE_MAX_ROLE_NUM; k++)
	{
		memcpy(info.draw_poker[k].data, &poker_info[k], sizeof(DrawCrystalPoker));
	}
	for (int i = 0; i < tmp_role_count && i < SIDE_MAX_ROLE_NUM; i++)
	{
		if (NULL == tmp_role_list[i])
		{
			continue;
		}
		EngineAdapter::Instance().NetSendSerializeMsg2(tmp_role_list[i]->GetNetId(), &info);
	}*/

	//ɾ��boss
	m_scene->DeleteObj(npc_monster->GetId());
}


