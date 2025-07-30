#ifndef __BBL_IMPL_CLOUD_ARENA_HPP__
#define __BBL_IMPL_CLOUD_ARENA_HPP__

#include "battle/battle_business_local.hpp"

namespace battlegameprotocol
{
	struct BattleGameStartFightAck;
	struct BattleGameFightResultAck;
	struct BattleGameRoleRunAway;
	struct BattleGameForceFinishFight;
}

struct RoleBattleData;
class BBLImplCloudArena : public BattleBusinessLocal
{
public:
	BBLImplCloudArena();
	virtual ~BBLImplCloudArena();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void OnBattlePrepare(int battle_mode, int role_count, Role** role_list, RoleBattleData** role_data_list);
	virtual void OnBattleStart(battlegameprotocol::BattleGameStartFightAck* ack);
	virtual void OnBattleFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnBattleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

protected:
	int m_attacker_uid;
	RoleBattleData m_attacker_battle_data;
};

#endif