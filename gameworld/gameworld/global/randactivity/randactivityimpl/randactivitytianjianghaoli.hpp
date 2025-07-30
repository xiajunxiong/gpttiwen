#ifndef __RAND_ACTIVITY_TIAN_JIANG_HAO_LI_HPP__
#define __RAND_ACTIVITY_TIAN_JIANG_HAO_LI_HPP__

#include "global/randactivity/randactivity.hpp"
#include <map>
#include <vector>
#include "npc/npcmanager.h"
#include "servercommon/roleactivity/ratianjianghaoliparam.hpp"


class RandActivityTianJiangHaoLi : public RandActivity
{
public:
	RandActivityTianJiangHaoLi(RandActivityManager *activity_manager);
	~RandActivityTianJiangHaoLi();

	virtual void Init(const ActivityData &arg_data);
	virtual void GetInitParam(ActivityData & arg_data);

	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	bool IsReceiveBox(Role* role, NPCGather* gahter);
	bool ReceiveBoxOp(Role* role, NPCGather* gather);
	bool IsRefreshBoxTime();

	virtual bool CanSyncActivityDataToCross() { return false; }

	void GmSetTime(int begin_time, int end_time);
private:
	void SetBoxTime();
	void RefreshBox();
	void ClearBox();
	void SendInfo();

	time_t ForHourMinuteGetTimestamp(int hour_minute);

	std::map<int, std::vector<ObjID> > m_scene_box_map;			//天降好礼各场景宝箱

	RATianJiangHaoLiRandParam m_param;

	bool is_start_time;
};

#endif
