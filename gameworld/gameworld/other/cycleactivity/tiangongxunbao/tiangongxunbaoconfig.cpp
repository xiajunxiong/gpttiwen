#include "tiangongxunbaoconfig.hpp"
#include "servercommon/servercommon.h"
#include "gameworld/item/itempool.h"

TianGongXunBaoConfig::TianGongXunBaoConfig()
{
}

TianGongXunBaoConfig::~TianGongXunBaoConfig()
{
	std::vector<int>().swap(m_rate_cfg);
}

bool TianGongXunBaoConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("cost", InitOtherCfg);
	LOAD_CONFIG_2("reward", InitReardGroupCfg);

	return true;
}

const TGXBRewardCfg * TianGongXunBaoConfig::GetNextReward(ARG_OUT int& cur_grid) const
{
	if (cur_grid < 0 || cur_grid >= (int)m_rate_cfg.size()) return NULL;

	{	//计算随机格点
		int total_rate = 0;
		for (int i = 1; i <= m_other_cfg.max_action_grid; ++i)
		{
			if (cur_grid + i >= (int)m_rate_cfg.size())
			{
				total_rate += m_rate_cfg[(cur_grid + i) % (int)m_rate_cfg.size()];
				continue;
			}
			total_rate += m_rate_cfg[cur_grid + i];
		}

		int rand_rate = RandomNum(total_rate);

		int accumulate_rate = 0;
		for (int i = 1; i <= m_other_cfg.max_action_grid; ++i)
		{
			if (cur_grid + i >= (int)m_rate_cfg.size())
			{
				accumulate_rate += m_rate_cfg[(cur_grid + i) % (int)m_rate_cfg.size()];
			}
			else
			{
				accumulate_rate += m_rate_cfg[cur_grid + i];
			}

			if (accumulate_rate >= rand_rate)
			{
				cur_grid = cur_grid + i >= (int)m_rate_cfg.size() ? (cur_grid + i) % (int)m_rate_cfg.size() : cur_grid + i;
				break;
			}
		}
	}

	std::map<int, TGXBRewardCfg>::const_iterator it_reward = m_reward_cfg.find(cur_grid);
	if (m_reward_cfg.end() != it_reward) return &it_reward->second;

	return NULL;
}

int TianGongXunBaoConfig::InitReardGroupCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		int seq = 0, rate = 0;
		TGXBRewardCfg reward_cfg;

		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0) return -1;
		if (!GetSubNodeValue(data_element, "weight", rate) || rate < 0) return -2;
		if (!GetSubNodeValue(data_element, "is_broadcast", reward_cfg.is_broadcast) || reward_cfg.is_broadcast < 0 || reward_cfg.is_broadcast > 1)
		{
			errormsg = STRING_SPRINTF("is_broadcast != 0 or 1");
			return -3;
		}

		TiXmlElement * reward_element = data_element->FirstChildElement("reward");
		if (NULL != reward_element)
		{
			if (!reward_cfg.reward.ReadConfig(reward_element)) return -4;
		}

		m_rate_cfg.push_back(seq);
		m_reward_cfg[seq] = reward_cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int TianGongXunBaoConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "cost", m_other_cfg.item_id)) return -1;
	if (NULL == ITEMPOOL->GetItem(m_other_cfg.item_id))
	{
		errormsg = STRING_SPRINTF("item_id[%d] not exist", m_other_cfg.item_id);
		return -__LINE__;
	}

	if (!GetSubNodeValue(data_element, "consumption_quantity", m_other_cfg.consume) || m_other_cfg.consume < 0) return -2;
	if (!GetSubNodeValue(data_element, "max_action", m_other_cfg.max_action_grid) || m_other_cfg.max_action_grid < 0) return -3;

	return 0;
}
