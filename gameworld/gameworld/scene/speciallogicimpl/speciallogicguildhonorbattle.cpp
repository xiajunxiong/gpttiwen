#include "config/logicconfigmanager.hpp"
#include "scene/scene.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "global/activity/activitymanager.hpp"
#include "global/guild/guild.hpp"
#include "battle/battle_manager_local.hpp"
#include "world.h"
#include "gamelog.h"
#include "item/itempool.h"
#include "item/itembase.h"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "other/event/eventhandler.hpp"
#include "global/usercache/usercache.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/guild/guildmember.hpp"
#include "global/worldstatus/worldstatus.hpp"

#include "global/guildhonorbattlemanager/guildhonorbattlemanager.hpp"
#include "config/activityconfig/guildhonorbattle/activityguildhonorbattleconfig.hpp"
#include "protocol/msgguildhonorbattle.h"
#include "speciallogicguildhonorbattle.hpp"

bool GuildHonorSendFilter(Role * role, void * func_param)
{
	int guild_id = *(int *)func_param;

	if (role->GetGuildID() == guild_id)
	{
		return true;
	}

	return false;
}

SpecialLogicGuildHonorBattle::SpecialLogicGuildHonorBattle(Scene * scene) :SpecialLogic(scene)
{
	m_is_finish = false;
	m_is_close = false;
	m_start_battle = false;

	for (int i = 0; i < GUILD_HONOR_BATTLE_MAX_ZONE; ++i)
	{
		m_battle_result[i] = 0;
		m_battle_id[i] = 0;
	}

	m_win_rank = 0;
	m_round = 1;
	m_win_guild_id = 0;
}

SpecialLogicGuildHonorBattle::~SpecialLogicGuildHonorBattle()
{
}

void SpecialLogicGuildHonorBattle::OnRoleEnterScene(Role * role)
{
	int guild_id = role->GetGuildID();

	GuildHonorBattleGuildInfo * guild_info_ptr = this->GetGuildHonorBattleGuildInfo(guild_id);
	if (guild_info_ptr == NULL || m_is_finish)
	{
		this->DelayKickOutRole(role);
		return;
	}

	if (!m_start_battle)
	{
		GuildHonorBattleGuildInfo & guild_info = *guild_info_ptr;
		guild_info.zone_role_info[0].push_back(role->GetUID());
		this->SendRoleEnterScene(role);
	}

	this->SendGuildZoneInfo(guild_id, role);
	this->SendAllZoneBattleResult(role);
	this->AutoAssignedBeforStartBattle();
	this->SendBattleState(role);

	// 检查玩家是否在战斗中（也有可能是战斗数据未清干净，导致开战时进不了战斗，所以在进场景的时候检查一下）
	this->CheckRoleReallyInBattle(role); 
}

void SpecialLogicGuildHonorBattle::OnRoleLeaveScene(Role * role, bool is_logout)
{
	for (std::map<int, GuildHonorBattleGuildInfo *>::iterator iter = m_guild_battle_info.begin(); iter != m_guild_battle_info.end(); ++iter)
	{
		GuildHonorBattleGuildInfo & guild_info = *iter->second;
		for (std::map<int, std::vector<GuildHonorBattleRoleInfo> >::iterator zone_iter = guild_info.zone_role_info.begin(); zone_iter != guild_info.zone_role_info.end(); ++zone_iter)
		{
			int zone_id = zone_iter->first;
			std::vector<GuildHonorBattleRoleInfo> & role_info_vec = zone_iter->second;
			for (int i = 0; i < (int)role_info_vec.size(); ++i)
			{
				int role_id = role->GetUID();
				if (role_info_vec[i].role_id == role_id)
				{
					if (!m_start_battle)
					{
						zone_iter->second.erase(role_info_vec.begin() + i);
						int guild_id = iter->first;
						this->SendZoneChangeInfo(guild_id, role_id, 0, GUILD_HONOR_CHANGE_RESON_DEL);
					}

					MapAddOrUpdate(guild_info.role_to_zone_id, role_id, zone_id);

					return;
				}
			}
		}
	}
}

void SpecialLogicGuildHonorBattle::Update(unsigned long interval, time_t now_second)
{
}

void SpecialLogicGuildHonorBattle::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (BATTLE_MODE_GUILD_HONOR_BATTLE == ack->battle_mode)
	{
		int zone_id = ack->business_data.param1;
		int win_guild_id = -1;
		switch (ack->battle_result)
		{
			case RESULT_TYPE_ATTACKER_WIN:
				{
					win_guild_id = m_guild_battle_info1.guild_id;
				}
				break;
			case RESULT_TYPE_DEFENDER_WIN:
				{
					win_guild_id = m_guild_battle_info2.guild_id;
				}
				break;
			default:
				{
					if (ack->statistic_data.attacker_total_damage > ack->statistic_data.defender_total_damage)
						win_guild_id = m_guild_battle_info1.guild_id;
					if (ack->statistic_data.attacker_total_damage < ack->statistic_data.defender_total_damage)
						win_guild_id = m_guild_battle_info2.guild_id;

					win_guild_id = m_win_guild_id;
				}
				break;
		}

		this->SetResult(zone_id, win_guild_id, __FUNCTION__);
		this->CheakFinish();
	}
}

void SpecialLogicGuildHonorBattle::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{
	if (BATTLE_MODE_GUILD_HONOR_BATTLE == ack->battle_mode)
	{
		int zone_id = ack->business_data.param1;
		int win_guild_id = m_win_guild_id;

		if (ack->statistic_data.attacker_total_damage > ack->statistic_data.defender_total_damage)
			win_guild_id = m_guild_battle_info1.guild_id;
		if (ack->statistic_data.attacker_total_damage < ack->statistic_data.defender_total_damage)
			win_guild_id = m_guild_battle_info2.guild_id;

		this->SetResult(zone_id, win_guild_id, __FUNCTION__);
		this->CheakFinish();
	}
}

