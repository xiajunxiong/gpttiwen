#ifndef __MSG_RA_HELP_OTHERS_2_HPP__
#define __MSG_RA_HELP_OTHERS_2_HPP__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{

	// ��Э��3000
	enum RA_HELP_OTHERS_REQ_TYPE_2
	{
		RA_HELP_OTHERS_REQ_TYPE_2_START = 0,			//!< ��ɿͻ���α����
		RA_HELP_OTHERS_REQ_TYPE_2_ACCEPT = 1,			//!< ��ȡ
		RA_HELP_OTHERS_REQ_TYPE_2_DELIVERY = 2,		//!< ����	param1 : column param2 : index
		RA_HELP_OTHERS_REQ_TYPE_2_TALK = 3,			//!< ��̸
		RA_HELP_OTHERS_REQ_TYPE_2_DELIVERY_PET = 4,	//!< �Ͻ����� param1 : pet_index
		RA_HELP_OTHERS_REQ_TYPE_2_BATTLE_FINISH = 5,	//!< ������,�ύ����
	};

	class SCRAHelpOthersInfo2			//!< 3091  ����-�嵨����2(��԰һ����)��Ϣ�·�
	{
	public:
		SCRAHelpOthersInfo2();
		MessageHeader header;

		short pass_count;				//
		short task_id;					//����ID
		short task_status;				//����״̬ HELP_OTHERS_TASK_TYPE
		short progress;					//�����������ʱ��Ҫ
	};
}

#pragma pack(pop)

#endif	//__MSG_RA_HELP_OTHERS_HPP__