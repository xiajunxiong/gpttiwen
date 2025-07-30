#ifndef __MSG_SHOP_H__
#define __MSG_SHOP_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/shopdef.h"
#include "servercommon/exchangedef.h"
#include "gamedef.h"

#pragma pack(push, 4)

namespace Protocol
{
	class SCShopAllInfo					//!< 2490 - ������Ϣ
	{
	public:
		SCShopAllInfo();
		MessageHeader		header;

		unsigned char shop_buy_count[SHOP_ITEM_MAX_COUNT];		//!< SHOP_ITEM_MAX_COUNT = 256
	};

	class SCShopSingleInfo				//!< 2491 - ������Ϣ
	{
	public:
		SCShopSingleInfo();
		MessageHeader		header;

		int index;
		int count;
	};

	class CSShopBuyReq					//!< 2492 - ����
	{
	public:
		CSShopBuyReq();
		MessageHeader		header;

		int index;
		int num;
	};

	class CSFastPurchase				//!< 2493 - ���ٹ���
	{
	public:
		CSFastPurchase();
		MessageHeader		header;

		ItemID item_id;		//!< ���ٹ���ĵ���ID
		UInt16 buy_num;		//!< ��������
	};

	class CSGoldShopBuyReq					//!< 2494 - Ԫ���̳�
	{
	public:
		CSGoldShopBuyReq();
		MessageHeader		header;

		int index;
		short num;
		short is_huashen_buy;		//0:���ǻ����  !0:�����
	};

	class SCGoldShopAllInfo					//!< 2495 - Ԫ���̳� ������Ϣ
	{
	public:
		SCGoldShopAllInfo();
		MessageHeader		header;

		unsigned char shop_buy_count[GOLD_SHOP_ITEM_MAX_COUNT];		//!< SHOP_ITEM_MAX_COUNT = 128
	};

	class SCGoldShopSingleInfo				//!< 2496 - Ԫ���̳� ������Ϣ
	{
	public:
		SCGoldShopSingleInfo();
		MessageHeader		header;

		int index;
		int count;
	};

	class CSExchangeReq				//!< 2497 - �һ�����
	{
	public:
		CSExchangeReq();
		MessageHeader		header;

		int seq;
		int num;
	};

	class SCExchangeAllInfo				//!< 2498 - �һ�������Ϣ
	{
	public:
		SCExchangeAllInfo();
		MessageHeader		header;
		struct LimitData
		{
			unsigned short seq;
			unsigned short buy_count;
		};

		int count;
		LimitData limit_list[LIMIT_DATA_NUM];
	}; 

	class SCExchangeSingleInfo				//!< 2499 - �һ�������Ϣ
	{
	public:
		SCExchangeSingleInfo();
		MessageHeader		header;

		unsigned short seq;
		unsigned short buy_count;
	};

	class CSGoldShopNotifyCancelReq
	{
	public:
		CSGoldShopNotifyCancelReq();		//!< 2540
		MessageHeader header;

		int cancel_index;
	};

	class SCGoldShopPurchaseNotifys
	{
	public:
		SCGoldShopPurchaseNotifys();		//!< 2541
		MessageHeader header;

		BitMap<GOLD_SHOP_ITEM_MAX_COUNT> notify_flag;
	};
}

#pragma pack(pop)
#endif