#include "dbsessionreconnector.hpp"
#include "engineadapter.h"
#include "internalcomm.h"
#include "ilogmodule.h"
#include "servercommon/dbhandler.h"

DBSessionReconnector::DBSessionReconnector() : m_reconnect_handle_generator(0)
{

}

DBSessionReconnector::~DBSessionReconnector()
{

}

DBSessionReconnector& DBSessionReconnector::Instance()
{
	static DBSessionReconnector inst;
	return inst;
}

void DBSessionReconnector::Update(unsigned int now)
{
	if (0u == m_next_reconnect_timestamp || now < m_next_reconnect_timestamp) return;

	if (!this->NeedReconnect()) return;

	this->DoReconnect();

	if (!this->NeedReconnect())
	{
		m_next_reconnect_timestamp = 0u;
	}
	else
	{
		m_next_reconnect_timestamp = now + 10;
	}
}

bool DBSessionReconnector::AddReconnectInfo(const std::string& ip, Port port, ARG_OUT int* reconnect_handle)
{
	static ReconnectInfo info;
	info.ip = ip;
	info.port = port;
	info.reconnect_handle = this->GenerateReconnectHandle();
	m_reconnect_list.push_back(info);

	if (NULL != reconnect_handle) * reconnect_handle = info.reconnect_handle;

	if (0u == m_next_reconnect_timestamp)
	{
		m_next_reconnect_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 10;
	}

	EngineAdapter::Instance().GetLogModule()->printf(LL_MAINTANCE, 0, "DBSessionReconnector::AddReconnectInfo ip[%s] port[%d] reconnect_handle[%d]",
		ip.c_str(), port, info.reconnect_handle);

	return true;
}

void DBSessionReconnector::DoReconnect()
{
	ReconnectList::iterator it = m_reconnect_list.begin();
	for (; it != m_reconnect_list.end();)
	{
		const ReconnectInfo& info = *it;
		DBSession* db_session = InternalComm::Instance().GetDBSessionByReconnectHandle(info.reconnect_handle);
		if (NULL == db_session)
		{
			// �Ҳ����ˣ���֤��session�ѱ����¸��ǣ�����Ҫ������
			EngineAdapter::Instance().GetLogModule()->printf(LL_MAINTANCE, 0, 
				"DBSessionReconnector::DoReconnect db_session not found, erase it. ip[%s] port[%d] reconnect_handle[%d]",
				info.ip.c_str(), info.port, info.reconnect_handle);

			it = m_reconnect_list.erase(it);
			continue;
		}
		else
		{
			rmi::Session session = EngineAdapter::Instance().RMICreateSession(info.ip.c_str(), info.port);
			if (0 == session.handle)
			{
				// û�гɹ����ϣ���ŵ��б���󣬲��˳�
				//����Ϊ�����첽�����ӣ�����ÿ��ֻ��������һ�������⿨ס�߼��߳�̫��ʱ�䣩
				EngineAdapter::Instance().GetLogModule()->printf(LL_MAINTANCE, 0, 
					"DBSessionReconnector::DoReconnect FAIL, will try later. ip[%s] port[%d] reconnect_handle[%d]",
					info.ip.c_str(), info.port, info.reconnect_handle);
				
				ReconnectInfo info_2 = info;
				it = m_reconnect_list.erase(it);
				m_reconnect_list.push_back(info_2);
				return; 
			}
			else
			{
				// ���ӳɹ��Ļ������Լ�����һ��
				EngineAdapter::Instance().GetLogModule()->printf(LL_MAINTANCE, 0,
					"DBSessionReconnector::DoReconnect SUCC. ip[%s] port[%d] reconnect_handle[%d] session_netid[%d]",
					info.ip.c_str(), info.port, info.reconnect_handle, session.netid);

				db_session->reconnect_handle = -1;
				db_session->session = session;
				it = m_reconnect_list.erase(it);
				continue;
			}
		}
	}
}

int DBSessionReconnector::GenerateReconnectHandle()
{
	int ret = ++m_reconnect_handle_generator;
	if (m_reconnect_handle_generator >= MAX_INT)
	{
		m_reconnect_handle_generator = 0;
	}

	return ret;
}
