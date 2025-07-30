#ifndef __MSG_TIAN_GONG_XUN_BAO_HPP__
#define __MSG_TIAN_GONG_XUN_BAO_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)


namespace Protocol
{
	class CSTianGongXunBaoClientOpReq		//!<4735 天宫寻宝操作请求
	{
	public:
		CSTianGongXunBaoClientOpReq();
		MessageHeader header;

		int opera_req_type;
	};

	class SCTianGongXunBaoGridInfo			//!<4736 天宫寻宝信息下发
	{
	public:
		SCTianGongXunBaoGridInfo();
		MessageHeader header;

		int cur_grid;
		int used_free;		// 免费次数是否使用了
	};



}

#pragma pack(pop)

#endif
