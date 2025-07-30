#ifndef __DEF_GIFT_H__
#define __DEF_GIFT_H__

#include "giftitem.hpp"

class DefGift : public GiftItem
{
public:
	DefGift();
	~DefGift();

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

private:
	int m_need_gold;
	int m_need_bind_gold;
	bool  m_is_pet_gift;		//�Ƿ��ǳ���������·��õ�����ʱ���ͻ������ֲ��Ų�ͬ����
};

#endif //__DEF_GIFT_H__


