#include "itembase.h"
#include "itemgriddata.h"

#include "gamedef.h"
#include "itempool.h"

#include "engineadapter.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/logicconfigmanager.hpp"
#include "other/currencyconfig.hpp"
#include "other/pet/petconfig.hpp"
#include "gamelog.h"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "other/medal/medalconfig.hpp"
#include "servercommon/petdef.h"
#include "other/equipstar/equipstarconfig.hpp"
#include "global/trademarket/trademarketconfig.hpp"

//////////////////////////////////////////////////////////////////////////

ItemBase::ItemBase(short i_type) 
	: m_item_type(i_type), 
	m_item_id(0), 
	m_color(0), 
	m_is_bind(false), 
	m_can_discard(false), 
	m_is_broadcast(false), 
	m_sell_price(0), 
	m_is_record(true),
	m_time_length(0),
	m_invalid_time(0),
	m_limit_prof(0),
	m_limit_level(0),
	m_pile_limit(0),
	m_item_column(0),
	m_bags_belong(0),
	m_item_trade_money_type(ItemNamespace::ITEM_TRADE_MONEY_TYPE_INVALID),
	m_trade_need_announcement(false),
	m_item_trade_min_price(0),
	m_item_trade_max_price(0),
	m_trade_search_type(-1),
	m_trade_num_per_group(1),
	m_is_limit_drop(false),
	m_drop_cooldown_s(0)
{
	memset(m_item_name, 0, sizeof(m_item_name));
}

ItemBase::~ItemBase()
{

}

bool ItemBase::CanDiscard(NetValueItemParam* param, int *reason) const
{
	return true; //!< \since G6

	//if (NULL != reason) *reason = NDR_DEFAULT;

	//return m_can_discard;
}

bool ItemBase::IsNeedReCalSmeltEntryNum() const
{
	//仅装备和灵饰有熔炼
	if(m_item_type != I_TYPE_JEWELRY && m_item_type != I_TYPE_EQUIPMENT) return false;

	if (m_color >= I_COLOR_STAR && m_color <= I_COLOR_GOD)
	{
		return true;
	}

	return false;
}

NetValueItemParam * ItemBase::CreateParam()
{
	return new NetValueItemParam();
}

NetValueItemParam * ItemBase::CreateParamFromStruct(const NetValueItemParam& param_data)
{
	NetValueItemParam* param = new NetValueItemParam();
	param_data.GetData(param->data);
	return param;
}

NetValueItemParam * ItemBase::CreateParamFromParamData(const NetValueItemParamData param_data)
{
	NetValueItemParam* param = new NetValueItemParam();
	memcpy(param->data, param_data, sizeof(NetValueItemParamData));
	return param;
}

