#include "chongzhiconfig.hpp"
#include "globalconfig/spidconfig.h"

ChongzhiConfig::ChongzhiConfig() : m_dev_spid(0)
{

}

ChongzhiConfig::~ChongzhiConfig()
{
	std::vector<ChongzhiFanliCfg>().swap(m_fanli_cfg_list);
}

bool ChongzhiConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	{
		// 计算dev的spid
		std::string dev_str = "dev";
		m_dev_spid = globalconfig::SpidConfig::GetSpidByStrPlatName(dev_str);
	}

	LOAD_CONFIG("rebate_recharge", InitFanliCfg);
	LOAD_CONFIG("spid_list", InitSpidChongzhiCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("first_recharge", InitFirstRechargeCfg);
	LOAD_CONFIG("first_recharge_exclusive", InitFirstRechargeExclusiveCfg);
	LOAD_CONFIG("activity_list", InitFirstRechargeActivityCfg);

	{
		for (int i = 0; i < NEW_CHONGZHI_SHEET_COUNT; ++i)
		{
			char reward_str[20] = { 0 };
			sprintf(reward_str, "%s_%d", "reward", i);

			//充值奖励配置
			TiXmlElement *rwd_element = RootElement->FirstChildElement(reward_str);
			if (NULL == rwd_element)
			{
				sprintf(errinfo, "%s: no [%s].", configname.c_str(), reward_str);
				*err = errinfo;
				return false;
			}

			iRet = this->InitChongzhiCfg(rwd_element, &m_sheet_chongzhi_cfg_list[i].reward_count, m_sheet_chongzhi_cfg_list[i].reward_list);
			if (iRet)
			{
				sprintf(errinfo, "%s: InitNewChongzhiCfg failed %d", configname.c_str(), iRet);
				*err = errinfo;
				return false;
			}
		}
		m_new_chongzhi_sheet_count = NEW_CHONGZHI_SHEET_COUNT;
	}

	return true;
}

int ChongzhiConfig::InitChongzhiCfg(TiXmlElement *RootElement, int* reward_count, ChongzhiReward* reward_list)
{
	*reward_count = 0;

	int last_chongzhi = 0;

	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	while (NULL != dataElement)
	{
		if ((*reward_count) >= CHONGZHI_REWARD_CONFIG_MAX_NUM)
		{
			break;
		}

		ChongzhiReward cfg;

		if (!GetSubNodeValue(dataElement, "seq", cfg.seq) || cfg.seq != (*reward_count))
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "chongzhi", cfg.chongzhi))
		{
			return -2;
		}
		last_chongzhi = cfg.chongzhi;

		if (!GetSubNodeValue(dataElement, "first_reward_num", cfg.reward_coin) || cfg.reward_coin < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "reward_type", cfg.reward_type) || cfg.reward_type < CHONGZHI_EXTRA_REWARD_TYPE_INVALID || cfg.reward_type >= CHONGZHI_EXTRA_REWARD_TYPE_MAX)
		{
			return -6;
		}

		double tmp_money = 0.0;
		if (GetSubNodeValue(dataElement, "rmb_show", tmp_money))
		{
			if (tmp_money < 0)
			{
				return -7;
			}
		}
		cfg.money_show[CHONGZHI_CURRENCY_TYPE_RMB] = (int)(tmp_money * 100);

		if (GetSubNodeValue(dataElement, "twd_show", tmp_money))
		{
			if (tmp_money < 0)
			{
				return -8;
			}
		}
		cfg.money_show[CHONGZHI_CURRENCY_TYPE_TWD] = (int)(tmp_money * 100);

		if (GetSubNodeValue(dataElement, "dollar_show", tmp_money))
		{
			if (tmp_money < 0)
			{
				return -9;
			}
		}
		cfg.money_show[CHONGZHI_CURRENCY_TYPE_USD] = (int)(tmp_money * 100);

		if (GetSubNodeValue(dataElement, "kr_show", tmp_money))
		{
			if (tmp_money < 0)
			{
				return -9;
			}
		}
		cfg.money_show[CHONGZHI_CURRENCY_TYPE_KRW] = (int)(tmp_money * 100);

		if (GetSubNodeValue(dataElement, "vn_show", tmp_money))
		{
			if (tmp_money < 0)
			{
				return -9;
			}
		}
		cfg.money_show[CHONGZHI_CURRENCY_TYPE_VND] = (int)(tmp_money * 100);

		{
			std::string description_str;
			if (GetSubNodeValue(dataElement, "discretion", description_str))
			{
				if ((int)description_str.length() >= DESCRIPTION_STR_LEN)
				{
					return -12;
				}
				memcpy(cfg.description_first_chongzhi, description_str.c_str(), (int)description_str.length());
				cfg.description_first_chongzhi[description_str.length()] = '\0';
			}
		}

		if (cfg.reward_type != CHONGZHI_EXTRA_REWARD_TYPE_MONTH_CARD && 
			cfg.reward_type != CHONGZHI_EXTRA_REWARD_TYPE_NEW_MONTH_CARD &&
			cfg.reward_coin == 0)
		{
			return -100;
		}

		reward_list[*reward_count] = cfg;
		++(*reward_count);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ChongzhiConfig::InitOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

