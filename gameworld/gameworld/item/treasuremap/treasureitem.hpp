#ifndef __TREASURE_MAP_ITEM_HPP__
#define __TREASURE_MAP_ITEM_HPP__

#include "item/itembase.h"

class TreasureMapItem : public ItemBase
{
public:
	TreasureMapItem();
	virtual ~TreasureMapItem() {}

	virtual void RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const;

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;
	int GetMapType() const { return m_map_type; }
	int GetPutReasonType()const;

protected:
	bool RandReward(Role * role, const ItemGridData & item_data,bool ARG_OUT & out_need_set_dirty) const;
	//void SendNotice(Role * role, int param, int is_notice) const;

	int m_param1;
	int m_param2;
	int m_map_type;
};

#endif // __OTHER_ITEM_HPP__

