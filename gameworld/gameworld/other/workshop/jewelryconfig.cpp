#include "jewelryconfig.hpp"
#include "equipment/jewelry.h"
#include "item/itempool.h"
#include "item/itembase.h"

JewelryConfig::JewelryConfig()
{
}

JewelryConfig::~JewelryConfig()
{
	for (std::map<int, std::vector<CompoundRand> >::iterator it = m_coumpound_cfg.begin(); it != m_coumpound_cfg.end(); it++)
	{
		std::vector<CompoundRand>().swap(it->second);
	}

	std::vector<AnimaRandCfg>().swap(m_anima_rand);
}

bool JewelryConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	//LOAD_CONFIG("suit", InitSuitLyricsCfg);
	LOAD_CONFIG("item_group", InitCompoundGroup);
	LOAD_CONFIG("ornament_forge", InitCraftInforCfg);
	LOAD_CONFIG("quality_group",InitQualityGroupCfg);
	//LOAD_CONFIG("lingqi", InitAnimaNumbRand);

	//this->InitJewelryMap();			// 从 ITEMPOOL 拉取灵饰相关信息
	return true;
}
/*
void JewelryConfig::InsertJewelryInfo(const JewelryBasic & p_jb, const JewelryInfor & p_ji)
{
	std::map<JewelryBasic, std::vector<JewelryInfor> >::iterator f_it = m_jewelry_list.find(p_jb);
	if (m_jewelry_list.end() == f_it)	// 如果没找到
	{
		std::vector<JewelryInfor> t_v_ji; t_v_ji.push_back(p_ji);
		m_jewelry_list.insert(std::pair<JewelryBasic, std::vector<JewelryInfor> >(p_jb, t_v_ji));
	}
	else
	{
		f_it->second.push_back(p_ji);
	}
}
*/
const SuitBasicCfg * JewelryConfig::GetBasicCfgByLyricID(int lyric_id)
{
	std::map<int, SuitBasicCfg>::iterator f_it = m_basic_cfg.find(lyric_id);
	if (m_basic_cfg.end() != f_it) return & f_it->second;

	return NULL;
}

const SuitInfoTail * JewelryConfig::GetSuitInfo(int level, int color, int lyric_type)
{
	std::map<SuitInfoHead, std::set<SuitInfoTail> >::iterator f_it = m_suit_basic_cfg.find(SuitInfoHead(level, color));
	if (m_suit_basic_cfg.end() == f_it) return NULL;

	std::set<SuitInfoTail>::iterator f_s_it = f_it->second.find(SuitInfoTail(lyric_type));
	if (f_it->second.end() != f_s_it) return & *f_s_it;

	return NULL;
}

const CompoundCont * JewelryConfig::GetCompoundInfo(int list_id)
{
	std::map<int, CompoundCont>::iterator f_it = m_craft_info.find(list_id);
	if (m_craft_info.end() != f_it) return & f_it->second;
	
	return NULL;
}

const BattleAttrPair * JewelryConfig::GetLegendaryAttr(int lyric_type)
{
	std::map<int, BattleAttrPair>::iterator f_it = m_legendary_attr.find(lyric_type);
	if (m_legendary_attr.end() != f_it) return & f_it->second;

	return NULL;
}

const int JewelryConfig::RandomAJewelryID(int group_id)
{
	std::map<int, std::vector<CompoundRand> >::iterator f_it = m_coumpound_cfg.find(group_id);
	if (m_coumpound_cfg.end() == f_it) return 0;

	int rand_val = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;

	for (std::vector<CompoundRand>::iterator f_c_it = f_it->second.begin(); f_c_it != f_it->second.end(); ++f_c_it)
	{
		if (rand_val < f_c_it->rand_prob) return f_c_it->jewelry_id;
		
		rand_val -= f_c_it->rand_prob;
	}

	return 0;
}

