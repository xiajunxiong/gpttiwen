#ifndef __MSG_RA_TIGER_FISH_EVERY_YEAR_HPP__
#define __MSG_RA_TIGER_FISH_EVERY_YEAR_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ratigerfisheveryyearparam.hpp"

#pragma pack(push,4)

namespace Protocol
{

	class SCRATigerFishEveryYearInfo	//!< 3138 ����-�������� ��Ϣ�·�
	{
	public:
		SCRATigerFishEveryYearInfo();
		MessageHeader header;

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
	};

}


#pragma pack(pop)

#endif