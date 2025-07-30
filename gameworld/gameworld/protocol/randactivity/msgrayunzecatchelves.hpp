#ifndef __MSG_RA_YUN_ZE_CATCH_ELVES_HPP__
#define __MSG_RA_YUN_ZE_CATCH_ELVES_HPP__

#include "servercommon/userprotocal/msgheader.h"



#pragma pack(push, 4)

static const int MAX_ELVES_NUM = 10;			// �����������

namespace Protocol
{

	class SCRAYunZeCatchElvesGetInfo			//!< 3135 ����-�����ץ��ץ����Ϣ�·�
	{
	public:
		SCRAYunZeCatchElvesGetInfo();
		MessageHeader header;

		int catch_elves_num[MAX_ELVES_NUM + 1];   //ֻ��elves_num���ȣ��±�0--�Ƿ�ץ�����⾫���־λ������id���±���ͬ
	};


}


#pragma pack(pop)

#endif
