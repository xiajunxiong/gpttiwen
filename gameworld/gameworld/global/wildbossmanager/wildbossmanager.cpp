#include "engineadapter.h"
#include "global/wildbossmanager/wildbossconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "world.h"
#include "internalcomm.h"
#include "scene/scenemanager.hpp"
#include "scene/scene.h"
#include "scene/speciallogicimpl/speciallogiccommon.hpp"
#include "obj/character/role.h"
#include "wildbossmanager.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "protocol/msgwildboss.h"
#include "gameworld/engineadapter.h"
#include "servercommon/commondata.hpp"
#include "gameworld/other/rolemodulemanager.hpp"

#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "obj/character/role.h"
#include "global/team/team.hpp"
#include "gamelog.h"
#include "other/poker/pokermodulemanager.hpp"

WildBossManager & WildBossManager::Instance()
{
	static WildBossManager wild_boss_mgr;
	return wild_boss_mgr;
}

void WildBossManager::Init(const WildBossParam & param)
{

	m_param = param;
	memset(monster_pos, 0, sizeof(monster_pos));
	memset(wild_number_list, 0, sizeof(wild_number_list));
	memset(offline_role_list, 0, sizeof(offline_role_list));
}

void WildBossManager::GetInitParam(WildBossParam * param)
{
	*param = m_param;

	for (int i = 0; i < MAX_WILD_BOSS_NUM; ++i)
	{
		if (param->monster_state[i] == WILD_BOSS_STATE_EXIST)
		{
			param->monster_state[i] = WILD_BOSS_STATE_NEED_CREATE;
		}
	}
}

void WildBossManager::OnRoleLogin()
{
// 	for (int idx = 0; idx < MAX_PLAY_THIS_MODEL; ++idx)
// 	{
// 		//WildbossPoker *cur_offline_role = this->GetOnceRolePokerInfo(offline_role_list[idx].team_id,offline_role_list[idx].role_id);
// 		WildbossPoker *cur_offline_role = POKER_MODULE.Get<WildbossPoker, TeamWildBossPokerInfo>(offline_role_list[idx].role_id, offline_role_list[idx].team_id);
// 		if (cur_offline_role == NULL)
// 		{
// 			offline_role_list[idx].Reset();
// 			continue;
// 		}
// 		if (cur_offline_role && cur_offline_role->role_uid == offline_role_list[idx].role_id && cur_offline_role->is_can_turn == 1 && cur_offline_role->is_turned == 0)
// 		{
// 
// 			static MailContentParam contentparam;
// 			contentparam.Reset();
// 			const ItemBase *item = ITEMPOOL->GetItem(cur_offline_role->reward_id);
// 			if (!item) return;
// 
// 			int length_1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_wildboss_poker_title);
// 			int length_2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_wildboss_poker_reward_content);
// 
// 			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_LUCK_COIN] = cur_offline_role->luck_coin;
// 			contentparam.item_list[0] = ItemDataWrapper(item->GetItemId(), cur_offline_role->reward_count, item->IsBind());
// 
// 
// 			if (length_1 > 0 && length_2 > 0)
// 			{
// 				cur_offline_role->is_can_turn = 0;
// 				cur_offline_role->is_turned = 1;
// 
// 				UserID cur_user_id = UserID(cur_offline_role->role_uid);
// 				MailRoute::MailToUser(cur_user_id, MAIL_REASON_DEFAULT, contentparam);
// 			}
// 
// 			offline_role_list[idx].Reset();
// 		}
// 		
// 	}

}

void WildBossManager::OnRoleLogout(Role *role)
{

	for (int idx = 0;idx < MAX_PLAY_THIS_MODEL;++idx)
	{
		if (offline_role_list[idx].team_id == 0 && offline_role_list[idx].role_id == 0)
		{
			
			Team *tt = role->GetMyTeam();
			int team_id = 0;
			if (tt)
			{
				team_id = tt->GetTeamIndex();
			}
			
			offline_role_list[idx].team_id = team_id;
			offline_role_list[idx].role_id = role->GetUID();
			break;
		}
	}

}

