#ifndef __RA_TIGER_FISH_EVERY_YEAR_PARAM_HPP__
#define __RA_TIGER_FISH_EVERY_YEAR_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

namespace RATIGERFEY
{
	static const int MAX_REWARD_STAGE = 32;
	static const int MAX_LEVEL = 120;

	enum RA_TIRGER_FISH_EVERY_YEAR_TYPE
	{
		TIRGER_FEY_TYPE_JING_JI_CHANG = 0,			//!< ������
		TIRGER_FEY_TYPE_QIONG_DING_ZHI_ZHENG = 1,	//!< 񷶥֮��
		TIRGER_FEY_TYPE_WU_FANG_LEI_TAI = 2,		//!< �巽��̨
		TIRGER_FEY_TYPE_XIAN_SHI_HUO_DONG = 3,		//!< ������ʱ�	[ħ������,�������,��һ�а�,�ٹ�ҹ��,������Ϯ,�Թ�����,��������ս,�ý�ս��,��ϯ��ѡ,������Ҷ�,�����Կ���]
		TIRGER_FEY_TYPE_CONSUME_TI_LI = 4,			//!< ʹ������
		TIRGER_FEY_TYPE_JING_YING_SHI_LIAN = 5,		//!< ��Ӣ����
		TIRGER_FEY_TYPE_YE_WAI_SHOU_LING = 6,		//!< Ұ������
		TIRGER_FEY_TYPE_YUAN_ZHENG_MO_ZHONG = 7,	//!< Զ��ħڣ
		TIRGER_FEY_TYPE_BEI_DOU_QI_XING = 8,		//!< ��������

		TIRGER_FEY_TYPE_MAX,
	};
	UNSTD_STATIC_CHECK(TIRGER_FEY_TYPE_MAX <= 32);

};

#pragma pack(push,4)

struct RATigerFishEveryYearParam
{
	RATigerFishEveryYearParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		is_accept_task = 0;
		fish_num = 0;
		role_level = 0;
		open_activity_level = 0;

		task_status.Reset();
		reward_stage_status.Reset();
		is_receive_reward.Reset();
		task_reward_status.Reset();
		memset(task_extent, 0, sizeof(task_extent));
	}

	void DayReset()
	{
		is_accept_task = 0;
		role_level = 0;

		task_status.Reset();
		task_reward_status.Reset();
		memset(task_extent, 0, sizeof(task_extent));
	}

	unsigned int ra_begin_timestamp;

	// ÿ���Ƿ��ȡ���������0--δ��ȡ��1--�ѽ�ȡ
	short is_accept_task;

	// ��ҵ��ս�����ʱ�ȼ�
	short role_level;

	// ���ӵ�е�����
	int fish_num;

	// ÿ�������Ƿ���� [0-TIRGER_FEY_TYPE_MAX(9)]
	BitMap<RATIGERFEY::TIRGER_FEY_TYPE_MAX> task_status;

	// ÿ���������Ƿ�����ȡ[0-TIRGER_FEY_TYPE_MAX(9)]
	BitMap<RATIGERFEY::TIRGER_FEY_TYPE_MAX> task_reward_status;

	// ÿ��������� [0-TIRGER_FEY_TYPE_MAX(9)]
	int task_extent[RATIGERFEY::TIRGER_FEY_TYPE_MAX];

	// ������λ�Ƿ����ȡ[0-31]
	BitMap<RATIGERFEY::MAX_REWARD_STAGE> is_receive_reward;

	// ������λ�Ƿ�����ȡ[0-31]
	BitMap<RATIGERFEY::MAX_REWARD_STAGE> reward_stage_status;

	int open_activity_level;	//�����ʱ��ҵȼ�
};
UNSTD_STATIC_CHECK(sizeof(RATigerFishEveryYearParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif