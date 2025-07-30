#include "petconfig.hpp"
#include "item/itempool.h"
#include "item/itembase.h"
#include "checkresourcecenter.hpp"
#include "servercommon/attributedef.hpp"
#include "servercommon/struct/moneyotherdef.h"
#include "other/pet/petconfig.hpp"

PetConfig::PetConfig() : m_attr_dec_total_rate(0), m_max_attr_dec_value(0), m_max_extend_times(0)
{
	memset(m_extend_price_list, 0, sizeof(m_extend_price_list));
}

PetConfig::~PetConfig()
{
	std::vector<PetComposeCfg>().swap(m_pet_compose_cfg);
	for (std::map<MutantPetCfgKey, std::vector<int> >::iterator it = m_variant_pet_cfg.begin(); it != m_variant_pet_cfg.end(); it++)
	{
		std::vector<int>().swap(it->second);
	}
	std::vector<PetSkillAddCapabilityCfg>().swap(m_pet_skill_add_capability_cfg);
	std::vector<PetConvertCfg>().swap(m_pet_convert_cfg);

	for (GrowthVerCfgMap::iterator it = m_growth_version_map.begin(); it != m_growth_version_map.end(); it++)
	{
		GrowthVerCfgList().swap(it->second);
	}
	std::vector<UniqueUserID>().swap(m_check_param_user_list);
	std::vector<ItemID>().swap(m_pet_bulletin_list);
}

bool PetConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("pet_info_2", InitPetCfg2);
	LOAD_CONFIG("pet_levelup", InitPetLevelCfg);
	LOAD_CONFIG("pet_strengthen", InitPetStrengthenCfg);
	LOAD_CONFIG("attributes_exchange", InitAttrExchangeCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("pet_strengthen_buff", InitPetStrengthenBuffCfg);
	LOAD_CONFIG("pet_exp_pill", InitExpPillCfg);
	LOAD_CONFIG("attr_rate", InitAttrDecCfg);
	LOAD_CONFIG("slot_type", InitComprehendPassiveCfg);
	LOAD_CONFIG("pet_compose", InitPetComposeCfg);
	LOAD_CONFIG("pet_group", InitPetComposeRewardCfg);	
	LOAD_CONFIG("gift_group", InitPetRandPassiveGroupCfg);
	LOAD_CONFIG("pet_hitbook_score", InitPetSkillAddCapabilityCfg);
	LOAD_CONFIG("expand_price", InitPetExtendPriceCfg);
	LOAD_CONFIG("bookup_price", InitLockSkillCfg);
	LOAD_CONFIG("mythical_animals_convert", InitPetConvertCfg);
	LOAD_CONFIG("pet_growup_version", InitGrowthVersionCfg);
	LOAD_CONFIG("pet_strengthen_hearsay", InitStrengthenBroadcastCfg);
	LOAD_CONFIG("breach_attributes", InitBreakAttrCfg);
	LOAD_CONFIG("strengthen_protect", InitStrengthenProtectCfg);
	LOAD_CONFIG("strengthen_consume", InitPetStrengtheConsumeCfg);
	LOAD_CONFIG("special_pet_strengthen_consume", InitSpecifyPetStrengtheConsumeCfg);	
	LOAD_CONFIG("benediction_sp", InitPetBenedictionCfg);			//宠物祝福需要在宠物信息读取之后
	//LOAD_CONFIG("fs_att", InitFeiShengAttrCfg);   //需求54994 取消了每个任务阶段给属性
	LOAD_CONFIG("fly_att", InitFeiShengTimesAttrCfg);
	LOAD_CONFIG("skill_book_intensify", InitPetUpgradeSkillCfg);			
	LOAD_CONFIG("fs_task", InitPetFeiShengTaskCfg);
	LOAD_CONFIG("pet_fly", InitPetFeiShengConditionsCfg);
	LOAD_CONFIG("text_unlock", InitPetYiWenLuRewardsCfg);
	LOAD_CONFIG("fly_skill_strengthen", InitPetFeiShengSkillUpLevelCfg);
	LOAD_CONFIG("pet_param_check_data", InitPetParamCheckUserListCfg);
	LOAD_CONFIG("pet_bulletin", InitPetBulletinCfg);
	LOAD_CONFIG("pet_return", InitPetAbandonmentCfg);

	return true;
}

const PetCfg * PetConfig::GetPetCfg(int id)
{
	std::map<int, PetCfg>::const_iterator it = m_pet_map.find(id);
	if (it == m_pet_map.end())
	{
		return NULL;
	}
	return &it->second;
}

const PetComprehendPassiveCfg * PetConfig::GetPetComprehendPassiveCfg(int comprehend_id)
{
	std::map<int, PetComprehendPassiveCfg>::iterator iter = m_comprehend_passive_cfg.find(comprehend_id);
	if (iter == m_comprehend_passive_cfg.end())
	{
		return NULL;
	}

	return &iter->second;
}

const PetLevelCfg * PetConfig::GetLevelCfg(int level)
{
	std::map<int, PetLevelCfg>::const_iterator it = m_level_map.find(level);
	if (it != m_level_map.end()) return &it->second;
	return NULL;
}

const PetStrengthenCfg * PetConfig::GetPetStrengthenCfg(int strengthen_cfg_id, int level)
{
	std::map<int, std::vector<PetStrengthenCfg> > ::iterator iter=m_pet_strengthen_map.find(strengthen_cfg_id);
	if (iter == m_pet_strengthen_map.end()) return NULL;

	const std::vector<PetStrengthenCfg> & vec = iter->second;

	int index = level - 1;
	if (index < 0 || index >= (int)vec.size())
	{
		return NULL;
	}

	return &vec[index];
}

const PetStrengthenCfg * PetConfig::GetPetStrengthenCfgByPetId(int pet_id, int str_level)
{
	const PetCfg * cfg = this->GetPetCfg(pet_id);
	if (NULL == cfg) return NULL;

	return this->GetPetStrengthenCfg(cfg->strengthen_cfg_id, str_level);
}

const PetStrengthenBuffCfg * PetConfig::GetPetStrengthenBuffCfg(int pet_quality)
{
	return MapValuesConstPtr(m_pet_strengthen_buff_map, pet_quality);;
}

const PetStrengthenBuffItemCfg * PetConfig::GetPetStrengthenBuffCfg(int pet_quality,int level)
{
	const PetStrengthenBuffCfg * cfg = this->GetPetStrengthenBuffCfg(pet_quality);
	if (!cfg)
	{
		return NULL;
	}

	return MapValuesConstPtr(cfg->item_cfg_list, level);
}

int PetConfig::GetStrRate(int strengthen_cfg_id, int main_level, int consume_level)
{
	std::map<int, std::vector<PetStrengthenCfg> > ::iterator iter = m_pet_strengthen_map.find(strengthen_cfg_id);
	if (iter == m_pet_strengthen_map.end()) return 0;

	const std::vector<PetStrengthenCfg> & vec = iter->second;

	int index = main_level - 1;
	if (index < 0 || index >= (int)vec.size())
	{
		return 0;
	}

	int index2 = consume_level - 1;
	if (index2 < 0 || index2 >= ARRAY_LENGTH(vec[index].add_rate))
	{
		return 0;
	}

	return vec[index].rate + vec[index].add_rate[consume_level - 1];
}

const PetAttrExchangeCfg::ItemCfg* PetConfig::GetAttrExchange(int role_pet_train_skill_level, int attr_id)
{
	std::map<int, PetAttrExchangeCfg>::iterator iter = m_pet_attr_exchange_map.find(role_pet_train_skill_level);
	if (iter != m_pet_attr_exchange_map.end())
	{
		if (iter->second.attr_cfg.find(attr_id) != iter->second.attr_cfg.end())
		{
			return &iter->second.attr_cfg[attr_id];
		}
		
	}
	return NULL;
}

const PetExpPillCfg * PetConfig::GetExpPillCfg(int item_id)
{
	std::map<int, PetExpPillCfg>::const_iterator it = m_pet_exp_pill_map.find(item_id);
	if (it == m_pet_exp_pill_map.end()) return NULL;
	return &it->second;
}

int PetConfig::RandAttrDecValue()
{
	int rand_num = RandomNum(m_attr_dec_total_rate);

	for (std::map<int, int>::const_iterator it = m_addr_dec_map.begin(); it != m_addr_dec_map.end(); ++it)
	{
		if (rand_num < it->second)
		{
			return it->first;
		}
		rand_num -= it->second;
	}
	return 0;
}

std::vector<ItemConfigData> PetConfig::PutEXPtoConversionPetEXPPill(long long exp)
{
	std::vector<ItemConfigData> ret;

	// 检查经验药换算--从最高经验药开始判断
	for (std::map<int, int>::reverse_iterator it = m_pet_exptopill_map.rbegin(); it != m_pet_exptopill_map.rend(); it++)
	{
		ItemConfigData temp;
		temp.is_bind = true;
		temp.item_id = it->second;
		temp.num = (int)(exp / it->first);
		exp = exp % it->first;
		
		if (temp.num > 0)
		{
			ret.push_back(temp);
		}
	}

	if (exp > 0 || ret.empty())
	{
		std::map<int, int>::iterator it_first = m_pet_exptopill_map.begin();
		if (it_first != m_pet_exptopill_map.end())
		{
			bool is_get = false;
			for (int i = 0; i < (int)ret.size(); ++i)
			{
				if (ret[i].item_id == it_first->second)
				{
					is_get = true;
					ret[i].num += 1;
					break;
				}
			}

			if (!is_get)
			{
				ItemConfigData temp;
				temp.is_bind = true;
				temp.item_id = it_first->second;
				temp.num = 1;
				ret.push_back(temp);
			}
		}
	}

	return ret;
}

std::vector<int> PetConfig::GetRandPassive(const std::vector<int> & passive_group_id_list)
{
	std::set<int> temp;

	for (int i = 0; i < (int)passive_group_id_list.size(); ++i)
	{
		std::map<int, PetRandPassiveGroupCfg>::iterator iter = m_pet_passive_group_cfg.find(passive_group_id_list[i]);
		if (iter == m_pet_passive_group_cfg.end())
		{
			continue;
		}

		int r = RandomNum(PetRandPassiveGroupCfg::MAX_RATE_COUNT);

		const std::vector<PetRandPassiveGroupCfg::ItemCfg> & passive_group = iter->second.passive_group;
		for (int k = 0; k < (int)passive_group.size(); ++k)
		{
			if (r < passive_group[k].rate)
			{
				temp.insert(passive_group[k].passive_skill_id);
				break;
			}

			r -= passive_group[k].rate;
		}
	}

	return std::vector<int>(temp.begin(), temp.end());
}