const int JewelryConfig::RandomALyricsID(int jewelry_lv, int jewelry_color)
{
	std::map<SuitInfoHead, std::set<SuitInfoTail> >::iterator f_it = m_suit_basic_cfg.find(SuitInfoHead(jewelry_lv, jewelry_color));
	if (m_suit_basic_cfg.end() == f_it) return 0;
	
	int rand_val = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;

	for (std::set<SuitInfoTail>::iterator f_s_it = f_it->second.begin(); f_s_it != f_it->second.end(); ++f_s_it)
	{
		if (rand_val < f_s_it->rand_prob) return f_s_it->lyrics_id;
		
		rand_val -= f_s_it->rand_prob;
	}

	return 0;
}

const int JewelryConfig::RandomAnimaValue()
{
	int rand_val = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;

	for (std::vector<AnimaRandCfg>::iterator f_it = m_anima_rand.begin(); f_it != m_anima_rand.end(); ++f_it)
	{
		if (rand_val < f_it->rand_prob) return f_it->anima_val >= 0 ? f_it->anima_val : INT_MAX;

		rand_val -= f_it->rand_prob;
	}

	return 0;
}

const int JewelryConfig::SearchLegalJewelry(const JewelryBasic & s_head, int attr1, int attr2)
{
	/*std::map<JewelryBasic, std::vector<JewelryInfor> >::iterator f_it = m_jewelry_list.find(s_head);
	if (m_jewelry_list.end() == f_it) return 0;
	
	std::vector<int> t_id_list;
	for (std::vector<JewelryInfor>::iterator f_v_it = f_it->second.begin(); f_v_it != f_it->second.end(); ++f_v_it)
	{
		if (attr1 == f_v_it->attr_type || attr2 == f_v_it->attr_type) t_id_list.push_back(f_v_it->jewelry_id);
	}

	int list_size = (int)t_id_list.size();
	if (list_size <= 0) return 0;

	int rand_idx = rand() % list_size;
	return (rand_idx < list_size && rand_idx >= 0) ? t_id_list[rand_idx] : t_id_list.front();*/

	if (s_head.jewelry_type < 0 || s_head.jewelry_type >= Jewelry::J_TYPE_MAX 
		|| s_head.jewelry_level < 0 || s_head.jewelry_level >= MAX_ROLE_LEVEL + 1 
		|| s_head.jewelry_color < 0 || s_head.jewelry_color >= ItemBase::I_COLOR_MAX) return 0;

	std::vector<int> t_id_list;
	const DynamicArray<JewelryInfor> & list = m_jewelry_list[s_head.jewelry_type][s_head.jewelry_level][s_head.jewelry_color];
	int list_count = list.Count();
	for (int i = 0; i < list_count; ++i)
	{
		const JewelryInfor & info = list[i];
		if (attr1 == info.attr_type || attr2 == info.attr_type) t_id_list.push_back(info.jewelry_id);
	}

	int list_size = (int)t_id_list.size();
	if (list_size <= 0) return 0;

	int rand_idx = rand() % list_size;
	return (rand_idx < list_size && rand_idx >= 0) ? t_id_list[rand_idx] : t_id_list.front();
}

const int JewelryConfig::SearchLegalLyricID(int j_lv, int j_color, int lyric_1, int lyric_2)
{
	std::map<SuitInfoHead, std::set<SuitInfoTail> >::iterator f_it = m_suit_basic_cfg.find(SuitInfoHead(j_lv, j_color));
	if (m_suit_basic_cfg.end() == f_it) return 0;
	
	std::map<int, SuitBasicCfg>::iterator f_l_it_1 = m_basic_cfg.find(lyric_1);
	if (m_basic_cfg.end() == f_l_it_1) return 0;

	std::map<int, SuitBasicCfg>::iterator f_l_it_2 = m_basic_cfg.find(lyric_2);
	if (m_basic_cfg.end() == f_l_it_2) return 0;

	std::vector<int> t_id_list;
	for (std::set<SuitInfoTail>::iterator f_s_it = f_it->second.begin(); f_s_it != f_it->second.end(); ++f_s_it)
	{
		if (f_l_it_1->second.lyric_type == f_s_it->lyric_type || f_l_it_2->second.lyric_type == f_s_it->lyric_type) t_id_list.push_back(f_s_it->lyrics_id);
	}

	int list_size = (int)t_id_list.size();
	if (list_size <= 0) return 0;

	int rand_idx = rand() % list_size;
	return (rand_idx < list_size && rand_idx >= 0) ? t_id_list[rand_idx] : t_id_list.front();
}

