#include "collectionconfig.hpp"
#include "servercommon/collectiondef.hpp"
#include "checkresourcecenter.hpp"

CollectionConfig::CollectionConfig()
{
	m_max_seq = 0;
}

CollectionConfig::~CollectionConfig()
{
	for (std::map<int, std::vector<CollectionActivationCfg> >::iterator it = m_collection_activation_map.begin(); it != m_collection_activation_map.end(); it++)
	{
		std::vector<CollectionActivationCfg>().swap(it->second);
	}
}
bool CollectionConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;	
	
	LOAD_CONFIG("collection_jihuo", InitCollectionCfg);
	LOAD_CONFIG("convert", InitConvertCfg);
	LOAD_CONFIG("collect", InitCollectCfg);
	LOAD_CONFIG("treasures", InitTreasuresCfg);
	return true;
}

const CollectionCfg * CollectionConfig::GetCollectionCfg(int seq)
{
	std::map<int, CollectionCfg>::const_iterator it = m_collection_map.find(seq);
	if (it == m_collection_map.end()) return NULL;
	return &it->second;
}

const CollectionActivationCfg * CollectionConfig::GetPetCollectBySeq(int seq)
{
	std::map<int,CollectionActivationCfg>::iterator it = m_collection_activation_seq_map.find(seq);
	if (it != m_collection_activation_seq_map.end())
	{
		return &it->second;
	}

	return NULL;
}

const CollectionActivationCfg * CollectionConfig::GetPetCollectByPetId(int pet_id)
{
	std::map<int, std::vector<CollectionActivationCfg> >::iterator it = m_collection_activation_map.find(COLLECTION_TYPE_PET);
	if (it != m_collection_activation_map.end())
	{
		for (std::vector<CollectionActivationCfg>::iterator it_pet = it->second.begin(); it_pet != it->second.end(); it_pet++)
		{
			if (it_pet->id == pet_id)
			{
				return &(*it_pet);
			}
		}
	}
	return NULL;
}

const CollectionActivationCfg * CollectionConfig::GetPartnerCollectByPartnerId(int partner_id)
{
	std::map<int, std::vector<CollectionActivationCfg> >::iterator it = m_collection_activation_map.find(COLLECTION_TYPE_PARTNER);
	if (it != m_collection_activation_map.end())
	{
		for (std::vector<CollectionActivationCfg>::iterator it_pet = it->second.begin(); it_pet != it->second.end(); it_pet++)
		{
			if (it_pet->id == partner_id)
			{
				return &(*it_pet);
			}
		}
	}
	return NULL;
}

const CollectionActivationCfg * CollectionConfig::GetFabaoCollectByFabaoId(int fabao_id)
{
	std::map<int, std::vector<CollectionActivationCfg> >::iterator it = m_collection_activation_map.find(COLLECTION_TYPE_FABAO);
	if (it != m_collection_activation_map.end())
	{
		for (std::vector<CollectionActivationCfg>::iterator it_pet = it->second.begin(); it_pet != it->second.end(); it_pet++)
		{
			if (it_pet->id == fabao_id)
			{
				return &(*it_pet);
			}
		}
	}
	return NULL;
}

const CollectionActivationCfg * CollectionConfig::GetHolyEquipCollectByHolyEquipId(int holy_equip_id)
{
	std::map<int, std::vector<CollectionActivationCfg> >::iterator it = m_collection_activation_map.find(COLLECTION_TYPE_HOLY_EQUIP);
	if (it != m_collection_activation_map.end())
	{
		for (std::vector<CollectionActivationCfg>::iterator it_holy_equip = it->second.begin(); it_holy_equip != it->second.end(); it_holy_equip++)
		{
			if (it_holy_equip->id == holy_equip_id)
			{
				return &(*it_holy_equip);
			}
		}
	}
	return NULL;
}

const CollectionActivationCfg * CollectionConfig::GetPetSoulCollectByPetSoulId(int pet_soul_id)
{
	std::map<int, std::vector<CollectionActivationCfg> >::iterator it = m_collection_activation_map.find(COLLECTION_TYPE_PET_SOUL);
	if (it != m_collection_activation_map.end())
	{
		for (std::vector<CollectionActivationCfg>::iterator it_cfg = it->second.begin(); it_cfg != it->second.end(); it_cfg++)
		{
			if (it_cfg->id == pet_soul_id)
			{
				return &(*it_cfg);
			}
		}
	}
	return NULL;
}

const CollectionActivationCfg * CollectionConfig::GetCollectionActivationCfg(int type, int id)
{
	std::map<int, std::vector<CollectionActivationCfg> >::iterator it = m_collection_activation_map.find(type);
	if(it == m_collection_activation_map.end()) return NULL;

	for (std::vector<CollectionActivationCfg>::iterator type_it = it->second.begin(); type_it != it->second.end(); type_it++)
	{
		if (type_it->id == id)
		{
			return &(*type_it);
		}
	}

	return NULL;
}

