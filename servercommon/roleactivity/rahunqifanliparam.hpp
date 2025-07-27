#ifndef __RA_ACTIVITY_HUN_QI_FAN_LI_PARAM_HPP__
#define __RA_ACTIVITY_HUN_QI_FAN_LI_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


#pragma pack(push,4)

////////////////////////////////// �������� ///////////////////////////////////////
static const int MAX_RA_HUN_QI_FAN_LI_BITMIP = 32;

struct RAHunQiFanLiParam
{
	RAHunQiFanLiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		m_last_lottery_count = 0;
		m_today_recharge = 0;
		m_total_lottery_count = 0;
		m_recharge_reward_flag.Reset();
		m_lottery_reward_flag.Reset();
		m_cur_rotary_table_state.Reset();
	}

	void DayReset()
	{
		m_today_recharge = 0;
		m_recharge_reward_flag.Reset();
	}

	unsigned int ra_begin_timestamp;

	uint32_t m_last_lottery_count;	// ���ʣ��齱����
	int m_total_lottery_count;		// ����ܳ齱����
	int m_today_recharge;			// ���ճ�ֵ���
	BitMap<MAX_RA_HUN_QI_FAN_LI_BITMIP> m_recharge_reward_flag;		// ��ֵ������ȡflag
	BitMap<MAX_RA_HUN_QI_FAN_LI_BITMIP> m_lottery_reward_flag;		// �齱����������ȡflag
	BitMap<MAX_RA_HUN_QI_FAN_LI_BITMIP> m_cur_rotary_table_state;	// ��ǰ����״̬, �ѳ鵽���Ľ�����Ϊ1,ȫ�����ˢ��
};
UNSTD_STATIC_CHECK(sizeof(RAHunQiFanLiParam) <= sizeof(RoleActivityRoleData));

////////////////////////////////// ����ҫ�� ///////////////////////////////////////

static const int MAX_RA_TIAN_YI_YAO_SHI_LOG = 50;	// ��¼50���齱��־
static const int MAX_RA_TIAN_YI_YAO_SHI_BUY_TYPE = 10;// �̳����������

enum RA_TIAN_YI_YAO_SHI_BUY_LIMIT
{
	RA_TIAN_YI_YAO_SHI_BUY_LIMIT_TOTAL = 1,	// ���޹�
	RA_TIAN_YI_YAO_SHI_BUY_LIMIT_TODAY = 2,	// ÿ���޹�

	RA_TIAN_YI_YAO_SHI_BUY_LIMIT_MAX,
};

// ����ҫ����־
struct RATianYiYaoShiLOG
{
	RATianYiYaoShiLOG() { this->Reset(); }

	void Reset()
	{
		lottery_timestamp = 0;
		item_id = 0;
		num = 0;
	}

	uint32_t lottery_timestamp;		// �齱ʱ��ʱ���
	ItemID item_id;
	uint16_t num;
};

struct RATianYiYaoShiParam
{
	RATianYiYaoShiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		cur_log_tail = 0;
		memset(m_buy_count, 0, sizeof(m_buy_count));

		for (int i = 0; i < MAX_RA_TIAN_YI_YAO_SHI_LOG; i++)
		{
			m_lottery_log[i].Reset();
		}
	}


	unsigned int ra_begin_timestamp;

	uint32_t cur_log_tail;		// ��ǰ��־β��(��һ��Ҫ���滻��index)

	uint16_t m_buy_count[MAX_RA_TIAN_YI_YAO_SHI_BUY_TYPE];		// �����������
	RATianYiYaoShiLOG m_lottery_log[MAX_RA_TIAN_YI_YAO_SHI_LOG];	// �齱��־
};
UNSTD_STATIC_CHECK(sizeof(RATianYiYaoShiParam) <= sizeof(RoleActivityRoleData));


#pragma pack(pop)
#endif