bool ItemBase::HasRandomNetValue(NetValueItemParam* param_data) const
{
	if (NULL == param_data) return false;

	UNSTD_STATIC_CHECK(20 == NET_VALLUE_ITEM_COUNT); // 需要随机的净值需要加这个
	bool ret = false;
	switch (m_item_type)
	{
	case I_TYPE_FU_YU:
		ret = param_data->fu_yu_param.has_net_value != 0;
		break;
	case I_TYPE_RUNE_STONE:
		ret = param_data->rune_stone_param.has_net_value != 0;
		break;
	case I_TYPE_HOLY_EQUIP:
		ret = param_data->holy_equip_param.has_net_value != 0;
		break;
	case I_TYPE_ZAO_HUA_EQUIP:
		ret = param_data->zao_hua_param.has_net_value != 0;
		break;
	case I_TYPE_PEAK_TOKEN:
		ret = param_data->peak_token_param.has_net_value != 0;
		break;
	case I_TYPE_DIAN_HUA_SHI:
		ret = param_data->dian_hua_shi_param.has_net_value != 0;
		break;
	case I_TYPE_ACCESSORIES:
		ret = param_data->accessoriess_param.has_net_value != 0;
		break;
	case I_TYPE_GOD_PRINT:
		ret = param_data->god_print_param.has_random_net_value != 0;
		break;
	case I_TYPE_EQUIPMENT:
		ret = param_data->equip_param.has_random_net_value != 0;
		break;
	case I_TYPE_TREASURE_MAP:
		ret = param_data->treasure_map_param.scene_id != 0;
		break;
	case I_TYPE_ELEMENT_CRYSTAL:
		ret = param_data->element_crystal_param.has_set_net_value > 0;
		break;
	case I_TYPE_JEWELRY:
		ret = param_data->jewelry_param.has_random_net_value > 0;
		break;
	case I_TYPE_PARTNER_NATAL_CARD:
		ret = param_data->partner_natal_card_param.has_net_value > 0;
		break;
	case I_TYPE_PET_EQUIPMENT:
		ret = param_data->pet_equipment_param.has_random_net_value == 1;
		break;
	case I_TYPE_MEDAL:
		ret = param_data->medal_param.has_net_value != 0;
		break;
	case I_TYPE_PET_SOUL_EQUIP:
		ret = param_data->pet_soul_equipment_param.has_random_net_value != 0;
		break;
	case I_TYPE_PET_BADGE:
		ret = param_data->pet_badge_item_param.has_random_net_value != 0;
		break;
	case I_TYPE_YUAN_HUN_ZHU:
		ret = param_data->yuan_hun_zhu_param.has_net_value != 0;
		break;
	default:
		break;
	}

	return ret;
}

void ItemBase::SetRandomNetValueFlag(NetValueItemParam* param_data) const
{
	if (NULL == param_data) return;

	UNSTD_STATIC_CHECK(20 == NET_VALLUE_ITEM_COUNT); // 需要随机的净值需要加这个
	switch (m_item_type)
	{
	case I_TYPE_FU_YU:
		param_data->fu_yu_param.has_net_value = 1;
		break;
	case I_TYPE_RUNE_STONE:
		param_data->rune_stone_param.has_net_value = 1;
		break;
	case I_TYPE_HOLY_EQUIP:
		param_data->holy_equip_param.has_net_value = 1;
		break;
	case I_TYPE_ZAO_HUA_EQUIP:
		param_data->zao_hua_param.has_net_value = 1;
		break;
	case I_TYPE_PEAK_TOKEN:
		param_data->peak_token_param.has_net_value = 1;
		break;
	case I_TYPE_DIAN_HUA_SHI:
		param_data->dian_hua_shi_param.has_net_value = 1;
		break;
	case I_TYPE_ACCESSORIES:
		param_data->accessoriess_param.has_net_value = 1;
		break;
	case I_TYPE_GOD_PRINT:
		param_data->god_print_param.has_random_net_value = 1;
		break;
	case I_TYPE_EQUIPMENT:
		param_data->equip_param.has_random_net_value = 1;
		break;
	case I_TYPE_TREASURE_MAP:
		param_data->treasure_map_param.is_set_rand = 1;
		break;
	case I_TYPE_ELEMENT_CRYSTAL:
		param_data->element_crystal_param.has_set_net_value = 1;
		break;
	case I_TYPE_JEWELRY:
		param_data->jewelry_param.has_random_net_value = 1;
		break;
	case I_TYPE_PARTNER_NATAL_CARD:
		param_data->partner_natal_card_param.has_net_value = 1;
		break;
	case I_TYPE_PET_EQUIPMENT:
		param_data->pet_equipment_param.has_random_net_value = 1;
		break;
	case I_TYPE_MEDAL:
		param_data->medal_param.has_net_value = 1;
		break;
	case I_TYPE_PET_SOUL_EQUIP:
		param_data->pet_soul_equipment_param.has_random_net_value = 1;
		break;
	case I_TYPE_PET_BADGE:
		param_data->pet_badge_item_param.has_random_net_value = 1;
		break;
	case I_TYPE_YUAN_HUN_ZHU:
		param_data->yuan_hun_zhu_param.has_net_value = 1;
		break;
	default:
		break;
	}
}

