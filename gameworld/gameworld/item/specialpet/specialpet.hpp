#ifndef __SPECIAL_PET_HPP__
#define __SPECIAL_PET_HPP__

#include "item/itembase.h"
#include "servercommon/attributedef.hpp"

class SpecialPet : public ItemBase
{
public:
	SpecialPet();
	virtual ~SpecialPet() {}

	virtual bool Init(const char* path, std::string* err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	int GetPetId() const { return m_pet_id; }
	int GetRandBp(int point_type) const;

protected:
	int m_pet_id;						//!< 对应的宠物ID
	int m_max_bp[ADD_POINT_TYPE_MAX];	//!< 最大BP属性
	int m_min_bp[ADD_POINT_TYPE_MAX];	//!< 最小BP属性
};

#endif