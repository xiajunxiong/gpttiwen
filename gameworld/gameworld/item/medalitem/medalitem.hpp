#ifndef __MEDAL_ITEM_HPP__
#define __MEDAL_ITEM_HPP__

#include "item/itembase.h"

class MedalItem : public ItemBase
{
public:
	MedalItem();

	virtual ~MedalItem() {}

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	virtual int GetSellPrice(NetValueItemParam* param) const;
	virtual void RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const;

	inline int GetAttrGroup() const { return m_attr_group; }
	inline int GetSpGroup() const { return m_sp_group; }
	inline int GetInitialRank() const { return m_initial_rank; }

public:
	int m_attr_group;
	int m_sp_group;
	int m_initial_rank;
};

#endif // __OTHER_ITEM_HPP__

