#include "randactivitysaltysweetbattleconfig.hpp"
#include "item/itempool.h"
#include <algorithm>
#include "servercommon/roleactivity/razongxiangparam.hpp"

RandActivitySaltySweetBattleConfig::RandActivitySaltySweetBattleConfig()
{
}

RandActivitySaltySweetBattleConfig::~RandActivitySaltySweetBattleConfig()
{
	std::vector<SaltySweetBaseCfg>().swap(base_cfg);
	std::vector<SaltySweetVoteCfg>().swap(vote_cfg);
}

bool RandActivitySaltySweetBattleConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basis_configuration", InitBaseCfg);
	LOAD_RA_CONFIG("vote_configuration", InitVoteCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);
	return true;
}

const SaltySweetBaseCfg * RandActivitySaltySweetBattleConfig::GetBaseCfg(int open_day) const
{
	for (std::vector<SaltySweetBaseCfg>::const_iterator it = base_cfg.begin(); it != base_cfg.end(); it++)
	{
		if (it->section_start <= open_day && it->section_end >= open_day)
		{
			return &(*it);
		}
	}
	return NULL;
}

const SaltySweetVoteCfg * RandActivitySaltySweetBattleConfig::GetVoteCfg(int group_id) const
{
	std::vector<const SaltySweetVoteCfg*> rand_vec;
	int rand_value = 0;
	for (std::vector<SaltySweetVoteCfg>::const_iterator it = vote_cfg.begin(); it != vote_cfg.end(); it++)
	{
		if (it->group == group_id)
		{
			rand_vec.push_back(&(*it));
			rand_value += it->rate;
		}
	}
	int weight = rand() % rand_value;
	for (std::vector<const SaltySweetVoteCfg*>::iterator it = rand_vec.begin(); it != rand_vec.end(); it++)
	{
		if ((*it)->rate >= weight)
		{
			return (*it);
		}
		weight -= (*it)->rate;
	}

	return NULL;
}

int RandActivitySaltySweetBattleConfig::InitBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		SaltySweetBaseCfg node;

		if (!GetSubNodeValue(data_element, "type", node.type) || node.type < 0) return -1;
		if (!GetSubNodeValue(data_element, "section_start", node.section_start)) return -2;
		if (!GetSubNodeValue(data_element, "section_end", node.section_end)) return -3;
		if (!GetSubNodeValue(data_element, "vote_reward_group", node.vote_reward_group))return -4;
		if (node.section_end == 0)
		{
			node.section_end = MAX_INT;
		}
		
		TiXmlElement * suc_list_element = data_element->FirstChildElement("suc_reward_item_list");
		if (NULL != suc_list_element)
		{
			TiXmlElement * suc_reward_element = suc_list_element->FirstChildElement("suc_reward_item");
			while (NULL != suc_reward_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(suc_reward_element)) return -5;

				node.win_reward_vec.push_back(item);
				suc_reward_element = suc_reward_element->NextSiblingElement();
			}
		}

		TiXmlElement * fail_list_element = data_element->FirstChildElement("fail_reward_item_list");
		if (NULL != fail_list_element)
		{
			TiXmlElement * fail_reward_element = fail_list_element->FirstChildElement("fail_reward_item");
			while (NULL != fail_reward_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(fail_reward_element)) return -5;

				node.lose_reward_vec.push_back(item);
				fail_reward_element = fail_reward_element->NextSiblingElement();
			}
		}

		base_cfg.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivitySaltySweetBattleConfig::InitVoteCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		SaltySweetVoteCfg node;

		if (!GetSubNodeValue(data_element, "seq", node.seq) || node.seq < 0) return -1;
		if (!GetSubNodeValue(data_element, "group", node.group)) return -2;
		if (!GetSubNodeValue(data_element, "rate", node.rate)) return -3;

		TiXmlElement * reward_element = data_element->FirstChildElement("vote_reward_item");
		if (NULL != reward_element)
		{
			if (!node.reward.ReadConfig(reward_element)) return -5;
		}

		vote_cfg.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivitySaltySweetBattleConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "times", other_cfg.times) || other_cfg.times < 0) return -1;
	if (!GetSubNodeValue(data_element, "salty_add_vote", other_cfg.salty_add_vote)) return -2;
	if (!GetSubNodeValue(data_element, "sweet_add_vote", other_cfg.sweet_add_vote)) return -3;
	if (!GetSubNodeValue(data_element, "consume_item", other_cfg.consume_item) || NULL  == ITEMPOOL->GetItem(other_cfg.consume_item)) return -4;
	if (!GetSubNodeValue(data_element, "vote_time_end", other_cfg.vote_time_end)) return -5;
	if (!GetSubNodeValue(data_element, "settlement_times", other_cfg.settlement_times)) return -6;

	return 0;
}