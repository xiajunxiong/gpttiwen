#include "fuyuconfig.hpp"
#include "servercommon/fuyudef.h"
#include "item/itempool.h"
#include "item/fuyu/fuyuitem.h"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"

FuYuConfig::FuYuConfig()
{
}

FuYuConfig::~FuYuConfig()
{
	for (std::map<int, SPList>::iterator it = m_sp_cfg.begin(); it != m_sp_cfg.end(); it++)
	{
		SPList().swap(it->second);
	}
	std::vector<int>().swap(m_sp_group_list);
}

bool FuYuConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("target", InitTargetCfg);
	LOAD_CONFIG_2("strengthen_fuyu", InitStrengthenCfg);
	LOAD_CONFIG_2("sp_group", InitSpGroupCfg);
	LOAD_CONFIG_2("belong_fuyu", InitBelongCfg);
	LOAD_CONFIG_2("protect_item", InitProtectCfg);
	LOAD_CONFIG_2("fuyu_decompose", InitDecomposeCfg);
	LOAD_CONFIG_2("other", InitOtherCfg);

	return true;
}

const FuYuUpgradeCfg * FuYuConfig::GetUpgradeCfg(int fuyu_rank) const
{
	std::map<int, FuYuUpgradeCfg>::const_iterator it = m_fuyu_upgrade_cfg.find(fuyu_rank);
	if (it != m_fuyu_upgrade_cfg.end())
	{
		return &it->second;
	}

	return NULL;
}

bool FuYuConfig::IsStrengthenSucc(int materials_id, int target_fuyu_level) const
{
	std::map<int, LvProbMap>::const_iterator it = m_strengthen_cfg.find(materials_id);
	if (it == m_strengthen_cfg.end()) return false;

	LvProbMap::const_iterator node_it = it->second.find(target_fuyu_level);
	if (node_it == it->second.end()) return false;

	int rand_num = RandomNum(FU_YU_STRENGTHEN_TOTAL_RATE);

	return rand_num < node_it->second ? true : false;
}

int FuYuConfig::RandSpGroup() const
{
	std::vector<int> temp_vec = m_sp_group_list;
	RandomShuffle(&temp_vec[0], temp_vec.size());
	return temp_vec[0];
}

const FuYuSPCfg * FuYuConfig::GetSpByGroup(int sp_group, int fuyu_level) const
{
	std::map<int, SPList>::const_iterator it = m_sp_cfg.find(sp_group);
	if (it == m_sp_cfg.end()) return NULL;

	for (size_t i = 0; i < it->second.size(); i++)
	{
		if (fuyu_level == it->second[i].sp_level)
		{
			return &it->second[i];
		}
	}

	return NULL;
}

bool FuYuConfig::IsvalidSpId(int sp_id) const
{
	std::set<int>::const_iterator it = m_sp_id_list.find(sp_id);
	return (it != m_sp_id_list.end());
}

ItemID FuYuConfig::GetNextIdByRank(int fuyu_level)const
{
	std::map<int, int>::const_iterator it = m_rank_itemid_cfg.find(fuyu_level);
	if (it != m_rank_itemid_cfg.end()) return (ItemID)it->second;

	return 0;
}

int FuYuConfig::GetLevelByMaterials(int materials_id) const
{
	std::map<int, int>::const_iterator it = m_materialsId_materials_level_map.find(materials_id);
	if (it != m_materialsId_materials_level_map.end()) return it->second;

	return 0;
}

int FuYuConfig::GetSpGroupBySpId(int sp_id) const
{
	if (sp_id <= 0) return 0;

	std::map<int, SPList>::const_iterator it = m_sp_cfg.begin();
	for (; it != m_sp_cfg.end(); it++)
	{
		for (size_t j = 0; j < it->second.size(); j++)
		{
			if (it->second[j].sp_id == sp_id)
			{
				return it->first;
			}
		}
	}

	return 0;
}

const FuYuSPCfg* FuYuConfig::GetLowSpIdByGroup(int sp_group) const
{
	std::map<int, SPList>::const_iterator it = m_sp_cfg.find(sp_group);
	if (it == m_sp_cfg.end()) return NULL;

	return &it->second[0];
}