bool SpecialLogicGuildHonorBattle::CanDestroy()
{
	return m_is_close;
}

bool SpecialLogicGuildHonorBattle::CanEnterTeam(Role * role, Team * team, bool is_from_create)
{
	return false;
}

bool SpecialLogicGuildHonorBattle::CanInvitationRole(Role * role, int uid)
{
	return false;
}

void SpecialLogicGuildHonorBattle::AutoAssignedRole(Role * role)
{
	if (m_start_battle)
	{
		return;
	}

	Guild * guild = role->GetGuild();
	if (guild == NULL)
	{
		return;
	}

	int guild_id = guild->GetGuildID();
	GuildMember * guild_member = guild->GetMemberManagerRef().GetMember(role->GetUID());
	if (guild_member == NULL)
	{
		return;
	}

	if (guild_member->GetPost() != GUILD_POST_PATRIARCH && guild_member->GetPost() != GUILD_POST_VICE_PATRIARCH
	    && guild_member->GetPost() != GUILD_POST_ELDER)
	{
		return;
	}

	GuildHonorBattleGuildInfo * guild_info_ptr = this->GetGuildHonorBattleGuildInfo(guild_id);
	if (guild_info_ptr)
	{
		GuildHonorBattleGuildInfo & guild_info = *guild_info_ptr;
		guild_info.AutoAssigned();
	}

	this->SendGuildZoneInfo(guild_id);
}

void SpecialLogicGuildHonorBattle::AssignedRole(Role * role, int zone_id, int role_id)
{
	if (m_start_battle)
	{
		return;
	}

	Guild * guild = role->GetGuild();
	if (guild == NULL)
	{
		return;
	}

	GuildMember * guild_member = guild->GetMemberManagerRef().GetMember(role->GetUID());
	if (guild_member == NULL)
	{
		return;
	}

	if (guild_member->GetPost() != GUILD_POST_PATRIARCH && guild_member->GetPost() != GUILD_POST_VICE_PATRIARCH
	    && guild_member->GetPost() != GUILD_POST_ELDER)
	{
		return;
	}

	GuildHonorBattleGuildInfo * battle_guild_info = this->GetGuildHonorBattleGuildInfo(guild->GetGuildID());
	if (!battle_guild_info)
	{
		return;
	}

	if (zone_id < 0 || zone_id > GUILD_HONOR_BATTLE_MAX_ZONE)
	{
		return;
	}

	if (!guild->GetMemberManagerRef().IsGuildMember(role_id))
	{
		return;
	}

	GuildHonorBattleGuildInfo & guild_info = *battle_guild_info;
	for (std::map<int, std::vector<GuildHonorBattleRoleInfo> >::iterator zone_iter = guild_info.zone_role_info.begin(); zone_iter != guild_info.zone_role_info.end(); ++zone_iter)
	{
		std::vector<GuildHonorBattleRoleInfo> & role_info_vec = guild_info.zone_role_info[zone_iter->first];

		std::vector<GuildHonorBattleRoleInfo>::iterator find_iter = std::find(role_info_vec.begin(), role_info_vec.end(), role_id);
		if (find_iter != role_info_vec.end())
		{
			zone_iter->second.erase(find_iter);
			break;
		}
	}

	guild_info.zone_role_info[zone_id].push_back(role_id);
	this->SendZoneChangeInfo(guild->GetGuildID(), role_id, zone_id);
}


void SpecialLogicGuildHonorBattle::SetGuildInfo(int win_rank, int curr_round,const GuildHonorBattleFigtherInfo * guild1, const GuildHonorBattleFigtherInfo * guild2)
{
	if (guild1 == NULL || guild2 == NULL)
	{
		return;
	}

	m_guild_battle_info1.guild_id = guild1->guild_id;
	m_guild_battle_info2.guild_id = guild2->guild_id;

	SameArrayTypeCopy(m_guild_battle_info1.guild_name, guild1->guild_name);	
	SameArrayTypeCopy(m_guild_battle_info2.guild_name, guild2->guild_name);

	m_guild_battle_info[m_guild_battle_info1.guild_id] = &m_guild_battle_info1;
	m_guild_battle_info[m_guild_battle_info2.guild_id] = &m_guild_battle_info2;

	m_win_rank = win_rank;
	m_round = curr_round;
	m_win_guild_id = guild1->guild_fight_rank < guild2->guild_fight_rank ? guild1->guild_id : guild2->guild_id;
}

void SpecialLogicGuildHonorBattle::OnActivityGuildHonorBattleRoundFinish()
{
	if (m_is_close)
	{
		return;
	}

	m_is_close = true;

	for (int i = 0; i < ARRAY_LENGTH(m_battle_result); ++i)
	{
		if (m_battle_result[i] == 0)
		{
			continue;
		}

		int win_guild = m_win_guild_id;  //默认排名的高

		int zone_id = i + 1;
		if (m_guild_battle_info1.zone_battle_total_damage[zone_id] > m_guild_battle_info2.zone_battle_total_damage[zone_id])  //如果伤害不一致伤害来判断
		{
			win_guild = m_guild_battle_info1.guild_id;
		}

		if (m_guild_battle_info1.zone_battle_total_damage[zone_id] < m_guild_battle_info2.zone_battle_total_damage[zone_id])
		{
			win_guild = m_guild_battle_info2.guild_id;
		}

		this->SetResult(zone_id, win_guild, __FUNCTION__);
	}
	
	this->CheakFinish(true);
}

