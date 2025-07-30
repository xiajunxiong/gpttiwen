#include "world.h"
#include "engineadapter.h"
#include "internalcomm.h"
#include "servercommon/errornum.h"
#include "global/guild/guildmanager.hpp"
#include "global/guild/guild.hpp"
#include "global/cross/crossmanager.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/route/mailroute.hpp"
#include "config/logicconfigmanager.hpp"
#include "gamelog.h"
#include "global/activity/activitymanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"

#include "global/commonsave/commonsavemanager.hpp"
#include "global/activity/activityimpl/activityguildhonorbattle.hpp"
#include "protocol/msgguildhonorbattle.h"
#include "config/activityconfig/guildhonorbattle/activityguildhonorbattleconfig.hpp"
#include "scene/speciallogicimpl/speciallogicguildhonorbattle.hpp"
#include "guildhonorbattlemanager.hpp"


#include<cmath>


GuildHonorBattleManager & GuildHonorBattleManager::Instance()
{
	static GuildHonorBattleManager r;
	return r;
}

bool GuildHonorBattleManager::NEED_SEND = false;

void GuildHonorBattleManager::OnWeekChange()
{
	m_guild_fight_data->ClearAllData();
	m_guild_fight_data->Save();

	CommonSaveGuildHonorData * honor_data = this->GetGuildHonorData();
	if (honor_data)
	{
		const ActivityGuildHonorBattleOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityGuildHonorBattleConfig()->GetOtherCfg();
		honor_data->guild_honor_process = GUILD_HONOR_BATTLE_PROCESS_WEEK_START;
		honor_data->settlement_list_time = (unsigned int)(EngineAdapter::Instance().Time() +
								  EngineAdapter::Instance().NextWeekInterval(other_cfg.settlement_list_time_wday, other_cfg.settlement_list_time_hour, other_cfg.settlement_list_time_min, 0));
		m_guild_honor_info_data->Save();

		this->SendProcessInfo();
	}
}

void GuildHonorBattleManager::OnUserLogin(Role * role)
{
	this->SendProcessInfo(false, role);
	this->SendGuildHonorTopInfo(role);

	if (ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_GUILD_HONOR_BATTLE))
	{
		this->SendAllMatchInfo(false, role, 0);
	}		
}

void GuildHonorBattleManager::OnUserLoginOriginGame(int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	this->SendProcessInfo(false, NULL, role_id);


	if (ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_GUILD_HONOR_BATTLE))
	{
		this->SendAllMatchInfo(false, NULL, role_id);
	}	
}

void GuildHonorBattleManager::OnConnectCrossServerSucc()
{
}

void GuildHonorBattleManager::OnGameServerConnect(std::set<int> server_id)
{
	if (server_id.empty())
	{
		return;
	}

	this->HiddenToGameSyncGuildHonorRecord(*server_id.begin(), false);	
}

void GuildHonorBattleManager::Update(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	for (std::set<SceneIndex>::iterator iter = m_need_del_scene_index.begin(); iter != m_need_del_scene_index.end(); )
	{
		if (!this->IsGuildHonorBattleScene(*iter) || World::GetInstWorld()->GetSceneManager()->DestroyFB(*iter))
		{
			m_need_del_scene_index.erase(iter++);
		}
		else
		{
			++iter;
		}
	}

	this->UpdataProcess(now_second);
	this->UpdateRankScore();
	this->UpdateMatchInfos(now_second);
}

int GuildHonorBattleManager::UpdataProcess(time_t now_second)
{
	CommonSaveGuildHonorData * honor_data = this->GetGuildHonorData();
	if (!honor_data)
	{
		return -__LINE__;
	}

	if (honor_data->settlement_list_time == 0)
	{
		const ActivityGuildHonorBattleOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityGuildHonorBattleConfig()->GetOtherCfg();
		honor_data->settlement_list_time = (unsigned int)(EngineAdapter::Instance().Time() +
								  EngineAdapter::Instance().NextWeekInterval(other_cfg.settlement_list_time_wday, other_cfg.settlement_list_time_hour, other_cfg.settlement_list_time_min, 0));
	}

	if (now_second < honor_data->settlement_list_time)
	{
		return -__LINE__;
	}

	if (honor_data->guild_honor_process >= GUILD_HONOR_BATTLE_PROCESS_READY)
	{
		return -__LINE__;
	}

	honor_data->guild_honor_process = GUILD_HONOR_BATTLE_PROCESS_READY;
	this->UpdateRankScore(true);
	this->SendProcessInfo();

	// 传闻
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_guild_honor_settlement_list_notice);
	if (length > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgAllGame(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
	}

	return 0;
}

bool GuildHonorBattleScoreSort(const  CommonSaveGuildFightData & d1, const  CommonSaveGuildFightData & d2)
{
	const ActivityGuildHonorBattleOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityGuildHonorBattleConfig()->GetOtherCfg();

	int sum1 = 0;
	int sum2 = 0;
	int last_index = other_cfg.GetLastVaildDay();

	for (int i = 0; i < ARRAY_LENGTH(d1.records); ++i)
	{
		if (!other_cfg.IsVaildDay(i))
		{
			continue;
		}

		sum1 += d1.records[i].score;
		sum2 += d2.records[i].score;
	}

	if (sum1 > sum2)
		return true;
	if (sum1 < sum2)
		return false;

	bool last_join1 = d1.join_guild_fight_day & 1 << last_index;
	bool last_join2 = d2.join_guild_fight_day & 1 << last_index;

	if (last_join1 && !last_join2)			//总分数相同则 天数大为准，周二和周四 都有，就用周四的
		return true;
	if (!last_join1 && last_join2)
		return false;

	if (d1.records[last_index].score > d2.records[last_index].score)
		return true;
	if (d1.records[last_index].score < d2.records[last_index].score)
		return false;

	if (d1.records[last_index].time < d2.records[last_index].time)
		return true;
	if (d1.records[last_index].time > d2.records[last_index].time)
		return false;

	if (d1.guild_id < d2.guild_id)
		return true;
	if (d1.guild_id > d2.guild_id)
		return false;

	return false;
}

void GuildHonorBattleManager::UpdateRankScore(bool is_force)
{
	if (!is_force && !m_is_score_rank_need_update)
	{
		return;
	}

	m_is_score_rank_need_update = false;

	m_score_rank.clear();

	RetCommonSaveData get_data;
	m_guild_fight_data->BeginDataForeach();
	while (m_guild_fight_data->GetNextData(get_data))
	{
		int guild_id = get_data.key.param1;
		const CommonSaveGuildFightData & fight_data = get_data.data.guild_fight_data;
		if (guild_id == 0)
		{
			continue;
		}

		//bool has_score = false;
		//for (int i = 0; i < ARRAY_LENGTH(fight_data.records); ++i)
		//{
		//	if (!other_cfg.IsVaildDay(i) || fight_data.records[i].score == 0)
		//	{
		//		continue;
		//	}

		//	has_score = true;
		//	break;
		//}

		//if (!has_score)
		//{
		//	continue;
		//}

		m_score_rank.push_back(fight_data);
	}

	std::sort(m_score_rank.begin(), m_score_rank.end(), GuildHonorBattleScoreSort);
}

void GuildHonorBattleManager::UpdateMatchInfos(time_t now_second)
{
	if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_GUILD_HONOR_BATTLE))
	{
		return;
	}

	for (int i = 0; i < (int)m_add_matchs.size(); ++i)
	{
		GuildHonorBattleMatchInfo & curr = m_add_matchs[i];
		curr.is_start = true;

		GuildHonorBattleMatchInfo * first_match_info = GetGuildHonorBattleFirstMatchInfo();
		if (first_match_info)
		{
			curr.start_first_round = first_match_info->curr_match_round;
			curr.round_state = first_match_info->round_state;
			curr.next_round_state_time = first_match_info->next_round_state_time;
		}

		if (curr.figther_info.size() <= 1)
		{
			curr.OnMatchFinish();
		}

		MapAdd(m_match_infos, curr.win_rank, curr);
		GuildHonorBattleManager::NEED_SEND = true;
	}
	m_add_matchs.clear();

	this->CheckAllMatchFinish();

	for (std::map<int, GuildHonorBattleMatchInfo>::iterator iter = m_match_infos.begin(); iter != m_match_infos.end(); ++iter)
	{
		GuildHonorBattleMatchInfo & match_info = iter->second;
		match_info.Update(now_second);
	}

	if (GuildHonorBattleManager::NEED_SEND)
	{
		GuildHonorBattleManager::NEED_SEND = false;
		this->SendAllMatchInfo(true);
	}	
}


void GuildHonorBattleManager::CheckAllMatchFinish()
{
	if (m_all_match_finish)
	{
		return;
	}

	for (std::map<int, GuildHonorBattleMatchInfo>::iterator iter = m_match_infos.begin(); iter != m_match_infos.end(); ++iter)
	{
		const GuildHonorBattleMatchInfo & match_info = iter->second;
		if (!match_info.is_match_finish)
		{
			return;
		}
	}

	m_all_match_finish = true;

	std::map<int, GuildHonorBattleFigtherInfo> rank_map;
	std::vector<GuildHonorBattleFigtherInfoRecordItemTemp> record_list;
	for (std::map<int, GuildHonorBattleMatchInfo>::iterator iter = m_match_infos.begin(); iter != m_match_infos.end(); ++iter)
	{
		const GuildHonorBattleMatchInfo & match_info = iter->second;
		std::vector<GuildHonorBattleFigtherInfo> temp = MapValuesAsVector(match_info.figther_info);

		std::sort(temp.begin(), temp.end(), GuildHonorBattleMatchInfo::SortFuncFinish);
	
		for (int i = 0; i < (int)temp.size(); ++i)
		{
			GuildHonorBattleFigtherInfoRecordItemTemp t;
			t.rank = 0;
			t.start_first_round = match_info.start_first_round;
			t.fighter_info = temp[i];
			if (i == 0)
			{
				t.rank = temp[0].win_rank;
				rank_map[match_info.win_rank] = temp[0];
			}

			record_list.push_back(t);
		}
	}

	static long long GUILD_HONOR_OPEN_ID = 0;		//每次活动开启时新赋值，用来这个id 来记录log表示是这一场的
	UniqueIDGenerator::Instance()->GenUniqueID(&GUILD_HONOR_OPEN_ID);

	for (std::map<int, GuildHonorBattleFigtherInfo> ::iterator iter = rank_map.begin(); iter != rank_map.end(); ++iter)
	{
		const GuildHonorBattleFigtherInfo & curr = iter->second;
		int rank = iter->first;
		int guild_id = curr.guild_id;

		gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  rank:%d %s ", __FUNCTION__, __LINE__, rank, curr.ToString().c_str());
		ROLE_LOG_QUICK16(IntToUid(guild_id).db_index, LOG_TYPE_GUILD_HONOR_RESULT, guild_id, curr.guild_name, NULL, rank, GUILD_HONOR_OPEN_ID, NULL, NULL, IntToUid(guild_id).db_index, 0, 0, 0, 0);

		const ActivityGuildHonorBattleRankRewardCfg * reward_cfg = LOGIC_CONFIG->GetActivityGuildHonorBattleConfig()->GetActivityGuildHonorBattleRankRewardCfg(rank);
		if (reward_cfg == NULL)
		{
			continue;
		}

		Guild * guild = GuildManager::Instance().GetGuild(guild_id);
		if (guild == NULL)
		{
			continue;
		}

		// 发邮件
		static MailContentParam contentparam;
		{
			bool is_join = true;
			contentparam.Reset();
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = reward_cfg->coin;
			::FillMailContentParam(contentparam, reward_cfg->rewards);

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_honor_battle_rank_reward_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_honor_battle_rank_reward_content, rank);
			if (lensub > 0 && length > 0) guild->GetMemberManager()->SendMailToAllMember(MAIL_REASON_DEFAULT, contentparam, MAIL_TYPE_SYSTEM, FilterJoinHonor, &is_join);
		}
		{
			bool is_join = false;
			contentparam.Reset();
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = reward_cfg->low_winner_coin;
			::FillMailContentParam(contentparam, reward_cfg->low_rewards);

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_honor_battle_rank_reward_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_honor_battle_rank_unjoin_reward_content, rank);
			if (lensub > 0 && length > 0) guild->GetMemberManager()->SendMailToAllMember(MAIL_REASON_DEFAULT, contentparam, MAIL_TYPE_SYSTEM, FilterJoinHonor, &is_join);
		}

		guild->GetMemberManager()->ClearAllMemberHonorBattleFlag();
	}

	GuildHonorBattleManager::Instance().SetGuildHonorRecord(record_list, true);

	if (is_zero_fighter)
	{
		ActivityGuildHonorBattle * activity = (ActivityGuildHonorBattle *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_HONOR_BATTLE);
		if (activity != NULL && activity->IsActivityOpen())
		{
			activity->ForceToCloseState();
		}
	}
}

