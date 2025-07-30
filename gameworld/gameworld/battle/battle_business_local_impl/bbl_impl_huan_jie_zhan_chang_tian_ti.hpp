#ifndef __BBL_IMPL_HUAN_JIE_ZHAN_CHANG_HPP__
#define __BBL_IMPL_HUAN_JIE_ZHAN_CHANG_HPP__

#include "battle/battle_business_local.hpp"

class BBLImplHuanJieZhanChang : public BattleBusinessLocal
{
public:
	BBLImplHuanJieZhanChang();
	virtual ~BBLImplHuanJieZhanChang();

	virtual void OnBattleFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnBattleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	void* operator new(size_t c);
	void operator delete(void* m);
};

#endif