void SpecialLogicGuildHonorBattle::StartBattle(long long battle_length_of_time)
{
	if (m_start_battle)
	{
		return;
	}

	m_start_battle = true;

	this->AutoAssignedBeforStartBattle();


	int role_num = m_scene->RoleNum();
	for (int i = 0; i < role_num; i++)
	{
		Role * role = m_scene->GetRoleByIndex(i);
		if (NULL == role) continue;

		Guild * guild = GuildManager::Instance().GetGuild(role->GetGuildID());
		if (NULL != guild)
		{
			GuildMember * guild_member = guild->GetMemberManager()->GetMember(role->GetUID());
			if (NULL != guild_member)
			{
				guild_member->SetHonorBattleFlag();
				guild->GetMemberManager()->OnMemberInfoChange(guild_member->GetUid());
				gamelog::g_log_guild_honor_battle.printf( LL_INFO, "%s uid[%d] SetHonorBattleFlag!", __FUNCTION__, guild_member->GetUid());
			}
		}

		EventHandler::Instance().OnParticipateLimitActivity(role, ACTIVITY_TYPE_GUILD_HONOR_BATTLE, __FUNCTION__, role->GetGuildID());
	}

	GuildHonorBattleGuildInfo & attacker_guild_info = m_guild_battle_info1;
	GuildHonorBattleGuildInfo & defender_guild_info = m_guild_battle_info2;

	if (!attacker_guild_info.IsNobodyEachZone() && !defender_guild_info.IsNobodyEachZone())
	{
		//两个工会至少都有一个人参加才执行这里
		for (int zone_id = 1; zone_id <= GUILD_HONOR_BATTLE_MAX_ZONE; ++zone_id)
		{
			std::vector<Role *> attacker;
			std::vector<Role *> defender;
			std::vector<Role *> attacker_ober;		//要观战的人
			std::vector<Role *> defender_ober;

			std::map<int, std::vector<GuildHonorBattleRoleInfo> >::iterator attacker_zone_iter = attacker_guild_info.zone_role_info.find(zone_id);
			if (attacker_zone_iter != attacker_guild_info.zone_role_info.end())
			{
				int guild_id = attacker_guild_info.guild_id;
				std::vector<GuildHonorBattleRoleInfo> & zone_role_info = attacker_zone_iter->second;
				gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  %s zone_id:%d guild:%d role_num:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), zone_id, guild_id, (int)zone_role_info.size());
				for (int i = 0; i < (int)zone_role_info.size(); ++i)
				{
					GuildHonorBattleRoleInfo & role_info = zone_role_info[i];
					Role * tmp_role = m_scene->GetRoleByUID(role_info.role_id);
					if (tmp_role == NULL)
					{
						m_role_battle_state[role_info.role_id] = GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_FINISH;
						gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  win_rank:%d curr_round:%d zone_id:%d guild:%d not_add_role_id:%d", __FUNCTION__, __LINE__, m_win_rank, m_round, zone_id, guild_id, role_info.role_id);
						continue;
					}

					if (!BattleManagerLocal::GetInstance().CheckRoleCanStartBattle(role_info.role_id, BATTLE_MODE_GUILD_HONOR_BATTLE))
					{
						m_role_battle_state[role_info.role_id] = GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_FINISH;
						gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  %s zone_id:%d guild:%d not_add_role_id:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), zone_id, guild_id, role_info.role_id);
						continue;
					}

					m_role_battle_state[role_info.role_id] = GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_BATTLE;

					attacker.push_back(tmp_role);
					gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | %s zone_id:%d guild:%d add_role_id:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), zone_id, guild_id, role_info.role_id);
#ifdef _DEBUG
					if (attacker.size() >= 1)
#else
					if (attacker.size() >= MAX_TEAM_MEMBER_NUM)
#endif
					{
						break;
					}
				}
				attacker_guild_info.zone_id_to_battle_role_index[zone_id] += (int)attacker.size();

				//拉剩下其余的人进入观战
				for (int i = (int)attacker.size(); i < (int)zone_role_info.size(); ++i)
				{
					Role * tmp_role = m_scene->GetRoleByUID(zone_role_info[i].role_id);
					if (tmp_role != NULL)
					{
						attacker_ober.push_back(tmp_role);
						gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | %s zone_id:%d guild:%d ob_role_id:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), zone_id, guild_id, zone_role_info[i].role_id);
					}
					else
					{
						gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  %s zone_id:%d guild:%d _not_ob_role_id:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), zone_id, guild_id, zone_role_info[i].role_id);
					}
				}
			}

			std::map<int, std::vector<GuildHonorBattleRoleInfo> >::iterator defender_zone_iter = defender_guild_info.zone_role_info.find(zone_id);
			if (defender_zone_iter != defender_guild_info.zone_role_info.end())
			{
				int guild_id = defender_guild_info.guild_id;
				std::vector<GuildHonorBattleRoleInfo> & zone_role_info = defender_zone_iter->second;
				gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | %s zone_id:%d guild:%d role_num:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), zone_id, guild_id, (int)zone_role_info.size());
				for (int i = 0; i < (int)zone_role_info.size(); ++i)
				{
					GuildHonorBattleRoleInfo & role_info = zone_role_info[i];
					Role * tmp_role = m_scene->GetRoleByUID(role_info.role_id);
					if (tmp_role == NULL)
					{
						m_role_battle_state[role_info.role_id] = GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_FINISH;
						gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  %s zone_id:%d guild:%d not_add_role_id:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), zone_id, guild_id, role_info.role_id);
						continue;
					}

					if (!tmp_role->GetRoleStatusManager()->CanRoleStartFight(BATTLE_MODE_GUILD_HONOR_BATTLE) || tmp_role->GetRoleStatusManager()->IsInBattleStatus())
					{
						m_role_battle_state[role_info.role_id] = GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_FINISH;
						gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | %s zone_id:%d guild:%d not_add_role_id:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), zone_id, guild_id, role_info.role_id);
						continue;
					}

					m_role_battle_state[role_info.role_id] = GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_BATTLE;

					defender.push_back(tmp_role);
					gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  win_rank:%d curr_round:%d zone_id:%d guild:%d add_role_id:%d", __FUNCTION__, __LINE__, m_win_rank, m_round, zone_id, guild_id, role_info.role_id);
