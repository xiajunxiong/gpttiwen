#include "randactivityyunzeslotmachineconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

RandActivityYunZeSlotMachineConfig::RandActivityYunZeSlotMachineConfig()
	: m_total_rate(0)
{

}

RandActivityYunZeSlotMachineConfig::~RandActivityYunZeSlotMachineConfig()
{
	std::vector<RAYZSlotMachineBasicsCfg>().swap(m_basics_cfg);
	std::vector<RAYZSlotMachineTaskRewardCfg>().swap(m_task_reward_cfg);
}

bool RandActivityYunZeSlotMachineConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basics", InitBasicsCfg);
	LOAD_RA_CONFIG("reward", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

int RandActivityYunZeSlotMachineConfig::InitBasicsCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		RAYZSlotMachineBasicsCfg t_basics;

		if (!GetSubNodeValue(data_element, "item_id", t_basics.item_pool) || t_basics.item_pool <= YUN_ZE_SLOT_MACHINE_TYPE_BEGIN || t_basics.item_pool >= YUN_ZE_SLOT_MACHINE_TYPE_MAX)
		{
			return -1;
		}

		int rate = 0;
		if (!GetSubNodeValue(data_element, "probability", rate) || rate <= 0)
		{
			return -2;
		}

		m_total_rate += rate;
		t_basics.rate = m_total_rate;
		m_basics_cfg.push_back(t_basics);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYunZeSlotMachineConfig::InitRewardCfg(TiXmlElement *RootElement)
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

		if (m_task_reward_cfg.size() > 0)
		{
			int pre_index = m_task_reward_cfg.size() - 1;
			if (section_start != m_task_reward_cfg[pre_index].section_start ||
				section_end != m_task_reward_cfg[pre_index].section_end)
			{
				if (section_start < m_task_reward_cfg[pre_index].section_end)
				{
					return -777;
				}

				m_task_reward_cfg.push_back(RAYZSlotMachineTaskRewardCfg());
			}
		}
		if (m_task_reward_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_task_reward_cfg.push_back(RAYZSlotMachineTaskRewardCfg());
		}

		RAYZSlotMachineTaskRewardCfg & node_cfg = m_task_reward_cfg[m_task_reward_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAYZSlotMachineRewardCfg reward_cfg;
		if (!GetSubNodeValue(data_element, "item_id", reward_cfg.item_group) || reward_cfg.item_group <= 0) return -1;
		if (!GetSubNodeValue(data_element, "item_pool", reward_cfg.item_pool) ||
			reward_cfg.item_pool <= YUN_ZE_SLOT_MACHINE_TYPE_BEGIN || reward_cfg.item_pool >= YUN_ZE_SLOT_MACHINE_TYPE_MAX) return -2;

		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -111;
		ItemConfigData t_item;
		if (!t_item.ReadConfig(reward_element))
		{
			return -3;
		}

		reward_cfg.item = t_item;
		node_cfg.reward_list.push_back(reward_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYunZeSlotMachineConfig::InitOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "price_type", m_other_cfg.price_type)||m_other_cfg.price_type<0) return -1;
	if (!GetSubNodeValue(data_element, "price", m_other_cfg.price) || m_other_cfg.price < 0) return -2;
	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver) || m_other_cfg.cfg_ver < 0) return -3;
	if (!GetSubNodeValue(data_element, "item_num", m_other_cfg.item_consume) || m_other_cfg.item_consume <= 0) return -4;
	if (!GetSubNodeValue(data_element, "limit_type", m_other_cfg.limit_type) || (m_other_cfg.limit_type != YUN_ZE_SLOT_MACHINE_BUY_TYPE_TOTAL && m_other_cfg.limit_type != YUN_ZE_SLOT_MACHINE_BUY_TYPE_TODAY)) return -5;
	if (!GetSubNodeValue(data_element, "buy_times", m_other_cfg.total_buy_times) || m_other_cfg.total_buy_times < 0) return -6;
	if (!GetSubNodeValue(data_element, "today_times", m_other_cfg.today_buy_times) || m_other_cfg.today_buy_times < 0) return -7;
	
	TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
	if (NULL == reward_list_element) return -666;

	TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
	if (NULL == reward_element) return -555;
	if (!m_other_cfg.reward_item.ReadConfig(reward_element))
	{
		return -8;
	}

	return 0;
}

int RandActivityYunZeSlotMachineConfig::GetRandRewardByRateCfg()const
{
	int rand_reward = RandomNum(m_total_rate);

	int item_pool = 0;
	for (int i = 0; i < m_basics_cfg.size(); ++i)
	{
		if (m_basics_cfg[i].rate > rand_reward)
		{
			item_pool = m_basics_cfg[i].item_pool;
			break;
		}
	}

	return item_pool;
}

const RAYZSlotMachineTaskRewardCfg* RandActivityYunZeSlotMachineConfig::GetTaskRewardCfg(RandActivityManager* ramgr)const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE);
	for (int i = 0; i < (int)m_task_reward_cfg.size(); ++i)
	{
		if (m_task_reward_cfg[i].section_start <= act_real_open_day && m_task_reward_cfg[i].section_end >= act_real_open_day)
		{
			const RAYZSlotMachineTaskRewardCfg & node_list = m_task_reward_cfg[i];
			return &node_list;
		}
	}

	return NULL;
}

