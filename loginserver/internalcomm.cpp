#include "common/md5/md5cpp.h"
#include "internalcomm.h"
#include <sstream>

void InternalComm::Init(INetworkModule *internal_network, const std::string &session_key_base)
{
	m_internal_network = internal_network;
	MD5 md5;
	MD5_buffer(&md5, session_key_base.c_str(), (unsigned long)session_key_base.size());
	MD52String(&m_session_key_base, md5);
}

void InternalComm::SetDBSession(const DBSession &accounter, DBSession *role_db, unsigned int role_db_num, const DBSession &global_session,
								const DBSession &system_session, const DBSession &name_session)
{
	m_db_handler.Init(accounter, role_db, role_db_num, global_session, system_session, name_session);
}

DBSession* InternalComm::GetDBSession(NetID netid)
{
	return m_db_handler.GetDBSession(netid);
}

DBSession* InternalComm::GetDBSessionByReconnectHandle(int reconnect_handle)
{
	return m_db_handler.GetDBSessionByReconnectHandle(reconnect_handle);
}

void InternalComm::CloseAllDBSession(IRMIModule* rmi)
{
	m_db_handler.CloseAllSession(rmi);
}
