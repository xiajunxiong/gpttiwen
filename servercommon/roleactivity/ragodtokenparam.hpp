#ifndef __RA_GOD_TOKEN_PARAM_HPP__
#define __RA_GOD_TOKEN_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2081 开服活动 - 封神战令  //////////////////////////////////////////

enum RA_GOD_TOKEN_DAY_TYPE								//封神战令每日任务类型
{
	RA_GOD_TOKEN_DAY_TYPE_BEGIN = 0,
	RA_GOD_TOKEN_DAY_TYPE_CRYSTAL_FB,					//完成水晶副本(秘境)
	RA_GOD_TOKEN_DAY_TYPE_COLOSSEUM,					//参与竞技场
	RA_GOD_TOKEN_DAY_TYPE_WST_QUESTION,					//完成神算子
	RA_GOD_TOKEN_DAY_TYPE_TRANSACTION_TASK,				//完成远洋贸易(丝绸之路)
	RA_GOD_TOKEN_DAY_TYPE_RESEARCH_TASK,				//参与三界除魔
	RA_GOD_TOKEN_DAY_TYPE_MAKE_EQUIP,					//打造装备
	RA_GOD_TOKEN_DAY_TYPE_LIMIT_ACTIVITY,				//参与限时活动
	RA_GOD_TOKEN_DAY_TYPE_GUILD_STORE,					//家族商店消费
	RA_GOD_TOKEN_DAY_TYPE_PRESTIGE,						//声望捐赠
	RA_GOD_TOKEN_DAY_TYPE_SEAL_PET,						//封印宠物
	RA_GOD_TOKEN_DAY_TYPE_CONSUME_COIN,					//消耗铜币
	RA_GOD_TOKEN_DAY_TYPE_SHOP_BUY_GEM,					//商会购买宝石
	RA_GOD_TOKEN_DAY_TYPE_TOWER,						//挑战锢魔之塔
	RA_GOD_TOKEN_DAY_TYPE_EQUIP_VOC_CHANGE,				//装备熔炼
	RA_GOD_TOKEN_DAY_TYPE_PLATFORM_BATTLE,				//参与擂台战
	RA_GOD_TOKEN_DAY_TYPE_MAKE_JEWELTY,					//打造灵饰
	RA_GOD_TOKEN_DAY_TYPE_ESCORT,						//完成运镖
	RA_GOD_TOKEN_DAY_TYPE_WILD_BOSS,					//击败野外boss
	RA_GOD_TOKEN_DAY_TYPE_LOGIN,						//登陆
	RA_GOD_TOKEN_DAY_TYPE_CRYSTAL_FB_TI_LI,				//秘境体力

	RA_GOD_TOKEN_DAY_TYPE_MAX
};
enum RA_GOD_TOKEN_WEEK_TYPE								//封神战令每周任务类型
{
	RA_GOD_TOKEN_TYPE_BEGIN = 0,
	RA_GOD_TOKEN_TYPE_WEEK_PET_LEARN_SKILL,				//宠物学习技能书
	RA_GOD_TOKEN_TYPE_WEEK_LIMIT_ACTIVITY,				//参与限时活动
	RA_GOD_TOKEN_TYPE_WEEK_COMSUME_COIN,				//消费铜币
	RA_GOD_TOKEN_TYPE_WEEK_SEAL_PET,					//封印宠物
	RA_GOD_TOKEN_TYPE_WEEK_CRYSTAL_FB,					//完成水晶副本(秘境)
	RA_GOD_TOKEN_TYPE_WEEK_MARKET,						//完成交易所交易
	RA_GOD_TOKEN_TYPE_WEEK_GET_PRESTIGE,				//获得声望
	RA_GOD_TOKEN_TYPE_WEEK_XUN_BAO,						//参与寻宝
	RA_GOD_TOKEN_TYPE_WEEK_WILD_BOSS,					//击败野外boss
	RA_GOD_TOKEN_TYPE_WEEK_PET_BREAKTHROUGHS,			//宠物强化
	RA_GOD_TOKEN_TYPE_WEEK_MI_YI,						//击败远征魔冢首领(密医阴谋)
	RA_GOD_TOKEN_TYPE_WEEK_CRYSTAL_FB_TI_LI,			//秘境体力
	RA_GOD_TOKEN_TYPE_WEEK_LOGIN,						//登陆
	RA_GOD_TOKEN_TYPE_WEEK_SHOP_BUY_GEM,				//商会购买宝石

	RA_GOD_TOKEN_WEEK_TYPE_MAX
};
enum RA_GOD_TOKEN_TASK_TYPE
{
	RA_GOD_TOKEN_TASK_TYPE_DAILY = 0,
	RA_GOD_TOKEN_TASK_TYPE_WEEKLY
};
static const int RA_GOD_TOKEN_TYPE_MAXNUM = 32;					//封神战令最大类型
UNSTD_STATIC_CHECK(RA_GOD_TOKEN_TYPE_MAXNUM >= RA_GOD_TOKEN_DAY_TYPE_MAX);
UNSTD_STATIC_CHECK(RA_GOD_TOKEN_TYPE_MAXNUM >= RA_GOD_TOKEN_WEEK_TYPE_MAX);

