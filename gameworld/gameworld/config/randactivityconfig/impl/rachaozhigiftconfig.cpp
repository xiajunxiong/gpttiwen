#include "rachaozhigiftconfig.hpp"

RAChaoZhiGiftConfig::RAChaoZhiGiftConfig()
{
}

RAChaoZhiGiftConfig::~RAChaoZhiGiftConfig()
{
}

bool RAChaoZhiGiftConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("buy_configure", InitCmdBuyCfg);
	LOAD_RA_CONFIG("gift_configure", InitGiftCfg);
	LOAD_RA_CONFIG("rate_configure", InitRewardCfg);

	return true;
}

const RAChaoZhiGift::CmdBuyCfg * RAChaoZhiGiftConfig::GetCmdBuyCfg(RandActivityManager * ramgr) const
{
	const int now_game_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CHAO_ZHI_GIFT);
	std::map<int, RAChaoZhiGift::CmdBuyCfg>::const_iterator section_day_cit
		= m_cmd_buy_day_section_cfg.upper_bound(now_game_open_day);
	if (m_cmd_buy_day_section_cfg.begin() == section_day_cit)
	{
		return NULL;
	}
	section_day_cit--;


	return &section_day_cit->second;
}

const RAChaoZhiGift::GiftCfg * RAChaoZhiGiftConfig::GetGiftCfg(RandActivityManager * ramgr, unsigned int act_begin_time, int gift_index) const
{
	const int act_begin_game_open_day
		= this->GetOpenDayByTimestamp(ramgr, act_begin_time);

	std::map<int, RAChaoZhiGift::GiftIndexMap>::const_iterator section_day_cit
		= m_gift_day_section_cfg.upper_bound(act_begin_game_open_day);
	if (m_gift_day_section_cfg.begin() == section_day_cit)
	{
		return NULL;
	}
	section_day_cit--;

	RAChaoZhiGift::GiftIndexMap::const_iterator gift_cit = section_day_cit->second.find(gift_index);
	if (section_day_cit->second.end() == gift_cit)
	{
		return NULL;
	}

	return &gift_cit->second;
}

int RAChaoZhiGiftConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "cfg_ver", m_other_cfg.cfg_ver) || m_other_cfg.cfg_ver < 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "buy_times", m_other_cfg.can_buy_day)
		|| 0 >= m_other_cfg.can_buy_day)
	{
		return -2;
	}

	return 0;
}

