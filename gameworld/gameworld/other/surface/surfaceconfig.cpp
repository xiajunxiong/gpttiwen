#include "surfaceconfig.hpp"
#include "checkresourcecenter.hpp"

SurfaceConfig::SurfaceConfig()
{
}

SurfaceConfig::~SurfaceConfig()
{
	std::vector<SurfaceExchangeCfg>().swap(m_exchange_cfg);
}

bool SurfaceConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;	
	
	LOAD_CONFIG("surface_list", InitSurfaceListCfg);
	LOAD_CONFIG("suit_list", InitSuitListCfg);
	LOAD_CONFIG("exchange_shop", InitExchangeCfg);
	LOAD_CONFIG("surface_value", InitSuitValueCfg);
	return true;
}

const SurfaceCfg * SurfaceConfig::GetSurfaceCfg(int id)
{
	std::map<int, SurfaceCfg>::const_iterator it = m_surface_map.find(id);
	if (it == m_surface_map.end())return NULL;
	return &it->second;
}

const SurfaceSuitCfg * SurfaceConfig::GetSurfaceSuitCfg(int id)
{
	std::map<int, SurfaceSuitCfg>::const_iterator it = m_surface_suit_map.find(id);
	if (it == m_surface_suit_map.end())return NULL;
	return &it->second;
}

const SurfaceExchangeCfg * SurfaceConfig::GetSurfaceExchangeCfg(int seq)
{
	if (seq < 0 || seq >= (int)m_exchange_cfg.size())
	{
		return NULL;
	}

	return &m_exchange_cfg[seq];
}

const SurfaceAttrCfg * SurfaceConfig::GetSurfaceAttrCfgByIdAndProf(int id, int prof)
{
	if(id < 0 || id >= SURFACE_MAX_NUM || prof < 0 || prof >= PROFESSION_TYPE_NUM) return NULL;

	std::map<int, SurfaceAttrValueCfg>::iterator it = m_surface_attr_value_map.find(id);
	if(it == m_surface_attr_value_map.end()) return NULL;
	
	return &it->second.prof_attr[prof];
}