bool GuildHonorBattleManager::IsJoinMatchGuild(int guild_id) const
{
	return m_join_match_guild_id.find(guild_id) != m_join_match_guild_id.end();
}

void GuildHonorBattleManager::OnGuildFightHappen()
{
	CommonSaveGuildHonorData * honor_data = this->GetGuildHonorData();
	if (!honor_data)
	{
		return;
	}

	if (honor_data->guild_honor_process >= GUILD_HONOR_BATTLE_PROCESS_GUILD_FIGHT)
	{
		return;
	}

	honor_data->guild_honor_process = GUILD_HONOR_BATTLE_PROCESS_GUILD_FIGHT;
	this->SendProcessInfo();
}

int GuildHonorBattleManager::SetGuildFightResult(int guild_id, int score, unsigned int score_change_time)
{
	Guild * guild = GuildManager::Instance().GetGuild(guild_id);
	if (guild == NULL)
	{
		return -__LINE__;
	}

	const CommonSaveGuildHonorData * honor_data = this->GetGuildHonorData();
	if (honor_data == NULL)
	{
		return -__LINE__;
	}

	CommonSaveGuildFightData * guild_fight_data = this->GetGuildFightData(guild_id);
	if (guild_fight_data == NULL)
	{
		return -__LINE__;
	}

	int combine_server_id = 0;
	{
		int server_id = IntToUid(guild_id).db_index;
		combine_server_id = server_id;
		UniqueServerID origin_usid(LocalConfig::Instance().GetPlatType(), server_id);
		const UniqueServerID * usid = World::GetInstWorld()->GetMotherServerId(origin_usid);
		if (usid)
		{
			combine_server_id = usid->server_id;
		}
	}

	guild_fight_data->server_id = combine_server_id;
	guild_fight_data->guild_id = guild_id;
	guild->GetGuildName(guild_fight_data->guild_name);
	memcpy(guild_fight_data->banner, guild->GetBanner(), sizeof(guild_fight_data->banner));
	guild->GetMemberManagerRef().GetPatriarchName(guild_fight_data->patriarch_name);

	//和策划确认过，一天不会开多次
	const tm * now_tm = EngineAdapter::Instance().LocalTime();
	int index = now_tm->tm_wday;
	if (index < 0 || index >= ARRAY_LENGTH(guild_fight_data->records))
	{
		return -__LINE__;
	}

	guild_fight_data->join_guild_fight_day |= 1 << index;
	CommonSaveGuildFightRecord & record = guild_fight_data->records[index];
	record.score = score;
	record.time = score_change_time;

	m_is_score_rank_need_update = true;

	return 0;
}

void GuildHonorBattleManager::SetGuildHonorRecord(std::vector<GuildHonorBattleFigtherInfoRecordItemTemp> & record_list, bool is_finish)
{
	std::sort(record_list.begin(), record_list.end());

	CommonSaveGuildHonorData * honor_data = this->GetGuildHonorData();
	if (is_finish && honor_data != NULL)
	{
		honor_data->guild_honor_process = GUILD_HONOR_BATTLE_PROCESS_FINISH;		
		m_guild_honor_info_data->Save();
		this->SendProcessInfo();
	}

	CommonSaveGuildHonorRecordData * record_data = this->GetGuildHonorRecordData();
	if (record_data == NULL)
	{
		return;
	}

	std::map<int, int> change_list;
	for (int i = 0; i < ARRAY_LENGTH(record_data->record_item); ++i)
	{
		CommonSaveGuildHonorRecordDataItem & record_item = record_data->record_item[i];	
		change_list[record_item.guild_id] = 0;
		record_item.Reset();

		if (i < (int)record_list.size())
		{
			//赋值
			const GuildHonorBattleFigtherInfo & src_item = record_list[i].fighter_info;
			record_item.finish_rank = record_list[i].rank;
			record_item.start_first_round = record_list[i].start_first_round;
			record_item.win_rank = src_item.win_rank;
			record_item.number = src_item.number;
			record_item.match_round = src_item.match_round;
			record_item.guild_fight_rank = src_item.guild_fight_rank;

			record_item.guild_id = src_item.guild_id;
			record_item.server_id = src_item.server_id;
			memcpy(record_item.guild_name, src_item.guild_name, sizeof(record_item.guild_name));
			memcpy(record_item.banner, src_item.banner, sizeof(record_item.banner));
			memcpy(record_item.patriarch_name, src_item.patriarch_name, sizeof(record_item.patriarch_name));

			record_item.champion_guess_num = src_item.champion_guess_num;
			for (int i = 0; i < GUILD_HONOR_BATTLE_MAX_ROUND; ++i)
			{
				record_item.guess_num[i] = src_item.guess_num[i];
			}

			record_item.wday_flag = src_item.wday_flag;

			for (int i = 0; i < ARRAY_LENGTH(record_item.score_records) && i < ARRAY_LENGTH(src_item.score_records); ++i)
			{
				record_item.score_records[i] = src_item.score_records[i];
			}

			change_list[record_item.guild_id] = record_item.finish_rank;
		}
	}

	m_guild_honor_record_data->Save();

	for (std::map<int, int>::iterator iter = change_list.begin(); iter != change_list.end(); ++iter)
	{
		Guild * guild = GuildManager::Instance().GetGuild(iter->first);
		if (guild != NULL)
		{
			guild->UpdateGuildHonorTitleId(iter->second);
		}
	}

	this->SendGuildHonorTopInfo();
	this->HiddenToGameSyncGuildHonorRecord();
}

void GuildHonorBattleManager::HiddenToGameSyncGuildHonorRecord(int server_id, bool is_notice)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSaveGuildHonorRecordData * record_data = this->GetGuildHonorRecordData();
	if (record_data == NULL)
	{
		return;
	}

	crossgameprotocal::HiddenGameGuildHonorBattleMatchRecordSyncInfo protocol;
	protocol.is_notice = is_notice;
	protocol.match_record_data = *record_data;

	if (server_id == 0)
	{
		InternalComm::Instance().SendToAllGameThroughCross((const char *)&protocol, sizeof(protocol));
	}
	else
	{
		InternalComm::Instance().SendToGameThroughCross2((const char *)&protocol, sizeof(protocol), server_id);
	}		
}

void GuildHonorBattleManager::SetGuildHonorRecordFromHidden(const CommonSaveGuildHonorRecordData & record, bool is_notice)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSaveGuildHonorRecordData * record_data = this->GetGuildHonorRecordData();
	if (record_data == NULL)
	{
		return;
	}

	std::set<int> change_list;
	for (int i = 0; i < ARRAY_LENGTH(record_data->record_item); ++i)
	{
		CommonSaveGuildHonorRecordDataItem & record_item = record_data->record_item[i];
		change_list.insert(record_item.guild_id);
		record_item.Reset();
	}

	for (int i = 0; i < ARRAY_LENGTH(record.record_item); ++i)
	{
		const CommonSaveGuildHonorRecordDataItem & record_item = record.record_item[i];
		change_list.insert(record_item.guild_id);
	}

	*record_data = record;
	m_guild_fight_data->Save();

	for (std::set<int>::iterator iter = change_list.begin(); iter != change_list.end(); ++iter)
	{
		Guild * guild = GuildManager::Instance().GetGuild(*iter);
		if (guild != NULL)
		{
			guild->UpdateGuildHonorTitleIdFromHidden();
		}
	}

	this->SendGuildHonorTopInfo();
}

int GuildHonorBattleManager::GetMatchTitle(int guild_id)
{
	CommonSaveGuildHonorRecordData * record_data = this->GetGuildHonorRecordData();
	if (record_data == NULL)
	{
		return 0;
	}

	if (guild_id == 0)
	{
		return 0;
	}

	int rank = this->GetMatchRank(guild_id);
	const ActivityGuildHonorBattleRankRewardCfg * reward_cfg = LOGIC_CONFIG->GetActivityGuildHonorBattleConfig()->GetActivityGuildHonorBattleRankRewardCfg(rank);
	if (reward_cfg != NULL)
	{
		return reward_cfg->title_id;
	}

	return 0;
}

int GuildHonorBattleManager::GetMatchRank(int guild_id)
{
	CommonSaveGuildHonorRecordData * record_data = this->GetGuildHonorRecordData();
	if (record_data == NULL)
	{
		return 0;
	}

	if (guild_id == 0)
	{
		return 0;
	}

	for (int i = 0; i < ARRAY_LENGTH(record_data->record_item); ++i)
	{
		const CommonSaveGuildHonorRecordDataItem & curr = record_data->record_item[i];
		if (curr.guild_id == guild_id && curr.finish_rank > 0)
		{
			return curr.finish_rank;
		}
	}

	return 0;
}

bool GuildHonorBattleManager::GetSceneKey(int role_id, int & scene_id)
{
	if (m_all_match_finish)
	{
		return false;
	}

	for (std::set<int>::iterator iter = m_join_match_guild_id.begin(); iter != m_join_match_guild_id.end(); ++iter)
	{
		int guild_id = *iter;
		Guild * guild = GuildManager::Instance().GetGuild(guild_id);
		if (NULL == guild)
		{
			continue;
		}

		if (NULL == guild->GetMemberManager()->GetMember(role_id))
		{
			continue;
		}

		GuildHonorBattleMatchInfo * match_info = this->GetGuildHonorBattleMatchInfoByGuildId(guild_id);
		if (!match_info)
		{
			return false;
		}

		return match_info->GetSceneKey(role_id, scene_id);
	}

	return false;
}

int GetGuildHonorBattleMaxMatchRound(int role_num)
{
	return (int)std::ceil(std::log(role_num) / std::log(2));
}

struct GuildHonorJoinRankItem
{
	int guild_guild_rank;
	CommonSaveGuildFightData data;
};


