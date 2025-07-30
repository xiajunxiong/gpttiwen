#include "other/petmonopoly/petmonopolyconfig.hpp"
#include "checkresourcecenter.hpp"
#include "other/pet/petconfig.hpp"

PetMonopolyConfig::PetMonopolyConfig() : m_reward_group_num(0), m_turn_table_group_num(0),
	m_layer_cfg_num(0)
{
	memset(m_level_exp_list, 0, sizeof(m_level_exp_list));
	memset(m_special_item_param,0,sizeof(m_special_item_param));
}

PetMonopolyConfig::~PetMonopolyConfig()
{

}

bool PetMonopolyConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("reward_group", InitRewardGroup);
	LOAD_CONFIG("monster_group", InitPetGroup);
	LOAD_CONFIG("turn_table", InitTurnTableGroup);
	//LOAD_CONFIG("exp", InitExpCfg);			//经验奖励使用水晶副本配置
	LOAD_CONFIG("bless_rate", InitExpectPetRateUpCfg);
	LOAD_CONFIG("scene", InitLayerCfg);
	LOAD_CONFIG("special_item", InitSpecialItemCfg);
	LOAD_CONFIG("wish_card", InitWishCade);

	return true;
}

const PetMonopolyLayerCfg* PetMonopolyConfig::GetLowerBoundLayerCfg(int layer)
{
	for (int i = m_layer_cfg_num - 1; i >= 0; --i)
	{
		if (m_layer_cfg_list[i].layer <= layer)
		{
			return &m_layer_cfg_list[i];
		}
	}

	return NULL;
}

const PetMonopolyRewardGroup* PetMonopolyConfig::GetRewardGroup(int group_id)
{
	return this->MutableRewardGroupCfg(group_id);
}

const PetMonopolyTurnTableGroup* PetMonopolyConfig::GetTurntableGroup(int group_id)
{
	return this->MutableTurnTableGroupCfg(group_id);
}

bool PetMonopolyConfig::IsPetInPool(int pet_id, int quality)
{
	return this->IsPetInPoolImpl(pet_id, m_pet_group.quality_pet_num[quality], m_pet_group.quality_pet_list[quality]);
}

bool PetMonopolyConfig::RandItemByRewardGroupId(int reward_group_id, ARG_OUT PetMonopolyGridItem* item)
{
	if (NULL == item) return false;

	const PetMonopolyRewardGroup* reward_group = this->GetRewardGroup(reward_group_id);
	if (NULL == reward_group) return false;

	const PetMonopolyRewardGroupItem* reward_item = this->RandItemFromRewardGroup(reward_group);
	if (NULL == reward_item) return false;

	item->item_id = reward_item->item.item_id;
	item->item_num = reward_item->item.num;
	item->is_bind = reward_item->item.is_bind ? 1 : 0;
	item->reserve_ch = 0;
	item->reserve_sh = 0;

	return true;
}

bool PetMonopolyConfig::RandPetFragmentByPetQuality(int pet_quality, int specific_pet, int expect_silver_pet_id, int expect_gold_pet_id, ARG_OUT PetMonopolyGridItem* item)
{
	if (NULL == item) return false;

	int expect_pet_id = 0;
	if (PET_QUALITY_GOLD == pet_quality) expect_pet_id = expect_gold_pet_id;
	if (PET_QUALITY_SILVER == pet_quality) expect_pet_id = expect_silver_pet_id;

	const PetMonopolyPetGroupItem* pet_group_item = this->RandPetItemFromPetGroup(expect_pet_id, specific_pet, pet_quality);
	if (NULL == pet_group_item) return false;

	item->item_id = pet_group_item->fragment_item_id;
	item->item_num = pet_group_item->fragment_item_num;
	item->is_bind = 1;
	item->reserve_ch = 0;
	item->reserve_sh = 0;

	

	return true;
}

bool PetMonopolyConfig::RandPetMonsterByPetQuality(int silver_pet_chance, int specific_pet, int expect_silver_pet_id, int expect_gold_pet_id, ARG_OUT PetMonopolyGridPet* pet)
{
	if (NULL == pet) return false;

	int pet_quality = this->RandPetQuality(silver_pet_chance);
	int expect_pet_id = 0;
	if (PET_QUALITY_GOLD == pet_quality) expect_pet_id = expect_gold_pet_id;
	if (PET_QUALITY_SILVER == pet_quality) expect_pet_id = expect_silver_pet_id;

	const PetMonopolyPetGroupItem* pet_group_item = this->RandPetItemFromPetGroup(expect_pet_id, specific_pet, pet_quality);
	if (NULL == pet_group_item) return false;

	pet->pet_id = pet_group_item->pet_id;
	pet->monster_group_id = pet_group_item->monster_group_id;

	return true;
}