const JewelryColorInfoList * JewelryConfig::GetJewelryColorListCfgByGroup(int group_id)
{
	std::map<int, JewelryColorInfoList>::iterator it = m_jewelry_color_map.find(group_id);
	if (it != m_jewelry_color_map.end())
	{
		return &it->second;
	}
	return NULL;
}

int JewelryConfig::InitSuitLyricsCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	char * attr_reader = new char[10];
	while (NULL != root_element)
	{
		SuitInfoHead t_sih; SuitInfoTail t_sit; BattleAttrPair t_bap;
		if (!GetSubNodeValue(root_element, "level", t_sih.lyrics_level) || t_sih.lyrics_level < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "quality", t_sih.lyrics_color) || t_sih.lyrics_color <= 0 || t_sih.lyrics_color >= ItemBase::I_COLOR_MAX)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "id", t_sit.lyrics_id) || t_sit.lyrics_id <= 0 || m_basic_cfg.end() != m_basic_cfg.find(t_sit.lyrics_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "suit_type", t_sit.lyric_type) || t_sit.lyric_type <= 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "rate", t_sit.rand_prob) || t_sit.rand_prob <= 0 || t_sit.rand_prob > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "legend_attr", t_bap.attr_type) || t_bap.attr_type < 0 || t_bap.attr_type >= LGP_TYPE_MAX)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "legend_percent", t_bap.attr_value) || t_bap.attr_value < 0)
		{
			return -7;
		}

		for (int t_cur = 0; t_cur < SUIT_ATTR_COUNT_MAX; ++t_cur)
		{
			BattleAttrPair & reader_bap = t_sit.attr_add[t_cur];
			sprintf(attr_reader, "param_%d", t_cur + 1);
			if (!GetSubNodeValue(root_element, attr_reader, reader_bap.attr_type) || reader_bap.attr_type < 0 
				|| (reader_bap.attr_type >= BATTLE_ATTR_MAX && reader_bap.attr_type < Jewelry::S_ATTR_MIN) 
				|| reader_bap.attr_type >= Jewelry::S_ATTR_MAX)
			{
				return -(9 + t_cur);
			}

			sprintf(attr_reader, "attr_%d", t_cur + 1);
			if (!GetSubNodeValue(root_element, attr_reader, reader_bap.attr_value) || reader_bap.attr_value < 0)
			{
				return -(9 + SUIT_ATTR_COUNT_MAX + t_cur);
			}
		}

		std::map<SuitInfoHead, std::set<SuitInfoTail> >::iterator f_it = m_suit_basic_cfg.find(t_sih);
		if (m_suit_basic_cfg.end() == f_it)
		{
			std::set<SuitInfoTail> t_set_sit; t_set_sit.insert(t_sit);
			m_suit_basic_cfg.insert(std::pair<SuitInfoHead, std::set<SuitInfoTail> >(t_sih, t_set_sit));
		}
		else
		{
			f_it->second.insert(t_sit);
		}

		SuitBasicCfg t_sbc(t_sit.lyric_type, t_sih.lyrics_level, t_sih.lyrics_color);
		m_basic_cfg.insert(std::pair<int, SuitBasicCfg>(t_sit.lyrics_id, t_sbc));

		if (LGP_TYPE_INVALID != t_bap.attr_type && m_legendary_attr.end() == m_legendary_attr.find(t_sit.lyric_type))
		{
			m_legendary_attr.insert(std::pair<int, BattleAttrPair>(t_sit.lyric_type, t_bap));
		}

		root_element = root_element->NextSiblingElement();
	}
	
	for (std::map<SuitInfoHead, std::set<SuitInfoTail> >::iterator c_it = m_suit_basic_cfg.begin(); c_it != m_suit_basic_cfg.end(); ++c_it)
	{
		int rand_total = 0;
		for (std::set<SuitInfoTail>::iterator c_s_it = c_it->second.begin(); c_s_it != c_it->second.end(); ++c_s_it)
		{
			rand_total += c_s_it->rand_prob;
		}

		if (RAND_UNIVERSAL_RATIO_BASE_NUM != rand_total) return -1;
	}

	delete[] attr_reader;
	return 0;
}