#ifdef _DEBUG
					if (defender.size() >= 1)
#else
					if (defender.size() >= MAX_TEAM_MEMBER_NUM)
#endif
					{
						break;
					}
				}
				defender_guild_info.zone_id_to_battle_role_index[zone_id] += (int)defender.size();

				//拉剩下其余的人进入观战
				for (int i = (int)defender.size(); i < (int)zone_role_info.size(); ++i)
				{
					Role * tmp_role = m_scene->GetRoleByUID(zone_role_info[i].role_id);
					if (tmp_role != NULL)
					{
						defender_ober.push_back(tmp_role);
						gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | %s zone_id:%d guild:%d ob_role_id:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), zone_id, guild_id, zone_role_info[i].role_id);
					}
					else
					{
						gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |%s zone_id:%d guild:%d _not_ob_role_id:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), zone_id, guild_id, zone_role_info[i].role_id);
					}
				}
			}

			int battle_id = 0;
			int result = BattleManagerLocal::GetInstance().StartGuildHonorPVPReq(zone_id, battle_length_of_time, attacker, defender, attacker_ober, defender_ober, &battle_id);

			gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  %s %d to %d zone_id:%d  BattleManagerLocal::GetInstance().StartGuildHonorPVPReq result:%d battle_id:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(),
								 attacker_guild_info.guild_id, defender_guild_info.guild_id, zone_id, result, battle_id);
			switch (result)
			{
				case RESULT_ATTACKER_FAIL:
					{
						this->SetResult(zone_id, defender_guild_info.guild_id, __FUNCTION__);
					}
					break;
				case RESULT_DEFENDER_FAIL:
					{
						this->SetResult(zone_id, attacker_guild_info.guild_id, __FUNCTION__);
					}
					break;
				case RESULT_BOTH_FAIL:
					{
						this->SetResult(zone_id, -1, __FUNCTION__);
					}
					break;
				case START_BATTLE_RESULT_SUCC:
					{
						this->SetZoneBattleId(zone_id, battle_id);
					}
				default:
					break;
			}
		}
	}
	else
	{
		//否则直接设置有人那一方3个区域胜利
		int win_guild_id = m_win_guild_id;
		if (!attacker_guild_info.IsNobodyEachZone())
		{
			win_guild_id = attacker_guild_info.guild_id;
		}

		if (!defender_guild_info.IsNobodyEachZone())
		{
			win_guild_id = defender_guild_info.guild_id;
		}

		for (int i = 0; i < ARRAY_LENGTH(m_battle_result); ++i)
		{
			int zone_id = i + 1;
			this->SetResult(zone_id, win_guild_id, __FUNCTION__);
		}

		gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  %s %d to %d,direct winner:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(),
							 attacker_guild_info.guild_id, defender_guild_info.guild_id, win_guild_id);

		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_guild_honor_battle_direct_win_broadcast);
		if (win_guild_id != 0 && length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT)))
			{
				m_scene->SendToRole((const char *)&sm, sendlen, GuildHonorSendFilter, (void *)&win_guild_id);
			}
		}
	}

	this->CalZoneRemainRoleNum(false);
	this->CalZoneRemainRoleNum(true);
	this->SendGuildZoneBattleRoleNumInfo();
	this->SendAllZoneBattleResult();
	this->CheakFinish();

}

void SpecialLogicGuildHonorBattle::BattleAddBackUp(int battle_id, int need_attacker_role, int need_defender_num, int zone_id)
{
	const BattleLocalInfo * local_info = BattleManagerLocal::GetInstance().GetBattleLocalInfoConst(battle_id);
	if (NULL == local_info)
	{
		return;
	}

	battlegameprotocol::GameBattleGuildHonorFightSendBackup protocol;
	protocol.attacker_num = 0;
	protocol.defender_num = 0;
	protocol.battle_id = battle_id;
	
	bool is_ack_change = false;
	bool is_def_change = false;
	if (!m_is_finish)
	{
		GuildHonorBattleGuildInfo & attacker_guild_info = m_guild_battle_info1;
		GuildHonorBattleGuildInfo & defender_guild_info = m_guild_battle_info2;
		
		while (protocol.attacker_num < need_attacker_role)
		{
			int & curr_role_index = attacker_guild_info.zone_id_to_battle_role_index[zone_id];
			std::vector<GuildHonorBattleRoleInfo> & zone_role_info = attacker_guild_info.zone_role_info[zone_id];
			if (curr_role_index < 0 || curr_role_index >= (int)zone_role_info.size())
			{
				break;
			}

			GuildHonorBattleRoleInfo & role_info = zone_role_info[curr_role_index++];
			Role * tmp_role = m_scene->GetRoleByUID(role_info.role_id);
			if (tmp_role != NULL)
			{
				protocol.attacker_data_list[protocol.attacker_num].uid = tmp_role->GetUID();
				protocol.attacker_data_list[protocol.attacker_num].bg_netid = tmp_role->GetBGNetID();
				tmp_role->GetRoleModuleManager()->CollectRoleBattleData(&protocol.attacker_data_list[protocol.attacker_num].role_battle_data, false);
				protocol.attacker_num += 1;

				gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  %s zone_id:%d guild:%d backup_role_id:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), zone_id, attacker_guild_info.guild_id, role_info.role_id);
			}
			else
			{
				is_ack_change = true;
				m_role_battle_state[role_info.role_id] = GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_FINISH;
				gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | %s zone_id:%d guild:%d not_backup_role_id:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), zone_id, attacker_guild_info.guild_id, role_info.role_id);
			}	
		}
		
		while (protocol.defender_num < need_defender_num)
		{
			int & curr_role_index = defender_guild_info.zone_id_to_battle_role_index[zone_id];
			std::vector<GuildHonorBattleRoleInfo> & zone_role_info = defender_guild_info.zone_role_info[zone_id];
			if (curr_role_index < 0 || curr_role_index >= (int)zone_role_info.size())
			{
				break;
			}
			GuildHonorBattleRoleInfo & role_info = zone_role_info[curr_role_index++];
			Role * tmp_role = m_scene->GetRoleByUID(role_info.role_id);
			if (tmp_role != NULL)
			{
				protocol.defender_data_list[protocol.defender_num].uid = tmp_role->GetUID();
				protocol.defender_data_list[protocol.defender_num].bg_netid = tmp_role->GetBGNetID();
				tmp_role->GetRoleModuleManager()->CollectRoleBattleData(&protocol.defender_data_list[protocol.defender_num].role_battle_data, false);
				protocol.defender_num += 1;

				gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | %s zone_id:%d guild:%d backup_role_id:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), zone_id, defender_guild_info.guild_id, role_info.role_id);
			}
			else
			{
				is_def_change = true;
				m_role_battle_state[role_info.role_id] = GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_FINISH;				

				gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  %s zone_id:%d guild:%d not_backup_role_id:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), zone_id, defender_guild_info.guild_id, role_info.role_id);
			}
		}
	}

	World::GetInstWorld()->SendSerializeMsgToOneBattle(local_info->battle_server_index, &protocol);
	if(is_ack_change)this->CalZoneRemainRoleNum(true, zone_id);
	if (is_def_change)this->CalZoneRemainRoleNum(false, zone_id);
	if (is_ack_change || is_def_change)this->SendGuildZoneBattleRoleNumInfo();
}

