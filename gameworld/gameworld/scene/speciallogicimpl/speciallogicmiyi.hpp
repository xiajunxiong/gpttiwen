#ifndef __SPECIAL_LOGIC_MIYI_HPP__
#define __SPECIAL_LOGIC_MIYI_HPP__

#include "other/poker/pokerinterface.hpp"
#include "servercommon/miyidef.hpp"
#include "scene/speciallogic.hpp"
#include <map>

static const int MSX_GSNETID_NUM = 1024;
static GSNetID gsnetidlist[MSX_GSNETID_NUM];

class Role;
class SpecialLogicMiYi : public SpecialLogic , public PokerInterface
{
public:
	typedef std::map<int, ObjID> NPCMonsterHash;

public:
	SpecialLogicMiYi(Scene* scene);
	virtual ~SpecialLogicMiYi();

	virtual void OnRoleEnterScene(Role *role);

	virtual void Update(unsigned long interval, time_t now_second);

	virtual bool CanDestroy() { return true; }
	virtual bool IsLevelOneMonsterOccur() { return false; }
	virtual bool IsDropCard() { return false; }

	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader) { return true; }

//	virtual void GetPoker(int role_uid, int poker_index, int reson = POKER_REASON_BY_SELF, int game_mode = GAME_MODE_DEFAULT);
//	virtual void FightFinishPoker(battlegameprotocol::BattleGameFightResultAck * ack, int index, Role *role, const void *load_data_arr[LOAD_DATA_MAX], int game_mode = GAME_MODE_DEFAULT);
	void SendPokerRoleLevelReward();   

private:
	void RefreshNPCMonster();
	void SendSceneInfo(Role * role = NULL);

	bool m_refreshed_monster;
	bool m_is_finish;
 	int m_npc_monster_count;						//现存明怪数
	unsigned int m_kill_flag;						//场景中怪物击杀标识(二进制) 0:未击杀  1:已击杀

	MiYiPoker m_myp[SIDE_MAX_ROLE_NUM];				// 是否可以翻牌领取奖励
	//short pokker_index[SIDE_MAX_ROLE_NUM + 1];	// 判断如果都不翻牌的情况避免全部默认选第一张牌，而是按顺序选
};

#endif