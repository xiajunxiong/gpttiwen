#ifndef __RAND_ACTIVITY_XUNMENG_HUDIE_HPP__
#define __RAND_ACTIVITY_XUNMENG_HUDIE_HPP__

#include "global/randactivity/randactivity.hpp"
#include <vector>

class RandAcitivityXunMengHuDie : public RandActivity
{
public:
	RandAcitivityXunMengHuDie(RandActivityManager *activity_manager);
	~RandAcitivityXunMengHuDie();

	struct NpcInfo
	{
		NpcInfo() :npc_seq(0), scene_id(0), npc_objid(INVALID_OBJ_ID) {}
		int npc_seq;
		int scene_id;
		ObjID npc_objid;
	};

	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);

	void OnRefreshNpc(time_t now_second);
	void OnComplete(Role * role, bool is_win, ObjID npc_objid);
	void CleanNpc();

private:
	bool check_init;
	long long next_refresh_npc_tamp;

	std::vector<NpcInfo> m_npc_vec;
};


#endif