void GuildHonorBattleManager::OnGuildHonorBattleBegin()
{
	const ActivityGuildHonorBattleOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityGuildHonorBattleConfig()->GetOtherCfg();

	this->ResetMatchs();
	this->UpdateRankScore(true);

	GuildHonorBattleMatchInfo match_info;
	match_info.win_rank = 1;

	int join_size = std::min((int)m_score_rank.size(), GUILD_HONOR_BATTLE_FIGHTER_MAX_NUM);

	std::vector<GuildHonorJoinRankItem> tmp;
	for (int i = 0; i < join_size; ++i)
	{
		GuildHonorJoinRankItem t;
		t.guild_guild_rank = i + 1;
		t.data = m_score_rank[i];
		tmp.push_back(t);

		gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | guild[%d %s] guild_guild_rank:%d ", __FUNCTION__, __LINE__, t.data.guild_id, t.data.guild_name, t.guild_guild_rank);
	}

	std::random_shuffle(tmp.begin(), tmp.end());
	std::vector<int> num_vec = this->GetRandNum(join_size);
		
	for (int i = 0; i < join_size; ++i)
	{
		const GuildHonorJoinRankItem & curr = tmp[i];
		int guild_id = curr.data.guild_id;
		int guild_fight_rank = curr.guild_guild_rank;
		int last_match_rank = this->GetMatchRank(guild_id);
		int number = num_vec[i];

		gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | guild[%d %s] number:%d ", __FUNCTION__, __LINE__, guild_id, curr.data.guild_name, number);

		GuildHonorBattleFigtherInfo tmp_fighter(match_info.win_rank, number, guild_fight_rank, last_match_rank, other_cfg.settlement_list_day, curr.data);
		MapAdd(match_info.figther_info, number, tmp_fighter);
		m_join_match_guild_id.insert(guild_id);
	}

	match_info.is_start = true;
	match_info.max_match_round = ::GetGuildHonorBattleMaxMatchRound((int)match_info.figther_info.size());

	this->ClearLastGuildHonorBattleRecord();  //设置 last_match_rank 上次比赛的排名才能调用	
	m_add_matchs.push_back(match_info);

	is_zero_fighter = false;
	if (join_size <= 0)
	{
		is_zero_fighter = true;
	}
}

void GuildHonorBattleManager::OnGuildHonorBattleClose()
{	
	for (std::map<int, GuildHonorBattleMatchInfo>::iterator iter = m_match_infos.begin(); iter != m_match_infos.end(); ++iter)
	{
		GuildHonorBattleMatchInfo & match_info = iter->second;
		if (match_info.is_match_finish)
		{
			return;
		}

		match_info.is_match_finish = true;
		match_info.OnRoundFinish(true);
	}
}

std::vector<int> GuildHonorBattleManager::GetRandNum(int join_num)
{
	std::vector<int> ret;
	if (join_num <= 1)
	{
		ret.push_back(1);
	}
	else
	{
		int max_number = (int)std::pow(2, ::GetGuildHonorBattleMaxMatchRound(join_num));
		int mid = max_number / 2;

		for (int i = 1; i <= mid; ++i)
		{
			ret.push_back(i);
			ret.push_back(i + mid);
		}
	}

	return ret;
}

GuildHonorBattleMatchInfo * GuildHonorBattleManager::GetGuildHonorBattleFirstMatchInfo()
{
	return GetGuildHonorBattleMatchInfoPtr(1);
}


GuildHonorBattleMatchInfo * GuildHonorBattleManager::GetGuildHonorBattleMatchInfoPtr(int win_rank)
{
	return MapValuesPtr(m_match_infos, win_rank);
}

GuildHonorBattleMatchInfo * GuildHonorBattleManager::GetGuildHonorBattleMatchInfoByGuildId(int guild_id)
{
	for (std::map<int, GuildHonorBattleMatchInfo>::iterator iter = m_match_infos.begin(); iter != m_match_infos.end(); ++iter)
	{
		GuildHonorBattleMatchInfo & match_info = iter->second;
		
		for (std::map<int, GuildHonorBattleFigtherInfo> ::iterator iter = match_info.figther_info.begin(); iter != match_info.figther_info.end(); ++iter)
		{
			GuildHonorBattleFigtherInfo & curr = iter->second;
			if (curr.match_round == match_info.curr_match_round && curr.guild_id == guild_id)
			{
				return &match_info;
			}
		}
	}

	return NULL;
}

void GuildHonorBattleManager::SetZoneBattleId(int win_rank, int guild_id1, int guild_id2, int zone_id, int battle_id)
{
	GuildHonorBattleMatchInfo * match_info = this->GetGuildHonorBattleMatchInfoPtr(win_rank);
	if (!match_info)
	{
		return;
	}

	match_info->SetZoneBattleId(guild_id1, guild_id2, zone_id, battle_id);
}

void GuildHonorBattleManager::SetZoneResult(int win_rank, int guild_id1, int guild_id2, int zone_id, int win_guild_id, const char * reason)
{
	GuildHonorBattleMatchInfo * match_info = this->GetGuildHonorBattleMatchInfoPtr(win_rank);
	if (!match_info)
	{
		return;
	}

	match_info->SetZoneResult(guild_id1, guild_id2, zone_id, win_guild_id, reason);
	GuildHonorBattleManager::NEED_SEND = true;
}

void GuildHonorBattleManager::SetRoundResult(int win_rank, int guild_id, bool is_win)
{
	GuildHonorBattleMatchInfo * match_info = this->GetGuildHonorBattleMatchInfoPtr(win_rank);
	if (!match_info)
	{
		return;
	}

	match_info->SetRoundResult(guild_id, is_win);
}

void GuildHonorBattleManager::StartLoserGroupMatch(int slot_num, int win_rank, std::vector<GuildHonorBattleFigtherInfo>  fighter_list)
{
	if (fighter_list.empty())
	{
		return;
	}

	GuildHonorBattleMatchInfo * match_info = this->GetGuildHonorBattleMatchInfoPtr(win_rank);
	if (match_info)
	{
		assert(false);
		return;
	}

	int join_size = (int)fighter_list.size();
	if (slot_num < join_size)
	{
		assert(false);
		slot_num = join_size;
	}

	GuildHonorBattleMatchInfo loer_group_match_info;
	std::vector<int> num_vec = this->GetRandNum(slot_num);

	for (int i = 0; i < (int)fighter_list.size() && i < (int)num_vec.size(); ++i)
	{
		GuildHonorBattleFigtherInfo  & fighter_Info = fighter_list[i];
		int number = num_vec[i];
		fighter_Info.ResetMatchInfo(win_rank, number);
		MapAdd(loer_group_match_info.figther_info, number, fighter_Info);
	}

	loer_group_match_info.win_rank = win_rank;
	loer_group_match_info.max_match_round = ::GetGuildHonorBattleMaxMatchRound(slot_num);

	m_add_matchs.push_back(loer_group_match_info);
}

void GuildHonorBattleManager::AddDelScene(const SceneIndex & scene_index)
{
	m_need_del_scene_index.insert(scene_index);
}


void GuildHonorBattleManager::GoToBattleScene(Role * role)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		if (!CrossManager::Instance().IsCrossServerConnected())
		{
			role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
			return;
		}

		role->GetRoleModuleManager()->GetRoleActivityManager()->JoinActivityInCross(ACTIVITY_TYPE_GUILD_HONOR_BATTLE, 0, true);
		return;
	}
	int guild_id = role->GetGuildID();
	GuildHonorBattleMatchInfo * match_info_ptr =this->GetGuildHonorBattleMatchInfoByGuildId(guild_id);
	if (!match_info_ptr)
	{
		role->NoticeNum(errornum::EN_GUILD_HONOR_BATTLE_CANT_ENTER);
		return;
	}

	 GuildHonorBattleMatchInfo & match_info = *match_info_ptr;
	if (match_info.is_match_finish)
	{
		role->NoticeNum(errornum::EN_GUILD_HONOR_BATTLE_CANT_ENTER_2);
		return;
	}

	if (match_info.round_state != GUILD_HONOR_BATTLE_STATE_READY)
	{
		role->NoticeNum(errornum::EN_GUILD_HONOR_BATTLE_CANT_ENTER);
		return;
	}

	
	GuildHonorBattleRoundBattleInfo * round_battle_info = match_info.GetGuildHonorBattleRoundBattleInfo(guild_id);
	if (!round_battle_info)
	{
		role->NoticeNum(errornum::EN_GUILD_HONOR_BATTLE_CANT_ENTER);
		return;
	}

	if (round_battle_info->is_bye)			//轮空的
	{
		role->NoticeNum(errornum::EN_GUILD_HONOR_BATTLE_CANT_ENTER_IS_BYE);
		return;
	}

	if (role->GetMyTeam())
	{
		role->NoticeNum(errornum::EN_IN_TEAM_CAN_NOT);
		return;
	}

	World::GetInstWorld()->GetSceneManager()->GoTo(role, round_battle_info->scene_info.scene_id, round_battle_info->scene_info.scene_key, round_battle_info->scene_info.birth_pos);
}

void GuildHonorBattleManager::Guessing(int win_rank,int guild_id, bool is_champion, int role_guild_id, int role_id)
{
	GuildHonorBattleMatchInfo* match_info = this->GetGuildHonorBattleMatchInfoPtr(win_rank);
	if (!match_info)
	{
		return;
	}

	if (this->IsJoinMatchGuild(role_guild_id))
	{
		return;
	}

	match_info->Guessing(guild_id, is_champion, role_id, role_guild_id);
}

void GuildHonorBattleManager::CancelGuessing(int win_rank, int guild_id, bool is_champion, int role_id)
{
	GuildHonorBattleMatchInfo * match_info = this->GetGuildHonorBattleMatchInfoPtr(win_rank);
	if (!match_info)
	{
		return;
	}

	match_info->CancelGuessing(guild_id, is_champion, role_id);
}

void GuildHonorBattleManager::SendProcessInfo(bool is_send_all, Role * role, int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	Protocol::SCGuildHonorBattleProcessInfo protocol;
	protocol.process = 0;
	protocol.settlement_list_time = 0;

	CommonSaveGuildHonorData * honor_data = this->GetGuildHonorData();
	if (honor_data)
	{
		protocol.process = honor_data->guild_honor_process;
		protocol.settlement_list_time = honor_data->settlement_list_time;
	}

	if (is_send_all)
	{
		World::GetInstWorld()->SendToALLGameAllGateway((const char *)&protocol, sizeof(protocol));
	}
	else
	{
		if (role)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
		}

		if (role_id)
		{
			InternalComm::Instance().SendToGameUserThroughCross(role_id, (const char *)&protocol, sizeof(protocol));
		}
	}
}

void GuildHonorBattleManager::SendGuildScoreInfo(int guild_id, Role * role, int role_id)
{
	Protocol::SCGuildHonorBattleScoreInfo protocol;
	protocol.guild_id = guild_id;
	protocol.join_guild_fight_day_flag = 0;

	for (int i = 0; i < ARRAY_LENGTH(protocol.score); ++i)
	{
		protocol.score[i] = 0;
	}

	CommonSaveGuildFightData * fight_data = this->GetGuildFightData(guild_id, false, false);
	if (fight_data)
	{
		protocol.join_guild_fight_day_flag = (unsigned short)fight_data->join_guild_fight_day;
		for (int i = 0; i < ARRAY_LENGTH(protocol.score) && i < ARRAY_LENGTH(fight_data->records); ++i)
		{
			protocol.score[i] = fight_data->records[i].score;
		}
	}

	if (role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}

	if (role_id)
	{
		InternalComm::Instance().SendToGameUserThroughCross(role_id, (const char *)&protocol, sizeof(protocol));
	}
}


void GuildHonorBattleManager::SendGuildScoreRankInfo(Role * role, int role_id)
{
	Protocol::SCGuildHonorScoreRankInfo rank_info;
	rank_info.count = 0;

	for (int i = 0; i < (int)m_score_rank.size() && i < ARRAY_LENGTH(rank_info.item); ++i)
	{
		const CommonSaveGuildFightData & curr = m_score_rank[i];
		SCGuildHonorScoreRankInfo::ItemInfo & item = rank_info.item[i];

		item.server_id = curr.server_id;
		item.guild_id = curr.guild_id;
		memcpy(item.name, curr.guild_name, sizeof(item.name));
		memcpy(item.banner, curr.banner, sizeof(item.banner));

		item.join_guild_fight_day_flag = (unsigned short)curr.join_guild_fight_day;
		for (int w = 0; w < ARRAY_LENGTH(item.score) && w < ARRAY_LENGTH(curr.records); ++w)
		{
			item.score[w] = (unsigned short)curr.records[w].score;
		}

		rank_info.count += 1;
	}

	unsigned int send_len = sizeof(rank_info) - (ARRAY_LENGTH(rank_info.item) - rank_info.count) * sizeof(rank_info.item[0]);

	if (role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &rank_info, send_len);
	}

	if (role_id)
	{
		InternalComm::Instance().SendToGameUserThroughCross(role_id, (const char *)&rank_info, send_len);
	}
}

