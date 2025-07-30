#include "randactivitycolorfullanternconfig.hpp"
#include "servercommon/roleactivity/raburningsummerparam.hpp"

RandActivityColorfulLanternConfig::RandActivityColorfulLanternConfig() : rate_count(0)
{
}

RandActivityColorfulLanternConfig::~RandActivityColorfulLanternConfig()
{
	std::vector<RAColorfulLanternSectionBaseCfg>().swap(m_section_base_cfg);
}

bool RandActivityColorfulLanternConfig::Init(TiXmlElement * RootElement, std::string * err)
{	
	PRE_LOAD_RA_CONFIG;
	 
	LOAD_RA_CONFIG("other", InitOtherCfg);	
	LOAD_RA_CONFIG("colorful_lantern", InitBaseCfg);
	LOAD_RA_CONFIG("lantern_configuration", InitLightCfg);

	return true;
}

const RAColorfulLanternBaseCfg * RandActivityColorfulLanternConfig::GetBaseCfg(RandActivityManager * ramgr, int task_id) const
{
	if (NULL == ramgr || task_id <= 0 || task_id > MAX_RA_COLORFUL_LANTERN_TASK_NUM) return NULL;
	
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_COLORFUL_LANTERN);
	for (int i = 0; i < m_section_base_cfg.size(); i++)
	{
		if (act_real_open_day >= m_section_base_cfg[i].section_start && act_real_open_day <= m_section_base_cfg[i].section_end)
		{
			std::map<int, RAColorfulLanternBaseCfg>::const_iterator it = m_section_base_cfg[i].m_task_cfg.find(task_id);
			if (it != m_section_base_cfg[i].m_task_cfg.end())
			{
				return &it->second;
			}
		}
	}

	return NULL;
}

int RandActivityColorfulLanternConfig::GetLightScoreNum(int type) const
{
	std::map<int, RAColorfulLanternLightCfg>::const_iterator it = m_light_cfg.find(type);
	if (it == m_light_cfg.end())
	{
		return 0;
	}

	return it->second.score;
}

void RandActivityColorfulLanternConfig::GetRefreshLightList(std::vector<int>& light_list) const
{
	// 开闭区间, 所以需要 + 1
	int refresh_num = (m_other_cfg.refresh_num_down == m_other_cfg.refresh_num_up) ? m_other_cfg.refresh_num_up : RandomNum(m_other_cfg.refresh_num_down, m_other_cfg.refresh_num_up + 1);	
	for (int i = 0; i < refresh_num; i++)
	{
		int rate_num = RandomNum(rate_count);
		for (std::map<int, RAColorfulLanternLightCfg>::const_iterator it = m_light_cfg.begin(); it != m_light_cfg.end(); it++)
		{
			if (rate_num < it->second.rate)
			{
				light_list.push_back(it->first);
				break;
			}
			rate_num -= it->second.rate;
		}
	}
}

void RandActivityColorfulLanternConfig::GetTaskList(RandActivityManager * ramgr, std::map<int, RAColorfulLanternBaseCfg>& task_list) const
{
	if(NULL == ramgr) return;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_COLORFUL_LANTERN);
	for (int i = 0; i < m_section_base_cfg.size(); i++)
	{
		if (act_real_open_day >= m_section_base_cfg[i].section_start && act_real_open_day <= m_section_base_cfg[i].section_end)
		{
			task_list = m_section_base_cfg[i].m_task_cfg;
			break;
		}
	}
}

int RandActivityColorfulLanternConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "refresh_time", m_other_cfg.refresh_time) || m_other_cfg.refresh_time <= 0) return -1;
	if (!GetSubNodeValue(data_element, "refresh_num_down", m_other_cfg.refresh_num_down) || m_other_cfg.refresh_num_down <= 0) return -2;
	if (!GetSubNodeValue(data_element, "refresh_num_up", m_other_cfg.refresh_num_up) || m_other_cfg.refresh_num_up < m_other_cfg.refresh_num_down) return -3;
	if (!GetSubNodeValue(data_element, "game_time", m_other_cfg.game_time) || m_other_cfg.game_time <= 0) return -4;
	
	return 0;
}

int RandActivityColorfulLanternConfig::InitBaseCfg(TiXmlElement * RootElement)
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

		if (m_section_base_cfg.size() > 0)
		{
			int pre_index = m_section_base_cfg.size() - 1;
			if (section_start != m_section_base_cfg[pre_index].section_start ||
				section_end != m_section_base_cfg[pre_index].section_end)
			{
				if (section_start < m_section_base_cfg[pre_index].section_end)
				{
					return -777;
				}

				m_section_base_cfg.push_back(RAColorfulLanternSectionBaseCfg());
			}
		}
		if (m_section_base_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_base_cfg.push_back(RAColorfulLanternSectionBaseCfg());
		}

		RAColorfulLanternSectionBaseCfg & node_cfg = m_section_base_cfg[m_section_base_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		int task_id = 0;
		if (!GetSubNodeValue(data_element, "task_id", task_id) || task_id <= 0 || task_id > MAX_RA_COLORFUL_LANTERN_TASK_NUM) return -1;

		RAColorfulLanternBaseCfg & base_cfg = node_cfg.m_task_cfg[task_id];
		if (!GetSubNodeValue(data_element, "task_type", base_cfg.task_type) || base_cfg.task_type <= COLORFUL_LANTERN_TASK_TYPE_BEGIN || base_cfg.task_type >= COLORFUL_LANTERN_TASK_TYPE_MAX) return -2;
		if (!GetSubNodeValue(data_element, "task_param", base_cfg.task_param) || base_cfg.task_param <= COLORFUL_LANTERN_TASK_MODE_BEGIN || base_cfg.task_param >= COLORFUL_LANTERN_TASK_MODE_MAX) return -3;
		if (!GetSubNodeValue(data_element, "param_1", base_cfg.param1) || base_cfg.param1 <= 0) return -4;
		if (!GetSubNodeValue(data_element, "param_2", base_cfg.param2) || base_cfg.param2 < 0) return -5;
		
		//如果要允许无奖励的话,发奖励那需要改下,否则每次都会发文本邮件
		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element)
		{
			return -6;
		}
		if (!base_cfg.reward_item.ReadConfig(reward_element))
		{
			return -7;
		}
	
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityColorfulLanternConfig::InitLightCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	rate_count = 0;
	while (NULL != data_element)
	{
		int type = 0;
		if (!GetSubNodeValue(data_element, "type", type) || type < 0 || type >= MAX_RA_COLORFUL_LANTERN_LIGHT_TYPE_NUM) return -1;

		if (m_light_cfg.end() != m_light_cfg.find(type))
		{
			return -2;
		}
		RAColorfulLanternLightCfg & node_cfg = m_light_cfg[type];
		if (!GetSubNodeValue(data_element, "score", node_cfg.score) || node_cfg.score < 0) return -3;
		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate <= 0) return -4;

		rate_count+= node_cfg.rate;
		data_element = data_element->NextSiblingElement();
	}
	if (m_light_cfg.empty())
	{
		return -66;
	}

	return 0;
}
