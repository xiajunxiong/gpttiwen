#ifndef __MSG_RA_LING_YU_JI_JIN_HPP__
#define __MSG_RA_LING_YU_JI_JIN_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRALingYuJiJinInfo						//<! 3900 ���������Ϣ
	{
	public:
		SCRALingYuJiJinInfo();
		MessageHeader header;

		int active_flag;				//�����Ǹ��׶εı�־
		int fetch_flag[RA_LING_YU_JI_JIN_MAX_PHASE];		//ÿ���׶���ȡ������־
	};

}

#pragma pack(pop)

#endif  
