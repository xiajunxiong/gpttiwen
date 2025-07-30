#ifndef __BBL_IMPL_COLOSSEUM_HPP__
#define __BBL_IMPL_COLOSSEUM_HPP__

#include "battle/battle_business_local.hpp"

class BBLImplColosseum : public BattleBusinessLocal
{
public:
	BBLImplColosseum();
	virtual ~BBLImplColosseum();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight* ack);
};

#endif