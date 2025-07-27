#ifndef __RA_PAI_MAI_HANG_PARAM_HPP__
#define __RA_PAI_MAI_HANG_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"
#include "common/tlvprotocol.h"

#pragma pack(push, 4)

static const int MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_NUM = 10;				//ÿ�������Ʒ����
static const int MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM = 4;			//��ǰÿ�������Ʒ����
UNSTD_STATIC_CHECK(MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_NUM >= MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM);
static const int MAX_PAI_MAI_HANG_RECORD_NUM = 30;						//���ļ�¼����
static const int MAX_NEXT_SAVE_DB_TIMESTAMP = 60;						//�洢���ݿ�ʱ����

enum RA_PAI_MAI_HANG_BID_STATUS
{
	RA_PAI_MAI_HANG_BID_STATUS_NOT_START = 0,		//���Ļ�δ��ʼ
	RA_PAI_MAI_HANG_BID_STATUS_BIDDING = 1,			//���Ľ�����
	RA_PAI_MAI_HANG_BID_STATUS_END = 2,				//���Ľ���
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

	unsigned int end_timestamp;		//��Ʒ���Ľ���ʱ��
	int bid_price;					//��ǰ���ĳ���
	int remain_deposit_gold;		//ʣ��Ĵ�Ԫ��
	int add_price;					//�Ӽ۽��
	int uid;						//��ǰ���ĵĽ�ɫuid
	GameName role_name;				//��ǰ���ĵĽ�ɫ����
	char is_show_user_flag;			//��ǰ���Ľ�ɫ��Ϣ�Ƿ���ʾ 0:����ʾ 1:��ʾ
	char is_big_end_flag;			//�����Ƿ�����ʶ 0:δ���� 1:�ѽ���
	char is_auto_bid;				//�Ƿ��Զ����� 0:���Զ�����
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

	int last_bid_gold;				//��¼�ϴγ���
	int bid_price;					//�Ĵ�����(ȡ��ʱ��ֵ��仯)
	int add_price;					//���üӼ۽��
	int retrieve_gold;				//��ȡ������
	char is_show_user_flag;			//��ǰ���Ľ�ɫ��Ϣ�Ƿ���ʾ 0:����ʾ 1:��ʾ
	char is_auto_bid;				//�Ƿ��Զ����� 0:���Զ�����
	short reserve_sh;
};

//������
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
	RAPaiMaiHangUserSingleItemInfo shop_item_list[MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_NUM];			//���ĳ���
};
UNSTD_STATIC_CHECK(sizeof(RARolePaiMaiHangParam) <= sizeof(RoleActivityRoleData));

enum RA_PAI_HANG_BANG_RECORD_TYPE
{
	RA_PAI_HANG_BANG_RECORD_TYPE_BID = 0,					//���۾��� param1:bid_price 
	RA_PAI_HANG_BANG_RECORD_TYPE_BID_FAILD = 1,				//����ʧ��
	RA_PAI_HANG_BANG_RECORD_TYPE_AUTO_BID = 2,				//�Զ����� param1:bid_price param2:remain_deposit_gold
	RA_PAI_HANG_BANG_RECORD_TYPE_DEPOSIT_SUCC = 3,			//�Ĵ�ɹ� param1:add_deposit_gold
	RA_PAI_HANG_BANG_RECORD_TYPE_FETCH_DEPOSIT = 4,			//ȡ�ؼĴ� param1:retrieve_gold 

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
