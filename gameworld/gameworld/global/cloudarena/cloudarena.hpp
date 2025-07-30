#ifndef __CLOUD_ARENA_HPP__
#define __CLOUD_ARENA_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/cloudarenadef.hpp"
#include "cloudarenadataloader.hpp"
#include "cloudarenarankmanager.hpp"
#include <queue>

namespace battlegameprotocol
{
	struct BattleGameFightResultAck;
}

namespace Protocol
{
	class SCCloudArenaFormationInfo;
	class CSCloudArenaSetFormation;
	class CSSetPetHelperFormation;
	class SCPetHelperFormationInfo;
}

class CloudArenaUser;
class Role;
struct RoleBattleData;
class CloudArena
{
public:
	struct Opponent
	{
		Opponent(int _uid, int _score) : uid(_uid), score(_score) {}
		int uid;
		int score;
	};

	typedef std::map<int, CloudArenaUser*> UserMap;
	typedef std::map<int, std::vector<CloudArenaUser*> > ScoreUserListHash;
	typedef std::vector<Opponent> OpponentList;
	friend class RMIInitCloudArenaUserDataBackObjectImpl;
	friend class RMIInitCloudArenaDataBackObjectImpl;
	friend class CloudArenaUser;

public:
	static CloudArena& Instance();

	void Init(const CloudArenaParam& param);
	bool InitUserData(const CloudArenaUserListParam::CloudArenaUserData& data);
	void OnUserLogin(const UserID& user_id);
	void OnUserLoginOriginSendUserInfo(int uid);

	void OnDayChange();

	void Update(unsigned long interval, unsigned int now_second);
	void UserUpdate(unsigned long interval, unsigned int now_second);
	bool OnServerStart();
	void OnServerStop();
	inline bool IsLoadFinish() const { return m_data_loader.IsAllLoadFinish(); }
	void OnLoadAllSucc();

	void OnRoleOperateReq(Role* role, int oper_type, int param1, int param2);
	bool OnRoleSetFormation(Protocol::CSSetPetHelperFormation* req, Role* role);
	void OnFightFinish(int attacker_uid, int defender_uid, bool is_win, bool is_draw, const RoleBattleData& attacker_battle_data, const BattleData_Statistic& stats);
	void OnFightStart(int attacker_uid);
	void OnGameRegisterCross(const UniqueServerID& mother_sid);
	int GetUserRank(int uid);

	void OnUserScoreChangedUpdateScoreHash(int old_score, CloudArenaUser* user);

	inline unsigned int GetSeasonTimestamp() const { return m_param.season_info.season_timestamp; }
	inline int GetHiddenServerIdx() const { return m_param.season_info.hidden_server_idx; }

	const CloudArenaUser* GetUser(int uid, bool include_newbie = true);
	void GmAddChallengeTimes(Role* role, int times);
	void GmAddScore(Role* role, int score);
	void GmClearRefreshCD(Role* role);
	void GmRandomSetFormation(Role* role);

	static bool IsRobot(int uid) { return IntToUid(uid).db_index == CLOUD_ARENA_ROBOT_SERVER_ID; }
	int GetSeasonWorldLevel();
	std::set<int> GetAllUserUidSet() const;
private:
	CloudArena();
	~CloudArena();
	NONCOPYABLE(CloudArena);

	void Release();
	CloudArenaUser* MutableUser(int uid, bool include_newbie = true);
	const CloudArenaUser* GetUser(Role* role, bool create_if_not_found = false);
	CloudArenaUser* CreateUserPtr(Role* role);
	CloudArenaUser* CreateUserPtr(const CloudArenaUserListParam::CloudArenaUserData& data);
	bool RefreshOpponent(int uid, bool is_manual_refresh = false);
	bool ManualRefreshOpponent(Role* role);
	const CloudArenaUser* FindOpponent(int base_score, int low_match_factor, int high_match_factor, 
		const OpponentList& cur_opponent_list, const std::vector<int>& exclude_list, bool ignore_robot);

	void StartNewSeason();
	unsigned int CalcMondayTimestamp();
	unsigned int CalcSeasonEndTimestamp(unsigned int season_start_timestamp);
	
	void AddRobotFromConfig();
	void AddARobotByID(int id);
	void AddUserToScoreHash(int score, CloudArenaUser* user);
	void RemoveUserFromScoreHash(int score, int uid);

	void SendUserSelfInfo(int uid, const CloudArenaUser* user = NULL, Role* role = NULL);
	void SendUserFakeSelfInfo(int uid, Role* role = NULL);
	void SendUserTargetInfo(int uid, const CloudArenaUser* user = NULL, Role* role = NULL);
	void SendUserNewFightRecordNotice(int uid, const CloudArenaUser* user = NULL);
	void SendUserFormationInfo(int uid, const CloudArenaUser* user = NULL, Role* role = NULL);
	void QueryTargetInfo(Role* role);
	void QueryFightRecord(Role* role, int offset, int count);
	void SendAchievementInfo(int uid, const CloudArenaUser* user = NULL, Role* role = NULL);

	bool LoadAllData();
	void StartFight(Role* role, int target_idx);
	void ClearAllRobot();
	void ModifyUserScore(CloudArenaUser* user, int modify_score);
	void SaveData(bool is_save_all, unsigned int now);
	bool SaveCloudArenaParam();
	bool SaveAllUserData();
	bool SaveUserData();
	bool CallRMISaveUserData(const CloudArenaUserListParam& param);
	void ClearUserDirtyFlag(const CloudArenaUserListParam& param);
	void SetUserDirty(CloudArenaUser* user);
	void ResetDefenseBuff();
	void AddArenaCoin(int uid, int add_num);
	void QueryRankList(Role* role , int rank_type);
	void QueryTotalRankList(Role* role);
	void QueryDanRankList(Role* role);
	void QueryServerRankList(Role* role);
	void FetchRankingReward(Role* role, int rank_id);
	void ReleaseSeasonReward();
	void SettleSeasonFinalRank();
	bool SendSeasonRewardMail(int uid, int rank, int score, int season_idx);
	void SendSeasonInitialScoreNotice(int uid, const CloudArenaUser* user = NULL, Role* role = NULL);
	void OnNoticeSeasonInitialScore(Role* role);
	void SendUserDanTypeChangeNotice(int uid, int old_dan_type, int new_dan_type, const CloudArenaUser* user = NULL);
	void CheckAllUserRank(unsigned long interval, unsigned int now_second);
	void FetchAchievementReward(Role* role, int achieve_type);
	void FetchDayReward(Role* role);
	void QueryFormation(Role* role);
	void CheckAllUserSeasonTimestamp();
	void CheckFormation(CloudArenaUser* user, Role* role);
	void AssembleFormationInfo(ARG_OUT Protocol::SCPetHelperFormationInfo* info, const RoleBattleData& battle_data);
	void AssembleFormationInfo(ARG_OUT Protocol::CSSetPetHelperFormation* info, const RoleBattleData& battle_data);
	void SyncSeasonInfoToGame(const UniqueServerID* mother_sid);
	void SyncRoleBattleData(Role* role);

	CloudArenaDataLoader m_data_loader;
	CloudArenaParam m_param;
	UserMap m_user_map;
	UserMap m_newbie_user_map;		//!< 新进入该系统，未打第一场，但已经存在匹配数据
	ScoreUserListHash m_score_user_list_hash;	
	CloudArenaRankManager m_rank_mgr;
	unsigned int m_next_save_time;
	std::queue<int> m_dirty_user_queue;	//!< 存的是dirty的uid
	bool m_has_gen_robot;
	unsigned int m_next_update_rank_time;
};

#endif