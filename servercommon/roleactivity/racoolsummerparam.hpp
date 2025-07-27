#ifndef __RA_COOL_SUMMER_PARAM_HPP__
#define __RA_COOL_SUMMER_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/roleactivitydef.hpp"
#include <set>

#pragma pack(push, 4)

static const int MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM = 27;			//超级折扣商城阶段中正常货架的最大数量
static const int MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM = 5;				//超级折扣商城阶段中折扣货架的最大数量
static const int MAX_RA_SUPER_OFF_SHOP_PHASE_ITEM_NUM = MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM + MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM;
static const int MAX_RA_SUPER_OFF_SHOP_PHASE_NUM = 5;						//超级折扣商城最大阶段数
static const int MAX_RA_SUPER_OFF_SHOP_NORMAL_ITEM_NUM = MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM * MAX_RA_SUPER_OFF_SHOP_PHASE_NUM;
static const int MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM = MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM * MAX_RA_SUPER_OFF_SHOP_PHASE_NUM;
static const int MAX_RA_SUPER_OFF_SHOP_iTEM_NUM = MAX_RA_SUPER_OFF_SHOP_NORMAL_ITEM_NUM + MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM;

static const int MAX_RA_COOL_BREEZE_FIXE_TASK_NUM = 3;						//凉风送爽固定任务可以同时存在的数量
static const int MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM = 2;					//凉风送爽随机任务可以同时存在的数量

static const int MAX_RA_LOTUS_MI_BAO_ROW_NUM = 4;							//荷香觅宝一行固定格子数
static const int MAX_RA_LOTUS_MI_BAO_COLUMN_NUM = 4;						//荷香觅宝一列固定格子数
static const int MAX_RA_LOTUS_MI_BAO_GRID_NUM = MAX_RA_LOTUS_MI_BAO_ROW_NUM * MAX_RA_LOTUS_MI_BAO_COLUMN_NUM;
static const int RA_LOTUS_MI_BAO_COND_BASE_NUM = 10;
static const int RA_LOUTS_MI_BAO_COND_NUM = 32;
UNSTD_STATIC_CHECK(MAX_RA_LOTUS_MI_BAO_GRID_NUM < (int)(sizeof(int) * BIT_COUNT_OF_BYTE));
UNSTD_STATIC_CHECK(MAX_RA_LOTUS_MI_BAO_ROW_NUM < RA_LOTUS_MI_BAO_COND_BASE_NUM && MAX_RA_LOTUS_MI_BAO_COLUMN_NUM < RA_LOTUS_MI_BAO_COND_BASE_NUM);

//超级折扣商城
UNSTD_STATIC_CHECK(MAX_RA_SUPER_OFF_SHOP_PHASE_ITEM_NUM % sizeof(int) == 0);
UNSTD_STATIC_CHECK(MAX_RA_SUPER_OFF_SHOP_iTEM_NUM % sizeof(int) == 0);
struct RASuperOffShopParam
{
	RASuperOffShopParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		reserve_sh = 0;
		memset(phase_rand_disc_item_list, 0, sizeof(phase_rand_disc_item_list));
		memset(today_normal_buy_times, 0, sizeof(today_normal_buy_times));
		memset(today_disc_buy_times, 0, sizeof(today_disc_buy_times));
		memset(activity_normal_buy_times, 0, sizeof(activity_normal_buy_times));
		memset(activity_disc_buy_times, 0, sizeof(activity_disc_buy_times));
	}

	unsigned int ra_begin_timestamp;							
	short reserve_sh;													
	short phase_rand_disc_item_list[MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM];		//刷出的折扣商品列表
	short today_normal_buy_times[MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM];	//正常货架每日限购
	short today_disc_buy_times[MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM];		//折扣货架每日限购
	short activity_normal_buy_times[MAX_RA_SUPER_OFF_SHOP_NORMAL_ITEM_NUM];		//正常货架活动限购
	short activity_disc_buy_times[MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM];			//折扣货架活动限购
};
UNSTD_STATIC_CHECK(sizeof(RASuperOffShopParam) <= sizeof(RoleActivityRoleData));

//凉风送爽
enum RA_COOL_BREEZE_TASK_GROUP_TYPE
{
	RA_COOL_BREEZE_TASK_GROUP_TYPE_BEGIN = 0,
	RA_COOL_BREEZE_TASK_GROUP_TYPE_FIXE = 1,			//固定任务
	RA_COOL_BREEZE_TASK_GROUP_TYPE_RANDOM = 2,			//随机任务

	RA_COOL_BREEZE_TASK_GROUP_TYPE_MAX
};

enum RA_COOL_BREEZE_TASK_TYPE
{
	RA_COOL_BREEZE_TASK_TYPE_BEGIN = 0,					
	RA_COOL_BREEZE_TASK_TYPE_PASS_CRYSTAL = 1,			   // 通关秘宝阁		param1:次数
	RA_COOL_BREEZE_TASK_TYPE_ACTIVE = 2,				   // 获得活跃度		param1:活跃度数值
	RA_COOL_BREEZE_TASK_TYPE_STRENGTHEN_NUM = 3,		   // 强化宠物次数   param1:次数
	RA_COOL_BREEZE_TASK_TYPE_STRENGTHEN_LEVEL = 4,		   // 强化宠物等级   param1:等级
	RA_COOL_BREEZE_TASK_TYPE_CONSUME_COIN = 5,			   // 消耗铜币		param1:数量
	RA_COOL_BREEZE_TASK_TYPE_CONSUME_GOLD = 6,			   // 消耗元宝		param1:数量
	RA_COOL_BREEZE_TASK_TYPE_CONSUME_ENERGY = 7,		   // 消耗体力		param1:数量
	RA_COOL_BREEZE_TASK_TYPE_XUNBAO_NUM = 8,			   // 唤灵		  	param1:次数
	RA_COOL_BREEZE_TASK_TYPE_SEAL_PET = 9,				   // 封印宠物		param1:只
	RA_COOL_BREEZE_TASK_TYPE_PET_SKILL_LEARN = 10,		   // 宠物技能学习   param1:次数

