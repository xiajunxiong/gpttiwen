#ifndef __BBL_IMPL_WORLD_BOSS_HPP__
#define __BBL_IMPL_WORLD_BOSS_HPP__

#include "battle/battle_business_local.hpp"

class BBLImplWorldBoss : public BattleBusinessLocal
{
public:
	BBLImplWorldBoss();
	virtual ~BBLImplWorldBoss();

	virtual void OnBattleFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	void* operator new(size_t c);
	void operator delete(void* m);
};

#endif