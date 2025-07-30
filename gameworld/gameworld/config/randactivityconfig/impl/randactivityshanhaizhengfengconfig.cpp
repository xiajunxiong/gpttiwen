#include "randactivityshanhaizhengfengconfig.hpp"

#include "item/itempool.h"
#include "monster/monster_group_pool.hpp"

RandActivityShanHaiZhengFengConfig::RandActivityShanHaiZhengFengConfig()
{
}

RandActivityShanHaiZhengFengConfig::~RandActivityShanHaiZhengFengConfig()
{
	std::vector<RAShanHaiZhengFengSectionCfg>().swap(m_base_cfg_vec);
	for (std::map<int, std::vector<RAShanHaiZhengFengBossGroupCfg> >::iterator it = m_boss_group_cfg.begin(); it!= m_boss_group_cfg.end(); it++)
	{
		std::vector<RAShanHaiZhengFengBossGroupCfg>().swap(it->second);
	}
}

bool RandActivityShanHaiZhengFengConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("leader_configuration", InitBossgGroupCfg);
	LOAD_RA_CONFIG("reward_group", InitRewardGroupCfg);
	LOAD_RA_CONFIG("basis_configuration", InitBaseCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);		//需要放在奖励组读取之后

	return true;
}

const RAShanHaiZhengFengBaseCfg * RandActivityShanHaiZhengFengConfig::GetBaseCfg(RandActivityManager * ramgr, int activity_time)const
{
	if(NULL == ramgr || activity_time <= 0 || activity_time > MAX_RA_SUMMER_LI_LIAN_BOSS_NUM) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG);
	for (int i = 0; i < (int)m_base_cfg_vec.size(); i++)
	{
		if (act_real_open_day >= m_base_cfg_vec[i].section_start && act_real_open_day <= m_base_cfg_vec[i].section_end)
		{
			std::map<int, RAShanHaiZhengFengBaseCfg>::const_iterator it = m_base_cfg_vec[i].m_base_cfg.find(activity_time);
			if (it == m_base_cfg_vec[i].m_base_cfg.end())
			{
				continue;
			}
			return &it->second;
		}
	}

	return NULL;
}

const std::map<int, RAShanHaiZhengFengBaseCfg> * RandActivityShanHaiZhengFengConfig::GetBaseMapCfg(RandActivityManager * ramgr) const
{
	if(NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG);
	for (int i = 0; i < (int)m_base_cfg_vec.size(); i++)
	{
		if (act_real_open_day >= m_base_cfg_vec[i].section_start && act_real_open_day <= m_base_cfg_vec[i].section_end)
		{
			return &m_base_cfg_vec[i].m_base_cfg;
		}
	}

	return NULL;
}

const RAShanHaiZhengFengBossGroupCfg * RandActivityShanHaiZhengFengConfig::GetBossGroupCfg(int boss_group_id, int role_level)const
{
	if(role_level <= 0 || role_level > MAX_ROLE_LEVEL) return NULL;

	std::map<int, std::vector<RAShanHaiZhengFengBossGroupCfg> >::const_iterator it = m_boss_group_cfg.find(boss_group_id);
	if(it == m_boss_group_cfg.end()) return NULL;

	for (int i = 0; i < (int)it->second.size(); i++)
	{
		if (role_level >= it->second[i].level_down && role_level <= it->second[i].level_up)
		{
			return &it->second[i];
		}
	}
	return NULL;
}

const RAShanHaiZhengFengRewardCfg * RandActivityShanHaiZhengFengConfig::GetRewardCfg(int reward_group_id)const
{
	std::map<int, RAShanHaiZhengFengRewardGroupCfg>::const_iterator it = m_reward_group_cfg.find(reward_group_id);
	if(it == m_reward_group_cfg.end() || it->second.reward_cfg_list.empty()) return NULL;

	int rate_num = rand() % it->second.rate_count;
	for (int i = 0; i < (int)it->second.reward_cfg_list.size(); i++)
	{
		if (rate_num < it->second.reward_cfg_list[i].rate)
		{
			return &it->second.reward_cfg_list[i];
		}
		rate_num -= it->second.reward_cfg_list[i].rate;
	}

	return NULL;
}

int RandActivityShanHaiZhengFengConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "consume_item", m_other_cfg.consume_item_id) || NULL == ITEMPOOL->GetItem(m_other_cfg.consume_item_id)) return -1;
	if (!GetSubNodeValue(data_element, "num", m_other_cfg.consume_num) || m_other_cfg.consume_num <= 0) return -2;
	if (!GetSubNodeValue(data_element, "all_pass_reward_group", m_other_cfg.all_pass_reward_group_id) || m_reward_group_cfg.end() == m_reward_group_cfg.find(m_other_cfg.all_pass_reward_group_id)) return -3;

	return 0;
}

