#ifndef __MSG_RA_QI_XI_TE_HUI_HPP__
#define __MSG_RA_QI_XI_TE_HUI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raqixitehuiparam.hpp"

//////////////////////////////////////////// 2209  ����  ��Ϧ�ػ�  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_QI_XI_TE_HUI_OPERA_TYPE
	{
		RA_QI_XI_TE_HUI_OPERA_TYPE_INFO = 0,					//��ȡ��Ϣ
		RA_QI_XI_TE_HUI_OPERA_TYPE_BUY_ITEM = 1,				//������� param1 ����seq
		RA_QI_XI_TE_HUI_OPERA_TYPE_CANCEL_RED_DOT = 2,			//ȡ�����(���޳�ʼ�򿪽���)
		RA_QI_XI_TE_HUI_OPERA_TYPE_FETCH_PHASE_REWARD = 3,		//��ȡ�׶ν��� param1:seq
	};

	struct SCRAQiXiTeHuiInfo			//<! 3187 ��Ϧ�ػ���Ϣ
	{
	public:
		SCRAQiXiTeHuiInfo();
		MessageHeader header;

		char is_init_open_interface_flag;									//�Ƿ��ʼ�򿪹����� 1:�Ѵ�
		char reserve_ch;
		short activity_buy_count;											//��ڼ乺����ܴ���
		BitMap<RA_QI_XI_TE_HUI_MAX_PHASE_NUM> phase_reward_flag;			//�׶ν�����ȡ��ʶ
		short today_buy_times[RA_QI_XI_TE_HUI_MAX_BASE_NUM];				//ÿ���޹�
		short activity_buy_times[RA_QI_XI_TE_HUI_MAX_BASE_NUM];				//��޹�
	};

	class SCRAQiXiTeHuiSingleInfo		//!< 3188 ����-��Ϧ�ػݵ�����Ϣ�·�
	{
	public:
		SCRAQiXiTeHuiSingleInfo();
		MessageHeader header;

		short activity_buy_count;
		short seq;
		short today_buy_times;
		short activity_buy_times;
	};

	struct SCRAQiXiTeHuiOtherInfo			//<! 3189 ��Ϧ�ػ�������Ϣ
	{
	public:
		SCRAQiXiTeHuiOtherInfo();
		MessageHeader header;

		char is_init_open_interface_flag;									//�Ƿ��ʼ�򿪹����� 1:�Ѵ�
		char reserve_ch;
		short reserve_sh;
		BitMap<RA_QI_XI_TE_HUI_MAX_PHASE_NUM> phase_reward_flag;			//�׶ν�����ȡ��ʶ
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_QI_XI_TE_HUI_HPP__