int SurfaceConfig::InitSurfaceListCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int id = 0;
		if (!GetSubNodeValue(root_element, "id", id) || id < 0 || id >= SURFACE_MAX_NUM)return -1;
		if (m_surface_map.find(id) != m_surface_map.end()) return -2;

		SurfaceCfg& node_cfg = m_surface_map[id];
		node_cfg.id = id;

		if (!GetSubNodeValue(root_element, "type", node_cfg.type) || node_cfg.type < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "suit_id", node_cfg.suit_id) || node_cfg.suit_id < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "decompose_item", node_cfg.decompose_item) || node_cfg.decompose_item < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "decompose_num", node_cfg.decompose_num) || node_cfg.decompose_num < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "time", node_cfg.duration) || node_cfg.duration < 0)
		{
			return -6;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int SurfaceConfig::InitSuitListCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int id = 0;
		if (!GetSubNodeValue(root_element, "suit_id", id) || id <= 0 || id >= SURFACE_SUIT_MAX_NUM)return -1;
		if (m_surface_suit_map.find(id) != m_surface_suit_map.end()) return -2;

		SurfaceSuitCfg& node_cfg = m_surface_suit_map[id];
		node_cfg.suit_id = id;

		if (!GetSubNodeValue(root_element, "suit_num", node_cfg.suit_num) || node_cfg.suit_num < 0)return -3;

		char str[20][20] = { "maxhp","maxmp","gongji","fangyu","minjie","jingshen","huifu",
			"bisha","bisha_dmg","jianren","fanji","mingzhong","shanduo",
			"def_zhongdu","def_shihua","def_hunshui","def_jiuzui","def_hunluan",
			"mogong","mokang" };
		int attr_id[20] = {
			BATTLE_ATTR_MAX_HP,
			BATTLE_ATTR_MAX_MP,
			BATTLE_ATTR_ATTACK,
			BATTLE_ATTR_DEFENSE,
			BATTLE_ATTR_AGILITY,
			BATTLE_ATTR_MENTAL,
			BATTLE_ATTR_RECOVERY,

			BATTLE_ATTR_CRITICAL,
			BATTLE_ATTR_CRITICAL_INC_VALUE,
			BATTLE_ATTR_CRITICAL_DEC_VALUE,
			BATTLE_ATTR_COUNTER_ATTACK,
			BATTLE_ATTR_HIT,
			BATTLE_ATTR_DODGE,

			BATTLE_ATTR_ANTI_POISON,
			BATTLE_ATTR_ANTI_PETRIFY,
			BATTLE_ATTR_ANTI_FREEZE,
			BATTLE_ATTR_ANTI_DRUNK,
			BATTLE_ATTR_ANTI_CHAOS,

			BATTLE_ATTR_MAGIC_ATTACK,
			BATTLE_ATTR_MAGIC_DEFENSE,
		};

		for (int i = 0; i < 20; ++i)
		{
			if (!GetSubNodeValue(root_element, str[i], node_cfg.attrs[attr_id[i]]) || node_cfg.attrs[attr_id[i]] < 0)return -100 - i;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int SurfaceConfig::InitExchangeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -888;
	}

	int next_seq = 0;
	while (NULL != root_element)
	{
		SurfaceExchangeCfg cfg;

		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq != next_seq)
		{
			return -__LINE__;
		}
		next_seq += 1;

		if (!GetSubNodeValue(root_element, "consume_item", cfg.consume_item_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "use_universal_patch", cfg.consume_num) || cfg.consume_num <= 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "time", cfg.dealline_time))
		{
			return -__LINE__;
		}

		TiXmlElement * reward_element = root_element->FirstChildElement("exchange_item");
		if (!cfg.reward.ReadConfig(reward_element))
		{
			return -__LINE__;
		}

		m_exchange_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int SurfaceConfig::InitSuitValueCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int id = 0;
		if (!GetSubNodeValue(root_element, "surface_id", id) || id <= 0 || id >= SURFACE_MAX_NUM)return -1;

		SurfaceAttrValueCfg& node_cfg = m_surface_attr_value_map[id];

		int job = -1;
		if (!GetSubNodeValue(root_element, "job", job) || job < 0 || job >= PROFESSION_TYPE_NUM)return -2;
		SurfaceAttrCfg & attr_cfg = node_cfg.prof_attr[job];

		char str[20][20] = { "maxhp","maxmp","gongji","fangyu","minjie","jingshen","huifu",
			"bisha","shanduo","mingzhong","fanji","mokang","mogong","jianren","bisha_dmg",
			"def_zhongdu","def_shihua","def_hunshui","def_jiuzui","def_hunluan" };
		int attr_id[20] = {
			BATTLE_ATTR_MAX_HP,
			BATTLE_ATTR_MAX_MP,
			BATTLE_ATTR_ATTACK,
			BATTLE_ATTR_DEFENSE,
			BATTLE_ATTR_AGILITY,
			BATTLE_ATTR_MENTAL,
			BATTLE_ATTR_RECOVERY,

			BATTLE_ATTR_CRITICAL,
			BATTLE_ATTR_DODGE,
			BATTLE_ATTR_HIT,
			BATTLE_ATTR_COUNTER_ATTACK,
			BATTLE_ATTR_MAGIC_DEFENSE,
			BATTLE_ATTR_MAGIC_ATTACK,
			BATTLE_ATTR_CRITICAL_DEC_VALUE,
			BATTLE_ATTR_CRITICAL_INC_VALUE,

			BATTLE_ATTR_ANTI_POISON,
			BATTLE_ATTR_ANTI_PETRIFY,
			BATTLE_ATTR_ANTI_FREEZE,
			BATTLE_ATTR_ANTI_DRUNK,
			BATTLE_ATTR_ANTI_CHAOS,
		};

		for (int i = 0; i < 20; ++i)
		{
			if (!GetSubNodeValue(root_element, str[i], attr_cfg.attrs[attr_id[i]]) || attr_cfg.attrs[attr_id[i]] < 0)return -100 - i;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
