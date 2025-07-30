#ifndef __GIFT_ITEM_H__
#define __GIFT_ITEM_H__

#include "item/itembase.h"
#include "servercommon/struct/itemlistparam.h"

class GiftItem : public ItemBase
{
public:
	static const int GIFT_ITEM_MAX_NUM = 18;

	enum GIFT_TYPE
	{
		GIFT_TYPE_INVALID,

		GIFT_TYPE_DEF,							// 固定礼包
		GIFT_TYPE_RAND,							// 随机礼包
		GIFT_TYPE_OPTIONAL,						// 自选礼包

		GIFT_TYPE_MAX,
	};

	GiftItem(short sub_type) : ItemBase(I_TYPE_GIFT), m_sub_type(sub_type), m_coin_bind(0), m_gold_bind(0), m_gold(0), m_is_check_prof(false), m_item_num(0) {}
	virtual ~GiftItem() {}

protected:
	short m_sub_type;
	int m_coin_bind;							// 绑定钱币
	int m_gold_bind;							// 绑定元宝
	int m_gold;									// 元宝
	bool m_is_check_prof;						// 是否区分职业
	short m_item_num;							// 开出数量
	ItemConfigData m_gift_item_list[GIFT_ITEM_MAX_NUM];
};

#endif //__GIFT_ITEM_H__
