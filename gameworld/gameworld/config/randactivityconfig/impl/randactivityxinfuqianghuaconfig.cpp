#include "randactivityxinfuqianghuaconfig.hpp"
#include "other/pet/petconfig.hpp"

RandActivityXinFuQiangHuaConfig::RandActivityXinFuQiangHuaConfig()
{
}

RandActivityXinFuQiangHuaConfig::~RandActivityXinFuQiangHuaConfig()
{
}

bool RandActivityXinFuQiangHuaConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("reward", InitRewardCfg);
	LOAD_RA_CONFIG("open_sort", InitOpenCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const std::vector<ItemConfigData>* RandActivityXinFuQiangHuaConfig::GetRewardCfg(int pet_quality, int st_level, int * out_index) const
{
	if (pet_quality <= PET_QUALITY_INVALID || pet_quality >= PET_QUALITY_MAX) return NULL;
	if (st_level <= 0 || st_level > PET_STRENGTHEN_MAX_LEVEL) return NULL;

	std::map<int, STLVRewardMap>::const_iterator it = m_reward_cfg.find(pet_quality);
	if (it == m_reward_cfg.end()) return NULL;

	STLVRewardMap::const_iterator reward_it = it->second.find(st_level);
	if (reward_it != it->second.end())
	{
		if (NULL != out_index) *out_index = reward_it->second.first;
		return &reward_it->second.second;
	}	

	return NULL;
}

int RandActivityXinFuQiangHuaConfig::GetStLevelBySeq(int pet_quality, int seq) const
{
	std::map<int, STLVRewardMap>::const_iterator it = m_reward_cfg.find(pet_quality);
	if (it != m_reward_cfg.end())
	{
		STLVRewardMap::const_iterator reward_it_begin = it->second.begin();
		for (; reward_it_begin != it->second.end(); reward_it_begin++)
		{
			if (reward_it_begin->second.first == seq)
			{
				return reward_it_begin->first;
			}
		}
	}
	return -1;
}

bool RandActivityXinFuQiangHuaConfig::IsOpen(int pet_quality, int role_level) const
{
	if (pet_quality <= PET_QUALITY_INVALID || pet_quality >= PET_QUALITY_MAX) return false;
	if (role_level <= 0 || role_level > MAX_ROLE_LEVEL) return false;

	std::map<int, int>::const_iterator it = m_open_cfg.find(pet_quality);
	if (it == m_open_cfg.end()) return false;

	if (role_level >= it->second)
	{
		return true;
	}

	return false;
}

int RandActivityXinFuQiangHuaConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int pet_quality = 0;
		if (!GetSubNodeValue(data_element, "type", pet_quality) || pet_quality <= PET_QUALITY_INVALID || pet_quality >= PET_QUALITY_MAX) return -1;

		std::map<int, STLVRewardMap>::iterator it = m_reward_cfg.find(pet_quality);
		if (it == m_reward_cfg.end())
		{
			m_reward_cfg.insert(std::make_pair(pet_quality, STLVRewardMap()));
			it = m_reward_cfg.find(pet_quality);
		}
		
		STLVRewardMap& reward_map = it->second;

		int seq = -1;
		if (!GetSubNodeValue(data_element, "seq1", seq) || seq < 0 || seq >= PET_STRENGTHEN_MAX_LEVEL) return -2;

		int st_level = 0;
		if (!GetSubNodeValue(data_element, "st_level", st_level) || st_level <= 0 || st_level > PET_STRENGTHEN_MAX_LEVEL) return -3;

		std::vector<ItemConfigData> reward_list;
		TiXmlElement* reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -100;
		
		TiXmlElement* reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -101;
		
		while (reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element)) return -4;

			reward_list.push_back(item);
			reward_element = reward_element->NextSiblingElement();
		}
		if (reward_list.empty()) return -5;
		
		reward_map.insert(std::make_pair(st_level, std::make_pair(seq, reward_list)));

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityXinFuQiangHuaConfig::InitOpenCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int pet_quality = 0, role_level = 0;
		if (!GetSubNodeValue(data_element, "type", pet_quality) || pet_quality <= PET_QUALITY_INVALID || pet_quality >= PET_QUALITY_MAX) return -1;
		if (!GetSubNodeValue(data_element, "start_lv", role_level) || role_level <= 0 || role_level > MAX_ROLE_LEVEL) return -2;

		m_open_cfg[pet_quality] = role_level;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityXinFuQiangHuaConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	TiXmlElement* reward_list_element = data_element->FirstChildElement("free_reward_item_list");
	if (NULL != reward_list_element)
	{
		TiXmlElement* reward_element = reward_list_element->FirstChildElement("free_reward_item");
		if (NULL != reward_element)
		{
			while (reward_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(reward_element)) return -1;

				m_other_cfg.free_reward_list.push_back(item);

				reward_element = reward_element->NextSiblingElement();
			}
		}
	}

	return 0;
}