void SpecialLogicGuildHonorBattle::BattleChangeBackupRoleNumNotice(int battle_id, int attacker_backup_exist_role_num, int defender_backup_exist_role_num, int zone_id)
{

}

void SpecialLogicGuildHonorBattle::BattleStateChangeNotice(bool is_attack, std::vector<int> start_battle_role_id, std::vector<int> battle_end_role_id, int zone_id)
{
	GuildHonorBattleGuildInfo & guild_info = is_attack ? m_guild_battle_info1 : m_guild_battle_info2;
	
	for (int i = 0; i < (int)start_battle_role_id.size(); ++i)
	{
		int role_id = start_battle_role_id[i];
		std::vector<GuildHonorBattleRoleInfo> & zone_role_info = guild_info.zone_role_info[zone_id];
		for (int role_index = 0; role_index < (int)zone_role_info.size(); ++role_index)
		{
			GuildHonorBattleRoleInfo & role_info = zone_role_info[role_index];
			if (role_info.role_id == role_id)
			{
				m_role_battle_state[role_id]= GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_BATTLE;
				this->SendBattleStateBuyRoleId(role_id);
			}
		}
	}

	for (int i = 0; i < (int)battle_end_role_id.size(); ++i)
	{
		int role_id = battle_end_role_id[i];
		std::vector<GuildHonorBattleRoleInfo> & zone_role_info = guild_info.zone_role_info[zone_id];
		for (int role_index = 0; role_index < (int)zone_role_info.size(); ++role_index)
		{
			GuildHonorBattleRoleInfo & role_info = zone_role_info[role_index];
			if (role_info.role_id == role_id)
			{
				m_role_battle_state[role_id] = GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_FINISH;
				this->SendBattleStateBuyRoleId(role_id);
			}
		}
	}

	this->CalZoneRemainRoleNum(is_attack, zone_id);
	this->SendGuildZoneBattleRoleNumInfo();
}

void SpecialLogicGuildHonorBattle::BattleRoundTotalDamage(long long ack_total_damage, long long def_total_damage, int zone_id)
{
	m_guild_battle_info1.zone_battle_total_damage[zone_id] = ack_total_damage;
	m_guild_battle_info2.zone_battle_total_damage[zone_id] = def_total_damage;
}

void SpecialLogicGuildHonorBattle::CalZoneRemainRoleNum(bool is_attack, int zone_id)
{
	GuildHonorBattleGuildInfo & guild_info = is_attack ? m_guild_battle_info1 : m_guild_battle_info2;

	if (zone_id == -1)
	{
		for (std::map<int, std::vector<GuildHonorBattleRoleInfo> >::const_iterator zone_iter = guild_info.zone_role_info.begin(); zone_iter != guild_info.zone_role_info.end(); ++zone_iter)
		{
			int curr_zone_id = zone_iter->first;
			int sum = 0;
			const std::vector<GuildHonorBattleRoleInfo> & zone_role_info = zone_iter->second;
			for (int i = 0; i < (int)zone_role_info.size(); ++i)
			{
				const GuildHonorBattleRoleInfo & role_info = zone_role_info[i];
				if (this->GetRoleBattleState(role_info.role_id) == GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_DEFAULT)
				{
					sum += 1;
				}
			}

			guild_info.zone_remain_role_num[curr_zone_id] = sum;
		}
	}
	else
	{
		int sum = 0;
		const std::vector<GuildHonorBattleRoleInfo> & zone_role_info = guild_info.zone_role_info[zone_id];
		for (int i = 0; i < (int)zone_role_info.size(); ++i)
		{
			const GuildHonorBattleRoleInfo & role_info = zone_role_info[i];
			if (this->GetRoleBattleState(role_info.role_id) == GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_DEFAULT)
			{
				sum += 1;
			}
		}

		guild_info.zone_remain_role_num[zone_id] = sum;
	}
}


void SpecialLogicGuildHonorBattle::SendRoleEnterScene(Role * enter_role)
{
	int guild_id = enter_role->GetGuildID();

	Protocol::SCGuildHonorAddRoleInfo protocol;

	protocol.role_info.role_id = enter_role->GetUID();

	protocol.role_info.zone_id = 0;

	protocol.role_info.level = enter_role->GetLevel();
	protocol.role_info.prof = enter_role->GetProfession();
	enter_role->GetName(protocol.role_info.role_name);
	
	m_scene->SendToRole((const char *)&protocol, sizeof(protocol), GuildHonorSendFilter, (void *)&guild_id);
}

