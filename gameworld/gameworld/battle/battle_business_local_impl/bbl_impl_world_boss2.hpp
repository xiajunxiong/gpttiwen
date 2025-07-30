#ifndef __BBL_IMPL_WORLD_BOSS_2_HPP__
#define __BBL_IMPL_WORLD_BOSS_2_HPP__

#include "battle/battle_business_local.hpp"

class BBLImplWorldBoss2 : public BattleBusinessLocal
{
public:
	BBLImplWorldBoss2();
	virtual ~BBLImplWorldBoss2();

	virtual void OnBattleFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	void* operator new(size_t c);
	void operator delete(void* m);
};

#endif