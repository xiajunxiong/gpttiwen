#ifndef __MSG_RA_YUN_ZE_CUTE_PET_HPP__
#define __MSG_RA_YUN_ZE_CUTE_PET_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rayunzecutepetparam.hpp"


#pragma pack(push, 4)

namespace Protocol
{
	
	class SCRAYunZeCutePetInfo		//!< 3134 ����-�ȳ�ݵ���Ϣ�·�
	{
	public:
		SCRAYunZeCutePetInfo();
		MessageHeader header;

		int pet_combat_num;					// ����ս����
		int today_fraction[RAYZCutePet::ACTIVITY_DAY_MAX];	 //��ʷÿ�ջ��ս��[������ʱ��--12]															
		int task_param_list[RAYZCutePet::TASK_TYPE_MAX];	 //�������[��������], ÿ������÷�															
		BitMap<RAYZCutePet::DAY_TASK_COUNT_MAX> task_status; //�������״̬
		
	};

	class SCRAYunZeCutePetChallengeInfo		//!< 3136 ����-�ȳ�ݵ���ս��Ϣ�·�
	{
	public:
		SCRAYunZeCutePetChallengeInfo();
		MessageHeader header;

		int is_challenge_status;		// �����Ƿ���ս�ɹ�, -1--��¼�·�,0--ʧ�ܣ�1--�ɹ���2--ƽ��
		BitMap<RAYZCutePet::MONSTER_MAX_NUM + 1> challenge_status; //�ȳ���ս�ɹ�״̬[1-10]--��ǰ�±�λ�Ƿ���ս�ɹ���
	};

}


#pragma pack(pop)

#endif
