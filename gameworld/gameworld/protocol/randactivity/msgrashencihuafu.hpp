#ifndef __MSG_RA_SHEN_CI_HUA_FU_HPP__
#define __MSG_RA_SHEN_CI_HUA_FU_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityshencihuafuparam.hpp"

//////////////////////////////////////////// 2208  ����  �����۱�  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	enum SHEN_CI_HUA_FU_INFO_RESON
	{
		SHEN_CI_HUA_FU_INFO_RESON_DEFAULT,
		SHEN_CI_HUA_FU_INFO_RESON_SET,		//����
		SHEN_CI_HUA_FU_INFO_RESON_EXCHANGE,		//�һ���Ʒ
		SHEN_CI_HUA_FU_INFO_RESON_BUY,		//����
		SHEN_CI_HUA_FU_INFO_RESON_RESET,		//����
	};

	class SCRAShenCiHuaFuInfo						//<! 5225 ��Ϣ
	{
	public:
		SCRAShenCiHuaFuInfo();
		MessageHeader header;
	
		unsigned short ra_day_buy_times[SHEN_CI_HUA_FU_MAX_BUY_ITEM];		//20 ÿ����Ʒÿ���޹�����
		unsigned short ra_buy_times[SHEN_CI_HUA_FU_MAX_BUY_ITEM];			//20 ÿ����Ʒ�޹�����
		unsigned short ra_day_exchange_times[SHEN_CI_HUA_FU_MAX_EXCHANGE_SHOP_ITEM];	//26 ÿ����Ʒÿ�� �һ�����
		unsigned short ra_exchange_times[SHEN_CI_HUA_FU_MAX_EXCHANGE_SHOP_ITEM];	//26 ÿ����Ʒ    �һ�����
		RAShenCiHuaFuItem item_list_select[SHEN_CI_HUA_FU_SELECT_NUM_SELECT];		//3 ��ѡ�����б�
		RAShenCiHuaFuItem item_list_rand[SHEN_CI_HUA_FU_SELECT_NUM_RAND];		//6 ����ѡ�����б�
		int reason;
	};

	struct SCRAShenCiHuaFuRetItem
	{
		int type;
		int index;
	};

	class SCRAShenCiHuaFuDrawRet						//<! 5226 �齱����
	{
	public:
		SCRAShenCiHuaFuDrawRet();
		MessageHeader header;

		int count;
		SCRAShenCiHuaFuRetItem item_list[10];
	};
}

#pragma pack(pop)

#endif 
