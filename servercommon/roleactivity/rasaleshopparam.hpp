#ifndef __RA_SALE_SHOP_PARAM_HPP__
#define __RA_SALE_SHOP_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2059 ���� - �����̵�  //////////////////////////////////////////
static const int MAX_SALE_SHOP_TYPE_SEQ_NUM = 24;

enum SALE_SHOP_ITEM_TYPE					//�̵��������
{
	SALE_SHOP_ITEM_TYPE_RECOMMEND = 0,		//�Ƽ�
	SALE_SHOP_ITEM_TYPE_MOUNT,				//����
	SALE_SHOP_ITEM_TYPE_SURFACE,			//ʱװ
	SALE_SHOP_ITEM_TYPE_ARMS,				//����
	SALE_SHOP_ITEM_TYPE_PET_GIFT,			//����
	SALE_SHOP_ITEM_TYPE_EQUIPMENT_GIFT,		//װ�����

	SALE_SHOP_ITEM_TYPE_MAX
};

enum SALE_SHOP_LIMIT_TYPE
{
	SALE_SHOP_LIMIT_TYPE_BEGIN = 0,			//��Ч
	SALE_SHOP_LIMIT_TYPE_BUY_COUNT = 1,		//���޶�
	SALE_SHOP_LIMIT_TYPE_TODAY = 2,			//ÿ���޶�

	SALE_SHOP_LIMIT_TYPE_MAX,
};

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RASaleShopParam
{
	RASaleShopParam() { this->Reset(); }
	void Reset()
	{
		ra_sale_shop_begin_timestamp = 0;
		memset(ra_sale_shop_buy_count, 0, sizeof(ra_sale_shop_buy_count));
	}

	unsigned int ra_sale_shop_begin_timestamp;
	short ra_sale_shop_buy_count[SALE_SHOP_ITEM_TYPE_MAX][MAX_SALE_SHOP_TYPE_SEQ_NUM];      //��Ҹ��������еĸ������߹�������

};

UNSTD_STATIC_CHECK(sizeof(RASaleShopParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_SALE_SHOP_PARAM_HPP__
