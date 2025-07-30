#ifndef __MSG_RA_EXPEL_MONSTER_HPP__
#define __MSG_RA_EXPEL_MONSTER_HPP__

#include "servercommon/userprotocal/msgheader.h"

//////////////////////////////////////////// 2055 ����  ��������  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_EXPEL_MONSTER_OPERA_TYPE
	{
		RA_EXPEL_MONSTER_OPERA_TYPE_BUY = 0,						// ������Ʒ param_1 :��Ʒ���
 
	};

	struct SCRAExpelMonsterInfo			//<! 3015 ����������Ϣ
	{
	public:
		SCRAExpelMonsterInfo();
		MessageHeader header;

		int today_times;				//����������������
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_EXPEL_MONSTER_HPP__
