#ifndef __MSG_RA_LING_PAI_TIAN_CI_HPP__
#define __MSG_RA_LING_PAI_TIAN_CI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2085  ����  ���Ʊ���  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRALingPaiTianCiInfo						//<! 3810  �������
	{
	public:
		SCRALingPaiTianCiInfo();
		MessageHeader header;

		int chong_zhi_num;				//��ǰ��ۼƳ�ֵ���
		unsigned int fetch_flag;			//��ȡ������־
	};

	struct SCRALingPaiTianCi2Info						//<! 3811  �����ķ�(�������2)
	{
	public:
		SCRALingPaiTianCi2Info();
		MessageHeader header;

		int chong_zhi_num;				//��ǰ��ۼƳ�ֵ���
		unsigned int fetch_flag;			//��ȡ������־
	};

	struct SCRALingPaiTianCi3Info						//<! 3812  ��������(�������3)
	{
	public:
		SCRALingPaiTianCi3Info();
		MessageHeader header;

		int chong_zhi_num;				//��ǰ��ۼƳ�ֵ���
		unsigned int fetch_flag;			//��ȡ������־
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
