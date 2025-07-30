#ifndef __NPC_MONSTER_H__
#define __NPC_MONSTER_H__

#include "obj/character/npc.h"
#include "npc/npcpool.h"

class NPCMonster : public NPC
{
public:
	NPCMonster();
	virtual ~NPCMonster();

	void* operator new(size_t c);
	void operator delete(void *m);

	int InitNPCMonster(int battle_mode, int monster_group_id, int head_monster_id);

	void SetBattleMode(int mode) { m_battle_mode = mode; }
	int GetMonsterGroupId() const { return m_monster_group_id; }
	int GetBattleMode() const { return m_battle_mode; }
	int GetHeadMonsterId() const { return m_head_monster_id; }

	bool IsMonsterLock() const { return m_is_lock; }
	void SetMonsterLock(bool is_lock) { m_is_lock = is_lock; }
	void SetBattleID(int battle_id) { m_battle_id = battle_id; }
	int GetBattleID() const { return m_battle_id; }
	bool IsNPCMineMonster() const { return NPC_MINE_MONSTER_NPC_ID == m_npc_id; }
	
	void SetParam1(int param1);
	int GetParam1();

	void SetLevel(int level);
	int GetLevel();

	void AddDeathNum();
	int GetDeathNum()const;

	void AddBattleTeamNum();
	void DecBattleTeamNum();
	int GetBattleTeamNum()const;
protected:
	int m_monster_group_id;
	int m_battle_mode;
	int m_head_monster_id;

	bool m_is_lock;
	int m_battle_id;
	int m_param1;

	int m_death_num;			//死亡次数
	int m_battle_team_num;		//正在挑战队伍数
	int m_use_level;
};

#endif