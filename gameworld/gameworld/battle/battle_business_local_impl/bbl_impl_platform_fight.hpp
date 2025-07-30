#ifndef __BBL_IMPL_PLATFORM_FIGHT_HPP__
#define __BBL_IMPL_PLATFORM_FIGHT_HPP__

#include "battle/battle_business_local.hpp"

class BBLImplPlatformFight : public BattleBusinessLocal
{
public:
	BBLImplPlatformFight();
	virtual ~BBLImplPlatformFight();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void OnBattleFinish(battlegameprotocol::BattleGameFightResultAck* ack);

protected:
	
};

#endif