bool ItemBase::CanMerge() const
{
	return this->GetItemType() < I_TYPE_NET_VALUE_ITEM_BEGIN;  // 带有净值的物品都不能merge
}

std::vector<int> ItemBase::GetTradeMinPrice(const ItemDataWrapper* wrapper) const
{
	switch (m_item_type)
	{
	case I_TYPE_MEDAL:
		{
			if (NULL != wrapper)
			{
				static NetValueItemParam param;
				param.SetData(wrapper->param_data);

				const MedalMarketCfg* cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalMarketCfg(param.medal_param.grade);
				if (NULL != cfg)
				{
					return cfg->trade_min_gold_price;
				}
			}
		}
		break;
	case I_TYPE_EQUIPMENT:
		{
			if (NULL != wrapper)
			{
				static NetValueItemParam param;
				param.SetData(wrapper->param_data);

				const EquipStarCfg* cfg = LOGIC_CONFIG->GetEquipStarConfig()->GetEquipStarAttrCfg(m_item_id, param.equip_param.star_level);
				if (NULL != cfg)
				{
					return cfg->trade_min_gold_price;
				}
			}
		}
		break;
	case I_TYPE_JEWELRY:
		{
			if (NULL != wrapper)
			{
				static NetValueItemParam param;
				param.SetData(wrapper->param_data);

				const EquipStarCfg* cfg = LOGIC_CONFIG->GetEquipStarConfig()->GetEquipStarAttrCfg(m_item_id, param.jewelry_param.star_level);
				if (NULL != cfg)
				{
					return cfg->trade_min_gold_price;
				}
			}
		}
		break;
	case I_TYPE_PET:
		{
			if (NULL != wrapper)
			{
				static NetValueItemParam param;
				param.SetData(wrapper->param_data);

				const TradePetPriceRangeCfg * price_range_cfg = TradeMarketConfig::Instance().GetPetPriceRangeCfg(m_item_id, param.pet_param.str_level);
				if (NULL != price_range_cfg)
				{
					std::vector<int> tmp_ret;
					tmp_ret.push_back(price_range_cfg->min_price);
					return tmp_ret;
				}
			}
		}
		break;
	default:
		break;
	}

	if (m_item_trade_min_price.empty())
	{
#ifdef _DEBUG
		assert(0);
#endif
	}

	return m_item_trade_min_price;
}

std::vector<int> ItemBase::GetTradeMaxPrice(const ItemDataWrapper* wrapper) const
{
	switch (m_item_type)
	{
	case I_TYPE_MEDAL:
		{
			if (NULL != wrapper)
			{
				static NetValueItemParam param;
				param.SetData(wrapper->param_data);

				const MedalMarketCfg* cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalMarketCfg(param.medal_param.grade);
				if (NULL != cfg)
				{
					return cfg->trade_max_gold_price;
				}
			}
		}
		break;
	case I_TYPE_EQUIPMENT:
		{
			if (NULL != wrapper)
			{
				static NetValueItemParam param;
				param.SetData(wrapper->param_data);

				const EquipStarCfg* cfg = LOGIC_CONFIG->GetEquipStarConfig()->GetEquipStarAttrCfg(m_item_id, param.equip_param.star_level);
				if (NULL != cfg)
				{
					return cfg->trade_max_gold_price;
				}
			}
		}
		break;
	case I_TYPE_JEWELRY:
		{
			if (NULL != wrapper)
			{
				static NetValueItemParam param;
				param.SetData(wrapper->param_data);

				const EquipStarCfg* cfg = LOGIC_CONFIG->GetEquipStarConfig()->GetEquipStarAttrCfg(m_item_id, param.jewelry_param.star_level);
				if (NULL != cfg)
				{
					return cfg->trade_max_gold_price;
				}
			}
		}
		break;
	case I_TYPE_PET:
		{
			if (NULL != wrapper)
			{
				static NetValueItemParam param;
				param.SetData(wrapper->param_data);

				const TradePetPriceRangeCfg * price_range_cfg = TradeMarketConfig::Instance().GetPetPriceRangeCfg(m_item_id, param.pet_param.str_level);
				if (NULL != price_range_cfg)
				{
					std::vector<int> tmp_ret;
					tmp_ret.push_back(price_range_cfg->max_price);
					return tmp_ret;
				}
			}
		}
		break;
	default:
		break;
	}

	if (m_item_trade_max_price.empty())
	{
#ifdef _DEBUG
		assert(0);
#endif
	}

	return m_item_trade_max_price;
}

