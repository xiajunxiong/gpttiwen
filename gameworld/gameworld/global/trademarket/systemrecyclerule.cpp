#include "systemrecyclerule.hpp"
#include "servercommon/struct/global/trademarketparam.hpp"
#include "item/itembase.h"
#include "engineadapter.h"

RecycleRuleInfo::RecycleRuleInfo() : m_is_dirty(false), m_old_state(false),
	m_invalid_flag(false), m_item_id(0u), m_recycle_num_limit(0), m_recycle_price_limit(0),
	m_cur_recycle_num(0), m_begin_timestamp(0u), m_end_timestamp(0u), m_rule_id(0LL)
{
	m_rule_detail.Reset();
}

RecycleRuleInfo::~RecycleRuleInfo()
{

}

bool RecycleRuleInfo::Init(const RecycleRuleParam& param, bool is_from_init)
{
	if (is_from_init)
	{
		m_is_dirty = false;
		m_old_state = true;
	}
	else
	{
		m_is_dirty = true;
		m_old_state = false;
	}

	m_item_id = param.item_id;
	m_begin_timestamp = param.begin_timestamp;
	m_end_timestamp = param.end_timestamp;
	m_recycle_num_limit = param.recycle_num_limit;
	m_recycle_price_limit = param.recycle_price_limit;
	m_cur_recycle_num = param.cur_recycle_num;
	m_rule_id = param.rule_id;
	m_rule_detail.SetData(param.data);

	return true;
}

bool RecycleRuleInfo::GetInitParam(ARG_OUT RecycleRuleParam* param)
{
	if (NULL == param) return false;

	param->rule_id = m_rule_id;
	param->recycle_num_limit = m_recycle_num_limit;
	param->recycle_price_limit = m_recycle_price_limit;
	param->cur_recycle_num = m_cur_recycle_num;
	param->begin_timestamp = m_begin_timestamp;
	param->end_timestamp = m_end_timestamp;
	param->item_id = m_item_id;
	param->reserve_sh = 0;
	m_rule_detail.GetData(param->data);

	return true;
}

bool RecycleRuleInfo::IsMatchRule(ItemID item_id, int price, const NetValueItemParam& item_param)
{
	if (item_id != m_item_id) return false;

	if (m_cur_recycle_num >= m_recycle_num_limit) return false;

	if (price > m_recycle_price_limit) return false;
	
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (now < m_begin_timestamp || now > m_end_timestamp) return false;

	const ItemBase* item = ITEMPOOL->GetItem(item_id);
	if (NULL == item) return false;

	if (item->GetItemType() < I_TYPE_NET_VALUE_ITEM_BEGIN) return true;

	switch (item->GetItemType())
	{
	case I_TYPE_PET:
		{
			return this->IsMatchRulePet(item_param);
		}
		break;

	case I_TYPE_EQUIPMENT:
		{
			return this->IsMatchRuleEquipment(item_param);
		}
		break;


	default:
		{
			return false;
		}
		break;
	}

	return false;
}

bool RecycleRuleInfo::IsMatchRulePet(const NetValueItemParam& item_param)
{
	if (m_rule_detail.pet.growth_total > 0)
	{
		int total_growth = 0;
		for (int i = ADD_POINT_TYPE_MIN; i < ADD_POINT_TYPE_MAX; ++i)
		{
			total_growth += item_param.pet_param.growth[i];
		}
		if (total_growth < m_rule_detail.pet.growth_total)
		{
			return false;
		}
	}

	if (m_rule_detail.pet.skill_num > 0)
	{
		int total_skill_num = 0;
		for (int i = 0; i < PET_PASSIVE_MAX_COUNT; ++i)
		{
			if (item_param.pet_param.passive_list[i].passive_skill_id > 0)
			{
				total_skill_num += 1;
			}
		}
		if (total_skill_num < m_rule_detail.pet.skill_num)
		{
			return false;
		}
	}

	for (int i = ADD_POINT_TYPE_MIN; i < ADD_POINT_TYPE_MAX; ++i)
	{
		if (m_rule_detail.pet.growth[i] > 0 && item_param.pet_param.growth[i] < m_rule_detail.pet.growth[i])
		{
			return false;
		}
	}

	return true;
}

bool RecycleRuleInfo::IsMatchRuleEquipment(const NetValueItemParam& item_param)
{
	for (int i = 0; i < m_rule_detail.equipment.attr_num; ++i)
	{
		for (int k = 0; k < item_param.equip_param.attr_num; ++k)
		{
			if (item_param.equip_param.attr_list[k].attr_type == m_rule_detail.equipment.attr_list[i].attr_type
				&& item_param.equip_param.attr_list[k].attr_value < m_rule_detail.equipment.attr_list[i].attr_value)
			{
				return false;
			}
		}
	}

	for (int i = 0; i < m_rule_detail.equipment.extra_attr_num; ++i)
	{
		for (int k = 0; k < item_param.equip_param.inc_num; ++k)
		{
			if (item_param.equip_param.attr_extra_inc[k].attr_type == m_rule_detail.equipment.extra_attr_list[i].attr_type
				&& item_param.equip_param.attr_extra_inc[k].attr_value < m_rule_detail.equipment.extra_attr_list[i].attr_value)
			{
				return false;
			}
		}
	}

	return true;
}

void RecycleRuleInfo::OnRecycle()
{
	m_cur_recycle_num += 1;
	m_is_dirty = true;
}

bool RecycleRuleInfo::IsLapse(unsigned int now)
{
	if (m_end_timestamp > 0 && now > m_end_timestamp) return true;
	if (m_recycle_num_limit > 0 && m_cur_recycle_num >= m_recycle_num_limit) return true;

	return false;
}

