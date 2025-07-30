#include "optionalgift.hpp"
#include "world.h"

#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "item/itempool.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "item/itemextern.hpp"
#include "protocol/msgitem.h"

#include <vector>
#include <map>

OptionalGift::OptionalGift() : GiftItem(GIFT_TYPE_OPTIONAL), m_need_gold(0), m_optional_num(0), m_is_pet_gift(false)
{

}

OptionalGift::~OptionalGift()
{

}

int OptionalGift::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	if (param == NULL)
	{
		return -90;
	}

	if (ITEM_PARAM_TYPE_OPTIOAN_GIFT != param_type)
	{
		return -999;
	}

	if (m_need_gold > 0)
	{
		if (!role->GetRoleModuleManager()->GetMoney()->GoldMoreThan(m_need_gold * num))
		{
			role->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return -91;
		}
	}

	int select_item_count = param1;
	if (select_item_count != m_optional_num)
	{
		return -92;
	}
	Protocol::CSUseOptionalGift::SelectInfo * select_info_list = (Protocol::CSUseOptionalGift::SelectInfo *) param;
	
	std::set<int> index_list;
	std::vector<ItemConfigData> optional_item_list;
	for (int i = 0; i < select_item_count; ++i)
	{
		Protocol::CSUseOptionalGift::SelectInfo & select_info = select_info_list[i];
		if (select_info.index < 0 || select_info.index >= m_item_num || select_info.num < 0)
		{
			return -93;
		}
		if (index_list.end() != index_list.find(select_info.index))
		{
			return -94;
		}
		index_list.insert(select_info.index);
		optional_item_list.push_back(ItemConfigData(m_gift_item_list[select_info.index].item_id, m_gift_item_list[select_info.index].is_bind, m_gift_item_list[select_info.index].num * num));
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckEmptyGridNoLessThanInTemporary(optional_item_list.size()))
	{
		role->NoticeNum(errornum::EN_TEMPORARY_KNAPSACK_NO_SPACE);
		return -101;
	}

	if (!optional_item_list.empty())
	{
		int put_reson = m_is_pet_gift ? PUT_REASON_OPTIONAL_GIFT_PET_EGG : PUT_REASON_OPTIONAL_GIFT;
		if (!role->GetRoleModuleManager()->GetKnapsack()->PutList((short)optional_item_list.size(), &optional_item_list[0], put_reson))
		{
			return -103;
		}
		else
		{
			if (!m_is_pet_gift)
			{
				role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)optional_item_list.size(), &optional_item_list[0], GET_REASON_RAND_GIFT, m_item_id, num);
			}
		}
	}

	if (m_need_gold > 0)
	{
		role->GetRoleModuleManager()->GetMoney()->UseGold(m_need_gold * num, "OptionalGift");
	}

	return 0;
}
