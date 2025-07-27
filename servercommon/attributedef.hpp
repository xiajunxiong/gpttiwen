#ifndef __ATTRIBUTE_DEF_HPP__
#define __ATTRIBUTE_DEF_HPP__

#include "servercommon/servercommon.h"

struct AttrItemCfg
{
	AttrItemCfg(int _type, int _value) :is_pet_attr(0), attr_type(_type), attr_value(_value){}
	AttrItemCfg()
	{
		is_pet_attr = 0;
		attr_type = 0;
		attr_value = 0;
	}

	int is_pet_attr;
	int attr_type;
	int attr_value;
};

#pragma pack(push, 4)

enum ADD_POINT_TYPE
{
	ADD_POINT_TYPE_MIN = 0,
	ADD_POINT_TYPE_VITALITY = ADD_POINT_TYPE_MIN,	//!< 体力 0 
	ADD_POINT_TYPE_STRENGTH,						//!< 力量 1 
	ADD_POINT_TYPE_POWER,							//!< 强度 2 
	ADD_POINT_TYPE_AGILITY,						//!< 敏捷 3
	ADD_POINT_TYPE_MAGIC,							//!< 魔法 4 
	ADD_POINT_TYPE_MAX							//!< 枚举项数量 5
};

static const int ATTRIBUTE_PLAN_NAME_LENGTH = 64;
static const int AUTO_ADD_ATTR_LEVEL = 10;
static const int UPLEVEL_ADD_ATTR_POINT = 4;
static const int ATTR_BASE_POINT_MAX_NUM = 30;

struct AttributePlan
{
	AttributePlan() { this->Reset(); }

	void Reset()
	{
		memset(plan_name, 0, sizeof(plan_name));
		remain_point = 0;
		memset(add_point_list, 0, sizeof(add_point_list));
	}

	char plan_name[ATTRIBUTE_PLAN_NAME_LENGTH];
	short remain_point;
	short add_point_list[ADD_POINT_TYPE_MAX];
};

UNSTD_STATIC_CHECK(ADD_POINT_TYPE_MAX == 5);

static const int MAX_ATTR_PLAN_NUM = 3;

struct AttributePlanParam
{
	AttributePlanParam() { this->Reset(); }

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		reserve_int = 0;
		tui_jian_auto_add = 1;
		reset_plan_times = 0;
		plan_num = 0;
		cur_plan_index = 0;
		for (int i = 0; i < MAX_ATTR_PLAN_NUM; ++i)
		{
			plan_list[i].Reset();
		}

		for (int i = 0; i < MAX_ATTR_PLAN_NUM; ++i)
		{
			for (int k = 0; k < ADD_POINT_TYPE_MAX; ++k)
			{
				custom_add_point_list[i][k] = 0;
			}
		}

		reserve_sh = 0;
	}

	long long reserve_ll[2];
	int reserve_int;
	short tui_jian_auto_add;
	unsigned short reset_plan_times;
	short plan_num;
	short cur_plan_index;
	AttributePlan plan_list[MAX_ATTR_PLAN_NUM];
	short custom_add_point_list[MAX_ATTR_PLAN_NUM][ADD_POINT_TYPE_MAX];
	short reserve_sh;	// 保留字 用于对齐
};
UNSTD_STATIC_CHECK(5 == ADD_POINT_TYPE_MAX);

typedef char AttributeParamHex[sizeof(AttributePlanParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(AttributeParamHex) < 1024);

#pragma pack(pop)

#endif