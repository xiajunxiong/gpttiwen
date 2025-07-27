#ifndef __RA_PAI_MAI_HANG_PARAM_HPP__
#define __RA_PAI_MAI_HANG_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"
#include "common/tlvprotocol.h"

#pragma pack(push, 4)

static const int MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_NUM = 10;				//每天最多商品数量
static const int MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM = 4;			//当前每天最多商品数量
UNSTD_STATIC_CHECK(MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_NUM >= MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM);
static const int MAX_PAI_MAI_HANG_RECORD_NUM = 30;						//竞拍记录数量
static const int MAX_NEXT_SAVE_DB_TIMESTAMP = 60;						//存储数据库时间间隔

enum RA_PAI_MAI_HANG_BID_STATUS
{
	RA_PAI_MAI_HANG_BID_STATUS_NOT_START = 0,		//竞拍还未开始
	RA_PAI_MAI_HANG_BID_STATUS_BIDDING = 1,			//竞拍进行中
	RA_PAI_MAI_HANG_BID_STATUS_END = 2,				//竞拍结束
};

struct RAPaiMaiHangSingleShopItemInfo
{
	RAPaiMaiHangSingleShopItemInfo() {}

	void Reset()
	{
		end_timestamp = 0;
		bid_price = 0;
		remain_deposit_gold = 0;
		add_price = 0;
		uid = 0;
		memset(role_name, 0, sizeof(role_name));
		is_show_user_flag = 0;
		is_big_end_flag = 0;
		is_auto_bid = 0;
		reserve_ch = 0;
	}

	unsigned int end_timestamp;		//商品竞拍结束时间
	int bid_price;					//当前竞拍出价
	int remain_deposit_gold;		//剩余寄存元宝
	int add_price;					//加价金额
	int uid;						//当前竞拍的角色uid
	GameName role_name;				//当前竞拍的角色名字
	char is_show_user_flag;			//当前竞拍角色信息是否显示 0:不显示 1:显示
	char is_big_end_flag;			//竞拍是否结算标识 0:未结算 1:已结算
	char is_auto_bid;				//是否自动竞价 0:不自动竞价
	char reserve_ch;
};

struct RAPaiMaiHangUserSingleItemInfo
{
	RAPaiMaiHangUserSingleItemInfo() { this->Reset(); }

	void Reset()
	{
		last_bid_gold = 0;
		bid_price = 0;
		add_price = 0;
		retrieve_gold = 0;
		is_show_user_flag = 0;
		is_auto_bid = 0;
		reserve_sh = 0;
	}
	void DayReset()
	{
		last_bid_gold = 0;
		bid_price = 0;
		add_price = 0;
		retrieve_gold = 0;
		is_auto_bid = 0;
		reserve_sh = 0;
	}
	void RetReset()
	{
		bid_price = 0;
		add_price = 0;
		retrieve_gold = 0;
	}

	int last_bid_gold;				//记录上次出价
	int bid_price;					//寄存数量(取回时该值会变化)
	int add_price;					//设置加价金额
	int retrieve_gold;				//已取回数量
	char is_show_user_flag;			//当前竞拍角色信息是否显示 0:不显示 1:显示
	char is_auto_bid;				//是否自动竞价 0:不自动竞价
	short reserve_sh;
};

//拍卖行
struct RARolePaiMaiHangParam
{
	RARolePaiMaiHangParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		for (int i = 0; i < ARRAY_LENGTH(shop_item_list); i++)
		{
			shop_item_list[i].Reset();
		}
	}

	unsigned int ra_begin_timestamp;
	RAPaiMaiHangUserSingleItemInfo shop_item_list[MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_NUM];			//竞拍出价
};
UNSTD_STATIC_CHECK(sizeof(RARolePaiMaiHangParam) <= sizeof(RoleActivityRoleData));

enum RA_PAI_HANG_BANG_RECORD_TYPE
{
	RA_PAI_HANG_BANG_RECORD_TYPE_BID = 0,					//出价竞拍 param1:bid_price 
	RA_PAI_HANG_BANG_RECORD_TYPE_BID_FAILD = 1,				//竞拍失败
	RA_PAI_HANG_BANG_RECORD_TYPE_AUTO_BID = 2,				//自动竞拍 param1:bid_price param2:remain_deposit_gold
	RA_PAI_HANG_BANG_RECORD_TYPE_DEPOSIT_SUCC = 3,			//寄存成功 param1:add_deposit_gold
	RA_PAI_HANG_BANG_RECORD_TYPE_FETCH_DEPOSIT = 4,			//取回寄存 param1:retrieve_gold 

	RA_PAI_HANG_BANG_RECORD_TYPE_MAX
};

struct RAPaiMaiHangRecordParam
{
	RAPaiMaiHangRecordParam() {}

	bool operator < (const RAPaiMaiHangRecordParam & data)const
	{
		if (record_timestamp == 0)
		{
			return false;
		}
		if (data.record_timestamp == 0)
		{
			return true;
		}
		if (record_timestamp < data.record_timestamp)
		{
			return false;
		}
		return activity_open_day < data.activity_open_day;
	}

	void Reset()
	{
		type = 0;
		seq = 0;
		activity_open_day = 0;
		param1 = 0;
		param2 = 0;
		param3 = 0;
		record_timestamp = 0;
	}
	short type;
	short seq;
	int activity_open_day;
	int param1;
	int param2;
	int param3;
	unsigned int record_timestamp;
};

#pragma pack(pop)

struct RARoleMaiHangDBItem
{
	RARoleMaiHangDBItem() { this->Reset(); }
	
	bool IsValid()
	{
		return uid > 0;
	}

	void Reset()
	{
		change_state = 0;
		uid = 0;
		record_count = 0;
		for (int i = 0; i < ARRAY_LENGTH(record_list); i++)
		{
			record_list[i].Reset();
		}
	}

	char change_state;
	int uid;
	int record_count;
	RAPaiMaiHangRecordParam record_list[MAX_PAI_MAI_HANG_RECORD_NUM];
};

typedef char RAPaiMaiHangRecordParamHex[sizeof(RAPaiMaiHangRecordParam) * MAX_PAI_MAI_HANG_RECORD_NUM * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RAPaiMaiHangRecordParamHex) < 2048);

struct RARoleParMaiHangDBParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	std::map<int, RARoleMaiHangDBItem> data_list;
};


#endif //__RA_PAI_MAI_HANG_PARAM_HPP__
