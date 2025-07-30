#include "speciallogicchallengefb.hpp"
#include "world.h"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/challengefb/rolechallengefb.hpp"
#include "other/challengefb/challengefbconfig.hpp"
#include "scene/map.h"
#include "scene/scene.h"
#include "npc/npcmanager.h"
#include "obj/character/role.h"
#include "protocol/msgchallengefb.h"
#include "item/knapsack.h"
#include "item/itempool.h"
#include "item/expense/norexitem.hpp"

SpecialLogicChallengeFb::SpecialLogicChallengeFb(Scene * scene) : SpecialLogic(scene), m_fb_id(0), m_chapter(0), m_play_type(0), m_count_param(0)
{
}

SpecialLogicChallengeFb::~SpecialLogicChallengeFb()
{
}

bool SpecialLogicChallengeFb::IsForceEncounterMineMonster()
{
	bool is_force_ecnter = false;
	switch (m_play_type)
	{
	case CF_TYPE_KILL_APPO_MON_NUM:
		is_force_ecnter = true;
		break;
	}

	return is_force_ecnter;
}

void SpecialLogicChallengeFb::OnRoleEnterScene(Role * role)
{
	if (NULL == role) return;

	switch (m_play_type)
	{
	case CF_TYPE_KILL_APPO_MON_NUM:
		role->SetEncounterMonsterFlag(true);
		break;
	}
}

void SpecialLogicChallengeFb::OnRoleLeaveScene(Role * role, bool is_logout)
{
	if (NULL == role) return;

	switch (m_play_type)
	{
	case CF_TYPE_KILL_APPO_MON_NUM:
		role->RestoreLastEncounterMonsterFlag();
		break;
	}
}

void SpecialLogicChallengeFb::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (BATTLE_MODE_CHALLENGE_FB != ack->battle_mode) return;

	if (ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}

	if (RESULT_TYPE_ATTACKER_WIN != ack->battle_result) return;

	switch (m_play_type)
	{
	case CF_TYPE_BASICAL_PLAY_TYPE:
		{
			int npc_seq_id = ack->business_data.monster_gen_id;

			this->OnKillAMonster(npc_seq_id);
			if (npc_seq_id == m_count_param) this->OnFinishChallFb();
		}
		break;
	case CF_TYPE_KILL_APPO_MON_NUM:
		{
			const BattleData_Statistic & data = ack->statistic_data;
			if (data.monster_list_size <= 0) return;

			const ChallChapterInfor * fb_info = LOGIC_CONFIG->GetChallengeFbConfig()->GetChapterInfor(m_fb_id, m_chapter);
			if (NULL == fb_info) return;

			std::set<int> tmp_mon_id_checker;
			for (int list_idx = 0; list_idx < fb_info->param1_list_num && list_idx < PARAM_1_LIST_COUNT; ++list_idx)
			{
				tmp_mon_id_checker.insert(fb_info->param1_list[list_idx]);
			}

			for (int mon_idx = 0; mon_idx < data.monster_list_size && mon_idx < SIDE_CHARACTER_NUM; ++mon_idx)
			{
				if (tmp_mon_id_checker.end() != tmp_mon_id_checker.find(data.monster_id_list[mon_idx]))
				{
					++m_count_param;
				}
			}

			if (m_count_param >= fb_info->param2) this->OnFinishChallFb();
		}
		break;
	case CF_TYPE_KILL_MON_IN_ORDER:
		{
			int npc_seq_id = ack->business_data.monster_gen_id;

			bool is_last = false;
			if (this->CheckIsRightOrder(npc_seq_id, is_last))	// 顺序正确，此明雷消失；
			{
				this->OnKillAMonster(npc_seq_id);
				if (is_last) this->OnFinishChallFb();
			}
			else		// 顺序错误，此明雷不消失，其它怪物复活，顺序重置；
			{
				m_count_param = 0;
				this->ResurgenceAllMonster();
			}
		}
		break;
	}
}

void SpecialLogicChallengeFb::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack)
{
	if (ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}
}

void SpecialLogicChallengeFb::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack)
{
	if (ack->special_business_data.data_length >= (int)sizeof(BattleFBData) &&
		BATTLE_BUSINESS_DATA_TYPE_FB == ack->special_business_data.data_type)
	{
		this->SyncHpMp(ack->special_business_data.data);
	}
}

