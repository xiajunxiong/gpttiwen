#include "item/itempool.h"
#include "servercommon/commondata.hpp"

#include "xunbaoconfig.hpp"

#include "checkresourcecenter.hpp"

XunBaoConfig::XunBaoConfig()
{
}

XunBaoConfig::~XunBaoConfig()
{
	for (std::map<int, std::vector<XunBaoExchangeComsumeCfg> >::iterator it = m_exchange_comsume_cfg.begin(); it != m_exchange_comsume_cfg.end(); it++)
	{
		std::vector<XunBaoExchangeComsumeCfg>().swap(it->second);
	}

	for (std::map<int, std::vector<XunBaoExchangeGetCfg> >::iterator it = m_exchange_get_cfg.begin(); it!= m_exchange_get_cfg.end(); it++)
	{
		std::vector<XunBaoExchangeGetCfg>().swap(it->second);
	}
}

bool XunBaoConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("minimum_award", InitDrawFloorRewardCfg);
	LOAD_CONFIG("first_award", InitFirstDrawRewardCfg);
	LOAD_CONFIG("treasure", InitXunBaoCfg);
	LOAD_CONFIG("bless_rate", InitBlessRateCfg);
	LOAD_CONFIG("reward", InitRewardGroupCfg);
	LOAD_CONFIG("level_section", InitLevelCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("add_up_award", InitDrawCumulativeRewardCfg);
	LOAD_CONFIG("dis_material", InitExchangeComsumeCfg);
	LOAD_CONFIG("dis_acquire", InitExchangeGetCfg);
	LOAD_CONFIG("minimum_award_rare", InitXiYouBaoDiCfg);
	
	return true;
}

XunBaoDrawCfg * XunBaoConfig::GetXunBaoDrawCfg(int draw_type)
{
	std::map<int, XunBaoDrawCfg >::iterator iter = m_draw_cfg.find(draw_type);
	if (iter != m_draw_cfg.end())
	{
		return &iter->second;
	}

	return NULL;
}

std::set<int> XunBaoConfig::GetXunBaoAllDrawTypeCfg()
{
	std::set<int> ret;
	for (std::map<int, XunBaoDrawCfg >::iterator iter = m_draw_cfg.begin(); iter != m_draw_cfg.end(); ++iter)
	{
		ret.insert(iter->first);
	}

	return ret;
}


std::vector<const XunBaoDrawCfg *> XunBaoConfig::GetOpenDrawCfg()
{
	std::vector<const XunBaoDrawCfg *> ret;
	for (std::map<int, XunBaoDrawCfg >::iterator iter = m_draw_cfg.begin(); iter != m_draw_cfg.end(); ++iter)
	{
		ret.push_back(&iter->second);
	}

	return ret;
}

bool XunBaoConfig::IsHitDrawCumulativeReward(int group_id, const std::vector<ItemConfigData> & item_list)
{
	std::map<int, XunBaoDrawCumulativeRewardCfg >::iterator iter = m_draw_cumulative_reward_cfg.find(group_id);
	if (iter == m_draw_cumulative_reward_cfg.end())
	{
		return false;
	}

	const XunBaoDrawCumulativeRewardCfg & cfg = iter->second;
	for (int index = 0; index < (int)item_list.size(); ++index)
	{
		for (int i = 0; i < (int)cfg.group_reward.size(); ++i)
		{
			if (item_list[index].item_id == cfg.group_reward[i].reward.item_id)
			{
				return true;
			}
		}
	}

	return false;
}

const ItemConfigData * XunBaoConfig::GetDrawCumulativeReward(int group_id, int draw_times, int need_times)
{
	if (need_times <= 0)
	{
		return NULL;
	}

	if (draw_times < need_times)
	{
		return NULL;
	}

	std::map<int, XunBaoDrawCumulativeRewardCfg >::iterator iter = m_draw_cumulative_reward_cfg.find(group_id);
	if (iter == m_draw_cumulative_reward_cfg.end())
	{
		return NULL;
	}

	const XunBaoDrawCumulativeRewardCfg & cfg = iter->second;

	int r = RandomNum(cfg.rate_count);

	for (int i = 0; i < (int)cfg.group_reward.size(); ++i)
	{
		if (r < cfg.group_reward[i].rate)
		{
			return &cfg.group_reward[i].reward;
		}

		r -= cfg.group_reward[i].rate;
	}

	return NULL;
}