std::vector<int> ItemBase::GetTradeRecommendPrice(const ItemDataWrapper* wrapper) const
{
	switch (m_item_type)
	{
	case I_TYPE_MEDAL:
		{
			if (NULL != wrapper)
			{
				static NetValueItemParam param;
				param.SetData(wrapper->param_data);

				const MedalMarketCfg* cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalMarketCfg(param.medal_param.grade);
				if (NULL != cfg)
				{
					return cfg->trade_recommend_price;
				}
			}
		}
		break;
	case I_TYPE_PET:
		{
			if (NULL != wrapper)
			{
				static NetValueItemParam param;
				param.SetData(wrapper->param_data);

				const TradePetPriceRangeCfg * price_range_cfg = TradeMarketConfig::Instance().GetPetPriceRangeCfg(m_item_id, param.pet_param.str_level);
				if (NULL != price_range_cfg)
				{
					std::vector<int> tmp_ret;
					tmp_ret.push_back(price_range_cfg->min_price);
					return tmp_ret;
				}
			}
		}
		break;
	default:
		break;
	}

	if (m_item_trade_recommend_price.empty())
	{
#ifdef _DEBUG
		assert(0);
#endif
	}

	return m_item_trade_recommend_price;
}

bool ItemBase::CheckTradePrice(const std::vector<int>& min_price_vec, const std::vector<int>& max_price_vec, const std::vector<int>& recommend_price_vec)
{
	if (min_price_vec.size() != max_price_vec.size() || min_price_vec.size() != recommend_price_vec.size())
	{
		return false;
	}

	for (size_t i = 0; i < min_price_vec.size() && i < max_price_vec.size(); ++i)
	{
		if (min_price_vec[i] > max_price_vec[i])
		{
			return false;
		}
	}

	for (size_t i = 0; i < min_price_vec.size() && i < max_price_vec.size() && i < recommend_price_vec.size(); ++i)
	{
		if (recommend_price_vec[i] < min_price_vec[i] || recommend_price_vec[i] > min_price_vec[i])
		{
			return false;
		}
	}

	return true;
}

bool ItemBase::CanTrade(const ItemGridData *item_data)
{
	if (NULL == item_data || item_data->is_bind) return false;
	const ItemBase* item = ITEMPOOL->GetItem(item_data->item_id);
	if (NULL == item) return false;

	return item->CanTrade();
}

bool ItemBase::CanTrade() const
{
	return ItemNamespace::ITEM_TRADE_MONEY_TYPE_INVALID != m_item_trade_money_type;
}

bool ItemBase::HasInvalidTime() const
{
	return m_time_length > 0 || m_invalid_time > 0;
}

UInt32 ItemBase::CalInvalidTime() const
{
	UInt32 tmp_time = 0;
	if (m_time_length > 0)
	{
		tmp_time = static_cast<UInt32>(EngineAdapter::Instance().Time()) + m_time_length;
	}

	if (m_invalid_time > 0 && (0 == tmp_time || tmp_time > m_invalid_time))
	{
		tmp_time = m_invalid_time;
	}

	return tmp_time;
}