bool SpecialLogicChallengeFb::CanUseItem(Role * role, ItemID item_id, int num)
{
	if (NULL == role) return false;

	const ItemBase * item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return false;

	if (I_TYPE_EXPENSE != item_base->GetItemType()) return true;

	NorexItem * norex_item = (NorexItem *)item_base;
	if (NULL == norex_item) return false;

	int use_type = norex_item->GetUseType();

	int times_limit = LOGIC_CONFIG->GetChallengeFbConfig()->GetItemUseTimesLimit(m_fb_id, m_chapter, use_type);
	if (times_limit >= MAX_INT) return true;

	return m_item_use_times_map[role->GetUID()][use_type] + num <= times_limit;
}

void SpecialLogicChallengeFb::OnUseItem(Role * role, ItemID item_id, int num)
{
	if (NULL == role) return;

	const ItemBase *item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base || I_TYPE_EXPENSE != item_base->GetItemType()) return;

	NorexItem * norex_item = (NorexItem *)item_base;
	int use_type = norex_item->GetUseType();

	m_item_use_times_map[role->GetUID()][use_type] += num;
}

bool SpecialLogicChallengeFb::InitChallengeScene(int fb_id, int chapter)
{
	const ChallChapterInfor * fb_info = LOGIC_CONFIG->GetChallengeFbConfig()->GetChapterInfor(fb_id, chapter);
	if (NULL == fb_info) return false;

	m_fb_id = fb_id;
	m_chapter = chapter;
	m_play_type = fb_info->play_type;

	// CreateMonster ->
	UNSTD_STATIC_CHECK(3 == CF_TYPE_MAX)
	switch (m_play_type)
	{
	case CF_TYPE_BASICAL_PLAY_TYPE:
		{
			const ChallMonsterCfg * mon_cfg = LOGIC_CONFIG->GetChallengeFbConfig()->RandAMonsterCfg(m_scene->GetSceneID());
			if (NULL == mon_cfg) return false;

			NPCMonster * monster = this->CreateAMonster(mon_cfg->npc_id, mon_cfg->posi);
			if (NULL == monster) return false;

			m_count_param = monster->GetNpcSeq();
		}
		break;
	case CF_TYPE_KILL_APPO_MON_NUM:
		{
			if (!m_scene->HasMineMonster()) return false;

			std::set<int> tmp_mon_id_checker;
			for (int list_idx = 0; list_idx < fb_info->param1_list_num && list_idx < PARAM_1_LIST_COUNT; ++list_idx)
			{
				tmp_mon_id_checker.insert(fb_info->param1_list[list_idx]);
			}

			bool found = false;
			const Scene::MineMonster & mine_mon_cfg = m_scene->GetMineMonsterCfg();
			for (int mine_mon_type_idx = 0; mine_mon_type_idx < mine_mon_cfg.mine_monster_num; ++mine_mon_type_idx)
			{
				if (tmp_mon_id_checker.end() != tmp_mon_id_checker.find(mine_mon_cfg.mine_monsters[mine_mon_type_idx]))
				{
					found = true;
					break;
				}
			}

			if (!found) return false;
		}
		break;
	case CF_TYPE_KILL_MON_IN_ORDER:
		{
			std::set<ChallMonsterCfg> tmp_mon_cfg_set;
			if (!LOGIC_CONFIG->GetChallengeFbConfig()->GetMonsterCfg(m_scene->GetSceneID(), tmp_mon_cfg_set)) return false;

			for (std::set<ChallMonsterCfg>::iterator mon_it = tmp_mon_cfg_set.begin(); mon_it != tmp_mon_cfg_set.end(); ++mon_it)
			{
				if (NULL == this->CreateAMonster(mon_it->npc_id, mon_it->posi)) return false;
			}
		}
		break;
	}

	return true;
}