const ItemConfigData * XunBaoConfig::GetXiYouBaoReward(int draw_type)
{
	std::map<int, XunBaoXiYouBaoDiRewardCfg >::iterator iter = m_xi_you_bao_di_map_cfg.find(draw_type);
	if (iter == m_xi_you_bao_di_map_cfg.end())
	{
		return NULL;
	}

	const XunBaoXiYouBaoDiRewardCfg & cfg = iter->second;

	int r = RandomNum(cfg.rate_count);

	for (int i = 0; i < (int)cfg.group_reward.size(); ++i)
	{
		if (r < cfg.group_reward[i].rate)
		{
			return &cfg.group_reward[i].reward;
		}

		r -= cfg.group_reward[i].rate;
	}

	return NULL;
}

const XunBaoExchangeComsumeCfg * XunBaoConfig::GetExchangeComsumeCfg(int group_id, int seq)
{
	std::map<int, std::vector<XunBaoExchangeComsumeCfg> >::iterator iter = m_exchange_comsume_cfg.find(group_id);
	if (iter == m_exchange_comsume_cfg.end())
	{
		return NULL;
	}

	if (seq < 0 || seq >= (int)iter->second.size())
	{
		return NULL;
	}

	return &iter->second[seq];
}

const XunBaoExchangeGetCfg * XunBaoConfig::GetExchangeGetCfg(int group_id, int seq)
{
	std::map<int, std::vector<XunBaoExchangeGetCfg> >::iterator iter = m_exchange_get_cfg.find(group_id);
	if (iter == m_exchange_get_cfg.end())
	{
		return NULL;
	}

	if (seq < 0 || seq >= (int)iter->second.size())
	{
		return NULL;
	}

	return &iter->second[seq];
}