int PetParam::GetPetPassiveSkillAndLianYaoPassiveSkillNum()
{
	int count = 0;
	for (int i = 0; i < ARRAY_LENGTH(passive_list); ++i)
	{
		if (passive_list[i].passive_skill_id != 0)
		{
			count += 1;
		}
	}

	for (int i = 0; i < ARRAY_LENGTH(lian_yao_passive_list); ++i)
	{
		if (lian_yao_passive_list[i].passive_skill_id != 0)
		{
			count += 1;
		}
	}

	return count;
}

//目前删除时候会调用，记录log
std::string PetParam::ToString(long long pet_uuid) const  
{
	UNSTD_STATIC_CHECK(ADD_POINT_TYPE_MAX == 5);//下面用到growth[4] attr[4]
	UNSTD_STATIC_CHECK(PET_BADGE_ITEM_ATTR_NUM == 2);  //pet_bage_item.attrs[1]

	std::string ret = STRING_SPRINTF("PetParam:{pet_id:%d, name:%s, str_level:%d, level:%d, exp:%lld, add_exp:%lld, "
		"maxhp:%d, maxmp:%d, pet_growth_version[%d], passive_list:{%s},lian_yao_passive_list:{%s}, growth_list:[%d,%d,%d,%d,%d], comprehend_num:%d, comprehend_value:%d, "
		"attr:[%d,%d,%d,%d,%d] "
		"m_soul_equip_1[itemid:%d is_bind:%d is_lock:%d unique_id:%lld exp:%d max_exp:%d level:%d gread:%d refine:%d sp_id:%d attr[%s], "
		"m_soul_equip_2[itemid:%d is_bind:%d is_lock:%d unique_id:%lld exp:%d max_exp:%d level:%d gread:%d refine:%d sp_id:%d attr[%s]"
		"lian_yao_pet_id:%d lian_yao_pet_str_lv:%d feisheng_times:%d feisheng_skill_id:%d feisheng_skill_lv:%d PetBadge[item_id:%d item_uuid:%lld skill_id:%d attr0:%d-%d attr1:%d-%d] }",
		pet_id, name, (int)str_level, (int)level,
		exp, add_exp, max_hp, max_mp, (int)pet_growth_version,
		this->GetPassiveInfoLog(passive_list, PET_PASSIVE_MAX_COUNT),
		this->GetPassiveInfoLog(lian_yao_passive_list, ARRAY_LENGTH(lian_yao_passive_list)),
		growth[0], growth[1], growth[2], growth[3], growth[4],
		comprehend_gift_num, comprehend_passive_value,
		attr[0], attr[1], attr[2], attr[3], attr[4],
		m_soul_equip_1.item_id, m_soul_equip_1.is_bind, m_soul_equip_1.is_lock, m_soul_equip_1.item_unique_id,
		m_soul_equip_1.exp, m_soul_equip_1.m_max_exp, m_soul_equip_1.m_level, m_soul_equip_1.m_gread,
		m_soul_equip_1.m_refine, m_soul_equip_1.sp_id, m_soul_equip_1.GetAttrInfoLog(m_soul_equip_1.add_attr_list, m_soul_equip_1.attr_num),
		m_soul_equip_2.item_id, m_soul_equip_2.is_bind, m_soul_equip_2.is_lock, m_soul_equip_2.item_unique_id,
		m_soul_equip_2.exp, m_soul_equip_2.m_max_exp, m_soul_equip_2.m_level, m_soul_equip_2.m_gread,
		m_soul_equip_2.m_refine, m_soul_equip_2.sp_id, m_soul_equip_2.GetAttrInfoLog(m_soul_equip_2.add_attr_list, m_soul_equip_2.attr_num),
		lian_yao_pet_id, lian_yao_pet_str_lv,
		fei_sheng_times, fei_sheng_select_skill_id, fei_sheng_select_skill_level,
		pet_bage_item.pet_badgei_item_id, pet_bage_item.item_uuid, pet_bage_item.skill_id, pet_bage_item.attrs[0].attr_type, pet_bage_item.attrs[0].attr_value, pet_bage_item.attrs[1].attr_type, pet_bage_item.attrs[1].attr_value
	);



	return ret;
}

