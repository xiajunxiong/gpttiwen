#include "specialpet.hpp"
#include "obj/character/role.h"
#include "servercommon/errornum.h"

SpecialPet::SpecialPet() : ItemBase(I_TYPE_SPECIAL_PET), m_pet_id(0)
{
	memset(m_max_bp, 0, sizeof(m_max_bp));
	memset(m_min_bp, 0, sizeof(m_min_bp));
}

int SpecialPet::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);

	return -1;
}

int SpecialPet::GetRandBp(int point_type) const
{
	if (point_type < ADD_POINT_TYPE_MIN || point_type >= ADD_POINT_TYPE_MAX)
	{
		return 0;
	}

	return RandomNum(m_min_bp[point_type], m_max_bp[point_type] + 1);
}

