#ifndef __ROLE_FIRST_KILL_HPP__
#define __ROLE_FIRST_KILL_HPP__

#include <map>
#include <vector>
#include "servercommon/struct/rolefirstkillparam.hpp"
#include "servercommon/servercommon.h"

class RoleModuleManager;
class RoleFirstKill
{
public:
	typedef std::map<int, RoleFirstKillInfo> FirstKillInfoMap;
	typedef std::map<int, char> DirtyGroupMap;

public:
	RoleFirstKill();
	~RoleFirstKill(); 

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* mgr, const RoleFirstKillParamList& list);
	
	bool GetChangedFirstKillInfo(RoleFirstKillParamList* list);
	void OnWeekChange();

	void OnKillMonsterGroup(int group_id, int team_member_num, int win_round, int team_level_high, int team_level_low, int average_level, int dead_role_num);
	void ClearDirtyMark();
	bool HasKillMonsterGroup(int group_id);
	bool HasWeekKiilMonsterGroup(int group_id);
private:
	const RoleFirstKillInfo* GetFirstKillInfo(int group_id);

	RoleModuleManager* m_mgr;
	FirstKillInfoMap m_info_map;
	DirtyGroupMap m_dirty_group_map;
	DirtyGroupMap m_snap_dirty_group_map;
};

#endif