const std::vector<CollectionActivationCfg>* CollectionConfig::GetCollectionByType(int type)
{
	std::map<int, std::vector<CollectionActivationCfg> >::iterator it = m_collection_activation_map.find(type);
	if (it != m_collection_activation_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const CollectionTreasuresCfg * CollectionConfig::GetTreasuresCfg(int seq, int grade)
{
	std::map<int, std::map<int, CollectionTreasuresCfg> >::iterator it = treasures_map.find(seq);
	if (treasures_map.end() != it)
	{
		std::map<int, CollectionTreasuresCfg>::iterator it2 = it->second.find(grade);
		if (it->second.end() != it2)
		{
			return &it2->second;
		}
	}
	return NULL;
}

const CollectionConvertCfg * CollectionConfig::GetConvertCfg(int seq)
{
	std::map<int, CollectionConvertCfg>::const_iterator it = m_convert_map.find(seq);
	if (it == m_convert_map.end()) return NULL;
	return &it->second;
}

int CollectionConfig::InitCollectionCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	m_max_seq = -1;
	while (NULL != root_element)
	{
		int seq = -1;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0 || ++m_max_seq != seq)return -1;
		CollectionCfg &node_cfg = m_collection_map[seq];
		node_cfg.seq = seq;

		if (!GetSubNodeValue(root_element, "page", node_cfg.page))return -2;
		if (!GetSubNodeValue(root_element, "color", node_cfg.color) || node_cfg.color <= COLLECTION_COLOR_TYPE_INVALID || node_cfg.color >= COLLECTION_COLOR_TYPE_MAX)return -3;
		if (!GetSubNodeValue(root_element, "limit_prof", node_cfg.limit_prof))return -4;
		int is_back_card = 0;
		if (!GetSubNodeValue(root_element, "back_card", is_back_card))return -5;
		node_cfg.is_back_card = 0 == is_back_card ? false : true;
		if (!GetSubNodeValue(root_element, "maxhp", node_cfg.maxhp))return -15;
		if (!GetSubNodeValue(root_element, "maxmp", node_cfg.maxmp))return -25;
		if (!GetSubNodeValue(root_element, "gongji", node_cfg.gongji))return -35;
		if (!GetSubNodeValue(root_element, "fangyu", node_cfg.fangyu))return -45;
		if (!GetSubNodeValue(root_element, "minjie", node_cfg.minjie))return -55;
		if (!GetSubNodeValue(root_element, "jingshen", node_cfg.jingshen))return -65;
		if (!GetSubNodeValue(root_element, "huifu", node_cfg.hueifu))return -75;

		if (!GetSubNodeValue(root_element, "fee_num", node_cfg.fee_num) || node_cfg.fee_num < 0 || node_cfg.fee_num >= COLLECTION__FEE_MAX_NUM)return -6;

		char id_str[30] = { 0 };
		char num_str[30] = { 0 };
		for (int i = 0; i < node_cfg.fee_num && i < COLLECTION__FEE_MAX_NUM; i++)
		{
			SNPRINTF(id_str, sizeof(id_str), "collection_itemfee_%d", i + 1);
			SNPRINTF(num_str, sizeof(num_str), "collection_itemnum_%d", i + 1);
			if (!GetSubNodeValue(root_element, id_str, node_cfg.collection_itemfee_id[i]))return -100;
			if (!GetSubNodeValue(root_element, num_str, node_cfg.collection_itemfee_num[i]))return -200;
			CheckResourceCenter::Instance().GetItemValidCheck()->Add(node_cfg.collection_itemfee_id[i], node_cfg.collection_itemfee_id[i], 1, __FUNCTION__);
		}
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int CollectionConfig::InitConvertCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int seq = -1;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0)return -1;
		CollectionConvertCfg& node_cfg = m_convert_map[seq];
		node_cfg.seq = seq;
		if (!GetSubNodeValue(root_element, "convert_page", node_cfg.convert_page))return -2;
		if (!GetSubNodeValue(root_element, "convert_itemfee", node_cfg.convert_itemfee))return -3;
		if (!GetSubNodeValue(root_element, "convert_itemnum", node_cfg.convert_itemnum))return -4;
		CheckResourceCenter::Instance().GetItemValidCheck()->Add(node_cfg.convert_itemfee, node_cfg.convert_itemfee, 1, __FUNCTION__);

		TiXmlElement *item_element = root_element->FirstChildElement("get_item");
		if (NULL == item_element)return -9999;
		if (!node_cfg.get_item.ReadConfig(item_element))return -5;

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int CollectionConfig::InitCollectCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	m_max_included_seq = 0;
	while (dataElement != NULL)
	{
		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq <= 0 || seq >= 2000)return -1;
		CollectionActivationCfg & node_cfg = m_collection_activation_seq_map[seq];
		node_cfg.seq = seq;
		if (!GetSubNodeValue(dataElement, "page", node_cfg.page) || node_cfg.page < COLLECTION_TYPE_PET || node_cfg.page >= COLLECTION_TYPE_MAX)return -2;
		if (!GetSubNodeValue(dataElement, "id", node_cfg.id) || node_cfg.id < 0)return -3;
		if (!GetSubNodeValue(dataElement, "maxhp", node_cfg.maxhp) || node_cfg.maxhp < 0)return -4;
		if (!GetSubNodeValue(dataElement, "maxmp", node_cfg.maxmp) || node_cfg.maxmp < 0)return -5;
		if (!GetSubNodeValue(dataElement, "gongji", node_cfg.gongji) || node_cfg.gongji < 0)return -6;
		if (!GetSubNodeValue(dataElement, "fangyu", node_cfg.fangyu) || node_cfg.fangyu < 0)return -7;
		if (!GetSubNodeValue(dataElement, "minjie", node_cfg.minjie) || node_cfg.minjie < 0)return -8;
		if (!GetSubNodeValue(dataElement, "jingshen", node_cfg.jingshen) || node_cfg.jingshen < 0)return -9;
		if (!GetSubNodeValue(dataElement, "huifu", node_cfg.huifu) || node_cfg.huifu < 0)return -10;
		if (!GetSubNodeValue(dataElement, "bisha", node_cfg.bisha) || node_cfg.bisha < 0)return -11;
		if (!GetSubNodeValue(dataElement, "jianren", node_cfg.jianren) || node_cfg.jianren < 0)return -12;
		if (!GetSubNodeValue(dataElement, "fanji", node_cfg.fanji) || node_cfg.fanji < 0)return -13;
		if (!GetSubNodeValue(dataElement, "mingzhong", node_cfg.mingzhong) || node_cfg.mingzhong < 0)return -14;
		if (!GetSubNodeValue(dataElement, "shanduo", node_cfg.shanduo) || node_cfg.shanduo < 0)return -15;
		if (!GetSubNodeValue(dataElement, "def_zhongdu", node_cfg.def_zhongdu) || node_cfg.def_zhongdu < 0)return -16;
		if (!GetSubNodeValue(dataElement, "def_shihua", node_cfg.def_shihua) || node_cfg.def_shihua < 0)return -17;
		if (!GetSubNodeValue(dataElement, "def_hunshui", node_cfg.def_hunshui) || node_cfg.def_hunshui < 0)return -18;
		if (!GetSubNodeValue(dataElement, "def_jiuzui", node_cfg.def_jiuzui) || node_cfg.def_jiuzui < 0)return -19;
		if (!GetSubNodeValue(dataElement, "def_hunluan", node_cfg.def_hunluan) || node_cfg.def_hunluan < 0)return -20;
		if (!GetSubNodeValue(dataElement, "mogong", node_cfg.mogong) || node_cfg.mogong < 0)return -21;
		if (!GetSubNodeValue(dataElement, "mokang", node_cfg.mokang) || node_cfg.mokang < 0)return -22;
		if (!GetSubNodeValue(dataElement, "bisha_dmg", node_cfg.bisha_dmg) || node_cfg.bisha_dmg < 0)return -23;
		m_collection_activation_map[node_cfg.page].push_back(node_cfg);
		m_max_included_seq += 1;
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int CollectionConfig::InitTreasuresCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -999;
	}
	
	while (dataElement != NULL)
	{
		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq <= 0 || seq >= MAX_ATLAS_CARD_NUM)return -1;
		int grade = 0;
		if (!GetSubNodeValue(dataElement, "grade", grade) || grade <= 0)return -2;
		CollectionTreasuresCfg & node_cfg = treasures_map[seq][grade];
		node_cfg.seq = seq;
		node_cfg.grade = grade;
		
		if (!GetSubNodeValue(dataElement, "id", node_cfg.id) || node_cfg.id < 0)return -3;

		if (!GetSubNodeValue(dataElement, "attr_type_1", node_cfg.attr_type_1) || node_cfg.attr_type_1 < 0)return -4;
		if (!GetSubNodeValue(dataElement, "attr_value_1", node_cfg.attr_value_1) || node_cfg.attr_value_1 < 0)return -5;

		if (!GetSubNodeValue(dataElement, "attr_type_2", node_cfg.attr_type_2) || node_cfg.attr_type_2 < 0)return -6;
		if (!GetSubNodeValue(dataElement, "attr_value_2", node_cfg.attr_value_2) || node_cfg.attr_value_2 < 0)return -7;

		if (!GetSubNodeValue(dataElement, "attr_type_3", node_cfg.attr_type_3) || node_cfg.attr_type_3 < 0)return -8;
		if (!GetSubNodeValue(dataElement, "attr_value_3", node_cfg.attr_value_3) || node_cfg.attr_value_3 < 0)return -9;

		if (!GetSubNodeValue(dataElement, "attr_type_4", node_cfg.attr_type_4) || node_cfg.attr_type_4 < 0)return -10;
		if (!GetSubNodeValue(dataElement, "attr_value_4", node_cfg.attr_value_4) || node_cfg.attr_value_4 < 0)return -11;

		if (!GetSubNodeValue(dataElement, "consume_quantity", node_cfg.consume_quantity) || node_cfg.consume_quantity < 0)return -12;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}
