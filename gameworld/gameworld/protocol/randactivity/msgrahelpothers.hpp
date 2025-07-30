#ifndef __MSG_RA_HELP_OTHERS_HPP__
#define __MSG_RA_HELP_OTHERS_HPP__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{

	// ��Э��3000
	enum RA_HELP_OTHERS_REQ_TYPE
	{
		RA_HELP_OTHERS_REQ_TYPE_START = 0,			//!< ��ɿͻ���α����
		RA_HELP_OTHERS_REQ_TYPE_ACCEPT = 1,			//!< ��ȡ
		RA_HELP_OTHERS_REQ_TYPE_DELIVERY = 2,		//!< ����	param1 : column param2 : index
		RA_HELP_OTHERS_REQ_TYPE_TALK = 3,			//!< ��̸
		RA_HELP_OTHERS_REQ_TYPE_DELIVERY_PET = 4,	//!< �Ͻ����� param1 : pet_index
		RA_HELP_OTHERS_REQ_TYPE_BATTLE_FINISH = 5,	//!< ������,�ύ����
	};

	class SCRAHelpOthersInfo			//!< 3072  ����-�嵨������Ϣ�·�
	{
	public:
		SCRAHelpOthersInfo();
		MessageHeader header;

		short pass_count;				//
		short task_id;					//����ID
		short task_status;				//����״̬ HELP_OTHERS_TASK_TYPE
		short progress;					//�����������ʱ��Ҫ
	};
}

#pragma pack(pop)

#endif	//__MSG_RA_HELP_OTHERS_HPP__