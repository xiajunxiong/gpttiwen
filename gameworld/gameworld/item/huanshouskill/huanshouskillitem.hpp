#ifndef __HUAN_SHOU_SKILL_ITEM_HPP__
#define __HUAN_SHOU_SKILL_ITEM_HPP__

#include "item/itembase.h"

class HuanShouSkillItem : public ItemBase
{
public:

	HuanShouSkillItem() : ItemBase(I_TYPE_HUAN_SHOU_SKILL), m_skill_id(0) {}
	virtual ~HuanShouSkillItem() {}

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	int GetSkillId() const { return m_skill_id; }
protected:
	int m_skill_id;
};

#endif // __HUAN_SHOU_SKILL_ITEM_HPP__

