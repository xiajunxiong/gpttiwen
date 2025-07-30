#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "item/itempool.h"
#include "engineadapter.h"
#include "item/knapsack.h"
#include "config/logicconfigmanager.hpp"
#include "servercommon/errornum.h"
#include "item/itemextern.hpp"
#include "gamelog.h"
#include "equipment/equipmentmanager.h"
#include "other/shop/shopconfig.h"

#include "other/rolezhuling/zhulingconfig.hpp"
#include "protocol/msgzhuling.h"
#include "rolezhuling.hpp"
#include "global/marketmanager/marketmanager.hpp"

RoleZhuLing::RoleZhuLing()
{
	m_role_module_mgr = NULL;
}

RoleZhuLing::~RoleZhuLing()
{
}

void RoleZhuLing::Init(RoleModuleManager * mgr)
{
	m_role_module_mgr = mgr;
}

int RoleZhuLing::UpLevelZhuLing(int item_src, int index, int column, int slot_index, int need_buy_base_num, int count, unsigned short * item_list)
{
	if (slot_index < 0 || slot_index >= EQUIP_ZHU_LING_SLOT_NUM || item_list == NULL) return -__LINE__;
	if (need_buy_base_num < 0) return -__LINE__;

	const ItemGridData * grid_data = this->GetItemGridData(item_src, index, column);
	if (grid_data == NULL)
	{
		return -__LINE__;
	}

	ZhuLinParamRet  zhu_ling_info = this->GetZhuLinParam(grid_data, slot_index);
	if (!zhu_ling_info.is_ret_succ)
	{
		return -__LINE__;
	}

	const ZhuLingCfg * cfg = LOGIC_CONFIG->GetZhuLingConfig()->GetZhuLingCfg(zhu_ling_info.item_id);
	if (cfg == NULL)
	{
		return -__LINE__;
	}

	if (cfg->zhu_ling_level + 1 != count)
	{
		return -__LINE__;
	}

	int type = cfg->type;
	int up_base_item_id = LOGIC_CONFIG->GetZhuLingConfig()->GetZhuLingUpLevelBaseItemId(type);

	const ZhuLingCfg * next_cfg = LOGIC_CONFIG->GetZhuLingConfig()->GetZhuLingCfgByTypeAndLevel(type, cfg->zhu_ling_level + 1);
	if (next_cfg == NULL)
	{
		return -__LINE__;
	}

	int need_total_base_num = next_cfg->need_base_item_num;
	if (!zhu_ling_info.is_zhu_ling)
	{
		need_total_base_num -= cfg->need_base_item_num;
	}
	else
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPut(next_cfg->item_id, 1))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
			return -__LINE__;
		}
	}

	int consume_base_num = need_buy_base_num;
	std::map<int, int> consume_map;
	for (int level = 0; level <= cfg->zhu_ling_level; ++level)
	{
		if (item_list[level] == 0)
		{
			continue;
		}

		const ZhuLingCfg * tmp_cfg = LOGIC_CONFIG->GetZhuLingConfig()->GetZhuLingCfgByTypeAndLevel(type, level);
		if (tmp_cfg == NULL)
		{
			return -__LINE__;
		}

		consume_base_num += tmp_cfg->need_base_item_num * item_list[level];
		consume_map[tmp_cfg->item_id] += item_list[level];
	}

	if (need_total_base_num != consume_base_num)
	{
		return -__LINE__;
	}

	int money_type = 0;
	int money_num = 0;
	//检查消耗的物品够不够
	{
		if (need_buy_base_num > 0)
		{
			if (!MarketManager::Instance().GetItemPrice(up_base_item_id, need_buy_base_num, &money_type, &money_num))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_WORKSHOP_WITHOUT_LEGAL_RESULTS);
				return -__LINE__;
			}

			if (!m_role_module_mgr->GetMoney()->MoneyTypeMoreThan(money_type, money_num))
			{
				return -__LINE__;
			}
		}

		if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(cfg->need_coin))
		{
			return -__LINE__;
		}

		for (std::map<int, int>::iterator iter = consume_map.begin(); iter != consume_map.end(); ++iter)
		{
			int item_id = iter->first;
			int num = iter->second;

			if (num > 0 && !m_role_module_mgr->GetKnapsack()->HasItem(item_id, num))
			{
				return -__LINE__;
			}
		}
	}

	//消耗
	{
		if (need_buy_base_num > 0)
		{
			if (!m_role_module_mgr->GetMoney()->UseMoneyType(money_type, money_num, __FUNCTION__))
			{
				return -__LINE__;
			}
		}

		m_role_module_mgr->GetMoney()->UseCoinBind(cfg->need_coin, __FUNCTION__);
		for (std::map<int, int>::iterator iter = consume_map.begin(); iter != consume_map.end(); ++iter)
		{
			int item_id = iter->first;
			int num = iter->second;

			if (num > 0)
			{
				m_role_module_mgr->GetKnapsack()->ConsumeItem(item_id, num, __FUNCTION__);
			}			
		}
	}

	int old_zhu_ling_id = zhu_ling_info.item_id;
	//----------------物品已消耗不能return
	if (zhu_ling_info.is_zhu_ling)
	{
		ItemConfigData tmp;
		tmp.item_id = next_cfg->item_id;
		tmp.num = 1;
		tmp.is_bind = true;

		m_role_module_mgr->GetKnapsack()->Put(tmp, PUT_REASON_ZHU_LING_UP_LEVEL);
	}
	else if (zhu_ling_info.param != NULL)
	{
		zhu_ling_info.param->zhu_ling_item_id = next_cfg->item_id;
		this->SetItemDirty(item_src, index, column);
	}

	gamelog::g_log_zhu_ling.printf(LL_INFO, "%s line:%d | role[%d %s] item_src:%d  index:%d column:%d slot_index:%d item_id[%d to %d] level[%d to %d] uuid:%lld", __FUNCTION__, __LINE__,
				       m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), item_src, index, column, slot_index, old_zhu_ling_id, next_cfg->item_id, cfg->zhu_ling_level, next_cfg->zhu_ling_level, grid_data->item_unique_id);

	if (item_src != ZHU_LING_ITEM_SRC_KNAPSACK)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	}

	return 0;
}