void GuildHonorBattleManager::SendMatchRecordInfo(Role * role, int role_id)
{
	static Protocol::SCGuildHonorBattleMatchRecord protocol;
	protocol.fighter_num = 0;

	CommonSaveGuildHonorRecordData * record_data = this->GetGuildHonorRecordData();
	if (record_data != NULL)
	{
		for (int i = 0; i < ARRAY_LENGTH(record_data->record_item) && protocol.fighter_num < ARRAY_LENGTH(protocol.figther_info); ++i)
		{
			CommonSaveGuildHonorRecordDataItem & figther_item = record_data->record_item[i];
			SCGuildHonorBattleMatchRecord::InfoItem & item = protocol.figther_info[protocol.fighter_num];

			if (figther_item.guild_id == 0)
			{
				continue;
			}

			item.start_first_round = figther_item.start_first_round;
			item.win_rank = figther_item.win_rank;
			item.number = figther_item.number;
			item.server_id = figther_item.server_id;
			item.guild_id = figther_item.guild_id;
			memcpy(item.guild_name, figther_item.guild_name, sizeof(item.guild_name));
			memcpy(item.banner, figther_item.banner, sizeof(item.banner));
			memcpy(item.role_name, figther_item.patriarch_name, sizeof(item.role_name));
			item.guild_fight_rank = figther_item.guild_fight_rank;				//家族大乱斗排名
			item.match_round = figther_item.match_round;		//到达的回合，和curr_round 不同			

			item.champion_guess_num = figther_item.champion_guess_num;
			for (int i = 0; i < GUILD_HONOR_BATTLE_MAX_ROUND; ++i)
			{
				item.guess_num[i] = figther_item.guess_num[i];
			}

			protocol.fighter_num += 1;
		}
	}

	unsigned int send_len = sizeof(protocol) - (ARRAY_LENGTH(protocol.figther_info) - protocol.fighter_num) * sizeof(protocol.figther_info[0]);

	if (role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, send_len);
	}

	if (role_id)
	{
		InternalComm::Instance().SendToGameUserThroughCross(role_id, (const char *)&protocol, send_len);
	}
}

void GuildHonorBattleManager::SendGuildHonorTopInfo(Role * role)
{
	Protocol::SCGuildHonorBattleTopInfo protocol;
	for (int i = 0; i < ARRAY_LENGTH(protocol.guild_id); ++i)
	{
		protocol.guild_id[i] = 0;
	}

	CommonSaveGuildHonorRecordData * record_data = this->GetGuildHonorRecordData();
	if (record_data != NULL)
	{	
		int count = 0;
		for (int i = 0; i < ARRAY_LENGTH(record_data->record_item); ++i)
		{
			int index = record_data->record_item[i].finish_rank - 1;

			if (index < 0 || index >= protocol.ITEM_NUM)
			{
				continue;
			}

			protocol.guild_id[index] = record_data->record_item[i].guild_id;
			memcpy(protocol.guild_name[index], record_data->record_item[i].guild_name, sizeof(protocol.guild_name[0]));

			if (++count >= protocol.ITEM_NUM)
			{
				break;
			}
		}
	}

	if (role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}
	else
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&protocol, sizeof(protocol));
	}
}

void GuildHonorBattleManager::SendGuessInfo(int role_id)
{
	if (role_id == 0)
	{
		return;
	}

	Protocol::SCGuildHonorBattleGuessInfo protocol;
	protocol.champion_guild_id = 0;
	protocol.guess_item_count = 0;
	for (std::map<int, GuildHonorBattleMatchInfo>::iterator match_info_iter = m_match_infos.begin(); match_info_iter != m_match_infos.end(); ++match_info_iter)
	{
		const GuildHonorBattleMatchInfo & curr = match_info_iter->second;
		std::map<int, GuildHonorBattleGuessInfo>::const_iterator iter = curr.role_guess_info.find(role_id);
		if (iter != curr.role_guess_info.end())
		{
			if (curr.IsFirstRankMatch())
			{
				protocol.champion_guild_id = iter->second.champion_guild_id;
			}

			for (std::map<GuildHonorBattleGuessInfoKey, std::set<GuildHonorBattleGuessItem> >::const_iterator iter2 = iter->second.round_guess.begin(); iter2 != iter->second.round_guess.end(); ++iter2)
			{
				for (std::set<GuildHonorBattleGuessItem> ::const_iterator iter3 = iter2->second.begin(); iter3 != iter2->second.end(); ++iter3)
				{
					if (protocol.guess_item_count >= ARRAY_LENGTH(protocol.guess_item))
					{
						break;
					}

					SCGuildHonorBattleGuessInfo::Item & item = protocol.guess_item[protocol.guess_item_count];
					item.win_rank = iter2->first.win_rank;
					item.round = iter2->first.round;
					item.guild_id = iter3->guild_id;
					protocol.guess_item_count += 1;
				}
			}
		}
	}

	int send_len = sizeof(protocol) - (ARRAY_LENGTH(protocol.guess_item) - protocol.guess_item_count) * sizeof(protocol.guess_item[0]);
	assert(send_len < 1000);
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, send_len);
	}
	else if (role_id)
	{
		InternalComm::Instance().SendToGameUserThroughCross(role_id, (const char *)&protocol, send_len);
	}
}

void GuildHonorBattleManager::SendAllMatchInfo(bool is_send_all, Role * role, int role_id)
{
	static Protocol::SCGuildHonorBattleMatchInfo protocol;
	protocol.match_info_count = 0;

	for (std::map<int, GuildHonorBattleMatchInfo>::iterator iter = m_match_infos.begin(); iter != m_match_infos.end() && protocol.match_info_count < ARRAY_LENGTH(protocol.match_info); ++iter)
	{
		GuildHonorBattleMatchInfoItem & msg_match_info = protocol.match_info[protocol.match_info_count++];
		GuildHonorBattleMatchInfo & match_info = iter->second;

		msg_match_info.start_first_round = match_info.start_first_round;
		msg_match_info.win_rank = match_info.win_rank;
		msg_match_info.curr_round = match_info.curr_match_round;				//当前回合	
		msg_match_info.round_state = match_info.round_state;				//0是回合准备状态，1是开始
		msg_match_info.next_round_state_time = (unsigned int)match_info.next_round_state_time;		//回合结束时间
		msg_match_info.is_finish = match_info.is_match_finish;				//是否完成
		msg_match_info.fighter_num = 0;

		for (std::map<int, GuildHonorBattleFigtherInfo> ::iterator iter = match_info.figther_info.begin(); iter != match_info.figther_info.end(); ++iter)
		{
			const GuildHonorBattleFigtherInfo & figther_item = iter->second;
			GuildHonorBattleMatchInfoItem::FigtherInfoItem & item = msg_match_info.figther_info[msg_match_info.fighter_num];
			if (figther_item.guild_id == 0)
			{
				continue;
			}

			item.number = figther_item.number;
			item.server_id = figther_item.server_id;
			item.guild_id = figther_item.guild_id;
			memcpy(item.guild_name, figther_item.guild_name, sizeof(item.guild_name));
			memcpy(item.banner, figther_item.banner, sizeof(item.banner));
			memcpy(item.role_name, figther_item.patriarch_name, sizeof(item.role_name));

			const GuildHonorBattleRoundBattleInfo * round_battle_info = match_info.GetGuildHonorBattleRoundBattleInfo(figther_item.guild_id);
			item.guild_fight_rank = figther_item.guild_fight_rank;				//家族大乱斗排名
			item.last_match_rank = figther_item.last_match_rank;				//
			item.match_round = figther_item.match_round;		//到达的回合，和curr_round 不同
			item.is_finish = round_battle_info ? round_battle_info->is_finish_battle : true;			//match_round 回合是否完成战斗
			item.is_lose = round_battle_info ? round_battle_info->is_lose : true;			//match_round 回合是不是输了

			item.champion_guess_num = figther_item.champion_guess_num;
			for (int i = 0; i < GUILD_HONOR_BATTLE_MAX_ROUND; ++i)
			{
				item.guess_num[i] = figther_item.guess_num[i];
			}

			for (int i = 0; i < GUILD_HONOR_BATTLE_MAX_ZONE; ++i)
			{
				item.battle_id[i] = round_battle_info ? round_battle_info->scene_info.battle_id[i] : 0;
			}

			for (int z = 0; z < GUILD_HONOR_BATTLE_MAX_ZONE; ++z)
			{
				item.battle_result[z] = round_battle_info ? round_battle_info->scene_info.zone_result[z] : 0;
			}

			msg_match_info.fighter_num += 1;
		}
	}

	if (is_send_all)
	{
		World::GetInstWorld()->SendToALLGameAllGateway((const char *)&protocol, sizeof(protocol));
	}
	else
	{
		if (role)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
		}

		if (role_id)
		{
			InternalComm::Instance().SendToGameUserThroughCross(role_id, (const char *)&protocol, sizeof(protocol));
		}
	}
}

struct GuildHonorBattleGmTemp
{
	GuildHonorBattleGmTemp(std::string s = "")
	{
		guild_name = s;
	}

	bool operator<(const GuildHonorBattleGmTemp & o)const
	{
		return guild_name < o.guild_name;
	}

	std::string guild_name;
	int score;
};

