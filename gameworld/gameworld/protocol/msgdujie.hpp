#ifndef __MSG_DU_JIE_H__
#define __MSG_DU_JIE_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commondata.hpp"
#include "servercommon/servercommon.h"
#include "gamedef.h"

#include "servercommon/dujiedef.hpp"

#pragma pack(push) 
#pragma pack(4)


namespace Protocol
{
	//////�ɽ�
	//�ɽ� �û�����	3935
	class CSDuJieReq
	{
	public:
		CSDuJieReq();
		MessageHeader		header;

		enum ReqType
		{
			REQ_TYPE_ROLE_INFO = 0,		//��ɫ����
		};

		int req_type;		//��������
		int param_1;
	};

	//��ɫ��Ϣ  3936
	class SCDuJieRoleInfo
	{
	public:
		SCDuJieRoleInfo();
		MessageHeader		header;
		
		int max_pass_fb_seq;			//��߹ؿ�
		int next_front_gate_pass;		//ǰ���������(��߹ؿ���һ�ص�)

		int day_assist_times;
	};

	//������Ϣ  3937
	class SCDuJieFBInfo
	{
	public:
		SCDuJieFBInfo();
		MessageHeader		header;

		int m_fb_level;		//����level
		int m_is_finish;	//������ɱ��
	};
};

#pragma pack(pop)

#endif