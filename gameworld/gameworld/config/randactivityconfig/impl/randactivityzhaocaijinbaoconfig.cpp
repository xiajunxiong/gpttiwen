#include "randactivityzhaocaijinbaoconfig.hpp"

#include "servercommon/struct/moneyotherdef.h"

RandActivityZhaoCaiJinBaoConfig::RandActivityZhaoCaiJinBaoConfig()
{
}

RandActivityZhaoCaiJinBaoConfig::~RandActivityZhaoCaiJinBaoConfig()
{
	for (std::map<int, std::vector<RAZhaoCaiJinBaoBuyCfg> >::iterator it = m_buy_cfg.begin(); it!= m_buy_cfg.end(); it++)
	{
		std::vector<RAZhaoCaiJinBaoBuyCfg>().swap(it->second);
	}
}

bool RandActivityZhaoCaiJinBaoConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basis_configuration", InitBasisCfg);
	LOAD_RA_CONFIG("reward_group", InitGroupCfg);
	LOAD_RA_CONFIG("buy_configuration", InitBuyCfg);
	LOAD_RA_CONFIG("top_up_reward", InitTopUpCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RAZhaoCaiJinBaoGroupMap* RandActivityZhaoCaiJinBaoConfig::GetGroupCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO);

	std::map<int, int>::const_iterator it = m_basis_cfg.upper_bound(act_real_open_day);
	if (it == m_basis_cfg.begin())
		return NULL;
	it--;

	int reward_group = it->second;
	std::map<int, RAZhaoCaiJinBaoGroupMap >::const_iterator group_it = m_group_cfg.find(reward_group);
	if (group_it == m_group_cfg.end())
		return NULL;

	return &group_it->second;
}

const std::vector<RAZhaoCaiJinBaoBuyCfg>* RandActivityZhaoCaiJinBaoConfig::GetBuyMapCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO);

	std::map<int, std::vector<RAZhaoCaiJinBaoBuyCfg> >::const_iterator it = m_buy_cfg.upper_bound(act_real_open_day);
	if (it == m_buy_cfg.begin())
		return NULL;
	it--;

	return &it->second;
}

const RAZhaoCaiJinBaoBuyCfg * RandActivityZhaoCaiJinBaoConfig::GetBuyCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO);

	std::map<int, std::vector<RAZhaoCaiJinBaoBuyCfg> >::const_iterator it = m_buy_cfg.upper_bound(act_real_open_day);
	if (it == m_buy_cfg.begin())
		return NULL;
	it--;

	if (seq < 0 || seq >= (int)it->second.size())
		return NULL;

	return &it->second[seq];
}

const RAZhaoCaiJinBaoChongZhiMap * RandActivityZhaoCaiJinBaoConfig::GetTopUpMapCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO);

	std::map<int, RAZhaoCaiJinBaoChongZhiMap >::const_iterator it = m_top_up_cfg.upper_bound(act_real_open_day);
	if (it == m_top_up_cfg.begin())
		return NULL;
	it--;

	return &it->second;
}

const RAZhaoCaiJinBaoChongZhiCfg * RandActivityZhaoCaiJinBaoConfig::GetTopUpCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO);

	std::map<int, RAZhaoCaiJinBaoChongZhiMap >::const_iterator it = m_top_up_cfg.upper_bound(act_real_open_day);
	if (it == m_top_up_cfg.begin())
		return NULL;
	it--;

	if (seq < 0 || seq >= (int)it->second.chongzhi_cfg.size())
		return NULL;

	return &it->second.chongzhi_cfg[seq];
}

