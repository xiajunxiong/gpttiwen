#ifndef INTERNALCOMM_H
#define INTERNALCOMM_H

#include "inetworkmodule.h"
#include <string>

#include "servercommon/dbhandler.h"
#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

class IMessageSerializer;
class IMessageSerializer2;
class InternalComm
{
public:
	static InternalComm &Instance()
	{
		static InternalComm ic;
		return ic;
	}

	void Init(INetworkModule *internal_network);

	bool NetListen(Port port, int backlog, NetID *netid_out = 0)
	{
		return m_internal_network->Listen(port, backlog, netid_out);
	}

	bool NetConnect(const char *ip, Port port, NetID *netid_out, unsigned long time_out = 3000)
	{
		return m_internal_network->Connect(ip, port, netid_out, time_out);
	}
	
	bool NetConnectAsyn(const char *ip, Port port, int *handle, unsigned long time_out=3000)
	{
		return m_internal_network->ConnectAsyn(ip, port, handle, time_out);
	}

	bool NetSend(NetID netid, const char *data, unsigned int length)
	{
#ifdef _DEBUG
		if (length > 100000)
		{
			printf("InternalComm::NetSend length=%u\n", length);
			PrintStackBackTrace(false);
		}
#endif
		return m_internal_network->Send(netid, data, length);
	}

	bool NetSendSerializeMsg(NetID netid, IMessageSerializer* msg);

	void NetDisconnect(NetID netid)
	{
		m_internal_network->Disconnect(netid);
	}

	void SetSesstionKeyBase(const std::string &session_key_base);
	void SetDBSession(const DBSession &accounter, DBSession *role_db, unsigned int role_db_num, const DBSession &global_session,
		const DBSession &systm_session, const DBSession &name_session);
	void RegisterOriginalDB(long long unique_server_index, const DBSession& original_session);
	const DBHandler &GetDBHandler() { return m_db_handler; }

	//!< Hidden����ԭ����db_session
	void SetOriginGameworldDBSession(const UniqueServerID& server_id, const DBSession &accounter, DBSession *role_db, unsigned int role_db_num, const DBSession &global_session,
		const DBSession &systm_session, const DBSession &name_session);
	const DBHandler* GetHiddenDBHandler(const UniqueServerID& server_id);
	void RemoveOriginalDB(const UniqueServerID& server_id);

	bool SendToAllGameUserThroughCross(int role_id, const char * data, int length);				//���������,���ת����������Ϸ��(�������ط�)�����
	bool SendToAllGameAllUserThroughCross(const char * data, int length);				//���������,���ת����������Ϸ��(�������ط�)���������
	bool SendToCross(const char *data, int length);

	bool SendToHiddenUserThroughCross(int uid, const void * data, int length);
	bool SendToHiddenThroughCross(const char* data, int length, long long send_to_uuid = 0);
	bool SendSerializeMsgToHiddenThroughCross(IMessageSerializer * data,long long send_to_uuid = 0);
	bool SendToHiddenAllUserThroughCross(const char* data, int length);						// ԭ���������ط��������
	bool SendToHiddenGuildMemberThroughCross(int guild_id, const char* data, int length);	// ԭ������ָ�����������г�Ա
	bool SendToHiddenUniqueServerUserThroughCross(const char* data, int length);
	bool SendSerializeMsgToHiddenUserThroughCross(int send_to_uid, IMessageSerializer * data);

	bool SendToAllGameThroughCross(const void * data, int length);
	bool SendToGameThroughCross(int origin_plat_type, int origin_server_id, const char* data, int length, int send_to_uid = 0);  //���ط�������Ϸ��
	bool SendToGameThroughCross2(const void * data, int length, int origin_server_id);  //���ط�������Ϸ��
	bool SendToGameThroughCrossByRoleId(const void * data, int length, int role_id);  //���ط�������Ϸ��
	bool SendSerializeMsgToGameThroughCross(IMessageSerializer * data, int server_id = 0);
	bool SendSerializeMsgToAllGameThroughCross(IMessageSerializer * data);
	bool SendToAllGameUserThroughCross(const char* data, int length);							//���ط�������Ϸ���������
	bool SendSerializeMsgToAllGameUserThroughCross(IMessageSerializer * data);		//���ط�������Ϸ���������
	bool SendToGameUserThroughCross(int send_to_uid, const void * data, int length);			//���ط�������Ϸ��ָ�����
	bool SendSerializeMsgToGameUserThroughCross(int send_to_uid, IMessageSerializer * data);	//���ط�������Ϸ��ָ�����
	bool SendToGameGuildMemberThroughCross(int guild_id, const void * data, int length);		//���ط�����ָ�����������г�Ա
	bool SendToGameAllUserThroughCross(int origin_plat_type, int origin_server_id, const char* data, int length);			//���ط�����ָ�������������
	bool UserMsgTransmitToCross(const UserID &user_id, int msg_type, const char *data, int length);

	bool SendSerializeMsgToHiddenThroughCross2(IMessageSerializer2 * data, long long send_to_uuid = 0);


	// BigCorss
	bool SendToBigCrossServer(const char *data, int length);
	bool SendSerializeMsgToBigCrossServer(IMessageSerializer * data);
	bool SendToBigCrossServerThroughHidden(const char* data, int length);
	bool SendToHiddenToBigCross(const char *data, int length); // 消息路径 : 主服->跨服->隐藏�?>bigcross
	bool TransferBigCrossMsgToAllOriginGame(const char* data, int length);
	bool TransferBigCrossMsgToOriginGame(int plat_type, int server_id, const char* data, int length);

	DBSession* GetDBSession(NetID netid);
	DBSession* GetDBSessionByReconnectHandle(int reconnect_handle);
	void CloseAllDBSession(IRMIModule* rmi);
	
protected:
	InternalComm();
	~InternalComm();

	DBHandler			m_db_handler;

	typedef std::map<UniqueServerID, DBHandler*> ServerDBMap;

public:
	INetworkModule		*m_internal_network;
	std::string			m_session_key_base;

	NetID				m_login_server_netid;
	NetID				m_cross_server_netid;
	NetID				m_bigc_server_netid;

	int					m_reconnet_login_handle;
	int					m_reconnet_cross_handle;
	int					m_reconnet_bigc_handle;

	char*				m_buffer;		// ����NetSend�����

	ServerDBMap			m_server_db_map;	//!< \note �������ط�������dataaccess
};

#endif