void GuildHonorBattleManager::GmOperate(Role * role, int type, int param1)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_GUILD_HONOR_BATTLE);

	//ActivityGuildHonorBattle * activity = (ActivityGuildHonorBattle *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_HONOR_BATTLE);
	//if (activity != NULL)
	//{

	//	activity->GmOperate(type);
	//}

	if (type == 1)
	{
		if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_GUILD_HONOR_BATTLE))
		{
			return;
		}

		for (std::map<int, GuildHonorBattleMatchInfo>::iterator iter = m_match_infos.begin(); iter != m_match_infos.end(); ++iter)
		{
			GuildHonorBattleMatchInfo & match_info = iter->second;
			match_info.next_round_state_time = EngineAdapter::Instance().Time();
		}		
	}

	if (type == 2)
	{
		this->OnWeekChange();
		type = 3; //打印信息
	}

	if (type == 3)   //打印信息
	{
		CommonSaveGuildHonorData * honor_data = this->GetGuildHonorData();
		if (!honor_data)
		{
			return;
		}

		time_t settlement_list_time = honor_data->settlement_list_time;
		tm * settlement_list_time_tm_ptr = ::localtime(&settlement_list_time);
		if (!settlement_list_time_tm_ptr)
		{
			return;
		}

		const tm & settlement_list_time_tm = *settlement_list_time_tm_ptr;
		std::string s = STRING_SPRINTF("process:%d settlement_list_time(%d-%d-%d %d:%d:%d  %u)", honor_data->guild_honor_process,
					       settlement_list_time_tm.tm_year + 1900, settlement_list_time_tm.tm_mon + 1, settlement_list_time_tm.tm_mday,
					       settlement_list_time_tm.tm_hour, settlement_list_time_tm.tm_min, settlement_list_time_tm.tm_sec, (unsigned int)settlement_list_time);

		World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), s.c_str(), (int)s.size(), GetMsgType(SYS_MSG_SYSTEM_CHAT));		
	}

	if (type == 10)
	{
		time_t now_second = EngineAdapter::Instance().Time();

		CommonSaveGuildHonorData * honor_data = this->GetGuildHonorData();
		if (!honor_data)
		{
			return;
		}

		int process = param1 % GUILD_HONOR_BATTLE_PROCESS_MAX;

		if (now_second > honor_data->settlement_list_time && process < GUILD_HONOR_BATTLE_PROCESS_READY)
		{
			const tm * now_tm = EngineAdapter::Instance().LocalTime();
			
			time_t settlement_list_time = honor_data->settlement_list_time;
			tm * settlement_list_time_tm_ptr=::localtime(&settlement_list_time);
			if (!settlement_list_time_tm_ptr)
			{
				return;
			}

			const tm & settlement_list_time_tm = *settlement_list_time_tm_ptr;

			
			std::string s = STRING_SPRINTF("now(%d-%d-%d %d:%d:%d,%u) > settlement_list_time(%d-%d-%d %d:%d:%d,%u)",
						       now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday, now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec, (unsigned int)now_second,
						       settlement_list_time_tm.tm_year + 1900, settlement_list_time_tm.tm_mon + 1, settlement_list_time_tm.tm_mday, settlement_list_time_tm.tm_hour, settlement_list_time_tm.tm_min, settlement_list_time_tm.tm_sec, (unsigned int)settlement_list_time
						       );
			World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), s.c_str(), (int)s.size(), GetMsgType(SYS_MSG_SYSTEM_CHAT));

			return;
		}

		honor_data->guild_honor_process = process;
		this->SendProcessInfo();
	}

	if (type == 200)
	{
		std::set<GuildHonorBattleGmTemp> guild_cfg;
		for (int i = 0; i < param1; ++i)
		{
			GuildHonorBattleGmTemp t;
			t.guild_name = STRING_SPRINTF("g_auto%d", i + 1);
			t.score = 100 - i;
			guild_cfg.insert(t);
		}

		bool is_first = true;
		const GuildManager::GuildMap & guild_map = GuildManager::Instance().GetGuildMap();
		for (GuildManager::GuildMapConstIt iter = guild_map.begin(), end = guild_map.end(); iter != end; ++iter)
		{
			Guild * guild = iter->second;
			if (guild)
			{
				std::string guild_name = guild->GetGuildName();
				int guild_id = guild->GetGuildID();


				std::set<GuildHonorBattleGmTemp>::iterator iter = guild_cfg.find(guild_name);
				if (iter != guild_cfg.end())
				{
					if (is_first)
					{
						is_first = false;
						m_guild_fight_data->ClearAllData();
					}

					this->GmSetGuildFightResult(guild_id, iter->score);
				}
			}
		}
	}


	if (type == 300)
	{
		m_guild_fight_data->ClearAllData();
	}

	if (type == 400)
	{
		std::set<GuildHonorBattleGmTemp> guild_cfg;
		for (int i = 0; i < param1; ++i)
		{
			GuildHonorBattleGmTemp t;
			t.guild_name = STRING_SPRINTF("auto%d", i + 1);
			t.score = 100 - i;
			guild_cfg.insert(t);
		}

		bool is_first = true;
		const GuildManager::GuildMap & guild_map = GuildManager::Instance().GetGuildMap();
		for (GuildManager::GuildMapConstIt iter = guild_map.begin(), end = guild_map.end(); iter != end; ++iter)
		{
			Guild * guild = iter->second;
			if (guild)
			{
				std::string guild_name = guild->GetGuildName();
				int guild_id = guild->GetGuildID();


				std::set<GuildHonorBattleGmTemp>::iterator iter = guild_cfg.find(guild_name);
				if (iter != guild_cfg.end())
				{
					if (is_first)
					{
						is_first = false;
						m_guild_fight_data->ClearAllData();
					}

					this->GmSetGuildFightResult(guild_id, iter->score);
				}
			}
		}
	}
}

int GuildHonorBattleManager::GmSetGuildFightResult(int guild_id, int score, int wday)
{
	Guild * guild = GuildManager::Instance().GetGuild(guild_id);
	if (guild == NULL)
	{
		return -__LINE__;
	}

	CommonSaveGuildFightData * guild_fight_data = this->GetGuildFightData(guild_id);
	if (guild_fight_data == NULL)
	{
		return -__LINE__;
	}

	int combine_server_id = 0;
	{
		int server_id = IntToUid(guild_id).db_index;
		combine_server_id = server_id;
		UniqueServerID origin_usid(LocalConfig::Instance().GetPlatType(), server_id);
		const UniqueServerID * usid = World::GetInstWorld()->GetMotherServerId(origin_usid);
		if (usid)
		{
			combine_server_id = usid->server_id;			
		}
	}

	guild_fight_data->server_id = combine_server_id;
	guild_fight_data->guild_id = guild_id;
	guild->GetGuildName(guild_fight_data->guild_name);
	guild->GetMemberManagerRef().GetPatriarchName(guild_fight_data->patriarch_name);
	memcpy(guild_fight_data->banner, guild->GetBanner(), sizeof(guild_fight_data->banner));

	const ActivityGuildHonorBattleOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityGuildHonorBattleConfig()->GetOtherCfg();

	if (wday < 0)
	{
		for (int i = 0; i < ARRAY_LENGTH(guild_fight_data->records); ++i)
		{
			guild_fight_data->join_guild_fight_day |= 1 << i;
			guild_fight_data->records[i].score = other_cfg.IsVaildDay(i) ? score : 0;
			guild_fight_data->records[i].time = (unsigned int)EngineAdapter::Instance().Time();
		}
	}
	else
	{
		wday = wday % ARRAY_LENGTH(guild_fight_data->records);
		guild_fight_data->join_guild_fight_day |= 1 << wday;
		guild_fight_data->records[wday].score = score;
		guild_fight_data->records[wday].time = (unsigned int)EngineAdapter::Instance().Time();
	}

	CommonSaveGuildHonorData * honor_data = this->GetGuildHonorData();
	if (honor_data)
	{
		if (honor_data->guild_honor_process < GUILD_HONOR_BATTLE_PROCESS_GUILD_FIGHT)
		{
			honor_data->guild_honor_process = GUILD_HONOR_BATTLE_PROCESS_GUILD_FIGHT;
			this->SendProcessInfo();
		}
	}

	m_is_score_rank_need_update = true;

	return 0;
}

void GuildHonorBattleManager::GetGuildHonorRankkbuff(int guild_id, short(&ARG_OUT buff)[2])
{
	int rank = this->GetMatchRank(guild_id);

	std::vector<int> v = LOGIC_CONFIG->GetActivityGuildHonorBattleConfig()->GetGuildHonorRankkbuff(rank);
	for (int i = 0; i < ARRAY_LENGTH(buff) && i < (int)v.size(); ++i)
	{
		buff[i] = (short)v[i];
	}
}

GuildHonorBattleManager::GuildHonorBattleManager()
{
	m_guild_fight_data = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_GUILD_FIGHT_DATA);
	m_guild_honor_info_data = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_GUILD_HONOR_DATA);
	m_guild_honor_record_data = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_GUILD_HONOR_RECORD_DATA);
	assert(m_guild_fight_data != NULL); 
	assert(m_guild_honor_info_data != NULL);
	assert(m_guild_honor_record_data != NULL);

	m_is_score_rank_need_update = true;

	m_match_seq=100;
	m_all_match_finish = false;
	is_zero_fighter = false;
}

GuildHonorBattleManager::~GuildHonorBattleManager()
{
}

void GuildHonorBattleManager::ClearLastGuildHonorBattleRecord()
{
	std::vector<GuildHonorBattleFigtherInfoRecordItemTemp>  record;
	this->SetGuildHonorRecord(record, false);
}

bool GuildHonorBattleManager::IsGuildHonorBattleScene(SceneIndex scene_index)
{
	Scene * scene = World::GetInstWorld()->GetSceneManager()->GetSceneByIndex(scene_index);
	if (NULL != scene && Scene::SCENE_TYPE_GUILD_HONOR_BATTLE == scene->GetSceneType())
	{
		return true;
	}

	return false;
}

void GuildHonorBattleManager::ResetMatchs()
{
	m_all_match_finish = false;
	m_join_match_guild_id.clear();
	m_add_matchs.clear();
	m_match_infos.clear();
}

int GuildHonorBattleManager::GetMatchSeq()
{
	return m_match_seq++;
}

CommonSaveGuildHonorRecordData * GuildHonorBattleManager::GetGuildHonorRecordData()
{
	CommonSaveGuildHonorRecordData * save_data = m_guild_honor_record_data->GetTypeData<CommonSaveGuildHonorRecordData>(0);
	assert(save_data != NULL);

	if (save_data != NULL)
	{
		m_guild_honor_record_data->SetDataChange(0);
		return save_data;
	}

	return NULL;
}

CommonSaveGuildHonorData * GuildHonorBattleManager::GetGuildHonorData()
{
	CommonSaveGuildHonorData * save_data = m_guild_honor_info_data->GetTypeData<CommonSaveGuildHonorData>(0);
	assert(save_data != NULL);

	if (save_data != NULL)
	{
		m_guild_honor_info_data->SetDataChange(0);
		return save_data;
	}

	return NULL;
}

CommonSaveGuildFightData * GuildHonorBattleManager::GetGuildFightData(int guild_id, bool is_auto_create, bool set_data_change)
{
	CommonSaveGuildFightData * save_data = m_guild_fight_data->GetTypeData<CommonSaveGuildFightData>(guild_id, is_auto_create);
	if (save_data != NULL && set_data_change)
	{
		m_guild_fight_data->SetDataChange(guild_id);
	}

	return save_data;
}

bool GuildHonorBattleMatchInfo::SortFuncFinish(const GuildHonorBattleFigtherInfo & item1, const GuildHonorBattleFigtherInfo & item2)
{
	if (item1.match_round > item2.match_round)
		return true;
	if (item1.match_round < item2.match_round)
		return false;

	if (item1.guild_fight_rank < item2.guild_fight_rank)
		return true;
	if (item1.guild_fight_rank > item2.guild_fight_rank)
		return false;

	if (item1.guild_id < item2.guild_id)
		return true;
	if (item1.guild_id > item2.guild_id)
		return false;

	return false;
}

GuildHonorBattleRoundBattleInfo GuildHonorBattleMatchInfo::FillGuildHonorBattleRoundBattleInfo(const GuildHonorBattleFigtherInfo * figher_item, const GuildHonorBattleFigtherInfo * item2, bool is_ack)
{
	GuildHonorBattleRoundBattleInfo round_battle_info1;
	//if (item1 == NULL)
	//{
	//	return round_battle_info1;
	//}
	//
	//round_battle_info1.is_ack = is_ack;
	//round_battle_info1.guild_id = item1->guild_id;
	//memcpy(round_battle_info1.guild_name, figher_item.guild_name, sizeof(round_battle_info1.guild_name));
	//round_battle_info1.opponent_guilid = opponent_item->guild_id;
	//round_battle_info1.opponent_index = opponent_item->number - 1;
	//memcpy(round_battle_info1.opponent_guild_name, opponent_item->guild_name, sizeof(round_battle_info1.opponent_guild_name));
	//round_battle_info1.scene_info.scene_id = match_scene_id;
	//round_battle_info1.scene_info.scene_key = match_scene_key;
	//round_battle_info1.scene_info.birth_pos = other_cfg.ack_pos;
	//round_battle_info1.vs_str = vs_str;

	return round_battle_info1;
}

GuildHonorBattleMatchInfo::GuildHonorBattleMatchInfo()
{
	is_start = false;
	is_match_finish = false;			//比赛是否完成了

	start_first_round = 1;
	win_rank = 1;
	max_match_round = 0;
	curr_match_round = 0;
	round_state = GUILD_HONOR_BATTLE_STATE_INVALID;
	next_round_state_time = 0;
}