std::set<int> PetConfig::GetPossiblePassiveList(const std::vector<int> & passive_group_id_list, const std::vector<int> & baodi_group_id_list)
{
	std::set<int> temp;

	for (int i = 0; i < (int)passive_group_id_list.size(); ++i)
	{
		std::map<int, PetRandPassiveGroupCfg>::iterator iter = m_pet_passive_group_cfg.find(passive_group_id_list[i]);
		if (iter == m_pet_passive_group_cfg.end())
		{
			continue;
		}

		const std::vector<PetRandPassiveGroupCfg::ItemCfg> & passive_group = iter->second.passive_group;
		for (int k = 0; k < (int)passive_group.size(); ++k)
		{
			temp.insert(passive_group[k].passive_skill_id);
		}
	}

	// 保底
	for (int i = 0; i < (int)baodi_group_id_list.size(); ++i)
	{
		std::map<int, PetRandPassiveGroupCfg>::iterator iter = m_pet_passive_group_cfg.find(baodi_group_id_list[i]);
		if (iter == m_pet_passive_group_cfg.end())
		{
			continue;
		}

		const std::vector<PetRandPassiveGroupCfg::ItemCfg> & passive_group = iter->second.passive_group;
		for (int k = 0; k < (int)passive_group.size(); ++k)
		{
			temp.insert(passive_group[k].passive_skill_id);
		}
	}

	return temp;
}

int PetConfig::GetOneRandPassive(const std::vector<int>& passive_list)
{
	if (passive_list.empty())
	{
		return 0;
	}

	int r = rand() % (int)passive_list.size();

	return passive_list[r];
}

const PetComposeCfg * PetConfig::GetPetComposeCfg(int index)
{
	if (index < 0 || index >= (int)m_pet_compose_cfg.size())
	{
		return NULL;
	}

	return &m_pet_compose_cfg[index];
}

int PetConfig::GetPetGroupIdByItemId(int item_id)
{
	std::map<int, int>::iterator it = m_pet_compose_cfg_by_item_id.find(item_id);
	if(it == m_pet_compose_cfg_by_item_id.end()) return 0;

	return it->second;
}

int PetConfig::GetPetComposeRandPetId(int compose_reward_id)
{
	std::map<int, PetComposeRewardCfg>::iterator iter = m_pet_compose_reward_cfg.find(compose_reward_id);
	if (iter == m_pet_compose_reward_cfg.end())
	{
		return 0;
	}

	const std::vector<PetComposeRewardCfg::ItemCfg> & reward_gruop = iter->second.group_reward;
	int r = RandomNum(PetComposeRewardCfg::MAX_RATE_COUNT);
	
	for (int i = 0; i < (int)reward_gruop.size(); ++i)
	{
		if (r < reward_gruop[i].rate)
		{
			return  reward_gruop[i].pet_id;
		}

		r -= reward_gruop[i].rate;
	}

	return 0;
}

bool PetConfig::GetSealPetReward(int ARG_IN pet_id, ItemConfigData ARG_OUT & reward_item, bool ARG_OUT  * is_get_pet)
{
	const PetCfg *cfg = this->GetPetCfg(pet_id);
	if (NULL == cfg) return false;

	int r = RandomNum(cfg->catch_reward_rate_count);
	
	bool is_get_pet_tmp = false;
	bool is_get_tmp = false;

	if (!is_get_tmp && r < cfg->catch_reward_rate_shards)
	{
		reward_item.item_id = cfg->shards_item_id;				//给碎片	
		reward_item.num = cfg->catch_pet_get_shards_num;
		reward_item.is_bind = true;

		is_get_tmp = true;
	}

	if (!is_get_tmp && r < cfg->catch_reward_rate_variant_pet + cfg->catch_reward_rate_shards)
	{
		int mutant_pet_id = this->GetVariantPetByPetId(pet_id);
		const PetCfg * mutant_pet_cfg = this->GetPetCfg(mutant_pet_id);
		if (mutant_pet_cfg)
		{
			reward_item.item_id = mutant_pet_id;
			reward_item.num = 1;
			reward_item.is_bind = false;

			is_get_tmp = true;
			is_get_pet_tmp = true;
		}
	} 

	if (!is_get_tmp &&  r < cfg->catch_reward_rate_pet + cfg->catch_reward_rate_variant_pet + cfg->catch_reward_rate_shards)
	{	
		reward_item.item_id = pet_id;				//给正常宠物
		reward_item.num = 1;
		reward_item.is_bind = false;

		is_get_tmp = true;
		is_get_pet_tmp = true;
	}

	if (is_get_pet != NULL)
	{
		*is_get_pet = is_get_pet_tmp;
	}

	return true;
}

int PetConfig::GetVariantPetByPetId(int pet_id)
{
	const PetCfg * pet_cfg = this->GetPetCfg(pet_id);
	if (!pet_cfg)
	{
		return 0;
	}

	return this->GetVariantPet(pet_cfg->mutant_id, pet_cfg->pet_mutant_times + 1);
}

int PetConfig::GetVariantPet(int mutant_id,int mutant_times)
{
	MutantPetCfgKey key(mutant_id, mutant_times);
	std::map<MutantPetCfgKey, std::vector<int> >::iterator iter = m_variant_pet_cfg.find(key);
	if (iter != m_variant_pet_cfg.end() && !iter->second.empty())
	{
		int r = RandomNum((int)iter->second.size());
		return iter->second[r];
	}

	return 0;
}

const PetSkillAddCapabilityCfg * PetConfig::GetPetSkillAddCapabilityCfg(int pet_level)
{
	int index = pet_level - 1;

	if (index < 0 || index >= (int)m_pet_skill_add_capability_cfg.size())
	{
		return NULL;
	}
	
	return &m_pet_skill_add_capability_cfg[index];
}

bool PetConfig::GetExtendPrice(int times, ARG_OUT int* price)
{
	if (NULL == price) return false;
	if (times <= 0 || times > m_max_extend_times) return false;

	*price = m_extend_price_list[times];
	return true;
}

bool PetConfig::RandPetParam(int pet_id, int str_level, bool is_max, NetValueItemParam& param)
{
	const PetCfg *cfg = this->GetPetCfg(pet_id);
	if (NULL == cfg) return false;

	param.Reset();
	param.pet_param.pet_id = cfg->id;
	memcpy(param.pet_param.name, cfg->name, sizeof(GameName));
	param.pet_param.str_level = GetMax(str_level, 1);
	param.pet_param.level = 1;
	param.pet_param.exp = 0;
	param.pet_param.hp = 0;
	param.pet_param.mp = 0;

	std::vector<int> get_passive_list = this->GetRandPassive(cfg->passive_group_id_vec);
	if (get_passive_list.empty())
	{
		get_passive_list = this->GetRandPassive(cfg->passive_group_id_lowest_vec);
	}

	param.pet_param.ClearGift();
	for (int i = 0; i < (int)get_passive_list.size() && i < ARRAY_LENGTH(param.pet_param.passive_list); ++i)
	{
		param.pet_param.passive_list[i].passive_skill_id = get_passive_list[i];
	}

	param.pet_param.lock = 0;

	{// 随机资质
		if (cfg->quality == PET_QUALITY_GOD)
		{
			is_max = true;			//神兽满资质
		}

		int rand_num = 0;
		for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
		{
			if (!is_max)
			{
				rand_num = this->RandAttrDecValue();
			}
			param.pet_param.growth[i] = (unsigned char)(cfg->growth[i] - rand_num);
		}
	}

	{// 已分配的属性点
		memset(param.pet_param.attr, 0, sizeof(param.pet_param.attr));
	}

	param.pet_param.pet_growth_version = this->GetPetCurGrowthVersion(pet_id);

	return true;
}

bool PetConfig::RandPetGrowth(int pet_id,bool is_max, NetValueItemParam & param)
{
	const PetCfg * cfg = this->GetPetCfg(pet_id);
	if (NULL == cfg) return false;

	{// 随机资质
		if (cfg->quality == PET_QUALITY_GOD)
		{
			is_max = true;			//神兽满资质
		}

		int rand_num = 0;
		for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
		{
			if (!is_max)
			{
				rand_num = this->RandAttrDecValue();
			}
			param.pet_param.growth[i] = (unsigned char)(cfg->growth[i] - rand_num);
		}
	}

	return true;
}

const PetLockSkillCfg * PetConfig::GetPetLockSkillCfg(int pet_pin_zhi)
{
	std::map<int, PetLockSkillCfg>::iterator iter = m_lock_skill_cfg.find(pet_pin_zhi);
	if (iter != m_lock_skill_cfg.end())
	{
		return &iter->second;
	}
	return NULL;
}

const PetConvertCfg * PetConfig::GetPetConvertCfg(int seq)
{
	if(seq < 0 || seq >= (int)m_pet_convert_cfg.size()) return NULL;

	return &m_pet_convert_cfg[seq];
}

const PetGrowthVersionCfg* PetConfig::GetGrowthVersion(int pet_id, int version)
{
	GrowthVerCfgMap::iterator it = m_growth_version_map.find(pet_id);
	if (it == m_growth_version_map.end())
	{
		return NULL;
	}

	GrowthVerCfgList& version_list = it->second;
	for (size_t i = 0; i < version_list.size(); ++i)
	{
		if (version == version_list[i].version)
		{
			return &version_list[i];
		}
	}

	return NULL;
}

int PetConfig::GetPetCurGrowthVersion(int pet_id)
{
	GrowthVerCfgMap::iterator it = m_growth_version_map.find(pet_id);
	if (it == m_growth_version_map.end())
	{
		return 0;
	}

	int max_version = 0;
	GrowthVerCfgList& version_list = it->second;
	for (size_t i = 0; i < version_list.size(); ++i)
	{
		if (version_list[i].version > max_version)
		{
			max_version = version_list[i].version;
		}
	}

	return max_version + 1; // 因为该列表中的都是旧的资质版本，所以要+1
}

bool PetConfig::IsStrengthenBroadcast(int pet_id, int str_lv)
{
	std::map<int, int>::iterator iter = m_strengthen_broadcast_cfg.find(pet_id);
	if (iter != m_strengthen_broadcast_cfg.end() && str_lv >= iter->second)
	{
		return true;
	}

	return false;
}

const PetBreakAttrCfg * PetConfig::GetPetBreakAttrCfg(int pet_lv, bool is_break)
{
	const PetLevelCfg * level_cfg = this->GetLevelCfg(pet_lv);
	if (level_cfg == NULL)
	{
		return NULL;
	}

	int break_attr_id = is_break ? level_cfg->break_attr_id : level_cfg->last_break_attr_id;

	std::map<int, PetBreakAttrCfg>::const_iterator iter = m_break_attr_cfg.find(break_attr_id);
	if (iter == m_break_attr_cfg.end())
	{
		return NULL;
	}

	return &iter->second;
}

