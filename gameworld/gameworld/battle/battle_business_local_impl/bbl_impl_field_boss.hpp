#ifndef __BBL_IMPL_FIELD_BOSS_HPP__
#define __BBL_IMPL_FIELD_BOSS_HPP__

#include "battle/battle_business_local.hpp"

class BBLImplFieldBoss : public BattleBusinessLocal
{
public:
	BBLImplFieldBoss();
	virtual ~BBLImplFieldBoss();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void OnBattleStart(battlegameprotocol::BattleGameStartFightAck* ack);
	virtual void OnBattlePrepare(int battle_mode, int role_count, Role** role_list, RoleBattleData** role_data_list);
	virtual void OnBattleInstanceDeleted();
	virtual void OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

protected:
	void ReleaseBossLock();

};

#endif