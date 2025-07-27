#ifndef __DBHANDLER_H__
#define __DBHANDLER_H__

#include "irmimodule.h"
#include <map>

struct DBSession
{
	DBSession() : port(-1), reconnect_handle(-1) {}
	DBSession(const rmi::Session& _session, const std::string& _ip, Port _port)
		: session(_session), ip(_ip), port(_port), reconnect_handle(-1) {}

	bool IsNeedReconnect() const { return reconnect_handle >= 0; }

	rmi::Session session;
	std::string ip;
	Port port;
	int reconnect_handle;
};

struct DBHandler
{
	static const int MAX_ROLE_DB_INDEX = 128;

	DBSession m_accounter;
	DBSession m_name;
	DBSession m_global;
	DBSession m_system;

	DBSession m_role[MAX_ROLE_DB_INDEX];
	unsigned int m_role_session_num;

	DBSession m_cross;
	std::map<long long, DBSession> m_original;
	std::map<long long, DBSession> m_original_acount;

	void Init(const DBSession &accounter, DBSession *role_db, unsigned int role_db_num, const DBSession &global_session,
		const DBSession &system_session, const DBSession &name_session, const DBSession &cross_session = DBSession())
	{
		m_accounter = accounter;

		for (unsigned int i = 0; i < (unsigned int)MAX_ROLE_DB_INDEX && i < role_db_num; ++i)
		{
			m_role[i] = role_db[i];
		}

		m_role_session_num = role_db_num;
		m_global = global_session;
		m_system = system_session;
		m_name = name_session;

		m_cross = cross_session;
	}

	const rmi::Session &GetAcounterDB() const{ return m_accounter.session; }
	const rmi::Session &GetGlobalDB() const { return m_global.session; }
	const rmi::Session &GetSystemDB() const { return m_system.session; }
	const rmi::Session &GetNameDB() const { return m_name.session; }
	const rmi::Session &GetRoleDB() const { return m_role[0].session; }
	const rmi::Session &GetCrossDB() const { return m_cross.session; }
	const DBSession& GetRoleDBSession() const { return m_role[0]; }
	const DBSession& GetGlobalDBSession() const { return m_global; }

	void RegisterOriginalDB(long long unique_server_index, const DBSession &oiginal_db)
	{
		m_original[unique_server_index] = oiginal_db;
	}

	void RegisterOriginalAccountDB(long long unique_server_index, const DBSession &oiginal_db)
	{
		m_original_acount[unique_server_index] = oiginal_db;
	}

	const rmi::Session *GetOriginalDB(long long unique_server_index) const
	{
		std::map<long long, DBSession>::const_iterator c_it = m_original.find(unique_server_index);
		if (c_it == m_original.end())
		{
			return NULL;
		}

		return &c_it->second.session;
	}

	const rmi::Session *GetAccountOriginalDB(long long unique_server_index) const
	{
		std::map<long long, DBSession>::const_iterator c_it = m_original_acount.find(unique_server_index);
		if (c_it == m_original_acount.end())
		{
			return NULL;
		}

		return &c_it->second.session;
	}

	DBSession* GetDBSession(NetID netid)
	{
		if (netid == m_accounter.session.netid) return &m_accounter;
		if (netid == m_name.session.netid) return &m_name;
		if (netid == m_global.session.netid) return &m_global;
		if (netid == m_system.session.netid) return &m_system;
		if (netid == m_cross.session.netid) return &m_cross;

		for (unsigned int i = 0; i < m_role_session_num; ++i)
		{
			if (netid == m_role[i].session.netid)
			{
				return &m_role[i];
			}
		}

		std::map<long long, DBSession>::iterator it = m_original.begin();
		for (; it != m_original.end(); ++it)
		{
			if (netid == it->second.session.netid)
			{
				return &it->second;
			}
		}

		it = m_original_acount.begin();
		for (; it != m_original_acount.end(); ++it)
		{
			if (netid == it->second.session.netid)
			{
				return &it->second;
			}
		}

		return NULL;
	}

	DBSession* GetDBSessionByReconnectHandle(int reconnect_handle)
	{
		if (reconnect_handle == m_accounter.reconnect_handle) return &m_accounter;
		if (reconnect_handle == m_name.reconnect_handle) return &m_name;
		if (reconnect_handle == m_global.reconnect_handle) return &m_global;
		if (reconnect_handle == m_system.reconnect_handle) return &m_system;
		if (reconnect_handle == m_cross.reconnect_handle) return &m_cross;

		for (unsigned int i = 0; i < m_role_session_num; ++i)
		{
			if (reconnect_handle == m_role[i].reconnect_handle)
			{
				return &m_role[i];
			}
		}

		std::map<long long, DBSession>::iterator it = m_original.begin();
		for (; it != m_original.end(); ++it)
		{
			if (reconnect_handle == it->second.reconnect_handle)
			{
				return &it->second;
			}
		}

		it = m_original_acount.begin();
		for (; it != m_original_acount.end(); ++it)
		{
			if (reconnect_handle == it->second.reconnect_handle)
			{
				return &it->second;
			}
		}

		return NULL;
	}

	void CloseAllSession(IRMIModule* rmi)
	{
		rmi->CloseSession(m_accounter.session);
		rmi->CloseSession(m_name.session);
		rmi->CloseSession(m_global.session);
		rmi->CloseSession(m_system.session);
		rmi->CloseSession(m_cross.session);

		for (unsigned int i = 0; i < m_role_session_num; ++i)
		{
			rmi->CloseSession(m_role[i].session);
		}

		std::map<long long, DBSession>::iterator it = m_original.begin();
		for (; it != m_original.end(); ++it)
		{
			rmi->CloseSession(it->second.session);
		}

		it = m_original_acount.begin();
		for (; it != m_original_acount.end(); ++it)
		{
			rmi->CloseSession(it->second.session);
		}
	}
};

struct BigCrossDBHandler
{
	DBSession m_big_cross;

	void Init(const DBSession &_big_cross)
	{
		m_big_cross = _big_cross;
	}

	const rmi::Session &GetBigCrossDB() const { return m_big_cross.session; }

	DBSession* GetDBSession(NetID netid)
	{
		if (netid == m_big_cross.session.netid) 
			return &m_big_cross;

		return NULL;
	}

	DBSession* GetDBSessionByReconnectHandle(int reconnect_handle)
	{
		if (reconnect_handle == m_big_cross.reconnect_handle) 
			return &m_big_cross;

		return NULL;
	}

	void CloseAllSession(IRMIModule* rmi)
	{
		rmi->CloseSession(m_big_cross.session);
	}
};

#endif


