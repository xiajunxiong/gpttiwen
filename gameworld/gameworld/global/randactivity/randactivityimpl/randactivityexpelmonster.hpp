#ifndef __RAND_ACTIVITY_EXPEL_MONSTER_HPP__
#define __RAND_ACTIVITY_EXPEL_MONSTER_HPP__

#include "global/randactivity/randactivity.hpp"
#include <vector>

class RandAcitivityExpelMonster : public RandActivity
{
public:
	RandAcitivityExpelMonster(RandActivityManager *activity_manager);
	~RandAcitivityExpelMonster();
	struct ExpelMonsterInfo
	{
		ExpelMonsterInfo():npc_seq(0),monster_seq(-1), monster_scene(0),monster_objid(INVALID_OBJ_ID){}
		int npc_seq;
		int monster_seq;
		int monster_scene;
		ObjID monster_objid;
	};
	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnUserLogin(Role *user);

	void OnRefreshMonster(time_t now_second);
	void OnFightFinish(Role * role);
	void CleanMonster();
	void GmRefreshMonster() { next_refresh_monster_tamp = EngineAdapter::Instance().Time(); }
	virtual bool CanSyncActivityDataToCross() { return false; } 

	bool check_init;
	long long next_refresh_monster_tamp;
	int m_monster_scene_id;//妖兽所在场景
	std::vector<ExpelMonsterInfo> m_monster_vec;//捣乱妖兽信息
};


#endif