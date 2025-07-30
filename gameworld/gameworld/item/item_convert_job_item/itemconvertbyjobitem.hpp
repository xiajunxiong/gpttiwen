#ifndef __ITEM_CONVERT_BY_JOB_ITEM_HPP__
#define __ITEM_CONVERT_BY_JOB_ITEM_HPP__

#include "item/itembase.h"
#include "servercommon/attributedef.hpp"

class ItemConvertByJobItem : public ItemBase
{
public:
	ItemConvertByJobItem();
	virtual ~ItemConvertByJobItem() {}

	virtual bool Init(const char* path, std::string* err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;
protected:

};

#endif