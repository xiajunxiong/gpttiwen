#include "elementelfconfig.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "checkresourcecenter.hpp"
#include "servercommon/elementelfdef.hpp"

ElementElfConfig::ElementElfConfig()
{

}

ElementElfConfig::~ElementElfConfig()
{
	std::vector<int>().swap(m_element_elf_monsters);
	for (std::map<int, std::vector<ElementElfRewardCfg> >::iterator it = m_reward_group_cfg.begin(); it != m_reward_group_cfg.end(); it++)
	{
		std::vector<ElementElfRewardCfg>().swap(it->second);
	}
	std::vector<ElementElfShowUpTimesCfg>().swap(m_show_up_cfgs);
}

bool ElementElfConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("elf", InitElementElfMonsterList);
	LOAD_CONFIG("reward", InitRewardCfg);
	LOAD_CONFIG("battle_mode", InitShowUpBattleMode);
	LOAD_CONFIG("shield_monster_group", InitMonsterGroupCfg);
	LOAD_CONFIG("rate", InitShowUpRateCfg);

	//if (!this->CheckRewardGroupCfg(err))
	//{
	//	return false;
	//}

	//if (!this->CheckShowUpCfg(err))
	//{
	//	return false;
	//}

	return true;
}

bool ElementElfConfig::RandElementElfShowUp(int monster_group_id, int battle_mode, int today_showup_times, int* out_reward_group) const
{
	{
		std::set<int>::const_iterator it =
			m_monster_group_noshow_set.find(monster_group_id);
		if (it != m_monster_group_noshow_set.end()) return false;
	}
	
	{
		std::set<int>::const_iterator it =
			m_showup_battle_mode_set.find(battle_mode);
		if (it == m_showup_battle_mode_set.end()) return false;
	}

	return this->IsShowUp(today_showup_times, out_reward_group);
}

int ElementElfConfig::RandMonsterId() const
{
	assert(m_element_elf_monsters.size() > 0);

	return m_element_elf_monsters[rand() % m_element_elf_monsters.size()];
}

const ElementElfRewardCfg* ElementElfConfig::RandRewardCfg(int reward_group) const
{
	std::map<int, std::vector<ElementElfRewardCfg> >::const_iterator it 
		= m_reward_group_cfg.find(reward_group);
	if (it == m_reward_group_cfg.end()) return NULL;

	const std::vector<ElementElfRewardCfg>& vec = it->second;
	int previous_sum_rate = 0;
	int r = RandomNum(PERCENT_NUM);
	for (size_t i = 0; i < vec.size(); ++i)
	{
		if (r < previous_sum_rate + vec[i].rate)
		{
			return &vec[i];
		}
		previous_sum_rate += vec[i].rate;
	}

	return NULL;
}

int ElementElfConfig::InitElementElfMonsterList(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	m_element_elf_monsters.reserve(4);
	while (NULL != data_element)
	{
		int monster_id = 0;
		if (!GetSubNodeValue(data_element, "monster_id", monster_id))
		{
			return -1;
		}
		m_element_elf_monsters.push_back(monster_id);
		
		CheckResourceCenter::Instance().GetMonsterCheck()
			->Add(monster_id, STRING_SPRINTF("Func[%s] monster_id[%d]", 
				__FUNCTION__, monster_id).c_str());

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ElementElfConfig::InitRewardCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int reward_group = 0;
		if (!GetSubNodeValue(data_element, "reward_group", reward_group))
		{
			return -1;
		}

		ElementElfRewardCfg cfg;
		if (!GetSubNodeValue(data_element, "rate", cfg.rate))
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "reward_seq", cfg.seq))
		{
			return -3;
		}

		TiXmlElement * reward_elements = data_element->FirstChildElement("reward_list");
		if (NULL != reward_elements)
		{
			TiXmlElement * reward_item_element = reward_elements->FirstChildElement("reward");
			while (reward_item_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_item_element))
				{
					return -200;
				}

				cfg.rewards.push_back(reward);
				reward_item_element = reward_item_element->NextSiblingElement();
			}
		}

		m_reward_group_cfg[reward_group].push_back(cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ElementElfConfig::InitMonsterGroupCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int monster_group = 0;
		if (!GetSubNodeValue(data_element, "monster_group_id", monster_group))
		{
			return -1;
		}

		m_monster_group_noshow_set.insert(monster_group);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ElementElfConfig::InitShowUpRateCfg(TiXmlElement* RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	int last_times = 0; // times必须从1开始
	while (NULL != data_element)
	{
		int times = 0;
		if (!GetSubNodeValue(data_element, "time", times) || times != last_times + 1)
		{
			return -1;
		}
		last_times = times;

		ElementElfShowUpTimesCfg cfg;
		if (!GetSubNodeValue(data_element, "rate", cfg.show_up_rate) || cfg.show_up_rate < 0)
		{
			return -2;
		}
		
		if (!GetSubNodeValue(data_element, "reward_group", cfg.reward_group) || cfg.reward_group < 0)
		{
			return -3;
		}
		m_show_up_cfgs.push_back(cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ElementElfConfig::InitShowUpBattleMode(TiXmlElement* RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int battle_mode = 0;
		if (!GetSubNodeValue(data_element, "battle_mode", battle_mode))
		{
			return -1;
		}

		m_showup_battle_mode_set.insert(battle_mode);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

bool ElementElfConfig::IsShowUp(int today_showup_times, int* out_reward_group) const
{
	if (today_showup_times >= m_show_up_cfgs.size()
		|| today_showup_times < 0)
	{
		return false;
	}

	const ElementElfShowUpTimesCfg& cfg = m_show_up_cfgs[today_showup_times];
	int r = RandomNum(PERCENT_NUM);
	if (r >= cfg.show_up_rate)
	{
		return false;
	}

	if (NULL != out_reward_group)
	{
		*out_reward_group = cfg.reward_group;
	}
	return true;
}

bool ElementElfConfig::CheckRewardGroupCfg(std::string* errmsg) const
{
	std::map<int, std::vector<ElementElfRewardCfg> >::const_iterator it = m_reward_group_cfg.begin();
	for (; it != m_reward_group_cfg.end(); ++it)
	{
		int sum = 0;
		for (size_t i = 0; i < it->second.size(); ++i)
		{
			sum += it->second[i].rate;
		}
		if (sum != PERCENT_NUM)
		{
			if (NULL != errmsg)
			{
				*errmsg = STRING_SPRINTF("CheckRewardGroupCfg TOTAL_RATE MUST BE %d, reward_group[%d] total_rate[%d]",
					PERCENT_NUM, it->first, sum);
			}
			return false;
		}
	}

	return true;
}

bool ElementElfConfig::CheckShowUpCfg(std::string* errmsg) const
{
	for (size_t i = 0; i < m_show_up_cfgs.size(); ++i)
	{
		int reward_group = m_show_up_cfgs[i].reward_group;
		if (m_reward_group_cfg.find(reward_group) 
			== m_reward_group_cfg.end())
		{
			if (NULL != errmsg)
			{
				*errmsg = STRING_SPRINTF("CheckShowUpCfg reward_group[%d] NOT_FOUND",
					reward_group);
			}
			return false;
		}
	}

	return true;
}