void SpecialLogicGuildHonorBattle::SendZoneChangeInfo(int guild_id, int role_id, int zond_id, int reason)
{
	Protocol::SCGuildHonorRoleChange protocol;
	protocol.change_reson = reason;
	protocol.zone_id = zond_id;
	protocol.role_id = role_id;

	Guild * guild = GuildManager::Instance().GetGuild(guild_id);
	if (guild == NULL)
	{
		gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  %s guild:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), guild_id);
		return;
	}

	GuildHonorBattleGuildInfo * guild_info_ptr=this->GetGuildHonorBattleGuildInfo(guild->GetGuildID());
	if (guild_info_ptr == NULL)
	{
		return;
	}

	const GuildHonorBattleGuildInfo & guild_info = *guild_info_ptr;
	for (std::map<int, std::vector<GuildHonorBattleRoleInfo> >::const_iterator zone_iter = guild_info.zone_role_info.begin(); zone_iter != guild_info.zone_role_info.end(); ++zone_iter)
	{
		const std::vector<GuildHonorBattleRoleInfo> & role_info_vec = zone_iter->second;
		for (int i = 0; i < (int)role_info_vec.size(); ++i)
		{
			Role * tmp_role = m_scene->GetRoleByUID(role_info_vec[i].role_id);
			if (tmp_role == NULL)
			{
				continue;
			}

			EngineAdapter::Instance().NetSend(tmp_role->GetNetId(), &protocol, sizeof(protocol));
		}
	}
}

void SpecialLogicGuildHonorBattle::SendGuildZoneInfo(int guild_id, Role * role)
{
	Protocol::SCGuildHonorRoleInfo protocol;
	protocol.count = 0;

	GuildHonorBattleGuildInfo * guild_info_ptr = this->GetGuildHonorBattleGuildInfo(guild_id);
	if (guild_info_ptr)
	{
		const GuildHonorBattleGuildInfo & guild_info = *guild_info_ptr;
		for (std::map<int, std::vector<GuildHonorBattleRoleInfo> >::const_iterator map_iter = guild_info.zone_role_info.begin(); map_iter != guild_info.zone_role_info.end(); ++map_iter)
		{
			for (std::vector<GuildHonorBattleRoleInfo>::const_iterator vec_iter = map_iter->second.begin(); vec_iter != map_iter->second.end() && protocol.count < ARRAY_LENGTH(protocol.role_info); ++vec_iter)
			{
				Protocol::MsgGuildHonorRoleItemInfo & role_item = protocol.role_info[protocol.count];

				role_item.role_id = vec_iter->role_id;
				role_item.zone_id = (short)map_iter->first;

				UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(vec_iter->role_id);
				if (NULL != user_cache_node)
				{
					role_item.level = user_cache_node->level;
					role_item.prof = user_cache_node->profession;
					user_cache_node->GetName(role_item.role_name);
				}

				protocol.count += 1;
			}
		}
	}

	unsigned int send_length = sizeof(protocol) - (ARRAY_LENGTH(protocol.role_info) - protocol.count) * sizeof(protocol.role_info[0]);

	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, send_length);
	}
	else
	{
		m_scene->SendToRole((const char *)&protocol, sizeof(protocol), GuildHonorSendFilter, &guild_id);
	}
}

void SpecialLogicGuildHonorBattle::SendGuildZoneBattleRoleNumInfo(Role * role)
{
	for (std::map<int, GuildHonorBattleGuildInfo * >::iterator iter = m_guild_battle_info.begin(); iter != m_guild_battle_info.end(); ++iter)
	{
		GuildHonorBattleGuildInfo & guild_info = *iter->second;
		if (role != NULL && role->GetGuildID() != guild_info.guild_id)
		{
			continue;
		}

		Protocol::SCGuildHonorAllZoneRoleNumInfo protocol;
		for (int i = 0; i < ARRAY_LENGTH(protocol.zone_role_num); ++i)
		{
			int zone_id = i + 1;
			protocol.zone_role_num[i] = guild_info.zone_remain_role_num[zone_id];
		}

		if (role != NULL)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
			break;
		}
		else
		{
			int guild_id = iter->first;
			m_scene->SendToRole((const char *)&protocol, sizeof(protocol), GuildHonorSendFilter, &guild_id);
		}
	}
}

void SpecialLogicGuildHonorBattle::SendZoneBattleResult(int zone_id)
{
	int zone_index = zone_id - 1;

	if (zone_index < 0 || zone_index >= ARRAY_LENGTH(m_battle_result))
	{
		return;
	}

	Protocol::SCGuildHonorZoneBattleResultInfo protocl;
	protocl.zone_id = zone_id;
	protocl.win_guild_id = m_battle_result[zone_index];

	m_scene->SendToRole((const char *)&protocl, sizeof(protocl));
}

void SpecialLogicGuildHonorBattle::SendAllZoneBattleResult(Role * role)
{
	Protocol::SCGuildHonorAllZoneBattleResultInfo protocol;

	for (int i = 0; i < ARRAY_LENGTH(protocol.zone_battle_result) && i < ARRAY_LENGTH(m_battle_result); ++i)
	{
		protocol.zone_battle_result[i] = m_battle_result[i];
	}

	int temp_count = 0;
	for (std::map<int, GuildHonorBattleGuildInfo *>::iterator iter = m_guild_battle_info.begin(); iter != m_guild_battle_info.end(); ++iter)
	{
		if (temp_count >= ARRAY_LENGTH(protocol.guild_info))
		{
			break;
		}

		int guild_id = iter->first;
		Guild * guild = GuildManager::Instance().GetGuild(guild_id);
		if (guild == NULL)
		{
			gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | %s guild:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), guild_id);
			continue;
		}
		protocol.guild_info[temp_count].guild_id = guild_id;
		guild->GetName2(protocol.guild_info[temp_count].guild_name);

		temp_count += 1;
	}


	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}
	else
	{
		m_scene->SendToRole((const char *)&protocol, sizeof(protocol));
	}
}

