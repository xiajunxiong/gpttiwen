#ifndef __ACTIVITY_CHIEF_ELECTION_HPP__
#define __ACTIVITY_CHIEF_ELECTION_HPP__

#include "global/activity/activity.hpp"
#include <vector>
#include <map>
#include <set>

class Role;
struct UserCacheNode;

//淘汰赛对战结果
enum ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT	
{
	ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_INVAILD = 0,
	ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE,
};

//淘汰赛每轮状态
enum ACTIVITY_CHIEF_ELECTION_ROUND_STATE
{
	ROUND_STATE_INVALID = -1,
	ROUND_STATE_READY = 0,
	ROUND_STATE_START,
	ROUND_STATE_MAX,
};

//混战还是淘汰赛
enum ACTIVITY_CHIEF_ELECTION_STATE
{
	ACTIVITY_CHIEF_ELECTION_STATE_SEA_ELECTION_READY,		//混战准备
	ACTIVITY_CHIEF_ELECTION_STATE_SEA_ELECTION_FIGHT,		//混战
	ACTIVITY_CHIEF_ELECTION_STATE_SEA_ELECTION_FINISH,		//混战完成
	ACTIVITY_CHIEF_ELECTION_STATE_ELIMINATION,		//16强淘汰赛
	ACTIVITY_CHIEF_ELECTION_STATE_FINISH,		//16强淘汰赛完成
};

struct ActivityChiefElectionBattleInfo
{
	ActivityChiefElectionBattleInfo()
	{
		scene_id = 0;
		scene_key = 0;
	}

	int scene_id;
	int scene_key;
};

struct ActivityChiefElectionFigtherInfo
{
	ActivityChiefElectionFigtherInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		role_id = 0;
		number = 0;
		match_round = 1;
		state = ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_INVAILD;
		opponent_number = 0;
	}

	int role_id;
	int number;
	int match_round;
	int state;
	int opponent_number;		//待删，没作用
	ActivityChiefElectionBattleInfo scene_info;
};

struct  ActivityChiefElectionPerProfMatchInfo
{
	bool static SortFuncFinish(const ActivityChiefElectionFigtherInfo & item1, const ActivityChiefElectionFigtherInfo & item2);

	ActivityChiefElectionPerProfMatchInfo();

	void Update(time_t now_second);
	void UpdateSeaElection(time_t now_second);
	void UpdateMatchInfo(time_t now_second);
	void NoticeRoundInfo();
	void SendNoticeToRole(Role * role);
	void HiddenToGameSendNoticeToRoleById(int role_id) const;
	void CheckFinish();
	void OnActivityClose();

	void GotoMatchScene(int figther_index);
	bool GetSceneInfo(int role_id, int * scene_id, int * scene_key);
	void StartRoundFight();
	void SetFightResult(int role_id, bool is_win, bool is_check_finish = false);
	void SendBattleReward(int role_id, bool is_win,int match_round);
	void SendBattleResultNotice(int role_id, int is_win);
	void SendMatchInfoUpdate() const;
	void OnMatchFinish();
	void OnRoleLeaveMatch(int role_id);

	int MAX_MATCH_ROUND;
	int curr_match_round;
	int round_state;
	time_t m_next_round_state_time;
	bool is_match_finish;			//比赛是否完成了

	int which_prof_match;
	int figther_info_num;
	ActivityChiefElectionFigtherInfo figther_info[ACTIVITY_CHIEF_ELECTION_FIGTHER_NUM];

	//---------------------------------------------
	void SendMatchState(Role * role = NULL);
	void HiddenToGameSendMatchState(int role_id);
	void OnSeaElectionFinish(std::vector<int> sea_election_win_role);	//混战结束
	void SetSeaElectionFinish();
	int match_state;		//混战还是淘汰赛
	time_t next_match_state_time;

private:
	ActivityChiefElectionFigtherInfo * GetOpponentInfo(int figther_index);
	int GetOpponentRoleId(int figther_index);
};

struct ActivityChiefElectionRank
{
	ActivityChiefElectionRank()
	{
		role_id = 0;
		capability = 0;
	}

	ActivityChiefElectionRank(int rold_id,int capability)
	{
		this->role_id = rold_id;
		this->capability = capability;
	}

	int role_id;
	int capability;
};


class ActivityChiefElection : public Activity
{
public:
	static int FIGHTER_NUMBER[ACTIVITY_CHIEF_ELECTION_FIGTHER_NUM];

	ActivityChiefElection(ActivityManager *activity_manager);
	~ActivityChiefElection();

	virtual void Init(const ActivityData & arg_data);
	virtual void GetInitParam(ActivityData & arg_data);

	int static GetMatchSeq();	//获取用来当scene_key
	virtual void OnUserLogin(Role *role);
	virtual void OnUserLoginOriginGame(int role_id);
	virtual void OnActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);

	//-----------------------------------------------
	void GotoChiefElectionSea(Role * role);

	//-----------------------------------------------
	void GotoReadyScene(Role * role);
	void SendMatchInfo(Role * role);
	//------------------------------------------------------------
	void HiddenToGameSendMatchInfo(int role_id, int base_prof);
	void HiddenToGameSendNoticeToRole(int role_id, int base_prof);
	void HiddenToGameSendMatchState(int role_id);
	//---------------------------------------------------------

	void SetFightResult(int role_id, bool is_win, int which_prof_match);
	void OnRoleLeaveMatch(int role_id, int which_prof_match);
	void OnSeaElectionFinish(int which_prof, std::vector<int> sea_election_win_role);
	void SetSeaElectionFinish(int which_prof);
	//---------------------------------------
	bool GetScene(int role_id, int base_prof,int now_scene_id, int * scene_id, int * scene_key);
	bool CanEnter(int role_id, int base_prof);
	//----------------

	void GmNextMatchState(int which_prof_match);
private:
	void ClearData();
	void UpdateMatchInfo(time_t now_second);
	bool IsAllFinish();

	static int m_seq;	
	std::map<int, ActivityChiefElectionPerProfMatchInfo> m_prof_match_info;
};

#endif