#ifndef  __MSG_XIU_WEI_HPP__
#define  __MSG_XIU_WEI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/xiuweiparam.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{	
	enum XIU_WEI_REQ_TYPE
	{
		XIU_WEI_REQ_TYPE_GET_INFO = 0,
		XIU_WEI_REQ_TYPE_UP_LEVEL = 1,			// param1:技能类型 , param2:修炼丹类型
		XIU_WEI_REQ_TYPE_ONE_KEY_UP = 2,		// 一键升级 param1:技能类型 , param2:修炼丹类型
	};

	class CSXiuWeiReq							//!< 4345 修为 客户端操作请求
	{
	public:
		CSXiuWeiReq();
		MessageHeader header;

		int req_type;
		int param1;
		int param2;
	};

	class SCXiuWeiInfo							//!< 4346 修为系统信息	
	{
	public:
		SCXiuWeiInfo();
		MessageHeader		header;

		XiuWeiInfo info[XIU_WEI_TYPE_MAX];		// 修为系统全信息
	};

	class SCXiuWeiInfoChange					// 4347 修为系统信息变更
	{
	public:
		SCXiuWeiInfoChange();
		MessageHeader		header;

		int type;								// 修为的技能类型
		XiuWeiInfo info;						// 对应的信息
	};

	class SCXiuWeiDanYaoLimitInfo				// 4348 高级丹药使用次数信息 
	{
	public:
		SCXiuWeiDanYaoLimitInfo();
		MessageHeader		header;

		int normal_dan_day_times;				// 低级丹药每日已使用次数
		int high_dan_day_times;					// 高级丹药每日已使用次数
	};
}

#pragma pack(pop)

#endif	// __MSG_XIU_WEI_HPP__
