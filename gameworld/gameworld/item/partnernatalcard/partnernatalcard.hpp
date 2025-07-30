#ifndef __PARTNER_NATAL_CARD_HPP__
#define __PARTNER_NATAL_CARD_HPP__

#include "item/itembase.h"

class PartnerNatalCard : public ItemBase
{
public:
	PartnerNatalCard();
	virtual ~PartnerNatalCard();

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	virtual void RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const;

	int GetEquipType() { return m_equip_type; }

private:
	int m_equip_type;
};

#endif