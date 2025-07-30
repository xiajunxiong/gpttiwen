#ifndef __SPECIAL_LOGIC_MAZE_RACER_HPP__
#define __SPECIAL_LOGIC_MAZE_RACER_HPP__

#include "scene/speciallogic.hpp"
#include "protocol/msgactivity.hpp"
#include "obj/character/npc.h"

#include <map>
#include <set>

#include <vector>

static const int MAZE_RACER_TRANSFER_NPC_NUM = 3;					// 传送NPC 的数量
static const int MAZE_RACER_TRANSFER_TARGET_POSI_NUM = 6;			// 随机传送目标坐标数量

class SpecialLogicMazeRacer: public SpecialLogic
{
public:
	SpecialLogicMazeRacer(Scene* scene);
	virtual ~SpecialLogicMazeRacer();

	virtual bool CanDestroy();
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnRoleEnterScene(Role *role);											// 进入场景时，若玩家是队长，将队伍ID插入表里
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);							// 离开场景时，将遇怪标记还原
	virtual void OnRoleLeaveTeam(Role * role);											// 玩家离开队伍时，将玩家踢出场景
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);		// 战斗掉落符咒
	virtual void OnTeamDismiss(Role * leader_role, Team * team);						// 队伍解散时，从队伍列表里删除队伍
	virtual void OnTransport(Role *role, SpecialTransport * transport_obj);				// 坐上传送阵
	virtual void OnAOIEnterRole(Role * role, Role * target_role);						// 闯入视野，下发受咒信息
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_MAZE_RACER; }
	virtual bool IsAutoRecover() { return false; }
	virtual bool IsForceEncounterMineMonster() { return true; }
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader);

	// 符咒效果函数
	virtual bool GetBattleAnimationSpeedType(Role* role, ARG_OUT int* speed, ARG_OUT bool* is_force_speed);	// 0: 加速符 / 1: 减速符：连续一定次数，战斗速率设定
	virtual bool CanMoveOn(Role* role);									// 2: 定身符：连续一定时间，移动许可设定
	virtual bool CanEncounterMineMonster(Role * role);					// 3: 免战符：连续一定时间，遇敌许可设定
	virtual long long GetMineMonsterEncounterInterval(Role * role);		// 4: 激战符：连续一定次数，遇敌间隔设定

	// 场景逻辑函数
	void InitMazeRacerScene(int copy_id, int maze_id, int layer, time_t  start_time);
	void RequestUseAmulet(Role * role, int amulet_id);	// 使用符咒
	void RequestTransport(Role * role);					// 请求传送 - 在当前场景内传送
	void GetTeamIndexList(std::vector<int> & out_list);
	void SendEctypeInfoToAllRole(Protocol::SCActivityMazeRacerCopysInfo * msg, int send_len);
	void OnMRActivityFinish();		// 
	void SendAmuletInfo(Role * role, Role * target_role = NULL);
	void SendSceneInfo(Role * role);

	bool IsPassTeam(int uid_team);
	void OnTeamChangeLeader(Role * leader, int last_leader_id, Team * team);
	// GM 命令
	void GmMazeRacerOrder(Role * role, int op_type, int param1, int param2);

private:
	int m_is_finish;			//发奖励前设置为完成

	int m_copy_id;				// 拷贝序号
	int m_maze_id;				// 迷宫ID
	int m_layer;				// 层数

	time_t m_next_refresh_rank_timestamp;		// 下次刷新排名的时间戳
	bool m_need_refresh_rank;	// 是否需要刷新排名
	Posi trans_posi_list[MAZE_RACER_TRANSFER_TARGET_POSI_NUM];		// 随机传送目标坐标位置：0 代表下一层的门坐标

	std::vector<int> m_team_id_vec;				// 队伍ID列表
	std::set<int> m_pass_team_index;			// 通关队伍index

	std::map<int, time_t> m_team_enter_time;		//记录每个队伍进入时间，给客户端用
	time_t m_start_time;
};

#endif