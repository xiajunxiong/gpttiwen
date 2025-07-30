#include "randactivityopencfg.hpp"
#include "config/sharedconfig/sharedconfig.h"
#include "engineadapter.h"
#include "gamelog.h"

RandActivityOpenCfg::RandActivityOpenCfg() : m_begin_day_idx(0), m_end_day_idx(0), m_rand_activity_cfg_switch_timestamp(0)
{

}

RandActivityOpenCfg::~RandActivityOpenCfg()
{
	for (int i = 0; i < ARRAY_LENGTH(m_open_detail_list); i++)
	{
		std::vector<RandActivityOpenDetail>().swap(m_open_detail_list[i]);
	}
}

bool RandActivityOpenCfg::Init(const std::string &path, std::string *err)
{
	int iRet = 0;
	char err_info[1024] = {0};

	TiXmlDocument document;
	if ("" == path || !document.LoadFile(path))
	{
		sprintf(err_info, "%s load RandActivityOpenCfg fail !\n %s", path.c_str(), document.ErrorDesc());
		*err = err_info;
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	if (NULL == RootElement)
	{
		*err = path + " xml root node error";
		return false;
	}

	{
		// 杂项配置
		TiXmlElement *root_element = RootElement->FirstChildElement("other");
		if (NULL == root_element)
		{
			*err = path + " xml not other node ";
			return false;
		}

		iRet = this->InitOtherCfg(root_element);
		if (0 != iRet)
		{
			sprintf(err_info, "%s InitOtherCfg fail %d ", path.c_str(), iRet);
			*err = err_info;
			return false;
		}
	}

	{
		// 活动开服天数限制
		TiXmlElement *root_element = RootElement->FirstChildElement("begin_day_cfg");
		if (NULL == root_element)
		{
			*err = path + " xml not begin_day_cfg node ";
			return false;
		}

		iRet = this->InitOpenDayLimitCfg(root_element);
		if (0 != iRet)
		{
			sprintf(err_info, "%s InitOpenDayLimitCfg fail %d ", path.c_str(), iRet);
			*err = err_info;
			return false;
		}
	}

	{
		// 基于开服开启配置
		TiXmlElement *root_element = RootElement->FirstChildElement("open_cfg");
		if (NULL == root_element)
		{
			*err = path + " xml not base_on_open_server_cfg node ";
			return false;
		}

		iRet = this->InitBaseOnServerOpenCfg(root_element);
		if (0 != iRet)
		{
			sprintf(err_info, "%s InitBaseOnServerOpenCfg fail %d ", path.c_str(), iRet);
			*err = err_info;
			return false;
		}
	}

	{
		// 基于日期的开启配置
		TiXmlElement *root_element = RootElement->FirstChildElement("base_on_day_cfg");
		if (NULL == root_element)
		{
			*err = path + " xml not base_on_day_cfg node ";
			return false;
		}

		iRet = this->InitBaseOnDayCfg(root_element);
		if (0 != iRet)
		{
			sprintf(err_info, "%s InitBaseOnDayCfg fail %d ", path.c_str(), iRet);
			*err = err_info;
			return false;
		}
	}

	if (!this->CheckActivityTimeValid())
	{
		sprintf(err_info, "CheckActivityTimeValid FAILED");
		*err = err_info;
		return false;
	}

	return true;
}

int RandActivityOpenCfg::InitOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL != dataElement)
	{
		if (!GetSubNodeValue(dataElement, "begin_day_idx", m_begin_day_idx) ||m_begin_day_idx < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "end_day_idx", m_end_day_idx) || m_end_day_idx <= m_begin_day_idx)
		{
			return -2;
		}

		// allow_set_time_dayidx 可以用 m_end_day_idx来代替使用，所以废弃这个配置
		// 		if (!GetSubNodeValue(dataElement, "allow_set_time_dayidx", m_allow_set_time_dayidx) || m_allow_set_time_dayidx < 0)
		// 		{
		// 			return -3;
		// 		}

		time_t zero_timestamp = SHAREDCONFIG->GetOpenServerConfig().GetOpenServerZeroTime();

		m_rand_activity_cfg_switch_timestamp = zero_timestamp + SECOND_PER_DAY * m_end_day_idx;
	}

	return 0;
}

