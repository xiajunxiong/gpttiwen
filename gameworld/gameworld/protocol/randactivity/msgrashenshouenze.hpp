#ifndef __MSG_RA_SHEN_SHOU_EN_ZE_HPP__
#define __MSG_RA_SHEN_SHOU_EN_ZE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2208  ����  �����۱�  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	class SCRAShenShouEnZeInfo						//<! 5180 ���޶�����Ϣ
	{
	public:
		SCRAShenShouEnZeInfo();
		MessageHeader header;

		unsigned short ra_day_buy_times[SHEN_SHOU_EN_ZE_MAX_BUY_ITEM];		//20 ÿ����Ʒÿ���޹�����
		unsigned short ra_buy_times[SHEN_SHOU_EN_ZE_MAX_BUY_ITEM];			//20 ÿ����Ʒ�޹�����
		int draw_times;						//   �齱����
		int is_select;					//�Ƿ�ѡ������Ʒ
		int select_reward_seq;				//ѡ����Ʒ��seq
	};

	struct RAShenShouEnZeDrawResultItem
	{
		int is_mail;
		int reward_seq;
	};

	struct SCRAShenShouEnZeDrawResult			 //<! 5181 ���޶���齱���
	{
	public:
		SCRAShenShouEnZeDrawResult();
		MessageHeader header;

		int draw_times;
		RAShenShouEnZeDrawResultItem reward_seq_list[10];
	};

	struct SCRAShenShouEnZeRecord			 //<! 5182 ���޶���齱��¼
	{
	public:
		SCRAShenShouEnZeRecord();
		MessageHeader header;

		RAShenShouEnZeRecord record_list[SHEN_SHOU_EN_ZE_RECORD_MAX_NUM];
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
