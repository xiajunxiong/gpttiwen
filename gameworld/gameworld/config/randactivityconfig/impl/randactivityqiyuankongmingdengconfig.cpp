#include "randactivityqiyuankongmingdengconfig.hpp"
#include "servercommon/activitydef.hpp"
#include "servercommon/roleactivity/raqiyuankongmingdengparam.hpp"

RandActivityQiYuanKongMingDengConfig::RandActivityQiYuanKongMingDengConfig()
{
}

RandActivityQiYuanKongMingDengConfig::~RandActivityQiYuanKongMingDengConfig()
{
	for (std::map<short, std::vector<RAQiYuanKongMingDengTaskCfg> >::iterator it = m_task_cfg.begin(); it != m_task_cfg.end(); it++)
	{
		std::vector<RAQiYuanKongMingDengTaskCfg>().swap(it->second);
	}
}

bool RandActivityQiYuanKongMingDengConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("task", InitTaskCfg);
	LOAD_RA_CONFIG("reward", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const std::vector<RAQiYuanKongMingDengTaskCfg>* RandActivityQiYuanKongMingDengConfig::GetTaskListCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_QIYUAN_KONGMINGDENG);

	std::map<short, std::vector<RAQiYuanKongMingDengTaskCfg> >::const_iterator day_it = m_task_cfg.find(act_real_open_day);
	if (day_it == m_task_cfg.end())
		return NULL;

	return &day_it->second;
}

const RAQiYuanKongMingDengRewardCfg * RandActivityQiYuanKongMingDengConfig::GetRewardCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_QIYUAN_KONGMINGDENG);

	std::map<short, RAQiYuanKongMingDengRewardCfg>::const_iterator it = m_reward_cfg.upper_bound(act_real_open_day);
	if (it == m_reward_cfg.begin())
		return NULL;
	it--;

	return &it->second;
}

int RandActivityQiYuanKongMingDengConfig::InitTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int section = 0;
		if (!GetSubNodeValue(data_element, "section", section) || section <= 0)
			return -1;

		RAQiYuanKongMingDengTaskCfg cfg;
		if (!GetSubNodeValue(data_element, "type", cfg.task_type) || cfg.task_type <= 0 || cfg.task_type >= RA_QIYUAN_KONGMINGDENG_TASK_TYPE_MAX)
			return -2;
		if (!GetSubNodeValue(data_element, "parameter", cfg.param) || cfg.param <= 0)
			return -3;

		m_task_cfg[section].push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityQiYuanKongMingDengConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0)
			return -1;

		RAQiYuanKongMingDengRewardCfg cfg;

		TiXmlElement * list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == list_element)
			return -2;

		TiXmlElement * item_element = list_element->FirstChildElement("reward_item");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
				return -3;

			cfg.reward.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		m_reward_cfg[section_start] = cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityQiYuanKongMingDengConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	unsigned int times = 0;
	if (!GetSubNodeValue(data_element, "time_start", times) || times < 0)
		return -1;
	m_other_cfg.time_start = times / 100 * 3600 + times % 100 * 60;

	if (!GetSubNodeValue(data_element, "time_end", times) || times < 0)
		return -2;
	m_other_cfg.time_end = times / 100 * 3600 + times % 100 * 60;

	if (!GetSubNodeValue(data_element, "reward_increase", m_other_cfg.reward_increase) || m_other_cfg.reward_increase <= 0)
		return -3;
	if (!GetSubNodeValue(data_element, "reward_consume", m_other_cfg.reward_consume) || m_other_cfg.reward_consume <= 0)
		return -4;

	return 0;
}
