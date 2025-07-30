#ifndef __FUYU_ITEM_H__
#define __FUYU_ITEM_H__

#include "item/itembase.h"

class FuYuItem : public ItemBase
{
public:
	FuYuItem();
	virtual ~FuYuItem() {}

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	virtual void RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const;
	void RandomNetValueSpecial(Role * role, NetValueItemParam * param_data, int put_reason, int sp_group) const;	// 特殊随机属性逻辑,指定特效

	inline int GetFuYuLevel() const { return m_fuyu_level; }

protected:

	int m_fuyu_level;
};

#endif