// 	if (!GetSubNodeValue(data_element, "first_recharge_extra_bind_gold_times", m_other_cfg.first_recharge_extra_bind_gold_times) || m_other_cfg.first_recharge_extra_bind_gold_times < 0)
// 	{
// 		return -2;
// 	}
// 
// 	if (!GetSubNodeValue(data_element, "daily_first_recharge_extra_reward_need_times", m_other_cfg.daily_first_recharge_extra_reward_need_times) || m_other_cfg.daily_first_recharge_extra_reward_need_times <= 0)
// 	{
// 		return -3;
// 	}
// 
// 	TiXmlElement *first_recharge_extra_reward_item_element = data_element->FirstChildElement("daily_first_recharge_extra_reward_item");
// 	if (NULL == first_recharge_extra_reward_item_element)
// 	{
// 		return -4;
// 	}
// 
// 	if (!m_other_cfg.daily_first_recharge_extra_reward_item.ReadConfig(first_recharge_extra_reward_item_element))
// 	{
// 		return -5;
// 	}
// 
// 	if (!GetSubNodeValue(data_element, "zai_chongzhi_need_chongzhi", m_other_cfg.zai_chongzhi_need_chongzhi) || m_other_cfg.zai_chongzhi_need_chongzhi <= 0)
// 	{
// 		return -6;
// 	}
// 
// 	TiXmlElement *zai_chongzhi_reward_item_element = data_element->FirstChildElement("zai_chongzhi_reward_item");
// 	if (NULL == zai_chongzhi_reward_item_element)
// 	{
// 		return -7;
// 	}
// 
// 	if (!m_other_cfg.zai_chongzhi_reward_item.ReadConfig(zai_chongzhi_reward_item_element))
// 	{
// 		return -8;
// 	}
// 
// 	if (!GetSubNodeValue(data_element, "san_chongzhi_need_chongzhi", m_other_cfg.san_chongzhi_need_chongzhi) || m_other_cfg.san_chongzhi_need_chongzhi <= 0)
// 	{
// 		return -9;
// 	}
// 
// 	TiXmlElement *san_chongzhi_reward_item_element = data_element->FirstChildElement("san_chongzhi_reward_item");
// 	if (NULL == san_chongzhi_reward_item_element)
// 	{
// 		return -10;
// 	}
// 
// 	if (!m_other_cfg.san_chongzhi_reward_item.ReadConfig(san_chongzhi_reward_item_element))
// 	{
// 		return -11;
// 	}

	if (!GetSubNodeValue(data_element, "extra_reward_effective_day", m_other_cfg.extra_reward_effective_day) || m_other_cfg.extra_reward_effective_day < 0)
	{
		return -9;
	}

	if (!GetSubNodeValue(data_element, "activity_day", m_other_cfg.first_recharge_grow_day) || m_other_cfg.first_recharge_grow_day <= 0)
	{
		return -10;
	}

	if (!GetSubNodeValue(data_element, "buy_money", m_other_cfg.buy_money) || m_other_cfg.buy_money <= 0)
	{
		return -11;
	}

	TiXmlElement *buy_reward_item_list = data_element->FirstChildElement("buy_reward_item_list");
	if (NULL == buy_reward_item_list)
	{
		return -12;
	}
	TiXmlElement * buy_reward_item_element = buy_reward_item_list->FirstChildElement("buy_reward_item");
	if (NULL == buy_reward_item_element)
	{
		return -13;
	}
	while (NULL != buy_reward_item_element)
	{
		ItemConfigData item;
		if (!item.ReadConfig(buy_reward_item_element))
		{
			return -14;
		}

		m_other_cfg.buy_reward_list.push_back(item);
		buy_reward_item_element = buy_reward_item_element->NextSiblingElement();
	}

	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver) || m_other_cfg.cfg_ver < 0)
	{
		return -15;
	}

	return 0;
}