int RoleZhuLing::PutOn(int item_src, int index, int column, int slot_index, int other_item_src, int other_index, int other_column, int other_slot_index)
{
	if (slot_index < 0 || slot_index >= EQUIP_ZHU_LING_SLOT_NUM) return -__LINE__;

	const ItemGridData * target_grid_data = this->GetItemGridData(item_src, index, column);
	if (target_grid_data == NULL)
	{
		return -__LINE__;
	}

	ZhuLinParamRet target_zhu_ling_param = this->GetZhuLinParam(target_grid_data, slot_index, true);
	if (!target_zhu_ling_param.is_ret_succ || target_zhu_ling_param.is_zhu_ling)
	{
		return -__LINE__;
	}

	const ItemGridData * other_grid_data = this->GetItemGridData(other_item_src, other_index, other_column);
	if (other_grid_data == NULL)
	{
		return -__LINE__;
	}

	ZhuLinParamRet other_zhu_ling_param = this->GetZhuLinParam(other_grid_data, other_slot_index);
	if (!other_zhu_ling_param.is_ret_succ)
	{
		return -__LINE__;
	}

	if (m_role_module_mgr->GetRole()->GetLevel() < LOGIC_CONFIG->GetZhuLingConfig()->GetSlotNeedLevel(slot_index))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return -__LINE__;
	}

	if (other_zhu_ling_param.is_zhu_ling)  //如果穿上的那个物品是一个铸灵
	{
		if (!LOGIC_CONFIG->GetZhuLingConfig()->CanPutOn(target_grid_data->item_id, other_zhu_ling_param.item_id))
		{
			return -__LINE__;
		}

		ItemConfigData take_off_zhu_ling;
		take_off_zhu_ling.item_id = target_zhu_ling_param.param->zhu_ling_item_id;
		take_off_zhu_ling.num = 1;
		take_off_zhu_ling.is_bind = true;

		if (take_off_zhu_ling.item_id != 0 && !m_role_module_mgr->GetKnapsack()->CheckForPut2(take_off_zhu_ling))
		{
			return -__LINE__;
		}

		if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(other_column, other_index, 1, __FUNCTION__))
		{
			return -__LINE__;
		}

		if (take_off_zhu_ling.item_id != 0)
		{
			m_role_module_mgr->GetKnapsack()->Put(take_off_zhu_ling, PUT_REASON_ZHU_LING_PUT_ON);
		}

		gamelog::g_log_zhu_ling.printf(LL_INFO, "%s line:%d | role[%d %s] item_src:%d  index:%d column:%d slot_index:%d item_id[%d to %d]  uuid:%lld", __FUNCTION__, __LINE__,
					       m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), item_src, index, column, slot_index, take_off_zhu_ling.item_id, other_zhu_ling_param.item_id, target_grid_data->item_unique_id);

		ROLE_LOG_QUICK10(LOG_TYPE_EQUIP_ZHULING, m_role_module_mgr->GetRole(), take_off_zhu_ling.item_id, other_zhu_ling_param.item_id, "chuan shang", NULL, target_zhu_ling_param.eq_item_id, target_grid_data->item_unique_id, 0, 0, 0);

		target_zhu_ling_param.param->zhu_ling_item_id = other_zhu_ling_param.item_id;
		this->SetItemDirty(item_src, index, column);
	}
	else   //如果穿上的那个物品是其他装备上一个铸灵
	{
		if (target_zhu_ling_param.param == other_zhu_ling_param.param)
		{
			return -__LINE__;
		}

		if (!LOGIC_CONFIG->GetZhuLingConfig()->CanPutOn(target_grid_data->item_id, other_zhu_ling_param.item_id))
		{
			return -__LINE__;
		}

		if (!LOGIC_CONFIG->GetZhuLingConfig()->CanPutOn(other_grid_data->item_id, target_zhu_ling_param.item_id))
		{
			return -__LINE__;
		}

		gamelog::g_log_zhu_ling.printf(LL_INFO, "%s line:%d | role[%d %s] item_src:%d  index:%d column:%d slot_index:%d item_id[%d to %d]  uuid:%lld", __FUNCTION__, __LINE__,
					       m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), item_src, index, column, slot_index, target_zhu_ling_param.item_id, other_zhu_ling_param.item_id, target_grid_data->item_unique_id);

		gamelog::g_log_zhu_ling.printf(LL_INFO, "%s line:%d | role[%d %s] item_src:%d  index:%d column:%d slot_index:%d item_id[%d to %d]  uuid:%lld", __FUNCTION__, __LINE__,
					       m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), other_item_src, other_index, other_column, other_slot_index, other_zhu_ling_param.item_id, target_zhu_ling_param.item_id, other_grid_data->item_unique_id);

		ROLE_LOG_QUICK10(LOG_TYPE_EQUIP_ZHULING, m_role_module_mgr->GetRole(), target_zhu_ling_param.item_id, other_zhu_ling_param.item_id, "jiao_huan", NULL, target_zhu_ling_param.eq_item_id, target_grid_data->item_unique_id, 0, 0, 0);
		ROLE_LOG_QUICK10(LOG_TYPE_EQUIP_ZHULING, m_role_module_mgr->GetRole(), other_zhu_ling_param.item_id, target_grid_data->item_id, "jiao_huan", NULL, other_zhu_ling_param.eq_item_id, other_grid_data->item_unique_id, 0, 0, 0);

		std::swap(*target_zhu_ling_param.param, *other_zhu_ling_param.param);

		this->SetItemDirty(item_src, index, column);
		this->SetItemDirty(other_item_src, other_index, other_column);
	}

	if (item_src != ZHU_LING_ITEM_SRC_KNAPSACK)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	}

	return 0;
}

