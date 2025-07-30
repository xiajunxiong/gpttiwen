#ifndef __SPECIAL_LOGIC_WORLD_TEAM_ARENA_HPP__
#define __SPECIAL_LOGIC_WORLD_TEAM_ARENA_HPP__

#include "scene/speciallogic.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/struct/global/worldteamarenaparam.hpp"
#include "engineadapter.h"

enum WORLD_TEAM_ARENA_RESULT_TYPE		// 英雄会武 - 战斗结果
{
	WTA_TYPE_LOSE = 0,		// 失败
	WTA_TYPE_VICT = 1,		// 胜利
	WTA_TYPE_DRAW = 2,		// 平局

	WTA_TYPE_MAX
};

struct WorldTeamArenaSceneUserInfo
{
	WorldTeamArenaSceneUserInfo() : level(0), particip_award_flag(0), reserve_ch(0) {}
	WorldTeamArenaSceneUserInfo(int _level) : level(_level), particip_award_flag(0), reserve_ch(0) {}

	short level;
	char particip_award_flag;			// 参与奖励识标 0:未领取 1:已领取 
	char reserve_ch;
};

class SpecialLogicWorldTeamArena : public SpecialLogic
{
public:
	SpecialLogicWorldTeamArena(Scene* scene);
	virtual ~SpecialLogicWorldTeamArena() {};

	virtual void OnRoleEnterScene(Role *role);
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);
	virtual void OnRoleLeaveTeam(Role * role);						// 玩家离开队伍时，将玩家踢出场景
	virtual bool CanJoinOtherActivity() { return false; }			//!< 是否允许加入其它活动
	virtual bool CanDestroy() { return false; }
	virtual bool CanLeaveGuild() { return false; }
	virtual void Update(unsigned long interval, time_t now_second);				
	virtual bool CanChallengeOthers(Role * role, Role * target);

	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create);						// 场景内不允许组队
	virtual bool CanInvitationRole(Role *role, int uid) { return false; }
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader);

	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_WORLD_TEAM_ARENA; }

	int GetTeamIndexByUnqiueId(long long team_unique_id);

	//活动逻辑
	void OnActivityStandy();
	void OnActivityEnterTime();		// 入场时间
	void OnActivityOpen();
	void OnActivityEnd();			// 活动结束，发放排名奖励，强制结束战斗；
	void RequestFightRankInfo(Role * role);

	void OnRoleLogin(Role * role);	
	void OnInitiateFight(Role * role, int target_uid);	//发起战斗

	//记录打开排行榜玩家
	void OnOpenRank(Role * role);
	void OnCloseRank(Role * role);

	void OnAdavanceFetch(Role * role, short op_type, int param1);
	void SendAdvanceFetchInfo(Role * role);

private:
	void InitFightingHall();													// 初始化场景

	void OnGuildFightOver(std::set<int> uid_list, int result_type);
	void RefresTeamArenaRank();													// 整理一次排名
	void SendWorldTeamArenaRank(Role * role = NULL, bool send_my_info = false);	// 发送排行信息
	void SendInSceneAllRoleInfo();												// 发送当前场景的所有玩家信息
	void SendAllParticipNotice();												// 通知所有参与玩家排行榜结果
	void SendOpenRankInfoUser();												// 排行榜变更,下发给停留在排行榜界面的玩家
	void SendWorldTeamArenaSpecialInfo(Role * role = NULL);						// 发送英雄会武特殊信息
	void WorldTeamArenaParticipAward();											// 发放参与奖励
	void WorldTeamArenaRankAward();												// 发放活动排名奖励
	void ForceStopWorldTeamArenaBattle();										// 强制中断英雄会武活动的战斗

	std::map<long long, std::set<int> > m_scene_team_map;						// 场景中存在的战队 key:team_unique_id value:role_id

	std::map<long long, WorldTeamArenaRankItem> m_team_arena_info_map;			// 战队竞技信息 key:team_unqiue_id
	std::vector<WorldTeamArenaRankItem> m_rank_list;							// 战队排名列表
	std::map<int, WorldTeamArenaSceneUserInfo> m_participate_user_list;			// 参与活动的所有玩家 - 活动开始时记录 key:role_id
	std::set<int> m_scene_open_rank_list;										// 场景中打开排行榜的玩家 uid

	bool m_need_refresh_rank;													// 是否需要刷新积分排行榜
	time_t m_next_refresh_rank_timestamp;										// 下次允许刷新积分排行榜的时间戳

	bool m_is_arena_close;														// 赛场是否已关闭
	time_t force_to_next_time;													// 场景人数不足延时关闭

	std::map<long long, time_t> m_defeat_protect_time;							// 战队保护时间 key:team_unique_id
	unsigned int m_delay_close_arena_time;										// 用于留出时间让掉线玩家重登
	bool is_send_fight_special_info;
};


#endif