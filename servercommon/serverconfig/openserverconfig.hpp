#ifndef __OPEN_SERVER_CONFIG_HPP__
#define __OPEN_SERVER_CONFIG_HPP__

#include "servercommon/configcommon.h"

class OpenServerConfig
{
public:
	OpenServerConfig();
	~OpenServerConfig();

	bool Init(const std::string &path, std::string *err);

	time_t GetServerRealStartTime() const { return m_server_real_start_time; }
	time_t GetServerRealCombineTime() const { return m_server_real_combine_time; }
	time_t GetOpenServerZeroTime(unsigned int* real_start_time = NULL) const;
	time_t GetOpenServerSixTime(unsigned int* real_start_time = NULL) const; // 取6点的时间

private:

	time_t		m_server_real_start_time;				// 正式开服时间
	time_t		m_server_real_combine_time;				// 正式合服时间
};

#endif // __OPEN_SERVER_CONFIG_HPP__

