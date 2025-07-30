#ifndef _BIG_DIPPER__HPP_
#define _BIG_DIPPER__HPP_

#include "servercommon/servercommon.h"
#include "servercommon/bigdipperdef.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

class RoleModuleManager;
class BigDipper
{
public:
	BigDipper();
	~BigDipper();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr,const BigDipperParam &param, const BigDipperHelpParam &help_param);
	void GetInitParam(BigDipperParam * param, BigDipperHelpParam * help_param);
	void OnWeekChange();
	void OnBattleFinish(BattleData_Business* business_data, int round_num);

	int GetBattleRound(int sn);
	void StarBattle(int sn, bool is_client_req = true);
	void GetFirstPassReward(int sn);
	void GetReward(int sn, int reward_idx);
	void SendInfo();

	bool HasPassSN(int sn);

	void OnHelpPass(int boss_index, int waves);
	bool IsFirstPass(int boss_index);
	bool IsWeekPass(int boss_index);
	int GetHelpPassCount(int boss_index);

private:

	RoleModuleManager *m_module_mgr;
	BigDipperParam m_param;
	BigDipperHelpParam m_help_param;
};
#endif
