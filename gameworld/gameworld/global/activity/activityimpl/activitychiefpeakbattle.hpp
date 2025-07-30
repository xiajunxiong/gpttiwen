#ifndef __ACTIVITY_CHIEF_PEAK_BATTLE_HPP__
#define __ACTIVITY_CHIEF_PEAK_BATTLE_HPP__

#include "global/activity/activity.hpp"
#include <vector>
#include <map>
#include <set>

class Role;
struct UserCacheNode;

enum ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE
{
	ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_INVAILD = 0,
	ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE,
};

enum ACTIVITY_CHIEF_PEAK_BATTLE_ROUND_STATE
{
	PEAK_BATTLE_ROUND_STATE_INVALID = -1,
	PEAK_BATTLE_ROUND_STATE_READY = 0,
	PEAK_BATTLE_ROUND_STATE_START,
	PEAK_BATTLE_ROUND_STATE_MAX,
};

struct ActivityChiefPeakBattleBattleInfo
{
	ActivityChiefPeakBattleBattleInfo()
	{
		scene_id = 0;
		scene_key = 0;
	}

	int scene_id;
	int scene_key;
};

struct ActivityChiefPeakBattleFigtherInfo
{
	ActivityChiefPeakBattleFigtherInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		role_id = 0;
		number = 0;
		match_round = 1;
		state = ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_INVAILD;
		opponent_number = 0;
	}

	int role_id;
	int number;
	int match_round;
	int state;
	int opponent_number;
	ActivityChiefPeakBattleBattleInfo scene_info;
};

struct  ActivityChiefPeakBattleMatchInfo
{
	bool static SortFuncFinish(const ActivityChiefPeakBattleFigtherInfo & item1, const ActivityChiefPeakBattleFigtherInfo & item2);

	ActivityChiefPeakBattleMatchInfo();

	void UpdateMatchInfo(time_t now_second);
	void NoticeRoundInfo();
	void SendNoticeToRole(Role * role);
	void HiddenToGameSendNoticeToRoleById(int role_id);
	void CheckFinish();
	void OnActivityClose();

	void GotoReadyMatchScene(Role * role);
	void GotoMatchScene(int figther_index);
	void StartRoundFight();
	void SetFightResult(int role_id, bool is_win, bool is_check_finish = false);
	void SendBattleReward(int role_id,bool is_win);
	void SendBattleResultNotice(int role_id, int is_win);
	void SendMatchInfoUpdate();
	void OnMatchFinish();
	void OnRoleLeaveMatch(int role_id);
	void SendReward();
	void CalTopFigther();

	int curr_match_round;
	int round_state;
	time_t m_next_state_time;
	bool is_match_finish; 			//比赛是否完成了

	int figther_info_num;
	ActivityChiefPeakBattleFigtherInfo figther_info[ACTIVITY_CHIEF_PEAK_BATTLE_FIGTHER_NUM];

	int MAX_MATCH_ROUND;
private:
	ActivityChiefPeakBattleFigtherInfo * GetOpponentInfo(int figther_index);
	int GetFighterRoleId(int fighter_number);
	int GetGetFighterIndex(int role_id);
};


class ActivityChiefPeakBattle : public Activity
{
public:
	ActivityChiefPeakBattle(ActivityManager *activity_manager);
	~ActivityChiefPeakBattle();
public:
	static int FIGHTER_NUMBER[ACTIVITY_CHIEF_PEAK_BATTLE_FIGTHER_NUM];
	static int  GetMatchSeq();	//获取用来当scene_key

	virtual void OnUserLogin(Role *role);
	virtual void OnActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnBattleServerDisconnect(int battle_server_id);;

	//----------------------------------------------------------------------------------------------
	void SendRoleInfo(Role * role);
	void SendMatchInfo(Role * role);
	void Bet(Role * role, int bet_type, int figther_index);			//下注
	void GotoReadyScene(Role * role);
	//----------------------------------------------------------------------------------------------
	void HiddenToGameSendRoleInfo(int role_id);
	void HiddenToGameSendMatchInfo(int role_id);
	void HiddenToGameSendNoticeToRole(int role_id);
	//----------------------------------------------------------------------------------------------

	void SetFightResult(int role_id, bool is_win);
	void OnRoleLeaveMatch(int role_id);
	void SendBetReward(int champion_role_id);


	bool IsJoinFighter(int uid, int ARG_OUT * out_index = NULL);
private:
	static int m_seq;

private:
	void UpdateMatchInfo(time_t now_second);	
	void ClearData();

	ActivityChiefPeakBattleMatchInfo m_match_info;
};


#endif