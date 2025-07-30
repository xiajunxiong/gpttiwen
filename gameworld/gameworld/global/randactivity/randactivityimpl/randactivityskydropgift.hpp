#ifndef __RANDACTIVITY_SKY_DROP_GIFT_HPP__
#define __RANDACTIVITY_SKY_DROP_GIFT_HPP__

#include "global/randactivity/randactivity.hpp"
#include <map>
#include <vector>
#include "npc/npcmanager.h"

class Role;

class RandActivitySkyDropGift : public RandActivity
{
public:
	RandActivitySkyDropGift(RandActivityManager *activity_manager);
	~RandActivitySkyDropGift();

	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);
	void GetBoxReward(Role * role,NPCGather * gather);

	virtual bool CanSyncActivityDataToCross() { return false; }
private:
	void RefreshBox();
	void ClearBox();
	std::map<int, std::vector<ObjID> > m_box_scene_map;//各场景宝箱
	time_t next_refresh_tamp;							// 下次刷新时间戳
};

#endif