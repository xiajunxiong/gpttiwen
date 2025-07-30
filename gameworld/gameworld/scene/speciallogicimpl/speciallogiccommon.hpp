#ifndef __SPECIAL_LOGIC_COMMON_HPP__
#define __SPECIAL_LOGIC_COMMON_HPP__

#include "other/poker/pokerinterface.hpp"
#include "scene/speciallogic.hpp"
#include "servercommon/wildbossdef.hpp"
#include "protocol/msgshanhaiboss.hpp"
#include <map>
#include <set>


static const int WILD_BOSS_MSX_GSNETID_NUM = 1024;
static GSNetID wild_boss_gsnetidlist[WILD_BOSS_MSX_GSNETID_NUM];

class SpecialLogicCommon : public SpecialLogic, public PokerInterface
{
public:
	SpecialLogicCommon(Scene* scene);
	virtual ~SpecialLogicCommon();

	virtual void Update(unsigned long interval, time_t now_second);

	virtual bool CanDestroy() { return false; }
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	bool CreateWildBoss(int npc_monster_id, int cfg_seq, Posi *out_posi = NULL, int * monster_group_id = NULL);

//	virtual void FightFinishPoker(battlegameprotocol::BattleGameFightResultAck * ack, int index, Role *role, const void *load_data_arr[LOAD_DATA_MAX], int game_mode = GAME_MODE_DEFAULT);

	bool CreateShanHaiBoss(int npc_monster_id, int* out_npc_seq, Posi in_posi);
	bool CreateHundredGhostMonster(int npc_monster_id, int* out_npc_seq, Posi in_posi);

private:
	//野外boss 战斗完成
	void FieldBossOnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);

	//山海boss 战斗完成
	void ShanHaiBossOnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);	

	//百鬼夜行 战斗完成
	void HundredGhostOnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
};

#endif