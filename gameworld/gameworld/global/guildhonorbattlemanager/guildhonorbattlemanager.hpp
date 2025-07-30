#ifndef __GUILD_HONOR_BATTLE_MANAGER_HPP__
#define __GUILD_HONOR_BATTLE_MANAGER_HPP__

#include "servercommon/commonsavedef.h"
#include "servercommon/guildhonorbattledef.hpp"

#include <vector>

class CommonSave;
class Role;

enum GUILD_HONOR_BATTLE_STATE
{
	GUILD_HONOR_BATTLE_STATE_INVALID = -1,
	GUILD_HONOR_BATTLE_STATE_READY,			//布置
	GUILD_HONOR_BATTLE_STATE_BATTLE,		//战斗
	GUILD_HONOR_BATTLE_STATE_FINISH,		//完成
	GUILD_HONOR_BATTLE_STATE_MAX,
};

struct GuildHonorBattleBattleSceneInfo
{
	GuildHonorBattleBattleSceneInfo()
	{
		scene_id = 0;
		scene_key = 0;
		birth_pos = Posi();
		for (int i = 0; i < GUILD_HONOR_BATTLE_MAX_ZONE; ++i)
		{
			zone_result[i] = 0;
			battle_id[i] = 0;
		}
	}

	int scene_id;
	int scene_key;
	Posi birth_pos;
	int zone_result[GUILD_HONOR_BATTLE_MAX_ZONE];
	int battle_id[GUILD_HONOR_BATTLE_MAX_ZONE];
};


struct GuildHonorBattleFigtherInfo
{
	GuildHonorBattleFigtherInfo();
	GuildHonorBattleFigtherInfo(int win_rank, int num, int guild_fight_rank, int last_match_rank, int wday_flag, const CommonSaveGuildFightData & data);
	void ResetMatchInfo(int win_rank, int number);
	std::string ToString() const;

	int win_rank;
	int number;
	int match_round;
	int guild_fight_rank;
	int last_match_rank;
	
	int guild_id;
	int server_id;
	GuildName guild_name;
	GuildBanner banner;
	GameName patriarch_name;	
	int champion_guess_num;
	int guess_num[GUILD_HONOR_BATTLE_MAX_ROUND];		//每轮支持的人数
	unsigned short wday_flag;			//暂时没用
	unsigned short score_records[7];			//一周每天家族大乱斗战绩	
};

enum GUILD_HONOR_BATTLE_GUESS_TYPE
{
	GUILD_HONOR_BATTLE_GUESS_TYPE_NORMAL,
	GUILD_HONOR_BATTLE_GUESS_TYPE_CHAMPION,
};

struct GuildHonorBattleGuessItem
{
	GuildHonorBattleGuessItem();
	bool operator<(const GuildHonorBattleGuessItem & other)const;

	int guild_id;
};

struct GuildHonorBattleGuessInfoKey
{
	bool operator<(const GuildHonorBattleGuessInfoKey & other) const;

	int win_rank;
	int round;
};

struct GuildHonorBattleGuessInfo
{
	GuildHonorBattleGuessInfo();

	int champion_guild_id;
	std::map<GuildHonorBattleGuessInfoKey, std::set<GuildHonorBattleGuessItem> > round_guess;
};

struct GuildHonorBattleRoundBattleInfo
{
	GuildHonorBattleRoundBattleInfo()
	{
		win_rank = 0;
		guild_id = 0;
		memset(guild_name, 0, sizeof(guild_name));
		number = 0;

		opponent_number = 0;
		opponent_guilid = 0;
		memset(opponent_guild_name, 0, sizeof(opponent_guild_name));
		is_finish_battle = false;
		is_lose = false;
		is_ack = false;
		is_bye = false;
	}

	std::string ToString() const;
	std::string BaseInfoToString() const;

	int win_rank;
	int guild_id;
	GuildName guild_name;
	int number;