const PetStrengthenProtectCfg * PetConfig::GetPetStrengthenProtectCfg(int pet_quality, int pet_strengthen_level)
{
	std::map<int, std::map<int, PetStrengthenProtectCfg> >  ::const_iterator iter = m_strengthen_protect_cfg.find(pet_quality);
	if (iter == m_strengthen_protect_cfg.end())
	{
		return NULL;
	}

	std::map<int, PetStrengthenProtectCfg>::const_iterator iter2 = iter->second.find(pet_strengthen_level);
	if (iter2 == iter->second.end())
	{
		return NULL;
	}

	//当宠物品质和强化等级，对应的消耗道具为0时，代表无法使用“宠物强化保护券”
	if (iter2->second.protect_item_num <= 0)
	{
		return NULL;
	}

	return &iter2->second;
}

const PetStrengtheConsumeCfg * PetConfig::GetPetStrengtheConsumeCfg(int pet_quality, int pet_strengthen_level)
{
	PetStrengtheConsumeCfgKey key;
	key.pet_quality = pet_quality;
	key.pet_strengthen_level = pet_strengthen_level;

	return MapValuesConstPtr(m_strengthen_consume_cfg, key);
}

const PetStrengtheConsumeCfg * PetConfig::GetSpecifyPetStrengtheConsumeCfg(int prototype_id, int pet_strengthen_level)
{
	SpecifyPetStrengtheConsumeCfgKey key;
	key.prototype_id = prototype_id;
	key.pet_strengthen_level = pet_strengthen_level;

	return MapValuesConstPtr(m_specify_strengthen_consume_cfg, key);
}

bool PetConfig::IsHasBenediction(int benediction_id)
{
	std::map<int, PetBenedictionSimpleCfg>::iterator it = m_benediction_simple_map.find(benediction_id);
	if (it == m_benediction_simple_map.end()) return false;

	return true;
}

bool PetConfig::IsMeetActiveCond(int benediction_id, PetBenedictionSimpleCfg simple_info)
{
	std::map<int, PetBenedictionSimpleCfg>::iterator it = m_benediction_simple_map.find(benediction_id);
	if(it == m_benediction_simple_map.end() || it->second.active_set != simple_info.active_set) return false;

	return true;
}

const PetBenedictionDetailCfg * PetConfig::GetBenedictionDetailCfg(int benediction_id, int score)
{
	std::map<int, std::map<int, PetBenedictionDetailCfg> >::iterator benediction_it = m_benediction_detail_map.find(benediction_id);
	if(benediction_it == m_benediction_detail_map.end() || benediction_it->second.empty()) return NULL;

	for (std::map<int, PetBenedictionDetailCfg>::reverse_iterator detail_it = benediction_it->second.rbegin(); detail_it != benediction_it->second.rend(); ++detail_it)
	{
		if (score >= detail_it->second.activate_score)
		{
			return &detail_it->second;
		}
	}

	return NULL;
}

int PetConfig::GetPrototypePetId(int pet_id)
{
	const PetCfg* cfg = this->GetPetCfg(pet_id);
	if (NULL == cfg) return pet_id;

	return cfg->prototype_pet_id;
}

bool PetConfig::IsSamePrototypePet(int pet_id_1, int pet_id_2)
{
	if (pet_id_1 == pet_id_2) return true;

	return this->GetPrototypePetId(pet_id_1) == this->GetPrototypePetId(pet_id_2);
}

bool PetConfig::IsNormalAttackPet(int pet_id)
{
	const PetCfg* cfg = this->GetPetCfg(pet_id);
	if (NULL == cfg) return false;

	return cfg->is_normal_attack_pet;
}

const PetFeiShengAttrCfg * PetConfig::GetPetFeiShengAttrCfg(int pet_id, int fei_sheng_task_jie_duan)
{
	PetFeiShengAttrCfgKey key(pet_id, fei_sheng_task_jie_duan);
	return MapValuesConstPtr(m_fei_sheng_attr_cfg, key);
}

const PetFeiShengTimesAttrCfg * PetConfig::GetPetFeiShengTimesAttrCfg(int pet_id, int fei_sheng_times)
{
	PetFeiShengTimesAttrCfgKey key(pet_id, fei_sheng_times);
	return MapValuesConstPtr(m_fei_sheng_times_attr_cfg, key);
}

int PetConfig::GetFeiShengShuXingDanUseTimes(int fei_sheng_times)
{
	int index = fei_sheng_times;
	if (index < 0 || index >= ARRAY_LENGTH(m_other_cfg.fei_sheng_shu_xing_dan_use_times))
	{
		return 0;
	}

	return m_other_cfg.fei_sheng_shu_xing_dan_use_times[index];
}

const PetSkillUpgradeCfg * PetConfig::GetPetSkillUpgradeCfg(int skill_id, int skill_level)
{
	PetSkillUpgradeCfgKey key(skill_id, skill_level);

	return MapValuesConstPtr(m_pet_skill_upgrade_cfg, key);
}

int PetConfig::GetPetFeiShengFirstTask(int pet_id, int fei_sheng_task_jie_duan)
{
	const PetFeiShengTaskCfg * cfg = MapValuesConstPtr(m_fei_sheng_task_cfg, pet_id);
	if (cfg == NULL)
	{
		return 0;
	}

	if (fei_sheng_task_jie_duan < 0 || fei_sheng_task_jie_duan >= (int)cfg->task_cfg.size())
	{
		return 0;
	}

	return cfg->task_cfg[fei_sheng_task_jie_duan].start_task_id;
}

const PetFeiShengConditionsCfg * PetConfig::GetPetFeiShengConditionsCfg(int fei_sheng_times)
{
	return MapValuesConstPtr(m_fei_sheng_conditions_cfg, fei_sheng_times);
}

const PetYiWenLuRewardCfg * PetConfig::GetPetYiWenLuRewardCfg(int mutant_id, int reward_type)
{
	const PetYiWenLuCfg * cfg = this->GetPetYiWenLuCfg(mutant_id);
	if (!cfg)
	{
		return NULL;
	}

	return MapValuesConstPtr(cfg->rewards_cfg, reward_type);
}

const PetYiWenLuCfg * PetConfig::GetPetYiWenLuCfg(int mutant_id)
{
	return MapValuesConstPtr(m_yi_wen_lu_rewards_cfg, mutant_id);;
}

const PetFeiShengSkillUpgradeCfg * PetConfig::GetPetFeiShengSkillUpgradeCfg(int skill_id, int skill_level)
{
	PetFeiShengSkillUpgradeCfgKey key;
	key.skill_id = skill_id;
	key.level = skill_level;
	return MapValuesConstPtr(m_fei_sheng_skill_up_lv_cfg, key);
}

const ItemID PetConfig::RandPetId() const
{
	std::vector<ItemID> temp_list = m_pet_bulletin_list;
	RandomShuffle(&temp_list[0], temp_list.size());
	return temp_list[0];
}

bool PetConfig::IsInCheckParamUserList(int plat_type, int role_id)
{
	UniqueUserID uuid(plat_type, role_id);
	for (size_t i = 0; i < m_check_param_user_list.size(); ++i)
	{
		if (uuid == m_check_param_user_list[i])
		{
			return true;
		}
	}
	return false;
}

bool PetConfig::IsPetHasExclusiveSkill(int pet_id, int pet_str_level, int skill_id, int skill_level)
{
	const PetCfg* pet_cfg = this->GetPetCfg(pet_id);
	if (NULL == pet_cfg)
	{
		return false;
	}

	bool is_found = false;
	for (size_t i = 0; i < pet_cfg->exclusive_skills.size(); ++i)
	{
		if (pet_cfg->exclusive_skills[i].skill_id != skill_id)
		{
			continue;
		}
		int max_pet_skill_level = pet_cfg->exclusive_skills[i].skill_level;
		const PetStrengthenCfg* psc = this->GetPetStrengthenCfgByPetId(pet_id, pet_str_level);
		if (NULL != psc)
		{
			if (0 == i) max_pet_skill_level = psc->exclusive_skill_level;
			if (1 == i) max_pet_skill_level = psc->exclusive_skill_2_level;
		}
		if (skill_level > max_pet_skill_level)
		{
			return false;
		}

		is_found = true;
		break;
	}

	return is_found;
}

const PetAbandonmentCfg * PetConfig::GetPetAbandonmentCfg(int abandonment_id, int strengthen_level)
{
	PetAbandonmentCfgKey key;
	key.abandonment_id = abandonment_id;
	key.strengthen_level = strengthen_level;

	return MapValuesConstPtr(m_abandonment_cfg, key);
}

int PetConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		if (!GetSubNodeValue(root_element, "respec_item", m_other_cfg.respec_item))return -10;
		CheckResourceCenter::Instance().GetItemValidCheck()->Add(m_other_cfg.respec_item, m_other_cfg.respec_item, 1, __FUNCTION__);
		if (!GetSubNodeValue(root_element, "max_pet_bag", m_other_cfg.max_pet_bag) || m_other_cfg.max_pet_bag <= 0 || m_other_cfg.max_pet_bag > ItemNamespace::MAX_PET_GRID_NUM)return -1;
		if (!GetSubNodeValue(root_element, "start_num", m_other_cfg.start_pet_bag) || m_other_cfg.start_pet_bag <= 0 || m_other_cfg.start_pet_bag > m_other_cfg.max_pet_bag) return -111;
		if (!GetSubNodeValue(root_element, "add_num", m_other_cfg.extend_add_num) || m_other_cfg.extend_add_num <= 0) return -112;
		if (!GetSubNodeValue(root_element, "max_pet_on", m_other_cfg.max_pet_on) || m_other_cfg.max_pet_on <= 0)return -2;
		if (!GetSubNodeValue(root_element, "min_return_exp", m_other_cfg.min_return_exp) || m_other_cfg.min_return_exp <= 0)return -3;
		if (!GetSubNodeValue(root_element, "rate_return_exp", m_other_cfg.rate_return_exp) || m_other_cfg.rate_return_exp <= 0)return -4;
		if (!GetSubNodeValue(root_element, "item_return_exp", m_other_cfg.return_id))return -17;
		CheckResourceCenter::Instance().GetItemValidCheck()->Add(m_other_cfg.return_id, m_other_cfg.return_id, 1, __FUNCTION__);
		if (!GetSubNodeValue(root_element, "relife_need", m_other_cfg.relife_need))return -5;
		CheckResourceCenter::Instance().GetItemValidCheck()->Add(m_other_cfg.relife_need, m_other_cfg.relife_need, 1, __FUNCTION__);
		if (!GetSubNodeValue(root_element, "att_point", m_other_cfg.att_point_level) || m_other_cfg.att_point_level <= 0)return -6;
		if (!GetSubNodeValue(root_element, "start_att_point", m_other_cfg.attr_point_init) || m_other_cfg.attr_point_init <= 0)return -66;
		if (!GetSubNodeValue(root_element, "pet_rename_need", m_other_cfg.pet_rename_need) || m_other_cfg.pet_rename_need <= 0)return -7;
		if (!GetSubNodeValue(root_element, "exchange_strengthen_need", m_other_cfg.exchange_strengthen_need))return -8;
		CheckResourceCenter::Instance().GetItemValidCheck()->Add(m_other_cfg.exchange_strengthen_need, m_other_cfg.exchange_strengthen_need, 1, __FUNCTION__);

		if (!GetSubNodeValue(root_element, "normal_sealcard", m_other_cfg.common_sealcard) || NULL == ITEMPOOL->GetItem(m_other_cfg.common_sealcard))
		{
			return -9;
		}

		if (!GetSubNodeValue(root_element, "silver_sealcard", m_other_cfg.silver_sealcard) || NULL == ITEMPOOL->GetItem(m_other_cfg.silver_sealcard))
		{
			return -10;
		}

		if (!GetSubNodeValue(root_element, "gold_sealcard", m_other_cfg.gold_sealcard) || NULL == ITEMPOOL->GetItem(m_other_cfg.gold_sealcard))
		{
			return -11;
		}

		if (!GetSubNodeValue(root_element, "fragment_item_id", m_other_cfg.common_shards_item_id) || NULL == ITEMPOOL->GetItem(m_other_cfg.common_shards_item_id))
		{
			return -12;
		}

		if (!GetSubNodeValue(root_element, "refresh_mutant_rate", m_other_cfg.refined_mutant_rate))
		{
			return -13;
		}

		if (!GetSubNodeValue(root_element, "skill_stage_1", m_other_cfg.skill_stage_1) || m_other_cfg.skill_stage_1 < 0 || m_other_cfg.skill_stage_1 >= PET_PASSIVE_MAX_COUNT)
		{
			return -14;
		}

		if (!GetSubNodeValue(root_element, "skill_stage_2", m_other_cfg.skill_stage_2) || m_other_cfg.skill_stage_2 < 0 || m_other_cfg.skill_stage_2 >= PET_PASSIVE_MAX_COUNT)
		{
			return -15;
		}

		if (m_other_cfg.skill_stage_1 > m_other_cfg.skill_stage_2)
		{
			return -16;
		}

		if (!GetSubNodeValue(root_element, "auto_pet_levels", m_other_cfg.auto_pet_levels))
		{
			return -17;
		}

		if (!GetSubNodeValue(root_element, "reset_price", m_other_cfg.reset_level_need_coint))
		{
			return -18;
		}

		if (!GetSubNodeValue(root_element, "rare_fragment_item_id", m_other_cfg.rare_fragment_item_id) || NULL == ITEMPOOL->GetItem(m_other_cfg.rare_fragment_item_id))
		{
			return -19;
		}

		if (!GetSubNodeValue(root_element, "first_change_min", m_other_cfg.first_change_min) || m_other_cfg.first_change_min < 0)
		{
			return -20;
		}

		if (!GetSubNodeValue(root_element, "first_change_max", m_other_cfg.first_change_max) || m_other_cfg.first_change_max < 0 || m_other_cfg.first_change_max < m_other_cfg.first_change_min)
		{
			return -21;
		}

		if (!GetSubNodeValue(root_element, "first_change_value", m_other_cfg.first_change_mutant) || m_other_cfg.first_change_mutant <= 0 || m_other_cfg.first_change_mutant > 10000)
		{
			return -22;
		}

		for (int i = 0; i < ARRAY_LENGTH(m_other_cfg.fei_sheng_skill_id); ++i)
		{
			std::string s = STRING_SPRINTF("fly_skill_id%d", i + 1);
			if (!GetSubNodeValue(root_element, s, m_other_cfg.fei_sheng_skill_id[i]) || m_other_cfg.fei_sheng_skill_id[i] <= 0)
			{
				return -23;
			}
		}

		if (!GetSubNodeValue(root_element, "change_expend_coin", m_other_cfg.fei_sheng_skill_change_need_coin))
		{
			return -24;
		}

		for (int i = 0; i < ARRAY_LENGTH(m_other_cfg.fei_sheng_shu_xing_dan_use_times); i++)
		{
			std::string s = STRING_SPRINTF("pellet_fs%d", i);
			if (!GetSubNodeValue(root_element, s, m_other_cfg.fei_sheng_shu_xing_dan_use_times[i]) || m_other_cfg.fei_sheng_shu_xing_dan_use_times[i] < 0)
			{
				return -25;
			}
		}

		if (!GetSubNodeValue(root_element, "interval", m_other_cfg.broadcast_interval) || m_other_cfg.broadcast_interval < 0) return -26;
		if (!GetSubNodeValue(root_element, "time", m_other_cfg.start_broadcast_time) || m_other_cfg.start_broadcast_time < 0) return -27;
		if (!GetSubNodeValue(root_element, "world_rank", m_other_cfg.world_rank) || m_other_cfg.world_rank < 0) return -28;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitPetLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	int next_level = 1;
	int last_break_attr_id = 0;
	while (NULL != root_element)
	{
		PetLevelCfg cfg;
		if (!GetSubNodeValue(root_element, "level", cfg.level) || cfg.level != next_level)	return -1;
		next_level++;
		if (!GetSubNodeValue(root_element, "exp", cfg.exp) || cfg.exp < 0)	return -2;
		if (!GetSubNodeValue(root_element, "is_need_breach", cfg.is_need_breach)) return -3;
		if (!GetSubNodeValue(root_element, "breach_level", cfg.break_attr_id)) return -4;

		cfg.last_break_attr_id = last_break_attr_id;
		last_break_attr_id = cfg.break_attr_id;

		if (cfg.is_need_breach)
		{
			if (!GetSubNodeValue(root_element, "breach_consume", cfg.break_consume_id) || NULL == ITEMPOOL->GetItem(cfg.break_consume_id)) return -4;
			if (!GetSubNodeValue(root_element, "breach_consume_num", cfg.break_consume_num)) return -4;

		}

		m_level_map[cfg.level] = cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetConfig::InitPetStrengthenCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -__LINE__;

	while (NULL != root_element)
	{
		int strengthen_id = 0;
		if (!GetSubNodeValue(root_element, "strengthen_id", strengthen_id))	return -__LINE__;
		std::vector<PetStrengthenCfg> & node_cfg_vec = m_pet_strengthen_map[strengthen_id];


		PetStrengthenCfg node_cfg;

		int level = 0;
		if (!GetSubNodeValue(root_element, "level", level) || level != (int)node_cfg_vec.size() + 1)	return -__LINE__;
		node_cfg.level = level;
		if (!GetSubNodeValue(root_element, "rate", node_cfg.rate) || node_cfg.rate < 0)	return -__LINE__;

		if (!GetSubNodeValue(root_element, "add_con", node_cfg.add_growth[ADD_POINT_TYPE_VITALITY]) || node_cfg.add_growth[ADD_POINT_TYPE_VITALITY] < 0)	return -__LINE__;
		if (!GetSubNodeValue(root_element, "add_str", node_cfg.add_growth[ADD_POINT_TYPE_STRENGTH]) || node_cfg.add_growth[ADD_POINT_TYPE_STRENGTH] < 0)	return -__LINE__;
		if (!GetSubNodeValue(root_element, "add_def", node_cfg.add_growth[ADD_POINT_TYPE_POWER]) || node_cfg.add_growth[ADD_POINT_TYPE_POWER] < 0)	return -__LINE__;
		if (!GetSubNodeValue(root_element, "add_agi", node_cfg.add_growth[ADD_POINT_TYPE_AGILITY]) || node_cfg.add_growth[ADD_POINT_TYPE_AGILITY] < 0)	return -__LINE__;
		if (!GetSubNodeValue(root_element, "add_magic", node_cfg.add_growth[ADD_POINT_TYPE_MAGIC]) || node_cfg.add_growth[ADD_POINT_TYPE_MAGIC] < 0)	return -__LINE__;
		if (!GetSubNodeValue(root_element, "bind_coin_consume", node_cfg.cost_coin) || node_cfg.cost_coin < 0)	return -__LINE__;
		if (!GetSubNodeValue(root_element, "maxhp", node_cfg.maxhp))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "maxmp", node_cfg.maxmp))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "gongji", node_cfg.gongji))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "fangyu", node_cfg.fangyu))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "minjie", node_cfg.minjie))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "jingshen", node_cfg.jingshen))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "huifu", node_cfg.huifu))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "skill_level_1", node_cfg.exclusive_skill_level))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "skill_level_2", node_cfg.exclusive_skill_2_level))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "fei_sheng_times", node_cfg.need_fei_sheng_times))	return -__LINE__;
		
		node_cfg_vec.push_back(node_cfg);

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetConfig::InitAttrExchangeCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int role_pet_train_skill_level = 0;
		if (!GetSubNodeValue(root_element, "skill_level", role_pet_train_skill_level))
		{
			return -1999;
		}
		PetAttrExchangeCfg& cfg = m_pet_attr_exchange_map[role_pet_train_skill_level];

		int id = 0;
		if (!GetSubNodeValue(root_element, "id", id) || id < ADD_POINT_TYPE_MIN || id >= ADD_POINT_TYPE_MAX)
		{
			return -1;
		}

		PetAttrExchangeCfg::ItemCfg& node_cfg = cfg.attr_cfg[id];
		node_cfg.attr_id = id;

		if (!GetSubNodeValue(root_element, "maxhp", node_cfg.maxhp))	return -1;
		if (!GetSubNodeValue(root_element, "maxmp", node_cfg.maxmp))	return -2;
		if (!GetSubNodeValue(root_element, "gongji", node_cfg.gongji))	return -3;
		if (!GetSubNodeValue(root_element, "fangyu", node_cfg.fangyu))	return -4;
		if (!GetSubNodeValue(root_element, "minjie", node_cfg.minjie))	return -5;
		if (!GetSubNodeValue(root_element, "jingshen", node_cfg.jingshen))	return -6;
		if (!GetSubNodeValue(root_element, "huifu", node_cfg.huifu))	return -7;

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetConfig::InitPetStrengthenBuffCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)return -999;

	while (NULL != data_element)
	{
		int pet_quality = 0;
		if (!GetSubNodeValue(data_element, "pets_quality", pet_quality))
		{
			return -__LINE__;
		}

		PetStrengthenBuffCfg & cfg = m_pet_strengthen_buff_map[pet_quality];
		cfg.pet_quality = pet_quality;

		if (!GetSubNodeValue(data_element, "index", cfg.pet_strengthen_buff_type))
		{
			return -__LINE__;
		}


		PetStrengthenBuffItemCfg item_cfg;
		item_cfg.pet_quality = pet_quality;
		item_cfg.pet_strengthen_buff_type = cfg.pet_strengthen_buff_type;

		if (!GetSubNodeValue(data_element, "strengthening_level", item_cfg.strengthen_buff_level))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "unlock_number", item_cfg.grid_num))
		{
			return -__LINE__;
		}

		for (int i = 0; i < 7; ++i)
		{
			AttrItemCfg attr_cfg;

			std::string type = STRING_SPRINTF("attr_type_%d", i + 1);
			std::string value = STRING_SPRINTF("attr_add_%d", i + 1);

			if (!GetSubNodeValue(data_element, type, attr_cfg.attr_type))
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(data_element, value, attr_cfg.attr_value))
			{
				return -__LINE__;
			}

			if (attr_cfg.attr_value == 0)
			{
				continue;
			}

			item_cfg.attr_cfg.push_back(attr_cfg);
		}

		if (ReadCfgListValue(data_element, "sp_id_add", item_cfg.sp_id_list, true) != 0)
		{
			return -__LINE__;
		}
	
		cfg.item_cfg_list[item_cfg.strengthen_buff_level] = item_cfg;
		data_element = data_element->NextSiblingElement();

	}
	return 0;
}

