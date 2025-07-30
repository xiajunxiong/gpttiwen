#ifndef __MSG_TIAN_DI_DAO_HNE_HPP__
#define __MSG_TIAN_DI_DAO_HNE_HPP__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum TIAN_DI_DAO_HEN_REQ_TYPE
	{
		TIAN_DI_DAO_HEN_REQ_TYPE_ACTIVE,   // param1 0~3（水火风地）
		TIAN_DI_DAO_HEN_REQ_TYPE_FETCH,	//领取4个激活后的奖励
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

		short seq;					//今天随机的配置seq
		short is_fetch_reward;		//领取4个激活后的奖励 (非0已经领取)
		int active_flag;				// param1 0~3（水火风地）标志 二进制
	};

	class SCTianDiDaoHenOpenInfo		//!< 	4042 天地道痕开启信息 , 跨服开启了才会下发
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