	int opponent_number;
	int opponent_guilid;
	GuildName opponent_guild_name;
	std::string vs_str;
	bool is_finish_battle;
	bool is_lose;
	bool is_ack;		//防守还是攻击，入场出生位置和战斗函数用到
	bool is_bye;		//轮空，没有对手

	GuildHonorBattleBattleSceneInfo scene_info;
}; 

struct  GuildHonorBattleMatchInfo
{
	bool static SortFuncFinish(const GuildHonorBattleFigtherInfo & item1, const GuildHonorBattleFigtherInfo & item2);
	static GuildHonorBattleRoundBattleInfo FillGuildHonorBattleRoundBattleInfo(const GuildHonorBattleFigtherInfo * item1, const GuildHonorBattleFigtherInfo * item2, bool is_ack);

	GuildHonorBattleMatchInfo();

	void Update(time_t now_second);
	int UpdateMatchInfo(time_t now_second);
	void StartRoundFight();
	
	void SetZoneBattleId(int guild_id1, int guild_id2, int zone_id, int battle_id);
	void SetZoneResult(int guild_id1, int guild_id2, int zone_id, int win_guild_id, const char * reason);
	void SetRoundResult(int guild_id, bool is_win);
	GuildHonorBattleRoundBattleInfo * GetGuildHonorBattleRoundBattleInfo(int guild_id);
	void Guessing(int guild_id, bool is_champion, int role_id, int role_guild_id);
	void CancelGuessing(int guild_id, bool is_champion, int role_id);

	bool IsFirstRankMatch() const;
	bool IsFinalRound() const;
public:
	void SendMatchInfo(bool is_send_all, Role * role = NULL, int role_id = 0);

	void OnRoundReady();
	void OnRoundFinish(bool is_close = false);
	void Check();
	void CheckCanMatchFinish();
	void OnMatchFinish();
	void CheckCanNextState();		
	int GetMatchSeq();
	bool GetSceneKey(int role_id, int & scene_id);
	void StartLoserGroupMatch();

	std::string BaseInfoToString()const;
public:
	bool is_start;
	bool is_match_finish;			//比赛是否完成了

	int start_first_round;			//没什么用，客户端用，比赛记录需要
	int win_rank;
	int max_match_round;
	int curr_match_round;
	int round_state;
	time_t next_round_state_time;

	std::map<int, GuildHonorBattleFigtherInfo> figther_info;
	std::map<int, GuildHonorBattleRoundBattleInfo> curr_round_figther;   //OnRoundFinish 将赢的加+1
	std::map<int, GuildHonorBattleGuessInfo> role_guess_info;
private:
	GuildHonorBattleFigtherInfo * GetGuildHonorBattleFigtherInfoById(int guild_id);
	GuildHonorBattleFigtherInfo * GetOpponentInfo(int fighter_number);
	void SendByeReward(int bye_guild_id);//发送轮空奖励
};

struct GuildHonorBattleFigtherInfoRecordItemTemp
{
	bool operator<(const GuildHonorBattleFigtherInfoRecordItemTemp & other)const;

	int rank;
	int start_first_round;
	GuildHonorBattleFigtherInfo fighter_info;
};

class GuildHonorBattleManager
{
	friend struct GuildHonorBattleMatchInfo;
public:
	static GuildHonorBattleManager & Instance();
	static bool NEED_SEND;

	void OnWeekChange();
	void OnUserLogin(Role * role);
	void OnUserLoginOriginGame(int role_id);
	void OnConnectCrossServerSucc();
	void OnGameServerConnect(std::set<int> server_id);

	void Update(unsigned long interval, time_t now_second);
	int UpdataProcess(time_t now_second);
	void UpdateRankScore(bool is_force = false);
	void UpdateMatchInfos(time_t now_second);
	void CheckAllMatchFinish();

