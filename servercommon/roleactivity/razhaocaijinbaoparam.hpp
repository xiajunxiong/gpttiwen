#ifndef __RA_ZHAOCAIJINBAO_PARAM_HPP__
#define __RA_ZHAOCAIJINBAO_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 招财进宝 //////////////////////////////////////////
static const int MAX_RA_ZHAOCAIJINBAO_BUY_NUM = 10;							// 购买配置上限
static const int MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM = 15;					// 充值奖励配置上限
static const int MAX_RA_ZHAOCAIJINBAO_REFRESH_NUM = 10;						// 刷新奖池道具数
static const int MAX_RA_ZHAOCAIJINBAO_RECORD_NUM = 30;						// 抽奖记录上限

struct RAZhaoCaiJinBaoRecordInfo			// 抽奖记录信息
{
	RAZhaoCaiJinBaoRecordInfo(): times(0), item_id(0), num(0) {}

	unsigned int times;
	int item_id;			//物品ID
	int num;
};


#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////



////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
					
struct RAZhaoCaiJinBaoParam
{
	RAZhaoCaiJinBaoParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		next_refresh_timestamp = 0;

		history_chongzhi = 0;
		day_chongzhi = 0;
		memset(chongzhi_time, 0, sizeof(chongzhi_time));

		memset(pool, 0, sizeof(pool));
		memset(buy_list, 0, sizeof(buy_list));
		memset(chongzhi_list, 0, sizeof(chongzhi_list));

		memset(record_list, 0, sizeof(record_list));

		ArrayCallReset(bao_di_info_list);
	}

	unsigned int ra_begin_timestamp;
	unsigned int next_refresh_timestamp;						// 下一次刷新奖池时间戳

	int history_chongzhi;										// 活动累计充值金额
	int day_chongzhi;											// 活动每日累积充值金额
	short chongzhi_time[MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM];		// 单笔充值可领次数

	short pool[MAX_RA_ZHAOCAIJINBAO_REFRESH_NUM];				// 奖池 seq
	short buy_list[MAX_RA_ZHAOCAIJINBAO_BUY_NUM];				// 购买次数
	short chongzhi_list[MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM];		// 充值奖励次数

	RAZhaoCaiJinBaoRecordInfo record_list[MAX_RA_ZHAOCAIJINBAO_RECORD_NUM];
	BaoDiInfoItem bao_di_info_list[BAO_DI_INFO_ITEM_MAX_NUM];
};

UNSTD_STATIC_CHECK(sizeof(RAZhaoCaiJinBaoParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_ZHAOCAIJINBAO_PARAM_HPP__