void WildBossManager::Update(unsigned long interval, time_t now_second)
{
	if(!CrossConfig::Instance().IsHiddenServer()) return;

	const std::vector<WildBossItemCfg> & all_boss_item_cfg = LOGIC_CONFIG->GetWildBossConfig()->GetAllWildBossItemCfg();
	for (int i = 0; i < (int)all_boss_item_cfg.size() && i < MAX_WILD_BOSS_NUM; ++i)
	{
		const WildBossItemCfg & cfg = all_boss_item_cfg[i];
		if (m_param.next_refresh_time[i] == 0)
		{
			m_param.next_refresh_time[i] = (unsigned int)(now_second + LOGIC_CONFIG->GetWildBossConfig()->GetRefreshInterval(cfg.refresh_time_group_id));
		}

		if (m_param.monster_state[i] == WILD_BOSS_STATE_NEED_CREATE) //保存时会将WILD_BOSS_STATE_EXIST设置成WILD_BOSS_STATE_NEED_CREATE
		{
			m_param.monster_state[i] = WILD_BOSS_STATE_KILL;
			m_param.next_refresh_time[i] = (unsigned int)now_second;
		}


		if (now_second < m_param.next_refresh_time[i])
		{
			continue;
		}		

		if (m_param.monster_state[i] == WILD_BOSS_STATE_EXIST)
		{
			m_param.next_refresh_time[i] = (unsigned int)(now_second + LOGIC_CONFIG->GetWildBossConfig()->GetRefreshInterval(cfg.refresh_time_group_id));
			continue;
		}

		Scene * scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(cfg.scene_id, COMMON_SCENE_KEY);
		if (scene == NULL || scene->GetSceneType() != Scene::SCENE_TYPE_COMMON_SCENE)
		{
			continue;
		}

		SpecialLogicCommon * special_common = (SpecialLogicCommon *)scene->GetSpecialLogic();
		if (special_common == NULL)
		{
			continue;
		}

		const WildBossMonsterCfg * ordinary_cfg = LOGIC_CONFIG->GetWildBossConfig()->GetMonsterCfg(cfg.ordinary_number);
		if(NULL == ordinary_cfg) continue;

		int npc_monster_id = ordinary_cfg->npc_monster_id;
		bool is_broadcast = false;
		int wild_number = cfg.ordinary_number;
		if (cfg.wild_number > 0 && cfg.wild_rate > 0)
		{
			int rate_num = RandomNum(WildBossItemCfg::MAX_WILD_BOSS_RATE_COUNT);
			if (rate_num < cfg.wild_rate)
			{
				const WildBossMonsterCfg * wild_cfg = LOGIC_CONFIG->GetWildBossConfig()->GetMonsterCfg(cfg.wild_number);
				if (NULL == wild_cfg) continue;

				npc_monster_id = wild_cfg->npc_monster_id;
				is_broadcast = true;
				wild_number = cfg.wild_number;
			}
		}

		Posi out_posi(0, 0);
		int monster_group_id = 0;
		if (special_common->CreateWildBoss(npc_monster_id, i, &out_posi, &monster_group_id))
		{
			m_param.monster_state[i] = WILD_BOSS_STATE_EXIST;
			m_param.next_refresh_time[i] = (unsigned int)(now_second + LOGIC_CONFIG->GetWildBossConfig()->GetRefreshInterval(cfg.refresh_time_group_id));
			monster_pos[i] = out_posi;
			wild_number_list[i] = wild_number; 
			this->SendAllUser(i);
			if (is_broadcast)
			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_wild_boss_refresh_notice, monster_group_id, cfg.scene_id);
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgAllGame(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
				}
			}
		}
	}
}

void WildBossManager::GmCreateWildBoss(Role * role)
{
	if(!CrossConfig::Instance().IsHiddenServer()) return;

	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	const std::vector<WildBossItemCfg> &  all_boss_item_cfg = LOGIC_CONFIG->GetWildBossConfig()->GetAllWildBossItemCfg();
	for (int i = 0; i < (int)all_boss_item_cfg.size(); ++i)
	{
		const WildBossItemCfg & cfg = all_boss_item_cfg[i];
		if(WILD_BOSS_STATE_KILL != m_param.monster_state[i]) continue;

		Scene * scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(cfg.scene_id, COMMON_SCENE_KEY);
		if (scene == NULL || scene->GetSceneType() != Scene::SCENE_TYPE_COMMON_SCENE)
		{
			continue;
		}

		SpecialLogicCommon * special_common = (SpecialLogicCommon *)scene->GetSpecialLogic();
		if (NULL == special_common)
		{
			continue;
		}

		const WildBossMonsterCfg * ordinary_cfg = LOGIC_CONFIG->GetWildBossConfig()->GetMonsterCfg(cfg.ordinary_number);
		if (NULL == ordinary_cfg) continue;

		int npc_monster_id = ordinary_cfg->npc_monster_id;
		bool is_broadcast = false;
		int wild_number = cfg.ordinary_number;
		if (cfg.wild_number > 0 && cfg.wild_rate > 0)
		{
			int rate_num = RandomNum(WildBossItemCfg::MAX_WILD_BOSS_RATE_COUNT);
			if (rate_num < cfg.wild_rate)
			{
				const WildBossMonsterCfg * wild_cfg = LOGIC_CONFIG->GetWildBossConfig()->GetMonsterCfg(cfg.wild_number);
				if (NULL == wild_cfg) continue;

				npc_monster_id = wild_cfg->npc_monster_id;
				is_broadcast = true;
				wild_number = cfg.wild_number;
			}
		}

		Posi out_posi(0, 0);
		int monster_group_id = 0;
		if (special_common->CreateWildBoss(npc_monster_id, i, &out_posi, &monster_group_id))
		{
			m_param.monster_state[i] = WILD_BOSS_STATE_EXIST;
			m_param.next_refresh_time[i] = (unsigned int)(now_second + LOGIC_CONFIG->GetWildBossConfig()->GetRefreshInterval(cfg.refresh_time_group_id));
			monster_pos[i] = out_posi;
			wild_number_list[i] = wild_number; 
			this->SendAllUser(i);
			if (is_broadcast)
			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_wild_boss_refresh_notice, monster_group_id, cfg.scene_id);
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgAllGame(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
				}
			}
		}
	}
}