std::string PetParam::GetPetBaseInfoString(long long pet_uuid) const
{
	std::string ret = STRING_SPRINTF("Pet[pet_uuid:%lld pet_id:%d, name:%s lian_yao_pet_id:%d]", pet_uuid, pet_id, name, lian_yao_pet_id);
	return ret;
}

bool PetParam::IsHasPetTalent() const
{
	for (int i = 0; i < ARRAY_LENGTH(talent_list); ++i)
	{
		if (talent_list[i].inborn_level > 0)
		{
			return true;
		}
	}

	return false;
}

bool PetParam::CheckGrowthVersion(long long pet_item_unique_id, const char* caller_func)
{
	int old_version = pet_growth_version;
	
	const PetGrowthVersionCfg* version_cfg = LOGIC_CONFIG->GetPetConfig()->GetGrowthVersion(pet_id, old_version);
	if (NULL == version_cfg)
	{
		return false;
	}

	const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
	if (NULL == pet_cfg)
	{
		return false;
	}

	unsigned short old_growth[ADD_POINT_TYPE_MAX] = { 0 };
	int growth_dec_max = LOGIC_CONFIG->GetPetConfig()->GetAttrDecNumMax();

	// 如果能查找到，则该宠物的资质版本是旧的，直接更新到最新即可
	for (int i = 0; i < ARRAY_ITEM_COUNT(growth); ++i)
	{
		old_growth[i] = growth[i];

		// 防止数据中存在资质值比配置中的大(配置的资质值是满资质的值）
		growth[i] = GetMin(growth[i], static_cast<unsigned short>(version_cfg->growth[i]));
		growth[i] = GetMax(growth[i], static_cast<unsigned short>(version_cfg->growth[i] - growth_dec_max));
		if (new_growth[i] > 0)
		{
			new_growth[i] = GetMin(new_growth[i], static_cast<unsigned short>(version_cfg->growth[i]));
			new_growth[i] = GetMax(new_growth[i], static_cast<unsigned short>(version_cfg->growth[i] - growth_dec_max));
		}

		/*
#ifdef _DEBUG
		// 防止策划填错配置，这里用了unsigned short， 如果growth小于配置中的growth，相减会变成一个较大的值，造成数据问题
		assert(growth[i] <= version_cfg->growth[i] && growth[i] >= version_cfg->growth[i] - growth_dec_max);
		if (new_growth[i] > 0)
		{
			assert(new_growth[i] <= version_cfg->growth[i] && new_growth[i] >= version_cfg->growth[i] - growth_dec_max);
		}
#else
		// 防止数据中存在资质值比配置中的大(配置的资质值是满资质的值）
		growth[i] = GetMin(growth[i], version_cfg->growth[i]);
		growth[i] = GetMax(growth[i], version_cfg->growth[i] - growth_dec_max);
		if (new_growth[i] > 0)
		{
			new_growth[i] = GetMin(new_growth[i], version_cfg->growth[i]);
			new_growth[i] = GetMax(new_growth[i], version_cfg->growth[i] - growth_dec_max);
		}
#endif
		*/

		growth[i] = pet_cfg->growth[i] - (version_cfg->growth[i] - growth[i]);
		if (new_growth[i] > 0)
		{
			new_growth[i] = pet_cfg->growth[i] - (version_cfg->growth[i] - new_growth[i]);
		}
	}
	pet_growth_version = LOGIC_CONFIG->GetPetConfig()->GetPetCurGrowthVersion(pet_id);

	gamelog::g_log_pet.printf(LL_INFO, "PetParam::CheckGrowthVersion caller_func[%s] \
		pet_id[%d] name[%s] unique_id[%lld] \
		new_version[%d] new_growth[%d,%d,%d,%d,%d] \
		old_version[%d] old_growth[%d,%d,%d,%d,%d]",
		caller_func, pet_id, name, pet_item_unique_id, 
		pet_growth_version, growth[0], growth[1], growth[2], growth[3], growth[4],
		old_version, old_growth[0], old_growth[1], old_growth[2], old_growth[3], old_growth[4]);

	UNSTD_STATIC_CHECK(5 == ADD_POINT_TYPE_MAX);

	return true;
}

