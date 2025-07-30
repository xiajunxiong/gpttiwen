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
	bool is_challenge_monster;				// �Ƿ���ս�������
	bool is_team_challenge_monster;			// �Ƿ������ս����
	bool is_task_monster;					// �Ƿ�������
	bool is_guide_monster;					// �Ƿ�ָ���� ��ָ��������ͻ�����ͣս����
	bool is_branch_task_monster;			// �Ƿ�֧�����������
	bool is_advanced_task_monster;			// �Ƿ�������������
	std::vector<UInt16> dropid_list;
	bool captain_only_drop;
	std::vector<UInt16> captain_drop_list;
	std::vector<UInt16> decay_drop_list;	// ˥������
	int drop_times;			// һ�����Ե�����ٴ�
	int drop_item_id_checker; // �����������Ʒ�������һ�� ��0����û�� -1�������������Ʒ����һ�Σ�
	int monster_id_list[SIDE_CHARACTER_NUM];
	int kill_exp;
	long long all_monster_total_hp_num;
};

class MonsterGroupPool
{
public:
	static const int MAX_GROUP_ID = MAX_UINT16;
	static const int DROP_LIST_MAX_COUNT = 64;					// �����б�������64��
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

	//��ֹ�������ƣ��벻Ҫ���л����������������� ������������� ��Ϊ������ʱ��delete��ĳЩ��Ա������
	MonsterGroupPool(const MonsterGroupPool&);
	MonsterGroupPool& operator=(const MonsterGroupPool &);

	void Release();
	int InitMonsterGroupCfg(TiXmlElement *RootElement);
	bool ReadDropIDList(TiXmlElement *dropListElement, std::vector<UInt16> &dropid_list) const;

	MonsterGroupCfg* m_group_list[MAX_GROUP_ID];

	ItemDropMonsterGroupHash m_item_drop_monster_group_hash; // ����itemʱ��monster_group�����һ��
};
extern MonsterGroupPool* g_monster_group_pool;

#endif 