const std::pair<int, int>* FuYuConfig::GetProtectCfg(int fuyu_level) const
{
	std::map<int, std::pair<int, int> >::const_iterator it = m_protect_cfg.find(fuyu_level);
	if (it != m_protect_cfg.end())
	{
		return &it->second;
	}

	return NULL;
}

bool FuYuConfig::IsBelongFuYu(ItemID fuyu_id) const
{
	std::map<int, std::pair<int, int> >::const_iterator it = m_belong_fuyu_cfg.find(fuyu_id);
	return (it != m_belong_fuyu_cfg.end());
}

int FuYuConfig::GetSpLevelById(int sp_id) const
{
	if (sp_id <= 0) return 0;

	std::map<int, SPList>::const_iterator it = m_sp_cfg.begin();
	for (; it != m_sp_cfg.end(); it++)
	{
		for (size_t j = 0; j < it->second.size(); j++)
		{
			if (it->second[j].sp_id == sp_id)
			{
				return it->second[j].sp_level;
			}
		}
	}

	return 0;
}

bool FuYuConfig::IsBelongSpid(int sp_id) const
{
	if (!this->IsvalidSpId(sp_id)) return false;

	std::map<int, SPList>::const_iterator it = m_sp_cfg.begin();
	for (; it != m_sp_cfg.end(); it++)
	{
		for (size_t j = 0; j < it->second.size(); j++)
		{
			if (it->second[j].sp_id == sp_id)
			{
				return true;
			}
		}
	}

	return false;
}

const FuYuDecomposeCfg * FuYuConfig::GetDecomposeCfg(int item_id) const
{
	std::map<int, FuYuDecomposeCfg>::const_iterator it = m_decompose_cfg.find(item_id);
	if (it != m_decompose_cfg.end())
		return &it->second;
	return NULL;
}

bool FuYuConfig::RandFuYuParam(NetValueItemParam * param_data, ItemID item_id, int sp_group)
{
	if (NULL == param_data) return false;

	const ItemBase* fuyu_base = ITEMPOOL->GetItem(item_id);
	if (NULL == fuyu_base) return false;
	const FuYuItem* fuyu_item = (const FuYuItem *)fuyu_base;

	FuYuParam& data = param_data->fu_yu_param;
	data.Reset();
	data.has_net_value = 1;

	const FuYuUpgradeCfg* upgrade_cfg = this->GetUpgradeCfg(fuyu_item->GetFuYuLevel());
	if (NULL == upgrade_cfg) return false;

	data.attr_num = upgrade_cfg->attri_num;
	for (size_t i = 0; i < upgrade_cfg->attrs.size() && i < MAX_FU_YU_ATTR_NUM; i++)
	{
		data.base_attr_list[i].attr_type = upgrade_cfg->attrs[i].attr_type;
		data.base_attr_list[i].attr_value = upgrade_cfg->attrs[i].attr_value;
	}

	const FuYuSPCfg* sp_cfg = this->GetSpByGroup(sp_group, fuyu_item->GetFuYuLevel());
	if (NULL != sp_cfg)
	{
		data.sp_id = sp_cfg->sp_id;
	}

	// 如果找到了,说明是特殊符玉,直接给特殊的特效id
	std::map<int, std::pair<int, int> >::const_iterator it = m_belong_fuyu_cfg.find(item_id);
	if (it != m_belong_fuyu_cfg.end())
	{
		data.sp_id = it->second.second;
	}

	return true;
}

