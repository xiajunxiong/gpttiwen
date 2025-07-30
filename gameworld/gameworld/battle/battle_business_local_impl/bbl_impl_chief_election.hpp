#ifndef __BBL_IMPL_CHIEF_ELECTION_HPP__
#define __BBL_IMPL_CHIEF_ELECTION_HPP__

#include "battle/battle_business_local.hpp"

class BBLImplChiefElection : public BattleBusinessLocal
{
public:
	BBLImplChiefElection();
	virtual ~BBLImplChiefElection();

	virtual void OnBattleFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnBattleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	void* operator new(size_t c);
	void operator delete(void* m);
};

#endif