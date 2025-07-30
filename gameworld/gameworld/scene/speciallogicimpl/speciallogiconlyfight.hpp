#ifndef __SPECIAL_LOGIC_ONLY_FIGHT_HPP__
#define __SPECIAL_LOGIC_ONLY_FIGHT_HPP__

#include "scene/speciallogic.hpp"
#include "servercommon/servercommon.h"
#include "engineadapter.h"

#include <set>

enum ONLY_FIGHT_RESULT_TYPE		// 以一敌百(个人版家族大乱斗) - 战斗结果
{
	OF_TYPE_LOSE = 0,		// 失败
	OF_TYPE_VICT = 1,		// 胜利
	OF_TYPE_DRAW = 2,		// 平局

	OF_TYPE_MAX
};

struct OnlyFightPrivateInfo
{
	OnlyFightPrivateInfo(): lose_time(0), total_score(0), victory_times(0), fight_times(0), score_change_timestamp(0) {}
	OnlyFightPrivateInfo(int _scroe, bool is_win, int _guild_id = 0)
	{
		lose_time = is_win ? 0 : 1;
		total_score = _scroe;
		victory_times = is_win ? 1 : 0;
		fight_times = 1;
		score_change_timestamp = (UInt32)EngineAdapter::Instance().Time();
	}

	UInt32 lose_time;				// 玩家失败的次数
	UInt32 total_score;				// 玩家总积分
	UInt32 victory_times;			// 玩家胜利场次
	UInt32 fight_times;				// 玩家战斗场次
	UInt32 score_change_timestamp;	// 上一次积分变动的时间戳
};

struct OnlyFightRankInfo
{
	OnlyFightRankInfo(): uid(0) {}
	OnlyFightRankInfo(int _uid, const OnlyFightPrivateInfo & _info_con)
	{
		uid = _uid;
		info_con = _info_con;
	}

	int uid;
	OnlyFightPrivateInfo info_con;
};

struct OnlyFightPraInfo
{
	OnlyFightPraInfo(): m_level(0) {}
	OnlyFightPraInfo(int level, const UserID & id): m_level(level), user_id(id) {}

	int m_level;
	UserID user_id;
};

struct OnlyFightUserInfo
{
	OnlyFightUserInfo() : level(0), particip_award_flag(0), survival_award_flag (0) {}
	OnlyFightUserInfo(int _level) : level(_level), particip_award_flag(0), survival_award_flag(0) {}

	short level;
	char particip_award_flag;			// 参与奖励识标 0:未领取 1:已领取 
	char survival_award_flag;			// 存活奖励表示(二进制)
};

class SpecialLogicOnlyFight : public SpecialLogic 
{
public:
	SpecialLogicOnlyFight(Scene* scene);
	virtual ~SpecialLogicOnlyFight() {};

	virtual void OnRoleEnterScene(Role *role);
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);
	virtual bool CanJoinOtherActivity() { return false; }			//!< 是否允许加入其它活动
	virtual bool CanDestroy() { return false; }
	virtual bool CanLeaveGuild() { return false; }
	virtual void Update(unsigned long interval, time_t now_second);				// 检测存活奖励
	virtual bool CanChallengeOthers(Role * role, Role * target);

	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_ONLY_FIGHT; }
	virtual bool OnOpenTreasureChest(Role *role, TreasureChest * chest_obj);

	// 场景逻辑
	void OnActivityStandy();
	void OnActivityOpen();
	void OnActivityEnterTime();
	bool SceneRoleNumEmpty();     

	// 活动逻辑
	void RequestOnlyFightRankInfo(Role * role);
	void OnActivityEnd();			// 活动结束，发放排名奖励，强制结束战斗；

	// 世界逻辑
	void OnRoleLogin(Role * role);	
	
	//记录打开排行榜玩家
	void OnOpenRank(Role * role);
	void OnCloseRank(Role * role);

	void OnAdavanceFetch(Role * role, short op_type, int param1);
	void SendAdvanceFetchInfo(Role * role);

private:
	void InitFightingHall();												// 初始化场景

	void OnOnlyFightOver(int uid, int result_type);	
	void RefreshOnlyFightRank();											// 整理一次排名
	void SendOnlyFightRank(Role * role = NULL, bool send_my_info = false);	// 发送排行信息
	void SendInSceneAllRoleInfo();											// 发送当前场景的所有玩家信息
	void SendAllParticipNotice();											// 通知所有参与玩家排行榜结果
	void SendOpenRankInfoUser();											// 排行榜变更,下发给停留在排行榜界面的玩家
	void OnRefreshChest();													// 刷新宝箱
	void ClearAllChest();													// 清理宝箱
	bool RandAndPutChestAwardInBag(Role * role, int chest_id);				// 随机获取一次宝箱奖励，并将奖励放入背包
	void OnlyFightIssueActivityAward();										// 发放活动奖励
	bool OnlyFightIssueSurvivalAward(int need_check_num);					// 发放存活奖励(屏蔽)
	void OnlyFightIssueSurvivalAward();										// 活动结束发放所有存活奖励
	void OnlyFightIssueParticipAward();										// 发放参与奖励
	void SendOnlyFightSpecialInfo(Role * role = NULL);						// 发送乱斗特殊信息
	void ForceStopOnlyFightBattle();										// 强制中断以一敌百活动的战斗

	bool m_need_refresh_rank;										// 是否需要刷新积分排行榜
	time_t m_next_refresh_rank_timestamp;							// 下次允许刷新积分排行榜的时间戳

	std::set<int> m_survival_award_need_check_num;					// 存活角色数检测
	std::map<int, std::set<ObjID> > m_chest_list;					// 场景中存在的宝箱的 ObjID
	std::map<int, UInt64> m_next_chest_refersh_time;				// 宝箱下次刷新时间戳：键 - 宝箱ID；值 - 下次刷新的时间戳

	std::map<int, OnlyFightPrivateInfo> m_private_info_map;			// 个人信息列表；键 - 玩家UID；值 - 信息容器

	std::vector<OnlyFightRankInfo> m_of_rank_list;					// 排名列表
	std::map<int, OnlyFightUserInfo> m_participate_player;			// 参与活动的所有玩家 - 活动开始时记录一次 key:role_id
	std::set<int> m_player_in_scene;								// 场景内活跃的玩家   记录uid
	std::set<int> m_scene_open_rank_list;							// 场景中打开排行榜的玩家 uid

	std::map<int, std::vector<OnlyFightPraInfo> > m_survival_map;	// 存活奖励记录 key:存活奖励规则数 

	time_t next_box_refresh_tamp;									//宝箱刷新时间戳
	int m_box_num;													//未开宝箱数量
	time_t force_to_next_time;										//场景人数不足延时关闭 (由原来的关闭活动改为T玩家出场景)

	std::map<int, time_t> m_defeat_protect_time;
	bool is_refresh_box_flag;
	bool m_is_arena_close;											// 赛场是否已关闭

	unsigned int m_delay_close_arena_time;							// 用于留出时间让掉线玩家重登
	bool is_send_fight_special_info;
};

#endif