#ifndef __KUA_FU_YING_XIONG_LU_DEF_HPP__
#define __KUA_FU_YING_XIONG_LU_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

static const int MAX_KUA_FU_YING_XIONG_LU_RANK_NUM = 10;
static const int MAX_KUA_FU_YING_XIONG_LU_RANK_TASK_SEQ_NUM = 10;	//任务配置每个榜单的最大任务数量
static const int MAX_KUA_FU_YING_XIONG_LU_RANK_CHECK_INTERVAL_TIME = 15;	//榜单检测间隔时间

enum KUA_FU_YING_XIONG_LU_RANK_TYPE
{
	KUA_FU_YING_XIONG_LU_RANK_TYPE_BEGIN = 0,

	KUA_FU_YING_XIONG_LU_RANK_TYPE_ROLE_CAP = KUA_FU_YING_XIONG_LU_RANK_TYPE_BEGIN,			// 角色评分
	KUA_FU_YING_XIONG_LU_RANK_TYPE_EQUIP_CAP = 1,			// 装备评分
	KUA_FU_YING_XIONG_LU_RANK_TYPE_PARTNER_CAP = 2,			// 伙伴评分
	KUA_FU_YING_XIONG_LU_RANK_TYPE_PET_CAP = 3,				// 宠物评分
	KUA_FU_YING_XIONG_LU_RANK_TYPE_COMPLEX_CAP = 4,			// 综合评分

	KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX
};
UNSTD_STATIC_CHECK(MAX_KUA_FU_YING_XIONG_LU_RANK_NUM >= KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX);

enum KUA_FU_YING_XIONG_LU_TASK_TYPE
{
	KUA_FU_YING_XIONG_LU_TASK_TYPE_INVALID = 0,				
	KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_MEDAL = 1,			// 穿戴令牌		param1:令牌强化等级
	KUA_FU_YING_XIONG_LU_TASK_TYPE_INSERT_TONE = 2,			// 镶嵌宝石		param1:宝石等级
	KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_EQUIP = 3,			// 穿戴装备		param1:道具品质(0:不限制)\see I_COLOR_MAX
	KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_PET_EQUIP = 4,		// 穿戴宠物护符	param1:道具品质(0:不限制)
	KUA_FU_YING_XIONG_LU_TASK_TYPE_XUN_BAO = 5,				// 寻宝			param1:池子类型(0:不限制)\see XUN_BAO_TREASURE_TYPE_MAX
	KUA_FU_YING_XIONG_LU_TASK_TYPE_HAS_PET = 6,				// 拥有宠物		param1:宠物品质(0:不限制)\see PET_QUALITY_MAX
	KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_SOUL_EQUIP = 7,		// 穿戴魂器		param1:道具品质(0:不限制)
	KUA_FU_YING_XIONG_LU_TASK_TYPE_LEARN_PET_SKILL = 8,		// 学习宠物技能	param1:技能书品质(-1:不限制)\see GAME_PASSIVE_SKILL_PRIORITY_TYPE_MAX
	KUA_FU_YING_XIONG_LU_TASK_TYPE_PARTNER_STR = 9,			// 伙伴强化		param1:伙伴品质(0:不限制)\see PQT_MAX

	KUA_FU_YING_XIONG_LU_TASK_TYPE_MAX
};

struct KuaFuYingXiongLuRankItem
{
	KuaFuYingXiongLuRankItem() { this->Reset(); }

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));

		reserve_sh = 0;
		reserve_ch = 0;
		is_give_rand_activity_reward_flag = 0;
		rank_end_timestamp = 0;
	}
	bool IsInvalid() const
	{
		return rank_end_timestamp <= 0;
	}

	long long reserve_ll[2];

	short reserve_sh;
	char reserve_ch;
	char is_give_rand_activity_reward_flag;			// 该排行类型活动排名奖励结算发放标识 0:未发放 1:已发放
	unsigned int rank_end_timestamp;
	unsigned int next_check_timestamp;				// 下次检测榜单时间戳
};