void SpecialLogicGuildHonorBattle::SendBattleStateBuyRoleId(int role_id)
{
	if (role_id == 0)
	{
		this->SendBattleState(NULL);
	}
	else
	{
		Role * role = m_scene->GetRoleByUID(role_id);
		if (role == NULL)
		{
			return;
		}

		this->SendBattleState(role);
	}

}

void SpecialLogicGuildHonorBattle::SendBattleState(Role * role)
{
	Protocol::SCGuildHonorRoleBattleState protocol;
	protocol.state = GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_DEFAULT;
	if (role == NULL)
	{
		for (std::map<int, int>::iterator iter = m_role_battle_state.begin(); iter != m_role_battle_state.end(); ++iter)
		{
			protocol.state = iter->second;
			Role * tmep_role = m_scene->GetRoleByUID(iter->first);
			if (tmep_role)
			{
				EngineAdapter::Instance().NetSend(tmep_role->GetNetId(), &protocol, sizeof(protocol));
			}
		}
	}
	else
	{
		std::map<int, int>::iterator iter = m_role_battle_state.find(role->GetUID());
		if (iter != m_role_battle_state.end())
		{
			protocol.state = iter->second;
		}

		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}
}

bool SpecialLogicGuildHonorBattle::HasRole(int role_id)
{
	for (std::map<int, GuildHonorBattleGuildInfo *>::iterator iter = m_guild_battle_info.begin(); iter != m_guild_battle_info.end(); ++iter)
	{
		GuildHonorBattleGuildInfo & guild_info = *iter->second;
		for (std::map<int, std::vector<GuildHonorBattleRoleInfo> >::iterator zone_iter = guild_info.zone_role_info.begin(); zone_iter != guild_info.zone_role_info.end(); ++zone_iter)
		{
			std::vector<GuildHonorBattleRoleInfo> & role_info_vec = zone_iter->second;
			for (int i = 0; i < (int)role_info_vec.size(); ++i)
			{
				if (role_info_vec[i].role_id == role_id)
				{
					return true;
				}
			}
		}
	}

	return false;
}

void SpecialLogicGuildHonorBattle::SetResult(int zone_id, int winner_guild_id, const char * reason)
{
	if (!m_start_battle || m_is_finish)
	{
		return;
	}

	int zone_index = zone_id - 1;
	if (zone_index < 0 || zone_index >= ARRAY_LENGTH(m_battle_result))
	{
		return;
	}

	if (m_battle_result[zone_index] != 0)
	{
		return;
	}
	m_battle_result[zone_index] = winner_guild_id;

	this->SetZoneBattleId(zone_id, 0);
	this->SendZoneBattleResult(zone_id);

	GuildHonorBattleManager::Instance().SetZoneResult(m_win_rank, m_guild_battle_info1.guild_id, m_guild_battle_info2.guild_id, zone_id, winner_guild_id, reason);
}

void SpecialLogicGuildHonorBattle::SetZoneBattleId(int zone_id, int battle_id)
{
	GuildHonorBattleManager::Instance().SetZoneBattleId(m_win_rank, m_guild_battle_info1.guild_id, m_guild_battle_info2.guild_id, zone_id, battle_id);
}

void SpecialLogicGuildHonorBattle::CheakFinish(bool is_force)
{
	if (m_is_finish)
	{
		return;
	}

	if (!is_force)
	{
		for (int i = 0; i < ARRAY_LENGTH(m_battle_result); ++i)
		{
			if (m_battle_result[i] == 0)
			{
				return;
			}
		}
	}

	gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | %s", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str());

	m_is_finish = true;

	for (std::map<int, GuildHonorBattleGuildInfo *>::iterator iter = m_guild_battle_info.begin(); iter != m_guild_battle_info.end(); ++iter)
	{
		int guid_id = iter->first;
		GuildHonorBattleGuildInfo & guild_info = *iter->second;

		int win_count = 0;
		for (int i = 0; i < ARRAY_LENGTH(m_battle_result); ++i)
		{
			if (m_battle_result[i] == -1 || m_battle_result[i] == 0)
			{
				continue;  //平手
			}

			m_battle_result[i] == guid_id ? win_count += 1 : win_count -= 1;
		}

		int reward_type = ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_DRAW;
		if (win_count > 0)
		{
			reward_type = ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_WIN;

			if (win_count >= GUILD_HONOR_BATTLE_MAX_ZONE)
			{
				reward_type = ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_PERFECT_WIN;
			}
		}

		if (win_count < 0)
		{
			reward_type = ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_LOSE;
		}

		if (reward_type == ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_DRAW)
		{
			guid_id == m_win_guild_id ? reward_type = ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_WIN : reward_type = ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_LOSE;
		}

		guild_info.finish_reward_type = reward_type;
		GuildHonorBattleManager::Instance().SetRoundResult(m_win_rank, guid_id, reward_type != ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_LOSE); 
	}

	this->SendReward();
	this->DelayKickOutAllRole();
}

void SpecialLogicGuildHonorBattle::SendFinalBattleResult()
{
}

