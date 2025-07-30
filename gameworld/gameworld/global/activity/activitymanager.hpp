#ifndef __ACTIVITY_MANAGER_HPP__
#define __ACTIVITY_MANAGER_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/struct/global/activitydatalistparam.hpp"

#include <map>
#include <vector>
#include <set>

class Activity;
class Role;

class ActivityManager
{
public:
	static ActivityManager & Instance();
	static bool  IsCrossActivity(int activity_type);

	void Update(unsigned long interval, time_t now_second);
	void OnMonthChange();
	void OnWeekChange();
	void OnDayChange();

	void InitActivity(int activity_type, const ActivityData& data);
	void GetChangedActivityData(ActivityDataListParam* list_param);
	void GetActivityData(ActivityDataListParam* list_param);
	void ClearDirtyMark();

	void InitActivityRoleData(const ActivityRoleDataParam::DBActivityRoleDataItem & role_data_item);
	void SaveRoleData();

	bool IsActivityStandy(int activity_type);
	bool IsActivityOpen(int activity_type);
	bool IsActivityClose(int activity_type);

	void OnUserLogin(Role *role);	
	void OnUserLogout(Role *role);
	void OnUserLoginOriginGame(int role_id);
	void OnConnectCrossServerSucc();
	void OnGameServerConnect(std::set<int> server_id);

	void OnUserEnterScene(Role *role);
	void OnUserLeaveScene(Role *role);


	void OnQueryRoomStatusReq(Role *role, int activity_type);

	void OnForceToNextState(int activity_type, unsigned int * next_status_switch_timestamp = NULL);

	Activity * ForceGetActivity(int activity_type);

	void OnBattleServerDisconnect(int battle_server_id);
	void SyncAllActivityStatusHiddenToGameworld(int server_id = 0);
private:
	ActivityManager();
	~ActivityManager();

	Activity *m_activity_list[ACTIVITY_TYPE_MAX];
};

#endif // __ACTIVITY_MANAGER_HPP__