int XunBaoConfig::InitDrawFloorRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int draw_type = 0;
		if (!GetSubNodeValue(root_element, "treasure_id", draw_type))
		{
			return -1;
		}
		XunBaoDrawCfg & draw_cfg = m_draw_cfg[draw_type];

		XunBaoDrawCfg::DrawFloorRewardItem cfg;
		if (!GetSubNodeValue(root_element, "weight", cfg.rate) || cfg.rate <= 0)
		{
			return -12;
		}

		TiXmlElement * item_element = root_element->FirstChildElement("reward");
		if (NULL == item_element) return -13;		// 必须要有奖励

		if (!cfg.reward.ReadConfig(item_element))
		{
			return -33;
		}

		draw_cfg.five_draw_floor_item_rate_count += cfg.rate;
		draw_cfg.five_draw_floor_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int XunBaoConfig::InitFirstDrawRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int draw_type = 0;
		if (!GetSubNodeValue(root_element, "treasure_id", draw_type))
		{
			return -1;
		}

		if (draw_type < 0 || draw_type >= 64) //角色通用数据long long 记录每种寻宝是不是首抽，不能超过64
		{
			return -19999;
		}

		XunBaoDrawCfg & draw_cfg = m_draw_cfg[draw_type];

		XunBaoDrawCfg::DrawFirstDrawRewardItem cfg;
		if (!GetSubNodeValue(root_element, "weight", cfg.rate) || cfg.rate <= 0)
		{
			return -12;
		}

		TiXmlElement * item_element = root_element->FirstChildElement("reward");
		if (!cfg.reward.ReadConfig(item_element))
		{
			return -33;
		}

		draw_cfg.first_draw_item_rate_count += cfg.rate;
		draw_cfg.first_draw_item_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int XunBaoConfig::InitXunBaoCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int draw_type= 0;
		if (!GetSubNodeValue(root_element, "treasure_id", draw_type))
		{
			return -__LINE__;
		}
		XunBaoDrawCfg & draw_cfg = m_draw_cfg[draw_type];

		draw_cfg.draw_type = draw_type;
		if (!GetSubNodeValue(root_element, "is_open", draw_cfg.is_open))
		{
			return -__LINE__;
		}

		if (draw_cfg.draw_type >= XUN_BAO_TREASURE_ID_MAX)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "time_day", draw_cfg.open_day) || draw_cfg.open_day < 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "open_type", draw_cfg.open_type))
		{
			return -__LINE__;
		}

		if (draw_cfg.open_type == 1)
		{
			if (!GetSubNodeValue(root_element, "start_time", draw_cfg.start_time))
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(root_element, "end_time", draw_cfg.end_time))
			{
				return -__LINE__;
			}

			if (draw_cfg.start_time > draw_cfg.end_time)
			{
				return -__LINE__;
			}
		}

		if (draw_cfg.open_type == 2)
		{
			if (!GetSubNodeValue(root_element, "day_start_time", draw_cfg.week_day_open_begin_time))
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(root_element, "day_end_time", draw_cfg.week_day_open_end_time))
			{
				return -__LINE__;
			}

			if (draw_cfg.week_day_open_begin_time > draw_cfg.week_day_open_end_time)
			{
				return -__LINE__;
			}

			int open_hour = draw_cfg.week_day_open_begin_time / 100;
			int open_min = draw_cfg.week_day_open_begin_time % 100;
			int end_hour = draw_cfg.week_day_open_end_time / 100;
			int end_min = draw_cfg.week_day_open_end_time % 100;

			if (open_hour < 0 || open_hour > 23 || open_min < 0 || open_min > 59)
			{
				return -__LINE__;
			}

			if (end_hour < 0 || end_hour > 23 || end_min < 0 || end_min > 59)
			{
				return -__LINE__;
			}


			TiXmlElement * week_open_day_list_element = root_element->FirstChildElement("open_day");
			if (week_open_day_list_element != NULL)
			{
				TiXmlElement * node_element = week_open_day_list_element->FirstChildElement("node");
				while (node_element)
				{
					int week_day = 0;
					if (!GetNodeValue(node_element, week_day) || week_day < 0 || week_day >= 7)
					{
						return -__LINE__;
					}

					draw_cfg.week_open_flag |= (1 << week_day);

					node_element = node_element->NextSiblingElement();
				}
			}

		}


		if (!GetSubNodeValue(root_element, "once_item_id", draw_cfg.once_item_id) || NULL == ITEMPOOL->GetItem(draw_cfg.once_item_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "five_item_id", draw_cfg.five_item_id) || NULL == ITEMPOOL->GetItem(draw_cfg.five_item_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "is_pet", draw_cfg.is_has_pet_reward))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "group_id", draw_cfg.draw_times_reward_group_id))
		{
			return -__LINE__;
		}

		if (draw_cfg.draw_times_reward_group_id != 0 )
		{
			if (draw_cfg.draw_times_reward_group_id < 0 || draw_cfg.draw_times_reward_group_id > XUN_BAO_DRAW_CUMULATIVE_TIMES_GROUP_MAX_NUM)
			{
				return -__LINE__;
			}
		}

		if (!GetSubNodeValue(root_element, "material_id1", draw_cfg.exchange_material_id1))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "material_id2", draw_cfg.exchange_material_id2))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "get_id", draw_cfg.exchange_get_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "is_num_rare", draw_cfg.xi_you_bao_di_times))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "time_treasure_free", draw_cfg.free_draw_interval))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "first_treasure_hunt_time", draw_cfg.first_free_draw_interval))
		{
			return -__LINE__;
		}

		if (draw_cfg.first_free_draw_interval != 0 && draw_cfg.free_draw_interval == 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "free_times_num", draw_cfg.free_times_num) || draw_cfg.free_times_num < 0 || draw_cfg.free_times_num >= XUN_BAO_TREASURE_ID_MAX)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "once_floors", draw_cfg.second_draw_bao_di_times))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "once_floors_group", draw_cfg.second_draw_bao_di_reward_group_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "can_cumulative_bao_di", draw_cfg.can_cumulative_bao_di))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "cumulative_bao_di_times", draw_cfg.cumulative_bao_di_times))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "is_reset_floors", draw_cfg.is_reset_floors))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "treasure_hunt_type", draw_cfg.xun_bao_treasure_type) || 
			draw_cfg.xun_bao_treasure_type < XUN_BAO_TREASURE_TYPE_INVALID || draw_cfg.xun_bao_treasure_type >= XUN_BAO_TREASURE_TYPE_MAX)
		{
			return -__LINE__;
		}

		root_element = root_element->NextSiblingElement();
	}
	
	return 0;
}

int XunBaoConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int rand_group = 0; 
		if (!GetSubNodeValue(root_element, "reward_group", rand_group))
		{
			return -1;
		}
		int reward_type = 0;
		if (!GetSubNodeValue(root_element, "reward_type", reward_type) || (XUN_BAO_REWARD_TYPE_OTTHER != reward_type && m_bless_map.find(reward_type) == m_bless_map.end()))
		{
			return -__LINE__;
		}
		XunBaoRewardGroup::ItemCfg item_cfg;
		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "floor_refresh_times", item_cfg.floor_times) || item_cfg.floor_times < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "is_broadcast", item_cfg.is_broadcast) )
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "is_num_rare", item_cfg.xi_you_bao_di_type))
		{
			return -4441;
		}

		TiXmlElement * item_element = root_element->FirstChildElement("reward");
		if (NULL == item_element) return -3;		// 必须要有奖励

		if (!item_cfg.reward.ReadConfig(item_element))
		{
			return -33;
		}

		XunBaoRewardGroup & group_cfg = m_reward_group[rand_group];
		group_cfg.reward_group_id = rand_group;
		group_cfg.rate_count += item_cfg.rate;
		group_cfg.reward_type_map[reward_type].rate_count += item_cfg.rate;
		group_cfg.reward_type_map[reward_type].reward_list.push_back(item_cfg);

		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, XunBaoRewardGroup>::iterator groud_it = m_reward_group.begin(); groud_it != m_reward_group.end(); groud_it++)
	{
		for (std::map<int, XunBaoRewardGroup::ItemGroupCfg>::iterator item_group_it = groud_it->second.reward_type_map.begin(); item_group_it != groud_it->second.reward_type_map.end(); item_group_it++)
		{
			if (XUN_BAO_REWARD_TYPE_OTTHER != item_group_it->first)
			{
				std::map<int, int>::iterator it = m_bless_map.find(item_group_it->first);
				if (it != m_bless_map.end())
				{
					item_group_it->second.rate_up = it->second;
				}
			}
		}
	}

	return 0;
}

int XunBaoConfig::InitLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int draw_type = 0;
		if (!GetSubNodeValue(root_element, "treasure_id", draw_type))
		{
			return -1;
		}
		XunBaoDrawCfg & draw_cfg = m_draw_cfg[draw_type];

		XunBaoDrawCfg::LevelRewardItem item_cfg;
		if (!GetSubNodeValue(root_element, "level_min", item_cfg.min_level))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "level_max", item_cfg.max_level) )
		{
			return -3;
		}

		int reward_group = 0;
		if (!GetSubNodeValue(root_element, "reward_group", reward_group))
		{
			return -4;
		}

		if (m_reward_group.find(reward_group) == m_reward_group.end())
		{
			return -5;
		}

		item_cfg.reward_group = m_reward_group[reward_group];

		draw_cfg.level_draw_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int XunBaoConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "xunbao_pet_id_3", m_other_cfg.xunbao_pet_id_3))
	{
		return -1;
	}
	CheckResourceCenter::Instance().GetPetCheck()->Add(m_other_cfg.xunbao_pet_id_3, __FUNCTION__);

	return 0;
}

