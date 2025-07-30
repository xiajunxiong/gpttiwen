#ifndef __MONSTER_GROUP_POOL_HPP__
#define __MONSTER_GROUP_POOL_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include <vector>
#include <map>

struct MonsterGroupCfg
{
	MonsterGroupCfg() : monster_group_id(0), head_monster_id(0), is_challenge_monster(false), is_task_monster(false),
		is_guide_monster(false), is_branch_task_monster(false), is_advanced_task_monster(false), 
		captain_only_drop(false), drop_times(0), drop_item_id_checker(0), kill_exp(0)
		, all_monster_total_hp_num(0)
	{
		memset(monster_id_list, 0, sizeof(monster_id_list));
	}

	int monster_group_id;
	int head_monster_id;
	bool is_challenge_monster;				// 是否挑战任务怪物
	bool is_team_challenge_monster;			// 是否组队挑战怪物
	bool is_task_monster;					// 是否剧情怪物
	bool is_guide_monster;					// 是否指引怪 （指引怪允许客户端暂停战斗）
	bool is_branch_task_monster;			// 是否支线任务怪物组
	bool is_advanced_task_monster;			// 是否进阶任务怪物组
	std::vector<UInt16> dropid_list;
	bool captain_only_drop;
	std::vector<UInt16> captain_drop_list;
	std::vector<UInt16> decay_drop_list;	// 衰减奖励
	int drop_times;			// 一共可以掉落多少次
	int drop_item_id_checker; // 掉落了这个物品才算掉落一次 （0代表没有 -1代表掉落任意物品都算一次）
	int monster_id_list[SIDE_CHARACTER_NUM];
	int kill_exp;
	long long all_monster_total_hp_num;
};

class MonsterGroupPool
{
public:
	static const int MAX_GROUP_ID = MAX_UINT16;
	static const int DROP_LIST_MAX_COUNT = 64;					// 掉落列表不允许超过64个
	static const int MAX_DROP_ITEM_COUNT = 64;

	typedef std::map<int, int> ItemDropMonsterGroupHash;
	friend class AsyncReloadMonster;

public:
	static MonsterGroupPool& GetInstance();

	bool Init(const std::string &configname, std::string *err);
	static bool Reload(const std::string &configname, std::string *err);

	const MonsterGroupCfg* GetMonsterGroupCfg(int monster_group_id);
	int GetMonsterGroupIDByLimitedDropItemID(int item_id);
	long long GetMonsterGroupAllMonsterTotalHp(int monster_group_id);
	int GetFastForwardRoundByMonsterGroup(int group_id, int role_level);
private:
	MonsterGroupPool();
	~MonsterGroupPool();

	//禁止拷贝复制（请不要公有化以下这两个函数， 除非做好深拷贝， 因为在析构时会delete掉某些成员变量）
	MonsterGroupPool(const MonsterGroupPool&);
	MonsterGroupPool& operator=(const MonsterGroupPool &);

	void Release();
	int InitMonsterGroupCfg(TiXmlElement *RootElement);
	bool ReadDropIDList(TiXmlElement *dropListElement, std::vector<UInt16> &dropid_list) const;

	MonsterGroupCfg* m_group_list[MAX_GROUP_ID];

	ItemDropMonsterGroupHash m_item_drop_monster_group_hash; // 掉落item时，monster_group算掉落一次
};
extern MonsterGroupPool* g_monster_group_pool;

#endif 