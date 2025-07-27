#ifndef __HUAN_SHOU_DEF_H__
#define __HUAN_SHOU_DEF_H__

#include "common/tlvprotocol.h"
#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MAX_HUAN_SHOU_NUM = 80;						//幻兽数量
static const int NAX_HUAN_SHOU_STR_LEVEL_NUM = 20;				//幻兽强化等级
static const int MAX_HUAN_SHOU_SKILL_GRID_NUM = 12;				//幻兽技能格子
static const int MAX_HUAN_SHOU_POTENTIAL_POINT = 60;			//幻兽潜能点位
static const int MAX_HUAN_SHOU_EGG_ID_NUM = 10;					//幻兽蛋ID最大值
static const int MAX_HUAN_SHOU_POINT_LEVEL_NUM = 10;			//潜能点位等级
static const int MAX_HUAN_SHOU_POTENTIAL_POINT_GROUP_NUM = 10;	//潜能点位组配置最大数量
static const int MAX_HUAN_SHOU_ANNOUNCEMENT_INFO_NUM = 10;		//幻兽孵化公告最大条目

static const int MAX_HUAN_SHOU_SKILL_GRID_CUR_NUM = 10;			//幻兽技能格子数量(当前)
static const int MAX_HUAN_SHOU_CUR_POTENTIAL_POINT_NUM = 40;	//幻兽潜能点位(当前)

UNSTD_STATIC_CHECK(MAX_HUAN_SHOU_SKILL_GRID_NUM >= MAX_HUAN_SHOU_SKILL_GRID_CUR_NUM);
UNSTD_STATIC_CHECK(MAX_HUAN_SHOU_POTENTIAL_POINT >= MAX_HUAN_SHOU_CUR_POTENTIAL_POINT_NUM);

struct HuanShouAnnouncementInfo
{
	HuanShouAnnouncementInfo() { this->Reset(); }

	void Reset()
	{
		role_uid = 0;
		memset(role_name, 0, sizeof(role_name));
		egg_item_id = 0;
		huanshou_id = -1;
	}

	 int role_uid;
	 GameName role_name;
	 int egg_item_id;
	 int huanshou_id;
};

struct HuanShouAnnouncementParam
{
	HuanShouAnnouncementParam() { this->Reset(); }

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));

		reserve_sh = 0;
		ann_count = 0;
		for (int i = 0; i < ARRAY_ITEM_COUNT(ann_list); ++i)
		{
			ann_list[i].Reset();
		}
	}
	
	long long reserve_ll[3];
	
	short reserve_sh;	
	short ann_count;															//当前公告数量
	HuanShouAnnouncementInfo ann_list[MAX_HUAN_SHOU_ANNOUNCEMENT_INFO_NUM];		//公告列表
};

typedef char HuanShouAnnouncementParamHex[sizeof(HuanShouAnnouncementParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(HuanShouAnnouncementParamHex) < 2048);

struct RoleHuanShouData
{
	RoleHuanShouData() { this->Reset(); }

	void Reset()
	{
		memset(reserev_ll, 0, sizeof(reserev_ll));
		reserve_sh = 0;

		str_level = 0;
		new_skill_gird_idx = -1;
		new_skill_id = 0;
		memset(skill_list, 0 , sizeof(skill_list));
		memset(point_level_list, 0, sizeof(point_level_list));
	}

	bool IsValid() const
	{
		return str_level > 0;
	}

	void NewSkillReset()
	{
		new_skill_gird_idx = -1;
		new_skill_id = 0;
	}

	bool IsHasNewSkill() const
	{
		return new_skill_gird_idx >= 0 && new_skill_gird_idx < ARRAY_ITEM_COUNT(skill_list) && new_skill_id > 0;
	}

	int GetSkillCount()
	{
		int skill_count = 0;
		for (int i = 0; i < ARRAY_ITEM_COUNT(skill_list); ++i)
		{
			if (skill_list[i] <= 0)
			{
				break;
			}

			skill_count += 1;
		}

		return skill_count;
	}

	long long reserev_ll[2];

	short reserve_sh;
	unsigned short str_level;			//幻兽强化等级(激活则为1级)
	short new_skill_gird_idx;			//打书的数据暂存
	unsigned short new_skill_id;		//打书的数据暂存
	unsigned short skill_list[MAX_HUAN_SHOU_SKILL_GRID_NUM];	//幻兽打书技能列表
	unsigned char point_level_list[MAX_HUAN_SHOU_POTENTIAL_POINT];	//幻兽潜能点位等级列表
};

struct RoleHuanShouParam
{
	RoleHuanShouParam() { this->Reset(); }

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));

		fushen_id = -1;
		is_use_huanshou = 0;
		cur_lian_dan_count = 0;
		start_lian_dan_timestamp = 0;

		for (int i = 0; i < ARRAY_ITEM_COUNT(huan_shou_list); ++i)
		{
			huan_shou_list[i].Reset();
		}
	}

	bool IsFushen() const
	{
		return fushen_id >= 0;
	}
	
	long long reserve_ll[3];
	
	char fushen_id;								//此时附身的幻兽ID(id等同于序号)
	char is_use_huanshou;						//是否使用幻兽形象 0:不使用 1:使用幻兽形象
	short cur_lian_dan_count;					//丹炉内此时正在炼丹的次数
	unsigned int start_lian_dan_timestamp;		//开始炼丹时间戳
	int egg_cur_open_times_list[MAX_HUAN_SHOU_EGG_ID_NUM];	//幻兽保底次数			

	RoleHuanShouData huan_shou_list[MAX_HUAN_SHOU_NUM];			//幻兽数据列表
};

typedef char RoleHuanShouParamHex[sizeof(RoleHuanShouParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleHuanShouParamHex) < 18432);

#pragma pack(pop)

#endif 