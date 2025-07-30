#ifndef __CLOUD_ARENA_USER_HPP__
#define __CLOUD_ARENA_USER_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/cloudarenadef.hpp"
#include "cloudarenauserfightrecord.hpp"
#include "cloudarenauserachievement.hpp"
#include "cloudarena.hpp"
#include <vector>

namespace Protocol
{
	class CSCloudArenaSetFormation;
}

struct CloudArenaRobotAttrAdjustCfg;
class Role;
class CloudArenaUser
{
public:
	CloudArenaUser();
	~CloudArenaUser();

	void Update(unsigned long interval, unsigned int now_second, ARG_OUT bool* has_change);
	void OnDayChange(ARG_OUT bool* has_change);

	inline int GetUID() const { return m_uid; }
	inline int GetLevel() const { return !this->IsRobot() ? m_level : m_show_level; }
	inline int GetProfession() const { return m_profession; }
	inline int GetAvatar() const { return m_avatar; }
	inline const char* GetName() const { return m_name; }
	inline const Appearance& GetAppearance() const { return m_appearance; }
	inline int GetScore() const { return m_score; }
	inline int GetManualRefreshTimes() const { return m_other_param.manual_refresh_times; }
	inline bool IsLastTimeWin() const { return m_last_time_win; }
	inline bool IsLastTimeLose() const { return m_last_time_lose; }
	inline int GetCapability() const { return m_capability; }
	inline int GetRemainFightTimes() const { return m_remain_fighttimes; }
	inline const RoleBattleData& GetBattleData() const { return m_battle_data; }
	inline int GetTodayBuyFightTimesTimes() const { return m_today_buy_fighttimes_times; }
	inline unsigned int GetNextRecoverFightTimesTimestamp() const { return m_next_recover_fighttimes_timestamp; }
	inline unsigned int GetNextManualRefreshTargetTimestamp() const { return m_next_manual_refresh_timestamp; }
	void SetNextManualRefreshTargetTimestamp(unsigned int timestamp) { m_next_manual_refresh_timestamp = timestamp; }
	inline bool IsDirty() const { return m_is_dirty; }
	inline bool GetOldState() const { return m_old_state; }
	void SetIsDirty(bool is_dirty) { m_is_dirty = is_dirty; }
	void SetOldState(bool old_state) { m_old_state = old_state; }
	inline int GetLastSeasonRank() const { return m_last_season_rank; }
	inline unsigned int GetReachScoreTimestamp() const { return m_reach_score_timestamp; }
	inline int GetReachRankingFlag() const { return m_reach_ranking_flag; }
	inline int GetRankingRewardFlag() const { return m_ranking_reward_flag; }
	inline bool HasFetchRankingReward(int rank_id) const { return 0 != (m_ranking_reward_flag & (1 << rank_id)); }
	inline bool HasReachRanking(int rank_id) const { return 0 != (m_reach_ranking_flag & (1 << rank_id)); }
	void OnFetchRankingReward(int rank_id);
	inline int GetSeasonFinalRank() const { return m_season_final_rank; }
	void SetSeasonFinalRank(int season_final_rank) { m_season_final_rank = season_final_rank; }
	inline int GetDayRewardStatus() const { return m_day_reward_status; }
	inline const std::vector<int>& GetTodayChallengeList() const { return m_today_challenge_list; }
	inline bool HasTodayFight() const { return !m_today_challenge_list.empty(); }

	inline int GetSeasonHighestRank() const { return m_season_highest_rank; }
	inline int GetSeasonHighestDanId() const { return m_season_highest_dan_id; }
	inline int GetSeasonHighestScore() const { return m_other_param.season_highest_score; }
	inline int GetLastDayScore() const { return m_last_day_score; }

	bool IsRobot() const { return CloudArena::IsRobot(m_uid); }
	bool FillRobotData(int id);
	bool FillRobotPetData(PetBattleData& data, int robot_level, const CloudArenaRobotAttrAdjustCfg* adjust_cfg, bool is_pet_helper);
	bool FillRobotPartnerData(PartnerBattleData& partner_data, int robot_level, int partner_id, const CloudArenaRobotAttrAdjustCfg* adjust_cfg);
	bool FillRobotPetHelperData(PetHelperBattleData& pet_helper_data, int robot_level, const CloudArenaRobotAttrAdjustCfg* adjust_cfg);
	bool FillDataWithRole(Role* role);
	bool FillDataWithUserData(const CloudArenaUserListParam::CloudArenaUserData& data);

