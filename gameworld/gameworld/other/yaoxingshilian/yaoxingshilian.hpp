#ifndef _YAO_XING_SHI_LIAN_HPP_
#define _YAO_XING_SHI_LIAN_HPP_

#include "servercommon/servercommon.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/yaoxingshiliandef.hpp"

class RoleModuleManager;
class YaoXingShiLian
{
public:
	YaoXingShiLian();
	~YaoXingShiLian();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr, const YaoXingShiLianParam &param);
	void GetInitParam(YaoXingShiLianParam *param);

	void OnRoleLogin();

	void OnBattleFinishWin(bool is_win);

	void OnStartBattle();
	void GetReward(int seq);
	void GetReward2(int seq);

	void OnBuyItemCfgCheck(int type, int seq);
	int OnBuyGiftItem(int type,int seq, int ARG_OUT * price);

	int GetPassBattleSeq() { return m_param.pass_battle_seq; }

	void SendInfo();
	int OnCmd(int chong_zhi_gold, long long union_id);
	void GmSetParam(int type, int param);
private:

	RoleModuleManager *m_module_mgr;
	
	YaoXingShiLianParam m_param;
};
#endif
