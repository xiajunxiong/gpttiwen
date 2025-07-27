#ifndef __ROLE_NEW_LIFE_SKILL_DEF_HPP__
#define __ROLE_NEW_LIFE_SKILL_DEF_HPP__

#include "servercommon/servercommon.h"

static const int MAX_NEW_LIFT_SKILL_TYPE = 40;
//static const int MAX_LIFE_SKILL_COOK_GIFT_NUM = 10;
static const int MAX_NEW_LIFE_SKILL_PROPS_NUM = 30;
static const int MAX_NEW_LIFE_SKILL_CHANGE_SURFACE_NUM = 200;
static const int MAX_NEW_LIFE_SKILL_CHANGE_CARD_LOGOUT_TIME = 180;	//离线不超过该时间则不取消幻化

enum NEW_LIFE_SKILL_PROPS_TYPE
{
	NEW_LIFE_SKILL_PROPS_TYPE_BEGIN = 0,
	NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD = NEW_LIFE_SKILL_PROPS_TYPE_BEGIN,	//变身卡
	NEW_LIFE_SKILL_PROPS_TYPE_FOOD_0 = 1,		//食物道具
	NEW_LIFE_SKILL_PROPS_TYPE_FOOD_1 = 2,		//食物道具
	NEW_LIFE_SKILL_PROPS_TYPE_FOOD_2 = 3,		//食物道具
	NEW_LIFE_SKILL_PROPS_TYPE_FOOD_3 = 4,		//食物道具
	NEW_LIFE_SKILL_PROPS_TYPE_FOOD_4 = 5,		//食物道具
	NEW_LIFE_SKILL_PROPS_TYPE_FOOD_5 = 6,		//食物道具

	NEW_LIFE_SKILL_PROPS_TYPE_MAX
};
UNSTD_STATIC_CHECK(MAX_NEW_LIFE_SKILL_PROPS_NUM >= NEW_LIFE_SKILL_PROPS_TYPE_MAX);

enum NEW_LIFE_SKILL_TYPE
{
	NEW_LIFE_SKILL_TYPE_MIN = 0,
	NEW_LIFE_SKILL_TYPE_PICK_FLOWER = NEW_LIFE_SKILL_TYPE_MIN,		//采摘
	NEW_LIFE_SKILL_TYPE_FISHING = 1,								//钓鱼
	NEW_LIFE_SKILL_TYPE_LUMBERING = 2,								//砍伐
	NEW_LIFE_SKILL_TYPE_DIG = 3,									//挖矿
	NEW_LIFE_SKILL_TYPE_PHARMACIST = 4,								//药师
	NEW_LIFE_SKILL_TYPE_COOK = 5,									//厨师

	NEW_LIFE_SKILL_TYPE_MAX
};
UNSTD_STATIC_CHECK(MAX_NEW_LIFT_SKILL_TYPE >= NEW_LIFE_SKILL_TYPE_MAX);

enum NEW_LIFE_SKILL_RELIEVE_SURFACE_TYPE
{
	NEW_LIFE_SKILL_RELIEVE_SURFACE_TYPE_LOGIN = 0,			//登陆取消幻化
	NEW_LIFE_SKILL_RELIEVE_SURFACE_TYPE_FINISH_FIGHT = 1,	//战斗结束幻化点不足
	NEW_LIFE_SKILL_RELIEVE_SURFACE_TYPE_CLIENT = 2,			//客户端请求取消

	NEW_LIFE_SKILL_RELIEVE_SURFACE_TYPE_MAX
};

struct NewLifeSkillInfo
{
	NewLifeSkillInfo() { this->Reset(); }
	
	void Reset()
	{
		skill_level = 1;
		reserve_sh = 0;
		trigger_double_count = 0;
	}

	short skill_level;
	short reserve_sh;
	int trigger_double_count;
};

struct NewLifeSkillPropsInfo
{
	NewLifeSkillPropsInfo() { this->Reset(); }

	void Reset()
	{
		seq = 0;
		reserve_sh = 0;
		end_timestamp = 0;
	}

	void EndReset()
	{
		seq = 0;
		end_timestamp = 0;
	}

	short seq;
	short reserve_sh;
	unsigned int end_timestamp;
};

struct NewLifeSkillChangeSurfaceInfo
{
	NewLifeSkillChangeSurfaceInfo() { this->Reset(); }

	void Reset()
	{
		seq = 0;
		reserve_sh = 0;
		surface_point = 0;
	}

	short seq;
	short reserve_sh;
	int surface_point;
};

struct RoleNewLifeSkillParam
{
	RoleNewLifeSkillParam() { this->Reset(); }
	 
	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		for (int i = 0; i < ARRAY_LENGTH(life_skill_list); i++)
		{
			life_skill_list[i].Reset();
		}
		fight_change_card_seq = 0;
		can_cancel_change_card_timestamp = 0;
		memset(reserve_int_list, 0, sizeof(reserve_int_list));
		memset(reserve_LL, 0, sizeof(reserve_LL));
		is_use_change_card = 1;
		is_reset_use_change_card_flag = 0;
		change_surface_count = 0;
		for (int i = 0; i < ARRAY_LENGTH(life_skill_props_list); ++i)
		{
			life_skill_props_list[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(change_surface_list); ++i)
		{
			change_surface_list[i].Reset();
		}
	}
	NewLifeSkillChangeSurfaceInfo * GetChangeSurface(int seq)
	{
		for (int i = 0; i < change_surface_count && i < ARRAY_LENGTH(change_surface_list); ++i)
		{
			if (seq == change_surface_list[i].seq)
			{
				return&change_surface_list[i];
			}
		}
		return NULL;
	}


	long long reserve_ll[3];
	NewLifeSkillInfo life_skill_list[MAX_NEW_LIFT_SKILL_TYPE];			// 生活技能列表
	
	int fight_change_card_seq;			//开始战斗时带入的变身卡seq
	unsigned int can_cancel_change_card_timestamp;
	int reserve_int_list[9];
	long long reserve_LL[2];

	char is_use_change_card;			//是否使用变身卡形象 0:不使用 1:使用变身卡形象
	char is_reset_use_change_card_flag;	//是否重置使用变身卡形象
	short change_surface_count;
	NewLifeSkillPropsInfo life_skill_props_list[MAX_NEW_LIFE_SKILL_PROPS_NUM];	//食物道具类型列表
	NewLifeSkillChangeSurfaceInfo change_surface_list[MAX_NEW_LIFE_SKILL_CHANGE_SURFACE_NUM];	//变身卡信息列表(幻化)
};
typedef char RoleNewLifeSkillParamHex[sizeof(RoleNewLifeSkillParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleNewLifeSkillParamHex) < 6144);

#endif