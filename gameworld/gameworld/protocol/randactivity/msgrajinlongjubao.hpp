#ifndef __MSG_RA_JIN_LONG_JU_BAO_HPP__
#define __MSG_RA_JIN_LONG_JU_BAO_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2208  ����  �����۱�  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	enum JIN_LONG_JU_BAO_INFO_RESON
	{
		JIN_LONG_JU_BAO_INFO_RESON_DEFAULT,
		JIN_LONG_JU_BAO_INFO_RESON_SET,		//����
		JIN_LONG_JU_BAO_INFO_RESON_FETCH,		//��ȡ��������
		JIN_LONG_JU_BAO_INFO_RESON_BUY,		//����
		JIN_LONG_JU_BAO_INFO_RESON_RESET,		//����
	};

	class SCRAJinLongJuBaoInfo						//<! 5115 �����۱���Ϣ
	{
	public:
		SCRAJinLongJuBaoInfo();
		MessageHeader header;

		int reason;
		unsigned short ra_day_buy_times[JIN_LONG_JU_BAO_MAX_BUY_ITEM];		//20 ÿ����Ʒÿ���޹�����
		unsigned short ra_buy_times[JIN_LONG_JU_BAO_MAX_BUY_ITEM];			//20 ÿ����Ʒ�޹�����
		int draw_times;						//   �齱����
		unsigned int times_reward_fetch_flag;					//   ����������ȡ��־
		RAJinLongJuBaoItem item_list_select[JIN_LONG_JU_BAO_SELECT_NUM_SELECT];	//4 ��ѡ�����б�
		RAJinLongJuBaoItem item_list_rand[JIN_LONG_JU_BAO_SELECT_NUM_RAND];		//8 ����ѡ�����б�
		RAJinLongJuBaoRecord record_list[JIN_LONG_JU_BAO_RECORD_NUM];
	};

	struct SCRAJinLongJuBaoDrawRetItem
	{
		unsigned int time;
		int times;		//��������
		int type;
		int index;
	};

	class SCRAJinLongJuBaoDrawRet						//<! 5116 �����۱��齱����
	{
	public:
		SCRAJinLongJuBaoDrawRet();
		MessageHeader header;

		int count;
		SCRAJinLongJuBaoDrawRetItem item_list[10];
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
