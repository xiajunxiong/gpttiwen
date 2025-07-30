#ifndef __MSG_RA_YUN_ZE_HAO_LI_HPP__
#define __MSG_RA_YUN_ZE_HAO_LI_HPP__

#include "servercommon/userprotocal/msgheader.h"



#pragma pack(push, 4)

namespace Protocol
{

	class SCRAYunZeHaoLiInfo		//!< 3133 ����-���������Ϣ�·�
	{
	public:
		SCRAYunZeHaoLiInfo();
		MessageHeader header;

		short buy_times;			// �ѹ������
		short prop_free_num;		// ����������
	};

}


#pragma pack(pop)

#endif