void GuildHonorBattleMatchInfo::Update(time_t now_second)
{
	this->UpdateMatchInfo(now_second);
}

int GuildHonorBattleMatchInfo::UpdateMatchInfo(time_t now_second)
{
	if (!is_start)
	{
		return -__LINE__;
	}

	if (is_match_finish)
	{
		return -__LINE__;
	}

	if (now_second >= next_round_state_time)
	{
		const ActivityGuildHonorBattleOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityGuildHonorBattleConfig()->GetOtherCfg();

		round_state = (round_state + 1) % GUILD_HONOR_BATTLE_STATE_MAX;
		if (round_state == GUILD_HONOR_BATTLE_STATE_READY)
		{
			curr_match_round += 1;

			if (curr_match_round == 1 && this->IsFirstRankMatch())
			{
				next_round_state_time = now_second + other_cfg.first_ready_duration;
			}
			else
			{
				next_round_state_time = now_second + other_cfg.ready_duration;
			}
			
			if (this->IsFirstRankMatch())
			{		// 传闻
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_guild_honor_battle_start_round_notice, (int)figther_info.size(), curr_match_round);
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgAllGame(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
				}
			}
	
			gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | OnRoundReady %s", __FUNCTION__, __LINE__, this->BaseInfoToString().c_str());
			this->OnRoundReady();
		}

		if (round_state == GUILD_HONOR_BATTLE_STATE_BATTLE)
		{			
			if (curr_match_round == 1 && this->IsFirstRankMatch())
			{
				next_round_state_time = now_second + other_cfg.first_battle_duration;
			}
			else
			{
				next_round_state_time = now_second + other_cfg.battle_duration;
			}

			gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | OnRoundFight %s", __FUNCTION__, __LINE__, this->BaseInfoToString().c_str());
			this->StartRoundFight();
		}

		if (round_state == GUILD_HONOR_BATTLE_STATE_FINISH)
		{
			gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | OnRoundFinish %s", __FUNCTION__, __LINE__, this->BaseInfoToString().c_str());
			this->OnRoundFinish();
		}

		GuildHonorBattleManager::NEED_SEND = true;
	}

	return 0;
}

void GuildHonorBattleMatchInfo::StartRoundFight()
{
	long long battle_length_of_time = next_round_state_time - EngineAdapter::Instance().Time() - GUILD_HONOR_BATTLE_END_EARLY_TIME;
	battle_length_of_time = battle_length_of_time > 0 ? battle_length_of_time : 0;

	std::set<int> skip_repeat;
	for (std::map<int, GuildHonorBattleRoundBattleInfo>::iterator iter = curr_round_figther.begin(); iter != curr_round_figther.end(); ++iter)
	{
		const GuildHonorBattleRoundBattleInfo & round_battle_info = iter->second;
		if (round_battle_info.is_bye)
		{
			continue;
		}

		int guild_id = iter->first;
		int opponent_guilid = round_battle_info.opponent_guilid;
		if (skip_repeat.find(guild_id) != skip_repeat.end())
		{
			continue;
		}

		skip_repeat.insert(guild_id);
		skip_repeat.insert(opponent_guilid);


		GuildHonorBattleRoundBattleInfo * opponent_info = this->GetGuildHonorBattleRoundBattleInfo(round_battle_info.opponent_guilid);
		if (opponent_info)
		{
			gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  %s  %s vs  %s", __FUNCTION__, __LINE__, this->BaseInfoToString().c_str(), round_battle_info.BaseInfoToString().c_str(), opponent_info->BaseInfoToString().c_str());
		}
		else
		{
			gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  %s [%d %s] vs [%d %s]", __FUNCTION__, __LINE__, this->BaseInfoToString().c_str(), round_battle_info.guild_id, round_battle_info.guild_name, round_battle_info.opponent_guilid, round_battle_info.opponent_guild_name);
		}

		SpecialLogicGuildHonorBattle * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicGuildHonorBattle(round_battle_info.scene_info.scene_id, round_battle_info.scene_info.scene_key);
		if (special_lobgic != NULL)
		{
			if (this->IsFirstRankMatch() && this->IsFinalRound())
			{
				// 传闻
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_guild_honor_battle_last_round_battle_notice, round_battle_info.guild_id, round_battle_info.opponent_guilid);
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgAllGame(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
				}

			}

			special_lobgic->StartBattle(battle_length_of_time);
		}

		assert(special_lobgic != NULL);
	}
}



void GuildHonorBattleMatchInfo::SetZoneBattleId(int guild_id1, int guild_id2, int zone_id, int battle_id)
{
	int zone_index = zone_id - 1;
	if (zone_index < 0 || zone_index >= GUILD_HONOR_BATTLE_MAX_ZONE)
	{
		return;
	}

	GuildHonorBattleRoundBattleInfo * round_battle_info1 = this->GetGuildHonorBattleRoundBattleInfo(guild_id1);
	if (round_battle_info1)
	{
		round_battle_info1->scene_info.battle_id[zone_index] = battle_id;
	}
	GuildHonorBattleRoundBattleInfo * round_battle_info2 = this->GetGuildHonorBattleRoundBattleInfo(guild_id2);
	if (round_battle_info2)
	{
		round_battle_info2->scene_info.battle_id[zone_index] = battle_id;
	}
}

void GuildHonorBattleMatchInfo::SetZoneResult(int guild_id1, int guild_id2, int zone_id, int win_guild_id, const char * reason)
{
	if (!reason)
	{
		reason = "";
	}

	int zone_index = zone_id - 1;
	if (zone_index < 0 || zone_index >= GUILD_HONOR_BATTLE_MAX_ZONE)
	{
		return;
	}

	GuildHonorBattleRoundBattleInfo * round_battle_info1 = this->GetGuildHonorBattleRoundBattleInfo(guild_id1);
	if (round_battle_info1)
	{
		round_battle_info1->scene_info.zone_result[zone_index] = win_guild_id;
	}
	GuildHonorBattleRoundBattleInfo * round_battle_info2 = this->GetGuildHonorBattleRoundBattleInfo(guild_id2);
	if (round_battle_info2)
	{
		round_battle_info2->scene_info.zone_result[zone_index] = win_guild_id;
	}

	gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |reason:%s  %s guild1:%d guild2:%d zone_id:%d win_guild:%d", __FUNCTION__, __LINE__,
						 reason, this->BaseInfoToString().c_str(), guild_id1, guild_id2, zone_id, win_guild_id);

}

void GuildHonorBattleMatchInfo::SetRoundResult(int guild_id, bool is_win)
{
	if (is_match_finish || guild_id == 0)
	{
		return;
	}

	GuildHonorBattleRoundBattleInfo * round_battle_info = this->GetGuildHonorBattleRoundBattleInfo(guild_id);
	if (!round_battle_info)
	{
		gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  %s guild:%d is_win:%d round_battle_info is null", __FUNCTION__, __LINE__, this->BaseInfoToString().c_str(), guild_id, (int)is_win);
		assert(0);		
		return;
	}

	GuildHonorBattleRoundBattleInfo * opponent_round_battle_info = this->GetGuildHonorBattleRoundBattleInfo(round_battle_info->opponent_guilid);
	if (!opponent_round_battle_info)
	{
		gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  %s guild:%d is_win:%d opponent round_battle_info is null", __FUNCTION__, __LINE__, this->BaseInfoToString().c_str(), round_battle_info->opponent_guilid, (int)!is_win);
		assert(0);
		return;
	}

	round_battle_info->is_finish_battle = true;
	round_battle_info->is_lose = !is_win;

	opponent_round_battle_info->is_finish_battle = true;
	opponent_round_battle_info->is_lose = is_win;


	gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d |  %s %s is_win:%d  %s opponent_is_win:%d", __FUNCTION__, __LINE__, this->BaseInfoToString().c_str(),
						 round_battle_info->BaseInfoToString().c_str(), (int)is_win, opponent_round_battle_info->BaseInfoToString().c_str(), (int)!is_win);

	this->CheckCanNextState();
}

GuildHonorBattleRoundBattleInfo * GuildHonorBattleMatchInfo::GetGuildHonorBattleRoundBattleInfo(int guild_id)
{
	std::map<int, GuildHonorBattleRoundBattleInfo>::iterator iter = curr_round_figther.find(guild_id);
	if (iter != curr_round_figther.end())
	{
		return &iter->second;
	}

	return NULL;
}

void GuildHonorBattleMatchInfo::Guessing(int guild_id, bool is_champion, int role_id, int role_guild_id)
{
	if (round_state != GUILD_HONOR_BATTLE_STATE_READY)
	{
		return;
	}

	if (is_champion && curr_match_round != 1)		//冠军竞猜，只能在首轮
	{
		return;
	}

	int curr_match_round_index = curr_match_round - 1;
	if (curr_match_round_index < 0 || curr_match_round_index >= GUILD_HONOR_BATTLE_MAX_ROUND)
	{
		return;
	}

	GuildHonorBattleRoundBattleInfo * round_battle_info = this->GetGuildHonorBattleRoundBattleInfo(guild_id);
	if (round_battle_info == NULL)
	{
		return;
	}

	GuildHonorBattleFigtherInfo * figher_item_ptr = this->GetGuildHonorBattleFigtherInfoById(round_battle_info->guild_id);
	if (figher_item_ptr == NULL)
	{
		return;
	}

	GuildHonorBattleFigtherInfo & figher_item = *figher_item_ptr;

	GuildHonorBattleGuessInfo & guess_info = role_guess_info[role_id];
	if (is_champion)
	{
		int old_guild_id = guess_info.champion_guild_id;
		if (old_guild_id == guild_id)
		{
			return;
		}

		guess_info.champion_guild_id = guild_id;
		figher_item.champion_guess_num += 1;

		GuildHonorBattleFigtherInfo * fighter_info_tmp = this->GetGuildHonorBattleFigtherInfoById(old_guild_id);
		if (fighter_info_tmp)
		{
			fighter_info_tmp->champion_guess_num -= 1;
		}
	}
	else
	{
		GuildHonorBattleFigtherInfo * opponent_fighter_info = this->GetGuildHonorBattleFigtherInfoById(round_battle_info->opponent_guilid);
		if (opponent_fighter_info == NULL)
		{
			return;
		}

		GuildHonorBattleGuessInfoKey key;
		key.round = curr_match_round;
		key.win_rank = win_rank;

		GuildHonorBattleGuessItem erase_item;
		erase_item.guild_id = opponent_fighter_info->guild_id;

		if (guess_info.round_guess[key].erase(erase_item) > 0)
		{
			opponent_fighter_info->guess_num[curr_match_round_index] -= 1;
		}

		GuildHonorBattleGuessItem item;
		item.guild_id = guild_id;
		std::pair<std::set<GuildHonorBattleGuessItem>::iterator, bool> r = guess_info.round_guess[key].insert(item);
		if (r.second)
		{
			figher_item.guess_num[curr_match_round_index] += 1;
		}
	}

	GuildHonorBattleManager::Instance().SendGuessInfo(role_id);
}

