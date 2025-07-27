#ifndef __RA_COW_PARAM_HPP__
#define __RA_COW_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////

const static int MAX_COW_LOG_TIMES = 20;	//����¼����
const static int MAX_MAP_COUNT = 2;		//��ͼ��
const static int MAX_ONE_MAP_INDEX = 23;//һ�ŵ�ͼ��������
const static int SHOW_TAMP = 5;			//չʾʱ�������

const int MAX_COST_ITEM_COUNT = 5;//������ĵ�������--ţ�˵�ͷ

union KeyChangeUn
{
	int section[2];
	long long key;
};

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RACowLuckyExchangeParam
{
	RACowLuckyExchangeParam() { this->Reset(); }

	void Reset()
	{
		open_day = 0;
		begin_time = 0;
		initmacy = 0;
	}

	int open_day;
	unsigned int begin_time;
	int initmacy;
};

UNSTD_STATIC_CHECK(sizeof(RACowLuckyExchangeParam) <= sizeof(RoleActivityRoleData));

struct RACowStoreParam
{
	RACowStoreParam() { this->Reset(); }

	void Reset()
	{
		open_day = 0;
		begin_time = 0;
		memset(today_buy_times, 0, sizeof(today_buy_times));
		memset(activity_buy_times, 0, sizeof(activity_buy_times));
	}

	void DayReset()
	{
		memset(today_buy_times, 0, sizeof(today_buy_times));
	}

	int open_day;
	unsigned int begin_time;
	short today_buy_times[MAX_COW_LOG_TIMES];//���칺�����
	short activity_buy_times[MAX_COW_LOG_TIMES];//��������
};

UNSTD_STATIC_CHECK(sizeof(RACowStoreParam) <= sizeof(RoleActivityRoleData));

struct RACowExchangeParam
{
	RACowExchangeParam() { this->Reset(); }

	void Reset()
	{
		open_day = 0;
		begin_time = 0;
		m_unlock_integral = 0;
		m_jifen = 0;
		memset(today_buy_times, 0, sizeof(today_buy_times));
		memset(activity_buy_times, 0, sizeof(activity_buy_times));
	}

	void DayReset()
	{
		memset(today_buy_times, 0, sizeof(today_buy_times));
	}

	int open_day;
	unsigned int begin_time;
	int m_unlock_integral;//�ܻ���
	int m_jifen;//���л���
	short today_buy_times[MAX_COW_LOG_TIMES];//���칺�����
	short activity_buy_times[MAX_COW_LOG_TIMES];//��������
};

UNSTD_STATIC_CHECK(sizeof(RACowExchangeParam) <= sizeof(RoleActivityRoleData));

struct RAMonpolyGiftParam
{
	RAMonpolyGiftParam() { this->Reset(); }

	void Reset()
	{
		open_day = 0;
		begin_time = 0;
		m_show_tamp = 0;
		now_map_id = 0;
		now_map_index = 0;
		has_play_special_plan = 0;
		question_id = 0;
		memset(today_buy_times, 0, sizeof(today_buy_times));
	}

	void OnDayChnage()
	{
		memset(today_buy_times, 0, sizeof(today_buy_times));
	}

	int open_day;
	unsigned int begin_time;

	unsigned int m_show_tamp;
	int now_map_id;		// ��ͼid
	int now_map_index;	// �����ڵ�ͼ����һ����
	int has_play_special_plan;//�����淨�Ƿ�ִ�й��ˣ������Ƿ���ȡ���ˣ�
	int question_id;

	short today_buy_times[MAX_COW_LOG_TIMES];//���칺�����
};

UNSTD_STATIC_CHECK(sizeof(RAMonpolyGiftParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)


#endif	//__RA_DISCOUNT_STORE_PARAM_HPP__