bool PetParam::CheckModifyVersion()
{
	bool is_change = false;
	if (modify_ver == 0)
	{
		modify_ver += 1;

		PetPassiveItemInfo passive_list_old[8];
		PetPassiveItemInfo new_passive_list_old[8];

		memcpy(passive_list_old, &reserve_int1, sizeof(reserve_int1));
		memcpy((char *)passive_list_old + 4, use_attr_pellet_list, sizeof(passive_list_old) - 4);
		memcpy(new_passive_list_old, (char *)use_attr_pellet_list + (sizeof(passive_list_old) - 4), sizeof(new_passive_list_old));
		UNSTD_STATIC_CHECK(sizeof(passive_list_old) + sizeof(new_passive_list_old) == sizeof(reserve_int1) + sizeof(use_attr_pellet_list));
		UNSTD_STATIC_CHECK((int)sizeof(reserve_int1) == 4);
		UNSTD_STATIC_CHECK(sizeof(passive_list_old) > sizeof(reserve_int1));
		UNSTD_STATIC_CHECK(sizeof(use_attr_pellet_list) > sizeof(passive_list_old));
		UNSTD_STATIC_CHECK(sizeof(use_attr_pellet_list) > sizeof(new_passive_list_old));

		for (int i = 0; i < ARRAY_LENGTH(passive_list) && i < ARRAY_LENGTH(passive_list_old); i++)
		{
			passive_list[i] = passive_list_old[i];
		}

		for (int i = 0; i < ARRAY_LENGTH(new_passive_list) && i < ARRAY_LENGTH(new_passive_list_old); i++)
		{
			new_passive_list[i] = new_passive_list_old[i];
		}
		is_change = true;
	}
	if (modify_ver == 1)
	{
		reserve_int = 0;
		reserve_sh = 0;
		reserve_int1 = 0;
		memset(use_attr_pellet_list, 0, sizeof(use_attr_pellet_list));
		is_change = true;
		modify_ver += 1;
	}

	UNSTD_STATIC_CHECK(MAX_PET_MODIFY_VER == 2);

	return is_change;
}

bool MedalItemParam::CheckModifyVersion()
{
	if (medal_move_ver == 0)
	{
		const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_id);
		if (NULL != spe_cfg)
		{
			medal_move_ver += 1;
			grade = spe_cfg->medal_grade;
			
			return true;
		}
	}

	return false;
}

bool EquipmentParam::CheckModifyVersion()
{
	if (modify_ver == 0)
	{
		modify_ver += 1;

		for (int i = 0; i < ARRAY_LENGTH(old_stone) && i < ARRAY_LENGTH(new_stone); i++)
		{
			new_stone[i] = old_stone[i];
		}

		return true;
	}

	UNSTD_STATIC_CHECK(MAX_NEW_STONE_MODIFY_VER_NUM == 1);

	return false;
}

bool JewelryParam::CheckModifyVersion()
{
	if (modify_ver == 0)
	{
		modify_ver += 1;

		for (int i = 0; i < ARRAY_LENGTH(old_stone) && i < ARRAY_LENGTH(new_stone); i++)
		{
			new_stone[i] = old_stone[i];
		}

		return true;
	}

	UNSTD_STATIC_CHECK(MAX_NEW_STONE_MODIFY_VER_NUM == 1);

	return false;
}

