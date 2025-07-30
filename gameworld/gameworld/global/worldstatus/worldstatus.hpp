#ifndef __WORLD_STATUS_HPP__
#define __WORLD_STATUS_HPP__

#include "iclockmodule.h"
#include "inetworkmodule.h"
#include "servercommon/struct/global/worldstatusparam.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "protocol/msghuanshou.hpp"

#include <vector>

namespace crossgameprotocal
{
	struct HiddenGameSyncWorldStatus;
};

struct FakeAnnouncementCfg;
struct FakeAnnouncementInfo
{
	FakeAnnouncementInfo() : min_level(0), max_level(0), announcement_type(0), is_random_name(false), item_id(0)
	{}

	void SetData(const FakeAnnouncementCfg * data);

	int min_level;
	int max_level;
	int announcement_type;
	bool is_random_name;
	int item_id;
	std::string descripte_1;
	std::string descripte_2;
	std::string descripte_3;
};


enum WORLDSTATE_DATA_STATUS
{			
	WORLDSTATE_DATA_STATUS_INVALID = 0,						// 初始状态
	WORLDSTATE_DATA_STATUS_LOADING,							// 数据加载中 
	WORLDSTATE_DATA_STATUS_LOAD_FINISH,						// 数据加载完成 
	WORLDSTATE_DATA_STATUS_MAX,
};

static const int WORLDSTATUS_SAVE_INTERVAL = 30;			// 半分钟回写一次
class Role;
class WorldStatus
{
	friend class RMIInitWorldStatusBackObjectImpl;
	friend class RMIInitActivityDataBackImpl;
	friend class RMIInitActivityRoleDataBackObjectImpl;

public:
	static WorldStatus & Instance();

	void Init(const WorldStatusParam &worldstatus_param);

	void OnServerStart();
	void OnServerStop();

	void OnUserLogin(Role * role);
	void OnGameServerConnect(std::set<int> server_id);

	void Update(unsigned long interval, time_t now_second);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnWeekChange();
	void OnTwoWeekChange();
	void OnResetData(unsigned int old_dayid, unsigned int now_dayid);

	bool IsLoadFinish() const;
	void InitLoadFinish();

	unsigned int GetLastUpdateDayId();
	void SetLastUpdateDayId(unsigned int dayid);

	unsigned int GetLastResetRoleModuleDataDayId() { return m_common_data.last_reset_role_module_dayid; }
	void SetLastResetRoleModuleDataDayId(unsigned int dayid) { m_common_data.last_reset_role_module_dayid = dayid; }
	unsigned int GetLastResetRoleModuleDataTimestamp() { return m_common_data.last_reset_role_modlue_timestamp; }
	void SetLastResetRoleModuleDataTimestamp(unsigned int timestamp) { m_common_data.last_reset_role_modlue_timestamp = timestamp; }

	unsigned int GetStartTime() { return m_start_time; }


	WorldStatusCommonDataParam & GetCommonData() { return m_common_data; }
	WorldHonorRollDataParam & GetHonorRollData() { return m_honor_roll_data; }
	bool IsResetComCapRank() { return m_common_data.is_reset_complex_cap_rank_flag != 0;}
	void SetResetComCapRankFlag(bool flag) { m_common_data.is_reset_complex_cap_rank_flag = flag ? 1 : 0; }

	void SetCourageChallengeFirstKill(int level, Role * role);
	void SendCourageChallengeFirstKillInfo(Role * role = NULL);

	int GetWorldLevel(bool is_cal = false);
	int GetBraveGroundWorldLevel();
	void SendWorldLevelInfo(Role * role = NULL);
	void OnCalcCrossOpenDay();
	unsigned int GetCrossOpenTimestamp()const { return m_common_data.open_cross_server_zero_timestamp; }