int RandActivityZhaoCaiJinBaoConfig::InitBasisCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(root_element, "section_start", section_start) || section_start <= 0)
			return -1;

		int reward_group = 0;
		if (!GetSubNodeValue(root_element, "reward_group", reward_group) || reward_group <= 0)
			return -2;

		m_basis_cfg[section_start] = reward_group;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityZhaoCaiJinBaoConfig::InitGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int reward_group = 0;
		if (!GetSubNodeValue(root_element, "reward_group", reward_group) || reward_group <= 0)
			return -1;

		RAZhaoCaiJinBaoGroupCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq < 0)
			return -2;

		if (!GetSubNodeValue(root_element, "reward_type", cfg.reward_type) || cfg.reward_type < 0)
			return -3;

		if(0 == cfg.reward_type)
		{
			TiXmlElement * item_element = root_element->FirstChildElement("reward_item");
			if (NULL == item_element)
				return -1000;

			if (!cfg.reward_item.ReadConfig(item_element))
				return -1001;
		}
		else
		{
			if (!GetSubNodeValue(root_element, "gold_num", cfg.gold_num) || cfg.gold_num < 0)
				return -4;
		}

		if (!GetSubNodeValue(root_element, "select_rate", cfg.select_rate) || cfg.select_rate <= 0)
			return -5;

		if (!GetSubNodeValue(root_element, "extract_rate", cfg.extract_rate) || cfg.extract_rate <= 0)
			return -6;

		if (!GetSubNodeValue(root_element, "bao_di_id", cfg.bao_di_id)) return -61;
		if (!GetSubNodeValue(root_element, "guarantee_times", cfg.bao_di_times)) return -62;

		if (cfg.bao_di_times > 0 && (cfg.bao_di_id <= 0 || cfg.bao_di_id > BAO_DI_INFO_ITEM_MAX_NUM))
		{
			return -__LINE__;
		}

		int hearsay = 0;
		if (!GetSubNodeValue(root_element, "hearsay", hearsay) || hearsay < 0)
			return -7;

		cfg.is_hearsay = hearsay != 0;

		m_group_cfg[reward_group].MAX_RATE += cfg.select_rate;
		m_group_cfg[reward_group].group_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityZhaoCaiJinBaoConfig::InitBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(root_element, "section_start", section_start) || section_start <= 0)
			return -1;

		RAZhaoCaiJinBaoBuyCfg cfg;
		{
			TiXmlElement * list_element = root_element->FirstChildElement("reward_item_list");
			if (NULL == list_element)
				return -1000;

			TiXmlElement * item_element = list_element->FirstChildElement("reward_item");
			if (NULL == item_element)
				return -1001;

			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
					return -1002;

				cfg.reward_item.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}
		
		if (!GetSubNodeValue(root_element, "limit_type", cfg.limit_type) || cfg.limit_type <= 0)
			return -2;

		if (!GetSubNodeValue(root_element, "buy_times", cfg.buy_times) || cfg.buy_times <= 0)
			return -3;

		if (!GetSubNodeValue(root_element, "price_type", cfg.price_type) || cfg.price_type < -1 || cfg.price_type >= MONEY_TYPE_MAX)
			return -4;

		if (!GetSubNodeValue(root_element, "buy_money", cfg.buy_money) || cfg.buy_money <= 0)
			return -5;

		m_buy_cfg[section_start].push_back(cfg);

		if ((int)m_buy_cfg[section_start].size() > MAX_RA_ZHAOCAIJINBAO_BUY_NUM)
			return -6;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityZhaoCaiJinBaoConfig::InitTopUpCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(root_element, "section_start", section_start) || section_start <= 0)
			return -1;

		RAZhaoCaiJinBaoChongZhiCfg cfg;
		if (!GetSubNodeValue(root_element, "top_up_type", cfg.top_up_type) || cfg.top_up_type < 0)
			return -2;

		if (!GetSubNodeValue(root_element, "top_up_num", cfg.top_up_num) || cfg.top_up_num <= 0)
			return -3;

		{
			TiXmlElement * list_element = root_element->FirstChildElement("reward_item_list");
			if (NULL == list_element)
				return -1000;

			TiXmlElement * item_element = list_element->FirstChildElement("reward_item");
			if (NULL == item_element)
				return -1001;

			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
					return -1002;

				cfg.reward_item.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		if (!GetSubNodeValue(root_element, "get_type", cfg.get_type) || cfg.get_type <= 0)
			return -4;

		if (!GetSubNodeValue(root_element, "get_times", cfg.get_times) || cfg.get_times <= 0)
			return -5;

		m_top_up_cfg[section_start].top_up_type = cfg.top_up_type;
		m_top_up_cfg[section_start].chongzhi_cfg.push_back(cfg);

		if ((int)m_top_up_cfg[section_start].chongzhi_cfg.size() > MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM)
			return -6;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityZhaoCaiJinBaoConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(root_element, "item_id", m_other_cfg.item_id) || NULL == ITEMPOOL->GetItem(m_other_cfg.item_id))
		return -1;

	if (!GetSubNodeValue(root_element, "item_num", m_other_cfg.item_num) || m_other_cfg.item_num <= 0)
		return -2;

	if (!GetSubNodeValue(root_element, "refresh_gold", m_other_cfg.refresh_gold) || m_other_cfg.refresh_gold <= 0)
		return -3;

	return 0;
}
