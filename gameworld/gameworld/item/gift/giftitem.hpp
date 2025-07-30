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

		GIFT_TYPE_DEF,							// �̶����
		GIFT_TYPE_RAND,							// ������
		GIFT_TYPE_OPTIONAL,						// ��ѡ���

		GIFT_TYPE_MAX,
	};

	GiftItem(short sub_type) : ItemBase(I_TYPE_GIFT), m_sub_type(sub_type), m_coin_bind(0), m_gold_bind(0), m_gold(0), m_is_check_prof(false), m_item_num(0) {}
	virtual ~GiftItem() {}

protected:
	short m_sub_type;
	int m_coin_bind;							// ��Ǯ��
	int m_gold_bind;							// ��Ԫ��
	int m_gold;									// Ԫ��
	bool m_is_check_prof;						// �Ƿ�����ְҵ
	short m_item_num;							// ��������
	ItemConfigData m_gift_item_list[GIFT_ITEM_MAX_NUM];
};

#endif //__GIFT_ITEM_H__
