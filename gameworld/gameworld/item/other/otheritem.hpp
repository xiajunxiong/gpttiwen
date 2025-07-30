#ifndef __OTHER_ITEM_HPP__
#define __OTHER_ITEM_HPP__

#include "item/itembase.h"

class OtherItem : public ItemBase
{
public:
	enum OTHER_ITEM_SUB_TYPE
	{
		I_SUB_TYPE_INVALID = 0,
		I_SUB_TYPE_FLOWER,								// 1 �ʻ�
		I_SUB_TYPE_RESERVED,							// 2 ��������

		OTHER_ITEM_SUB_TYPE_NUM,
	};

	OtherItem() : ItemBase(I_TYPE_OTHER), m_sub_type(I_SUB_TYPE_INVALID), m_value(0) {}
	virtual ~OtherItem() {}

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;
	virtual UInt32 CalInvalidTime() const;

	short GetSubType() const { return m_sub_type; }
	int GetValue() const { return m_value; }

	virtual bool HasInvalidTime() const;
protected:
	int m_day_invalid_time;							// ʧЧʱ�䣨һ�����ض�ʱ��� -1 ����û�У�2010�����ǽ���20��10��ʧЧ��
	short m_sub_type;
	int m_value;
};

#endif // __OTHER_ITEM_HPP__

