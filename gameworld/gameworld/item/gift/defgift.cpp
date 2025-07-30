#include "defgift.hpp"
#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "globalconfig/globalconfig.h"
#include "item/itempool.h"
#include "obj/character/role.h"
#include "world.h"
#include "item/itemgriddata.h"
#include "config/logicconfigmanager.hpp"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "item/money.h"
#include "item/itemextern.hpp"

DefGift::DefGift() : GiftItem(GIFT_TYPE_DEF), m_need_gold(0), m_need_bind_gold(0), m_is_pet_gift(false)
{

}

DefGift::~DefGift()
{

}

// 注意：如果礼包得到一些不需要放入背包的物品，必须先判断好所有物品都能成功放入各自系统的背包，才能开始放入，一旦开始放入，后面不得提早return，否则可能出现无限刷礼包的情况
int DefGift::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	if (m_item_num > GIFT_ITEM_MAX_NUM) return -1;

	//1. 判断开启礼包需要的钱够不够
	if (m_need_gold > 0)
	{
		if (!role->GetRoleModuleManager()->GetMoney()->GoldMoreThan(m_need_gold * num))
		{
			role->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return -100;
		}
	}

	if (m_need_bind_gold > 0)
	{
		// 现在游戏没有绑元，一定是配置错了
#ifdef _DEBUG
		assert(0);
#endif
		return -101;
	}

	//2. 把物品平摊成不超过堆叠最大值的状态，计算这里一共有多少件物品
	int index_count = 0;
	static ItemConfigData item_cfg_list[ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX];
	for (int i = 0; i < m_item_num; ++i)
	{
		if (m_is_check_prof)
		{
			const ItemBase *item = ITEMPOOL->GetItem(m_gift_item_list[i].item_id);
			if (NULL == item || !role->IsProfSuitable(item->GetLimitProf())) continue;
		}

		for (int k = 0; k < num; ++k)
		{
			if (!ItemExtern::PutInItemConfigList(ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX, item_cfg_list, &index_count,
				m_gift_item_list[i].item_id, m_gift_item_list[i].num, item_data.is_bind || m_gift_item_list[i].is_bind))
			{
				role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return -1;
			}
		}
	}

	//3. 判断至少有足够临时背包空间，可保证物品一定能放进去
	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckEmptyGridNoLessThanInTemporary(index_count))
	{
		role->NoticeNum(errornum::EN_TEMPORARY_KNAPSACK_NO_SPACE);
		return -2;
	}

	//4. 扣钱
	if (m_need_gold > 0)
	{
		role->GetRoleModuleManager()->GetMoney()->UseGold(m_need_gold * num, "OpenGift");
	}
	//if (m_need_bind_gold > 0)
	//{
	//	role->GetKnapsack()->GetMoney()->UseAllGold(m_need_bind_gold * num, "OpenGift");
	//}

	std::vector<ItemConfigData> notice_list;
	//5. 放入物品
	for (int i = 0; i < m_item_num; ++i)
	{
		if (m_is_check_prof)
		{
			const ItemBase *item = ITEMPOOL->GetItem(m_gift_item_list[i].item_id);
			if (NULL == item || !role->IsProfSuitable(item->GetLimitProf())) continue;
		}

		int put_reson = m_is_pet_gift ? PUT_REASON_DEF_GIFT_PET_EGG : PUT_REASON_DEF_GIFT;
		int put_succ_count = 0;
		for (int k = 0; k < num; ++k)	// 多次放入
		{
			if (role->GetRoleModuleManager()->GetKnapsack()->Put(m_gift_item_list[i], put_reson))
			{
				put_succ_count++;
			}
		}
		if (!m_is_pet_gift && put_succ_count > 0)
		{
			bool is_find = false;
			ItemConfigData put_reward(m_gift_item_list[i].item_id, m_gift_item_list[i].is_bind, m_gift_item_list[i].num * put_succ_count);
			for (int z = 0; z < (int)notice_list.size(); z++)
			{
				if (notice_list[z].item_id == put_reward.item_id && notice_list[z].is_bind == put_reward.is_bind)
				{
					is_find = true;
					notice_list[z].num += put_reward.num;
				}
			}
			if (!is_find)
			{
				notice_list.push_back(put_reward);
			}
		}
	}
	//通知
	if (!notice_list.empty())
	{
		role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)notice_list.size(), &notice_list[0], GET_REASON_RAND_GIFT, m_item_id, num);
	}

	//6. 加钱（如果礼包有加钱的话）
	if (m_gold > 0)
	{
		role->GetRoleModuleManager()->GetMoney()->AddGold(1LL * m_gold * num, __FUNCTION__);
	}

	if (m_coin_bind > 0)
	{
		role->GetRoleModuleManager()->GetMoney()->AddCoinBind(1LL * m_coin_bind * num, __FUNCTION__);
	}

	return 0;
}

