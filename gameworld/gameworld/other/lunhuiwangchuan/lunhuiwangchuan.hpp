#ifndef _LUN_HUI_WANG_CHUAN_HPP_
#define _LUN_HUI_WANG_CHUAN_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/lunhuiwangchuandef.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

class RoleModuleManager;
class LunHuiWangChuan
{
public:
	LunHuiWangChuan();
	~LunHuiWangChuan();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr,const LunHuiWangChuanParam &param);
	void GetInitParam(LunHuiWangChuanParam * param);

	void OnRoleLogin();
	void Update(time_t now_second);
	void OnDayChange(int old_day, int now_day);

	void OnBattleFinishWin(bool is_win);
	void OnPassSeqChange();

	void OnStartBattle();
	void GetReward(int reward_seq);
	int GetLevel() { return m_param.start_level; }
	int GetPassTimes() { return m_param.pass_times; }

	int GetPassIDX() { return m_param.set_seq; }

	void RefreshNow();
	void SendInfo();
private:

	RoleModuleManager *m_module_mgr;
	LunHuiWangChuanParam m_param;
};
#endif
