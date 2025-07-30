#ifndef __DIAN_HUA_SHI_ITEM_HPP__
#define __DIAN_HUA_SHI_ITEM_HPP__

#include "item/itembase.h"

class DianHuaShiItem : public ItemBase
{
public:
	DianHuaShiItem(short equip_type);

	virtual ~DianHuaShiItem() {}

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;
	virtual void RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const;
private:
	
	int effect_type;
	int effect_level;
};

#endif // __OTHER_ITEM_HPP__

