#ifndef __PET_BADGE_ITEM_HPP__
#define __PET_BADGE_ITEM_HPP__

#include "item/itembase.h"

class PetBadgeItem : public ItemBase
{
public:
	PetBadgeItem();
	virtual ~PetBadgeItem();

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	virtual void RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const;
public:

};

#endif // __OTHER_ITEM_HPP__

