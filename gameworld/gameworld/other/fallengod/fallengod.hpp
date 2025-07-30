#ifndef _FALLEN_GOD_HPP_
#define _FALLEN_GOD_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/bigdipperdef.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

class RoleModuleManager;
class FallenGod
{
public:
	FallenGod();
	~FallenGod();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr, const FallenGodParam &param, const FallenGodHelpParam &help_param);
	void GetInitParam(FallenGodParam * param, FallenGodHelpParam *help_param);
	void OnWeekChange();
	void OnBattleFinish(BattleData_Business* business_data, int round_num);
	void StarBattle(int sn, bool is_client_req = true);

	int GetTopSn() { return m_param.top_sn; }
	void OnHelpPass(int boss_index, int waves);
	int GetHelpPassCount(int boss_index);
	int GetBattleRound(int sn);
	bool IsPassSn(int sn);

	void SendInfo();
private:

	RoleModuleManager *m_module_mgr;
	FallenGodParam m_param;
	FallenGodHelpParam m_help_param;
};
#endif
