#ifndef __MSG_RA_COOL_SUMMER_HPP__
#define __MSG_RA_COOL_SUMMER_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/racoolsummerparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	//超级折扣商城
	enum RA_SUPER_OFF_SHOP_REQ_TYPE
	{	
		RA_SUPER_OFF_SHOP_REQ_TYPE_INFO = 0,									//请求信息
		RA_SUPER_OFF_SHOP_REQ_TYPE_BUY_NORMAL_ITEM = 1,							//请求购买道具 param1:seq(配置seq)
		RA_SUPER_OFF_SHOP_REQ_TYPE_BUY_DISC_ITEM = 2,							//请求购买折扣道具 param1:seq(下发phase_rand_disc_item_list数组seq)
		RA_SUPER_OFF_SHOP_REQ_TYPE_BUY_CMD_ITEM_CONFIG_CHECK = 3,				//购买直购道具前请求此处进行配置检测  param1:seq param2: 0.折扣货架 1:正常货架
	};

	class SCRASuperOffShopInfo				//!< 3081  随机活动-超级折扣商城信息下发
	{
	public:
		SCRASuperOffShopInfo();
		MessageHeader header;

		short is_need_recal_phase_flag;												//0:不重算 1:重算阶段
		short phase_rand_disc_item_list[MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM];		//刷出的折扣商品列表			MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM - 25
		short today_normal_buy_times[MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM];	//正常货架每日限购			MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM - 27
		short today_disc_buy_times[MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM];		//折扣货架每日限购			MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM - 5
		short activity_normal_buy_times[MAX_RA_SUPER_OFF_SHOP_NORMAL_ITEM_NUM];		//正常货架活动限购			MAX_RA_SUPER_OFF_SHOP_NORMAL_ITEM_NUM - 135
		short activity_disc_buy_times[MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM];			//折扣货架活动限购			
	};

	class SCRASuperOffShopSingleInfo		//!< 3083  随机活动-超级折扣商城单个信息下发
	{
	public:
		SCRASuperOffShopSingleInfo();
		MessageHeader header;

		enum NOTICE_TYPE
		{
			NOTICE_TYPE_NORMAL = 1,		//正常货架 param1:seq(每日) param2:index(活动)
			NOTICE_TYPE_DISC = 2,		//折扣货架 param1:index(每日) param2:seq(活动)
		};
		int notice_type;
		short param1;
		short param2;
		short today_buy_times;
		short activity_buy_times;
	};

	//凉风送爽
	enum RA_COOL_BREEZE_REQ_TYPE
	{
		RA_COOL_BREEZE_REQ_TYPE_INFO = 0,					//请求信息下发
		RA_COOL_BREEZE_REQ_TYPE_COMMIT_TASK = 1,			//请求完成任务	  param1:数组下标(index) param2:0:固定任务 1:随机任务
		RA_COOL_BREEZE_REQ_TYPE_REFRESH_TASK = 2,			//请求刷新固定任务  param1:数组下标(index)
	};

	class SCRACoolBreezeInfo		//!< 3085 随机活动-凉风送爽信息下发
	{
	public:
		SCRACoolBreezeInfo();
		MessageHeader header;

		short last_refresh_fixe_open_day;												//上次刷新固定任务时对应的活动开启天数
		short last_refresh_random_open_day;												//上次刷新随机任务时对应活动开启天数
		unsigned int next_fixe_task_refresh_timestamp;									//下次固定任务刷新时间戳			
		unsigned int next_random_task_refresh_timestamp;								//下次随机任务刷新时间戳
		RACoolBreezeTaskInfo fixe_task_list[MAX_RA_COOL_BREEZE_FIXE_TASK_NUM];			//固定任务列表
		RACoolBreezeTaskInfo random_task_list[MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM];		//随机任务列表
	};

	class SCRACoolBreezeSingleInfo		//!< 3086 随机活动-凉风送爽单个信息下发
	{
	public:
		SCRACoolBreezeSingleInfo();
		MessageHeader header;

		short index;	
		short task_group_type;				//1:固定任务 2:随机任务
		RACoolBreezeTaskInfo task_info;
	};

	//荷香觅宝
	enum RA_LOTUS_MI_BAO_REQ_TYPE
	{
		RA_LOTUS_MI_BAO_REQ_TYPE_INFO = 0,						//请求信息下发
		RA_LOTUS_MI_BAO_REQ_TYPE_ACTIVE_GRID = 1,				//请求激活某个格子 param1:row[0,3] param2:column[0,3]
		RA_LOTUS_MI_BAO_REQ_TYPE_FETCH_ACTIVE_ALL_REWARD = 2,	//领取集齐奖励
	};

	class SCRALotusMiBaoInfo			//!< 3087  随机活动-荷香觅宝信息下发
	{
	public:
		SCRALotusMiBaoInfo();
		MessageHeader header;

		short reserve_sh;
		char reserve_ch;
		char is_fetch_all_reward_flag;								//领取集齐奖励标识  0:未领取 1:已领取
		//BitMap<RA_LOUTS_MI_BAO_COND_NUM> cond_flag;					//凑成条件奖励标识 一行或一列:(cond - 1) * 10 + (param - 1)  集齐全部(3)RA_LOUTS_MI_BAO_COND_NUM - 1  
		BitMap<MAX_RA_LOTUS_MI_BAO_GRID_NUM> grid_active_flag;		//每个格子的激活标识	row[0, 3] * MAX_RA_LOTUS_MI_BAO_COLUMN_NUM + column[0,3]
		
	};

	//荷香觅宝
	enum RA_SHAN_HAI_MI_BAO_REQ_TYPE
	{
		RA_SHAN_HAI_MI_BAO_REQ_TYPE_INFO = 0,						//请求信息下发
		RA_SHAN_HAI_MI_BAO_REQ_TYPE_ACTIVE_GRID = 1,				//请求激活某个格子 param1:row[0,3] param2:column[0,3]
		RA_SHAN_HAI_MI_BAO_REQ_TYPE_FETCH_ACTIVE_ALL_REWARD = 2,	//领取集齐奖励
	};

	class SCRAShanHaiMiBaoInfo			//!< 3122  随机活动山海觅宝信息下发
	{
	public:
		SCRAShanHaiMiBaoInfo();
		MessageHeader header;

		short reserve_sh;
		char reserve_ch;
		char is_fetch_all_reward_flag;								//领取集齐奖励标识  0:未领取 1:已领取
																	//BitMap<RA_LOUTS_MI_BAO_COND_NUM> cond_flag;					//凑成条件奖励标识 一行或一列:(cond - 1) * 10 + (param - 1)  集齐全部(3)RA_LOUTS_MI_BAO_COND_NUM - 1  
		BitMap<MAX_RA_LOTUS_MI_BAO_GRID_NUM> grid_active_flag;		//每个格子的激活标识	row[0, 3] * MAX_RA_LOTUS_MI_BAO_COLUMN_NUM + column[0,3]

	};
}


#pragma pack(pop)

#endif	//__MSG_RA_COOL_SUMMER_HPP__
