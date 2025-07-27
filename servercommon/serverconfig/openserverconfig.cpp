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
		// �������ˣ�������ԭʼ�ķ���(��������������Է���һ��
		return 6 * 3600 + this->GetOpenServerZeroTime(); 
	}
	else
	{
		// ��ȡ����0���ʱ�� 
		time_t zero_time = server_real_start_time
			- 3600 * start_server_tm.tm_hour 
			- 60 * start_server_tm.tm_min 
			- start_server_tm.tm_sec;

		// ��ȡ����6���ʱ��
		time_t six_time = zero_time + 6 * 3600;
		
		// �������
		// 1. ����ʱ����ڵ���6�� ��ֱ�ӷ���6���ʱ���
		// 2. ����ʱ��С��6�㣬���24Сʱ��������ǰһ�쿪�ķ���
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
