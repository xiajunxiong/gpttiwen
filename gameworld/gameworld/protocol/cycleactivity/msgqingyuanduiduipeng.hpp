#ifndef __MSG_QINGYUAN_DUIDUIPENG_HPP__
#define __MSG_QINGYUAN_DUIDUIPENG_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/cycleactivity/loversmarketparam.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSQingYuanDuiDuiPengReq			//!< 4660
	{
	public:
		enum REQ_TYPE
		{
			REQ_TYPE_START = 0,				// 准备开始
			REQ_TYPE_GOTO_COMPARE,			// 客户端判断两人是否都进入区域，判断成功直接让服务端开始校验幻化
			REQ_TYPE_RANK_INFO,				// 请求排行榜信息
			REQ_TYPE_DAY_COUNT,				// 请求每日次数
			REQ_TYPE_NPC_FORM,				// NPC幻化
		};

		CSQingYuanDuiDuiPengReq();
		MessageHeader header;

		int req_type;
		int p1;
	};

	class SCQingYuanDuiDuiPengStartNotice		//!< 4661 
	{
	public:
		SCQingYuanDuiDuiPengStartNotice();
		MessageHeader header;

		unsigned int begin_game_times;					// 开始时间戳
		unsigned int end_game_times;					// 结束时间戳
	};

	class SCQingYuanDuiDuiRoleForm		//!< 4662
	{
	public:
		SCQingYuanDuiDuiRoleForm();
		MessageHeader header;

		struct RoleData
		{
			int uid;
			int form;
		};

		RoleData role_form[2];
	};

	class SCQingYuanDuiDuiFbInfo		//!< 4663
	{
	public:
		SCQingYuanDuiDuiFbInfo();
		MessageHeader header;

		int integral;
		int succ_match;
		int reward_level;
	};

	class SCQingYuanDuiDuiFinish		//!< 4664
	{
	public:
		SCQingYuanDuiDuiFinish();
		MessageHeader header;

		int is_finish;
	};

	class SCQingYuanDuiDuiRankInfo			//!< 4665
	{
	public:
		SCQingYuanDuiDuiRankInfo();
		MessageHeader header;

		struct RoleData
		{
			int uid;
			GameName name;
			short jifen;
			short pass_times;
		};

		int count;
		RoleData rank_list[100];
	};

	class SCQingYuanDuiDuiDayTimesInfo		//!< 4666
	{
	public:
		SCQingYuanDuiDuiDayTimesInfo();
		MessageHeader header;

		int day_count;
	};

	class CSLoversMarketReq				// 4685
	{
	public:
		CSLoversMarketReq();
		MessageHeader header;

		int type;
		int param;
	};

	class SCLoversMarketInfo			//!< 4686
	{
	public:
		SCLoversMarketInfo();
		MessageHeader header;

		unsigned char store_list[MAX_LOVERS_MARKET_STORE_COUNT];
	};
}
#pragma pack(pop)

#endif