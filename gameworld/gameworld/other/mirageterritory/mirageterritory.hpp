#ifndef _MIRAGE_TERRITORY_HPP_
#define _MIRAGE_TERRITORY_HPP_

#include "servercommon/servercommon.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/mirageterritorydef.h"

class RoleModuleManager;
class MirageTerritory
{
public:
	MirageTerritory();
	~MirageTerritory();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr, const MirageTerritoryParam & param);
	void GetInitParam(MirageTerritoryParam * param);
	void GetInfoParam(RoleMirageTerritoryPartnerInfoParam * param);
	
	void OnBattleFinishWin(BattleData_Business* business_data, int round_num);
	void OnRoleLogin();
	void OnDayChange();

	void OnOpreat(int req_type, int param1, int param2, int param3, int param4);
	void StarBattle(int area);

	int GetNowBattleSeq(int area);
	int GetTopBattleSeq(int area);
	int GetMinAllTopBattleSeq();	//获取此时所有分区均达关卡

	void OneKeyPass(int area, int partner_id_1, int partner_id_2,int partner_id_3);//扫荡
	
	void SendInfo(int area);
	void SendAllInfo();
	//布阵
	void SetPartnerBattleInfo(int programme_idx, int partner_id_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM], int partner_posi_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM]);
	void SendPartnerBattleInfo(int programme_idx);
private:

	RoleModuleManager *m_module_mgr;
	MirageTerritoryParam m_param;
};
#endif
