#ifndef __MSG_TIAN_DI_DAO_HNE_HPP__
#define __MSG_TIAN_DI_DAO_HNE_HPP__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum TIAN_DI_DAO_HEN_REQ_TYPE
	{
		TIAN_DI_DAO_HEN_REQ_TYPE_ACTIVE,   // param1 0~3��ˮ���أ�
		TIAN_DI_DAO_HEN_REQ_TYPE_FETCH,	//��ȡ4�������Ľ���
	};

	class CSTianDiDaoHenReq			//!< 4040
	{
	public:
		CSTianDiDaoHenReq();
		MessageHeader header;

		int req_type;
		int param1;
		int param2;
	};

	class SCTianDiDaoHenRoleInfo		//!< 	4041 
	{
	public:
		SCTianDiDaoHenRoleInfo();
		MessageHeader header;

		short seq;					//�������������seq
		short is_fetch_reward;		//��ȡ4�������Ľ��� (��0�Ѿ���ȡ)
		int active_flag;				// param1 0~3��ˮ���أ���־ ������
	};

	class SCTianDiDaoHenOpenInfo		//!< 	4042 ��ص��ۿ�����Ϣ , ��������˲Ż��·�
	{
	public:
		SCTianDiDaoHenOpenInfo();
		MessageHeader header;

		int is_open;
		unsigned int next_state_time;
	};

}

#pragma pack(pop)

#endif