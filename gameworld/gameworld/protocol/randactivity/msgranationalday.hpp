#ifndef __MSG_RA_NATIONAL_DAY_HPP__
#define __MSG_RA_NATIONAL_DAY_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ranationaldayparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{

	//�ٱ����� ��3000ת��
	enum RA_MI_BAO_FANG_DIAN_REQ_TYPE
	{
		RA_MI_BAO_FANG_DIAN_REQ_TYPE_INFO = 0,									//������Ϣ�·�
		RA_MI_BAO_FANG_DIAN_REQ_TYPE_BUY_ITEM = 1,								//��������� param1:seq
		RA_MI_BAO_FANG_DIAN_REQ_TYPE_OPEN_ACTIVITY = 2,							//�򿪻����
	};

	class SCRAMiBaoFangDianInfo		//!< 3088 ����-�ٱ�������Ϣ�·�
	{
	public:
		SCRAMiBaoFangDianInfo();
		MessageHeader header;

		short is_need_recal_phase_flag;											//0:������ 1:����׶�
		short is_open_today_flag;												//�����Ƿ�򿪹����� 0:û�� 1���򿪹�
		int cur_use_open_day;													//ʹ�ø�������ȷ����������
		short today_buy_times[MAX_RA_MI_BAO_FANG_DIAN_PHASE_SHOP_ITEM_NUM];		//ÿ���޹�
		short activity_buy_times[MAX_RA_MI_BAO_FANG_DIAN_PHASE_SHOP_ITEM_NUM];	//��޹�
	};

	class SCRAMiBaoFangDianSingleInfo		//!< 3089 ����-�ٱ����굥����Ϣ�·�
	{
	public:
		SCRAMiBaoFangDianSingleInfo();
		MessageHeader header;

		short seq;
		short index;
		short today_buy_times;
		short activity_buy_times;
	};





}

#pragma pack(pop)


#endif
