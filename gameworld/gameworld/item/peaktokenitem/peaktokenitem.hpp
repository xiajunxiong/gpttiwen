#ifndef __PEAK_TOKEN_ITEM_HPP__
#define __PEAK_TOKEN_ITEM_HPP__

//ÌìÏÂÖÁ×ðÁî

#include "item/itembase.h"

class PeakTokenItem : public ItemBase
{
public:
	PeakTokenItem();
	virtual ~PeakTokenItem() {}

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	virtual void RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const;


	int CreatAct(Role *role) const;

private:
	int m_reward_group_id;
};

#endif
