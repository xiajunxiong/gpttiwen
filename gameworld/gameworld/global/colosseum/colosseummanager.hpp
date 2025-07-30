#ifndef __COLOSSEUM_MANAGER_HPP__
#define __COLOSSEUM_MANAGER_HPP__

#include "engineadapter.h"
#include <set>
#include <list>
#include <vector>
#include <deque>
#include "servercommon/colosseumdef.hpp"

enum CHALLENGE_FIELD_DATA_STATE
{
	CHALLENGE_FIELD_DATA_STATE_INVALID = 0,						// ��ʼ״̬
	CHALLENGE_FIELD_DATA_STATE_LOADING,							// ���ݼ����� 
	CHALLENGE_FIELD_DATA_STATE_LOAD_FINISH,						// ���ݼ������
	CHALLENGE_FIELD_DATA_STATE_MAX,
};

static const int SYS_MIN_LEVEL = 18;

class Role;
class Gladiator;

namespace Protocol
{
	class SCColosseumTargetInfo;
	class SCColosseumRoleInfo;
	class SCColosseumRankRewardInfo;
	class SCColosseumTopInfo;
	class SCColosseumRankInfo;
	class SCColosseumReportInfo;
}

class ColosseumManager
{
public:
	typedef std::map<UserID, int> RankPosMap;
	typedef std::map<UserID, int>::iterator RankPosMapIt;

	friend class RMIInitChallengeUserObjectImpl;
public:

	static ColosseumManager & Instance()
	{
		static ColosseumManager ret;
		return ret;
	}

	void OnUserLogin(Role* role);
	void OnUserLogout(Role* role);
	void OnLevelUp(Role* role, int to_level);
	void OnUserInfoChange(Role * role);

	void Update(unsigned long interval, time_t now_second);
	bool OnServerStart();
	void OnServerStop();
	void OnResetGlobalModuleData(unsigned int old_dayid, unsigned int now_dayid);

	inline bool IsLoadFinish() const { return CHALLENGE_FIELD_DATA_STATE_LOAD_FINISH == m_data_state; }

	void OnGetRankUidList(int count, std::vector<int> & rank_uid_list);

	int SyncUserData(Role* role, Gladiator** out_gladiator = NULL);

	void OnFightReq(Role* role, int target_index);
	void OnHiddenGameQueryTargetGladiatorBeforeFight(int role_id, int target_index, int is_sweep);
	void OnGameHiddenQueryTargetGladiatorBeforeFightResp(int role_id, int target_index, int target_uid, const RoleBattleData& target_battle_data);
	void OnRefreshTarget(int uid, int assign_target = -1);
	void SysReport(Role * role, int target_uid, bool is_win, BattleData_Business* business_data = NULL, BattleData_StatisticRole* role_statistic_data = NULL,
		BattleData_Statistic* statistic_data = NULL, int pet_stat_num = 0, BattleData_StatisticRole* pet_stat_list = NULL, bool is_sweep = false);
	void SysReportFromHidden(int role_id, int target_uid, bool is_win, const ColosseumFightStatistic& stat, bool is_sweep = false);
	void SyncNewRankFromGameAfterSysReportFromHidden(int role_id, int target_uid, bool is_win, bool is_in_pool, bool target_is_in_pool, int old_rank, int new_rank, bool is_sweep = false);

	void OnRoleGetAchievement(Role * role, int seq);
	void SendRoleAchievement(Role * role);
	//--------------------------
	void GameHiddenSendRoleInfo(int role_id);
	void GameHiddenSendRankRewardInfo(int role_id);
	//--------------------------
	void SendRoleInfo(Role* role);
	void SendTopInfo(Role* role);
	void SendTargetInfo(int uid);
	void SendReportInfo(Role* role);
	void SendRankInfo(Role* role, int rank_count);
	void SendRankTopSimpleInfo(Role * role);
	void FetchRankReward(Role * role, int award_seq);//��ȡ�������н������Ѿ����Ρ����ڸĳ����н���ÿ��ͨ���ʼ���һ��
	void SendRankRewardInfo(Role * role);
	void FetchRankEveryDayReward(Role *role);

	void GmColosseumHandler(int op_type);
	void GmColosseumHandler(Role * role, int op_type, int param1, int param2);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void GetDefenderBuffid(int max_num, ARG_OUT int* out_num, ARG_OUT int* out_buff_list);
	void InitDefenderBuffId(int buff_num, const int* buff_id_list);

	Gladiator* GetUserGladiator(UserID user_id, int* out_pos = NULL, bool* out_is_in_pool = NULL);
	int GetRoleRank(int uid);

	void OnRandActivityEnd();//��·���������

	void OnSweepReq(Role* role, int target_index);		//ɨ��
	void OnGameHiddenQueryTargetGladiatorBeforeSweepResp(int role_id, int target_index, int target_uid, int user_total_capability, int target_total_capability);

