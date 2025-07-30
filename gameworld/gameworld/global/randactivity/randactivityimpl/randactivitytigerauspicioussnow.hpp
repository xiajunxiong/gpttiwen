#ifndef __RAND_ACTIVITY_TIGER_AUSPICIOUS_SNOW_HPP__
#define __RAND_ACTIVITY_TIGER_AUSPICIOUS_SNOW_HPP__

#include "global/randactivity/randactivity.hpp"
#include "npc/npcmanager.h"

class RandActivityTigerAuspiciousSnow :public RandActivity
{
public:
	RandActivityTigerAuspiciousSnow(RandActivityManager *activity_manager);
	~RandActivityTigerAuspiciousSnow();

	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);

	bool IsReceiveSnow(Role* role, NPCGather* gahter);
	bool ReceiveSnowOp(Role* role, NPCGather* gather);

private:
	void RefreshSnow();
	void ClearSnow();

	std::map<int, std::vector<ObjID> > m_scene_snow_map;		// ÈðÑ©·áÄê¸÷³¡¾°Ñ©¶Ñ

	unsigned int next_refresh_timestamp;
};


#endif