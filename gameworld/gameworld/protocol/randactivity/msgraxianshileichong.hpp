#ifndef __MSG_RA_XIAN_SHI_LEI_CHONG_HPP__
#define __MSG_RA_XIAN_SHI_LEI_CHONG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raxianshileichongparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	//!<3000 �����������
	enum RA_XIAN_SHI_LEI_CHONG_REQ_TYPE						//��Ӧ 3000Э��� opera_type
	{
		RA_XIAN_SHI_LEI_CHONG_REQ_TYPE_INFO = 0,			//��ȡ���Ϣ
		RA_XIAN_SHI_LEI_CHONG_REQ_TYPE_GET_REWARD = 1,		//��ȡ����  param1:seq[��Ӧ���������seq]
	};

	struct SCRAXianShiLeiChongInfo							//<! 4331 ��ʱ�۳���Ϣ
	{
	public:
		SCRAXianShiLeiChongInfo();
		MessageHeader header;

		int open_day;										// �����ʱ�Ŀ�������
		int total_chong_zhi;								// ��ڼ��ۼƳ�ֵ���
		int can_fetch_flag;									// �ܹ���ȡ�ı��[��λ]
		int flag;											// �Ѿ���ȡ�˵ı��[��λ]
	};
}

#pragma pack(pop)

#endif  
