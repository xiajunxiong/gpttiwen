#ifndef __MSG_RA_YUN_ZE_SLOT_MACHINE_HPP__
#define __MSG_RA_YUN_ZE_SLOT_MACHINE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rayunzeslotmachineparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_YUN_ZE_SLOT_MACHINE_OPERA_TYPE
	{
		RA_YUN_ZE_SLOT_MACHINE_TYPE_GET_INFO = 0,		// ������Ϣ�·�
		RA_YUN_ZE_SLOT_MACHINE_TYPE_LOTTERY = 1,		// ����齱
		RA_YUN_ZE_SLOT_MACHINE_TYPE_LOCK_REWARD = 2,	// ������������ param1 - �������
		RA_YUN_ZE_SLOT_MACHINE_TYPE_BUY = 3,			// ������
	};

	class SCRAYunZeSlotMachineInfo			//!< 3118 ����-����ͬ������Ϣ�·�
	{
	public:
		SCRAYunZeSlotMachineInfo();				
		MessageHeader header;

		short buy_times;				// �齱�����ѹ������
		short surplus_key;				// ���Կ��ʣ������
		int lock_reward[YUN_ZE_SLOT_NACHINE_LOCK_REWARD_TYPE];		// MAXΪ3��ÿ��λ�ô�һ����������Ľ���λ
	};

	class SCRAYunZeSlotMachineResultInfo	//!< 3119 ����-����ͬ����ҡ�������Ϣ�·�
	{
	public:
		SCRAYunZeSlotMachineResultInfo();
		MessageHeader header;

		int rand_result;			//ҡ�����1-4
	};

}

#pragma pack(pop)

#endif