int JewelryConfig::InitCompoundGroup(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int t_g_id; CompoundRand t_cpr;
		if (!GetSubNodeValue(root_element, "id", t_g_id) || t_g_id <= 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "item_id", t_cpr.jewelry_id))
		{
			return -2;
		}

		/*const ItemBase * cfg_ib = ITEMPOOL->GetItem(t_cpr.jewelry_id);
		if (NULL == cfg_ib || I_TYPE_JEWELRY != cfg_ib->GetItemType()) return -3;*/

		if (!GetSubNodeValue(root_element, "rate", t_cpr.rand_prob) || t_cpr.rand_prob <= 0 || t_cpr.rand_prob > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -4;
		}

		std::map<int, std::vector<CompoundRand> >::iterator f_it = m_coumpound_cfg.find(t_g_id);
		if (m_coumpound_cfg.end() != f_it)
		{
			f_it->second.push_back(t_cpr);
		}
		else
		{
			std::vector<CompoundRand> t_v_cpr; t_v_cpr.push_back(t_cpr);
			m_coumpound_cfg.insert(std::pair<int, std::vector<CompoundRand> >(t_g_id, t_v_cpr));
		}

		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, std::vector<CompoundRand> >::iterator c_c_it = m_coumpound_cfg.begin(); c_c_it != m_coumpound_cfg.end(); ++c_c_it)
	{
		int rand_total = 0;
		for (std::vector<CompoundRand>::iterator c_v_it = c_c_it->second.begin(); c_v_it != c_c_it->second.end(); ++c_v_it)
		{
			rand_total += c_v_it->rand_prob;
		}

		if (RAND_UNIVERSAL_RATIO_BASE_NUM != rand_total) return -2;
	}

	return 0;
}

int JewelryConfig::InitCraftInforCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	char * mate_reader = new char[15];
	while (NULL != root_element)
	{
		int t_id; CompoundCont t_cc;
		if (!GetSubNodeValue(root_element, "seq", t_id) || t_id <= 0 || m_craft_info.end() != m_craft_info.find(t_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "level", t_cc.compound_lv) || t_cc.compound_lv < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "ornament_type", t_cc.jewelry_type) || t_cc.jewelry_type < 0 || t_cc.jewelry_type >= Jewelry::J_TYPE_MAX)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "item_random", t_cc.random_group) || m_coumpound_cfg.end() == m_coumpound_cfg.find(t_cc.random_group))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "price", t_cc.compound_pay) || t_cc.compound_pay < 0)
		{
			return -5;
		}

		for (int t_cur = 0; t_cur < CRAFT_MATE_NEED_MAX; ++t_cur)
		{
			ItemPairCon & t_ipc = t_cc.mate_list[t_cur];
			sprintf(mate_reader, "stuff_id_%d", t_cur + 1);
			if (!GetSubNodeValue(root_element, mate_reader, t_ipc.item_id) || (0 != t_ipc.item_id && NULL == ITEMPOOL->GetItem(t_ipc.item_id)))
			{
				return -(6 + t_cur);
			}

			sprintf(mate_reader, "stuff_num_%d", t_cur + 1);
			if (!GetSubNodeValue(root_element, mate_reader, t_ipc.item_num) || t_ipc.item_num < 0)
			{
				return -(6 + CRAFT_MATE_NEED_MAX + t_cur);
			}
		}

		m_craft_info.insert(std::pair<int, CompoundCont>(t_id, t_cc));
		root_element = root_element->NextSiblingElement();
	}

	delete[] mate_reader;
	return 0;
}

