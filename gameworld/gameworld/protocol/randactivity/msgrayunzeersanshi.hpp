#ifndef __MSG_RA_YUN_ZE_ER_SAN_SHI_HPP__
#define __MSG_RA_YUN_ZE_ER_SAN_SHI_HPP__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{

	class SCRAYunZeErSanShiInfo				//!< 3120 ����-�����������Ϣ�·�
	{
	public:
		SCRAYunZeErSanShiInfo();
		MessageHeader header;

		short times;
		short is_turntable_reward;			// �Ƿ���ת�̽���
		int task_id;		
	};

	class SCRAYunZeErSanShiTurntableInfo			//!< 3126 ����-���������ת����Ϣ�·�
	{
	public:
		SCRAYunZeErSanShiTurntableInfo();
		MessageHeader header;

		int seq;
	};
}

#pragma pack(pop)

#endif