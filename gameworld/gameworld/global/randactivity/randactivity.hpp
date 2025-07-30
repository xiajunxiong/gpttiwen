#ifndef __RAND_ACTIVITY_HPP__
#define __RAND_ACTIVITY_HPP__

#include "iclockmodule.h"

#include "servercommon/activitydef.hpp"
#include "servercommon/serverdef.h"

#include "engineadapter.h"
#include <string>

namespace Protocol
{
	class SCActivityStatus;
}

class Role;
class RandActivityManager;
struct RandActivityOpenDetail;
class RandActivity
{
public:
	RandActivity(RandActivityManager *rand_activity_manager, int activity_type);
	virtual ~RandActivity();

	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);

	virtual void Init(const ActivityData &arg_data);
	virtual void GetInitParam(ActivityData & arg_data);

	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid) {}
	virtual void OnWeekChange() {}
	virtual void OnMonthChange() {}

	virtual void OnUserLogin(Role *user);
	virtual void OnUserLogout(Role *user) {}

	bool CalActivityOpenTime(const RandActivityOpenDetail* open_cfg, std::string& error_msg);

	virtual void SetRandActivityTime(unsigned int begin_time, unsigned int end_time, int open_type, const char* reason);
	void OnForceToNextState();

	void SyncActivityStatusToGameworld();

	int GetActivityType() { return m_rand_activity_type; }
	int GetStatus() { return m_rand_activity_status; }

	void SetEndTime(unsigned int end_timestamp, bool is_notice = false, bool is_broadcast = false);
	unsigned int GetBeginTime() const { return m_begin_time; }
	unsigned int GetEndTime() const { return m_end_time; }

	void SetDirty() { m_is_dirty = true; }
	void ClearDirtyMark() { m_is_dirty = false; }
	bool IsDirty() { return m_is_dirty; }

	virtual void SyncActivityDataFromCross(CrossRandActivityDataParam &param);
	
	virtual bool CanSyncActivityDataToCross() { return true; }
	void SendActivityDataToCross();

	void GmNotfiyRandActivityStatus();
	void GetRandActivityStausInfo(std::string & out_str);

	void OnRoleParticipateRA(Role* role, const char* func);
	void NotifyRandActivityStatusToHiddenUser(int uid);
	void SendRAStatusToSpecificServerUsers(Protocol::SCActivityStatus* msg);
protected:
	void NotfiyRandActivityStatus(bool is_broadcast = false, Role *user = NULL);
	void AssembleActivityStatusMsg(Protocol::SCActivityStatus* msg);
	void QueryOriginServerRAStatus(Role* user);
	void NotifyOriginServerRAStatusToUsersInHidden();

	RandActivityManager *m_rand_activity_manager;

	int m_rand_activity_type;
	int m_rand_activity_status;

	int m_open_type;														// 随机活动开启类型 RAND_ACTIVITY_OPEN_TYPE

	unsigned int m_begin_time;
	unsigned int m_end_time;

	bool m_is_dirty;
};

#endif // __SPECIAL_ACTIVITY_HPP__
