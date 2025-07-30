#ifndef __MSG_RA_SECRET_TREASURE_HPP__
#define __MSG_RA_SECRET_TREASURE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rasecrettreasureparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	//��Э�� 3000
	enum RA_SECRET_TREASURE_REQ_TYPE
	{
		RA_SECRET_TREASURE_REQ_TYPE_INFO = 0,				// ������Ϣ�·�
		RA_SECRET_TREASURE_REQ_TYPE_BUY_ITEM = 1,			// ������� param1:type param2:seq
		RA_SECRET_TREASURE_REQ_TYPE_OPEN_ACTIVITY = 2,		// �򿪻����
	};

	class SCRASecretTreasureInfo			//!< 3073  ��Ӫ�-�ػ��ر���Ϣ�·�
	{
	public:
		SCRASecretTreasureInfo();
		MessageHeader header;

		short phase;
		short is_open_today_flag;												//�����Ƿ�򿪹����� 0:û�� 1���򿪹�
		short today_buy_times[MAX_SECRET_TREASURE_PHASE_SHOP_ITEM_NUM];			//ÿ���޹�
		short activity_buy_times[MAX_SECRET_TREASURE_SHOP_ITEM_NUM];			//��޹�
	};

}

#pragma pack(pop)


#endif	//__MSG_RA_SECRET_TREASURE_HPP__