#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "config/logicconfigmanager.hpp"
#include "other/pet/petbadgeconfig.hpp"

#include "petbadgeitem.hpp"

PetBadgeItem::PetBadgeItem() : ItemBase(I_TYPE_PET_BADGE)
{
}

PetBadgeItem::~PetBadgeItem()
{
}

int PetBadgeItem::Use(Role * role, const ItemGridData & item_data, short & num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type, void * param) const
{
	if (role)
		role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);
	return 0;
}

void PetBadgeItem::RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const
{
	PetBadgeItemParam & data = param_data->pet_badge_item_param;
	data.pet_badgei_item_id = this->GetItemId();
	data.skill_id = LOGIC_CONFIG->GetPetBadgeConfig()->GePetBadgeRandSkill(this->GetItemId());

	std::vector<AttrInfo> attrs = LOGIC_CONFIG->GetPetBadgeConfig()->GePetBadgeRandAttr(this->GetItemId());
	for (int i = 0; i < (int)attrs.size() && i < ARRAY_LENGTH(data.attrs); ++i)
	{
		data.attrs[i] = attrs[i];
	}
}