int RoleZhuLing::TakeOff(int item_src, int index, int column, int slot_index)
{
	if (slot_index < 0 || slot_index >= EQUIP_ZHU_LING_SLOT_NUM) return -__LINE__;

	const ItemGridData * grid_data = this->GetItemGridData(item_src, index, column);
	if (grid_data == NULL)
	{
		return -__LINE__;
	}

	ZhuLinParamRet zhu_ling_param = this->GetZhuLinParam(grid_data, slot_index, true);
	if (!zhu_ling_param.is_ret_succ)
	{
		return -__LINE__;
	}

	if (zhu_ling_param.item_id == 0)
	{
		return -__LINE__;
	}
	
	ItemConfigData take_off_zhu_ling;
	take_off_zhu_ling.item_id = zhu_ling_param.item_id;
	take_off_zhu_ling.num = 1;
	take_off_zhu_ling.is_bind = true;
	if (!m_role_module_mgr->GetKnapsack()->Put(take_off_zhu_ling, PUT_REASON_ZHU_LING_TAKE_OFF))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -__LINE__;
	}
	
	zhu_ling_param.param->Reset();
	this->SetItemDirty(item_src, index, column);

	gamelog::g_log_zhu_ling.printf(LL_INFO, "%s line:%d | role[%d %s] item_src:%d  index:%d column:%d slot_index:%d item_id[%d to %d]  uuid:%lld", __FUNCTION__, __LINE__,
				       m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), item_src, index, column, slot_index, zhu_ling_param.item_id, 0, grid_data->item_unique_id);

	ROLE_LOG_QUICK10(LOG_TYPE_EQUIP_ZHULING, m_role_module_mgr->GetRole(), take_off_zhu_ling.item_id, 0, "tuo xia", NULL, zhu_ling_param.eq_item_id, grid_data->item_unique_id, 0, 0, 0);

	if (item_src != ZHU_LING_ITEM_SRC_KNAPSACK)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	}

	return 0;
}