int FuYuConfig::InitTargetCfg(TiXmlElement * RootElement, std::string& err)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int rank = 0;
		if (!GetSubNodeValue(data_element, "rank", rank) || rank <= 0 || rank > MAX_FU_YU_RANK_NUM) return -101;

		std::map<int, FuYuUpgradeCfg>::const_iterator it = m_fuyu_upgrade_cfg.find(rank);
		if (it != m_fuyu_upgrade_cfg.end())
		{
			err = STRING_SPRINTF("rank repeat, rank=%d", rank);
			return -rank;
		}

		FuYuUpgradeCfg up_cfg;

		if (!GetSubNodeValue(data_element, "attri_num", up_cfg.attri_num) || up_cfg.attri_num < 0) return -103;

		std::string attri_tpye = "attri_tpye", value = "value_";
		for (int i = 1; i <= up_cfg.attri_num; i++)	// 可以在这里保证需要多少条属性就只读多少条属性, 使用时更方便
		{
			AttrInfo attr;
			if (!GetSubNodeValue(data_element, STRING_SPRINTF("%s%d", attri_tpye.c_str(), i), attr.attr_type) || attr.attr_type < BATTLE_ATTR_MIN || attr.attr_type >= BATTLE_ATTR_MAX) return -104;
			if (!GetSubNodeValue(data_element, STRING_SPRINTF("%s%d", value.c_str(), i), attr.attr_value) || attr.attr_value < 0) return 105;

			up_cfg.attrs.push_back(attr);
		}

		if (up_cfg.attri_num != (int)up_cfg.attrs.size())
		{
			return -200;
		}

		m_fuyu_upgrade_cfg[rank] = up_cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int FuYuConfig::InitStrengthenCfg(TiXmlElement * RootElement, std::string& err)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int materials_id = 0;
		if (!GetSubNodeValue(data_element, "materials_id", materials_id) || NULL == ITEMPOOL->GetItem(materials_id))
		{
			err = STRING_SPRINTF("can not find item_id[%d]", materials_id);
			return -1;
		}

		int materials_level = 0;
		if (!GetSubNodeValue(data_element, "materials_level", materials_level) || materials_level <= 0 || materials_level > MAX_FU_YU_RANK_NUM)
		{
			return -22;
		}	

		int next_item = 0;
		if (!GetSubNodeValue(data_element, "next_item", next_item) || NULL == ITEMPOOL->GetItem(next_item))
		{
			return -23;
		}

		std::map<int, LvProbMap>::const_iterator it = m_strengthen_cfg.find(materials_id);
		if (it != m_strengthen_cfg.end())
		{
			err = STRING_SPRINTF("materials_id repeat, materials_id=%d", materials_id);
			return -201;
		}

		LvProbMap lv_prob_cfg;

		std::string add_rate_front = "level", add_rate_suffix = "_add_rate";
		for (int i = 1; i < MAX_FU_YU_RANK_NUM; i++)
		{
			int rate = -1;
			if (!GetSubNodeValue(data_element, STRING_SPRINTF("%s%d%s", add_rate_front.c_str(), i, add_rate_suffix.c_str()), rate) || rate < 0 || rate > FU_YU_STRENGTHEN_TOTAL_RATE) return -2;

			lv_prob_cfg[i] = rate;
		}
		if ((int)lv_prob_cfg.size() != MAX_FU_YU_RANK_NUM - 1) return -202;
		
		m_strengthen_cfg[materials_id] = lv_prob_cfg;
		m_materialsId_materials_level_map[materials_id] = materials_level;
		m_rank_itemid_cfg[materials_level] = next_item;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int FuYuConfig::InitSpGroupCfg(TiXmlElement * RootElement, std::string& err)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int sp_group = 0;
		if (!GetSubNodeValue(data_element, "sp_group", sp_group) || sp_group <= 0) return -1;

		std::map<int, SPList>::iterator it = m_sp_cfg.find(sp_group);
		if (it == m_sp_cfg.end())
		{
			m_sp_cfg.insert(std::make_pair(sp_group, SPList()));			
			it = m_sp_cfg.find(sp_group);
			if (it == m_sp_cfg.end()) return -101;		
		}

		SPList& sp_list = it->second;

		FuYuSPCfg fuyu_sp_cfg;
		if (!GetSubNodeValue(data_element, "sp_level", fuyu_sp_cfg.sp_level) || fuyu_sp_cfg.sp_level <= 0 || fuyu_sp_cfg.sp_level > MAX_FU_YU_RANK_NUM) return -2;
		if (!GetSubNodeValue(data_element, "limit_type", fuyu_sp_cfg.limit_type) || fuyu_sp_cfg.limit_type < 0 || fuyu_sp_cfg.limit_type >= 3) return -3;
		if (!GetSubNodeValue(data_element, "param1", fuyu_sp_cfg.param1) || fuyu_sp_cfg.param1 < 0) return -4;
		if (!GetSubNodeValue(data_element, "sp_id", fuyu_sp_cfg.sp_id) || NULL == BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(fuyu_sp_cfg.sp_id))
		{
			err = STRING_SPRINTF("not find sp_id[%d]", fuyu_sp_cfg.sp_id);
			return -5;
		}

		for (size_t i = 0; i < sp_list.size(); i++)
		{
			if (sp_list[i].sp_level == fuyu_sp_cfg.sp_level)
			{
				err = STRING_SPRINTF("sp_level repeat, sp_level=%d", fuyu_sp_cfg.sp_level);
				return -102;
			}
		}

		sp_list.push_back(fuyu_sp_cfg);

		if (std::find(m_sp_group_list.begin(), m_sp_group_list.end(), sp_group) == m_sp_group_list.end())
		{
			m_sp_group_list.push_back(sp_group);
		}

		m_sp_id_list.insert(fuyu_sp_cfg.sp_id);

		data_element = data_element->NextSiblingElement();
	}

	if (m_sp_group_list.empty()) return -103;

	return 0;
}

int FuYuConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "fuyu_convert", m_other_cfg.switch_consume_item) || NULL == ITEMPOOL->GetItem(m_other_cfg.switch_consume_item)) return -1;
	
	return 0;
}

int FuYuConfig::InitDecomposeCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int item_id;
		if (!GetSubNodeValue(data_element, "item_id", item_id) || NULL == ITEMPOOL->GetItem(item_id))
		{
			err = STRING_SPRINTF("invalid item_id[%d]", item_id);
			return -1;
		}

		FuYuDecomposeCfg cfg;
		if (!GetSubNodeValue(data_element, "resolve_num_1", cfg.common_fuyu_decompose) || cfg.common_fuyu_decompose < 0) return -2;
		if (!GetSubNodeValue(data_element, "resolve_num_2", cfg.hunyuan_fuyu_decompose) || cfg.hunyuan_fuyu_decompose < 0) return -3;
		if (!GetSubNodeValue(data_element, "resolve_num_3", cfg.huashen_fuyu_decompose) || cfg.huashen_fuyu_decompose < 0) return -4;


		if (m_decompose_cfg.find(item_id) != m_decompose_cfg.end())
		{
			err = STRING_SPRINTF("repeat item_id[%d]", item_id);
			return -5;
		}

		m_decompose_cfg[item_id] = cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int FuYuConfig::InitBelongCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		ItemID item_id;
		if (!GetSubNodeValue(data_element, "id", item_id) || NULL == ITEMPOOL->GetItem(item_id)) return -1;

		int fuyu_level = 0;
		if (!GetSubNodeValue(data_element, "fuyu_level", fuyu_level) || fuyu_level <= 0 || fuyu_level > MAX_FU_YU_RANK_NUM) return -2;

		int sp_id = 0;
		if (!GetSubNodeValue(data_element, "sp_id", sp_id) || m_sp_id_list.find(sp_id) == m_sp_id_list.end())
		{
			err = STRING_SPRINTF("belong_fuyu sp_id[%d] not exist", sp_id);
			return -3;
		}

		m_belong_fuyu_cfg[item_id] = std::make_pair(fuyu_level, sp_id);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int FuYuConfig::InitProtectCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int level = 0;
		if (!GetSubNodeValue(data_element, "fuyu_level", level) || level <= 0 || level >= MAX_FU_YU_RANK_NUM) return -1;

		std::pair<int, int> temp_pair;
		if (!GetSubNodeValue(data_element, "protect_item", temp_pair.first) || NULL == ITEMPOOL->GetItem(temp_pair.first)) return -2;
		if (!GetSubNodeValue(data_element, "num", temp_pair.second) || temp_pair.second < 0) return -3;

		m_protect_cfg[level] = temp_pair;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