	bool IsJoinMatchGuild(int guild_id) const;  //是否参加的工会id
	void OnGuildFightHappen();
	int SetGuildFightResult(int guild_id, int score, unsigned int score_change_time);
	void SetGuildHonorRecord(std::vector<GuildHonorBattleFigtherInfoRecordItemTemp> & record, bool is_finish);
	void HiddenToGameSyncGuildHonorRecord(int server_id = 0, bool is_notice = true);
	void SetGuildHonorRecordFromHidden(const CommonSaveGuildHonorRecordData & record, bool is_notice);
	int GetMatchTitle(int guild_id);
	int GetMatchRank(int guild_id);

	bool GetSceneKey(int role_id, int & scene_id);

	
	void OnGuildHonorBattleBegin();
	void OnGuildHonorBattleClose();

	std::vector<int> GetRandNum(int num);
	GuildHonorBattleMatchInfo * GetGuildHonorBattleFirstMatchInfo();
	GuildHonorBattleMatchInfo * GetGuildHonorBattleMatchInfoPtr(int win_rank);
	GuildHonorBattleMatchInfo * GetGuildHonorBattleMatchInfoByGuildId(int guild_id);
	void SetZoneBattleId(int win_rank, int guild_id1, int guild_id2, int zone_id, int battle_id);
	void SetZoneResult(int win_rank, int guild_id1, int guild_id2, int zone_id, int win_guild_id, const char * reason);
	void SetRoundResult(int win_rank, int guild_id, bool is_win);
	void StartLoserGroupMatch(int slot_num,int win_rank, std::vector<GuildHonorBattleFigtherInfo>  fighter_list);
	void AddDelScene(const SceneIndex & scene_index);

	void GoToBattleScene(Role * role);
	void Guessing(int win_rank, int guild_id, bool is_champion, int role_guild_id, int role_id);
	void CancelGuessing(int win_rank, int guild_id, bool is_champion, int role_id);

	void SendProcessInfo(bool is_send_all = true, Role * role = NULL, int role_id = 0);
	void SendGuildScoreInfo(int guild_id, Role * role, int role_id = 0);
	void SendGuildScoreRankInfo(Role * role, int role_id = 0);
	void SendMatchRecordInfo(Role * role, int role_id = 0);
	void SendGuildHonorTopInfo(Role * role = NULL);  //游戏服也存有一份,没有role_id 参数
	void SendGuessInfo(int role_id);
	 void SendAllMatchInfo(bool is_send_all, Role * role = NULL, int role_id = 0);

	void GmOperate(Role * role, int type, int param1);
	int GmSetGuildFightResult(int guild_id, int score, int wday = -1);
public:
	void GetGuildHonorRankkbuff(int guild_id, short(&ARG_OUT buff)[2]);
private:
	GuildHonorBattleManager();
	~GuildHonorBattleManager();
private:
	void ClearLastGuildHonorBattleRecord();
	bool IsGuildHonorBattleScene(SceneIndex scene_index);
	void ResetMatchs();
	int GetMatchSeq();

	CommonSaveGuildHonorRecordData * GetGuildHonorRecordData();
	CommonSaveGuildHonorData * GetGuildHonorData();
	CommonSaveGuildFightData * GetGuildFightData(int guild_id, bool is_auto_create = true, bool set_data_change = true);

	CommonSave * m_guild_honor_record_data;
	CommonSave * m_guild_honor_info_data;
	CommonSave * m_guild_fight_data;	

	bool m_is_score_rank_need_update;
	std::vector<CommonSaveGuildFightData> m_score_rank;

	int m_match_seq;
	bool m_all_match_finish;
	std::set<int> m_join_match_guild_id;
	std::vector<GuildHonorBattleMatchInfo> m_add_matchs;			//update中遍历m_match_infos中添加比赛，下一帧遍历开始前才加入update
	std::map<int,GuildHonorBattleMatchInfo> m_match_infos;
	std::set<SceneIndex> m_need_del_scene_index;				// 待删除
	bool is_zero_fighter;
};

#endif

