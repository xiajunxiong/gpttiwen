#ifndef __MSG_COW_REPORT_GOOD_NEWS_HPP__
#define __MSG_COW_REPORT_GOOD_NEWS_HPP__

#include "gamedef.h"
#include "servercommon/struct/cowreportgoodnewsparam.h"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	enum COW_REPORT_GOOD_NEWS_CLIENT_REQ_TYPE
	{
		COW_REPORT_GOOD_NEWS_CLIENT_REQ_TYPE_BUY = 0,							// 购买
		COW_REPORT_GOOD_NEWS_CLIENT_REQ_TYPE_GET_REWARD_LIST = 1,				// 获取获奖名单
	};

	class CSCowReportGoodNewsClientReq						//!< 4090 - 客户端操作请求
	{
	public:
		CSCowReportGoodNewsClientReq();
		MessageHeader header;

		int req_type;						// 请求类型
	};

	class SCCowReportGoodNewsRewardListInfo					//!< 4091 - 获取中奖人员名单
	{
	public:
		SCCowReportGoodNewsRewardListInfo();
		MessageHeader header;

		struct UserInfo
		{
			UserInfo():uid(0) , avatar_type(0), headshot_id(0)
			{
				memset(role_name, 0, sizeof(GameName));
			}

			int uid;										// =0 则表示是机器人
			short	avatar_type;							// 体型类型
			short	headshot_id;							// 头像ID
			GameName role_name;								// 玩家名称
		};

		UserInfo user_info[MAX_COW_REPORT_GOOD_NEWS_REWARD_USER_COUNT];		// 获奖名单
	};

	class SCCowReportGoodNewsInfo								//!< 4092 - 福牛报喜的系统信息
	{
	public:
		SCCowReportGoodNewsInfo();
		MessageHeader header;

		int curren_status;				// 当前的活动状态

		unsigned int buy_start_times;			// 购买开启时间
		unsigned int buy_end_times;				// 购买结束时间
		unsigned int show_start_times;			// 展示期开启时间
		unsigned int show_end_times;			// 展示期结束时间
	};


	class SCCowReportGoodNewsRoleBuyInfo							//!< 4093 - 玩家的购买信息
	{
	public:
		SCCowReportGoodNewsRoleBuyInfo();
		MessageHeader header;

		int is_buy;													// 玩家自己是否购买过 0 没有 1 已购买
	};
}

#pragma pack(pop)
#endif	// __MSG_COW_REPORT_GOOD_NEWS_HPP__