int XunBaoConfig::InitDrawCumulativeRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id))
		{
			return -1;
		}

		XunBaoDrawCumulativeRewardCfg & cfg = m_draw_cumulative_reward_cfg[group_id];
		XunBaoDrawCumulativeRewardCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "weight", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		TiXmlElement * reward = root_element->FirstChildElement("reward");
		if (!item_cfg.reward.ReadConfig(reward))
		{
			return -104;
		}

		cfg.rate_count += item_cfg.rate;
		cfg.group_reward.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int XunBaoConfig::InitExchangeComsumeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -__LINE__;
	}

	while (NULL != root_element)
	{
		XunBaoExchangeComsumeCfg cfg;
		if (!GetSubNodeValue(root_element, "material_id", cfg.group_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "strengthen_num", cfg.strengthen_level))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "pet_num", cfg.num))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "pet_id", cfg.pet_id))
		{
			return -__LINE__;
		}

		m_exchange_comsume_cfg[cfg.group_id].push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int XunBaoConfig::InitExchangeGetCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -__LINE__;
	}

	while (NULL != root_element)
	{
		XunBaoExchangeGetCfg cfg;
		if (!GetSubNodeValue(root_element, "get_id", cfg.group_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "strengthen_num", cfg.strengthen_level))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "pet_num", cfg.num))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "pet_id", cfg.pet_id))
		{
			return -__LINE__;
		}

		m_exchange_get_cfg[cfg.group_id].push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int XunBaoConfig::InitBlessRateCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -__LINE__;
	}
	while (NULL != root_element)
	{
		int reward_type = 0;
		if (!GetSubNodeValue(root_element, "reward_type", reward_type) || reward_type <= XUN_BAO_REWARD_TYPE_OTTHER || reward_type > XUN_BAO_REWARD_TYPE_MAX)
		{
			return -__LINE__;
		}
		int rate_up = 0;
		if (!GetSubNodeValue(root_element, "rate_up", rate_up) || rate_up <= 0 || rate_up > 100)
		{
			return -__LINE__;
		}
		m_bless_map[reward_type] = rate_up;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int XunBaoConfig::InitXiYouBaoDiCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int draw_type = 0;
		if (!GetSubNodeValue(root_element, "treasure_id", draw_type))
		{
			return -1;
		}

		XunBaoXiYouBaoDiRewardCfg & cfg = m_xi_you_bao_di_map_cfg[draw_type];
		XunBaoXiYouBaoDiRewardCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "weight", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		TiXmlElement * reward = root_element->FirstChildElement("reward");
		if (!item_cfg.reward.ReadConfig(reward))
		{
			return -104;
		}

		cfg.rate_count += item_cfg.rate;
		cfg.group_reward.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

bool XunBaoDrawCfg::Draw(int role_level, ItemConfigData & out_item, bool & out_is_broadcast, int & outxi_you_type, int silver_pet_id, int gold_pet_id)
{
	for (int i = 0; i < (int)level_draw_cfg.size(); ++i)
	{
		if (level_draw_cfg[i].min_level <= role_level && role_level <= level_draw_cfg[i].max_level)
		{
			return level_draw_cfg[i].reward_group.Draw(out_item, out_is_broadcast, outxi_you_type, silver_pet_id, gold_pet_id);
		}
	}

	return false;
}

bool XunBaoDrawCfg::GetFiveDrawFloorItem(ItemConfigData & out)
{
	int r = RandomNum(five_draw_floor_item_rate_count);
	for (int i = 0; i < (int)five_draw_floor_cfg.size(); ++i)
	{
		if (r < five_draw_floor_cfg[i].rate)
		{
			out = five_draw_floor_cfg[i].reward;
			return true;
		}

		r -= five_draw_floor_cfg[i].rate;
	}

	return false;
}

bool XunBaoDrawCfg::GetFirstDrawFloorItem(ItemConfigData & out)
{
	int r = RandomNum(first_draw_item_rate_count);
	for (int i = 0; i < (int)first_draw_item_cfg.size(); ++i)
	{
		if (r < first_draw_item_cfg[i].rate)
		{
			out = first_draw_item_cfg[i].reward;
			return true;
		}

		r -= first_draw_item_cfg[i].rate;
	}

	return false;
}