void WildBossManager::SendInfoToRole(Role * role)
{
	Protocol::SCWildBossInfo info;
	info.today_times = role->GetWildBossRewardTimes();
	for (int i = 0;i < MAX_WILD_BOSS_NUM; ++i)
	{
		if (WILD_BOSS_STATE_EXIST == m_param.monster_state[i])
		{
			info.next_refresh_time[i] = 0;
			info.wild_number_list[i] = wild_number_list[i];
			continue;
		}
		info.next_refresh_time[i] = m_param.next_refresh_time[i];
		info.wild_number_list[i] = 0;
	}
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
}

void WildBossManager::HiddenSendInfoToGameRole(int uid, int param)
{
	if(!CrossConfig::Instance().IsHiddenServer()) return;

	Protocol::SCWildBossInfo info;
	info.today_times = param;
	for (int i = 0; i < MAX_WILD_BOSS_NUM; ++i)
	{
		if (WILD_BOSS_STATE_EXIST == m_param.monster_state[i])
		{
			info.next_refresh_time[i] = 0;
			info.wild_number_list[i] = wild_number_list[i];
			continue;
		}
		info.next_refresh_time[i] = m_param.next_refresh_time[i];
		info.wild_number_list[i] = 0;
	}
	InternalComm::Instance().SendToGameUserThroughCross(uid, (const char *)&info, sizeof(info));
}

void WildBossManager::SendAllUser(int index)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (index < 0 || index >= MAX_WILD_BOSS_NUM)
		{
			return;
		}

		Protocol::SCWildBossSingleInfo info;

		info.index = index;
		if (WILD_BOSS_STATE_EXIST == m_param.monster_state[index])
		{
			info.next_refresh_time = 0;
			info.wild_number = wild_number_list[index];
		}
		else
		{
			info.next_refresh_time = m_param.next_refresh_time[index];
			info.wild_number = 0;
		}
		InternalComm::Instance().SendToAllGameAllUserThroughCross((const char *)&info, sizeof(info));
	}
}

void WildBossManager::SendMonsterPosiToRole(Role * role, int monster_seq)
{
	if (monster_seq < 0 || monster_seq >= MAX_WILD_BOSS_NUM)return;
	Protocol::SCWildBossPosiInfo info;
	info.monster_pos_x = monster_pos[monster_seq].x;
	info.monster_pos_y = monster_pos[monster_seq].y;
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
}

void WildBossManager::HiddenSendMonsterPosiTorGameRole(int uid, int monster_seq)
{
	if (!CrossConfig::Instance().IsHiddenServer() || monster_seq < 0 || monster_seq >= MAX_WILD_BOSS_NUM)return;

	Protocol::SCWildBossPosiInfo info;
	info.monster_pos_x = monster_pos[monster_seq].x;
	info.monster_pos_y = monster_pos[monster_seq].y;
	InternalComm::Instance().SendToGameUserThroughCross(uid, (const char *)&info, sizeof(info));
}

void WildBossManager::OnKillMonster(int cfg_seq)
{
	if (cfg_seq < 0 || cfg_seq >= MAX_WILD_BOSS_NUM)
	{
		return;
	}
	const std::vector<WildBossItemCfg> & all_boss_item_cfg = LOGIC_CONFIG->GetWildBossConfig()->GetAllWildBossItemCfg();
	if ((int)all_boss_item_cfg.size() <= cfg_seq) return;

	m_param.monster_state[cfg_seq] = WILD_BOSS_STATE_KILL;
	m_param.next_refresh_time[cfg_seq] = static_cast<unsigned int>(EngineAdapter::Instance().Time() + LOGIC_CONFIG->GetWildBossConfig()->GetRefreshInterval(all_boss_item_cfg[cfg_seq].refresh_time_group_id));
	this->ResetPosi(cfg_seq);
	this->SendAllUser(cfg_seq);
}

WildBossManager::WildBossManager()
{
}

WildBossManager::~WildBossManager()
{
}

void WildBossManager::ResetPosi(int seq)
{
	if (seq < 0 || seq >= MAX_WILD_BOSS_NUM)return;
	monster_pos[seq].x = 0;
	monster_pos[seq].y = 0;
}