int RAChaoZhiGiftConfig::InitCmdBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -777;
	}

	while (NULL != data_element)
	{
		int day_section_begin = 0;
		if (!GetSubNodeValue(data_element, "section_start", day_section_begin)
			|| 0 >= day_section_begin)
		{
			return -1;
		}

		int index = 0;
		if (!GetSubNodeValue(data_element, "type", index)
			|| (int)m_cmd_buy_day_section_cfg.size() != index)
		{
			return -2;
		}

		RAChaoZhiGift::CmdBuyCfg & cfg_info = m_cmd_buy_day_section_cfg[day_section_begin];
		if (!GetSubNodeValue(data_element, "price", cfg_info.buy_money)
			|| 0 >= cfg_info.buy_money)
		{
			return -3;
		}

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RAChaoZhiGiftConfig::InitGiftCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -777;
	}

	while (NULL != data_element)
	{
		int day_section_begin = 0;
		if (!GetSubNodeValue(data_element, "section_start", day_section_begin)
			|| 0 >= day_section_begin)
		{
			return -1;
		}

		RAChaoZhiGift::GiftIndexMap &day_section_cfg = m_gift_day_section_cfg[day_section_begin];

		int gift_index = 0;
		if (!GetSubNodeValue(data_element, "seq", gift_index)
			|| (int)day_section_cfg.size() != gift_index
			|| 0 > gift_index
			|| RAChaoZhiGift::GIFT_MAX <= gift_index)
		{
			return -2;
		}

		RAChaoZhiGift::GiftCfg &gift_cfg = day_section_cfg[gift_index];
		gift_cfg.gift_index = gift_index;

		if (!GetSubNodeValue(data_element, "active_times", gift_cfg.can_fetch_act_day)
			|| 0 > gift_cfg.can_fetch_act_day)
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "item_id", gift_cfg.reward_id)
			|| 0 > gift_cfg.reward_id)
		{
			return -4;
		}

		if (!GetSubNodeValue(data_element, "is_bind", gift_cfg.is_bind)
			|| 0 > gift_cfg.is_bind
			|| 1 <  gift_cfg.is_bind)
		{
			return -5;
		}

		if (!GetSubNodeValue(data_element, "num_down", gift_cfg.reward_min)
			|| 0 >= gift_cfg.reward_min)
		{
			return -6;
		}

		if (!GetSubNodeValue(data_element, "num_up", gift_cfg.reward_max)
			|| gift_cfg.reward_min > gift_cfg.reward_max)
		{
			return -7;
		}

		//检查物品
		const ItemBase * item_base = ITEMPOOL->GetItem(gift_cfg.reward_id);
		if (NULL == item_base)
		{
			return -8;
		}

		if (!item_base->CanMerge()
			&& 1 < gift_cfg.reward_max)
		{
			return -9;
		}

		if (item_base->GetPileLimit() < gift_cfg.reward_max)
		{
			return -10;
		}

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RAChaoZhiGiftConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -777;
	}

	while (NULL != data_element)
	{
		int day_section_begin = 0;
		if (!GetSubNodeValue(data_element, "section_start", day_section_begin)
			|| 0 >= day_section_begin)
		{
			return -1;
		}

		RAChaoZhiGift::GiftIndexMap &day_section_cfg = m_gift_day_section_cfg[day_section_begin];

		int gift_index = 0;
		if (!GetSubNodeValue(data_element, "gift_index", gift_index))
		{
			return -2;
		}

		RAChaoZhiGift::GiftIndexMap::iterator gift_it = day_section_cfg.find(gift_index);
		if (day_section_cfg.end() == gift_it)
		{
			return -3;
		}

		RAChaoZhiGift::GiftCfg &gift_cfg = gift_it->second;

		int can_fetch_act_day = 0;
		if (!GetSubNodeValue(data_element, "active_times", can_fetch_act_day)
			|| gift_cfg.can_fetch_act_day != can_fetch_act_day)
		{//活动天数 配错位了
			return -4;
		}

		RAChaoZhiGift::RewardPoolItem pool_item;
		if (!GetSubNodeValue(data_element, "num_down", pool_item.reward_min)
			|| 0 >= pool_item.reward_min)
		{
			return -5;
		}

		if (!GetSubNodeValue(data_element, "num_up", pool_item.reward_max)
			|| pool_item.reward_min > pool_item.reward_max)
		{
			return -6;
		}

		if (gift_cfg.reward_min > pool_item.reward_min
			|| gift_cfg.reward_max < pool_item.reward_max)
		{//超过奖励限制
			return -7;
		}

		if (!GetSubNodeValue(data_element, "rate", pool_item.power)
			||0 >= pool_item.power)
		{
			return -8;
		}

		gift_cfg.reward_pool[gift_cfg.total_power] = pool_item;
		gift_cfg.total_power += pool_item.power;

		data_element = data_element->NextSiblingElement();
	}

	for (std::map<int, RAChaoZhiGift::GiftIndexMap>::const_iterator section_cit = m_gift_day_section_cfg.begin()
		; m_gift_day_section_cfg.end() != section_cit
		; section_cit++)
	{
		for (RAChaoZhiGift::GiftIndexMap::const_iterator gift_cit = section_cit->second.begin()
			; section_cit->second.end() != gift_cit
			; gift_cit++)
		{
			if (0 >= gift_cit->second.total_power)
			{
				return -100000 - section_cit->first * 1000 - gift_cit->first;
			}
		}
	}

	return 0;
}

const ItemConfigData RAChaoZhiGift::GiftCfg::RandRewardItem() const
{
	ItemConfigData out_item;

	int rand_num = RandomNum(total_power);

	std::map<int, RewardPoolItem>::const_iterator item_cit = reward_pool.upper_bound(rand_num);
	if (reward_pool.begin() == item_cit)
	{
		return out_item;
	}
	item_cit--;

	out_item.item_id = reward_id;
	out_item.is_bind = is_bind;
	out_item.num = RandomNum(item_cit->second.reward_min, item_cit->second.reward_max + 1);

	return out_item;
}
