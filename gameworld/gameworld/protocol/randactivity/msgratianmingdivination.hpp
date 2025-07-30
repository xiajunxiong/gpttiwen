#ifndef __MSG_RA_TIAN_MING_DIVINATION_HPP__
#define __MSG_RA_TIAN_MING_DIVINATION_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ratianmingdivinationparam.hpp"

//////////////////////////////////////////// 2077  随机活动  天命卜卦  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_TIANMING_DIVINATION_OPERA_TYPE
	{
		RA_TIANMING_DIVINATION_OPERA_TYPE_QUERY_INFO = 0,			// 请求活动信息
		RA_TIANMING_DIVINATION_OPERA_TYPE_ADD_LOT_TIMES = 1,		// 竹签加注
		RA_TIANMING_DIVINATION_OPERA_TYPE_RESET_ADD_LOT_TIMES = 2,	// 重置竹签加注倍数
		RA_TIANMING_DIVINATION_OPERA_TYPE_START_CHOU = 3,			// 开始抽签
		RA_TIANMING_DIVINATION_OPERA_TYPE_REWARD_POOL_INFO = 4,		// 下发历史奖池

		RA_TIANMING_DIVINATION_OPERA_TYPE_MAX
	};

	class SCRATianMingDivinationInfo							// 天命卜卦信息 1235
	{
		struct HistoryItem
		{
			char seq;											// 奖池下标
			char add_lot;										// 获奖加注倍数
		};

	public:
		SCRATianMingDivinationInfo();
		MessageHeader header;

		int free_chou_times;															// 今天已用免费卜卦次数
		char add_lots_list[RA_TIANMING_LOT_COUNT];										// 竹签加注
		short reserve_sh;
		unsigned int ra_tianming_today_chou_tims;										// 今日抽了几次
	};

	class SCTianMingDivinationActivityStartChouResult			// 天命卜卦获奖结果 
	{
	public:
		SCTianMingDivinationActivityStartChouResult();
		MessageHeader header;

		short reward_index;										// 获奖物品下标
		short reserve_sh;
	};

	class SCTianMingRewardPoolInfo								// 天命卜卦获奖池列表
	{
	public:
		const static int MAX_POOL_NUM = 40;
		SCTianMingRewardPoolInfo();
		MessageHeader header;

		int pool_num;
		TianMingDivinationInfo pool_info[MAX_POOL_NUM];
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_TIAN_MING_DIVINATION_HPP__