struct  RoleZhuLingDecomposeConsumeKey 
{
	bool operator<(const RoleZhuLingDecomposeConsumeKey & other) const
	{
		if (column < other.column)
			return true;
		if (column > other.column)
			return false;

		if (index < other.index)
			return true;
		if (index > other.index)
			return false;

		return false;
	}

	short column;
	short index;
};

int RoleZhuLing::Decompose(int count, Protocol::CSZhuLingDecomposeReq::Item * item_list)
{
	int fen_chen_num = 0;
	std::vector<ItemConfigData> rewards;

	std::map<RoleZhuLingDecomposeConsumeKey, int> consume_map;
	for (int i = 0; i < count; ++i)
	{
		int column = item_list[i].column;
		int index = item_list[i].index;
		int num = item_list[i].num;

		if (num <= 0)
		{
			return -__LINE__;
		}

		RoleZhuLingDecomposeConsumeKey consume_key;
		consume_key.column = column;
		consume_key.index = index;
		consume_map[consume_key] += num;
	}

	for (std::map<RoleZhuLingDecomposeConsumeKey, int>::iterator iter = consume_map.begin(); iter != consume_map.end(); ++iter)
	{
		int column = iter->first.column;
		int index = iter->first.index;
		int num = iter->second;

		ItemGridData grid_data;
		if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(column, index, &grid_data))
		{
			return -__LINE__;
		}

		if (num <= 0 || grid_data.num < num)
		{
			return -__LINE__;
		}

		const ItemBase * item_info = ITEMPOOL->GetItem(grid_data.item_id);
		if (item_info == NULL)
		{
			return -__LINE__;
		}

		if (item_info->GetItemType() != I_TYPE_ZHU_LING)
		{
			return -__LINE__;
		}

		const ZhuLingCfg * cfg = LOGIC_CONFIG->GetZhuLingConfig()->GetZhuLingCfg(item_info->GetItemId());
		if (cfg == NULL)
		{
			return -__LINE__;
		}

		ZhuLingDecomposeCfgKey key;
		key.spirit_stone_type = cfg->spirit_stone_type;
		key.zhu_ling_level = cfg->zhu_ling_level;
		key.color = item_info->GetColor();
		const ZhuLingDecomposeCfg * decompose_cfg = LOGIC_CONFIG->GetZhuLingConfig()->GetZhuLingDecomposeCfg(key);
		if (decompose_cfg == NULL)
		{
			return -__LINE__;
		}

		fen_chen_num += decompose_cfg->get_fen_chen_num * num;

		for (int i = 0; i < num; ++i)
		{
			int r = RandomNum(100);  //分解获得高一个品质的1级铸灵概率
			if (r < decompose_cfg->get_item_reward_rate)
			{
				const ZhuLingDecomposeRandRewardCfg::ItemCfg * item_cfg = LOGIC_CONFIG->GetZhuLingConfig()->GetDecomposeRandRewardCfg(decompose_cfg->get_item_reward_group_id);
				if (item_cfg)
				{
					rewards.push_back(item_cfg->reward);
				}
			}
		}
	}

	::MergeItemConfigData(rewards);
	if (!m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_ZHU_LING_DECOMPOSE))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -__LINE__;
	}
	m_role_module_mgr->GetMoney()->AddOtherCoin(fen_chen_num, MONEY_TYPE_FEN_CHEN, __FUNCTION__);

	for (std::map<RoleZhuLingDecomposeConsumeKey, int>::iterator iter = consume_map.begin(); iter != consume_map.end(); ++iter)
	{
		int column = iter->first.column;
		int index = iter->first.index;
		int num = iter->second;
		
		m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, index, num, __FUNCTION__);
	}
	
	Protocol::SCZhuLingDecomposeInfo protocol;
	protocol.count = 0;
	protocol.fen_chen_num = fen_chen_num;

	for (int i = 0; i < (int)rewards.size() && protocol.count < ARRAY_LENGTH(protocol.item_list); ++i)
	{
		protocol.item_list[protocol.count].item_id = rewards[i].item_id;
		protocol.item_list[protocol.count].num = rewards[i].num;
		protocol.count += 1;
	}

	int send_len = sizeof(protocol) - (ARRAY_LENGTH(protocol.item_list) - protocol.count) * sizeof(protocol.item_list[0]);
	m_role_module_mgr->NetSend(&protocol, send_len);
	return 0;
}

