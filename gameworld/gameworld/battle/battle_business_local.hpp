#ifndef __BATTLE_BUSSINESS_LOCAL_HPP__
#define __BATTLE_BUSSINESS_LOCAL_HPP__

#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/internalprotocal/battleprotocol.h"

struct BattleLocalInfo;
struct RoleBattleData;
class Role;

class BattleBusinessLocal
{
public:
	BattleBusinessLocal();
	virtual ~BattleBusinessLocal();

	static BattleBusinessLocal* CreateBBLImpl(int battle_mode);

	virtual void OnBattlePrepare(int battle_mode, int role_count, Role** role_list, RoleBattleData** role_data_list);
	
	virtual void OnBattleStart(battlegameprotocol::BattleGameStartFightAck* ack);
	virtual void OnBattleFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnBattleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	virtual void OnBattleInstanceDeleted(); // 不要在这个函数中给予奖励

	void SetBattleLocalInfo(BattleLocalInfo* local_info) { m_battle_local_info = local_info; }

protected:

	BattleLocalInfo* m_battle_local_info;
};

#endif