#ifndef __MSG_RA_YUN_ZE_BLOOMING_RECORD_HPP__
#define __MSG_RA_YUN_ZE_BLOOMING_RECORD_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rayunzebloomingrecordparam.hpp"

#pragma pack(push,4)

namespace Protocol
{

	class SCRAYunZeBloomingRecordInfo			//!< 3115 ����-���󷱻�¼��Ϣ�·�
	{
	public:
		SCRAYunZeBloomingRecordInfo();
		MessageHeader header;

		short times;						// ����ս����
		short cur_integral;					// ��ǰ��һ���
		int challenge_start_timestamp;		// ��ս��ʼʱ���,0-��ս����
		int reward_list[YUNZE_BLOOMING_RECORD_MAX_REWARD_NUM];	// �����Ƿ�����ȡ��0-δ��ȡ��1-����ȡ������5���±�0-4����1-5�ĺ��ӣ�
	};
}

#pragma pack(pop)

#endif // __MSG_RA_YUN_ZE_BLOOMING_RECORD_HPP__
