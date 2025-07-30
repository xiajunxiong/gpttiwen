#include "partnernatalcard.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/partner/partnerequipmentconfig.hpp"

PartnerNatalCard::PartnerNatalCard() : ItemBase(I_TYPE_PARTNER_NATAL_CARD)
{
}

PartnerNatalCard::~PartnerNatalCard()
{
}

int PartnerNatalCard::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	return 0;
}

void PartnerNatalCard::RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const
{
	NewPartnerNatalCardParam & card_param = param_data->partner_natal_card_param;
	BattleAttrPair attr_pair[MAX_PARTNER_NATAL_CARD_VOC_NUM];
	LOGIC_CONFIG->GetPartnerEquipmentConfig()->RandAttrTypeAndValue(m_item_id, &attr_pair[0]);
	for (int i = 0; i < MAX_PARTNER_NATAL_CARD_VOC_NUM; i++)
	{
		if (attr_pair[i].attr_type < BATTLE_ATTR_MAX && attr_pair[i].attr_type >= 0 && attr_pair[i].attr_value > 0)
		{
			card_param.m_voc_list[i].attr_type = attr_pair[i].attr_type;
			card_param.m_voc_list[i].attr_value = attr_pair[i].attr_value;
		}
	}
}