#ifndef __BBL_IMPL_WORLD_BOSS3_HPP__
#define __BBL_IMPL_WORLD_BOSS3_HPP__

#include "battle/battle_business_local.hpp"

class BBLImplWorldBoss3 : public BattleBusinessLocal
{
public:
	BBLImplWorldBoss3();
	virtual ~BBLImplWorldBoss3();

	virtual void OnBattleFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	void* operator new(size_t c);
	void operator delete(void* m);
};

#endif