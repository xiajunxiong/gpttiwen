#include "servercommon/struct/battle/battle_hallow_gift_def.hpp"
#include "servercommon/servercommon.h"
#include "battle_hallow_gift_pool.hpp"
#include "servercommon/configcommon.h"

BattleHallowGiftPool::BattleHallowGiftPool()
{

}

BattleHallowGiftPool::~BattleHallowGiftPool()
{

}

BattleHallowGiftPool* g_hallow_gift_pool = NULL;
BattleHallowGiftPool& BattleHallowGiftPool::Instance()
{
	if (NULL == g_hallow_gift_pool)
	{
		g_hallow_gift_pool = new BattleHallowGiftPool();
	}
	return *g_hallow_gift_pool;
}

bool BattleHallowGiftPool::Init(const std::string &configname, std::string *err)
{
	char errinfo[1024] = { 0 };
	int iRet = 0;

	TiXmlDocument document;

	if (configname == "" || !document.LoadFile(configname + "hallow_gift.xml"))
	{
		SNPRINTF(errinfo, sizeof(errinfo), "Load BattleHallowGiftPool Error,\n %s", document.ErrorDesc());
		*err = errinfo;
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();

	if (NULL == RootElement)
	{
		*err = configname + ": xml root node error.";
		return false;
	}

	{
		// gift_list
		TiXmlElement *element = RootElement->FirstChildElement("gift_list");
		if (NULL == element)
		{
			*err = configname + ": no [gift_list].";
			return false;
		}

		iRet = InitGiftCfg(element);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitGiftCfg failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		// timing_gift
		TiXmlElement *element = RootElement->FirstChildElement("timing_gift");
		if (NULL == element)
		{
			*err = configname + ": no [timing_gift].";
			return false;
		}

		iRet = InitGiftEffectCfg(element, HALLOW_GIFT_EFFECT_TYPE_TIMING);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitGiftEffectCfg HALLOW_GIFT_EFFECT_TYPE_TIMING failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		// other_gift
		TiXmlElement *element = RootElement->FirstChildElement("other_gift");
		if (NULL == element)
		{
			*err = configname + ": no [other_gift].";
			return false;
		}

		iRet = InitGiftEffectCfg(element, HALLOW_GIFT_EFFECT_TYPE_OTHER);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitGiftEffectCfg HALLOW_GIFT_EFFECT_TYPE_OTHER failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		// add_attr_gift
		TiXmlElement *element = RootElement->FirstChildElement("add_attr_gift");
		if (NULL == element)
		{
			*err = configname + ": no [add_attr_gift].";
			return false;
		}

		iRet = InitGiftEffectCfg(element, HALLOW_GIFT_EFFECT_TYPE_ADD_ATTR);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitGiftEffectCfg HALLOW_GIFT_EFFECT_TYPE_ADD_ATTR failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	if (!CheckGiftEffectCfg(err))
	{
		return false;
	}

	return true;
}

bool BattleHallowGiftPool::Reload(const std::string &configname, std::string *err)
{
	BattleHallowGiftPool *temp = new BattleHallowGiftPool();

	if (!temp->Init(configname, err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != g_hallow_gift_pool) delete g_hallow_gift_pool;

	g_hallow_gift_pool = temp;

	return true;
}

const HallowGiftCfg* BattleHallowGiftPool::GetGiftCfg(int gift_id) const
{
	GiftCfgMap::const_iterator it = m_gift_cfg_map.find(gift_id);
	if (it != m_gift_cfg_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const HallowGiftEffectCfg* BattleHallowGiftPool::GetGiftEffectCfg(const HallowGiftEffectKey& key) const
{
	GiftEffectCfgMap::const_iterator it = m_gift_effect_cfg_map.find(key);
	if (it != m_gift_effect_cfg_map.end())
	{
		return &it->second;
	}
	return NULL;
}

bool BattleHallowGiftPool::CheckEffectType(short effect_class, short effect_type)
{
	if (HALLOW_GIFT_EFFECT_TYPE_TIMING == effect_class)
	{
		if (effect_type <= HG_TIMING_EFFECT_INVALID
			|| effect_type >= HG_TIMING_EFFECT_NUM)
		{
			return false;
		}
	}
	else if (HALLOW_GIFT_EFFECT_TYPE_OTHER == effect_class)
	{
		if (effect_type <= HG_OTHER_EFFECT_INVALID
			|| effect_type >= HG_OTHER_EFFECT_NUM)
		{
			return false;
		}
	}
	else if (HALLOW_GIFT_EFFECT_TYPE_ADD_ATTR == effect_class)
	{
		if (effect_type <= HG_ADD_ATTR_INVALID
			|| effect_type >= HG_ADD_ATTR_NUM)
		{
			return false;
		}
	}

	return true;
}

int BattleHallowGiftPool::InitGiftCfg(TiXmlElement *RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	while (dataElement)
	{
		int gift_id = 0;
		if (!GetSubNodeValue(dataElement, "gift_id", gift_id))
		{
			return -1;
		}
		if (NULL != this->GetGiftCfg(gift_id))
		{
			printf("BattleHallowGiftPool::InitGiftCfg gift_id[%d] already exist!!!\n", gift_id);
			return -2;
		}

		HallowGiftCfg cfg;
		cfg.gift_id = gift_id;

		for (int i = 0; i < 3; ++i)
		{
			char tmp1[20] = { 0 };
			char tmp2[20] = { 0 };

			SNPRINTF(tmp1, sizeof(tmp1), "effect_class_%d", i + 1);
			SNPRINTF(tmp2, sizeof(tmp2), "effect_seq_%d", i + 1);

			HallowGiftEffectKey key;
			if (GetSubNodeValue(dataElement, tmp1, key.effect_class)
				&& GetSubNodeValue(dataElement, tmp2, key.effect_seq)
				&& key.effect_class > 0
				&& key.effect_seq > 0)
			{
				cfg.gift_effects.push_back(key);
			}	
		}
		m_gift_cfg_map[gift_id] = cfg;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int BattleHallowGiftPool::InitGiftEffectCfg(TiXmlElement *RootElement, short effect_class)
{
	if (effect_class <= HALLOW_GIFT_EFFECT_TYPE_INVALID
		|| effect_class >= HALLOW_GIFT_EFFECT_TYPE_MAX)
	{
		return -1000;
	}

	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	while (dataElement)
	{
		short effect_seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", effect_seq) || effect_seq <= 0)
		{
			return -1;
		}
		static HallowGiftEffectKey key;
		key.effect_seq = effect_seq;
		key.effect_class = effect_class;

		if (m_gift_effect_cfg_map.find(key) != m_gift_effect_cfg_map.end())
		{
			printf("BattleHallowGiftPool::InitGiftEffectCfg effect duplicate, class[%d] seq[%d]",
				effect_class, effect_seq);
			return -2;
		}

		HallowGiftEffectCfg cfg;
		cfg.effect_seq = effect_seq;

		if (HALLOW_GIFT_EFFECT_TYPE_TIMING == effect_class)
		{
			if (!GetSubNodeValue(dataElement, "trigger_type", cfg.trigger_timing) 
				|| cfg.trigger_timing <= HG_TIMING_INVALID 
				|| cfg.trigger_timing >= HG_TIMING_NUM)
			{
				return -3;
			}
		}

		if (HALLOW_GIFT_EFFECT_TYPE_ADD_ATTR != effect_class)
		{
			GetSubNodeValue(dataElement, "times", cfg.limit_trigger_times);
			GetSubNodeValue(dataElement, "round_times", cfg.limit_round_trigger_times);
		}

		GetSubNodeValue(dataElement, "param1", cfg.param1);
		GetSubNodeValue(dataElement, "param2", cfg.param2);
		GetSubNodeValue(dataElement, "param3", cfg.param3);
		GetSubNodeValue(dataElement, "param4", cfg.param4);

		if (!GetSubNodeValue(dataElement, "effect_type", cfg.effect_type)
			|| !CheckEffectType(effect_class, cfg.effect_type))
		{
			return -6;
		}
		
		m_gift_effect_cfg_map[key] = cfg;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

bool BattleHallowGiftPool::CheckGiftEffectCfg(std::string *err)
{
	GiftCfgMap::iterator it = m_gift_cfg_map.begin();
	for (; it != m_gift_cfg_map.end(); ++it)
	{
		HallowGiftCfg* cfg = &it->second;
		for (size_t i = 0; i < cfg->gift_effects.size(); ++i)
		{
			if (NULL == this->GetGiftEffectCfg(cfg->gift_effects[i]))
			{
				*err = STRING_SPRINTF("CheckGiftEffectCfg class[%d] seq[%d] NOT_FOUND",
					cfg->gift_effects[i].effect_class, 
					cfg->gift_effects[i].effect_seq);
				return false;
			}
		}
	}

	return true;
}