void GuildHonorBattleMatchInfo::CancelGuessing(int guild_id, bool is_champion, int role_id)
{
	if (round_state != GUILD_HONOR_BATTLE_STATE_READY)
	{
		return;
	}

	if (is_champion && curr_match_round != 1)		//冠军竞猜，只能在首轮
	{
		return;
	}

	GuildHonorBattleGuessInfo & guess_info = role_guess_info[role_id];
	if (is_champion)
	{
		GuildHonorBattleFigtherInfo * fighter_info_tmp = this->GetGuildHonorBattleFigtherInfoById(guess_info.champion_guild_id);
		if (fighter_info_tmp)
		{
			fighter_info_tmp->champion_guess_num -= 1;
		}

		guess_info.champion_guild_id = 0;
	}
	else
	{
		int curr_match_round_index = curr_match_round - 1;
		if (curr_match_round_index < 0 || curr_match_round_index >= GUILD_HONOR_BATTLE_MAX_ROUND)
		{
			return;
		}

		GuildHonorBattleGuessInfoKey key;
		key.round = curr_match_round;
		key.win_rank = win_rank;

		GuildHonorBattleGuessItem item;
		item.guild_id = guild_id;
		if (guess_info.round_guess[key].erase(item))
		{
			GuildHonorBattleFigtherInfo * fighter_info_tmp = this->GetGuildHonorBattleFigtherInfoById(guild_id);
			if (fighter_info_tmp)
			{
				fighter_info_tmp->guess_num[curr_match_round_index] -= 1;
			}
		}
	}

	GuildHonorBattleManager::Instance().SendGuessInfo(role_id);
}

bool GuildHonorBattleMatchInfo::IsFirstRankMatch() const
{
	return win_rank == 1;
}

bool GuildHonorBattleMatchInfo::IsFinalRound()const
{
	return curr_match_round >= max_match_round;
}

void GuildHonorBattleMatchInfo::SendMatchInfo(bool is_send_all, Role * role, int role_id)
{

}

void GuildHonorBattleMatchInfo::OnRoundReady()
{
	curr_round_figther.clear();

	const ActivityGuildHonorBattleOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityGuildHonorBattleConfig()->GetOtherCfg();

	for (std::map<int, GuildHonorBattleFigtherInfo> ::iterator iter = figther_info.begin(); iter != figther_info.end(); ++iter)
	{
		const GuildHonorBattleFigtherInfo & figher_item = iter->second;
		if (figher_item.match_round != curr_match_round)
		{
			continue;
		}

		if (curr_round_figther.find(figher_item.guild_id) != curr_round_figther.end())
		{
			continue;
		}

		const GuildHonorBattleFigtherInfo * opponent_item = this->GetOpponentInfo(figher_item.number);
		if (opponent_item == NULL)
		{			
			GuildHonorBattleRoundBattleInfo temp;
			temp.win_rank = win_rank;
			temp.guild_id = figher_item.guild_id;
			temp.number = figher_item.number;
			memcpy(temp.guild_name, figher_item.guild_name, sizeof(temp.guild_name));
			temp.is_finish_battle = true;
			temp.is_bye = true;
	
			curr_round_figther[figher_item.guild_id] = temp;

			gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | cal_fighter %s [%d %d %s] vs [ is_bye]", __FUNCTION__, __LINE__, this->BaseInfoToString().c_str(),
								 figher_item.number, figher_item.guild_id, figher_item.guild_name);

			this->SendByeReward(figher_item.guild_id);
			continue;
		}

		int guild_id1 = figher_item.guild_id;
		int guild_id2 = opponent_item->guild_id;
		int match_scene_id = other_cfg.scene_id;
		int match_scene_key = this->GetMatchSeq();

		std::string vs_str;
		vs_str = std::string(figher_item.guild_name) + " VS " + opponent_item->guild_name;

		GuildHonorBattleRoundBattleInfo round_battle_info1;
		round_battle_info1.win_rank = win_rank;
		round_battle_info1.is_ack = true;
		round_battle_info1.guild_id = guild_id1;
		round_battle_info1.number = figher_item.number;
		memcpy(round_battle_info1.guild_name, figher_item.guild_name, sizeof(round_battle_info1.guild_name));
		round_battle_info1.opponent_guilid = opponent_item->guild_id;
		round_battle_info1.opponent_number = opponent_item->number;
		memcpy(round_battle_info1.opponent_guild_name, opponent_item->guild_name, sizeof(round_battle_info1.opponent_guild_name));
		round_battle_info1.scene_info.scene_id = match_scene_id;
		round_battle_info1.scene_info.scene_key = match_scene_key;
		round_battle_info1.scene_info.birth_pos = other_cfg.GetAckPos();
		round_battle_info1.vs_str = vs_str;

		GuildHonorBattleRoundBattleInfo round_battle_info2;
		round_battle_info2.win_rank = win_rank;
		round_battle_info2.is_ack = false;
		round_battle_info2.guild_id = guild_id2;
		round_battle_info2.number = opponent_item->number;
		memcpy(round_battle_info2.guild_name, opponent_item->guild_name, sizeof(round_battle_info2.guild_name));
		round_battle_info2.opponent_guilid = figher_item.guild_id;
		round_battle_info2.opponent_number = figher_item.number;
		memcpy(round_battle_info2.opponent_guild_name, figher_item.guild_name, sizeof(round_battle_info2.opponent_guild_name));
		round_battle_info2.scene_info.scene_id = match_scene_id;
		round_battle_info2.scene_info.scene_key = match_scene_key;
		round_battle_info2.scene_info.birth_pos = other_cfg.GetDefPos();
		round_battle_info2.vs_str = vs_str;

		curr_round_figther[guild_id1] = round_battle_info1;
		curr_round_figther[guild_id2] = round_battle_info2;

		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_guild_honor_battle_broadcast, vs_str.c_str());
		if (length > 0)
		{
			static Protocol::SCSystemMsg sm;
			int sendlen = 0;

			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT)))
			{
				Guild * guild1 = GuildManager::Instance().GetGuild(guild_id1);
				if (guild1 != NULL)
				{
					guild1->GetMemberManagerRef().SendInfoToAllMemberFromHidden((const char *)&sm, sizeof(sm));
				}
				Guild * guild2 = GuildManager::Instance().GetGuild(guild_id2);
				if (guild2 != NULL)
				{
					guild2->GetMemberManagerRef().SendInfoToAllMemberFromHidden((const char *)&sm, sizeof(sm));
				}
			}
		}

		gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | cal_fighter %s [%d %d %s] vs [%d %d %s]", __FUNCTION__, __LINE__, this->BaseInfoToString().c_str(),
							 figher_item.number, figher_item.guild_id, figher_item.guild_name, opponent_item->number, opponent_item->guild_id, opponent_item->guild_name);

		SpecialLogicGuildHonorBattle * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicGuildHonorBattle(match_scene_id, match_scene_key);
		if (special_lobgic != NULL)
		{
			special_lobgic->SetGuildInfo(win_rank, curr_match_round, &figher_item, opponent_item);
		}

		assert(special_lobgic != NULL);
	}

	assert(curr_round_figther.size() >= 2);
}

void GuildHonorBattleMatchInfo::OnRoundFinish(bool is_force)
{
	std::set<int> skip_repeat;
	for (std::map<int, GuildHonorBattleRoundBattleInfo>::iterator iter = curr_round_figther.begin(); iter != curr_round_figther.end(); ++iter)
	{
		const GuildHonorBattleRoundBattleInfo & round_battle_info = iter->second;
		int guild_id = iter->first;
		int opponent_guilid = round_battle_info.opponent_guilid;
		if (skip_repeat.find(guild_id) != skip_repeat.end())
		{
			continue;
		}

		skip_repeat.insert(guild_id);
		skip_repeat.insert(opponent_guilid);

		SpecialLogicGuildHonorBattle * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicGuildHonorBattle(round_battle_info.scene_info.scene_id, round_battle_info.scene_info.scene_key, false);
		if (special_lobgic != NULL)
		{
			special_lobgic->OnActivityGuildHonorBattleRoundFinish();
			GuildHonorBattleManager::Instance().AddDelScene(special_lobgic->GetScene()->GetIndex());
		}
	}

	if (is_force)
	{
		return;
	}

	int champion_guild_id = 0;
	std::string champion_guildname;
	for (std::map<int, GuildHonorBattleRoundBattleInfo>::iterator iter = curr_round_figther.begin(); iter != curr_round_figther.end(); ++iter)
	{
		const GuildHonorBattleRoundBattleInfo & round_battle_info = iter->second;

		gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | finish result %s %s", __FUNCTION__, __LINE__, this->BaseInfoToString().c_str(), round_battle_info.ToString().c_str());

		if (round_battle_info.is_lose)
		{
			continue;
		}

		GuildHonorBattleFigtherInfo * figher_item = this->GetGuildHonorBattleFigtherInfoById(round_battle_info.guild_id);
		assert(figher_item != NULL);
		if (figher_item)
		{
			figher_item->match_round += 1;
		}

		if (this->IsFirstRankMatch() && this->IsFinalRound())
		{
			assert(champion_guild_id == 0);
			champion_guild_id = round_battle_info.guild_id;
			champion_guildname = round_battle_info.guild_name;

			gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | finish champion result %s champion_guild_id:%d champion_guildname:%s", __FUNCTION__, __LINE__,
								 this->BaseInfoToString().c_str(), champion_guild_id, champion_guildname.c_str());
		}		
	}

	//发放竞猜奖励
	for (std::map<int, GuildHonorBattleGuessInfo>::iterator role_iter = role_guess_info.begin(); role_iter != role_guess_info.end(); ++role_iter)
	{
		GuildHonorBattleGuessInfoKey key;
		key.round = curr_match_round;
		key.win_rank = win_rank;

		int role_id = role_iter->first;
		int guess_champion_guild_id = role_iter->second.champion_guild_id;
		const std::set<GuildHonorBattleGuessItem> & guess_guild = role_iter->second.round_guess[key];

		for (std::set<GuildHonorBattleGuessItem>::const_iterator iter2 = guess_guild.begin(); iter2 != guess_guild.end(); ++iter2)
		{
			const GuildHonorBattleRoundBattleInfo * round_battle_info = this->GetGuildHonorBattleRoundBattleInfo(iter2->guild_id);
			if (!round_battle_info)
			{
				continue;
			}

			bool is_right = !round_battle_info->is_lose;
			const ActivityGuildHonorBattleGuessCfg * reward_cfg = LOGIC_CONFIG->GetActivityGuildHonorBattleConfig()->GetActivityGuildHonorBattleGuessCfg(GUILD_HONOR_BATTLE_GUESS_TYPE_NORMAL);
			if (reward_cfg == NULL)
			{
				continue;
			}

			gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | %s role_id:%d guess_guild_id:%d is_right:%d", __FUNCTION__, __LINE__,
								 this->BaseInfoToString().c_str(), role_id, iter2->guild_id, (int)is_right);

			std::string winner_guild_name = round_battle_info->is_lose ? round_battle_info->opponent_guild_name : round_battle_info->guild_name;
			const std::string & vs_str = round_battle_info->vs_str;

			// 发邮件
			static MailContentParam contentparam; contentparam.Reset();
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN] = is_right ? reward_cfg->right_gong_xian : reward_cfg->wrong_gong_xian;
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = is_right ? reward_cfg->right_coin : reward_cfg->wrong_coin;

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_honor_battle_guess_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_honor_battle_guess_content, (int)figther_info.size(), curr_match_round,
					      vs_str.c_str(), is_right, winner_guild_name.c_str());

			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(role_id, MAIL_REASON_DEFAULT, contentparam);
			}
		}

		if (this->IsFinalRound() && this->IsFirstRankMatch())
		{
			bool is_right = champion_guild_id == guess_champion_guild_id;
			const ActivityGuildHonorBattleGuessCfg * reward_cfg = LOGIC_CONFIG->GetActivityGuildHonorBattleConfig()->GetActivityGuildHonorBattleGuessCfg(GUILD_HONOR_BATTLE_GUESS_TYPE_CHAMPION);
			if (reward_cfg == NULL)
			{
				continue;
			}


			gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | %s role_id:%d guess_champion_guild_id:%d is_right:%d", __FUNCTION__, __LINE__,
								 this->BaseInfoToString().c_str(), role_id, guess_champion_guild_id, (int)is_right);

			// 发邮件
			static MailContentParam contentparam; contentparam.Reset();
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN] = is_right ? reward_cfg->right_gong_xian : reward_cfg->wrong_gong_xian;
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = is_right ? reward_cfg->right_coin : reward_cfg->wrong_coin;

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_honor_battle_guess_champion_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_honor_battle_guess_champion_content, is_right, champion_guildname.c_str());

			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(role_id, MAIL_REASON_DEFAULT, contentparam);
			}			
		}		
	}

	if (this->IsFinalRound())
	{
		this->OnMatchFinish();
	}
	 
	this->StartLoserGroupMatch();
}

