#ifndef __RAND_ACTIVITY_MO_ZU_ZAI_NA_LI_HPP__
#define __RAND_ACTIVITY_MO_ZU_ZAI_NA_LI_HPP__

#include "global/randactivity/randactivity.hpp"
#include <vector>

class RandAcitivityMoZuZaiNaLi : public RandActivity
{
public:
	RandAcitivityMoZuZaiNaLi(RandActivityManager *activity_manager);
	~RandAcitivityMoZuZaiNaLi();

	struct MonsterInfo
	{
		MonsterInfo() :npc_seq(0), monster_seq(-1), monster_scene(0), monster_objid(INVALID_OBJ_ID) {}
		int npc_seq;
		int monster_seq;
		int monster_scene;
		ObjID monster_objid;
	};

	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);

	void OnRefreshMonster(time_t now_second);
	void OnFightFinish(Role * role, bool is_win);
	void CleanMonster();
	void GmRefreshMonster() { next_refresh_monster_tamp = EngineAdapter::Instance().Time(); }
	virtual bool CanSyncActivityDataToCross() { return false; }

private:
	bool check_init;
	long long next_refresh_monster_tamp;

	std::vector<MonsterInfo> m_monster_vec;
};


#endif