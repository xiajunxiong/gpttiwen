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

// ע�⣺�������õ�һЩ����Ҫ���뱳������Ʒ���������жϺ�������Ʒ���ܳɹ��������ϵͳ�ı��������ܿ�ʼ���룬һ����ʼ���룬���治������return��������ܳ�������ˢ��������
int DefGift::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	if (m_item_num > GIFT_ITEM_MAX_NUM) return -1;

	//1. �жϿ��������Ҫ��Ǯ������
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
		// ������Ϸû�а�Ԫ��һ�������ô���
#ifdef _DEBUG
		assert(0);
#endif
		return -101;
	}

	//2. ����Ʒƽ̯�ɲ������ѵ����ֵ��״̬����������һ���ж��ټ���Ʒ
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

	//3. �ж��������㹻��ʱ�����ռ䣬�ɱ�֤��Ʒһ���ܷŽ�ȥ
	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckEmptyGridNoLessThanInTemporary(index_count))
	{
		role->NoticeNum(errornum::EN_TEMPORARY_KNAPSACK_NO_SPACE);
		return -2;
	}

	//4. ��Ǯ
	if (m_need_gold > 0)
	{
		role->GetRoleModuleManager()->GetMoney()->UseGold(m_need_gold * num, "OpenGift");
	}
	//if (m_need_bind_gold > 0)
	//{
	//	role->GetKnapsack()->GetMoney()->UseAllGold(m_need_bind_gold * num, "OpenGift");
	//}

	std::vector<ItemConfigData> notice_list;
	//5. ������Ʒ
	for (int i = 0; i < m_item_num; ++i)
	{
		if (m_is_check_prof)
		{
			const ItemBase *item = ITEMPOOL->GetItem(m_gift_item_list[i].item_id);
			if (NULL == item || !role->IsProfSuitable(item->GetLimitProf())) continue;
		}

		int put_reson = m_is_pet_gift ? PUT_REASON_DEF_GIFT_PET_EGG : PUT_REASON_DEF_GIFT;
		int put_succ_count = 0;
		for (int k = 0; k < num; ++k)	// ��η���
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
	//֪ͨ
	if (!notice_list.empty())
	{
		role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)notice_list.size(), &notice_list[0], GET_REASON_RAND_GIFT, m_item_id, num);
	}

	//6. ��Ǯ���������м�Ǯ�Ļ���
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