int PetMonopolyConfig::RandPetQuality(int silver_pet_chance)
{
	int r = RandomNum(100);
	if (r < silver_pet_chance)
	{
		return PET_QUALITY_SILVER;
	}
	else
	{
		return PET_QUALITY_GOLD;
	}
}

bool PetMonopolyConfig::RandSpecialItem(ARG_OUT PetMonopolyGridSpecialItem * special_item)
{
	if (NULL == special_item)
	{
		return false;
	}
	int item_id = RandomNum(PET_MONOPOLY_SPECIAL_ITEM_MAX);

	special_item->special_item_id = item_id;

	return true;
}


int PetMonopolyConfig::GetAddExpNum(int level)
{
	if (level <= 0 || level > MAX_ROLE_LEVEL) return 0;

	return m_level_exp_list[level];
}

int PetMonopolyConfig::GetExpectPetRate(int pet_quality)
{
	if (PET_QUALITY_GOLD == pet_quality)
	{
		return m_other_cfg.expect_pet_rate_gold;
	}
	else if (PET_QUALITY_SILVER == pet_quality)
	{
		return m_other_cfg.expect_pet_rate_silver;
	}
	else
	{
		return 0;
	}
}

int PetMonopolyConfig::GetSpecialItemParam(int special_item_id)
{
	if (special_item_id < 0 || special_item_id >= PET_MONOPOLY_SPECIAL_ITEM_MAX)
	{
		return -1;
	}

	return m_special_item_param[special_item_id];
}

const PetMonopolyWishCard* PetMonopolyConfig::GetWishCardReward(int wish_card_seq)
{
	if (wish_card_seq <= 0 || wish_card_seq > MAX_WISH_CARD_NUM)
	{
		return NULL;
	}

	return &m_wish_card[wish_card_seq - 1];
}

int PetMonopolyConfig::InitOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "cost", m_other_cfg.dice_cost_energy) || m_other_cfg.dice_cost_energy <= 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "pet_monster_group", m_other_cfg.guide_pet_monster_group) || m_other_cfg.guide_pet_monster_group <= 0)
	{
		return -2;
	}
	CheckResourceCenter::Instance().GetMonsterGroupCheck()->Add(m_other_cfg.guide_pet_monster_group, __FUNCTION__);

	return 0;
}