int RandActivityOpenCfg::InitBaseOnServerOpenCfg(TiXmlElement *RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -100000;
	}

	while (NULL != dataElement)
	{
		int activity_type = 0;
		if (!GetSubNodeValue(dataElement, "activity_type", activity_type) || activity_type < RAND_ACTIVITY_TYPE_BEGIN || activity_type >= RAND_ACTIVITY_TYPE_END)
		{
			return -1;
		}

		RandActivityOpenDetail cfg_item;
		cfg_item.is_from_open_server_day_cfg = true;

		cfg_item.activity_type = activity_type;

		int begin_day_idx;
		int end_day_idx;

		if (!GetSubNodeValue(dataElement, "begin_day_idx", begin_day_idx) || begin_day_idx < m_begin_day_idx)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "end_day_idx", end_day_idx) || begin_day_idx >= end_day_idx)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "open_type", cfg_item.open_type) || cfg_item.open_type < RAND_ACTIVITY_OPEN_TYPE_BEGIN || cfg_item.open_type > RAND_ACTIVITY_OPEN_TYPE_END)
		{
			return -5;
		}

		cfg_item.sevenday_open = true; // 基于开服天数的活动，必定允许前七天开

		unsigned int zero_timestamp = static_cast<unsigned int>(SHAREDCONFIG->GetOpenServerConfig().GetOpenServerZeroTime());

		cfg_item.begin_timestamp = static_cast<unsigned int>(zero_timestamp + SECOND_PER_DAY * begin_day_idx);
		cfg_item.end_timestamp = static_cast<unsigned int>(zero_timestamp + SECOND_PER_DAY * end_day_idx);

		m_open_detail_list[activity_type - RAND_ACTIVITY_TYPE_BEGIN].push_back(cfg_item);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int RandActivityOpenCfg::InitBaseOnDayCfg(TiXmlElement *RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return 0; // 允许空配置
	}

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	while (NULL != dataElement)
	{
		int activity_type = 0;
		if (!GetSubNodeValue(dataElement, "activity_type", activity_type) || activity_type <= RAND_ACTIVITY_TYPE_BEGIN || activity_type >= RAND_ACTIVITY_TYPE_END)
		{
			return -1;
		}

		RandActivityOpenDetail cfg_item;

		if (!GetSubNodeValue(dataElement, "open_type", cfg_item.open_type) || cfg_item.open_type < RAND_ACTIVITY_OPEN_TYPE_BEGIN || cfg_item.open_type > RAND_ACTIVITY_OPEN_TYPE_END)
		{
			return -3;
		}

		cfg_item.activity_type = activity_type;

		int sevenday_open = 0;
		if (!GetSubNodeValue(dataElement, "sevenday_open", sevenday_open) || sevenday_open < 0 || sevenday_open > 1)
		{
			return -4;
		}

		cfg_item.sevenday_open = (sevenday_open > 0);

		{
			TiXmlElement *BeginTimeElement = dataElement->FirstChildElement("begin_day");
			if (!BeginTimeElement)
			{
				return -5;
			}

			time_t begin_time = 0;
			if (!ConvertTimeStringToUnixTime2(BeginTimeElement->FirstChild()->Value(), &begin_time))
			{
				return -6;
			}

			cfg_item.begin_timestamp = static_cast<unsigned int>(begin_time);
		}

		{
			TiXmlElement *EndTimeElement = dataElement->FirstChildElement("end_day");
			if (!EndTimeElement)
			{
				return -7;
			}

			time_t end_time = 0;
			if (!ConvertTimeStringToUnixTime2(EndTimeElement->FirstChild()->Value(), &end_time))
			{
				return -8;
			}
		
			cfg_item.end_timestamp = static_cast<unsigned int>(end_time);
		}

		if (cfg_item.begin_timestamp >= cfg_item.end_timestamp)
		{
			return -9;
		}

		if (cfg_item.end_timestamp < now) // 时间已过，就不需要读入配置了
		{
			dataElement = dataElement->NextSiblingElement();
			continue;
		}

		int limit_open_days = this->GetLimitOpenDay(activity_type);
		if (limit_open_days > 0)
		{
			unsigned int zero_timestamp = static_cast<unsigned int>(SHAREDCONFIG->GetOpenServerConfig().GetOpenServerZeroTime());
			unsigned int allow_begin_timestamp = zero_timestamp + (limit_open_days - 1) * SECOND_PER_DAY;
			if (allow_begin_timestamp > cfg_item.begin_timestamp)
			{
				if (allow_begin_timestamp >= cfg_item.end_timestamp)
				{
					gamelog::g_log_world.printf(LL_INFO,
						"RandActivityOpenCfg::InitBaseOnDayCfg[Ignore] activity[%d] limit_open_days[%d] allow_open_time[%u] cfg_begin_time[%u]",
						activity_type, limit_open_days, allow_begin_timestamp, cfg_item.begin_timestamp);

					dataElement = dataElement->NextSiblingElement();
					continue;
				}

				cfg_item.begin_timestamp = allow_begin_timestamp;
			}
		}

		m_open_detail_list[activity_type - RAND_ACTIVITY_TYPE_BEGIN].push_back(cfg_item);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int RandActivityOpenCfg::InitOpenDayLimitCfg(TiXmlElement *RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return 0; // 允许空配置
	}

	while (NULL != dataElement)
	{
		int activity_type = 0;
		if (!GetSubNodeValue(dataElement, "activity_type", activity_type) || activity_type <= RAND_ACTIVITY_TYPE_BEGIN || activity_type >= RAND_ACTIVITY_TYPE_END)
		{
			return -1;
		}

		int begin_day = 0;
		if (!GetSubNodeValue(dataElement, "begin_day", begin_day))
		{
			return -2;
		}

		m_ra_open_day_limit_map[activity_type] = begin_day;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

bool RandActivityOpenCfg::CheckActivityTimeValid()
{
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_open_detail_list); ++i)
	{
		if (!this->CheckActivityTimeValid(m_open_detail_list[i]))
		{
			return false;
		}
	}

	return true;
}