	bool IsChiefElectionWinner(int role_id,int prof);
	bool OnChiefElectionFinish(int role_id,int prof);
	bool GmsetChiefElection(int role_id, int prof);
	void ClearChiefElectionInfo(bool notice = true);
	void SetChiefElectionInfo(const ChiefElectionDataParam & chief_election_data);
	void SendChiefElectionInfo(Role * role = NULL);
	std::vector<int> GetChiefElectionTopRole();

	void OnChiefPeakBattleFinish(int role_id,int match_round);
	void ClearPeakBattleInfo(bool notice = true);
	void SetChiefPeakBattleInfo(const ChiefPeakBattleDataParam & chief_peak_battle_data);

	int GetChiefPeakBattleMatchRound(int role_id);
	void SendChiefPeakBattleInfo(Role * role = NULL);

	void GetServerSchoolTask(int* out_list, int daily_num);

	void OnConnectCrossServerSucc();

	void SyncWorldStatusHiddenToGameworld(int server_id = 0);			//隐藏服某些世界状态改变同步到游戏服
	void SetWorldStatus(const crossgameprotocal::HiddenGameSyncWorldStatus * msg);
	
	//*************************世界答题
	bool IsInWorldAnswerTime() { return m_common_data.answer_end_tamp > 0; }//世界答题活动是否开启中
	bool IsAnswerTime() { return m_common_data.answer_tamp > 0; }//是否世界答题时间
	void RefreshWorldQuestion(time_t now_time,bool is_begin = false);//刷新世界答题
	bool RoleAnswerQuestion(Role *role, int answer_id);//回答问题
	void OnRefreshAnswerSubject(time_t now_time);//公布结果
	void SendWorldAnswerInfo(Role * role = NULL);
	//----------------------以一敌百(个人版家族大乱斗)-----------------------
	void SendTitleToRole(int title_id, int uid, const char * reason);		//活动结束发放称号
	void OnlyFightStatusChange(int to_status, bool is_enter = false);								//活动状态改变
	void CheckOnlyFightSceneRoleNum();										//检测以一敌百所有赛场人数

	int GetOnlyFightRoleRank(int role_uid);
	int GetOnlyFightRankTitleId(int rank);									
	void GetOnlyFightRankTitleList(std::vector<int>* title_list);			
	void AddOnlyFightRecord(int length, int * id_list);						//记录排行信息
	void HiddenSynOnlyFightDataToAllGame();									//隐藏服向所有游戏服同步数据
	void HiddenGameOnlyFightDataSyn(OnlyFightDataParam * param);			//原服收到隐藏服同步过来的数据
	void GameHiddenOnlyFightDataSynReq(int server_id);						//隐藏服收到原服请求后同步数据回原服
	void ClearOnlyFightRankInfo();							
	void OnReCalOnlyFightMap();												//重新计算Map容器映射				

	//假公告
	void OnCalcFakeAnnouncement(unsigned int now_second);
	void OnCheckFakeAnnouncement(unsigned int now_second);

	//幻兽公告
	void AddHuanShouAnnInfo(Role * role, int egg_item_id, int huanshou_id);
	void AddHuanShouAnnInfo(HuanShouAnnouncementInfo ann_info);
	void OnSendHuanShouAnnInfo(Role * role);
	void OnHiddenToGameHuanShouAnnInfo(int uid);
	void GetHuanShouAnnInfo(Protocol::SCHuanShouAnnouncementInfo & info);

	//通知游戏服打log
	void SendNoticeLog(int uid, const GameName role_name, const PlatName plat_name, int activity_type, int param1, const char * reason_str);
	
	//GM控制
	void SetWorldQuestionStatus(int type);

	//*************************世界答题

	//----------------------全民团购-----------------------
	int GetRAQuanMinDiscountFirstMark();
	void SetRAQuanMinDiscountFirstMark();

