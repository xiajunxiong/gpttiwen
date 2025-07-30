#ifndef __WORLD_ARENA_HPP__
#define __WORLD_ARENA_HPP__

#include "servercommon/struct/global/worldarenaparam.hpp"
#include "worldarenasignup.hpp"
#include <vector>

class WorldArenaSignUp;
class WorldArenaUserMgr;
class WorldArenaSchedule;
class WorldArenaBattleMgr;
class WorldArenaGroupHash;
class WorldArenaUser;
struct RobotParam;
class Role;
namespace Protocol
{
	class SCWorldArenaUserScheduleInfo;
	class SCWorldArenaInfo;
	class SCWorldArenaUserBaseInfo;
	struct MsgWorldArenaShowUpRoleInfo;
}

class WorldArena
{
public:
	WorldArena();
	~WorldArena();

	void OnDayChange();

	void OnServerStart();
	void OnServerStop();

	static WorldArena& Instance();
	void Update(unsigned int now);
	bool IsLoadAllFinish();
	bool IsLoadBaseDataFinish();

	void OnUserLogin(const UserID& user_id);
	void OnUserLoginOrigin(const UserID& user_id);
	void OnUserLogout(const UserID& user_id);

	void OnRoleOperate(Role* role, int oper_type, int param1, int param2);
	void TransferOperateToHidden(Role* role, int oper_type, int param1, int param2);
	void TransferSignupOperateToHidden(Role* role);
	void OnUserOperateFromOrigin(const UserID& user_id, int oper_type, int param1, int param2);

	void OnUserSignUp(const UserID& user_id, const WorldArenaSignUpInfo& info, const RoleBattleData& rbd);
	bool IsInBattleStatus();
	bool IsInSignupStatus();
	bool IsInRoundBattleTime(unsigned int now, unsigned int round_battle_start_timestamp);
	bool IsInRoundBattleStandbyTime(unsigned int now, unsigned int round_battle_start_timestamp);
	bool IsInRoundBattleFightTime(unsigned int now, unsigned int round_battle_start_timestamp);
	
	bool IsRobot(long long uuid);
	const RoleBattleData* GetRobotBattleData(long long uuid);
	bool AssembleRobotParam(long long uuid, RobotParam& param);
	void SetFightResult(long long schedule_id, int result, int round_num, bool is_attacker_giveup = false, bool is_defender_giveup = false);

	void GmRestartSeason();
	void GmNextStatus();
	void GmStartBattle(int round_idx);
	short GetCurSeasonIndex() const;

	void OnAsyncSaveUserDataReturnSucc(const std::vector<long long>& delete_list);
	void OnAsyncSaveScheduleDataReturnSucc(const std::vector<long long>& delete_list);
	void OnLoadUserReturn(int ret, const WorldArenaUserListParamForInit& param);
	void OnLoadScheduleReturn(int ret, const WorldArenaScheduleListParamForInit& param);
	void OnLoadBaseDataReturn(int ret, const WorldArenaBaseParam& param);

	void CheckLoadAllFinish();
	bool TryReEnterBattleScene(long long uuid, int* out_scene_id, int* out_fb_key);
	unsigned int CalculateRoundBattleEndTimestamp(int round_index);
	int GetFightResult(long long schedule_id);
	void EnterScene(Role* role);
	void GiveWinBoxRewardAtOrigin(const UserID& user_id, int sn);
	void GiveSignupRewardAtOrigin(const UserID& user_id);
	void GiveDailyRewardAtOrigin(const UserID& user_id);
	
	void AssembleShowUpRoleInfo(long long uuid, Protocol::MsgWorldArenaShowUpRoleInfo* role_info);

	const WorldArenaUser* GetWorldArenaUser(long long uuid);

	void UpdateUserSignupInfo(Role* role);
	void UpdateUserSignupInfo(long long uuid, const WorldArenaSignUpInfo& info);
	int CalculateAverageLevelInGroup(long long group_id);
private:
	WorldArena& operator= (const WorldArena&);
	WorldArena(const WorldArena&);

