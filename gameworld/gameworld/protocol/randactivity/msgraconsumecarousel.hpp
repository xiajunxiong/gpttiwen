#ifndef __MSG_RA_CONSUME_CAROUSEL_HPP__
#define __MSG_RA_CONSUME_CAROUSEL_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raconsumecarouselparam.hpp"

//////////////////////////////////////////// 2055 ����  ����ת��  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_CONSUME_CAROUSEL_OPERA_TYPE
	{
		RA_CONSUME_CAROUSEL_OPERA_TYPE_GET_INFO = 0,						// ��ȡ��Ϣ
		RA_CONSUME_CAROUSEL_OPERA_TYPE_CONSUME_ONE = 1,						// ����ת������  ת��1�� 
		RA_CONSUME_CAROUSEL_OPERA_TYPE_CONSUME_ALL = 2,						// ����ת������  ת��ȫ������
		RA_CONSUME_CAROUSEL_OPERA_TYPE_GET_REWARD = 3,						// ��ȡ����  param_1:���

	};


	struct SCRAConsumeCarouselInfo             //3008 ����ת����Ϣ
	{
	public:
		SCRAConsumeCarouselInfo();
		MessageHeader header;

		int last_num;													//ʣ�����
		unsigned int consume_num_day;									//������������
		unsigned int consume_count;										//�ۼ�������
	};

	struct SCRAConsumeCarouselReward		//3009 ����ת�̵���ת���·�
	{
	public:
		SCRAConsumeCarouselReward();
		MessageHeader header;

		int index;
	};

	struct SCRAConsumeCarouselShow			//3010 ����ת����ϡ��ʾ�·�
	{
	public:
		SCRAConsumeCarouselShow();
		MessageHeader header;

		int count;																					//��ǰ�Ѿ���ȡ������      0-10
		RAShowInfoParam::RAShowInfoDataParam consume_carousel_list[RA_SHOW_INFO_MAX_NUM];			//��ȡ����ϡ��Ʒ���б�    RA_SHOW_INFO_MAX_NUM -- 10
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_CONSUME_CAROUSEL_HPP__
