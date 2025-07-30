#ifndef __SPECIAL_LOGIC_MAZE_RACER_WAITING_HALL_HPP__
#define __SPECIAL_LOGIC_MAZE_RACER_WAITING_HALL_HPP__

#include "scene/speciallogic.hpp"
#include "globalconfig/globalconfig.h"

#include <map>
#include <set>

struct MazeRaceDeleteData
{
	MazeRaceDeleteData() :box_id(0), obj_id(0) {}
	MazeRaceDeleteData(int _box_id, ObjID _obj_id) : box_id(_obj_id), obj_id(_obj_id) {}
	int box_id;
	ObjID obj_id;
};


class SpecialLogicMazeRacerWaitingHall: public SpecialLogic
{
public:
	SpecialLogicMazeRacerWaitingHall(Scene* scene);
	virtual ~SpecialLogicMazeRacerWaitingHall();
	void Reset();

	virtual bool CanDestroy();
	virtual void Update(unsigned long interval, time_t now_second);	// 时间戳到达，传送，并刷新时间戳
	virtual void OnRoleEnterScene(Role * role);						// 检查一次场景人数，超过 50 个就传送进副本，并刷新时间戳
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);		// 队长离开场景，把存储的数据删掉
	virtual void OnRoleLeaveTeam(Role * role);						// 离开队伍，把玩家踢出去
	virtual void OnTeamDismiss(Role * leader_role, Team * team);	// 队伍解散，踢出队员，删除队伍队列
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader);
	virtual bool CanInvitationRole(Role * role, int uid);
	virtual bool OnOpenTreasureChest(Role *role, TreasureChest * chest_obj);

	void OnActivityOpen();
	void OnActivityEnd();

	void GmTransportImmidiately();
private:
	void TransAllTeamIntoEctype();
	void SendWaitingHallInfo(Role * role = NULL);
	void OnRefreshChest();											// 刷新宝箱
	void ExtinctAllChest();											// 清理宝箱
	bool RandAndPutChestAwardInBag(Role * role, int chest_id, bool is_leader = false);		// 随机获取一次宝箱奖励，并将奖励放入背包

	bool m_need_send_info;
	time_t m_next_send_timestamp;


	int m_target_maze_id;
	time_t m_next_begin_timestamp;		// 下次传送的时间戳
	std::set<int> m_team_index_list;		// 队伍信息列表；键 - 队伍Index

	bool m_has_role_enter;

	std::map<int, std::set<ObjID> > m_chest_list;					// 场景中存在的宝箱的 ObjID
	time_t next_box_refresh_tamp;									//宝箱刷新时间戳
	int m_box_num;													//未开宝箱数量
	bool is_refresh_box_flag;										//是否刷新宝箱
};

#endif