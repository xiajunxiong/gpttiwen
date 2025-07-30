#include "spiritpactitem.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/partner/partner.hpp"
#include "item/knapsack.h"


SpiritPactItem::SpiritPactItem()
	: ItemBase(I_TYPE_PARTNER_PACT)
{
	m_partner_id = 0;
	m_star = 0;

	m_up_star_cost_id = 0;
	m_up_star_cost_num = 0;

	m_up_star_out_id = 0;

	m_attr_up_per_mil = 0;
}

int SpiritPactItem::Use(Role * role, const ItemGridData & item_data, short & num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type, void * param) const
{
	bool ret = role->GetRoleModuleManager()->GetPartner()
		->PactGridPutIn(item_data.item_id, num, PUT_REASON_PARTNER_PACT_GET);

	return ret ? 0 : -1;
}