int PetMonopolyConfig::InitRewardGroup(TiXmlElement *RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int group_id = 0;
		if (!GetSubNodeValue(dataElement, "reward_group_id", group_id) || group_id <= 0)
		{
			return -1;
		}

		PetMonopolyRewardGroup* cfg = this->MutableRewardGroupCfg(group_id);

		if (NULL == cfg)
		{
			if (m_reward_group_num >= MAX_REWARD_GROUP_NUM)
			{
				return -2;
			}

			PetMonopolyRewardGroup& group = m_reward_group_list[m_reward_group_num++];
			group.reward_num = 0;
			group.total_rate = 0;
			group.reward_group_id = group_id;

			cfg = this->MutableRewardGroupCfg(group_id);
			if (NULL == cfg)
			{
				return -3;
			}
		}

		if (cfg->reward_num >= ARRAY_ITEM_COUNT(cfg->reward_list))
		{
			return -4;
		}

		int special = -1;
		if (!GetSubNodeValue(dataElement, "special_item", special))
		{
			return -7;
		}
		cfg->special_item = special;

		if (0 == cfg->special_item)
		{
			TiXmlElement* rewardElement = dataElement->FirstChildElement("reward_item");
			if (NULL == rewardElement)
			{
				return -55;
			}

			ItemConfigData item;
			if (!item.ReadConfig(rewardElement))
			{
				return -5;
			}
			cfg->reward_list[cfg->reward_num].item = item;
		}

		int rate = 0;
		if (!GetSubNodeValue(dataElement, "rate", rate) || rate <= 0)
		{
			return -6;
		}
		cfg->reward_list[cfg->reward_num].rate = rate;
		cfg->reward_num += 1;
		cfg->total_rate += rate;


		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int PetMonopolyConfig::InitPetGroup(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement != NULL)
	{
		int pet_id = 0;
		if (!GetSubNodeValue(dataElement, "id", pet_id) || pet_id <= 0)
		{
			return -1;
		}
		CheckResourceCenter::Instance().GetPetCheck()->Add(pet_id, __FUNCTION__);

		int pet_quality = 0;
		if (!GetSubNodeValue(dataElement, "quality", pet_quality) || pet_quality <= PET_QUALITY_INVALID || pet_quality >= PET_QUALITY_MAX)
		{
			return -2;
		}

		int monster_group_id = 0;
		if (!GetSubNodeValue(dataElement, "monster_group_id", monster_group_id) || monster_group_id <= 0)
		{
			return -3;
		}
		CheckResourceCenter::Instance().GetMonsterGroupCheck()->Add(monster_group_id, __FUNCTION__);

		int fragment_item_id = 0;
		if (!GetSubNodeValue(dataElement, "patch_id", fragment_item_id) || NULL == ITEMPOOL->GetItem(fragment_item_id))
		{
			return -4;
		}

		int fragment_item_num = 0;
		if (!GetSubNodeValue(dataElement, "patch_num", fragment_item_num) || fragment_item_num <= 0)
		{
			return -5;
		}

		if (m_pet_group.quality_pet_num[pet_quality] < ARRAY_LENGTH(m_pet_group.quality_pet_list[pet_quality]))
		{
			int& num = m_pet_group.quality_pet_num[pet_quality];
			m_pet_group.quality_pet_list[pet_quality][num].pet_id = pet_id;
			m_pet_group.quality_pet_list[pet_quality][num].quality = pet_quality;
			m_pet_group.quality_pet_list[pet_quality][num].monster_group_id = monster_group_id;
			m_pet_group.quality_pet_list[pet_quality][num].fragment_item_id = fragment_item_id;
			m_pet_group.quality_pet_list[pet_quality][num].fragment_item_num = fragment_item_num;
			num += 1;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int PetMonopolyConfig::InitTurnTableGroup(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement != NULL)
	{
		int group_id = 0;
		if (!GetSubNodeValue(dataElement, "reward_group_id", group_id) || group_id <= 0)
		{
			return -1;
		}

		PetMonopolyTurnTableGroup* cfg = this->MutableTurnTableGroupCfg(group_id);
		if (NULL == cfg)
		{
			if (m_turn_table_group_num >= MAX_TURNTABLE_GROUP_NUM)
			{
				return -2;
			}

			PetMonopolyTurnTableGroup& group = m_turn_table_group_list[m_turn_table_group_num++];
			group.sector_num = 0;
			group.total_rate = 0;
			group.group_id = group_id;

			cfg = this->MutableTurnTableGroupCfg(group_id);
			if (NULL == cfg)
			{
				return -3;
			}
		}

		if (cfg->sector_num >= ARRAY_ITEM_COUNT(cfg->sector_list))
		{
			return -33;
		}

		int reward_type = 0;
		if (!GetSubNodeValue(dataElement, "reward_type", reward_type) || reward_type < 0 || reward_type >= PET_MONOPOLY_TURNTABLE_REWARD_MAX)
		{
			return -4;
		}

		int param = 0;
		if (!GetSubNodeValue(dataElement, "param", param) || param < 0)
		{
			return -5;
		}

		ItemConfigData item;
		if (PET_MONOPOLY_TURNTABLE_REWARD_ITEM == reward_type)
		{
			TiXmlElement* rewardElement = dataElement->FirstChildElement("reward_item");
			if (NULL == rewardElement)
			{
				return -66;
			}
			
			if (!item.ReadConfig(rewardElement))
			{
				return -6;
			}
		}
		
		int rate = 0;
		if (!GetSubNodeValue(dataElement, "rate", rate) || rate <= 0)
		{
			return -7;
		}

		cfg->sector_list[cfg->sector_num].reward_type = reward_type;
		cfg->sector_list[cfg->sector_num].item = item;
		cfg->sector_list[cfg->sector_num].param = param;
		cfg->sector_list[cfg->sector_num].rate = rate;
		cfg->sector_num += 1;

		cfg->total_rate += rate;

		dataElement = dataElement->NextSiblingElement();
	}

	for (int i = 0; i < m_turn_table_group_num; ++i)
	{
		if (m_turn_table_group_list[i].sector_num != PET_MONOPOLY_TURNTABLE_SECTOR_NUM)
		{
			return -888; // 转盘必须是8个格
		}
	}

	return 0;
}

int PetMonopolyConfig::InitExpCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	int last_max_level = 0;
	while (dataElement)
	{
		int min_level = 0;
		if (!GetSubNodeValue(dataElement, "min_level", min_level) || min_level < 1 || min_level > MAX_ROLE_LEVEL || min_level != last_max_level + 1)
		{
			return -1;
		}

		int max_level = 0;
		if (!GetSubNodeValue(dataElement, "max_level", max_level) || max_level < min_level)
		{
			return -2;
		}
		last_max_level = max_level;

		int exp = 0;
		if (!GetSubNodeValue(dataElement, "exp", exp) || exp <= 0)
		{
			return -3;
		}
		
		for (int i = min_level; i <= max_level; ++i)
		{
			m_level_exp_list[i] = exp;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	if (last_max_level != MAX_ROLE_LEVEL)
	{
		return -100;
	}

	return 0;
}

int PetMonopolyConfig::InitExpectPetRateUpCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int quality = 0;
		if (!GetSubNodeValue(dataElement, "quality", quality) || (quality != PET_QUALITY_SILVER && quality != PET_QUALITY_GOLD))
		{
			return -1;
		}

		int rate_up = 0;
		if (!GetSubNodeValue(dataElement, "rate_up", rate_up))
		{
			return -2;
		}

		if (PET_QUALITY_GOLD == quality)
		{
			m_other_cfg.expect_pet_rate_gold = rate_up;
		}
		else if (PET_QUALITY_SILVER == quality)
		{
			m_other_cfg.expect_pet_rate_silver = rate_up;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	if (0 == m_other_cfg.expect_pet_rate_silver || 0 == m_other_cfg.expect_pet_rate_gold)
	{
		return -100;
	}

	return 0;
}

int PetMonopolyConfig::InitLayerCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	int last_layer = 0;
	int last_grid = 0;
	while (dataElement)
	{
		int layer = 0;
		if (!GetSubNodeValue(dataElement, "layer", layer) || layer < 0 || layer < last_layer)
		{
			return -1;
		}

		if (last_layer != layer)
		{
			last_grid = 0;
			last_layer = layer;
		}

		PetMonopolyLayerCfg* cfg = this->MutableLayerCfg(layer);
		if (NULL == cfg)
		{
			if (m_layer_cfg_num >= ARRAY_ITEM_COUNT(m_layer_cfg_list))
			{
				return -2;
			}

			m_layer_cfg_list[m_layer_cfg_num++].layer = layer;

			cfg = this->MutableLayerCfg(layer);
			if (NULL == cfg)
			{
				return -222;
			}
		}

		int grid = 0;
		if (!GetSubNodeValue(dataElement, "seq", grid) || grid != last_grid + 1 || grid <= 0)
		{
			return -3;
		}
		last_grid = grid;
		int grid_idx = grid - 1;

		int reward_type = 0;
		if (!GetSubNodeValue(dataElement, "reward_type", reward_type) || reward_type < 0 || reward_type >= PET_MONOPOLY_GRID_REWARD_MAX)
		{
			return -4;
		}

		int param = 0;
		if (!GetSubNodeValue(dataElement, "param", param) || param < 0)
		{
			return -5;
		}
		if (PET_MONOPOLY_GRID_REWARD_PET_MONSTER == reward_type)
		{
			if (param <= 0 || param > 100)
			{
				return -51;
			}
		}
		else if (PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT == reward_type)
		{
			if (param <= PET_QUALITY_INVALID || param >= PET_QUALITY_MAX)
			{
				return -52;;
			}
		}

		int param2 = 0;
		if (!GetSubNodeValue(dataElement, "param_2", param2) || param2 < 0)
		{
			return -55;
		}
		if (param2 > 0 && (PET_MONOPOLY_GRID_REWARD_PET_MONSTER == reward_type ||
			PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT == reward_type))
		{
			CheckResourceCenter::Instance().GetPetCheck()->Add(param2, __FUNCTION__);
		}

		int grid_type = 0;
		if (!GetSubNodeValue(dataElement, "lattice_type", grid_type) || grid_type < 0 || grid_type >= PET_MONOPOLY_GRID_TYPE_MAX)
		{
			return -6;
		}

		int version = 0;
		if (!GetSubNodeValue(dataElement, "reset", version) || version < 0)
		{
			return -7;
		}

		cfg->grid_list[grid_idx].layer = layer;
		cfg->grid_list[grid_idx].grid_idx = grid_idx;
		cfg->grid_list[grid_idx].reward_type = reward_type;
		cfg->grid_list[grid_idx].param = param;
		cfg->grid_list[grid_idx].param2 = param2;
		cfg->grid_list[grid_idx].grid_type = grid_type;
		cfg->grid_list[grid_idx].version = version;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int PetMonopolyConfig::InitSpecialItemCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{

		int t_special_item_id = 0;
		if (!GetSubNodeValue(dataElement, "special_item_id", t_special_item_id)|| t_special_item_id<0 || t_special_item_id>= PET_MONOPOLY_SPECIAL_ITEM_MAX)
		{
			return -1;
		}

		int param = 0;
		if (!GetSubNodeValue(dataElement, "param", param) ||param <0)
		{
			return - 2;
		}

		m_special_item_param[t_special_item_id] = param;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int PetMonopolyConfig::InitWishCade(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int t_reward_group_id = 0;
		if (!GetSubNodeValue(dataElement, "seq", t_reward_group_id) || t_reward_group_id < 0 )
		{
			return -1;
		}

		int reward_type = 0;
		if (!GetSubNodeValue(dataElement, "reward_type", reward_type) || reward_type < 0 || reward_type >= PET_MONOPOLY_GRID_REWARD_MAX)
		{
			return -2;
		}

		TiXmlElement* rewardElement = dataElement->FirstChildElement("reward_item");
		if (NULL == rewardElement)
		{
			return -55;
		}

		ItemConfigData t_item;
		if (!t_item.ReadConfig(rewardElement))
		{
			return -3;
		}

		m_wish_card[t_reward_group_id - 1].reward_group_id = t_reward_group_id;
		m_wish_card[t_reward_group_id - 1].item = t_item;
		m_wish_card[t_reward_group_id - 1].reward_type = reward_type;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}


PetMonopolyRewardGroup* PetMonopolyConfig::MutableRewardGroupCfg(int group_id)
{
	for (int i = 0; i < m_reward_group_num; ++i)
	{
		if (group_id == m_reward_group_list[i].reward_group_id)
		{
			return &m_reward_group_list[i];
		}
	}

	return NULL;
}

PetMonopolyTurnTableGroup* PetMonopolyConfig::MutableTurnTableGroupCfg(int group_id)
{
	for (int i = 0; i < m_turn_table_group_num; ++i)
	{
		if (group_id == m_turn_table_group_list[i].group_id)
		{
			return &m_turn_table_group_list[i];
		}
	}

	return NULL;
}

PetMonopolyLayerCfg* PetMonopolyConfig::MutableLayerCfg(int layer)
{
	for (int i = 0; i < m_layer_cfg_num; ++i)
	{
		if (layer == m_layer_cfg_list[i].layer)
		{
			return &m_layer_cfg_list[i];
		}
	}

	return NULL;
}

const PetMonopolyRewardGroupItem* PetMonopolyConfig::RandItemFromRewardGroup(const PetMonopolyRewardGroup* reward_group)
{
	if (NULL == reward_group) return NULL;
	if (reward_group->total_rate <= 0) return NULL;

	int r = RandomNum(reward_group->total_rate);
	int previous_sum = 0;
	for (int i = 0; i < reward_group->reward_num; ++i)
	{
		if (r < reward_group->reward_list[i].rate + previous_sum)
		{
			return &reward_group->reward_list[i];
		}

		previous_sum += reward_group->reward_list[i].rate;
	}

	return NULL;
}

const PetMonopolyPetGroupItem* PetMonopolyConfig::RandPetItemFromPetGroup(int expect_pet_id, int specific_pet_id, int pet_quality)
{
	if (pet_quality <= PET_QUALITY_INVALID || pet_quality >= PET_QUALITY_MAX) return NULL;

	if (m_pet_group.quality_pet_num[pet_quality] <= 0) return NULL;

	if (specific_pet_id > 0)
	{
		for (int i = 0; i < m_pet_group.quality_pet_num[pet_quality]; ++i)
		{
			if (specific_pet_id == m_pet_group.quality_pet_list[pet_quality][i].pet_id)
			{
				return &m_pet_group.quality_pet_list[pet_quality][i];
			}
		}
	}

	int r = RandomNum(100);
	if (r < this->GetExpectPetRate(pet_quality))
	{
		for (int i = 0; i < m_pet_group.quality_pet_num[pet_quality]; ++i)
		{
			if (expect_pet_id == m_pet_group.quality_pet_list[pet_quality][i].pet_id)
			{
				return &m_pet_group.quality_pet_list[pet_quality][i];
			}
		}
	}

	int rand_idx = RandomNum(m_pet_group.quality_pet_num[pet_quality]);
	return &m_pet_group.quality_pet_list[pet_quality][rand_idx];
}

bool PetMonopolyConfig::IsPetInPoolImpl(int pet_id, int pool_pet_num, PetMonopolyPetGroupItem* pool_pet_list)
{
	for (int i = 0; i < pool_pet_num; ++i)
	{
		if (pet_id == pool_pet_list[i].pet_id)
		{
			return true;
		}
	}

	return false;
}

