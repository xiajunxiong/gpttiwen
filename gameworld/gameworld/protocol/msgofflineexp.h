#ifndef __MSG_OFFLINE_EXP_H__
#define __MSG_OFFLINE_EXP_H__

#include "gamedef.h"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	class SCOfflineExpAddition
	{
	public:
		SCOfflineExpAddition();		//!< 1755 - ���߾����ۻ���Ϣ
		MessageHeader header;

		int offline_min;			//!< ����ʱ�� - ��λ�����ӣ�m��
		unsigned int exp_add;		//!< �ۻ�����
	};

	class CSOfflineExpReceives
	{
	public:
		CSOfflineExpReceives();		//!< 1756 - ���߾�����ȡ����
		MessageHeader header;
	};
}

#pragma pack(pop)
#endif