#ifndef __RA_QING_YUAN_ACTIVITY_PARAM_HPP__
#define __RA_QING_YUAN_ACTIVITY_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 情缘活动 //////////////////////////////////////////


#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////



////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
// 情缘时装
struct RAQingYuanShiZhuangParam
{
	RAQingYuanShiZhuangParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		qing_yuan_id = 0;

		fetch_mark = 0;
		is_open_client = 0;
		is_role_buy_mark = 0;
	}

	unsigned int ra_begin_timestamp;
	long long qing_yuan_id;

	short fetch_mark;
	short is_open_client;
	short is_role_buy_mark;
};

UNSTD_STATIC_CHECK(sizeof(RAQingYuanShiZhuangParam) <= sizeof(RoleActivityRoleData))

// 情缘累充
struct RAQingYuanLeiChongParam
{
	RAQingYuanLeiChongParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		qing_yuan_id = 0;
		join_begin_timestamp = 0;
		join_end_timestamp = 0;
		lihun_refresh_timestamp = 0;

		fetch_mark = 0;
	}

	unsigned int ra_begin_timestamp;
	long long qing_yuan_id;
	unsigned int join_begin_timestamp;			// 参与开始时间戳
	unsigned int join_end_timestamp;			// 参与结束时间戳
	unsigned int lihun_refresh_timestamp;		// 离婚重置时间戳

	int fetch_mark;								// 领取标记
};

UNSTD_STATIC_CHECK(sizeof(RAQingYuanLeiChongParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_QING_YUAN_ACTIVITY_PARAM_HPP__
