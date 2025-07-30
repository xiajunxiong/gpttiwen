#ifndef __MSG_INCOME_HPP__
#define __MSG_INCOME_HPP__


#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/incomedef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	////// ����ͳ��
	//��Ҳ���		4460
	class CSIncomeRoleReq
	{
	public:
		CSIncomeRoleReq();
		MessageHeader		header;

		enum ReqType
		{
			REQ_TYPE_ROLE_INFO = 0,		//��ɫ����
		};

		int req_type;		//��������
	};

	//�����Ϣ		4461
	class SCIncomeRoleInfo
	{
	public:
		SCIncomeRoleInfo();
		MessageHeader		header;

		int role_count;
		long long role_param_list_[INCOME_ROLE_TYPE_MAX];
		int global_count;
		long long global_param_list_[INCOME_ROLE_TYPE_MAX];
	};
}


#pragma pack(pop)

#endif