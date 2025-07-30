#ifndef __MSG_RA_PAI_MAI_HANG_HPP__
#define __MSG_RA_PAI_MAI_HANG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rapaimaihangparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议3000
	enum RA_PAI_MAI_HANG_REQ_TYPE
	{
		RA_PAI_MAI_HANG_REQ_TYPE_ALL_INFO = 0,			//请求角色所有信息
		RA_PAI_MAI_HANG_REQ_TYPE_BID = 1,				//请求竞拍 P1:seq P2:bid_price
		RA_PAI_MAI_HANG_REQ_TYPE_AUTO_BID = 2,			//请求自动竞拍 P1:seq P2:bid_price P3:add_price (此处固定是自动竞价)
		RA_PAI_MAI_HANG_REQ_TYPE_CANCEL_AUTO = 3,		//请求取消自动竞拍 P1:seq  (取消自动则会立刻返还剩余寄存)
		RA_PAI_MAI_HANG_REQ_TYPE_RETRIEVE = 4,			//请求取回寄存元宝 P1:seq P2:retrieve_gold P3:add_price
		RA_PAI_MAI_HANG_REQ_TYPE_SET_SHOW_FALG = 5,		//请求设置信息展示状态 P1:seq P2: 0-不展示
		RA_PAI_MAI_HANG_REQ_TYPE_RECORD_INFO = 6,		//请求记录信息
		RA_PAI_MAI_HANG_REQ_TYPE_CHANGE_ADD_PRICE = 7,	//请求修改加价金额(只有拥有寄存才可修改) p1:seq p2:add_price
	};

	
	class SCRAPaiMaiHangNoticeInfo		//!< 3100 随机活动-拍卖行通知信息下发
	{
	public:
		SCRAPaiMaiHangNoticeInfo();
		MessageHeader header;

		unsigned int pai_mai_start_timestamp;				//竞拍开始时间戳
		unsigned int pai_mai_end_timestamp;					//竞拍结束时间戳
		unsigned int pai_mai_last_end_timestamp;			//竞拍最晚结束时间戳

		RAPaiMaiHangSingleShopItemInfo shop_item_list[MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM];	//全服商品信息
	};

	class SCRAPaiMaiHangSingleNoticeInfo	//!< 3101 随机活动-拍卖行单个通知信息下发
	{
	public:
		SCRAPaiMaiHangSingleNoticeInfo();
		MessageHeader header;
		
		int seq;
		RAPaiMaiHangSingleShopItemInfo shop_item_info;
	};

	class SCRAPaiMaiHangUserInfo	//!< 3102 随机活动-拍卖行角色信息下发
	{
	public:
		SCRAPaiMaiHangUserInfo();
		MessageHeader header;

		RAPaiMaiHangUserSingleItemInfo shop_item_list[MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM];	//角色自身商品信息
	};

	class SCRAPaiMaiHangUserSingleInfo	//!< 3103 随机活动-拍卖行角色单个信息下发
	{
	public:
		SCRAPaiMaiHangUserSingleInfo();
		MessageHeader header;

		int seq;
		RAPaiMaiHangUserSingleItemInfo shop_item_info;
	};

	class SCRAPaiMaiHangUserRecordInfo	//!< 3104 随机活动-拍卖行角色竞拍记录下发
	{
	public:
		SCRAPaiMaiHangUserRecordInfo();
		MessageHeader header;

		int record_count;
		RAPaiMaiHangRecordParam record_list[MAX_PAI_MAI_HANG_RECORD_NUM];
	};


}

#pragma pack(pop)

#endif //__MSG_RA_PAI_MAI_HANG_HPP__