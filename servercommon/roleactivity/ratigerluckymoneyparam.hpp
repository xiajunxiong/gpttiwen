#ifndef __RA_TIGER_LUCKY_MONEY_PARAM_HPP__
#define __RA_TIGER_LUCKY_MONEY_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

namespace RATLM
{
	enum LUCKY_MONEY_TYPE		// ѹ��Ǯ��������
	{
		LUCKY_MONEY_TYPE_GOLD = 0,	// Ԫ��
		LUCKY_MONEY_TYPE_ITEM = 1,	// ����
		LUCKY_MONEY_TYPE_JADE = 2,	// ����

		LUCKY_MONEY_TYPE_MAX,
	};

	enum RATLM_TASK_TYPE			// �����������
	{
		RATLM_TASK_TYPE_JING_JI_CHANG = 0,			//!< ������
		RATLM_TASK_TYPE_QIONG_DING_ZHI_ZHENG = 1,	//!< 񷶥֮��
		RATLM_TASK_TYPE_WU_FANG_LEI_TAI = 2,		//!< �巽��̨
		RATLM_TASK_TYPE_XIAN_SHI_HUO_DONG = 3,		//!< ������ʱ�	[ħ������,�������,��һ�а�,�ٹ�ҹ��,������Ϯ,�Թ�����,��������ս,�ý�ս��,��ϯ��ѡ,������Ҷ�,�����Կ���]
		RATLM_TASK_TYPE_CONSUME_TI_LI = 4,			//!< ʹ������
		RATLM_TASK_TYPE_JING_YING_SHI_LIAN = 5,		//!< ��Ӣ����
		RATLM_TASK_TYPE_YE_WAI_SHOU_LING = 6,		//!< Ұ������
		RATLM_TASK_TYPE_YUAN_ZHENG_MO_ZHONG = 7,	//!< Զ��ħڣ
		RATLM_TASK_TYPE_BEI_DOU_QI_XING = 8,		//!< ��������
		RATLM_TASK_TYPE_ONLINE_TIME = 9,			//!< ����ʱ��
		RATLM_TASK_TYPE_WAN_LING_HUA_BEN = 10,		//!< ���黰��

		RATLM_TASK_TYPE_MAX,
	};
	UNSTD_STATIC_CHECK(RATLM_TASK_TYPE_MAX <= 32);

};

#pragma pack(push, 4)

struct RATigerLuckyMoneyParam
{
	RATigerLuckyMoneyParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		ra_online_time_timestamp = 0;
		is_can_receive = 0;
		lucky_money_num = 0;
		role_level = 0;
		task_status.Reset();
		task_reward_status.Reset();
		memset(task_extent, 0, sizeof(task_extent));
	}
	
	void DayReset()
	{
		ra_online_time_timestamp = 0;
		task_status.Reset();
		task_reward_status.Reset();
		memset(task_extent, 0, sizeof(task_extent));
	}

	unsigned int ra_begin_timestamp;

	// ѹ��Ǯ�ۼ�����
	int lucky_money_num;

	// ѹ��Ǯ�Ƿ����ȡ,0-������ȡ
	short is_can_receive;

	// ��ҵ��յ�¼ʱ�ȼ�
	short role_level;

	// ÿ�������Ƿ���� [0-RATLM_TASK_TYPE_MAX(9)]
	BitMap<RATLM::RATLM_TASK_TYPE_MAX> task_status;

	// ÿ���������Ƿ�����ȡ[0-RATLM_TASK_TYPE_MAX(9)]
	BitMap<RATLM::RATLM_TASK_TYPE_MAX> task_reward_status;

	// ÿ��������� [0-RATLM_TASK_TYPE_MAX(9)]
	int task_extent[RATLM::RATLM_TASK_TYPE_MAX];

	// �������ʱ������ʱ���
	unsigned int ra_online_time_timestamp;

};
UNSTD_STATIC_CHECK(sizeof(RATigerLuckyMoneyParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif