#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivitychunridengfengconfig.hpp"
#include "randactivityzuheheliconfig.hpp"

RandActivityZuHeHeLiConfig::RandActivityZuHeHeLiConfig()
{
}

RandActivityZuHeHeLiConfig::~RandActivityZuHeHeLiConfig()
{
	std::vector<RAZuHeHeLiItemCfg>().swap(m_item_cfg);
}

bool RandActivityZuHeHeLiConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("combination_configure", InitBuyCfg);
	LOAD_RA_CONFIG("combination_off_configure", InitDiscountCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}


const RAZuHeHeLiItemCfg * RandActivityZuHeHeLiConfig::GetRAZuHeHeLiItemCfgBySeq(RandActivityManager * ramgr, int seq) const
{
	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_ZU_HE_HE_LI);
	for (int i = 0; i < (int)m_item_cfg.size(); i++)
	{
		if (m_item_cfg[i].seq == seq && m_item_cfg[i].min_nuy_day <= open_day && m_item_cfg[i].max_buy_day >= open_day)
		{
			return &m_item_cfg[i];
		}
	}

	return NULL;
}

const RAZuHeHeLiItemCfg * RandActivityZuHeHeLiConfig::GetRAZuHeHeLiItemCfg(RandActivityManager * ramgr, int type) const
{
	if (type < 0 || type >= (int)m_item_cfg.size())
	{
		return 0;
	}

	return &m_item_cfg[type];
}


double RandActivityZuHeHeLiConfig::GetDiscount(int phase, int buy_num) const
{
	std::map<int, RAZuHeHeLiDiscountCfg> ::const_iterator iter = m_discount_cfg.find(phase);
	if (iter == m_discount_cfg.end())
	{
		return 1.0;
	}

	std::map<int, int> ::const_iterator discount_iter = iter->second.num_to_discount.find(buy_num);
	if (discount_iter == iter->second.num_to_discount.end())
	{
		return 1.0;
	}

	return discount_iter->second / 100.0;
}

int RandActivityZuHeHeLiConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityZuHeHeLiConfig::InitDiscountCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{

		int phase = 0;
		if (!GetSubNodeValue(data_element, "phase", phase))
		{
			return -__LINE__;
		}

		RAZuHeHeLiDiscountCfg & cfg = m_discount_cfg[phase];

		int num = 0;
		if (!GetSubNodeValue(data_element, "gift_num", num))
		{
			return -__LINE__;
		}

		int discount = 0;
		if (!GetSubNodeValue(data_element, "discount", discount))
		{
			return -__LINE__;
		}

		cfg.num_to_discount[num] = discount;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityZuHeHeLiConfig::InitBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int next_type = 0;
	int next_seq = 0;
	int last_phase = 0;
	while (NULL != data_element)
	{
		RAZuHeHeLiItemCfg item_cfg;
		if (!GetSubNodeValue(data_element, "section_start", item_cfg.min_nuy_day))return -__LINE__;
		if (!GetSubNodeValue(data_element, "section_end", item_cfg.max_buy_day) || item_cfg.max_buy_day < item_cfg.min_nuy_day)return -__LINE__;
		if (!GetSubNodeValue(data_element, "type", item_cfg.type) || item_cfg.type != next_type || item_cfg.type >= RA_ZU_HE_HE_LI_MAX_ITEM) return -__LINE__;
		if (!GetSubNodeValue(data_element, "limit_type", item_cfg.limit_type)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "buy_times", item_cfg.limit_buy_times) || item_cfg.limit_buy_times < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "today_times", item_cfg.limit_buy_times_day) || item_cfg.limit_buy_times_day < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "price", item_cfg.need_chong_zhi_gold) || item_cfg.need_chong_zhi_gold < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "phase", item_cfg.phase) || item_cfg.phase <= 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "price_type", item_cfg.is_chong_zhi)) return -__LINE__;
		if (0 == last_phase)
		{
			last_phase = item_cfg.phase;
		}
		else if (last_phase != item_cfg.phase)
		{
			next_seq = 0;
			last_phase = item_cfg.phase;
		}
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq != next_seq || item_cfg.seq >= RA_ZU_HE_HE_LI_MAX_ITEM) 
		{
			return -__LINE__;
		}
		TiXmlElement * item_element = data_element->FirstChildElement("reward_item");
		if (!item_cfg.reward.ReadConfig(item_element))
		{
			return -__LINE__;
		}

		next_type += 1;
		next_seq += 1;
		m_item_cfg.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityZuHeHeLiConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver))return -__LINE__;

	return 0;
}
