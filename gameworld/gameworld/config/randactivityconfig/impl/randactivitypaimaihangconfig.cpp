#include "randactivitypaimaihangconfig.hpp"
#include "servercommon/roleactivity/rapaimaihangparam.hpp"

RandActivityPaiMaiHangConfig::RandActivityPaiMaiHangConfig()
{
}

RandActivityPaiMaiHangConfig::~RandActivityPaiMaiHangConfig()
{
}

bool RandActivityPaiMaiHangConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("gift_configure", InitShopCfg);

	return true;
}

const RAPaiMaiHangShopCfg * RandActivityPaiMaiHangConfig::GetShopCfg(RandActivityManager * ramgr, int seq, int activity_open_day) const
{
	if(NULL == ramgr || seq < 0) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_PAI_MAI_HANG);
	for (std::map<long long, std::map<int, RAPaiMaiHangShopCfg> >::const_iterator it = m_shop_cfg.begin(); it != m_shop_cfg.end(); ++it)
	{
		int section_start = 0, section_end = 0;
		this->GetSectionByKey(it->first, &section_start, &section_end);
		if (section_start <= act_real_open_day && section_end >= act_real_open_day)
		{
			int day_seq_key = this->GetDaySeqKey(activity_open_day, seq);
			std::map<int, RAPaiMaiHangShopCfg>::const_iterator day_seq_it = it->second.find(day_seq_key);
			if (day_seq_it != it->second.end())
			{
				return &day_seq_it->second;
			}
		}
	}

	return NULL;
}

int RandActivityPaiMaiHangConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "auction_time", m_other_cfg.auction_time)) return -1;
	if (!GetSubNodeValue(data_element, "end_time", m_other_cfg.end_time)) return -2;
	if (!GetSubNodeValue(data_element, "latest_end_time", m_other_cfg.last_end_time)) return -3;
	if (!GetSubNodeValue(data_element, "delay_time", m_other_cfg.delay_time)) return -4;
	if (!GetSubNodeValue(data_element, "pre_price", m_other_cfg.pre_price) || m_other_cfg.pre_price <= 0) return -5;

	return 0;
}

int RandActivityPaiMaiHangConfig::InitShopCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0) return -1;
		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0) return -2;
		if (section_end == 0)
		{
			section_end = MAX_INT;
		}
		if(section_start > section_end) return -3;
		long long section_key = this->GetSectionKey(section_start, section_end);

		RAPaiMaiHangShopCfg node_cfg;
		if (!GetSubNodeValue(data_element, "seq", node_cfg.seq) || node_cfg.seq < 0 || node_cfg.seq >= MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_NUM) return -4;
		if (!GetSubNodeValue(data_element, "activity_day", node_cfg.activity_day) || node_cfg.activity_day < 0) return -5;

		int day_seq_key = this->GetDaySeqKey(node_cfg.activity_day, node_cfg.seq);
		std::map<long long, std::map<int, RAPaiMaiHangShopCfg> >::iterator it = m_shop_cfg.find(section_key);
		if (it != m_shop_cfg.end() && it->second.find(day_seq_key) != it->second.end())
		{
			return -6;
		}
		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element)return -7;

		if (!node_cfg.item.ReadConfig(reward_element))
		{
			return -8;
		}
		if (!GetSubNodeValue(data_element, "base_price", node_cfg.base_price) || node_cfg.base_price < 0) return -9;

		m_shop_cfg[section_key][day_seq_key] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

long long RandActivityPaiMaiHangConfig::GetSectionKey(int section_start, int section_end)
{
	RAPaiMaiHangKey uniuqe_key;
	uniuqe_key.param[0] = section_start;
	uniuqe_key.param[1] = section_end;
	return uniuqe_key.unique_key;
}

void RandActivityPaiMaiHangConfig::GetSectionByKey(long long section_key, int * section_start, int * section_end)const
{
	if(NULL == section_start || NULL == section_end) return;
	RAPaiMaiHangKey uniuqe_key;
	uniuqe_key.unique_key = section_key;
	*section_start = uniuqe_key.param[0];
	*section_end = uniuqe_key.param[1];
}

int RandActivityPaiMaiHangConfig::GetDaySeqKey(int activity_day, int seq)const
{
	return activity_day * PAI_MAI_HANG_DAY_KEY_BASE_NUM + seq;
}
