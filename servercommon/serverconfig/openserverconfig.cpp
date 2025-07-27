#include "openserverconfig.hpp"

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
//#include "engineadapter.h"

#include <time.h>

OpenServerConfig::OpenServerConfig()
	: m_server_real_start_time(0), m_server_real_combine_time(0)
{

}

OpenServerConfig::~OpenServerConfig()
{

}

bool OpenServerConfig::Init(const std::string &path, std::string *err)
{
	TiXmlDocument document;
	if (path == "" || !document.LoadFile(path))
	{
		*err = path + " Load fail: " +document.ErrorDesc();
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	if (!RootElement)
	{
		*err = path + ": xml node error in root";
		return false;
	}

	{
		m_server_real_start_time = 0;

		TiXmlElement *StartTimeElement = RootElement->FirstChildElement("server_real_start_time");
		if (NULL == StartTimeElement)
		{
			*err = path + ": xml node error in [config->server_real_start_time]";
			return false;
		}

		if (NULL != StartTimeElement->FirstChild())
		{
			const char* value = StartTimeElement->FirstChild()->Value();
			bool cmp = (strcmp(value, "") == 0 || strcmp(value, "0") == 0);
			if (!cmp)
			{
				if (!ConvertTimeStringToUnixTime(StartTimeElement->FirstChild()->Value(), &m_server_real_start_time))
				{
					*err = path + ": xml node error in [config->server_real_start_time], time format error";
					return false;
				}
#ifndef _DEBUG
				if (m_server_real_start_time <= 1577808000)
				{
					*err = path + ": xml node error in [config->server_real_start_time], times error";
					return false;
				}
#endif
			}
		}
		else
		{
			*err = path + ": xml node error in [config->server_real_start_time].";
			return false;
		}
	}

	{
		m_server_real_combine_time = 0;

		TiXmlElement *CombineTimeElement = RootElement->FirstChildElement("server_real_combine_time");
		if (NULL == CombineTimeElement)
		{
			*err = path + ": xml node error in [config->server_real_combine_time]";
			return false;
		}

		if (NULL != CombineTimeElement->FirstChild())
		{
			const char* value = CombineTimeElement->FirstChild()->Value();
			bool cmp = (strcmp(value, "") == 0 || strcmp(value, "0") == 0);
			if (!cmp)
			{
				if (!ConvertTimeStringToUnixTime(CombineTimeElement->FirstChild()->Value(), &m_server_real_combine_time))
				{
					*err = path + ": xml node error in [config->server_real_combine_time], time format error";
					return false;
				}
			}
		}
	}

	return true;
}

time_t OpenServerConfig::GetOpenServerZeroTime(unsigned int* real_start_time /* = NULL */) const
{
	unsigned server_real_start_time = m_server_real_start_time;
	if (NULL != real_start_time)
	{
		server_real_start_time = *real_start_time;
	}

	time_t now_second = 0;/* = EngineAdapter::Instance().Time();*/
	time(&now_second);
	time_t today_zero_timestamp = GetZeroTime(now_second);
	int open_day = GetDayIndex(server_real_start_time, now_second);
	time_t open_server_zero_timestamp = today_zero_timestamp - SECOND_PER_DAY * open_day;

	return open_server_zero_timestamp;
}

time_t OpenServerConfig::GetOpenServerSixTime(unsigned int* real_start_time /* = NULL */) const
{
	unsigned server_real_start_time = m_server_real_start_time;
	if (NULL != real_start_time)
	{
		server_real_start_time = *real_start_time;
	}
	tm start_server_tm;
	memset((void*)&start_server_tm, 0, sizeof(start_server_tm));
	bool ret = GetTimeTM(server_real_start_time, start_server_tm);
	if (!ret)
	{
		// 若出错了，则用最原始的方法(正常不会出错，但以防万一）
		return 6 * 3600 + this->GetOpenServerZeroTime(); 
	}
	else
	{
		// 获取当天0点的时间 
		time_t zero_time = server_real_start_time
			- 3600 * start_server_tm.tm_hour 
			- 60 * start_server_tm.tm_min 
			- start_server_tm.tm_sec;

		// 获取当天6点的时间
		time_t six_time = zero_time + 6 * 3600;
		
		// 两种情况
		// 1. 开服时间大于等于6点 则直接返回6点的时间戳
		// 2. 开服时间小于6点，则减24小时（当作是前一天开的服）
		if (start_server_tm.tm_hour >= 6)
		{
			return six_time;
		}
		else
		{
#ifdef _DEBUG
			assert(six_time >= 24 * 3600);
#endif
			return six_time - 24 * 3600;
		}
	}
}