const ChongzhiFanliCfg* ChongzhiConfig::GetFanliCfg(int seq, int level)
{
	if(seq < 0 || seq >= (int)m_fanli_cfg_list.size()) return NULL;

	if (m_fanli_cfg_list[seq].seq != seq || m_fanli_cfg_list[seq].min_level > level || m_fanli_cfg_list[seq].max_level < level) return NULL;

	return &m_fanli_cfg_list[seq];
}

int ChongzhiConfig::InitFanliCfg(TiXmlElement *RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");

	int last_seq = -1;
	while (dataElement)
	{
		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq != last_seq + 1)
		{
			return -1;
		}
		last_seq = seq;

		ChongzhiFanliCfg node_cfg;
		node_cfg.seq = seq;

		if (!GetSubNodeValue(dataElement, "gold", node_cfg.gold) || node_cfg.gold <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "start_level", node_cfg.min_level) || node_cfg.min_level <= 0 || node_cfg.min_level > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "end_level", node_cfg.max_level) || node_cfg.max_level < 0 || node_cfg.max_level > MAX_ROLE_LEVEL)
		{
			return -4;
		}
		if (node_cfg.max_level == 0)
		{
			node_cfg.max_level = MAX_ROLE_LEVEL;
		}

		TiXmlElement *item_element_list = dataElement->FirstChildElement("reward_item_list");
		if (NULL == item_element_list)
		{
			return -3;
		}
		TiXmlElement *item_element = item_element_list->FirstChildElement("reward_item");
		while (item_element)
		{
			if (!node_cfg.reward_list[node_cfg.count].ReadConfig(item_element))
			{
				return -4;
			}
			node_cfg.count += 1;
			item_element = item_element->NextSiblingElement();
		}

		if (node_cfg.count <= 0)
		{
			return -5;
		}
		
		m_fanli_cfg_list.push_back(node_cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ChongzhiConfig::InitSpidChongzhiCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		std::string spid_name;
		if (!GetSubNodeValue(data_element, "spid", spid_name))
		{
			return -1;
		}
		int spid = globalconfig::SpidConfig::GetSpidByStrPlatName(spid_name);

		int sheet = 0;
		if (!GetSubNodeValue(data_element, "sheet", sheet) || sheet < 0 || sheet >= NEW_CHONGZHI_SHEET_COUNT)
		{
			return -3;
		}

		int currency = 0;
		if (!GetSubNodeValue(data_element, "type", currency) || currency < 0 || currency >= CHONGZHI_CURRENCY_TYPE_MAX)
		{
			return -4;
		}

		std::string double_spidname;
		int double_spid = 0;
		if (GetSubNodeValue(data_element, "double_spid", double_spidname))
		{
			double_spid = globalconfig::SpidConfig::GetSpidByStrPlatName(double_spidname);
		}

		SpidChongzhiCfgMap::iterator it = m_spid_chongzhi_cfg_map.find(spid);
		if (it == m_spid_chongzhi_cfg_map.end())
		{
			SpidChongzhiCfg cfg;
			it = m_spid_chongzhi_cfg_map.insert(SpidChongzhiCfgMap::value_type(spid, cfg)).first;
		}
		it->second.spid = spid;
		it->second.sheet[currency] = sheet;
		it->second.double_spid = double_spid;

		for (int i = 0; i < MAX_CHONG_ZHI_SHOW_ID_NUM; ++i)
		{
			std::string show_str = STRING_SPRINTF("show_id_%d", i);

			if (!GetSubNodeValue(data_element, show_str, it->second.show_id_list[currency][i]))
			{
				return -(10 + i);
			}
			if (it->second.show_id_list[currency][i] <= 0)
			{
				if (0 == i)
				{
					return -(20 + i);
				}
				else
				{
					break;
				}
			}
			++it->second.show_count;
		}

		data_element = data_element->NextSiblingElement();
	}

	if (0 == m_dev_spid || m_spid_chongzhi_cfg_map.find(m_dev_spid) == m_spid_chongzhi_cfg_map.end())
	{
		return -100;
	}

	return 0;
}

