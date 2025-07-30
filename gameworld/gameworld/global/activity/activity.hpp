#ifndef __ACTIVITY_HPP__
#define __ACTIVITY_HPP__

#include "iclockmodule.h"

#include "servercommon/struct/global/activitydatalistparam.hpp"
#include "servercommon/serverdef.h"
#include <set>

class ActivityManager;
class Role;

struct ActivityPerRoleDataNode
{
	ActivityPerRoleDataNode()
	{
		is_dirty = false;
		is_new_create = false;
		is_invalid = false;
		memset(&data, 0, sizeof(data));
	}

	bool is_dirty;
	bool is_new_create;		//数据库是否有对应记录
	bool is_invalid;
	ActivityRoleData data;
};

struct RetActivityRoleData
{
	RetActivityRoleData()
	{
		role_id = 0;
		memset(&data, 0, sizeof(data));
	}

	int role_id;
	ActivityRoleData data;
};

class Activity
{
public:
	Activity(ActivityManager *activity_manager, int m_activity_type);
	virtual ~Activity();

	virtual bool IsNeedSave() { return false; }		//活动自身的数据是否保存，像开启时间关闭时间之类的
	virtual void Init(const ActivityData &arg_data);
	virtual void GetInitParam(ActivityData & arg_data);
	virtual void OnBattleServerDisconnect(int battle_server_id);
	void SetDirty(); //活动自身数据设置脏
	void ClearDirtyMark() { m_is_dirty = false; }
	bool IsDirty() { return m_is_dirty; }


	virtual void OnActivityStatusChange(int from_status, int to_status);

	virtual void OnLeaveActivity(Role *role) {}

	virtual void Update(unsigned long interval, time_t now_second); //目前ActivityManager::IsCrossActivity 返回true 不会调用update
	virtual void OnCalcNextSwtichTimeByClose();						//活动处于关闭或无切换时间时,计算切换时间  --(不是周活动可以通过复写该函数来开启活动)

	virtual void OnUserLogin(Role *role);
	virtual void OnUserLogout(Role *role);
	virtual void OnUserEnterScene(Role *role);
	virtual void OnUserLeaveScene(Role *role);

	virtual void OnConnectCrossServerSucc();
	virtual void OnUserLoginOriginGame(int role_id);			//隐藏服 并且 是跨服活动 才会调用，
	virtual void OnGameServerConnect(std::set<int> server_id);	//隐藏服 并且 是跨服活动 才会调

	virtual void SetActivityState(const SyncActivityStatus & as);
	void ForceToNextState(unsigned int * next_status_switch_timestamp = NULL);
	void ForceToCloseState();
	int GetActivityStatus() { return m_activity_status; }

	void OnQueryRoomStatusReq(Role *role);

	int GetActivityType() { return m_activity_type; }
	unsigned int GetNextActivityBeginTime();
	int GetActivityLimitLevel(int activity_type);

	bool IsActivityOpen();
	bool IsActivityStandy();
	bool IsActivityClose();

	virtual void OnDayChange();
	virtual void OnWeekChange();
	virtual void OnMonthChange();

	//-------------------------------活动玩家数据函数---------------------------------------
	virtual void InitActivityRoleData(int role_id, const ActivityRoleData & role_data);
	virtual void SaveRoleData(bool is_stop_server = false);	
	void SetRoleData(int role_id, const ActivityRoleData & data);
	ActivityRoleData * GetRoleData(int role_id, bool is_auto_create = true);
	void SetRoleDataChange(int role_id);
	void ClearAllRoleData();
	void RemoveRoleData(int role_id);
	void BeginRoleDataForeach();
	bool GetNextRoleData(RetActivityRoleData ARG_OUT  & role_data, bool is_reset = false);
	//----------------------------------------------------------------------
	void SyncActivityStatusHiddenToGameworld(int server_id = 0);  //同步隐藏服活动状态（开启关闭状态）到游戏服

	void GetActivityStausInfo(std::string & out_str);
protected:
	void NotfiyActivityStatus(bool is_broadcast = false, Role *role = NULL);
	virtual void SendActivtyNotify(int notify_status);

	//-------------------------------活动玩家数据函数---------------------------------------
	void GetChangeRoleDataList(ActivityRoleDataParam & ARG_OUT role_data_list, bool is_clear = true);
	void ClearActivityRoleChangeFlag(const ActivityRoleDataParam & role_data_param);
	//----------------------------------------------------------------------

	ActivityManager * m_activity_manager;
	int m_activity_type;

	int m_activity_status;
	unsigned int m_next_status_switch_time;

	int m_next_open_wday;
	StatusTimeCfgList m_statustime_list_cfg;

	bool m_is_dirty;		//活动自身数据是否改变的标志

	bool m_already_notify_standy;		// 准备阶段前五分钟提示准备开始

private:
	//--------------------------
	std::map<int, ActivityPerRoleDataNode>::const_iterator m_role_data_iter;
	time_t m_next_role_data_save_time;
	bool m_is_role_data_change;
	std::map<int, ActivityPerRoleDataNode> m_role_data;
	//-------------------------------
};

#endif // __ACTIVITY_HPP__