	void SendRoleInfoToHidden(int uid);
	void SendRankRewardInfoToHidden(int uid);
	void SendTargetInfoToHidden(int uid);
	void SendTopInfoToHidden(int uid);
	void SendRankInfoToHidden(int uid, int rank_count);
	void SendReportInfoToHidden(int uid);
	void SendRankTopSimpleInfoToHidden(int uid);
	void RoleRefreshTargetOnHidden(int uid);

	void OnRoleChangeFormation(Role* role);

	std::set<int> GetAllUserUidSet() const;
private:
	ColosseumManager();
	~ColosseumManager();
	typedef std::deque<Gladiator *> RankUserList;

	void ReleaseRankList();
	void ReleasePoolList();

	void ResizeRankList(int size);
	void ResizePoolList(int size);

	void SetRankUser(int rankpos, Gladiator *gladiator, bool is_init = false);
	void SetPoolUser(int poolpos, Gladiator *gladiator, bool is_init = false);

	void AddRankUser(Gladiator* gladiator);
	void AddPoolUser(Gladiator* gladiator);

	void SetRankListDirty(int rankpos);
	void SetPoolListDirty(int poolpos);
	void SetPosDirty(int pos, bool is_in_pool);

	void ClearRankListDirtyMark(int begin_pos, int end_pos);
	void ClearPoolListDirtyMark(int begin_pos, int end_pos);

	bool LoadUser(long long next_id_from);
	void LoadUserSucc();
	bool InitChallengeUser(const ChallengeUserListParam::ChallengeUserAttr &user_attr);

	void GetChangeRankList(ChallengeUserListParam *p, int pos_begin, int* out_pos_end);
	void GetChangePoolList(ChallengeUserListParam *p, int pos_begin, int* out_pos_end);
	void CheckSave(time_t now_second, bool force_save = false);
	void SaveRankList();
	void SavePoolList();

	bool IsLevelLimit(int level);
	int GetFreeRankPosSlot();

	void RefreshTarget(Gladiator* gladiator, int assign_target_index = 0, bool is_send_to_hidden = false);
	void IssueCoinPool();
	void AddPlayerInPool();		// ��ǰ��������������߽����� - �����˲���
	void SendFightResultNotice(Role * role, int old_rank, int new_rank, bool is_win, int add_score, bool is_sweep = false);
	void SendOpenStatusNotice(Role * role = NULL);
	void InitRobot();		// ��ʼ��ǰ 300 ��������
	void StopAllColosseumBattle();
	void OnSeasonSettle();		// ���������Ĳ��� 

	void AddRobotData(int add_num);
	void AddARobotData(int insert_idx = -1);
	void FillRobotData(Gladiator* gladiator, int rank, int robot_id);
	void SendInPoolRankList(int reason, int from_rank = 0, Role * role = NULL);
	void ClearCurPoolList();
	int GenerateValidRobotId();
	
	bool CheckGladiatorUnique();
	void UpdateRobotLevel();

	void AssembleProtocolTargetInfo(Gladiator* gladiator, ARG_OUT Protocol::SCColosseumTargetInfo* info);
	void AssembleProtocolRoleInfo(Gladiator* gladiator, ARG_OUT Protocol::SCColosseumRoleInfo* info);
	void AssembleProtocolRankRewardInfo(Gladiator* gladiator, ARG_OUT Protocol::SCColosseumRankRewardInfo* info);
	void AssembleProtocolTopInfo(ARG_OUT Protocol::SCColosseumTopInfo* info);
	void AssembleProtocolRankInfo(ARG_OUT Protocol::SCColosseumRankInfo* info, int rank_count);
	void AssembleProtocolReportInfo(Gladiator* gladiator, ARG_OUT Protocol::SCColosseumReportInfo* info);

	void CalculateFightStat(ARG_OUT ColosseumFightStatistic& stat, BattleData_StatisticRole* role_statistic_data,
		BattleData_Statistic* statistic_data, int pet_stat_num, BattleData_StatisticRole* pet_stat_list);

	int m_data_state;
	bool m_is_first;
	bool m_is_change;
	int m_last_save_list_count;
	time_t m_next_save_time;		// ��һ�ΰ�����¼�����ݿ��ʱ���
	time_t m_next_pool_time;		// ��һ�ΰ���Ҽ��뱦���ʱ���
	time_t m_next_switch_time;		// ��һ�ν���ͭ�ҳؽ�����ʱ���

	int m_rank_user_num;
	int m_rank_user_capacity;
	Gladiator** m_rank_list;	
	bool* m_dirty_mark;
	bool* m_old_state;

	int m_pool_user_num;
	int m_pool_user_capacity;
	Gladiator** m_pool_user_list;
	bool* m_pool_dirty_mark;
	bool* m_pool_old_state;

	RankPosMap m_user_rank_map;
	RankPosMap m_user_pool_map;

	int m_robot_id_generator;

	int m_defender_buff_id;
	int m_defender_buff_num;
	int m_defender_buff_id_list[COLOSSEUM_DEFENDER_BUFF_NUM];
};

#endif