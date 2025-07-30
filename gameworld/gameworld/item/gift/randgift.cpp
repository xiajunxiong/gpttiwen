#include "randgift.hpp"
#include "servercommon/errornum.h"
#include "item/itempool.h"
#include "obj/character/role.h"
#include "world.h"
#include "servercommon/string/gameworldstr.h"
#include "item/knapsack.h"
#include "equipment/equipment.h"
#include "other/rolemodulemanager.hpp"
#include "item/itemextern.hpp"

RandGift::RandGift() : GiftItem(GIFT_TYPE_RAND), m_rand_num(0), m_sum_rate(0), m_boardcast_string_id(0), m_need_gold(0), m_need_bind_gold(0), m_is_pet_gift(false)
{
	memset(m_item_rate_list, 0, sizeof(m_item_rate_list));
	memset(m_boardcast_item_list, 0, sizeof(m_boardcast_item_list));
	m_reward_column_type = -1;
}

RandGift::~RandGift()
{

}

// 注意：如果礼包得到一些不需要放入背包的物品，必须先判断好所有物品都能成功放入各自系统的背包，才能开始放入，一旦开始放入，后面不得提早return，否则可能出现无限刷礼包的情况
int RandGift::Use(Role *role, const ItemGridData &item_data, short & num_arg, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	int use_num = 0;
	for (int i = 0; i < num_arg && use_num < num_arg; ++i)
	{
		if(1 == this->GetPileLimit() || this->IsNetValueItem())
		{
			int ret = this->UseHelper(role, item_data, 1);
			if (ret != 0)
			{
				if (i == 0)		//一个都没使用，就返回失败
				{
					return ret;
				}

				break;
			}
			use_num += 1;
		}
		else
		{
			int ret = this->UseHelper(role, item_data, num_arg);
			if (ret != 0)
			{
				if (i == 0)		//一个都没使用，就返回失败
				{
					return ret;
				}

				break;
			}
			use_num += num_arg;
		}
	}

	num_arg = use_num;

	return 0;
}

int RandGift::UseHelper(Role * role, const ItemGridData & item_data, short num) const
{
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

	if (m_reward_column_type != -1)
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->CheckEmptyGridNoLessThan(m_reward_column_type, num))
		{
			role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return -102;
		}		
	}

	//2. 随机获取的物品
	int index_count = 0;
	static ItemConfigData item_cfg_list[ItemNamespace::KNAPSACK_MAX_GRID_NUM];
	for (int i = 0; i < num; ++i)
	{
		if (!this->GetRandGiftItemList(role, item_data.is_bind, ItemNamespace::KNAPSACK_MAX_GRID_NUM, item_cfg_list, &index_count))
		{
			role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return -1;
		}
	}

	//3. 判断至少有足够临时背包空间，可保证物品一定能放进去
	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList(index_count, item_cfg_list))
	{
		role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
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

	//5. 发物品
	int put_reson = m_is_pet_gift ? PUT_REASON_RAND_GIFT_PET_EGG : PUT_REASON_RAND_GIFT;
	if (index_count > 0)
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->PutList(index_count, item_cfg_list, put_reson))
		{
			return -3;
		}
		else
		{
			if (!m_is_pet_gift)
			{
				role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList(index_count, item_cfg_list, GET_REASON_RAND_GIFT, m_item_id, num);
			}
		}
	}  

	//6. 判断是否要发广播（抽到好东西）
	if (m_boardcast_string_id > 0 && m_boardcast_string_id <= OPEN_RANDGIFT_BROADCAST_ID_MAX_NUM)
	{
		int list_str_len = 0; static char list_str[20480] = { 0 };

		for (int k = 0; k < RAND_GIFT_BROADCAST_ITEM_NUM; ++k)
		{
			for (int i = 0; i < index_count; ++i)
			{
				if (item_cfg_list[i].item_id == m_boardcast_item_list[k])
				{
					char item_str[64] = { 0 }; int itemlen = 0;

					if (list_str_len > 0)
					{
						itemlen = SNPRINTF(item_str, sizeof(item_str), "、{i;%d}x%d",
							(int)item_cfg_list[i].item_id, (int)item_cfg_list[i].num);
					}
					else
					{
						itemlen = SNPRINTF(item_str, sizeof(item_str), "{i;%d}x%d",
							(int)item_cfg_list[i].item_id, (int)item_cfg_list[i].num);
					}
					
					if (itemlen > 0)
					{
						STRNCPY(list_str + list_str_len, item_str, sizeof(list_str) - list_str_len);
						list_str_len += itemlen;
					}

					break;
				}
			}
		}

		if (list_str_len > 0)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_open_randgift[m_boardcast_string_id - 1],
				UidToInt(role->GetUserId()), role->GetName(), (int)m_item_id, list_str);

			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
			}
		}		
	}


	//7. 加钱（如果礼包有加钱的话）
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

bool RandGift::GetRandGiftItemList(Role *role, bool is_bind, int list_num, ItemConfigData *config_list, int *index_count) const
{
	if (NULL == config_list || m_item_num > GIFT_ITEM_MAX_NUM || m_rand_num > GIFT_ITEM_MAX_NUM ) return false;

	static short temp_rate_list[GIFT_ITEM_MAX_NUM] = {0};
	memcpy(temp_rate_list, m_item_rate_list, sizeof(temp_rate_list));

	int sum_rate = m_sum_rate;

	if (m_is_check_prof)
	{
		for (int i = 0; i < m_item_num; ++i)
		{
			const ItemBase *item = ITEMPOOL->GetItem(m_gift_item_list[i].item_id);
			if (NULL == item) return false;

			if (!role->IsProfSuitable(item->GetLimitProf()))
			{
				sum_rate -= temp_rate_list[i];
				temp_rate_list[i] = 0;
			}
		}
	}

	for (int i = 0; i < m_rand_num; ++i)
	{
		if (sum_rate <= 0) return false;
		int rand_int = rand() % sum_rate;

		for (int k = 0; k < m_item_num; ++k)
		{
			if (rand_int < temp_rate_list[k])
			{	
				if (!ItemExtern::PutInItemConfigList(list_num, config_list, index_count,
					m_gift_item_list[k].item_id, m_gift_item_list[k].num, 
					is_bind || m_gift_item_list[k].is_bind))
				{
					return false;
				}
				
				sum_rate -= temp_rate_list[k];
				temp_rate_list[k] = 0;
				break;
			}

			rand_int -= temp_rate_list[k];
		}
	}

	return true;
}