struct KuaFuYingXiongLuActivityParam
{
	KuaFuYingXiongLuActivityParam() { this->Reset(); }

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));

		is_has_ready_open = 0;
		is_lock_flag = 0;
		reserve_sh = 0;
		activity_open_cross_day = 0;
		activity_begin_timestamp = 0;
		activty_end_timestamp = 0;
		activity_unique_id = 0;
		for (int i = 0; i < ARRAY_LENGTH(rank_list); i++)
		{
			rank_list[i].Reset();
		}
	}
	void ReopenReset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));

		is_has_ready_open = 0;
		reserve_sh = 0;
		activity_open_cross_day = 0;
		activity_begin_timestamp = 0;
		activty_end_timestamp = 0;
		activity_unique_id = 0;
		for (int i = 0; i < ARRAY_LENGTH(rank_list); i++)
		{
			rank_list[i].Reset();
		}
	}
	
	long long reserve_ll[3];

	char is_has_ready_open;								//是否还有准备要开启(0:没有 1:有(此时需等待原先奖励发放完毕才能开启新的)) 
	char is_lock_flag;									//是否上锁 0:未上锁 1:已上锁(上锁开启活动无效)
	short reserve_sh;
	int activity_open_cross_day;						//活动开启时的跨服天数
	unsigned int activity_begin_timestamp;				//活动开始
	unsigned int activty_end_timestamp;					//活动结束
	long long activity_unique_id;						//活动唯一标识
	KuaFuYingXiongLuRankItem rank_list[MAX_KUA_FU_YING_XIONG_LU_RANK_NUM];		//各个排行榜结束时间戳
};

typedef char KuaFuYingXiongLuActivityParamHex[sizeof(KuaFuYingXiongLuActivityParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(KuaFuYingXiongLuActivityParamHex) < 1024);

struct KuaFuYingXiongLuTaskInfo
{
	KuaFuYingXiongLuTaskInfo() { this->Reset(); }

	void Reset()
	{
		task_type = 0;
		is_fetch_flag = 0;
		reserve_sh = 0;
		schedule = 0;
	}

	bool IsInvalid() const
	{
		return task_type <= KUA_FU_YING_XIONG_LU_TASK_TYPE_INVALID || task_type >= KUA_FU_YING_XIONG_LU_TASK_TYPE_MAX;
	}

	unsigned char task_type;
	char is_fetch_flag;		//奖励领取标识 0:未领取
	short reserve_sh;
	int schedule;			//任务进度
};
UNSTD_STATIC_CHECK(KUA_FU_YING_XIONG_LU_TASK_TYPE_MAX < MAX_UINT8);

struct KFYXLRoleRankTypeItem
{
	KFYXLRoleRankTypeItem() { this->Reset(); }

	void Reset()
	{
		reserve_sh = 0;
		reserve_ch = 0;
		is_fetch_rank_end_reward_flag = 0;
		for (int i = 0; i < ARRAY_LENGTH(task_list); ++i)
		{
			task_list[i].Reset();
		}
	}

	short reserve_sh;
	char reserve_ch;
	char is_fetch_rank_end_reward_flag;											// 是否经过该排行榜结束标识 0:未经历过 1:经历过
	KuaFuYingXiongLuTaskInfo task_list[MAX_KUA_FU_YING_XIONG_LU_RANK_TASK_SEQ_NUM];// 任务列表
};
UNSTD_STATIC_CHECK(MAX_KUA_FU_YING_XIONG_LU_RANK_TASK_SEQ_NUM == 10);

//跨服英雄录
struct RoleKuaFuYingXiongParam
{
	RoleKuaFuYingXiongParam() { this->Reset(); }

	void Reset()
	{
		memset(reserev_ll, 0, sizeof(reserev_ll));

		user_begin_participate_timestamp = 0;
		activity_begin_timestamp = 0;
		activity_open_cross_day = 0;
		activity_unique_id = 0;

		for (int i = 0; i < ARRAY_LENGTH(rank_info_list); i++)
		{
			rank_info_list[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(reserve_rank_list); i++)
		{
			reserve_rank_list[i].Reset();
		}
	}

	long long reserev_ll[3];
	unsigned int user_begin_participate_timestamp;			//角色开始参与的时间戳
	unsigned int activity_begin_timestamp;					//活动开启时间戳
	int activity_open_cross_day;							//活动开启时的跨服天数
	long long activity_unique_id;							//活动唯一标识
	unsigned int user_recently_participate_timestamp;		//玩家最近参与的时间(玩家需在榜单过程中在线过方可得任务奖励)

	KFYXLRoleRankTypeItem rank_info_list[MAX_KUA_FU_YING_XIONG_LU_RANK_NUM];	//角色榜单信息列表
	KFYXLRoleRankTypeItem reserve_rank_list[5];	//保留
};
UNSTD_STATIC_CHECK(MAX_KUA_FU_YING_XIONG_LU_RANK_NUM == 10);
typedef char RoleKuaFuYingXiongParamHex[sizeof(RoleKuaFuYingXiongParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleKuaFuYingXiongParamHex) < 4096);

#pragma pack(pop)

#endif	// __KUA_FU_YING_XIONG_LU_DEF_HPP__