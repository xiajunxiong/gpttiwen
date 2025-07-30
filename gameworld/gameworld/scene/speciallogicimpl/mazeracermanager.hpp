#ifndef __MAZE_RACER_MANAGER_HPP__
#define __MAZE_RACER_MANAGER_HPP__

#include "servercommon/serverdef.h"
#include "global/activity/activity.hpp"

#include <map>
#include <vector>
#include <set>

static const int MAZE_RACER_COPY_ID_BEGIN = 100;		// 副本的样品ID(SCENE_KEY) 从 100 开始
static const int MAZE_RACER_PASS_ID_BEGIN = 0;			// 通关编号 从 0 开始

struct TeamAmuletInfo		// 队伍受咒效果容器
{
	TeamAmuletInfo(UInt16 _id = -1, UInt16 _type = 0, UInt16 _val = 0, UInt32 _time = 0) : amulet_id(_id),
		effect_type(_type), effect_value(_val), effect_time(_time) {}

	UInt16 amulet_id;
	UInt16 effect_type;
	UInt16 effect_value;
	time_t effect_time;		// 持续的时间 - 剩余次数、失效时间戳
};

struct TeamParamCommonData
{
	TeamParamCommonData();
	int GetAmuletIdByEffectType(int effect_type);
	int GetFirstAmuletId();
	TeamAmuletInfo * GetAmuletInfo(int effect_type);

	std::map<int, TeamAmuletInfo > team_mmulet_info;
};

struct TeamLocation		// 队伍所处层数
{
	TeamLocation(int _copy_id = 0, int _layer = 0) : copy_id(_copy_id), layer(_layer) {};

	UInt32 copy_id;
	int layer;
};

struct MazeRacerRoleJoinInfo		// 玩家参加记录
{
	MazeRacerRoleJoinInfo()
	{
		role_id = 0;
		best_pass_time = 0;		//最好通关时间
		get_fb_pass_reward_times = 0;
		join_times = 0;
	}

	int role_id;
	time_t best_pass_time;		//最好通关时间
	int get_fb_pass_reward_times;
	int join_times;
};


class Team;
class SpecialLogicMazeRacer;
class MazeRacerManager
{
public:
	MazeRacerManager();
	virtual ~MazeRacerManager();

	static MazeRacerManager & Instance();

	void Update(time_t now_second);

	void InitWaitingHall();
	void KickWaitingHall();
	bool InitMazeCopy(int maze_id, int * copy_id, int * scene_id);
	int GetNextTiersScene(int copy_id, int layer);

	void OnEncounterMonster(int team_index);
	void OnTeamEnter(int copy_id, int layer, int team_index);
	void OnTeamPass(int copy_id, int maze_id, Team * team);
	void RefreshTeamStatus(int team_index);
	void OnSendRoleInfo(Role * role);

	void SendAllRankInfo(Role * role = NULL);
	void SendAllRankInfoHiddenToGame(int role_id);
	void SendEctypeRankInfo(int copy_id);		// 发送副本排名信息给副本内的玩家

	void CheckIssueActivityAward(time_t now_second, int next_open_day);
	void OnActivityStandy();
	void OnActivityOpen();
	void OnActivityEnd();
	void Clear();

	SpecialLogicMazeRacer * GetSPMazeRacer(int copy_id, int layer);
	bool IsMazeRacerScene(SceneIndex scene_index);

	int & GetTeamResiAmulet(int team_index, int amulet_id);
	void AddAmuletToTeam(int team_index, int amulet_id, int num);
	void AddAmuletBuffToTeam(int user_team_index, int target_team_index, int amulet_id);
	TeamParamCommonData * GetTeamParamCommonData(int team_index);
	TeamAmuletInfo * GetTeamSufferAmulet(int team_index, int effect_type);	
	bool TeamHasSufferAmuletBuff(int team_index);

	void SendAmuletNumToTeam(int team_index);		// 队伍符咒数量通知
	void AmuletEffectMessage(int team_index);		// 队伍受咒效果通知

	void AddJoinTimes(int role_id);
	bool CanJoin(int role_id);
	MazeRacerRoleJoinInfo & GetRoleJoinInfo(int role_id);

	void GmSetActivityRankTimestamp();
private:
	void SendMazeRacerActivityRank(int maze_id, int role_id = 0);
	void MazeRacerIssueActivityAward();			// 发放活动排名奖励 - 方式：邮件 - 时机：时间戳到达时

	void ForceStopMazeRacerBattle();
	void ReleaseSpLogicMap();

	// 活动逻辑
	time_t m_give_rank_award_timestamp;			// 发放排名奖励的时间戳
	std::set<UInt32> m_pass_id_list;			// 通关编号列表
	std::map<int, std::map<UInt32, std::vector<MazeRacerTeamInfo> > > m_pass_rank_list;		// 活动通关排行榜单：键 - 迷宫ID、通关秒数；值 - 队伍数据信息容器

	// 副本逻辑
	std::map<UInt32, UInt32> m_begin_timestamp;												// 副本创建的时间戳：键 - 副本拷贝ID；值：创建的时间戳
	std::map<UInt32, UInt32> m_encounter_times;												// 队伍遇到怪的次数：键 - 队伍 Index；值：遇怪次数
	std::map<UInt32, TeamLocation> m_location_map;											// 队伍所处层数：键 - 队伍 Index；值：容器
	std::map<UInt32, std::map<int, SceneIndex> > m_maze_list;								// 逻辑统合映射：键 - 副本拷贝ID、层数；值：场景唯一Index
	std::map<UInt32, std::map<UInt32, std::vector<MazeRacerTeamInfo> > > m_ectype_rank_list;// 副本队伍通关排行：键 - 副本拷贝ID、通关秒数；值：队伍信息
	std::set<SceneIndex> m_need_del_scene_index;										// 待删除

	std::map<int, std::map<int, int> > m_team_amulet_map;			// 队伍持有的符咒数量：键 - 队伍ID、符咒ID；值 - 持有数量
	std::map<int, TeamParamCommonData> m_team_common_data_map;							// 队伍受到符咒效果参数：键 - 队伍ID；值 - 效果容器

	//记录玩家参加次数
	std::map<int, MazeRacerRoleJoinInfo> m_role_to_join_info;
	int m_scenc_key;
};

#endif