
#include "servercommon/zhouqidef.hpp"
#include "zhouqiconfig.hpp"
#include "engineadapter.h"

ZhouQiConfig::ZhouQiConfig()
{
}

ZhouQiConfig::~ZhouQiConfig()
{
	std::vector<ZhouQiStartPointCfg>().swap(m_start_point_cfg);
}

bool ZhouQiConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("play_list", InitZhouQiCfg);
	LOAD_CONFIG("control_open", InitZhouQiStartPointCfg);
	LOAD_CONFIG("activity_list", InitZhouQiLimitItemCfg);
	LOAD_CONFIG("time_level", InitZhouQiOpenLimitCfg);	

	return true;
}

const ZhouQiCfg * ZhouQiConfig::GetZhouQiCfg(int zhou_qi_num)
{
	return MapValuesConstPtr(m_zhou_qi_cfg, zhou_qi_num);
}

bool ZhouQiConfig::GetZhouQiTimeInfo(time_t time, int * ARG_OUT zhou_qi_num_arg, time_t * ARG_OUT begin_time_arg, time_t * ARG_OUT end_time_arg)
{
	const ZhouQiStartPointCfg * cfg = this->GetZhouQiStartPointCfg(time);
	if (!cfg)
	{
		if (zhou_qi_num_arg)
		{
			*zhou_qi_num_arg = 0;
		}

		if (begin_time_arg)
		{
			*begin_time_arg = 0;
		}

		if (end_time_arg)
		{
			*end_time_arg = 0;
		}
	}
	else
	{
		if (zhou_qi_num_arg)
		{
			*zhou_qi_num_arg = cfg->zhou_num;
		}

		if (begin_time_arg)
		{
			*begin_time_arg = cfg->start_time;
		}

		if (end_time_arg)
		{
			*end_time_arg = cfg->end_time;
		}
	}

	return true;
}

const ZhouQiStartPointCfg * ZhouQiConfig::GetZhouQiStartPointCfg(time_t time)
{
	for (int i = 0; i < (int)m_start_point_cfg.size(); ++i)
	{
		const ZhouQiStartPointCfg & curr = m_start_point_cfg[i];
		if (curr.start_time <= time && time < curr.end_time)
		{
			return &curr;
		}
	}

	return NULL;
}

const ZhouQiLimitItemCfg * ZhouQiConfig::GetZhouQiLimitItemCfg(int item_id)
{
	return MapValuesConstPtr(m_zhou_qi_limit_item_cfg, item_id);
}

const ZhouQiOpenLimitCfg * ZhouQiConfig::GetZhouQiOpenLimitCfg(long long time)
{
	for (std::map<long long, ZhouQiOpenLimitCfg>::reverse_iterator iter = m_zhou_qi_open_limit_cfg.rbegin(); iter != m_zhou_qi_open_limit_cfg.rend(); ++iter)
	{
		if (time > iter->first)
		{
			return &iter->second;
		}
	}

	return NULL;
}

int ZhouQiConfig::InitZhouQiCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int zhou_qi_num = 0;
		if (!GetSubNodeValue(root_element, "period_type", zhou_qi_num))
		{
			ret_line;
		}

		ZhouQiCfg & cfg = m_zhou_qi_cfg[zhou_qi_num];
		ZhouQiPlayTypeCfg item_cfg;

		if (!GetSubNodeValue(root_element, "play_type", item_cfg.play_type))
		{
			ret_line;
		}

		unsigned int times = 0;
		if (!GetSubNodeValue(root_element, "start_time", times))
		{
			ret_line;
		}
		item_cfg.day_begin_time[0] = times / 100 * 3600 + times % 100 * 60;

		if (!GetSubNodeValue(root_element, "end_time", times))
		{
			ret_line;
		}
		item_cfg.day_end_time[0] = times / 100 * 3600 + times % 100 * 60;

		if (!GetSubNodeValue(root_element, "begin_time1", times))
		{
			ret_line;
		}
		item_cfg.day_begin_time[1] = times / 100 * 3600 + times % 100 * 60;

		if (!GetSubNodeValue(root_element, "end_time1", times))
		{
			ret_line;
		}
		item_cfg.day_end_time[1] = times / 100 * 3600 + times % 100 * 60;

		if (!GetSubNodeValue(root_element, "open_opint", item_cfg.start_day))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "open_days", item_cfg.end_day))
		{
			ret_line;
		}

		if (cfg.play_cfg.find(item_cfg.play_type) != cfg.play_cfg.end())
		{
			ret_line;
		}

		cfg.play_cfg[item_cfg.play_type] = item_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ZhouQiConfig::InitZhouQiStartPointCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		ZhouQiStartPointCfg cfg;

		if (!GetSubNodeValue(root_element, "period_type", cfg.zhou_num) || cfg.zhou_num <= 0 || cfg.zhou_num > ZHOU_QI_MAX)
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "start_time", cfg.start_time))
		{
			ret_line;
		}
		cfg.start_time = ::GetZeroTime(cfg.start_time);
		cfg.end_time = cfg.start_time + SECOND_PER_ZHOU_QI;

		if (!m_start_point_cfg.empty())
		{
			ZhouQiStartPointCfg last_cfg = m_start_point_cfg.back();
			if (cfg.start_time < last_cfg.end_time)
			{
				ret_line;
			}
		}

		m_start_point_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();

	}

	return 0;
}

int ZhouQiConfig::InitZhouQiLimitItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{

		ZhouQiLimitItemCfg cfg;

		if (!GetSubNodeValue(root_element, "recycle_coin_id", cfg.item_id))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "period_type", cfg.zhou_qi_num))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "period_name", cfg.zhou_zi_name))
		{
			ret_line;
		}

		m_zhou_qi_limit_item_cfg[cfg.item_id] = cfg;
		root_element = root_element->NextSiblingElement();

	}

	return 0;
}

int ZhouQiConfig::InitZhouQiOpenLimitCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{

		ZhouQiOpenLimitCfg cfg;

		if (!GetSubNodeValue(root_element, "time_stamp", cfg.time))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "level", cfg.need_role_level))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "start_time", cfg.need_hidden_open_server_day) || cfg.need_hidden_open_server_day <= 0)
		{
			ret_line;
		}

		m_zhou_qi_open_limit_cfg[cfg.time] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}


