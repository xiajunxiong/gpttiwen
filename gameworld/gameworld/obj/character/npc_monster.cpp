#include "npc_monster.h"

NPCMonster::NPCMonster() : NPC(Obj::OBJ_TYPE_NPC_MONSTER), m_monster_group_id(0), 
	m_battle_mode(0), m_head_monster_id(0), m_is_lock(false), m_battle_id(0), m_param1(0), m_death_num(0), m_battle_team_num(0)
{
	m_use_level = 0;
}

NPCMonster::~NPCMonster()
{

}

int NPCMonster::InitNPCMonster(int battle_mode, int monster_group_id, int head_monster_id)
{
	m_battle_mode = battle_mode;
	m_monster_group_id = monster_group_id;
	m_head_monster_id = head_monster_id;

	return 0;
}

void NPCMonster::SetParam1(int param1)
{
	m_param1 = param1;
}

int NPCMonster::GetParam1()
{
	return m_param1;
}

void NPCMonster::SetLevel(int level)
{
	m_use_level = level;
}

int NPCMonster::GetLevel()
{
	return m_use_level;
}

void NPCMonster::AddDeathNum()
{
	m_death_num++;
}

int NPCMonster::GetDeathNum() const
{
	return m_death_num;
}

void NPCMonster::AddBattleTeamNum()
{
	m_battle_team_num++;
}

void NPCMonster::DecBattleTeamNum()
{
	if (0 >= m_battle_team_num) return;
	m_battle_team_num--;
}

int NPCMonster::GetBattleTeamNum() const
{
	return m_battle_team_num;
}
