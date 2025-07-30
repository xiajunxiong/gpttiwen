#include "superskillbookconvert.hpp"

#include "gameworld/item/itembase.h"
#include "gameworld/item/itempool.h"
#include "servercommon/errornum.h"
#include "servercommon/struct/itemlistparam.h"
#include "other/rolemodulemanager.hpp"
#include "gameworld/item/knapsack.h"
#include "gamelog.h"


SkillBookConvert::SkillBookConvert()
{
}

SkillBookConvert::~SkillBookConvert()
{
}


bool SkillBookConvert::ConvertSkillBook(Role * role, ItemConfigData book_item_1, ItemConfigData book_item_2)
{
	if (NULL == role)
	{
		return false;
	}

	const ItemBase* item_1 = ITEMPOOL->GetItem(book_item_1.item_id);
	if (NULL == item_1)
	{
		return false;
	}

	const ItemBase* item_2 = ITEMPOOL->GetItem(book_item_2.item_id);
	if (NULL == item_2)
	{
		return false;
	}

	//判断传进来的技能书id
	if (item_1->GetItemSubType() != ItemNamespace::ITEM_SUB_TYPE_PET_SKILL || 
		item_1->GetColor() != ItemBase::I_COLOR_ORANGE)
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return false;
	}

	if (item_2->GetItemSubType() != ItemNamespace::ITEM_SUB_TYPE_PET_SKILL || 
		item_2->GetColor() != ItemBase::I_COLOR_ORANGE)
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return false;
	}

	ItemConfigData item;
	item.item_id = 25566;
	item.is_bind = true;
	item.num = 1;

	//判断背包占位
	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(item.item_id,item.num))
	{
		role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->HasItem(book_item_1.item_id, book_item_1.num) ||
		!role->GetRoleModuleManager()->GetKnapsack()->HasItem(book_item_2.item_id, book_item_2.num))
	{
		role->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return false;
	}

	//扣除道具
	if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(book_item_1.item_id, book_item_1.num, __FUNCTION__))
	{
		gamelog::g_log_knapsack.printf(LL_WARNING, "%s role[%d,%s] Consume_First_Item_Failed!! item_id[%d, %d]", __FUNCTION__,
			role->GetUID(), role->GetName(), book_item_1.item_id, book_item_1.num);
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return false;
	}
	if( !role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(book_item_2.item_id, book_item_2.num, __FUNCTION__))
	{
		gamelog::g_log_knapsack.printf(LL_WARNING, "%s role[%d,%s] Consume_Second_Item_Failed!! item_id[%d, %d]", __FUNCTION__,
			role->GetUID(), role->GetName(), book_item_2.item_id, book_item_2.num);
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	//下发道具
	if (!role->GetRoleModuleManager()->GetKnapsack()->Put(item, PUT_REASON_SKILL_BOOK_CONVERT))
	{
		gamelog::g_log_knapsack.printf(LL_WARNING, "%s role[%d,%s] Put_Failed!!", __FUNCTION__,
			role->GetUID(), role->GetName());
		role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}

	gamelog::g_log_knapsack.printf(LL_INFO, "%s role[%d,%s] Consume[%d,%d] gain[%d]",
		__FUNCTION__,role->GetUID(),role->GetName(),book_item_1.item_id,book_item_2.item_id,item.item_id);

		return true;
}