std::string YuanHunZhuParam::ToString() const
{
	std::string ret = STRING_SPRINTF("YuanHunZhuParam[item:%d has_net_value:%d attrs:", yuan_hun_zhu_item_id, has_net_value);
	for (int i = 0; i < ARRAY_LENGTH(attrs); ++i)
	{
		ret += STRING_SPRINTF("slot:%d seq:%d attr_type:%d attr_value:%d,", i, attr_seq[i], attrs[i].attr_type, attrs[i].attr_value);
	}
	
	return ret;
}

std::string RuneStoneParam::ToString() const 
{
	std::string ret = STRING_SPRINTF("FuShiParam(has_net_value:%d) item_id[%d] attr_num[%d] ", has_net_value, item_id, attr_num);
	for (int i = 0; i < ARRAY_LENGTH(voc_list); ++i)
	{
		ret += STRING_SPRINTF("<idx[%d] attr_type:%d attr_value:%d>", i, voc_list[i].attr_type,voc_list[i].attr_value);
	}

	ret += STRING_SPRINTF("<role: attr_type:%d attr_value:%d>", role_attr.attr_type, role_attr.attr_value);

	return ret;
}

std::string HolyEquipParam::AttrToString(const AttrInfo* attr_list, int attr_count)const
{
	std::string ret = STRING_SPRINTF("type:value");

	for (size_t i = 0; i < attr_count; i++)
	{
		ret += STRING_SPRINTF("[%d,%d]", (int)attr_list[i].attr_type, (int)attr_list[i].attr_value);
	}

	return ret;
}

std::string HolyEquipParam::ToString() const
{
	std::string ret = STRING_SPRINTF("HolyParam(has_net_value:%d) lv[%d] exp[%d] score[%d] MaxActive[%d] status[%d] extra_attr[%d]{%s}"
		" PetInfo[%d : %lld] total_attr[%s] base_attr[%s] UnsavedSkill[%d,%d] "
		"active_skills[%d, %d, %d, %d, %d] talent_skills[%d, %d] fushi_hole[%d, %d, %d, %d, %d]"
		" fushi_list{",
		has_net_value, level, exp, score, active_max, holy_status, valid_extra_attr_num, AttrToString(m_extra_attr_list, ARRAY_LENGTH(m_extra_attr_list)).c_str(),
		pet_id, pet_unique_id, AttrToString(total_attr_list, ARRAY_LENGTH(total_attr_list)).c_str(), AttrToString(base_attr_list, ARRAY_LENGTH(base_attr_list)).c_str(),
		unsaved_skill_id[0], unsaved_skill_id[1],
		active_skills[0], active_skills[1], active_skills[2], active_skills[3], active_skills[4],
		talent_skills[0], talent_skills[1],
		rune_stone_hole[0], rune_stone_hole[1], rune_stone_hole[2], rune_stone_hole[3], rune_stone_hole[4]	
		);

	for (size_t i = 0; i < ARRAY_LENGTH(rune_stone_list); i++)
	{
		const ItemBase * ib = ITEMPOOL->GetItem(rune_stone_list[i].item_id);
		if(NULL == ib) continue;
		ret += STRING_SPRINTF("hole[%d]:%s ", i, rune_stone_list[i].ToString().c_str());
	}

	ret += STRING_SPRINTF("}");

	return ret;
}

std::string FuYuParam::ToString() const 
{
	std::string ret = STRING_SPRINTF("FuYuParam[has_net_value:%d expired_timestamp:%ud sp_id:%d extra_sp_id:%d attr_num:%d] attrs:",
		has_net_value, expired_timestamp, sp_id, extra_sp_id, attr_num);
	for (int i = 0; i < ARRAY_LENGTH(base_attr_list); ++i)
	{
		ret += STRING_SPRINTF("index_%d:[attr_type:%d attr_value:%d] ", i, base_attr_list[i].attr_type, base_attr_list[i].attr_value);
	}

	return ret;
}
