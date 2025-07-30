#ifndef __MSG_TIAN_GONG_XUN_BAO_HPP__
#define __MSG_TIAN_GONG_XUN_BAO_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)


namespace Protocol
{
	class CSTianGongXunBaoClientOpReq		//!<4735 �칬Ѱ����������
	{
	public:
		CSTianGongXunBaoClientOpReq();
		MessageHeader header;

		int opera_req_type;
	};

	class SCTianGongXunBaoGridInfo			//!<4736 �칬Ѱ����Ϣ�·�
	{
	public:
		SCTianGongXunBaoGridInfo();
		MessageHeader header;

		int cur_grid;
		int used_free;		// ��Ѵ����Ƿ�ʹ����
	};



}

#pragma pack(pop)

#endif