int JewelryConfig::InitAnimaNumbRand(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		AnimaRandCfg t_arc;
		if (!GetSubNodeValue(root_element, "rate", t_arc.rand_prob) || t_arc.rand_prob <= 0 || t_arc.rand_prob > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -1;;
		}

		if (!GetSubNodeValue(root_element, "lingqi", t_arc.anima_val))
		{
			return -2;
		}

		m_anima_rand.push_back(t_arc);
		root_element = root_element->NextSiblingElement();
	}

	int rand_total = 0;
	for (std::vector<AnimaRandCfg>::iterator c_v_it = m_anima_rand.begin(); c_v_it != m_anima_rand.end(); ++c_v_it)
	{
		rand_total += c_v_it->rand_prob;
	}

	if (RAND_UNIVERSAL_RATIO_BASE_NUM != rand_total) return -3;

	return 0;
}

int JewelryConfig::InitQualityGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int quality_id = 0;
		if (!GetSubNodeValue(root_element, "quality_id", quality_id) || quality_id <= 0 || quality_id >= INTJEWELRY_SUCC_MAX_GROUP)
		{
			return -1;;
		}

		int color = 0;
		if (!GetSubNodeValue(root_element, "color", color) || color < 0 || color >= INTJEWELRY_SUCC_MAX_LIST)
		{
			return -2;
		}

		int rate = 0;
		if (!GetSubNodeValue(root_element, "rate", rate) || rate < 0 || rate > 100)
		{
			return -3;
		}

		int fine_tuning_value = 0;
		if (!GetSubNodeValue(root_element, "fine_tuning_value", fine_tuning_value))
		{
			return -4;
		}

		int id = 0;
		if (!GetSubNodeValue(root_element, "id", id) || 0 == this->RandomAJewelryID(id))
		{
			return -5;
		}

		JewelryColorInfoList & m_color_info = m_jewelry_color_map[quality_id];
		m_color_info.jewelry_color_list[color].id = id;
		m_color_info.jewelry_color_list[color].fine_tuning_value = fine_tuning_value;
		m_color_info.jewelry_color_list[color].quality_id = quality_id;
		m_color_info.jewelry_color_list[color].rate = rate;
		m_color_info.jewelry_color_list[color].color = color;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

void JewelryConfig::InitJewelryMap()
{
	for (int item_id = 0; item_id < ItemPool::MAX_ITEM_ID; ++item_id)
	{
		const ItemBase * item = ITEMPOOL->GetItem(item_id);
		if (NULL == item || I_TYPE_JEWELRY != item->GetItemType()) continue;

		Jewelry * info = (Jewelry *)item;
		if (NULL == info) continue;

		//this->InsertJewelryInfo(JewelryBasic(info->GetJewelryType(), info->GetLimitLevel(), info->GetColor()), JewelryInfor(info->GetAttrAddType(), item_id));
		// 插入
		int type = info->GetJewelryType(), level = info->GetLimitLevel(), color = info->GetColor();
		if (type < 0 || type >= Jewelry::J_TYPE_MAX || level < 0 || level > MAX_ROLE_LEVEL || color < 0 || color >= ItemBase::I_COLOR_MAX) continue;
		
		m_jewelry_list[type][level][color].PushBack(JewelryInfor(info->GetAttrAddType(), item_id));
	}
}