void GuildHonorBattleMatchInfo::Check()
{


}

void GuildHonorBattleMatchInfo::OnMatchFinish()
{
	if (is_match_finish)
	{
		return;
	}

	is_match_finish = true;
	round_state = GUILD_HONOR_BATTLE_STATE_FINISH;
}

void GuildHonorBattleMatchInfo::CheckCanNextState()
{
	if (!this->IsFinalRound())
	{
		return;
	}

	for (std::map<int, GuildHonorBattleRoundBattleInfo>::iterator iter = curr_round_figther.begin(); iter != curr_round_figther.end(); ++iter)
	{
		const GuildHonorBattleRoundBattleInfo & round_battle_info = iter->second;
		if (round_battle_info.is_bye)
		{
			continue;
		}

		if (!round_battle_info.is_finish_battle)
		{
			return;
		}
	}

	if (round_state == GUILD_HONOR_BATTLE_STATE_BATTLE)
	{
		next_round_state_time = EngineAdapter::Instance().Time();
	}
}

int GuildHonorBattleMatchInfo::GetMatchSeq()
{
	return GuildHonorBattleManager::Instance().GetMatchSeq();
}

bool GuildHonorBattleMatchInfo::GetSceneKey(int role_id, int & scene_id)
{
	if (is_match_finish)
	{
		return false;
	}

	for (std::map<int, GuildHonorBattleRoundBattleInfo>::iterator iter = curr_round_figther.begin(); iter != curr_round_figther.end(); ++iter)
	{
		const GuildHonorBattleRoundBattleInfo & round_battle_info = iter->second;
		if (round_battle_info.is_finish_battle)
		{
			continue;
		}

		SpecialLogicGuildHonorBattle * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicGuildHonorBattle(round_battle_info.scene_info.scene_id, round_battle_info.scene_info.scene_key, false);
		if (special_lobgic == NULL)
		{
			continue;
		}

		if (special_lobgic->HasRole(role_id))
		{
			scene_id = round_battle_info.scene_info.scene_key;
			return true;
		}
	}

	return false;
}

void GuildHonorBattleMatchInfo::StartLoserGroupMatch()
{
	int win_num = 0;
	int lose_num = 0;
	std::vector<GuildHonorBattleFigtherInfo> loser_group_join_list;
	for (std::map<int, GuildHonorBattleRoundBattleInfo>::iterator iter = curr_round_figther.begin(); iter != curr_round_figther.end(); ++iter)
	{
		const GuildHonorBattleRoundBattleInfo & round_battle_info = iter->second;
		
		if (!round_battle_info.is_finish_battle)
		{
			continue;
		}

		GuildHonorBattleFigtherInfo * fighter_info_ptr = this->GetGuildHonorBattleFigtherInfoById(round_battle_info.guild_id);
		if (!fighter_info_ptr)
		{
			continue;
		}

		if (round_battle_info.is_lose)
		{
			lose_num += 1;
			loser_group_join_list.push_back(*fighter_info_ptr);
		}
		else
		{
			win_num += 1;
		}	
	}

	if (win_num + lose_num != (int)curr_round_figther.size())
	{
		assert(false);
		gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s  line:%d | %s win_num%d lose_num:%d", __FUNCTION__, __LINE__,
							 this->BaseInfoToString().c_str(), win_num, lose_num);
	}

	int new_match_win_rank = win_rank + win_num;
	GuildHonorBattleManager::Instance().StartLoserGroupMatch((int)pow(2, max_match_round - curr_match_round), new_match_win_rank, loser_group_join_list);
}

std::string GuildHonorBattleMatchInfo::BaseInfoToString() const
{
	std::string ret = STRING_SPRINTF("GuildHonorBattleMatchInfo[start_first_round:%d win_rank:%d curr_match_round:%d round_state:%d next_round_state_time:%ud max_match_round:%d] ",
					 start_first_round, win_rank, curr_match_round, round_state, (unsigned int)next_round_state_time, max_match_round);

	return  ret;
}


GuildHonorBattleFigtherInfo * GuildHonorBattleMatchInfo::GetGuildHonorBattleFigtherInfoById(int guild_id)
{
	for (std::map<int, GuildHonorBattleFigtherInfo> ::iterator iter = figther_info.begin(); iter != figther_info.end(); ++iter)
	{
		GuildHonorBattleFigtherInfo & figher_item = iter->second;
		if (figher_item.guild_id == guild_id)
		{
			return &figher_item;
		}
	}

	return NULL;
}

GuildHonorBattleFigtherInfo * GuildHonorBattleMatchInfo::GetOpponentInfo(int fighter_number)
{
	GuildHonorBattleFigtherInfo * ptr=MapValuesPtr(figther_info, fighter_number);
	if (ptr)
	{
		const GuildHonorBattleFigtherInfo & info = *ptr;
		for (std::map<int, GuildHonorBattleFigtherInfo> ::iterator iter = figther_info.begin(); iter != figther_info.end(); ++iter)
		{
			int opponent_number = iter->first;
			GuildHonorBattleFigtherInfo & opponent_info = iter->second;
			if (fighter_number == opponent_number || opponent_info.guild_id == info.guild_id || opponent_info.match_round != info.match_round)
			{
				continue;
			}

			unsigned int flag = ~((unsigned int)0) << info.match_round;
			if (((fighter_number - 1) & flag) == ((opponent_number - 1) & flag))
			{
				return &opponent_info;
			}
		}
	}

	return NULL;
}

void GuildHonorBattleMatchInfo::SendByeReward(int bye_guild_id)
{
	Guild * guild = GuildManager::Instance().GetGuild(bye_guild_id);
	if (guild == NULL)
	{
		return;
	}

	const ActivityGuildHonorBattleRewardCfg * reward_cfg = LOGIC_CONFIG->GetActivityGuildHonorBattleConfig()->GetActivityReward(ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_WIN);
	if (reward_cfg == NULL)
	{
		return;
	}

	guild->AddExp(reward_cfg->guild_coin);

	// 发邮件
	static MailContentParam contentparam; contentparam.Reset();

	contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = reward_cfg->coin;
	contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN] = reward_cfg->gong_xian;

	const std::vector<ItemConfigData> & rewards = reward_cfg->items;
	for (int k = 0; k < (int)rewards.size() && k < MAX_ATTACHMENT_ITEM_NUM; ++k)
	{
		const ItemConfigData & reward = rewards[k];

		const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
		if (NULL == item_base) continue;

		ItemDataWrapper & mail_item = contentparam.item_list[k];
		mail_item.item_id = reward.item_id;
		mail_item.num = reward.num;
		mail_item.is_bind = reward.is_bind ? 1 : 0;
		mail_item.invalid_time = item_base->CalInvalidTime();
	}

	int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_honor_bye_reward_subject);
	int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_honor_bye_reward_content);
	if (lensub > 0 && length > 0) guild->GetMemberManager()->SendMailToAllMember(MAIL_REASON_DEFAULT, contentparam);
}

GuildHonorBattleFigtherInfo::GuildHonorBattleFigtherInfo()
{
	win_rank = 1;
}

GuildHonorBattleFigtherInfo::GuildHonorBattleFigtherInfo(int arg_win_rank, int num, int arg_guild_fight_rank, int arg_last_match_rank, int arg_wday_flag, const CommonSaveGuildFightData & data)
{
	win_rank = arg_win_rank;
	number = num;
	match_round = 1;
	guild_fight_rank = arg_guild_fight_rank;
	last_match_rank = arg_last_match_rank;

	guild_id = data.guild_id;
	server_id = data.server_id;
	memcpy(guild_name, data.guild_name, sizeof(guild_name));
	memcpy(banner, data.banner, sizeof(banner));
	memcpy(patriarch_name, data.patriarch_name, sizeof(patriarch_name));
	
	champion_guess_num = 0;
	for (int i = 0; i < ARRAY_LENGTH(guess_num); ++i)
	{
		guess_num[i] = 0;
	}
	
	wday_flag = arg_wday_flag;
	for (int i = 0; i < ARRAY_LENGTH(score_records) && i < ARRAY_LENGTH(data.records); ++i)
	{
		score_records[i] = data.records[i].score;
	}
}

void GuildHonorBattleFigtherInfo::ResetMatchInfo(int win_rank_arg, int number_arg)
{
	win_rank = win_rank_arg;
	number = number_arg;
	match_round = 1;
	champion_guess_num = 0;

	for (int i = 0; i <= ARRAY_LENGTH(guess_num); ++i)
	{
		guess_num[i] = 0;
	}
}

std::string GuildHonorBattleFigtherInfo::ToString() const
{
	std::string ret;

	ret = STRING_SPRINTF("GuildHonorBattleFigtherInfo [win_rank:%d number:%d guild_id:%d guild_name:%s  match_round:%d ] ",
			     win_rank, number, guild_id, guild_name, match_round);

	return  ret;
}

GuildHonorBattleGuessInfo::GuildHonorBattleGuessInfo()
{
	champion_guild_id = 0;
}

GuildHonorBattleGuessItem::GuildHonorBattleGuessItem()
{
	guild_id = 0;
}

bool GuildHonorBattleGuessItem::operator<(const GuildHonorBattleGuessItem & other) const
{
	if (guild_id < other.guild_id)
		return true;
	if (guild_id > other.guild_id)
		return false;

	return false;
}

std::string GuildHonorBattleRoundBattleInfo::ToString() const
{
	std::string ret;

	ret = STRING_SPRINTF("GuildHonorBattleRoundBattleInfo win_rank:%d [guild_id:%d guild_name:%s opponent_guild_id:%d opponent_guild_name:%s  is_finish_round_battle:%d is_lose%d is_ack:%d is_bye:%d  ",
			     win_rank, guild_id, guild_name, opponent_guilid, opponent_guild_name, (int)is_finish_battle, (int)is_lose, (int)is_ack, (int)is_bye);

	for (int i = 0; i < ARRAY_LENGTH(scene_info.zone_result); ++i)
	{
		ret = ret + STRING_SPRINTF("{zone_id:%d  win_guild:%d} ", i + 1, scene_info.zone_result[i]);
	}

	ret = ret + "] ";

	return  ret;
}

std::string GuildHonorBattleRoundBattleInfo::BaseInfoToString() const
{
	 std::string ret = STRING_SPRINTF("GuildHonorBattleRoundBattleInfo[win_rank:%d guild_id:%d guild_name:%s number:%d] ",
			     win_rank, guild_id, guild_name, number);

	return  ret;
}

bool GuildHonorBattleGuessInfoKey::operator<(const GuildHonorBattleGuessInfoKey & other) const
{
	if (win_rank < other.win_rank)
		return true;
	if (win_rank > other.win_rank)
		return false;

	if (round < other.round)
		return true;
	if (round > other.round)
		return false;

	return false;
}

bool GuildHonorBattleFigtherInfoRecordItemTemp::operator<(const GuildHonorBattleFigtherInfoRecordItemTemp & other) const
{
	if (rank < other.rank)
		return true;

	if (rank > other.rank)
		return false;

	return false;
}