void SpecialLogicGuildHonorBattle::SendReward()
{
	for (std::map<int, GuildHonorBattleGuildInfo *>::iterator iter = m_guild_battle_info.begin(); iter != m_guild_battle_info.end(); ++iter)
	{
		int guild_id = iter->first;
		const GuildHonorBattleGuildInfo & guild_info = *iter->second;
		Guild * guild = GuildManager::Instance().GetGuild(guild_id);
		if (guild == NULL)
		{
			gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | %s guild:%d", __FUNCTION__, __LINE__, this->GetBaseInfoString().c_str(), guild_id);
			continue;
		}

		int reward_type = guild_info.finish_reward_type;
		const ActivityGuildHonorBattleRewardCfg * reward_cfg = LOGIC_CONFIG->GetActivityGuildHonorBattleConfig()->GetActivityReward(reward_type);
		if (reward_cfg == NULL)
		{
			continue;
		}

		guild->AddExp(reward_cfg->guild_coin);
		const std::vector<ItemConfigData> & rewards = reward_cfg->items;

		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();

		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = reward_cfg->coin;
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN] = reward_cfg->gong_xian;


		for (int k = 0; k < (int)rewards.size() && k < MAX_ATTACHMENT_ITEM_NUM; ++k)
		{
			const ItemConfigData & reward = rewards[k];

			const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
			if (item_base == NULL)
			{
				continue;
			}

			contentparam.item_list[k].item_id = reward.item_id;
			contentparam.item_list[k].num = reward.num;
			contentparam.item_list[k].is_bind = (reward.is_bind) ? 1 : 0;
			contentparam.item_list[k].invalid_time = item_base->CalInvalidTime();
		}

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_honor_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_honor_reward_content, reward_type);
		if (lensub <= 0 || length <= 0) continue;

		const std::map<int, std::vector<GuildHonorBattleRoleInfo> > & zone_role_info = guild_info.zone_role_info;
		for (std::map<int, std::vector<GuildHonorBattleRoleInfo> >::const_iterator zone_iter = zone_role_info.begin(); zone_iter != zone_role_info.end(); ++zone_iter)
		{
			const std::vector<GuildHonorBattleRoleInfo> & zone_role_list = zone_iter->second;
			for (int i = 0; i < (int)zone_role_list.size(); ++i)
			{
				MailRoute::MailToUser(zone_role_list[i].role_id, MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}
}

GuildHonorBattleGuildInfo * SpecialLogicGuildHonorBattle::GetGuildHonorBattleGuildInfo(int guild_id)
{
	if (m_guild_battle_info1.guild_id == guild_id)
	{
		return &m_guild_battle_info1;
	}

	if (m_guild_battle_info2.guild_id == guild_id)
	{
		return  &m_guild_battle_info2;
	}

	return NULL;
}

int SpecialLogicGuildHonorBattle::GetRoleBattleState(int role_id)
{
	std::map<int, int>::iterator iter = m_role_battle_state.find(role_id);
	if (iter != m_role_battle_state.end())
	{
		return iter->second;
	}

	return GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_DEFAULT;
}

void SpecialLogicGuildHonorBattle::AutoAssignedBeforStartBattle()
{
	for (std::map<int, GuildHonorBattleGuildInfo *>::iterator iter = m_guild_battle_info.begin(); iter != m_guild_battle_info.end(); ++iter)
	{
		 GuildHonorBattleGuildInfo & guild_info = *iter->second;
		 guild_info.AutoAssigned();
		this->SendGuildZoneInfo(iter->first);
	}
}

void SpecialLogicGuildHonorBattle::CheckRoleReallyInBattle(Role* role)
{
	BattleManagerLocal::GetInstance().CheckRoleReallyInBattle(role, __FUNCTION__); 
}

std::string SpecialLogicGuildHonorBattle::GetBaseInfoString()
{
	std::string ret = STRING_SPRINTF("win_rank:%d curr_round:%d guild1[%d %s] vs guild2[%d %s] ",m_win_rank,m_round,
		m_guild_battle_info1.guild_id, m_guild_battle_info1.guild_name,
		m_guild_battle_info2.guild_id, m_guild_battle_info2.guild_name);
	return ret;
}

bool GuildHonorBattleGuildInfo::IsNobodyEachZone()
{
	for (std::map<int, std::vector<GuildHonorBattleRoleInfo> >::iterator zone_iter = zone_role_info.begin(); zone_iter != zone_role_info.end(); ++zone_iter)
	{
		std::vector<GuildHonorBattleRoleInfo> & role_info_vec = zone_role_info[zone_iter->first];
		if (!role_info_vec.empty())
		{
			return false;
		}
	}

	return true;
}

struct GuildHonorBattleRoleNumSort
{
	bool operator<(const GuildHonorBattleRoleNumSort & other)const
	{
		if (role_num < other.role_num)
			return true;
		if (role_num > other.role_num)
			return false;

		if (zone_id < other.zone_id)
			return true;
		if (zone_id > other.zone_id)
			return false;

		return false;
	}

	int role_num;
	int zone_id;
};

void GuildHonorBattleGuildInfo::AutoAssigned()
{
	int unassigned_zone_id = 0;

	std::vector<GuildHonorBattleRoleInfo> & unassigned_role = zone_role_info[unassigned_zone_id];

	for (int i = 0; i < (int)unassigned_role.size(); ++i)
	{
		int role_id = unassigned_role[i].role_id;
		std::vector<GuildHonorBattleRoleNumSort> sort_vec;
		for (int zone_id = 1; zone_id <= GUILD_HONOR_BATTLE_MAX_ZONE; ++zone_id)
		{
			GuildHonorBattleRoleNumSort t;
			t.zone_id = zone_id;
			t.role_num = zone_role_info[zone_id].size();
			sort_vec.push_back(t);
		}

		const int * zone_id_ptr = MapValuesConstPtr(role_to_zone_id, role_id);
		if (zone_id_ptr && *zone_id_ptr != unassigned_zone_id)
		{
			zone_role_info[*zone_id_ptr].push_back(role_id);
		}
		else
		{
			std::sort(sort_vec.begin(), sort_vec.end());
			GuildHonorBattleRoleNumSort min = sort_vec[0];
			zone_role_info[min.zone_id].push_back(role_id);
		}
	}

	unassigned_role.clear();
}