int PetConfig::InitExpPillCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int item_id;
		if (!GetSubNodeValue(root_element, "item_id", item_id) || item_id < 0)	return -1;
		CheckResourceCenter::Instance().GetItemValidCheck()->Add(item_id, item_id, 1, __FUNCTION__);
		if (m_pet_exp_pill_map.find(item_id) != m_pet_exp_pill_map.end())
		{
			return -33;
		}
		PetExpPillCfg & node_cfg = m_pet_exp_pill_map[item_id];
		node_cfg.item_id = item_id;

		if (!GetSubNodeValue(root_element, "add_exp", node_cfg.add_exp) || node_cfg.add_exp <= 0)	return -2;
		if (!GetSubNodeValue(root_element, "money_type", node_cfg.money_type) || node_cfg.money_type < 0 || node_cfg.money_type >= MONEY_TYPE_MAX) return -3;
		if (!GetSubNodeValue(root_element, "price", node_cfg.price) || node_cfg.price < 0)	return -4;
		int &pet_pill_item_id = m_pet_exptopill_map[node_cfg.add_exp];
		pet_pill_item_id = item_id;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetConfig::InitAttrDecCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	m_attr_dec_total_rate = 0;
	while (NULL != root_element)
	{
		int attr = -1;
		if (!GetSubNodeValue(root_element, "deduct_attr", attr) || attr < 0)	return -1;
		int rate = -1;
		if (!GetSubNodeValue(root_element, "rate", rate) || rate < 0)	return -2;

		m_addr_dec_map[attr] = rate;
		m_attr_dec_total_rate += rate;
		root_element = root_element->NextSiblingElement();

		m_max_attr_dec_value = GetMax(attr, m_max_attr_dec_value);
	}
	return 0;
}


