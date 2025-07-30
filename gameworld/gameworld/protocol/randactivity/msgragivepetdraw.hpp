#ifndef __MSG_RA_GIVE_PET_DRAW_HPP__
#define __MSG_RA_GIVE_PET_DRAW_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ragivepetdrawparam.hpp"

//////////////////////////////////////////// 2090 ���� - ��30���飨��Ѱ�����飩  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_GIVE_PET_DRAW_OPERA_TYPE
	{
		RA_GIVE_PET_DRAW_OPERA_TYPE_INFO = 0,					//��Ϣ�·�	
		RA_GIVE_PET_DRAW_OPERA_TYPE_RECEIVE = 1,				//��ȡ
	};

	struct SCRAGivePetDrawInfo        // 3042 ��30������Ϣ
	{
	public:
		SCRAGivePetDrawInfo();
		MessageHeader header;

		int give_mark;				//0:δ��ȡ 1:����ȡ
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_GIVE_PET_DRAW_HPP__



