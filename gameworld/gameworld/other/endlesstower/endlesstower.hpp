#ifndef _ENDLESS_TOWER_HPP_
#define _ENDLESS_TOWER_HPP_

#include "servercommon/servercommon.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

class RoleModuleManager;
class EndlessTower
{
public:
	EndlessTower();
	~EndlessTower();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr,const int & battle_seq, const unsigned int & battle_tamp_param);
	void GetInitParam(int * param, unsigned int * battle_tamp_param);
	void OnRoleLogin();
	
	void OnBattleFinishWin(BattleData_Business* business_data, int round_num);

	void OnOpreat(int req_type, int param1, int param2);

	int GetBattleSeq() { return battle_seq; }
	unsigned int GetBattleTamp() { return battle_tamp; }

	void StarBattle();
	void SendInfo();

	void GMsetBattleSeq(int seq) { battle_seq = seq; }
private:

	RoleModuleManager *m_module_mgr;
	int battle_seq;
	unsigned int battle_tamp;
};
#endif