int ChongzhiConfig::InitFirstRechargeCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}
	
	while (data_element != NULL)
	{
		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < FRIST_RECHARGE_TYPE_YIXIANER || seq >= FRIST_RECHARGE_TYPE_MAX)
		{
			return -1;
		}
		if (m_frist_recharge_cfg.find(seq) != m_frist_recharge_cfg.end())
		{
			return -2;
		}
		FristRechargeCfg &node_cfg = m_frist_recharge_cfg[seq];
		if (!GetSubNodeValue(data_element, "gold", node_cfg.need_gold) || node_cfg.need_gold <= 0)
		{
			return -3;
		}
		TiXmlElement *reward_element = data_element->FirstChildElement("reward_item");
		if (reward_element == NULL)
		{
			return -4;
		}
		
		if (!node_cfg.reward.ReadConfig(reward_element))
		{
			return -5;
		}
	
		TiXmlElement * sec_reward_item_element = data_element->FirstChildElement("sec_reward_item");
		if (NULL == sec_reward_item_element)
		{
			return -6;
		}
		if (!node_cfg.day_reward_list[0].ReadConfig(sec_reward_item_element))
		{
			return -7;
		}
	
		TiXmlElement * thr_reward_item_element = data_element->FirstChildElement("thr_reward_item");
		if (NULL == thr_reward_item_element)
		{
			return -8;
		}
		if (!node_cfg.day_reward_list[1].ReadConfig(thr_reward_item_element))
		{
			return -9;
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ChongzhiConfig::InitFirstRechargeExclusiveCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (data_element != NULL)
	{
		int type = 0;
		if (!GetSubNodeValue(data_element, "type", type) || type < 0 || type >= MAX_FIRST_RECHARGE_EXCLUSIVE_NUM) return -1;

		if (m_first_recharge_exclusive_cfg.end() != m_first_recharge_exclusive_cfg.find(type))
		{
			return -2;
		}
		FirstRechargeExclusiveCfg node_cfg;
		if (!GetSubNodeValue(data_element, "activity_type", node_cfg.activity_type) || node_cfg.activity_type <= FIRST_RECHARGE_EXCLUSIVE_TYPE_INVAILD || 
			node_cfg.activity_type >= FIRST_RECHARGE_EXCLUSIVE_TYPE_MAX) return -3;

		if (!GetSubNodeValue(data_element, "parameter", node_cfg.param1) || node_cfg.param1 <= 0) return -4;

		TiXmlElement *reward_item_list = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_item_list)
		{
			return -100;
		}
		TiXmlElement * reward_item_element = reward_item_list->FirstChildElement("reward_item");
		if (NULL == reward_item_element)
		{
			return -101;
		}
		while (NULL != reward_item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_item_element))
			{
				return -102;
			}

			node_cfg.reward_list.push_back(item);
			reward_item_element = reward_item_element->NextSiblingElement();
		}

		m_first_recharge_exclusive_cfg[type] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ChongzhiConfig::InitFirstRechargeActivityCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (data_element != NULL)
	{
		int activity_type = 0;
		if (!GetSubNodeValue(data_element, "activity_type", activity_type) || activity_type <= ACTIVITY_TYPE_INVALID || activity_type >= ACTIVITY_TYPE_MAX) return -1;

		if (m_first_recharge_activity_list.end() != m_first_recharge_activity_list.find(activity_type))
		{
			return -2;
		}
		m_first_recharge_activity_list.insert(activity_type);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

void ChongzhiConfig::GetNewChongzhiConfig(int spid, int currency, unsigned int chong_zhi_flag, int* chongzhi_reward_count, 
	const ChongzhiReward* chongzhi_reward_list[CHONGZHI_REWARD_CONFIG_MAX_NUM])
{
	if (currency < 0 || currency >= CHONGZHI_CURRENCY_TYPE_MAX)
	{
		return;
	}

	SpidChongzhiCfgMap::iterator it = m_spid_chongzhi_cfg_map.find(spid);
	if (it == m_spid_chongzhi_cfg_map.end())
	{
		it = m_spid_chongzhi_cfg_map.find(m_dev_spid);
		if (it == m_spid_chongzhi_cfg_map.end())
		{
			return; //不应该走到这里来
		}
	}

	int show_binary = it->second.GetShowBinary(currency, chong_zhi_flag);
	int sheet = it->second.sheet[currency];
	if (sheet < 0 || sheet >= m_new_chongzhi_sheet_count || sheet >= NEW_CHONGZHI_SHEET_COUNT)
	{
		return; //参数错误
	}

	UNSTD_STATIC_CHECK(CHONGZHI_REWARD_CONFIG_MAX_NUM <= (int)sizeof(show_binary) * 8);
	for (int i = 0; i < m_sheet_chongzhi_cfg_list[sheet].reward_count; ++i)
	{
		if ((show_binary & (1 << i)) != 0)
		{
			chongzhi_reward_list[(*chongzhi_reward_count)++] = &m_sheet_chongzhi_cfg_list[sheet].reward_list[i];
		}
	}
}

const ChongzhiReward * ChongzhiConfig::GetExtraRewardByNum(int chongzhi, int currency, int spid)
{
	if (currency < 0 || currency >= CHONGZHI_CURRENCY_TYPE_MAX)
	{
		return NULL;
	}

	SpidChongzhiCfgMap::iterator it = m_spid_chongzhi_cfg_map.find(spid);
	if (it == m_spid_chongzhi_cfg_map.end())
	{
		it = m_spid_chongzhi_cfg_map.find(m_dev_spid);
		if (it == m_spid_chongzhi_cfg_map.end())
		{
			return NULL; //不应该走到这里来
		}
	}

//	int show_binary = it->second.GetShowBinary();
	int sheet = it->second.sheet[currency];
	if (sheet < 0 || sheet >= m_new_chongzhi_sheet_count || sheet >= NEW_CHONGZHI_SHEET_COUNT)
	{
		return NULL; //参数错误
	}

//	UNSTD_STATIC_CHECK(CHONGZHI_REWARD_CONFIG_MAX_NUM <= (int)sizeof(show_binary) * 8);
	for (int i = 0; i < m_sheet_chongzhi_cfg_list[sheet].reward_count; ++i)
	{
		if (/*(show_binary & (1 << i)) != 0 && */ m_sheet_chongzhi_cfg_list[sheet].reward_list[i].chongzhi == chongzhi)
		{
			return &m_sheet_chongzhi_cfg_list[sheet].reward_list[i];
		}
	}

	return NULL;
}

const ChongzhiReward * ChongzhiConfig::GetExtraRewardBySeq(int seq, int currency, int spid)
{
	if (currency < 0 || currency >= CHONGZHI_CURRENCY_TYPE_MAX)
	{
		return NULL;
	}

	SpidChongzhiCfgMap::iterator it = m_spid_chongzhi_cfg_map.find(spid);
	if (it == m_spid_chongzhi_cfg_map.end())
	{
		it = m_spid_chongzhi_cfg_map.find(m_dev_spid);
		if (it == m_spid_chongzhi_cfg_map.end())
		{
			return NULL; //不应该走到这里来
		}
	}

	//int show_binary = it->second.GetShowBinary();
	int sheet = it->second.sheet[currency];
	if (sheet < 0 || sheet >= m_new_chongzhi_sheet_count || sheet >= NEW_CHONGZHI_SHEET_COUNT)
	{
		return NULL; //参数错误
	}

	//UNSTD_STATIC_CHECK(CHONGZHI_REWARD_CONFIG_MAX_NUM <= (int)sizeof(show_binary) * 8);

	if (seq >= 0 && seq < m_sheet_chongzhi_cfg_list[sheet].reward_count) // && (show_binary & (1 << seq)) != 0)
	{
		return &m_sheet_chongzhi_cfg_list[sheet].reward_list[seq];
	}

	return NULL;
}

int ChongzhiConfig::GetExtraRewardCfgCount(int currency, int spid)
{
	if (currency < 0 || currency >= CHONGZHI_CURRENCY_TYPE_MAX)
	{
		return 0;
	}

	SpidChongzhiCfgMap::iterator it = m_spid_chongzhi_cfg_map.find(spid);
	if (it == m_spid_chongzhi_cfg_map.end())
	{
		it = m_spid_chongzhi_cfg_map.find(m_dev_spid);
		if (it == m_spid_chongzhi_cfg_map.end())
		{
			return 0; //不应该走到这里来
		}
	}

	//int show_binary = it->second.show_id[currency];
	int sheet = it->second.sheet[currency];
	if (sheet < 0 || sheet >= m_new_chongzhi_sheet_count || sheet >= NEW_CHONGZHI_SHEET_COUNT)
	{
		return 0; //参数错误
	}

	return m_sheet_chongzhi_cfg_list[sheet].reward_count;
}

const FristRechargeCfg * ChongzhiConfig::GetFristRechargeCfg(int seq)
{
	std::map<int, FristRechargeCfg>::iterator it = m_frist_recharge_cfg.find(seq);
	if (it != m_frist_recharge_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}

const FirstRechargeExclusiveCfg * ChongzhiConfig::GetFirstRechargeExclusiveCfg(int type)
{
	std::map<int, FirstRechargeExclusiveCfg>::const_iterator it = m_first_recharge_exclusive_cfg.find(type);
	if(it == m_first_recharge_exclusive_cfg.end()) return NULL;

	return &it->second;
}

bool ChongzhiConfig::IsFirstRechargeActivity(int activity_type)
{
	if (m_first_recharge_activity_list.end() != m_first_recharge_activity_list.find(activity_type))
	{
		return true;
	}

	return false;
}
