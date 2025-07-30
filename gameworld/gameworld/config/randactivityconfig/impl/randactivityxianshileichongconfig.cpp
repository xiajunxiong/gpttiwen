#include "randactivityxianshileichongconfig.hpp"

#include "engineadapter.h"
#include "config/sharedconfig/sharedconfig.h"

RandActivityXianShiLeiChongConfig::RandActivityXianShiLeiChongConfig()
{

}

RandActivityXianShiLeiChongConfig::~RandActivityXianShiLeiChongConfig()
{
	for (std::map<int, std::vector<RAXianShiLeiChongCfg> >::iterator it = m_reawrd_cfg_map.begin(); it!= m_reawrd_cfg_map.end(); it++)
	{
		std::vector<RAXianShiLeiChongCfg>().swap(it->second);
	}
}


bool RandActivityXianShiLeiChongConfig::Init(TiXmlElement *RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitRewradCfg);

	return true;
}

const RAXianShiLeiChongCfg * RandActivityXianShiLeiChongConfig::GetRewardCfgBySeq(int _open_day, int _seq) const
{
	const std::vector<RAXianShiLeiChongCfg> * temp_vec = this->GetRewardCfg(_open_day);
	if (NULL == temp_vec || _seq >= temp_vec->size())
		return NULL;

	return &temp_vec->at(_seq);
}

const std::vector<RAXianShiLeiChongCfg> * RandActivityXianShiLeiChongConfig::GetRewardCfg(int _open_day) const
{
	const std::vector<RAXianShiLeiChongCfg> * temp_vec = NULL;
	std::map<int, std::vector<RAXianShiLeiChongCfg> >::const_iterator it_begin = m_reawrd_cfg_map.begin();
	std::map<int, std::vector<RAXianShiLeiChongCfg> >::const_iterator it_end = m_reawrd_cfg_map.end();

	for (; it_begin != it_end; ++it_begin)
	{
		if (_open_day < it_begin->first)
			return temp_vec;

		temp_vec = &it_begin->second;
	}

	return temp_vec;
}

int RandActivityXianShiLeiChongConfig::InitRewradCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -888;
	}

	int last_acc_price = -1 , last_start = -1;
	while (NULL != root_element)
	{
		int start_time = 0;
		if (!GetSubNodeValue(root_element, "section_start", start_time) || start_time <= 0)
		{
			return -1;
		}

		if (last_start != start_time)
		{
			last_acc_price = -1;
		}

		RAXianShiLeiChongCfg cfg;
		std::vector<RAXianShiLeiChongCfg > & temp_vec = m_reawrd_cfg_map[start_time];

		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg .seq != temp_vec.size())
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "acc_price_down", cfg.acc_price) || cfg.acc_price < 0)
		{
			return -3;
		}

		if (cfg.acc_price < last_acc_price)
			return -301;

		TiXmlElement * reward_list_element = root_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element)
			return -4;
		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element)
			return -401;
		cfg.count = 0;
		while (NULL != reward_element)
		{
			if (cfg.count >= MAX_ATTACHMENT_ITEM_NUM)
				return -402;

			if (!cfg.reward_arr[cfg.count++].ReadConfig(reward_element))
			{
				return -403;
			}

			reward_element = reward_element->NextSiblingElement();
		}

		last_start = start_time;
		last_acc_price = cfg.acc_price;
		temp_vec.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
