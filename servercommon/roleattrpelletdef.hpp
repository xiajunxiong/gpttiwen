#ifndef __ROLE_ATTR_PELLET_DEF_HPP__
#define __ROLE_ATTR_PELLET_DEF_HPP__

#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/partnerdef.h"

enum ROLE_ATTR_PELLET_TYPE
{
	ROLE_ATTR_PELLET_TYPE_ROLE = 0,			//角色
	ROLE_ATTR_PELLET_TYPE_PET = 1,			//宠物
	ROLE_ATTR_PELLET_TYPE_PARTNER = 2,		//伙伴

	ROLE_ATTR_PELLET_TYPE_MAX
};

enum ATTR_PELLET_TYPE
{
	ATTR_PELLET_TYPE_ATTACK = 0,			//攻击
	ATTR_PELLET_TYPE_DEFENSE = 1,			//防御
	ATTR_PELLET_TYPE_AGILITY = 2,			//敏捷(速度)	
	ATTR_PELLET_TYPE_RECOVERY = 3,			//回复
	ATTR_PELLET_TYPE_MENTAL = 4,			//精神
	ATTR_PELLET_TYPE_HP = 5,				//生命
	ATTR_PELLET_TYPE_MP = 6,				//法力

	ATTR_PELLET_TYPE_MAX
};
static const int MAX_ATTR_PELLET_TYPE_NUM = 30;
UNSTD_STATIC_CHECK(ATTR_PELLET_TYPE_MAX <= MAX_ATTR_PELLET_TYPE_NUM);
UNSTD_STATIC_CHECK(ATTR_PELLET_TYPE_MAX == 7);
static const int ATTR_PELLET_TYPE_LIST[ATTR_PELLET_TYPE_MAX] = 
{
	BATTLE_ATTR_ATTACK, BATTLE_ATTR_DEFENSE, BATTLE_ATTR_AGILITY, BATTLE_ATTR_RECOVERY, BATTLE_ATTR_MENTAL,
	BATTLE_ATTR_MAX_HP, BATTLE_ATTR_MAX_MP
};

static const int ATTR_BATTLE_TYPE_LIST[] = 
{
	ATTR_PELLET_TYPE_ATTACK, ATTR_PELLET_TYPE_DEFENSE, ATTR_PELLET_TYPE_AGILITY, ATTR_PELLET_TYPE_RECOVERY, ATTR_PELLET_TYPE_MENTAL,
	-1, ATTR_PELLET_TYPE_HP, -1, ATTR_PELLET_TYPE_MP
};


int GetAttrTypeByPelletType(int attr_pellet_type);
int GetPelletTypeByAttrType(int attr_type);

struct RoleAttrPelletParam
{
	RoleAttrPelletParam() { this->Reset();}

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		reserve_int = 0;
		memset(role_use_attr_list, 0, sizeof(role_use_attr_list));
		memset(reserve_int_list, 0, sizeof(reserve_int_list));
		memset(partner_use_attr_list, 0, sizeof(partner_use_attr_list));
	}

	long long reserve_ll[3];
	int reserve_int;
	short role_use_attr_list[MAX_ATTR_PELLET_TYPE_NUM];							//角色使用的各个类型属性丹数量
	int reserve_int_list[10];
	short partner_use_attr_list[PARTNER_NUM_MAX][MAX_ATTR_PELLET_TYPE_NUM];
};
typedef char RoleAttrPelletParamHex[sizeof(RoleAttrPelletParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleAttrPelletParamHex) < 6144);

#endif