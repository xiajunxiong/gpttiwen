#ifndef __MSG_RA_CHUN_RI_DENG_FENG_HPP__
#define __MSG_RA_CHUN_RI_DENG_FENG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2085  ����  ���Ʊ���  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAChunRiDengFengInfo						//<! 3611 ���յǷ�
	{
	public:
		SCRAChunRiDengFengInfo();
		MessageHeader header;

		unsigned int layer;				//��ǰ����
		unsigned int explore_total_times;		//̽���ܴ���							
		unsigned short explore_times;		//û������߲�ô������������ñ�ô���ֱ�ӵ����߲�
		unsigned short get_item_flag;		//��7�����ֻ�ɻ��һ�Σ������ظ���ã���¼���߲��ȡʲô����	
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
