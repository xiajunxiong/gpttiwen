#ifndef __MSG_SHAN_HAI_QUESTION_HPP__ 
#define __MSG_SHAN_HAI_QUESTION_HPP__

#include "servercommon/userprotocal/msgheader.h"

//////////////////////////////////////////// 2140 ���� - ɽ���ʴ�  //////////////////////////////////////////


#pragma pack(push,4)


namespace Protocol
{

	struct SCRAShanHaiQuestionInfo			//!< 3092 ɽ���ʴ������·�
	{
	public:
		SCRAShanHaiQuestionInfo();
		MessageHeader header;

		short is_can_answer;		// �Ƿ��ڴ���ʱ����� 1-�ڣ�0-����
		short times;				// ���ʣ��������
		short question_id;			// ��Ŀid
		short reward_seq;			// ����������
	};

	struct SCRAShanHaiQuestionStatusInfo	//!< 3099 ɽ���ʴ����״̬��Ϣ�·�
	{
	public:
		SCRAShanHaiQuestionStatusInfo();
		MessageHeader header;

		int is_can_answer;		// �Ƿ��ڴ���ʱ����� 1-�ڣ�0-����
	};

}

#pragma pack(pop)

#endif