const ItemGridData * RoleZhuLing::GetItemGridData(int item_src, int index, int column) const
{
	const ItemGridData * grid_data = NULL;
	if (ZHU_LING_ITEM_SRC_KNAPSACK == item_src)
	{
		grid_data = m_role_module_mgr->GetKnapsack()->GetItemGridData2(column, index);
	}

	if (ZHU_LING_ITEM_SRC_EQUIP == item_src)
	{
		grid_data = m_role_module_mgr->GetEquipmentManager()->GetEquipmentGridData2(index);
	}

	if (ZHU_LING_ITEM_SRC_LING_SHI == item_src)
	{
		grid_data = m_role_module_mgr->GetEquipmentManager()->GetJewelryGridData2(index);
	}

	return grid_data;
}

int  RoleZhuLing::GetZhuLinItemId(ItemGridData * grid_data, int slot_index)
{
	int zhu_ling_item_id = 0;
	if (grid_data == NULL)
	{
		return zhu_ling_item_id;
	}

	const ItemBase * item_info = ITEMPOOL->GetItem(grid_data->item_id);
	if (item_info == NULL)
	{
		return zhu_ling_item_id;
	}

	switch (item_info->GetItemType())
	{
		case I_TYPE_EQUIPMENT:
			{
				if (0 <= slot_index && slot_index < ARRAY_LENGTH(grid_data->param->equip_param.zhu_ling_info))
				{
					zhu_ling_item_id = grid_data->param->equip_param.zhu_ling_info[slot_index].zhu_ling_item_id;
				}
			}
			break;
		case I_TYPE_JEWELRY:
			{
				if (0 <= slot_index && slot_index < ARRAY_LENGTH(grid_data->param->jewelry_param.zhu_ling_info))
				{
					zhu_ling_item_id = grid_data->param->jewelry_param.zhu_ling_info[slot_index].zhu_ling_item_id;
				}
			}
			break;
		case I_TYPE_ZHU_LING:
			{
				zhu_ling_item_id = grid_data->item_id;
			}
			break;
	}

	return zhu_ling_item_id;
}

ZhuLinParamRet  RoleZhuLing::GetZhuLinParam(const ItemGridData * grid_data, int slot_index,bool is_need_equipment)
{
	ZhuLinParamRet ret;
	ret.item_id = 0;
	ret.eq_item_id= 0;
	ret.is_zhu_ling = false;
	ret.is_ret_succ = false;

	if (grid_data == NULL)
	{
		return ret;
	}

	const ItemBase * item_info = ITEMPOOL->GetItem(grid_data->item_id);
	if (item_info == NULL)
	{
		return ret;
	}
	
	switch (item_info->GetItemType())
	{
		case I_TYPE_EQUIPMENT:
			{
				if (0 <= slot_index && slot_index < ARRAY_LENGTH(grid_data->param->equip_param.zhu_ling_info))
				{
					ret.is_ret_succ = true;
					ret.eq_item_id = grid_data->item_id;
					ret.item_id = grid_data->param->equip_param.zhu_ling_info[slot_index].zhu_ling_item_id;
					ret.param = &grid_data->param->equip_param.zhu_ling_info[slot_index];
				}
			}
			break;
		case I_TYPE_JEWELRY:
			{
				if (0 <= slot_index && slot_index < ARRAY_LENGTH(grid_data->param->jewelry_param.zhu_ling_info))
				{
					ret.is_ret_succ = true;
					ret.eq_item_id = grid_data->item_id;
					ret.item_id = grid_data->param->jewelry_param.zhu_ling_info[slot_index].zhu_ling_item_id;
					ret.param = &grid_data->param->jewelry_param.zhu_ling_info[slot_index];
				}
			}
			break;
		case I_TYPE_ZHU_LING:
			{
				if (!is_need_equipment)
				{
					ret.is_ret_succ = true;
					ret.item_id = grid_data->item_id;
					ret.is_zhu_ling = true;
				}
			}
			break;
	}

	return ret;
}

int RoleZhuLing::SetItemDirty(int item_src, int index, int column)
{
	if (item_src == ZHU_LING_ITEM_SRC_KNAPSACK)
	{
		m_role_module_mgr->GetKnapsack()->SetItemDirty(column, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);	
	}

	if (item_src == ZHU_LING_ITEM_SRC_EQUIP)
	{
		m_role_module_mgr->GetEquipmentManager()->SetEquipGridDirty(index, true);		
	}

	if (item_src == ZHU_LING_ITEM_SRC_LING_SHI)
	{
		m_role_module_mgr->GetEquipmentManager()->SetJewelryGridDirty(index, true);
	}

	return 0;
}

bool RoleZhuLing::IsTargetAndSourceSame(int item_src, int index, int column, int slot_index, int other_item_src, int other_index, int other_column, int other_slot_index)
{
	return false;
}