	void OnLoadAllFinish();
	void EnterNextStatus();
	void EnterSignUpStatus();
	void EnterScheduleStatus();
	void EnterBattleStatus();
	void EnterCalculateStatus();
	void EnterRestStatus();
	void CalculateAllStatusTimestampNextMonth();
	void MakeSchedule(bool is_schedule_for_omit_users);
	long long MakeScheduleForGroup(const std::vector<const WorldArenaSignUpInfo*>& group_fighter_vec);

	std::vector<const WorldArenaSignUpInfo*> ExtractAndShuffleAllSignUpInfo(bool is_schedule_for_omit_users);
	long long GenerateRobotUUID();
	bool IsDirty();
	void SetDirty(bool is_dirty);

	void SaveChanged();
	void SaveAll();
	bool HasCreatedBattleScene();
	void CreateBattleScene(ARG_IN int* force_start_round_idx = NULL);
	int CalculateRoundIndex(unsigned int today_round_timestamp, unsigned int battle_status_timestamp);
	void SendWorldArenaInfo(const UserID& user_id);
	void SendWorldArenaUserSignupStatus(const UserID& user_id);
	void SendWorldArenaInfoToAll();
	void SendWorldArenaUserSignupStatusToAllUserOnSeasonStart();
	void SendUserScheduleInfo(const UserID& user_id);
	void SendUserBaseInfo(const UserID& user_id);
	void AssembleUserSignUpInfo(Role* role, ARG_OUT WorldArenaSignUpInfo* info);
	void AssembleUserScheduleMsg(const UserID& user_id, ARG_OUT Protocol::SCWorldArenaUserScheduleInfo* msg);
	void AssembleSeasonInfo(ARG_OUT Protocol::SCWorldArenaInfo* msg);
	void AssembleUserBaseInfoMsg(const UserID& user_id, ARG_OUT Protocol::SCWorldArenaUserBaseInfo* msg);

	int GetGroupIdx(long long group_id);
	int GetUserServerCapabilityRank(const UserID& user_id);
	void SaveBaseData();
	void SaveUserData(bool is_save_all);
	void SaveScheduleData(bool is_save_all);
	bool LoadBaseData();
	
	void TryEnterSceneInOrigin(Role* role);
	void FetchWinBoxReward(const UserID& user_id, int sn);
	void FetchSignupReward(const UserID& user_id);
	void FetchDailyReward(const UserID& user_id);
	
	int GetRankInGroup(long long uuid);
	void QueryGroupRankList(const UserID& user_id, int group_idx);
	void SendGroupRankList(const UserID& user_id, int group_idx, const std::vector<WorldArenaUser*>& users);
	
	void StandbyForFight(Role* role, bool is_ready);
	void CalculateSeasonRankReward();
	void CalcSeasonRankRewardForGroup(long long group_id);
	void SendSeasonRankRewardMailToUser(WorldArenaUser* user, int rank);
	void ModifyScheduleCheckResultTimestampOnForceStart(const std::vector<const WorldArenaScheduleInfo*>& schedule_list, unsigned int check_timestamp);
	void SendTodayGiveNotice(const UserID& user_id);
	void CheckTodayGiveup(const UserID& user_id);
	void CheckAndSendNoticeEnterScene(const UserID& user_id);
	void SyncUserInfo(const UserID& user_id);
	void GiveDayReward(const UserID& user_id);
	void LogStatus(const char* func);
	const char* GetStatusStr(int status);

	bool m_load_all_finished;
	int m_load_base_data_status;
	bool m_is_dirty;
	WorldArenaBaseParam m_base_param;

	WorldArenaSignUp* m_signup_module;
	WorldArenaSchedule* m_schedule_module;
	WorldArenaUserMgr* m_user_mgr;
	WorldArenaBattleMgr* m_battle_mgr;
	WorldArenaGroupHash* m_group_hash;

	bool m_has_battle_scene_created;
	unsigned int m_check_save_timestamp;
};

#endif