bool RandActivityOpenCfg::CheckActivityTimeValid(const std::vector<RandActivityOpenDetail>& cfg_vec)
{
	for (size_t i = 0; i < cfg_vec.size(); ++i)
	{
		const RandActivityOpenDetail& detail_i = cfg_vec[i];
		for (size_t k = i + 1; k < cfg_vec.size(); ++k)
		{
			const RandActivityOpenDetail& detail_k = cfg_vec[k];
			if (this->IsActivityTimeConflict(detail_i, detail_k))
			{
				printf("RandActivityOpenCfg::CheckActivityTimeValid Time Conflict activity[%d]\n", detail_i.activity_type);
				return false;
			}
		}
	}

	return true;
}

bool RandActivityOpenCfg::IsActivityTimeConflict(const RandActivityOpenDetail& cfg_1, const RandActivityOpenDetail& cfg_2)
{
	if (cfg_1.begin_timestamp >= cfg_2.begin_timestamp && cfg_1.begin_timestamp < cfg_2.end_timestamp)
	{
		return true;
	}

	if (cfg_2.begin_timestamp >= cfg_1.begin_timestamp && cfg_2.begin_timestamp < cfg_1.end_timestamp)
	{
		return true;
	}

	return false;
}

const std::vector<RandActivityOpenDetail>* RandActivityOpenCfg::GetRaOpenCfg(int activity_type)
{
	if (activity_type <= RAND_ACTIVITY_TYPE_BEGIN || activity_type >= RAND_ACTIVITY_TYPE_END)
	{
		return NULL;
	}

	return &m_open_detail_list[activity_type - RAND_ACTIVITY_TYPE_BEGIN];
}

int RandActivityOpenCfg::GetLimitOpenDay(int activity_type)
{
	std::map<int, int>::iterator it = m_ra_open_day_limit_map.find(activity_type);
	if (it != m_ra_open_day_limit_map.end())
	{
		return it->second;
	}

	return 0;
}