void SpecialLogicChallengeFb::OnFinishChallFb()
{
	// 清理场景内的所有明雷
	this->ClearAllNpcMonster();

	const ChallChapterInfor * chapter_cfg = LOGIC_CONFIG->GetChallengeFbConfig()->GetChapterInfor(m_fb_id, m_chapter);
	if (NULL == chapter_cfg) return;

	Protocol::SCChallengeFbAwardList cfal;

	int role_num = m_scene->RoleNum();
	for (int role_idx = 0; role_idx < role_num; ++role_idx)
	{
		Role * role = m_scene->GetRoleByIndex(role_idx);
		if (NULL == role) continue;
		
		// 发放章节奖励
		cfal.list_num = 0;
		if (m_chapter > role->GetRoleModuleManager()->GetRoleChallengeFb()->GetHighestPass(m_fb_id))
		{
			role->GetRoleModuleManager()->GetKnapsack()->PutList(chapter_cfg->reward_num, chapter_cfg->chapter_reward, PUT_REASON_CHALLENGE_FB);

			for (int reward_idx = 0; reward_idx < chapter_cfg->reward_num && reward_idx < 2 * ITEM_REWARD_LIST_COUNT; ++reward_idx)
			{
				const ItemConfigData & item = chapter_cfg->chapter_reward[reward_idx];
				cfal.reward_list[cfal.list_num++] = Protocol::PtcItemInfo(item.item_id, item.num);
			}

			// 副本通关奖励 - 如果找不到下一个章节的配置，说明本章是本 ID 的最后一章
			if (NULL == LOGIC_CONFIG->GetChallengeFbConfig()->GetChapterInfor(m_fb_id, m_chapter + 1))
			{
				const ChallEctypeReward * fb_reward = LOGIC_CONFIG->GetChallengeFbConfig()->GetEctypeReward(m_fb_id);
				if (NULL != fb_reward)
				{
					role->GetRoleModuleManager()->GetKnapsack()->PutList(fb_reward->reward_num, fb_reward->ectype_reward, PUT_REASON_CHALLENGE_FB);

					for (int idx = 0; idx < fb_reward->reward_num && idx < 2 * ITEM_REWARD_LIST_COUNT; ++idx)
					{
						const ItemConfigData & item = fb_reward->ectype_reward[idx];
						cfal.reward_list[cfal.list_num++] = Protocol::PtcItemInfo(item.item_id, item.num);
					}
				}
			}
		}

		int len = sizeof(cfal) - (2 * ITEM_REWARD_LIST_COUNT - cfal.list_num) * sizeof(cfal.reward_list[0]);
		if (len > 0) role->GetRoleModuleManager()->NetSend((const void *)&cfal, len);

		// 事件触控
		EventHandler::Instance().OnPassChallengeEctype(role, m_fb_id, m_chapter);
	}
}

NPCMonster * SpecialLogicChallengeFb::CreateAMonster(int npc_id, const Posi & posi)
{
	Posi real_posi(posi);
	if (!m_scene->GetMap()->Validate(Obj::OBJ_TYPE_ROLE, posi.x, posi.y))
	{
		real_posi = m_scene->GetARandPosi();
	}

	NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(npc_id, m_scene, real_posi, BATTLE_MODE_CHALLENGE_FB);
	if (NULL == monster) return NULL;

	m_monster_list[npc_id] = SpChallengeMonsterCon(npc_id, real_posi);
	return monster;
}

void SpecialLogicChallengeFb::OnKillAMonster(int seq_id)
{
	NPCMonster * npc_monster = NpcManager::Instance().GetNPCMonster(seq_id);
	if (NULL == npc_monster) return;

	std::map<int, SpChallengeMonsterCon>::iterator seq_it = m_monster_list.find(npc_monster->GetNpcId());
	if (m_monster_list.end() != seq_it) seq_it->second.already_defeat = true;

	m_scene->DeleteObj(npc_monster->GetId());
}

bool SpecialLogicChallengeFb::CheckIsRightOrder(int seq_id, bool & is_last)
{
	if (m_count_param < 0 || m_count_param >= PARAM_1_LIST_COUNT) return false;

	NPCMonster * npc_monster = NpcManager::Instance().GetNPCMonster(seq_id);
	if (NULL == npc_monster) return false;

	const ChallChapterInfor * fb_info = LOGIC_CONFIG->GetChallengeFbConfig()->GetChapterInfor(m_fb_id, m_chapter);
	if (NULL == fb_info || npc_monster->GetNpcId() != fb_info->param1_list[m_count_param]) return false;

	is_last = ++m_count_param >= fb_info->param1_list_num;
	return true;
}

void SpecialLogicChallengeFb::ResurgenceAllMonster()
{
	for (std::map<int, SpChallengeMonsterCon>::iterator mon_it = m_monster_list.begin(); mon_it != m_monster_list.end(); ++mon_it)
	{
		if (!mon_it->second.already_defeat) continue;
		
		NpcManager::Instance().CreateNPCMonster(mon_it->first, m_scene, mon_it->second.posi, BATTLE_MODE_CHALLENGE_FB);
		mon_it->second.already_defeat = false;
	}
}