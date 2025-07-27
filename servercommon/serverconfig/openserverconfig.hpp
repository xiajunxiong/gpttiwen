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
	time_t GetOpenServerSixTime(unsigned int* real_start_time = NULL) const; // ȡ6���ʱ��

private:

	time_t		m_server_real_start_time;				// ��ʽ����ʱ��
	time_t		m_server_real_combine_time;				// ��ʽ�Ϸ�ʱ��
};

#endif // __OPEN_SERVER_CONFIG_HPP__