	////////////////////////// 限时活动找回奖励////////////////////////////
	void SetFindEndTimestamp(int special_logic_activity_type);
	unsigned int GetFindEndTimestamp(int special_logic_activity_type);
	void GmSetFindEndTimestamp(int special_logic_activity_type, int end_day);
	

	unsigned int GetColosseumRefreshTamp() { return m_common_data.colosseum_refresh_tamp; }
	void SetColosseumRefreshTamp(unsigned int tamp) { m_common_data.colosseum_refresh_tamp = tamp; }

	unsigned int GetLunHuiWangChuanRefreshTamp() { return m_common_data.lun_hui_wang_chuan_refresh_tamp; }
	void SetLunHuiWangChuanRefreshTamp(unsigned int tamp) { m_common_data.lun_hui_wang_chuan_refresh_tamp = tamp; }

	void LogFunctionStats(Role * role, const char* function_name, int param1 = 0, int param2 = 0, int param3 = 0);

	unsigned int GetTopLevelRefreshTimestamp();	
	void OnReCalcTopLevelRefreshTimestamp(bool is_reset = false);

	void GmSendEndlessRankReward();
	void GmSetEndlessRankClear() { m_common_data.clear_person_rank_flag = 0; }

	//随机神装打造通告信息
	void RandBroadcastInfo();

	//情缘 婚宴id
	WeddingID CreatWeddingID();

	unsigned int GetHiddenOpenServerTime();
	//同步隐藏服开服时间,周期活动用到
	void SetSyncHiddenOpenServerTime(unsigned int hidden_open_time);
	void HiddenToGameSyncHiddenOpenServerTime(int server_id);
	void SendHiddenInfo(Role * role = NULL);

private:
	void GetInitHuanShouAnnParam(HuanShouAnnouncementParam * param);

private:
	WorldStatus();
	~WorldStatus();

	bool LoadWorldStatus(long long id_fram);
	void LoadWorldStatusSucc();
	bool SaveWorldStatus(); 

	bool LoadActivityData(long long id_from);
	void LoadActivityDataSucc();
	bool SaveActivityData(bool is_save_all = false);

	bool LoadActivityRoleData(long long inc_id_from);
	void LoadActivityRoleDataSucc();
	void SaveActivityRoleData();

	int m_data_state; 
	int m_activity_data_state;
	int m_activity_role_data_state;

	unsigned int m_start_time;							
	int m_world_time;									
	unsigned int m_last_update_dayid;	

	unsigned int m_last_pet_broadcast_timestamp;
	unsigned int m_last_godequip_broadcast_timestamp;

	bool world_gather_init;			// 常驻采集物
	int cross_server_open_day;		// 跨服服务器开服天数

	std::map<int, bool> m_world_answer_role_map;			//世界答题玩家回答列表uid - 只有回答正确才会在这
	std::map<int, int> m_only_fight_rank_map;				//以一敌百排行映射  key:role_uid second:排名 [1,3]

	unsigned int m_next_calc_fake_announcement_timestamp;		// 下次计算假公告的时间戳
	std::vector<FakeAnnouncementInfo> m_fake_announcement_list;	// 缓存假公告数据
	std::map<unsigned int, int> m_fake_announcement_time_map;	// 发布假公告数据 key:时间戳 second:缓存假公告数据下标

	std::list<HuanShouAnnouncementInfo> m_huanshou_announcement_list;	//幻兽公告列表

	WorldStatusCommonDataParam m_common_data;
	WorldHonorRollDataParam m_honor_roll_data;
	CourageChallengeDataParam m_courage_challenge_data;
	ChiefElectionDataParam m_chief_election_data;
	ChiefPeakBattleDataParam m_chief_peak_battle_data;
	OnlyFightDataParam m_only_fight_data;
	ShanHaiBossParam m_shan_hai_boss_data;
	WorldStatusGuildFightParam m_guild_fight_param;
	WeddingFB::HiddenGlobalParam m_wedding_global_param;

};

#endif // __WORLD_STATUS_HPP__


