#ifndef __ZHU_LING_ITEM_HPP__
#define __ZHU_LING_ITEM_HPP__

#include "item/itembase.h"

class ZhuLingItem : public ItemBase
{
public:
	ZhuLingItem();

	virtual ~ZhuLingItem() {}

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;
private:

};

#endif 

