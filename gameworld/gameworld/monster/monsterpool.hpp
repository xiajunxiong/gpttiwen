#ifndef __MONSTER_POOL_HPP__
#define __MONSTER_POOL_HPP__

#include "servercommon/servercommon.h"

class MonsterCfg
{
public:
	MonsterCfg() : m_monster_id(0), m_level(0), m_race(0), m_max_hp(0), m_catch_pet_id(0), m_drop_card_id(0), 
		m_drop_card_posibility(0), m_pet_refresh_probablity(0), m_drop_element_probability(0)
	{
		memset(m_name, 0, sizeof(m_name));
		memset(m_element_list, 0, sizeof(m_element_list));
	}

	bool Init(const std::string &configname, std::string &err);
	bool CanCatch() const { return m_catch_pet_id > 0; }
	int GetCatchPetId() const { return m_catch_pet_id; }
	
	bool IsRefreshLevelOneMonster() const;
	bool IsDropCard() const;

	int m_monster_id;
	GameName m_name;
	short m_level;
	short m_race;
	int m_max_hp;

	int m_catch_pet_id;						// 捕捉的宠物ID  0为不能捕捉
	
	int m_drop_card_id;						// 掉落图鉴卡id
	short m_drop_card_posibility;			// 掉落图鉴卡概率

	short m_pet_refresh_probablity;			// 刷一级怪的概率 万分比

	int m_drop_element_probability;			// 掉元素碎片概率

	int m_element_list[ELEMENT_TYPE_MAX];		// 怪物元素属性
};

class MonsterPool
{
public:
	static const int MAX_MONSTER_ID = 100000;
	friend class AsyncReloadMonster;

public:
	static MonsterPool& GetInstance();

	bool Init(const std::string &config_dir, const std::string& config_name, std::string *err);
	static bool Reload(const std::string &config_dir, const std::string& config_name, std::string *err);
	bool ReloadOneMonster(const std::string &dir, int monster_id, std::string *err);

	const MonsterCfg* GetMonsterCfg(int monster_id);
	bool IsMonsterExist(int monster_id);

private:
	void Release();

	MonsterCfg* m_monster_list[MAX_MONSTER_ID];


	INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(MonsterPool);
};
extern MonsterPool* g_monster_pool;

#endif