int RandActivityShanHaiZhengFengConfig::InitBossgGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		int boss_group_id = 0;
		if (!GetSubNodeValue(data_element, "monster_group", boss_group_id) || boss_group_id <= 0) return -1;

		RAShanHaiZhengFengBossGroupCfg node_cfg;
		if (!GetSubNodeValue(data_element, "level_down", node_cfg.level_down) || node_cfg.level_down <= 0 || node_cfg.level_down > MAX_ROLE_LEVEL) return -2;
		if (!GetSubNodeValue(data_element, "level_up", node_cfg.level_up) || node_cfg.level_up < node_cfg.level_down || node_cfg.level_up > MAX_ROLE_LEVEL) return -3;

		char buf[64] = {0};
		for (int i = 1; i <= ARRAY_LENGTH(node_cfg.monster_gourp_list); i++)
		{
			SNPRINTF(buf, sizeof(buf), "monster_id_%d", i);
			if (!GetSubNodeValue(data_element, buf, node_cfg.monster_gourp_list[i - 1]) || NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(node_cfg.monster_gourp_list[i - 1]))
			{
				return -10 - i;
			}

		}
		m_boss_group_cfg[boss_group_id].push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityShanHaiZhengFengConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		int reward_group_id = 0;
		if (!GetSubNodeValue(data_element, "reward_group", reward_group_id) || reward_group_id <= 0) return -1;

		RAShanHaiZhengFengRewardCfg node_cfg;
		if (!GetSubNodeValue(data_element, "exp", node_cfg.exp) || node_cfg.exp < 0) return -2;
		if (!GetSubNodeValue(data_element, "coin", node_cfg.coin) || node_cfg.coin < 0) return -3;
		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate <= 0) return -4;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL != reward_list_element)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
			while (reward_element != NULL)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_element))
				{
					return -101;
				}
				node_cfg.reward_list.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		m_reward_group_cfg[reward_group_id].reward_cfg_list.push_back(node_cfg);
		m_reward_group_cfg[reward_group_id].rate_count += node_cfg.rate;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityShanHaiZhengFengConfig::InitBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		bool has_section_cfg = true;
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start))
		{
			has_section_cfg = false;
		}
		if (!GetSubNodeValue(data_element, "section_end", section_end))
		{
			has_section_cfg = false;
		}

		if (!has_section_cfg) // 没配置，默认给一个无限区间
		{
			section_start = 1;
			section_end = INT_MAX;
		}

		if (0 == section_end) // 0 代表无穷
		{
			section_end = INT_MAX;
		}

		if (m_base_cfg_vec.size() > 0)
		{
			int pre_index = m_base_cfg_vec.size() - 1;
			if (section_start != m_base_cfg_vec[pre_index].section_start ||
				section_end != m_base_cfg_vec[pre_index].section_end)
			{
				if (section_start < m_base_cfg_vec[pre_index].section_end)
				{
					return -777;
				}

				m_base_cfg_vec.push_back(RAShanHaiZhengFengSectionCfg());
			}
		}
		if (m_base_cfg_vec.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_base_cfg_vec.push_back(RAShanHaiZhengFengSectionCfg());
		}

		RAShanHaiZhengFengSectionCfg & node_cfg = m_base_cfg_vec[m_base_cfg_vec.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		int activity_time = 0;
		if (!GetSubNodeValue(data_element, "activity_time", activity_time) || activity_time <= 0 || activity_time > MAX_RA_SUMMER_LI_LIAN_BOSS_NUM) return -1;

		if (node_cfg.m_base_cfg.end() != node_cfg.m_base_cfg.find(activity_time))
		{
			return -2;
		}

		RAShanHaiZhengFengBaseCfg reward_cfg;
		if (!GetSubNodeValue(data_element, "monster_group", reward_cfg.boss_group_id) || m_boss_group_cfg.end() == m_boss_group_cfg.find(reward_cfg.boss_group_id)) return -3;
		if (!GetSubNodeValue(data_element, "first_reward_group", reward_cfg.frist_reward_group_id) || m_reward_group_cfg.end() == m_reward_group_cfg.find(reward_cfg.boss_group_id)) return -4;
		if (!GetSubNodeValue(data_element, "random_reward_group", reward_cfg.common_reward_group_id) || m_reward_group_cfg.end() == m_reward_group_cfg.find(reward_cfg.boss_group_id)) return -5;

		node_cfg.m_base_cfg[activity_time] = reward_cfg;
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