static const int RA_GOD_TOKEN_EXCHANGE_MAXNUM = 32;				//封神战令兑换最大seq
static const int RA_GOD_TOKEN_EXCHANGE_BUY_EXP_ADD = 10;		//封神战令购买一次所加经验

static const int RA_GOD_TOKEN_TASK_NUM = 10;					//封神战令任务数(最大)
static const int RA_GOD_TOKEN_DAILY_TASK_NUM = 6;				//封神战令每日任务数
UNSTD_STATIC_CHECK(RA_GOD_TOKEN_TASK_NUM >= RA_GOD_TOKEN_DAILY_TASK_NUM);
static const int RA_GOD_TOKEN_WEEKLY_TASK_NUM = 10;				//封神战令每周任务数
UNSTD_STATIC_CHECK(RA_GOD_TOKEN_TASK_NUM >= RA_GOD_TOKEN_WEEKLY_TASK_NUM);

static const int RA_GOD_TOKEN_MAX_LEVEL = 300;					//封神战令最大等级
static const int RA_GOD_TOKEN_LEVEL_BIT_INFO = 64;				//等级奖励下发信息

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAGodTokenParam
{
	RAGodTokenParam() { this->Reset(); }
	void Reset()
	{
		ra_god_token_begin_timestamp = 0;
		ra_god_token_is_open = 0;
		ra_god_token_time_seq = -1;
		ra_god_token_is_active_token = 0;
		weekly_task_add_mark = 0;

		ra_god_token_level = 0;
		ra_god_token_exp = 0;

		ra_god_token_fetch_common_flag.Reset();
		ra_god_token_fetch_senior_flag.Reset();

		memset(ra_god_token_exchange_count, 0, sizeof(ra_god_token_exchange_count));

		ra_god_token_daily_task_level = 0;
		memset(ra_god_token_daily_task_type, 0, sizeof(ra_god_token_daily_task_type));
		memset(ra_god_token_daily_type_count, 0, sizeof(ra_god_token_daily_type_count));
		ra_god_token_daily_reward_flag.Reset();

		ra_god_token_weekly_task_level = 0;
		memset(ra_god_token_weekly_task_type, 0, sizeof(ra_god_token_weekly_task_type));
		memset(ra_god_token_weekly_type_count, 0, sizeof(ra_god_token_weekly_type_count));
		ra_god_token_week_reward_flag.Reset();

		surplus_exp = 0;
		surplus_receive_count = 0;
		surplus_senior_count = 0;
		ch_reserver = 0;

		ra_god_token_week_task_limit_act_flag.Reset();

		time_seq_timestamp = 0;
		time_level = 0;
		is_change_level_flag = 0;
		reserve_ch = 0;
		reserve_sh = 0;
	}

	unsigned int	ra_god_token_begin_timestamp;
	char			ra_god_token_is_open;											//是否达到要求等级开启活动
	char			ra_god_token_time_seq;											//时间戳索引
	char			ra_god_token_is_active_token;									//是否购买了高级战令
	char			weekly_task_add_mark;

	short			ra_god_token_level;												//战令等级
	short			ra_god_token_exp;												//战令经验

	BitMap<RA_GOD_TOKEN_MAX_LEVEL> ra_god_token_fetch_common_flag;					//普通等级奖励领取标志位
	BitMap<RA_GOD_TOKEN_MAX_LEVEL> ra_god_token_fetch_senior_flag;					//高级等级奖励领取标志位

	unsigned char	ra_god_token_exchange_count[RA_GOD_TOKEN_EXCHANGE_MAXNUM];		//兑换次数

	short			ra_god_token_daily_task_level;
	char			ra_god_token_daily_task_type[RA_GOD_TOKEN_TASK_NUM];			//每日任务类型（先给最大10个，目前6个）
	unsigned int	ra_god_token_daily_type_count[RA_GOD_TOKEN_TYPE_MAXNUM];		//每日任务类型完成次数
	BitMap<RA_GOD_TOKEN_TYPE_MAXNUM> ra_god_token_daily_reward_flag;				//每日任务类型奖励标志

	short			ra_god_token_weekly_task_level;
	char			ra_god_token_weekly_task_type[RA_GOD_TOKEN_TASK_NUM];			//每周任务类型（先给最大10个，目前10个）
	unsigned int	ra_god_token_weekly_type_count[RA_GOD_TOKEN_TYPE_MAXNUM];		//每周任务类型完成次数
	BitMap<RA_GOD_TOKEN_TYPE_MAXNUM> ra_god_token_week_reward_flag;					//每周任务类型奖励标志

	short			surplus_exp;													//溢出经验（超过配置加一次可领取次数）
	short			surplus_receive_count;											//溢出奖励可领取次数
	short			surplus_senior_count;											//溢出奖励高级奖励次数（没购买战令前领取的溢出奖励，购买后补上高级奖励
	short			ch_reserver;

	BitMap<INT_TYPE_BIT> ra_god_token_week_task_limit_act_flag;						//每周任务类型 - 限时活动 记录每天完成了哪个限时活动，每天重置

	unsigned int	time_seq_timestamp;
	int				time_level;														//玩家重置等级
	char			is_change_level_flag;											//老玩家新增等级字段数据处理 1:已处理
	char			reserve_ch;
	short			reserve_sh;										
};

UNSTD_STATIC_CHECK(sizeof(RAGodTokenParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_GOD_TOKEN_PARAM_HPP__
