#ifndef __SPECIAL_LOGIC_GUILD_FIGHT_HPP__
#define __SPECIAL_LOGIC_GUILD_FIGHT_HPP__

#include "scene/speciallogic.hpp"
#include "engineadapter.h"

#include <set>

enum GUILD_FIGHT_RESULT_TYPE		// 家族大乱斗 - 战斗结果
{
	GF_TYPE_LOSE = 0,		// 失败
	GF_TYPE_VICT = 1,		// 胜利
	GF_TYPE_DRAW = 2,		// 平局

	GF_TYPE_MAX
};

struct GuildFightFamilysInfo		// 家族信息容器
{
	GuildFightFamilysInfo() : total_score(0), victory_times(0), fight_times(0), score_change_time(0) {}
	GuildFightFamilysInfo(int _score, bool _is_win)
	{
		total_score = _score;
		victory_times = _is_win ? 1 : 0;
		fight_times = 1;
		score_change_time = (UInt32)EngineAdapter::Instance().Time();
	}

	UInt32 total_score;			// 总积分
	UInt32 victory_times;		// 本家族玩家的胜利场次
	UInt32 fight_times;			// 本家族玩家的战斗场次
	UInt32 score_change_time;	// 上一次积分变动的时间戳
};

struct GuildFightPrivateInfo		// 玩家个人信息容器
{
	GuildFightPrivateInfo() : failures(0) {}
	GuildFightPrivateInfo(bool is_win) : failures(is_win ? 0 : 1) {}

	int failures;	// 战斗没赢的次数；
};

struct GuildFightRankInfo
{
	GuildFightRankInfo() : guild_id(0) {}
	GuildFightRankInfo(int _guild_id, const GuildFightFamilysInfo & _info_con)
	{
		guild_id = _guild_id;
		info_con = _info_con;
	}

	int guild_id;
	GuildFightFamilysInfo info_con;
};

struct GuildFightPraInfo
{
	GuildFightPraInfo() : m_level(0) {}
	GuildFightPraInfo(int level, const UserID & id) : m_level(level), user_id(id) {}

	int m_level;
	UserID user_id;
};

struct GuildFightUserInfo
{
	GuildFightUserInfo() : level(0), particip_award_flag(0), survival_award_flag(0) {}
	GuildFightUserInfo(int _level) : level(_level), particip_award_flag(0), survival_award_flag(0) {}

	short level;
	char particip_award_flag;			// 参与奖励识标 0:未领取 1:已领取 
	char survival_award_flag;			// 存活奖励表示(二进制)
};

class SpecialLogicGuildFight: public SpecialLogic		// 家族大乱斗
{
public:
	SpecialLogicGuildFight(Scene* scene);
	virtual ~SpecialLogicGuildFight() {};

	virtual bool CanDestroy() { return false; }
	virtual bool CanLeaveGuild() { return false; }
	virtual void Update(unsigned long interval, time_t now_second);				// 检测存活奖励
	virtual void OnRoleEnterScene(Role *role);
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);
	virtual bool CanChallengeOthers(Role * role, Role * target);
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);
	virtual bool OnOpenTreasureChest(Role *role, TreasureChest * chest_obj);
	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create);						
	virtual bool CanInvitationRole(Role *role, int uid);
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader);
	virtual int CreateTeamType() { return 165; }

	// 场景逻辑
	void OnActivityStandy();
	void OnActivityOpen();
	void OnActivityEnterTime();
	
	// 活动逻辑
	void RequestGuildFightRankInfo(Role * role);
	void OnActivityEnd();		// 活动结束，发放排名奖励，强制结束战斗；

	// 世界逻辑
	void OnRoleLogin(Role * role);

	//记录打开排行榜玩家
	void OnOpenRank(Role * role);
	void OnCloseRank(Role * role);

	void OnAdavanceFetch(Role * role, short op_type, int param1);
	void SendAdvanceFetchInfo(Role * role);

private:
	void InitFightingHall();		// 初始化场景

	void OnGuildFightOver(std::set<int> uid_list, int result_type);
	void RefreshGuildFightRank();									// 整理一次排名
	void SendGuildFightRank(Role * role = NULL, bool send_my_family_info = false);
	void SendInSceneAllRoleInfo();									// 发送当前场景的所有玩家信息
	void SendActivityEndNoticeAndInfo();							// 发送给所有参与活动的家族的所有在线玩家(显示结算弹窗)
	void SendOpenRankInfoUser();									// 排行榜变更,下发给停留在排行榜界面的玩家
	void OnRefreshChest();											// 刷新宝箱
	void ExtinctAllChest();											// 清理宝箱
	bool RandAndPutChestAwardInBag(Role * role, int chest_id, bool is_leader = false);		// 随机获取一次宝箱奖励，并将奖励放入背包
	void GuildFightIssueActivityAward();							// 发放活动奖励
	bool GuildFightIssueSurvivalAward(int need_check_num);			// 发放存活奖励(屏蔽)
	void GuildFightIssueSurvivalAward();							// 发放所有存活奖励(活动结束发放)
	void GuildFightIssueParticipAward();							// 发放参与奖励
	void SendGuildParticipateRoleNum(Role * role = NULL);			// 发送本家族参与的人数
	void SendGuildFightSpecialInfo(Role * role = NULL);				// 发送乱斗特殊信息
	void ForceStopGuildFightBattle();
	void SetGuildHonorBattleNeedInfo();				//设置工会战需要的信息

	bool m_need_refresh_rank;				// 是否需要刷新积分排行榜
	time_t m_next_refresh_rank_timestamp;	// 下次允许刷新积分排行榜的时间戳
	
	std::set<int> m_survival_award_need_check_num;					// 存活角色数检测

	std::map<int, std::set<ObjID> > m_chest_list;					// 场景中存在的宝箱的 ObjID
	std::map<int, UInt64> m_next_chest_refersh_time;				// 宝箱下次刷新时间戳：键 - 宝箱ID；值 - 下次刷新的时间戳

	std::map<int, GuildFightFamilysInfo> m_familys_info_map;		// 家族信息列表：键 - 家族ID；值 - 信息容器
	std::map<int, GuildFightPrivateInfo> m_private_info_map;		// 个人信息列表；键 - 玩家UID；值 - 信息容器

	std::vector<GuildFightRankInfo> m_gf_rank_list;					// 排名列表
	std::map<int, GuildFightUserInfo> m_participate_player;			// 参与活动的所有玩家 - 活动开始时记录一次 key:role_id
	std::map<int, std::set<int> > m_family_palyer_in_scene;			// 场景内的活跃家族玩家；键 - 家族ID
	std::map<int, int> m_guild_participate_num;						// 家族参与人数；	键 - 家族ID - 活动开始时记录一次
	std::set<int> m_scene_open_rank_list;							// 场景中打开排行榜的玩家 uid

	std::map<int, std::vector<GuildFightPraInfo> > m_survival_map;	// 存活奖励记录 key:存活奖励规则数 
	time_t next_box_refresh_tamp;//宝箱刷新时间戳
	int m_box_num;				//未开宝箱数量
	time_t force_to_next_time;	//场景人数不足延时关闭

	std::map<int, time_t> m_defeat_protect_time;

	bool is_refresh_box_flag;	//是否刷新宝箱
	bool m_is_arena_close;		//赛场是否已关闭

	unsigned int m_delay_close_arena_time;	// 用于留出时间让掉线玩家重登
	bool is_send_fight_special_info;		
};

#endif