	int CalcCapability();
	int CalcRoleCapability(const BattleCharacterData& role_data);
	int CalcPartnerCapability(const BattleCharacterData& partner_data);
	int CalcPetCapability(const BattleCharacterData& pet_data);

	void SetOpponentList(const CloudArena::OpponentList& opponent_list);
	void GetOpponentList(int max_num, ARG_OUT int* opponent_num, ARG_OUT int* opponent_list) const;
	int GetOpponentUid(int idx) const;
	bool HasOpponent() const;
	void SetLastTimeFightResult(bool is_win);
	void ModifyScore(int score);
	void SetRoleBattleData(const RoleBattleData& battle_data);
	void GetUserData(ARG_OUT CloudArenaUserListParam::CloudArenaUserData* data) const;
	bool AddFightTimes(int times);
	bool UseFightTimes(int times);
	void OnBuyFightTimes();
	void OnStartNewSeason();
	void OnFetchAchievementReward(int achieve_type);
	void OnManualRefreshOpponent();
	void ResetManualRefreshTimes();

	void GetFightRecordUserInfo(ARG_OUT CloudArenaFightRecordUserInfo* info) const;
	void AddFightRecord(const CloudArenaFightRecord& record);
	void GetFightRecord(int offset, int count, ARG_OUT CloudArenaFightRecord* record_list, ARG_OUT int* record_num, int max_num) const;

	inline bool HasNoticeSeasonInitialScore() const { return 0 == m_season_initial_score; }
	void OnNoticeSeasonInitialScore();
	inline int GetSeasonInitialScore() const { return m_season_initial_score; }

	void OnRankUpdate(int cur_rank);
	void OnFightFinishUpdateAchievement(bool is_challenge_other, bool is_win, bool is_higher_dan, int total_damage, int round_num, int use_medicine_num, int realive_num);
	const CloudArenaAchievementItem* GetAchievementItem(int achieve_type) const;
	void OnFetchDayReward();
	void AddTodayChallengeList(int target_uid);
	bool CheckSeasonTimestamp();
	void TryUpdateHighestDanAndRank(bool is_sync_to_game = true);

	void UpdateLastDayScore();
	void CheckFormation(Role* role);

	bool OnRoleSetFormation(Protocol::CSSetPetHelperFormation* req, Role* role);
	void UpdateRoleBasicInfo(Role* role);

	bool FindTargetByUid(int target_uid, ARG_OUT int* target_seq = NULL);
private:
	void AdjustRobotAttr(BattleAttr& attr, int adjust_percent);
	void OnScoreModifiedCheckRankingFlag();
	int CalculateSeasonInitialScore(int last_season_score);
	void OnRoleBattleDataChanged();
	void SyncCommonDataToRole();
	

	int m_uid;
	int m_level;
	int m_show_level;
	int m_profession;
	int m_avatar;
	GameName m_name;
	Appearance m_appearance;
	RoleBattleData m_battle_data;
	int m_capability;
	int m_score;
	int m_remain_fighttimes;
	int m_today_buy_fighttimes_times;
	unsigned int m_next_recover_fighttimes_timestamp;
	bool m_last_time_win;
	bool m_last_time_lose;
	unsigned int m_next_manual_refresh_timestamp;
	int m_target_uid_list[CLOUD_ARENA_OPPONENT_NUM];

	bool m_is_dirty;
	bool m_old_state;	//!< old_state为false时，需要insert， old_state为true时需要update

	CloudArenaUserFightRecord m_fight_record;

	unsigned int m_reach_score_timestamp;
	int m_last_season_rank;
	int m_ranking_reward_flag;
	int m_reach_ranking_flag;
	int m_season_final_rank;
	int m_season_initial_score;
	int m_day_reward_status;

	unsigned int m_season_timestamp;	//!< 赛季时间戳（等同赛季号）
	int m_hidden_server_idx;			//!< 用于标识本赛季开始于哪个服务器
	int m_season_highest_rank;			//!< 赛季最高排名
	int m_season_highest_dan_id;		//!< 赛季最高段位id
	int m_last_day_score;				//!< 前一天的积分

	CloudArenaUserAchievement m_achievement;
	std::vector<int> m_today_challenge_list;	//!< 今天挑战过的玩家列表

	CloudArenaUserOtherParam m_other_param;
};

#endif