int PetConfig::InitPetCfg2(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int id = 0;
		if (!GetSubNodeValue(root_element, "id", id) || id < PET_ID_BEGIN || id >= PET_ID_BEGIN + PET_COUNT_MAX)return -1;
		PetCfg& node_cfg = m_pet_map[id];
		node_cfg.id = id;

		std::string name;
		if (!GetSubNodeValue(root_element, "name", name))return -2;
		SNPRINTF(node_cfg.name, sizeof(node_cfg.name), "%s", name.c_str());

		for (int i = 0; i < PET_EXCLUSIVE_SKILL_MAX; ++i)
		{
			PetExclusiveSkill   exclusive_skill;

			char skill_id[1024] = { 0 };
			char skill_level[1024] = { 0 };
			char skill_type[1024] = { 0 };

			SNPRINTF(skill_id, sizeof(skill_id), "skill_id_%d", i + 1);
			SNPRINTF(skill_type, sizeof(skill_type), "skill_type_%d", i + 1);
			SNPRINTF(skill_level, sizeof(skill_level), "skill_level_%d", i + 1);

			if (!GetSubNodeValue(root_element, skill_id, exclusive_skill.skill_id)) return -3;
			if (exclusive_skill.skill_id <= 0)
			{
				continue;
			}

			if (!GetSubNodeValue(root_element, skill_type, exclusive_skill.type) || (exclusive_skill.type != PET_EXCLUSIVE_SKILL_TYPE_ZHU_DONG && exclusive_skill.type != PET_EXCLUSIVE_SKILL_TYPE_PASSIVE))
			{
				return -994;
			}

			if (!GetSubNodeValue(root_element, skill_level, exclusive_skill.skill_level) || (exclusive_skill.skill_level <= 0 && exclusive_skill.type == PET_EXCLUSIVE_SKILL_TYPE_ZHU_DONG))
			{
				return -995;
			}

			node_cfg.exclusive_skills.push_back(exclusive_skill);
		}


		TiXmlElement *passive_group_id_element = root_element->FirstChildElement("gift_group_id");
		if (NULL != passive_group_id_element)
		{
			TiXmlElement *node_element = passive_group_id_element->FirstChildElement("node");
			while (node_element)
			{
				int passive_skill_id = 0;
				if (!GetNodeValue(node_element, passive_skill_id) || passive_skill_id <= 0)return -441;
				node_cfg.passive_group_id_vec.push_back(passive_skill_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		TiXmlElement *lowest_passive_group_id_element = root_element->FirstChildElement("lowest_gift_group_id");
		if (NULL != lowest_passive_group_id_element)
		{
			TiXmlElement *node_element = lowest_passive_group_id_element->FirstChildElement("node");
			while (node_element)
			{
				int passive_skill_id = 0;
				if (!GetNodeValue(node_element, passive_skill_id) || passive_skill_id <= 0)return -442;
				node_cfg.passive_group_id_lowest_vec.push_back(passive_skill_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		TiXmlElement * guide_pet_passive_element = root_element->FirstChildElement("guide_pet_skill");
		if (NULL != guide_pet_passive_element)
		{
			TiXmlElement *node_element = guide_pet_passive_element->FirstChildElement("node");
			while (node_element)
			{
				int passive_skill_id = 0;
				if (!GetNodeValue(node_element, passive_skill_id))return -443;
				node_cfg.guide_passive_vec.push_back(passive_skill_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		TiXmlElement *guide_passive_refined_vec_element = root_element->FirstChildElement("guide_skill_replace");
		if (NULL != guide_passive_refined_vec_element)
		{
			TiXmlElement *node_element = guide_passive_refined_vec_element->FirstChildElement("node");
			while (node_element)
			{
				int passive_skill_id = 0;
				if (!GetNodeValue(node_element, passive_skill_id))return -444;
				node_cfg.guide_passive_refined_vec.push_back(passive_skill_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		if (!GetSubNodeValue(root_element, "con", node_cfg.growth[ADD_POINT_TYPE_VITALITY]))	return -5;
		if (!GetSubNodeValue(root_element, "str", node_cfg.growth[ADD_POINT_TYPE_STRENGTH]))	return -6;
		if (!GetSubNodeValue(root_element, "def", node_cfg.growth[ADD_POINT_TYPE_POWER]))	return -7;
		if (!GetSubNodeValue(root_element, "agi", node_cfg.growth[ADD_POINT_TYPE_AGILITY]))	return -8;
		if (!GetSubNodeValue(root_element, "magic", node_cfg.growth[ADD_POINT_TYPE_MAGIC]))	return -9;

		if (!GetSubNodeValue(root_element, "maxhp", node_cfg.maxhp) || node_cfg.maxhp < 0)	return -10;
		if (!GetSubNodeValue(root_element, "maxmp", node_cfg.maxmp) || node_cfg.maxmp < 0)	return -11;
		if (!GetSubNodeValue(root_element, "gongji", node_cfg.gongji) || node_cfg.gongji < 0)	return -12;
		if (!GetSubNodeValue(root_element, "fangyu", node_cfg.fangyu) || node_cfg.fangyu < 0)	return -13;
		if (!GetSubNodeValue(root_element, "minjie", node_cfg.minjie) || node_cfg.minjie < 0)	return -113;
		if (!GetSubNodeValue(root_element, "jingshen", node_cfg.jingshen) || node_cfg.jingshen < 0)	return -14;
		if (!GetSubNodeValue(root_element, "huifu", node_cfg.huifu) || node_cfg.huifu < 0)	return -15;
		if (!GetSubNodeValue(root_element, "bisha", node_cfg.bisha) || node_cfg.bisha < 0)	return -16;
		if (!GetSubNodeValue(root_element, "bisha_dmg", node_cfg.bisha_dmg) || node_cfg.bisha_dmg < 0)	return -156;
		if (!GetSubNodeValue(root_element, "jianren", node_cfg.jianren) || node_cfg.jianren < 0)	return -17;
		if (!GetSubNodeValue(root_element, "fanji", node_cfg.fanji) || node_cfg.fanji < 0)	return -18;
		if (!GetSubNodeValue(root_element, "mingzhong", node_cfg.mingzhong) || node_cfg.mingzhong < 0)	return -19;
		if (!GetSubNodeValue(root_element, "shanduo", node_cfg.shanduo) || node_cfg.shanduo < 0)	return -20;
		if (!GetSubNodeValue(root_element, "def_zhongdu", node_cfg.def_zhongdu) || node_cfg.def_zhongdu < 0)	return -21;
		if (!GetSubNodeValue(root_element, "def_shihua", node_cfg.def_shihua) || node_cfg.def_shihua < 0)	return -22;
		if (!GetSubNodeValue(root_element, "def_hunshui", node_cfg.def_hunshui) || node_cfg.def_hunshui < 0)	return -23;
		if (!GetSubNodeValue(root_element, "def_jiuzui", node_cfg.def_jiuzui) || node_cfg.def_jiuzui < 0)	return -24;
		if (!GetSubNodeValue(root_element, "def_hunluan", node_cfg.def_hunluan) || node_cfg.def_hunluan < 0)	return -25;
		if (!GetSubNodeValue(root_element, "mogong", node_cfg.mogong) || node_cfg.mogong < 0)	return -26;
		if (!GetSubNodeValue(root_element, "mokang", node_cfg.mokang) || node_cfg.mokang < 0)	return -27;

		if (!GetSubNodeValue(root_element, "element_water", node_cfg.element_water) || node_cfg.element_water < 0)	return -127;
		if (!GetSubNodeValue(root_element, "element_fire", node_cfg.element_fire) || node_cfg.element_fire < 0)	return -227;
		if (!GetSubNodeValue(root_element, "element_wind", node_cfg.element_wind) || node_cfg.element_wind < 0)	return -327;
		if (!GetSubNodeValue(root_element, "element_land", node_cfg.element_land) || node_cfg.element_land < 0)	return -427;

		if (!GetSubNodeValue(root_element, "race", node_cfg.race) || node_cfg.race < 0)	return -28;
		if (!GetSubNodeValue(root_element, "is_long_range", node_cfg.is_long_range_unit) || node_cfg.is_long_range_unit < 0 || node_cfg.is_long_range_unit > 1) return -29;

		//配置表中与枚举相差1，所以减1
		int seal_card_type = 0;
		if (!GetSubNodeValue(root_element, "catch_type", seal_card_type) ||
		    seal_card_type - 1 < BATTLE_SEAL_CARD_TYPE_COMMON ||
		    seal_card_type - 1 > BATTLE_SEAL_CARD_TYPE_GOLD)
		{
			return -29;
		}
		node_cfg.seal_card_type = seal_card_type - 1;

		if (!GetSubNodeValue(root_element, "rate_common", node_cfg.rate_common) || node_cfg.rate_common < 0)	return -30;
		if (!GetSubNodeValue(root_element, "rate_silver", node_cfg.rate_silver) || node_cfg.rate_silver < 0)	return -31;
		if (!GetSubNodeValue(root_element, "rate_gold", node_cfg.rate_gold) || node_cfg.rate_gold < 0)	return -32;
		if (!GetSubNodeValue(root_element, "pr_up", node_cfg.pr_up_on_fail) || node_cfg.pr_up_on_fail < 0 || node_cfg.pr_up_on_fail > 50) return -322;
		if (!GetSubNodeValue(root_element, "fixed_times", node_cfg.catch_fixed_times) || node_cfg.catch_fixed_times <= 0) return -323;
		if (!GetSubNodeValue(root_element, "rate_patch", node_cfg.catch_reward_rate_shards) || node_cfg.catch_reward_rate_shards < 0)	return -33;
		if (!GetSubNodeValue(root_element, "rate_pet", node_cfg.catch_reward_rate_pet) || node_cfg.catch_reward_rate_pet < 0)	return -34;
		if (!GetSubNodeValue(root_element, "rate_mutant", node_cfg.catch_reward_rate_variant_pet) || node_cfg.catch_reward_rate_variant_pet < 0)	return -35;

		node_cfg.catch_reward_rate_count = node_cfg.catch_reward_rate_shards + node_cfg.catch_reward_rate_pet + node_cfg.catch_reward_rate_variant_pet;
		if (node_cfg.catch_reward_rate_count == 0)
		{
			return -36;
		}

		if (!GetSubNodeValue(root_element, "mutant_id", node_cfg.mutant_id))
		{
			return -37;
		}

		if (!GetSubNodeValue(root_element, "mutant_times", node_cfg.pet_mutant_times))
		{
			return -38;
		}

		if (node_cfg.pet_mutant_times > 0)
		{
			MutantPetCfgKey cfg_key(node_cfg.mutant_id,node_cfg.pet_mutant_times);
			m_variant_pet_cfg[cfg_key].push_back(node_cfg.id);
		}

		if (!GetSubNodeValue(root_element, "quality", node_cfg.quality) || node_cfg.quality <= PET_QUALITY_INVALID || node_cfg.quality >= PET_QUALITY_MAX)
		{
			return -39;
		}

		if (!GetSubNodeValue(root_element, "patch_id", node_cfg.shards_item_id) || (node_cfg.shards_item_id != 0 && ITEMPOOL->GetItem(node_cfg.shards_item_id) == NULL))
		{
			return -40;
		}

		if (!GetSubNodeValue(root_element, "return_type", node_cfg.abandonment_cfg_id))
		{
			return -41;
		}

		if (!GetSubNodeValue(root_element, "refresh_id", node_cfg.refined_need_item_id))
		{
			return -42;
		}

		if (!GetSubNodeValue(root_element, "refresh_num", node_cfg.refined_need_item_num) || node_cfg.refined_need_item_num <= 0)
		{
			return -421;
		}

		if (!GetSubNodeValue(root_element, "catch_patch_num", node_cfg.catch_pet_get_shards_num))
		{
			return -43;
		}

		if (!GetSubNodeValue(root_element, "slot_type", node_cfg.comprehend_id))
		{
			return -44;
		}

		if (!GetSubNodeValue(root_element, "prototype_id", node_cfg.prototype_pet_id))
		{
			return -45;
		}

		if (!GetSubNodeValue(root_element, "is_magic_pet", node_cfg.magic_pet_extra_attr_type))
		{
			return -46;
		}

		if (!GetSubNodeValue(root_element, "gift_max", node_cfg.max_passive_skill_num) || node_cfg.max_passive_skill_num > PET_PASSIVE_MAX_COUNT)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "strengthen_id", node_cfg.strengthen_cfg_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "base_score", node_cfg.base_score))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "main_attribute", node_cfg.main_attribute_id))
		{
			return -__LINE__;
		}

		node_cfg.main_attribute_id -= 1;

		if (!GetSubNodeValue(root_element, "change_consume_item", node_cfg.mutant_consume_item_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "change_consume_num", node_cfg.mutant_consume_item_num))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "benediction_id", node_cfg.benediction_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "benediction_att", node_cfg.bendiction_attr_add_precent) || node_cfg.bendiction_attr_add_precent <= 0)
		{
			return -__LINE__;
		}

		int is_normal_attack_pet = 0;
		if (GetSubNodeValue(root_element, "is_normal_attack_pet", is_normal_attack_pet))
		{
			node_cfg.is_normal_attack_pet = is_normal_attack_pet > 0;
		}

		if (!GetSubNodeValue(root_element, "inborn_type", node_cfg.inborn_type) || node_cfg.inborn_type < 0)
		{
			return -__LINE__;
		}

		CheckResourceCenter::Instance().GetPetCheck()->Add(node_cfg.prototype_pet_id, __FUNCTION__);
		if (0 != node_cfg.benediction_id)
		{
			CheckResourceCenter::Instance().GetPetCheck()->AddBenedictionCheck(node_cfg.benediction_id, __FUNCTION__);
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitComprehendPassiveCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	
	while (NULL != root_element)
	{
		

		int slot_type = 0;
		if (!GetSubNodeValue(root_element, "slot_type", slot_type))
		{
			return -1;
		}

		PetComprehendPassiveCfg & cfg = m_comprehend_passive_cfg[slot_type];
		if (!GetSubNodeValue(root_element, "stuff_id", cfg.need_item_id) || ITEMPOOL->GetItem(cfg.need_item_id) == NULL)
		{
			return -2;
		}
		
		char stuff_num[20] = {0};
		int need_item_num = 0;
		for (int i = 0; i < MAX_PET_STUFF_NUM; i++)
		{
			char stuff_num[1024] = { 0 };
			SNPRINTF(stuff_num, sizeof(stuff_num), "stuff_num%d", i);

			int need_item_num = 0;
			if (!GetSubNodeValue(root_element, stuff_num, need_item_num) || need_item_num <= 0)
			{
				return -3;
			}
			cfg.need_item_num.push_back(need_item_num);
		}

		if (!GetSubNodeValue(root_element, "rate", cfg.rate))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "progress_max", cfg.max_value))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "progress_value", cfg.add_value))
		{
			return -6;
		}

		TiXmlElement *passive_element = root_element->FirstChildElement("passive_skill");
		if (NULL != passive_element)
		{
			TiXmlElement *node_element = passive_element->FirstChildElement("node");
			while (node_element)
			{
				int passive_skill_id = 0;
				if (!GetNodeValue(node_element, passive_skill_id) || passive_skill_id <= 0)return -4;
				cfg.comprehend_passive_list.push_back(passive_skill_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		if (cfg.comprehend_passive_list.empty())
		{
			return -7;
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetConfig::InitPetComposeCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	int next_seq = 0;
	while (NULL != root_element)
	{		
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq != next_seq)
		{
			return -1;
		}
		next_seq += 1;

		PetComposeCfg  cfg;
		if (!GetSubNodeValue(root_element, "patch_id", cfg.need_item_id) || ITEMPOOL->GetItem(cfg.need_item_id) == NULL)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "patch_num", cfg.need_item_num) || cfg.need_item_num <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "personal_patch_num", cfg.personal_patch_num) || cfg.personal_patch_num <= 0 || cfg.personal_patch_num > cfg.need_item_num)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "use_universal_patch", cfg.can_use_common_shards) ||  cfg.can_use_common_shards >= PET_COMPOSE_SHARE_TYPE_MAX)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "reward_group", cfg.compose_reward_id))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "isbind", cfg.is_bind))
		{
			return -6;
		}

		TiXmlElement * item_list_element = root_element->FirstChildElement("release_item_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("release_item");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
				{
					return -__LINE__;
				}

				cfg.batch_decompose_rewards.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		m_pet_compose_cfg.push_back(cfg);
		m_pet_compose_cfg_by_item_id[cfg.need_item_id] = cfg.compose_reward_id;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitPetComposeRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int compose_reward_id = 0;
		if (!GetSubNodeValue(root_element, "reward_group", compose_reward_id))
		{
			return -1;
		}

		PetComposeRewardCfg & cfg = m_pet_compose_reward_cfg[compose_reward_id];
		PetComposeRewardCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}


		if (!GetSubNodeValue(root_element, "pet_id", item_cfg.pet_id))
		{
			return -3;
		}

		cfg.group_reward.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_pet_compose_reward_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, PetComposeRewardCfg >::iterator iter = m_pet_compose_reward_cfg.begin(); iter != m_pet_compose_reward_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_reward.size(); ++i)
		{
			count += iter->second.group_reward[i].rate;
		}

		if (count != PetComposeRewardCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int PetConfig::InitPetRandPassiveGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int passive_group_id = 0;
		if (!GetSubNodeValue(root_element, "gift_group_id", passive_group_id))
		{
			return -1;
		}

		PetRandPassiveGroupCfg & cfg = m_pet_passive_group_cfg[passive_group_id];
		PetRandPassiveGroupCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}


		if (!GetSubNodeValue(root_element, "gift_id", item_cfg.passive_skill_id))
		{
			return -3;
		}

		cfg.passive_group.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_pet_passive_group_cfg.empty())
	{
		return -10085;
	}

	for (std::map<int, PetRandPassiveGroupCfg >::iterator iter = m_pet_passive_group_cfg.begin(); iter != m_pet_passive_group_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.passive_group.size(); ++i)
		{
			count += iter->second.passive_group[i].rate;
		}

		if (count > PetRandPassiveGroupCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int PetConfig::InitPetSkillAddCapabilityCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int next_pet_level = 1;
	while (NULL != root_element)
	{
		PetSkillAddCapabilityCfg cfg;
		if (!GetSubNodeValue(root_element, "pet_level", cfg.pet_level) || cfg.pet_level != next_pet_level++)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "high_pet_hitbook", cfg.high_skill_add))
		{
			return -__LINE__;
		}


		if (!GetSubNodeValue(root_element, "lower_pet_hitbook",cfg.low_skill_add))
		{
			return -__LINE__;
		}

		for (size_t i = 0; i < 5; i++)
		{
			std::string str = STRING_SPRINTF("super_pet_hitbook_%d", i + 1);
			int add = 0;
			if (!GetSubNodeValue(root_element, str, add))
			{
				return -__LINE__;
			}

			cfg.super_skill_add.push_back(add);
		}

		m_pet_skill_add_capability_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitPetExtendPriceCfg(TiXmlElement *RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	int last_times = 0;
	while (root_element)
	{
		int times = 0;
		if (!GetSubNodeValue(root_element, "times", times) || times <= 0 || times != last_times + 1 || times > MAX_PET_BAG_EXTEND_TIMES)
		{
			return -1;
		}
		last_times = times;
		m_max_extend_times = times;
		
		int price = 0;
		if (!GetSubNodeValue(root_element, "bag_pet", price))
		{
			return -2;
		}
		
		m_extend_price_list[times] = price;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitLockSkillCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		PetLockSkillCfg cfg;
		if (!GetSubNodeValue(root_element, "quality", cfg.pet_pin_zhi))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "item_use", cfg.need_item_id))
		{
			return -__LINE__;
		}
	
		for (int i = 0; i < ARRAY_LENGTH(cfg.need_num); ++i)
		{
			std::string str = STRING_SPRINTF("price_%d", i + 1);
			if (!GetSubNodeValue(root_element, str, cfg.need_num[i]) || cfg.need_num[i] <= 0)
			{
				return -__LINE__;
			}
		}

		MapAddOrUpdate(m_lock_skill_cfg, cfg.pet_pin_zhi, cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitPetConvertCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_seq = -1;
	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq != last_seq + 1) return -1;
		last_seq = seq;

		PetConvertCfg node;
		TiXmlElement * item_element = root_element->FirstChildElement("item");
		if (NULL != item_element)
		{
			if(!node.convert_item.ReadConfig(item_element) || m_pet_map.end() == m_pet_map.find(node.convert_item.item_id)) return -333;
		}

		if (!GetSubNodeValue(root_element, "consume_item_id", node.consume_item_id) || NULL == ITEMPOOL->GetItem(node.consume_item_id)) return -2;
		if (!GetSubNodeValue(root_element, "consume_item_count", node.consume_num) || node.consume_num <= 0) return -3;
		
		int hearsay = 0;
		if (!GetSubNodeValue(root_element, "hearsay", hearsay)) return -4;
		node.hearsay = hearsay != 0 ? true : false;
		
		m_pet_convert_cfg.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitGrowthVersionCfg(TiXmlElement* RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -999;
	}

	while (dataElement)
	{
		int pet_id = 0;
		if (!GetSubNodeValue(dataElement, "id", pet_id) || NULL == this->GetPetCfg(pet_id))
		{
			return -1;
		}

		PetGrowthVersionCfg cfg;
		if (!GetSubNodeValue(dataElement, "con", cfg.growth[ADD_POINT_TYPE_VITALITY]))
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "str", cfg.growth[ADD_POINT_TYPE_STRENGTH]))
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "def", cfg.growth[ADD_POINT_TYPE_POWER]))
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "agi", cfg.growth[ADD_POINT_TYPE_AGILITY]))
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "magic", cfg.growth[ADD_POINT_TYPE_MAGIC]))
		{
			return -7;
		}

		if (!GetSubNodeValue(dataElement, "version", cfg.version) || cfg.version < 0)
		{
			return -8;
		}

		GrowthVerCfgMap::iterator it = m_growth_version_map.find(pet_id);
		if (it == m_growth_version_map.end())
		{
			// 之前没有这个宠物的资质版本配置，则新增一个
			if (cfg.version != 0)
			{
				// 版本号需从0开始，每次+1
				return -9;
			}

			GrowthVerCfgList list;
			list.push_back(cfg);

			m_growth_version_map.insert(GrowthVerCfgMap::value_type(pet_id, list));
		}
		else
		{
			if (it->second.empty())
			{
				return -10; // 不应该出现空的情况
			}

			GrowthVerCfgList& list = it->second;
			if (list[list.size() - 1].version + 1 != cfg.version)
			{
				return -11; // 版本应该紧跟上一版本号
			}

			list.push_back(cfg);
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitStrengthenBroadcastCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -999;
	}

	while (dataElement)
	{
		int pet_id = 0;
		if (!GetSubNodeValue(dataElement, "pet_id", pet_id))
		{
			return -__LINE__;
		}

		int lv_qh = 0;
		if (!GetSubNodeValue(dataElement, "lv_qh", lv_qh))
		{
			return -__LINE__;
		}

		m_strengthen_broadcast_cfg[pet_id] = lv_qh;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitBreakAttrCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int break_attr_id = 0;		//宠物突破属性id
		if (!GetSubNodeValue(root_element, "breach_level", break_attr_id))
		{
			return -1999;
		}

		PetBreakAttrCfg & cfg = m_break_attr_cfg[break_attr_id];
		cfg.break_attr_id = break_attr_id;

		int id = 0;
		if (!GetSubNodeValue(root_element, "id", id) || id < ADD_POINT_TYPE_MIN || id >= ADD_POINT_TYPE_MAX)
		{
			return -1;
		}

		PetBreakAttrCfg::AttrItem & node_cfg = cfg.attr_item[id];
		node_cfg.attr_id = id;

		if (!GetSubNodeValue(root_element, "maxhp", node_cfg.maxhp))	return -1;
		if (!GetSubNodeValue(root_element, "maxmp", node_cfg.maxmp))	return -2;
		if (!GetSubNodeValue(root_element, "gongji", node_cfg.gongji))	return -3;
		if (!GetSubNodeValue(root_element, "fangyu", node_cfg.fangyu))	return -4;
		if (!GetSubNodeValue(root_element, "minjie", node_cfg.minjie))	return -5;
		if (!GetSubNodeValue(root_element, "jingshen", node_cfg.jingshen))	return -6;
		if (!GetSubNodeValue(root_element, "huifu", node_cfg.huifu))	return -7;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitStrengthenProtectCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		PetStrengthenProtectCfg cfg;
		if (!GetSubNodeValue(root_element, "quality", cfg.pet_quality))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "strengthen_level", cfg.strengthen_level))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "item_use", cfg.protect_item_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "item_use_num", cfg.protect_item_num))
		{
			return -__LINE__;
		}

		m_strengthen_protect_cfg[cfg.pet_quality][cfg.strengthen_level] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitPetStrengtheConsumeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		PetStrengtheConsumeCfg cfg;
		cfg.prototype_id = 0;
		if (!GetSubNodeValue(root_element, "quality", cfg.pet_quality))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "strengthen_level", cfg.pet_strengthen_level))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "bind_coin_consume", cfg.need_coin))
		{
			return -__LINE__;
		}

		int material_num = 0;
		if (!GetSubNodeValue(root_element, "material_num", material_num))
		{
			return -__LINE__;
		}

		for (int i = 0; i < material_num && i < 5; ++i)
		{
			PetStrengtheConsumeCfg::ItemCfg item_cfg;
			

			std::string str1 = STRING_SPRINTF("is_oneself_%d",i+1);
			std::string str2 = STRING_SPRINTF("quality_%d", i + 1);
			std::string str3 = STRING_SPRINTF("strengthen_level_%d", i + 1);

			if (!GetSubNodeValue(root_element, str1, item_cfg.need_same_pet))
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(root_element, str2, item_cfg.need_pet_quality))
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(root_element, str3, item_cfg.pet_strengthen_level))
			{
				return -__LINE__;
			}

			cfg.consume_list.push_back(item_cfg);
		}

		PetStrengtheConsumeCfgKey key;
		key.pet_quality = cfg.pet_quality;
		key.pet_strengthen_level = cfg.pet_strengthen_level;
		m_strengthen_consume_cfg[key] = cfg;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitSpecifyPetStrengtheConsumeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		PetStrengtheConsumeCfg cfg;
		cfg.pet_quality = 0;
		if (!GetSubNodeValue(root_element, "prototype_id", cfg.prototype_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "strengthen_level", cfg.pet_strengthen_level))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "bind_coin_consume", cfg.need_coin))
		{
			return -__LINE__;
		}

		int material_num = 0;
		if (!GetSubNodeValue(root_element, "material_num", material_num))
		{
			return -__LINE__;
		}

		for (int i = 0; i < material_num && i < 5; ++i)
		{
			PetStrengtheConsumeCfg::ItemCfg item_cfg;


			std::string str1 = STRING_SPRINTF("is_oneself_%d", i + 1);
			std::string str2 = STRING_SPRINTF("quality_%d", i + 1);
			std::string str3 = STRING_SPRINTF("strengthen_level_%d", i + 1);

			if (!GetSubNodeValue(root_element, str1, item_cfg.need_same_pet))
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(root_element, str2, item_cfg.need_pet_quality))
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(root_element, str3, item_cfg.pet_strengthen_level))
			{
				return -__LINE__;
			}

			cfg.consume_list.push_back(item_cfg);
		}

		SpecifyPetStrengtheConsumeCfgKey key;
		key.prototype_id = cfg.prototype_id;
		key.pet_strengthen_level = cfg.pet_strengthen_level;
		m_specify_strengthen_consume_cfg[key] = cfg;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitPetBenedictionCfg(TiXmlElement * RootElement)
{

	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_benediction_id = 1;
	int last_benediction_level = 0;
	int last_benediction_active_score = 0;
	while (NULL != root_element)
	{
		int benediction_id = 0;
		if (!GetSubNodeValue(root_element, "benediction_id", benediction_id) || benediction_id <= 0) return -1;

		if (last_benediction_id != benediction_id)
		{
			last_benediction_id = benediction_id;
			last_benediction_level = 0;
			last_benediction_active_score = 0;
		}

		PetBenedictionSimpleCfg simple_cfg;
		char buf[64] = {0};
		for (int i = 0; i < MAX_PET_BENEDICTION__NUM; i++)
		{	
			SNPRINTF(buf, sizeof(buf), "activate_%d", i + 1);
			int active_pet_id = 0;
			if (!GetSubNodeValue(root_element, buf, active_pet_id)) return -(10 + i);

			const PetCfg * pet_cfg = this->GetPetCfg(active_pet_id);
			if (NULL == pet_cfg || pet_cfg->prototype_pet_id != active_pet_id)	//激活列表限制未变异
			{
				return -(20 + i);
			}
			simple_cfg.active_set.insert(active_pet_id);
		}
		if((int)simple_cfg.active_set.size() != MAX_PET_BENEDICTION__NUM) return -2;		//激活列表出现相同宠物

		std::map<int, PetBenedictionSimpleCfg>::iterator simple_it = m_benediction_simple_map.find(benediction_id);
		if (simple_it != m_benediction_simple_map.end() && simple_it->second.active_set != simple_cfg.active_set)		//同个祝福ID,激活条件需要相同
		{
			return -30;
		}

		PetBenedictionDetailCfg detail_cfg;
		if(!GetSubNodeValue(root_element, "benediction_level", detail_cfg.benediction_level) || detail_cfg.benediction_level != last_benediction_level + 1) return -3;
		last_benediction_level = detail_cfg.benediction_level;

		if (!GetSubNodeValue(root_element, "benediction_sp_id", detail_cfg.benediction_sp_id)) return -4;
		CheckResourceCenter::Instance().GetSpecialEffectCheck()->AddSpecial(detail_cfg.benediction_sp_id, __FUNCTION__);

		if (!GetSubNodeValue(root_element, "activate_score", detail_cfg.activate_score) || detail_cfg.activate_score <= last_benediction_active_score) return -5;
		last_benediction_active_score = detail_cfg.activate_score;
		if (!GetSubNodeValue(root_element, "sp_score", detail_cfg.sp_score) || detail_cfg.sp_score <= 0) return -6;

		std::map<int, std::map<int, PetBenedictionDetailCfg> >::iterator detail_it = m_benediction_detail_map.find(benediction_id);
		if (detail_it != m_benediction_detail_map.end() && detail_it->second.find(detail_cfg.benediction_level) != detail_it->second.end())
		{
			return -7;
		}
		
		m_benediction_simple_map[benediction_id] = simple_cfg;
		m_benediction_detail_map[benediction_id][detail_cfg.benediction_level] = detail_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitFeiShengAttrCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		PetFeiShengAttrCfg cfg;
		if (!GetSubNodeValue(root_element, "fs_pet_id", cfg.pet_id))	ret_line;
		if (!GetSubNodeValue(root_element, "fs_stage", cfg.fei_sheng_task_jie_duan))	ret_line;

		for (size_t i = 0; i < 5; i++)
		{
			AttrItemCfg attr_cfg;
			std::string type = STRING_SPRINTF("att_type_%d", i + 1);
			std::string value = STRING_SPRINTF("att_%d", i + 1);
			if (!GetSubNodeValue(root_element, type, attr_cfg.attr_type))
			{
				ret_line;
			}

			if (!GetSubNodeValue(root_element, value, attr_cfg.attr_value))
			{
				ret_line;
			}

			if (attr_cfg.attr_value == 0)
			{
				continue;
			}

			cfg.attr_list.push_back(attr_cfg);
		}

		PetFeiShengAttrCfgKey key(cfg.pet_id,cfg.fei_sheng_task_jie_duan);
		MapAddOrUpdate(m_fei_sheng_attr_cfg, key, cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetConfig::InitFeiShengTimesAttrCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		PetFeiShengTimesAttrCfg cfg;
		if (!GetSubNodeValue(root_element, "pet_id", cfg.pet_id))	ret_line;
		if (!GetSubNodeValue(root_element, "fly_num", cfg.fei_sheng_times))	ret_line;

		for (size_t i = 0; i < 5; i++)
		{
			AttrItemCfg attr_cfg;
			std::string type = STRING_SPRINTF("att_type%d", i + 1);
			std::string value = STRING_SPRINTF("att_num%d", i + 1);
			if (!GetSubNodeValue(root_element, type, attr_cfg.attr_type))
			{
				ret_line;
			}

			if (!GetSubNodeValue(root_element, value, attr_cfg.attr_value))
			{
				ret_line;
			}

			if (attr_cfg.attr_value == 0)
			{
				continue;
			}

			cfg.attr_list.push_back(attr_cfg);
		}

		PetFeiShengTimesAttrCfgKey key(cfg.pet_id, cfg.fei_sheng_times);
		MapAddOrUpdate(m_fei_sheng_times_attr_cfg, key, cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetConfig::InitPetUpgradeSkillCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		PetSkillUpgradeCfg cfg;
		if (!GetSubNodeValue(root_element, "src_skill_id", cfg.skill_id))	ret_line;
		if (!GetSubNodeValue(root_element, "intensify_level", cfg.skill_level))	ret_line;
		if (!GetSubNodeValue(root_element, "intensify_item_id", cfg.consume_item_id))	ret_line;
		if (!GetSubNodeValue(root_element, "item_num", cfg.consume_num))	ret_line;

		PetSkillUpgradeCfgKey key(cfg.skill_id,cfg.skill_level);
		MapAddOrUpdate(m_pet_skill_upgrade_cfg, key, cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetConfig::InitPetFeiShengTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		PetFeiShengTaskCfg cfg;
		if (!GetSubNodeValue(root_element, "fs_pet_id", cfg.pet_id)) ret_line;

		for (int i = 0; i < 12; i += 2)
		{
			PetFeiShengJieDuanTaskCfg task_cfg;
			std::string s1 =STRING_SPRINTF("task_id_%d", i + 1);
			std::string s2= STRING_SPRINTF("task_id_%d", i + 2);
			if (!GetSubNodeValue(root_element, s1, task_cfg.start_task_id))	ret_line;
			if (!GetSubNodeValue(root_element, s2, task_cfg.end_task_id))	ret_line;
			cfg.task_cfg.push_back(task_cfg);
		}
		m_fei_sheng_task_cfg[cfg.pet_id] = cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetConfig::InitPetFeiShengConditionsCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int fei_sheng_times;
		PetFeiShengConditionsItemCfg item_cfg;
		if (!GetSubNodeValue(root_element, "fly_num", fei_sheng_times)) ret_line;
		if (!GetSubNodeValue(root_element, "fly_type", item_cfg.conditions_type)) ret_line;
		if (!GetSubNodeValue(root_element, "param1", item_cfg.conditions_value)) ret_line;		
		PetFeiShengConditionsCfg & cfg = m_fei_sheng_conditions_cfg[fei_sheng_times];
		cfg.fei_sheng_times = fei_sheng_times;
		cfg.conditions.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitPetYiWenLuRewardsCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{

		int mutant_id = 0;
		if (!GetSubNodeValue(root_element, "mutant_id", mutant_id)) ret_line;

		PetYiWenLuCfg & cfg = m_yi_wen_lu_rewards_cfg[mutant_id];
		cfg.mutant_id = mutant_id;

		PetYiWenLuRewardCfg reward_cfg;

		if (!GetSubNodeValue(root_element, "seq", reward_cfg.reward_seq)) ret_line;
		if (!GetSubNodeValue(root_element, "condition", reward_cfg.condition)) ret_line;
		if (!GetSubNodeValue(root_element, "param", reward_cfg.condition_value)) ret_line;
		if (ReadItemConfigDataList(root_element, "item_list", reward_cfg.rewards ) != 0)ret_line;
		
		cfg.rewards_cfg[reward_cfg.reward_seq] = reward_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitPetFeiShengSkillUpLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		PetFeiShengSkillUpgradeCfg cfg;
		if (!GetSubNodeValue(root_element, "skill_id", cfg.skill_id)) ret_line;
		if (!GetSubNodeValue(root_element, "levels", cfg.level)) ret_line;
		if (!GetSubNodeValue(root_element, "strengthen_item", cfg.consume_item_id)) ret_line;
		if (!GetSubNodeValue(root_element, "strengthen_num", cfg.consume_item_num)) ret_line;
		if (!GetSubNodeValue(root_element, "skill_score", cfg.capability)) ret_line;

		for (int i = 0; i < 2; ++i)
		{
			std::string s1 = STRING_SPRINTF("att_type%d", i + 1);
			std::string s2 = STRING_SPRINTF("att_num%d", i + 1);
			AttrItemCfg attr_item;
			if (!GetSubNodeValue(root_element, s1, attr_item.attr_type)) ret_line;
			if (!GetSubNodeValue(root_element, s2, attr_item.attr_value)) ret_line;

			cfg.attr_list.push_back(attr_item);
		}

		PetFeiShengSkillUpgradeCfgKey key;
		key.skill_id = cfg.skill_id;
		key.level = cfg.level;

		m_fei_sheng_skill_up_lv_cfg[key] = cfg;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitPetParamCheckUserListCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int plat_type = 0;
		if (!GetSubNodeValue(root_element, "plat_type", plat_type))
		{
			return -1;
		}

		int role_id = 0;
		if (!GetSubNodeValue(root_element, "role_id", role_id))
		{
			return -2;
		}
		m_check_param_user_list.push_back(UniqueUserID(plat_type, role_id));

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetConfig::InitPetBulletinCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -999;

	while (root_element)
	{
		ItemID pet_id;
		if (!GetSubNodeValue(root_element, "id", pet_id) || NULL == ITEMPOOL->GetItem(pet_id)) return -1;

		m_pet_bulletin_list.push_back(pet_id);

		root_element = root_element->NextSiblingElement();
	}

	if (m_pet_bulletin_list.empty()) return -2;

	return 0;
}

int PetConfig::InitPetAbandonmentCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -999;

	while (root_element)
	{
		PetAbandonmentCfg cfg;
		if (!GetSubNodeValue(root_element, "return_type", cfg.abandonment_id)) ret_line;
		if (!GetSubNodeValue(root_element, "strengthen_level", cfg.strengthen_level)) ret_line;
		if (!GetSubNodeValue(root_element, "patch_id", cfg.abandonment_get_item_id)) ret_line;
		if (!GetSubNodeValue(root_element, "return_num", cfg.abandonment_get_item_num)) ret_line;

		PetAbandonmentCfgKey key;
		key.abandonment_id = cfg.abandonment_id;
		key.strengthen_level = cfg.strengthen_level;

		m_abandonment_cfg[key] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetComprehendPassiveCfg::GetComprehendPassiveConsumeItemNum(int passive_num) const
{
	//配置只配置学第2个被动才开始消耗，passive_num 是已经拥有数量。index=passive_num+1 - 2
	int index = passive_num - 1;
	if (index < 0 || index >= (int)need_item_num.size())
	{
		return 0;
	}

	return need_item_num[index];
}

bool PetCfg::HasSkill(int skill_id) const
{
	for (size_t i = 0; i < exclusive_skills.size(); ++i)
	{
		if (skill_id == exclusive_skills[i].skill_id)
		{
			return true;
		}
	}

	return false;
}
