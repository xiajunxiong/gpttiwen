#ifndef __MSG_INCOME_HPP__
#define __MSG_INCOME_HPP__


#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/incomedef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	////// 收益统计
	//玩家操作		4460
	class CSIncomeRoleReq
	{
	public:
		CSIncomeRoleReq();
		MessageHeader		header;

		enum ReqType
		{
			REQ_TYPE_ROLE_INFO = 0,		//角色数据
		};

		int req_type;		//操作类型
	};

	//玩家信息		4461
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