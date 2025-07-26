#ifndef __DB_SESSION_RECONNECTOR_HPP__
#define __DB_SESSION_RECONNECTOR_HPP__

#include <string>
#include <list>
#include "irmimodule.h"
#include "itimermodule.h"
#include "servercommon/servercommon.h"

class DBSessionReconnector
{
public:
	struct ReconnectInfo
	{
		std::string ip;
		Port port;
		int reconnect_handle;
	};

	typedef std::list<ReconnectInfo> ReconnectList;

public:
	static DBSessionReconnector& Instance();

	void Update(unsigned int now);
	bool AddReconnectInfo(const std::string& ip, Port port, ARG_OUT int* reconnect_handle);
	void DoReconnect();
	inline bool NeedReconnect() const { return !m_reconnect_list.empty(); }
private:
	DBSessionReconnector();
	~DBSessionReconnector();
	NONCOPYABLE(DBSessionReconnector);

	int GenerateReconnectHandle();

	unsigned int m_next_reconnect_timestamp;
	int m_reconnect_handle_generator;
	ReconnectList m_reconnect_list;
};

class DBSessionReconnectTimer : public ITimerCallback
{
public:
	DBSessionReconnectTimer(DBSessionReconnector* reconnector) : m_reconnector(reconnector) {}
	virtual ~DBSessionReconnectTimer() {}

	virtual void OnTimer();
	virtual void Free();

private:
	DBSessionReconnector* m_reconnector;
};

#endif