	RA_COOL_BREEZE_TASK_TYPE_MAX
};

struct RACoolBreezeTaskInfo
{
	RACoolBreezeTaskInfo() { this->Reset(); }
	
	void Reset()
	{
		type = 0;
		task_type = 0;
		schedule = 0;
		open_day = 0;
		refresh_num = 0;
		reserve_ch = 0;
	}

	short type;				//配置type
	short task_type;		//任务类型
	int schedule;			//进度
	short open_day;			//第几天刷新的任务
	char refresh_num;		//刷新任务次数(仅固定任务)
	char reserve_ch;
};

struct RACoolBreezeParam
{
	RACoolBreezeParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		next_fixe_task_refresh_timestamp = 0;
		next_random_task_refresh_timestamp = 0;
		last_refresh_fixe_open_day = 0;
		last_refresh_random_open_day = 0;
		for (int i = 0; i < MAX_RA_COOL_BREEZE_FIXE_TASK_NUM; i++)
		{
			fixe_task_list[i].Reset();
		}
		for (int i = 0; i < MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM; i++)
		{
			random_task_list[i].Reset();
		}
	}

	RACoolBreezeTaskInfo & GetSuitRefreshPosInfo(bool is_fixe = true)	//优先空位
	{
		short suit_refresh_index = 0;
		short min_open_day = MAX_INT16;
		if (is_fixe)
		{
			for (int i = 0; i < MAX_RA_COOL_BREEZE_FIXE_TASK_NUM; i++)
			{
				if (fixe_task_list[i].type <= 0)
				{
					return fixe_task_list[i];
				}
				if (fixe_task_list[i].open_day < min_open_day)
				{
					suit_refresh_index = i;
					min_open_day = fixe_task_list[i].open_day;
				}
			}
			return fixe_task_list[suit_refresh_index];
		}

		for (int i = 0; i < MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM; i++)
		{
			if (random_task_list[i].type <= 0)
			{
				return random_task_list[i];
			}
			if (random_task_list[i].open_day < min_open_day)
			{
				suit_refresh_index = i;
				min_open_day = random_task_list[i].open_day;
			}
		}
		return random_task_list[suit_refresh_index];
	}

	void GetTypeList(std::vector<int> * has_type_list, bool is_fixe = true)
	{
		if(NULL == has_type_list) return;

		if (is_fixe)
		{
			for (int i = 0; i < MAX_RA_COOL_BREEZE_FIXE_TASK_NUM; i++)
			{
				if(fixe_task_list[i].type <= 0) continue;

				has_type_list->push_back(fixe_task_list[i].type);
			}
			return;
		}
		for (int i = 0; i < MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM; i++)
		{
			if(random_task_list[i].type <= 0) continue;

			has_type_list->push_back(random_task_list[i].type);
		}
	}

	unsigned int ra_begin_timestamp;
	unsigned int next_fixe_task_refresh_timestamp;									//下次固定任务刷新时间戳			
	unsigned int next_random_task_refresh_timestamp;								//下次随机任务刷新时间戳
	short last_refresh_fixe_open_day;												//上次刷新固定任务时对应的活动开启天数
	short last_refresh_random_open_day;												//上次刷新随机任务时对应活动开启天数
	RACoolBreezeTaskInfo fixe_task_list[MAX_RA_COOL_BREEZE_FIXE_TASK_NUM];			//固定任务列表
	RACoolBreezeTaskInfo random_task_list[MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM];		//随机任务列表
};
UNSTD_STATIC_CHECK(sizeof(RACoolBreezeParam) <= sizeof(RoleActivityRoleData));

enum RA_LOTUS_MI_BAO_CONDITION_TYPE
{
	RA_LOTUS_MI_BAO_CONDITION_TYPE_BEGIN = 0,
	RA_LOTUS_MI_BAO_CONDITION_TYPE_ROW = 1,			//激活一行	
	RA_LOTUS_MI_BAO_CONDITION_TYPE_COLUMN = 2,		//激活一列	
	RA_LOTUS_MI_BAO_CONDITION_TYPE_ALL = 3,			//集齐全部

	RA_LOTUS_MI_BAO_CONDITION_TYPE_MAX
};

//荷香觅宝
struct RALotusMiBao
{
	RALotusMiBao() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		cond_flag.Reset();
		grid_active_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	BitMap<RA_LOUTS_MI_BAO_COND_NUM> cond_flag;					//凑成条件奖励标识  一行或一列:(cond - 1) * 10 + (param - 1)  集齐全部(3)RA_LOUTS_MI_BAO_COND_NUM - 1 
	BitMap<MAX_RA_LOTUS_MI_BAO_GRID_NUM> grid_active_flag;		//每个格子的激活标识

};
UNSTD_STATIC_CHECK(sizeof(RALotusMiBao) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif