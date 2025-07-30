#include "common/md5/md5cpp.h"
#include "internalcomm.h"
#include "servercommon/servercommon.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "servercommon/msgserialize.hpp"
#include "servercommon/crossdef.hpp"
#include "engineadapter.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/internalprotocal/bigcrosshgameprotocal.h"
#include "gamelog.h"
#include <cassert>
#include "servercommon/serverconfig/crossconfig.hpp"

#include "other/msgcachemanager/msgcachemanager.hpp"
#include "globalconfig/globalconfig.h"


void InternalComm::Init(INetworkModule *internal_network)
{
	m_internal_network = internal_network;

}

bool InternalComm::NetSendSerializeMsg(NetID netid, IMessageSerializer* msg)
{
	int length = 0;
	if (NULL == msg || !msg->Serialize(m_buffer, MAX_MSG_SIZE, &length))
	{
		return false;
	}

#ifdef _DEBUG
	if (length > 100000)
	{
		printf("InternalComm::NetSendSerializeMsg length=%d\n", length);
		PrintStackBackTrace(false);
	}
#endif

	return m_internal_network->Send(netid, m_buffer, length);
}

void InternalComm::SetSesstionKeyBase(const std::string &session_key_base)
{
	MD5 md5;
	MD5_buffer(&md5, session_key_base.c_str(), (unsigned long)session_key_base.size());
	MD52String(&m_session_key_base, md5);
}

void InternalComm::SetDBSession(const DBSession &accounter, DBSession *role_db, unsigned int role_db_num, const DBSession &global_session, const DBSession &systm_session, const DBSession &name_session)
{
	m_db_handler.Init(accounter, role_db, role_db_num, global_session, systm_session, name_session);
}

void InternalComm::RegisterOriginalDB(long long unique_server_index, const DBSession& original_session)
{
	m_db_handler.RegisterOriginalDB(unique_server_index, original_session);
}

void InternalComm::SetOriginGameworldDBSession(const UniqueServerID& server_id, const DBSession &accounter, DBSession *role_db, unsigned int role_db_num, const DBSession &global_session, const DBSession &systm_session, const DBSession &name_session)
{
	ServerDBMap::iterator it = m_server_db_map.find(server_id);
	if (it == m_server_db_map.end())
	{
		DBHandler* db_handler = new DBHandler();
		it = m_server_db_map.insert(ServerDBMap::value_type(server_id, db_handler)).first;

		db_handler->Init(accounter, role_db, role_db_num, global_session, systm_session, name_session);
	}
	else
	{
		//调用Close后句柄并没有置空，也不知道是否真的关闭了，但我们可以直接替换新的句柄
		DBHandler* db_handler = it->second;
		EngineAdapter::Instance().RMICloseSession(db_handler->GetAcounterDB());
		EngineAdapter::Instance().RMICloseSession(db_handler->GetRoleDB());

		db_handler->Init(accounter, role_db, role_db_num, global_session, systm_session, name_session);
	}
}

const DBHandler* InternalComm::GetHiddenDBHandler(const UniqueServerID& server_id)
{
	ServerDBMap::iterator it = m_server_db_map.find(server_id);
	if (it != m_server_db_map.end())
	{
		return it->second;
	}

	return NULL;
}

void InternalComm::RemoveOriginalDB(const UniqueServerID& server_id)
{
	ServerDBMap::iterator it = m_server_db_map.find(server_id);
	if (it != m_server_db_map.end())
	{
		delete it->second;
		it->second = NULL;
		m_server_db_map.erase(it);
	}
}

bool InternalComm::SendToAllGameUserThroughCross(int send_to_uuid, const char * data, int length)
{
	if (length + sizeof(crossgameprotocal::GameCrossSendAllUser) >= MAX_MSG_SIZE) return false;

	int plat_type = LocalConfig::Instance().GetPlatType();
	int server_id = IntToUid(send_to_uuid).db_index;

	crossgameprotocal::GameCrossSendAllUser * msg = (crossgameprotocal::GameCrossSendAllUser *)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_GAME_CROSS_SEND_ALL_USER;
	msg->origin_plat_type = plat_type;
	msg->origin_server_id = server_id;
	msg->send_to_uuid = send_to_uuid;

	memcpy(m_buffer + sizeof(crossgameprotocal::GameCrossSendAllUser), data, length);
	return this->SendToCross(m_buffer, length + sizeof(crossgameprotocal::GameCrossSendAllUser));
}

bool InternalComm::SendToAllGameAllUserThroughCross(const char * data, int length)
{
	return this->SendToAllGameUserThroughCross(0, data, length);
}

bool InternalComm::SendToCross(const char *data, int length)
{
	if (NULL == data || length <= 0 || length > crossgameprotocal::MAX_CROSSGAMEPROTOCAL_LENGTH) return false;

	bool ret = false;

	if (GlobalConfig::Instance()->IsOpenMsgCache())
	{
		MsgCacheBuf temp_buf;
		if (!temp_buf.Append(data, length))
		{
#ifdef _DEBUG
			gamelog::g_log_debug.printf(LL_INFO, "%s  temp_buf->Append(data, length)  length:%d", __FUNCTION__, length);
#endif
			return false;
		}
		ret = MsgCaCheManager::Instance().AddMsg(temp_buf);
	}
	else
	{
		ret = this->NetSend(m_cross_server_netid, (const char *)data, length);

		if (!ret)
		{
			crossgameprotocal::MessageHeader *header = (crossgameprotocal::MessageHeader*)data;
			int msg_type = header->msg_type;
			int sub_msg_type = 0;
			if (crossgameprotocal::MT_GAME_HIDDEN_MESSAGE == msg_type)
			{
				crossgameprotocal::MessageHeader * sub_header = (crossgameprotocal::MessageHeader*)(data + sizeof(crossgameprotocal::GameHiddenMessage));
				sub_msg_type = sub_header->msg_type;
			}
			else if (crossgameprotocal::MT_HIDDEN_GAME_MESSAGE == msg_type)
			{
				crossgameprotocal::MessageHeader * sub_header = (crossgameprotocal::MessageHeader*)(data + sizeof(crossgameprotocal::HiddenGameMessage));
				sub_msg_type = sub_header->msg_type;
			}

			gamelog::g_log_serious_error.printf(LL_INFO, "%s ret:%d  msg_type:%d  sub_msg_type:%d  length:%d ", __FUNCTION__, ret, msg_type, sub_msg_type, length);
		}
	}

	return ret;
}

bool InternalComm::SendToHiddenUserThroughCross(int uid, const void * data, int length)
{
	UniqueUserID uuid;
	uuid.plat_type = LocalConfig::Instance().GetPlatType();
	uuid.user_id = uid;

	return this->SendToHiddenThroughCross((const char *)data, length, UniqueUserIDToLongLong(uuid));
}

bool InternalComm::SendToHiddenThroughCross(const char* data, int length, long long send_to_uuid /* = 0 */)
{
	if (length + sizeof(crossgameprotocal::GameHiddenMessage) >= MAX_MSG_SIZE) return false;

	int plat_type = LocalConfig::Instance().GetPlatType();
	int server_id = LocalConfig::Instance().GetMergeServerId();

	crossgameprotocal::GameHiddenMessage* msg = (crossgameprotocal::GameHiddenMessage*)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_GAME_HIDDEN_MESSAGE;
	msg->type = crossgameprotocal::GAME_HIDDEN_MESSAGE_TYPE_DEFAULT;
	msg->origin_plat_type = plat_type;
	msg->origin_server_id = server_id;
	msg->send_to_uuid = send_to_uuid;

	memcpy(m_buffer + sizeof(crossgameprotocal::GameHiddenMessage), data, length);

	return this->SendToCross(m_buffer, length + sizeof(crossgameprotocal::GameHiddenMessage));
}

bool InternalComm::SendSerializeMsgToHiddenThroughCross(IMessageSerializer * data, long long send_to_uuid)
{
	if (sizeof(crossgameprotocal::GameHiddenMessage) >= MAX_MSG_SIZE) return false;

	int plat_type = LocalConfig::Instance().GetPlatType();
	int server_id = LocalConfig::Instance().GetMergeServerId();

	crossgameprotocal::GameHiddenMessage * msg = (crossgameprotocal::GameHiddenMessage *)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_GAME_HIDDEN_MESSAGE;
	msg->type = crossgameprotocal::GAME_HIDDEN_MESSAGE_TYPE_DEFAULT;
	msg->origin_plat_type = plat_type;
	msg->origin_server_id = server_id;
	msg->send_to_uuid = send_to_uuid;

	const int MAX_DATA_LENGTH = MAX_MSG_SIZE - sizeof(crossgameprotocal::GameHiddenMessage);
	int serialized_length = 0;
	if (!data->Serialize(m_buffer + sizeof(crossgameprotocal::GameHiddenMessage), MAX_DATA_LENGTH, &serialized_length))
	{
		return false;
	}

	return this->SendToCross(m_buffer, serialized_length + sizeof(crossgameprotocal::GameHiddenMessage));
}

bool InternalComm::SendToHiddenAllUserThroughCross(const char * data, int length)
{
	if (sizeof(crossgameprotocal::GameHiddenMessage) >= MAX_MSG_SIZE) return false;
	
	int plat_type = LocalConfig::Instance().GetPlatType();
	int server_id = LocalConfig::Instance().GetMergeServerId();

	crossgameprotocal::GameHiddenMessage * msg = (crossgameprotocal::GameHiddenMessage *)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_GAME_HIDDEN_MESSAGE;
	msg->type = crossgameprotocal::GAME_HIDDEN_MESSAGE_TYPE_SEND_ALL_GAME_USER;
	msg->origin_plat_type = plat_type;
	msg->origin_server_id = server_id;

	memcpy(m_buffer + sizeof(crossgameprotocal::GameHiddenMessage), data, length);

	return this->SendToCross(m_buffer, length + sizeof(crossgameprotocal::GameHiddenMessage));
}

bool InternalComm::SendToHiddenGuildMemberThroughCross(int guild_id, const char * data, int length)
{
	if (sizeof(crossgameprotocal::GameHiddenMessage) >= MAX_MSG_SIZE) return false;

	int plat_type = LocalConfig::Instance().GetPlatType();
	int server_id = LocalConfig::Instance().GetMergeServerId();

	crossgameprotocal::GameHiddenMessage * msg = (crossgameprotocal::GameHiddenMessage *)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_GAME_HIDDEN_MESSAGE;
	msg->type = crossgameprotocal::GAME_HIDDEN_MESSAGE_TYPE_SEND_GUILD_ALL_MEMBER;
	msg->origin_plat_type = plat_type;
	msg->origin_server_id = server_id;
	msg->guild_id = guild_id;

	memcpy(m_buffer + sizeof(crossgameprotocal::GameHiddenMessage), data, length);

	return this->SendToCross(m_buffer, length + sizeof(crossgameprotocal::GameHiddenMessage));
}

bool InternalComm::SendToHiddenUniqueServerUserThroughCross(const char* data, int length)
{
	if (sizeof(crossgameprotocal::GameHiddenMessage) >= MAX_MSG_SIZE) return false;

	int plat_type = LocalConfig::Instance().GetPlatType();
	int server_id = LocalConfig::Instance().GetMergeServerId();

	crossgameprotocal::GameHiddenMessage * msg = (crossgameprotocal::GameHiddenMessage *)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_GAME_HIDDEN_MESSAGE;
	msg->type = crossgameprotocal::GAME_HIDDEN_MESSAGE_TYPE_SEND_UNIQUE_SERVER_ALL_USER;
	msg->origin_plat_type = plat_type;
	msg->origin_server_id = server_id;

	memcpy(m_buffer + sizeof(crossgameprotocal::GameHiddenMessage), data, length);

	return this->SendToCross(m_buffer, length + sizeof(crossgameprotocal::GameHiddenMessage));
}

bool InternalComm::SendToAllGameThroughCross(const void * data, int length)
{
	return this ->SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), 0, (const char *)data, length);
}

bool InternalComm::SendToGameThroughCross(int origin_plat_type, int origin_server_id, const char* data, int length, int send_to_uid /* = 0 */)
{
	if (length + sizeof(crossgameprotocal::HiddenGameMessage) >= MAX_MSG_SIZE) return false;

	int plat_type = origin_plat_type;
	int server_id = origin_server_id;

	crossgameprotocal::HiddenGameMessage* msg = (crossgameprotocal::HiddenGameMessage*)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_HIDDEN_GAME_MESSAGE;
	msg->type = crossgameprotocal::HIDDEN_GAME_MESSAGE_TYPE_DEFAULT;
	msg->origin_plat_type = plat_type;
	msg->origin_server_id = server_id;
	msg->send_to_uid = send_to_uid;

	memcpy(m_buffer + sizeof(crossgameprotocal::HiddenGameMessage), data, length);

	return this->SendToCross(m_buffer, length + sizeof(crossgameprotocal::HiddenGameMessage));
}

bool InternalComm::SendToGameThroughCross2(const void * data, int length, int origin_server_id)
{
	if (length + sizeof(crossgameprotocal::HiddenGameMessage) >= MAX_MSG_SIZE) return false;

	crossgameprotocal::HiddenGameMessage * msg = (crossgameprotocal::HiddenGameMessage *)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_HIDDEN_GAME_MESSAGE;
	msg->type = crossgameprotocal::HIDDEN_GAME_MESSAGE_TYPE_DEFAULT;
	msg->origin_plat_type = LocalConfig::Instance().GetPlatType();
	msg->origin_server_id = origin_server_id;
	msg->send_to_uid = 0;

	memcpy(m_buffer + sizeof(crossgameprotocal::HiddenGameMessage), data, length);

	return this->SendToCross(m_buffer, length + sizeof(crossgameprotocal::HiddenGameMessage));
}

bool InternalComm::SendToGameThroughCrossByRoleId(const void * data, int length, int role_id)
{
	return this->SendToGameThroughCross2(data, length, IntToUid(role_id).db_index);
}

bool InternalComm::SendSerializeMsgToGameThroughCross(IMessageSerializer * data, int server_id)
{
	if (sizeof(crossgameprotocal::HiddenGameMessage) >= MAX_MSG_SIZE) return false;

	int plat_type = LocalConfig::Instance().GetPlatType();
	
	crossgameprotocal::HiddenGameMessage * msg = (crossgameprotocal::HiddenGameMessage *)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_HIDDEN_GAME_MESSAGE;
	msg->type = crossgameprotocal::HIDDEN_GAME_MESSAGE_TYPE_DEFAULT;
	msg->origin_plat_type = plat_type;
	msg->origin_server_id = server_id;
	msg->send_to_uid = 0;

	const int MAX_DATA_LENGTH = MAX_MSG_SIZE - sizeof(crossgameprotocal::HiddenGameMessage);
	int serialized_length = 0;
	if (!data->Serialize(m_buffer + sizeof(crossgameprotocal::HiddenGameMessage), MAX_DATA_LENGTH, &serialized_length))
	{
		return false;
	}

	return this->SendToCross(m_buffer, serialized_length + sizeof(crossgameprotocal::HiddenGameMessage));
}


bool InternalComm::SendSerializeMsgToAllGameThroughCross(IMessageSerializer * data)
{
	return this->SendSerializeMsgToGameThroughCross(data, 0);
}

bool InternalComm::SendToAllGameUserThroughCross(const char * data, int length)
{
	if (length + sizeof(crossgameprotocal::HiddenGameMessage) >= MAX_MSG_SIZE) return false;

	crossgameprotocal::HiddenGameMessage* msg = (crossgameprotocal::HiddenGameMessage*)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_HIDDEN_GAME_MESSAGE;
	msg->type = crossgameprotocal::HIDDEN_GAME_MESSAGE_TYPE_SEND_ALL_GAME_USER;
	msg->origin_plat_type = LocalConfig::Instance().GetPlatType();
	msg->origin_server_id = 0;
	memcpy(m_buffer + sizeof(crossgameprotocal::HiddenGameMessage), data, length);

	return this->SendToCross(m_buffer, length + sizeof(crossgameprotocal::HiddenGameMessage));
}

bool InternalComm::SendSerializeMsgToAllGameUserThroughCross(IMessageSerializer * data)
{
	if (sizeof(crossgameprotocal::HiddenGameMessage) >= MAX_MSG_SIZE) return false;

	crossgameprotocal::HiddenGameMessage * msg = (crossgameprotocal::HiddenGameMessage *)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_HIDDEN_GAME_MESSAGE;
	msg->type = crossgameprotocal::HIDDEN_GAME_MESSAGE_TYPE_SEND_ALL_GAME_USER;
	msg->origin_plat_type = LocalConfig::Instance().GetPlatType();
	msg->origin_server_id = 0;

	const int MAX_DATA_LENGTH = MAX_MSG_SIZE - sizeof(crossgameprotocal::HiddenGameMessage);
	int serialized_length = 0;
	if (!data->Serialize(m_buffer + sizeof(crossgameprotocal::HiddenGameMessage), MAX_DATA_LENGTH, &serialized_length))
	{
		return false;
	}

	return this->SendToCross(m_buffer, serialized_length + sizeof(crossgameprotocal::HiddenGameMessage));
}

bool InternalComm::SendToGameUserThroughCross(int send_to_uid, const void * data, int length)
{
	if (length + sizeof(crossgameprotocal::HiddenGameMessage) >= MAX_MSG_SIZE) return false;

	int plat_type = LocalConfig::Instance().GetPlatType();
	int server_id = IntToUid(send_to_uid).db_index;

	crossgameprotocal::HiddenGameMessage* msg = (crossgameprotocal::HiddenGameMessage*)m_buffer; 
	msg->header.msg_type = crossgameprotocal::MT_HIDDEN_GAME_MESSAGE;
	msg->type = crossgameprotocal::HIDDEN_GAME_MESSAGE_TYPE_DEFAULT;
	msg->origin_plat_type = plat_type;
	msg->origin_server_id = server_id;
	msg->send_to_uid = send_to_uid;

	memcpy(m_buffer + sizeof(crossgameprotocal::HiddenGameMessage), data, length);

	return this->SendToCross(m_buffer, length + sizeof(crossgameprotocal::HiddenGameMessage));
}

bool InternalComm::SendSerializeMsgToGameUserThroughCross(int send_to_uid, IMessageSerializer * data)
{
	if (sizeof(crossgameprotocal::HiddenGameMessage) >= MAX_MSG_SIZE) return false;

	int plat_type = LocalConfig::Instance().GetPlatType();
	int server_id = IntToUid(send_to_uid).db_index;

	crossgameprotocal::HiddenGameMessage * msg = (crossgameprotocal::HiddenGameMessage *)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_HIDDEN_GAME_MESSAGE;
	msg->type = crossgameprotocal::HIDDEN_GAME_MESSAGE_TYPE_DEFAULT;
	msg->origin_plat_type = plat_type;
	msg->origin_server_id = server_id;
	msg->send_to_uid = send_to_uid;

	const int MAX_DATA_LENGTH = MAX_MSG_SIZE - sizeof(crossgameprotocal::HiddenGameMessage);
	int serialized_length = 0;
	if (!data->Serialize(m_buffer + sizeof(crossgameprotocal::HiddenGameMessage), MAX_DATA_LENGTH, &serialized_length))
	{
		return false;
	}

	return this->SendToCross(m_buffer, serialized_length + sizeof(crossgameprotocal::HiddenGameMessage));
}

bool InternalComm::SendToGameGuildMemberThroughCross(int guild_id, const void * data, int length)
{
	if (length + sizeof(crossgameprotocal::HiddenGameMessage) >= MAX_MSG_SIZE) return false;

	crossgameprotocal::HiddenGameMessage* msg = (crossgameprotocal::HiddenGameMessage*)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_HIDDEN_GAME_MESSAGE;
	msg->type = crossgameprotocal::HIDDEN_GAME_MESSAGE_TYPE_SEND_GUILD_ALL_MEMBER;
	msg->origin_plat_type = LocalConfig::Instance().GetPlatType();
	msg->origin_server_id = IntToUid(guild_id).db_index;
	msg->guild_id = guild_id;
	memcpy(m_buffer + sizeof(crossgameprotocal::HiddenGameMessage), data, length);

	return this->SendToCross(m_buffer, length + sizeof(crossgameprotocal::HiddenGameMessage));
}

bool InternalComm::SendToGameAllUserThroughCross(int origin_plat_type, int origin_server_id, const char * data, int length)
{
	if (length + sizeof(crossgameprotocal::HiddenGameMessage) >= MAX_MSG_SIZE) return false;

	crossgameprotocal::HiddenGameMessage* msg = (crossgameprotocal::HiddenGameMessage*)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_HIDDEN_GAME_MESSAGE;
	msg->type = crossgameprotocal::HIDDEN_GAME_MESSAGE_TYPE_SEND_ALL_GAME_USER;
	msg->origin_plat_type = LocalConfig::Instance().GetPlatType();
	msg->origin_server_id = origin_server_id;
	memcpy(m_buffer + sizeof(crossgameprotocal::HiddenGameMessage), data, length);

	return this->SendToCross(m_buffer, length + sizeof(crossgameprotocal::HiddenGameMessage));
}

InternalComm::InternalComm() : m_internal_network(NULL), m_login_server_netid(-1), m_cross_server_netid(-1), m_bigc_server_netid(-1), m_reconnet_login_handle(-1), m_reconnet_cross_handle(-1), m_reconnet_bigc_handle(-1), m_buffer(NULL)
{
	m_buffer = new char[MAX_MSG_SIZE];	// 4M, 不可能有比4M还大的一个网络包，一定够用了
	memset(m_buffer, 0, MAX_MSG_SIZE);
}

InternalComm::~InternalComm()
{

}

bool InternalComm::UserMsgTransmitToCross(const UserID &user_id, int msg_type, const char *data, int length)
{
	if (length <= 0 || length + sizeof(crossgameprotocal::GameCrossUserMsg) > MAX_MSG_SIZE) return false;

	static const int PLAT_TYPE = LocalConfig::Instance().GetPlatType();
	static char tmp_buffer[MAX_MSG_SIZE];

	int send_len = 0;

	crossgameprotocal::GameCrossUserMsg *global_cross_user_msg = (crossgameprotocal::GameCrossUserMsg *)(tmp_buffer + send_len);
	*global_cross_user_msg = crossgameprotocal::GameCrossUserMsg();
	global_cross_user_msg->uuid = UniqueUserIDToLongLong(UniqueUserID(PLAT_TYPE, user_id));
	global_cross_user_msg->merge_server_id = LocalConfig::Instance().GetMergeServerId();

	send_len += sizeof(crossgameprotocal::GameCrossUserMsg);

	memcpy(tmp_buffer + send_len, data, length);
	send_len += length;

	return this->SendToCross((const char *)tmp_buffer, send_len);
}

bool InternalComm::SendSerializeMsgToHiddenThroughCross2(IMessageSerializer2 * data, long long send_to_uuid)
{
	if (sizeof(crossgameprotocal::GameHiddenMessage) >= MAX_MSG_SIZE) return false;

	int plat_type = LocalConfig::Instance().GetPlatType();
	int server_id = LocalConfig::Instance().GetMergeServerId();

	crossgameprotocal::GameHiddenMessage * msg = (crossgameprotocal::GameHiddenMessage *)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_GAME_HIDDEN_MESSAGE;
	msg->type = crossgameprotocal::GAME_HIDDEN_MESSAGE_TYPE_DEFAULT;
	msg->origin_plat_type = plat_type;
	msg->origin_server_id = server_id;
	msg->send_to_uuid = send_to_uuid;

	const int MAX_DATA_LENGTH = MAX_MSG_SIZE - sizeof(crossgameprotocal::GameHiddenMessage);

	TLVSerializer2  outstream;
	outstream.Reset(m_buffer + sizeof(crossgameprotocal::GameHiddenMessage), MAX_DATA_LENGTH);
	if (!data->Serialize(&outstream))
	{
		return false;
	}

	return this->SendToCross(m_buffer, (int)outstream.Size() + sizeof(crossgameprotocal::GameHiddenMessage));
}

bool InternalComm::SendSerializeMsgToHiddenUserThroughCross(int send_to_uid, IMessageSerializer * data)
{
	if (sizeof(crossgameprotocal::GameHiddenMessage) >= MAX_MSG_SIZE) return false;

	int plat_type = LocalConfig::Instance().GetPlatType();
	int server_id = LocalConfig::Instance().GetMergeServerId();

	crossgameprotocal::GameHiddenMessage * msg = (crossgameprotocal::GameHiddenMessage *)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_GAME_HIDDEN_MESSAGE;
	msg->type = crossgameprotocal::GAME_HIDDEN_MESSAGE_TYPE_DEFAULT;
	msg->origin_plat_type = plat_type;
	msg->origin_server_id = server_id;
	msg->send_to_uuid = UniqueUserIDToLongLong(UniqueUserID(plat_type, IntToUid(send_to_uid)));

	const int MAX_DATA_LENGTH = MAX_MSG_SIZE - sizeof(crossgameprotocal::GameHiddenMessage);
	int serialized_length = 0;
	if (!data->Serialize(m_buffer + sizeof(crossgameprotocal::GameHiddenMessage), MAX_DATA_LENGTH, &serialized_length))
	{
		return false;
	}

	return this->SendToCross(m_buffer, serialized_length + sizeof(crossgameprotocal::GameHiddenMessage));
}

bool InternalComm::SendToBigCrossServer(const char *data, int length)
{
	if (NULL == data || length <= 0 || length > bigchgameprotocol::MAX_BIG_CROSS_HGAME_PROTOCAL_LENGTH) return false;

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return this->SendToBigCrossServerThroughHidden(data, length);
	}

	bool ret = this->NetSend(m_bigc_server_netid, (const char *)data, length);

	if (!ret)
	{
		bigchgameprotocol::MessageHeader *header = (bigchgameprotocol::MessageHeader*)data;
		int msg_type = header->msg_type;
		int sub_msg_type = 0;

		gamelog::g_log_serious_error.printf(LL_INFO, "%s ret:%d  msg_type:%d  sub_msg_type:%d  length:%d ", __FUNCTION__, ret, msg_type, sub_msg_type, length);
	}

	return ret;
}

bool InternalComm::SendSerializeMsgToBigCrossServer(IMessageSerializer * data)
{
	return InternalComm::Instance().NetSendSerializeMsg(m_bigc_server_netid, data);
}

bool InternalComm::SendToBigCrossServerThroughHidden(const char* data, int length)
{
	assert(!CrossConfig::Instance().IsHiddenServer());

	if (length
		+ sizeof(crossgameprotocal::GameHiddenMessage)
		+ sizeof(crossgameprotocal::GameHiddenTransferMsgToBigcross)
		>= MAX_MSG_SIZE)
	{
		return false;
	}

	// ps: 之所以不直接封装GameHiddenTransferMsgToBigcross
	// 然后调用SendToHiddenThroughCross，
	// 是因为使用了m_buffer同时会被两个接口使用，会造成数据覆盖
	// 所以直接在这个接口里封装两层转发协议

	int plat_type = LocalConfig::Instance().GetPlatType();
	int server_id = LocalConfig::Instance().GetMergeServerId();

	// 封装第一层
	{
		crossgameprotocal::GameHiddenMessage* msg = (crossgameprotocal::GameHiddenMessage*)m_buffer;
		msg->header.msg_type = crossgameprotocal::MT_GAME_HIDDEN_MESSAGE;
		msg->type = crossgameprotocal::GAME_HIDDEN_MESSAGE_TYPE_DEFAULT;
		msg->origin_plat_type = plat_type;
		msg->origin_server_id = server_id;
		msg->send_to_uuid = 0LL;
	}
	
	// 封装第二层
	{
		crossgameprotocal::GameHiddenTransferMsgToBigcross* msg =
			(crossgameprotocal::GameHiddenTransferMsgToBigcross*)
			(m_buffer + sizeof(crossgameprotocal::GameHiddenMessage));
		msg->header.msg_type = crossgameprotocal::MT_GAME_HIDDEN_TRANSFER_MSG_TO_BIGCROSS;
		msg->origin_plat_type = plat_type;
		msg->origin_server_id = server_id;
	}
	
	// 拼接内容
	memcpy(m_buffer 
		+ sizeof(crossgameprotocal::GameHiddenMessage) 
		+ sizeof(crossgameprotocal::GameHiddenTransferMsgToBigcross)
		, data, length);

	int total_length = length 
		+ sizeof(crossgameprotocal::GameHiddenMessage) 
		+ sizeof(crossgameprotocal::GameHiddenTransferMsgToBigcross);

	return this->SendToCross(m_buffer, total_length);
}


bool InternalComm::SendToHiddenToBigCross(const char *data, int length)
{
	if (length + sizeof(crossgameprotocal::GameHiddenMessage) >= MAX_MSG_SIZE) return false;

	int plat_type = LocalConfig::Instance().GetPlatType();
	int server_id = LocalConfig::Instance().GetMergeServerId();

	crossgameprotocal::GameHiddenMessage* msg = (crossgameprotocal::GameHiddenMessage*)m_buffer;
	msg->header.msg_type = crossgameprotocal::MT_GAME_HIDDEN_MESSAGE;
	msg->type = crossgameprotocal::GAME_HIDDEN_MESSAGE_TYPE_SEND_TO_BIG_CROSS;
	msg->origin_plat_type = plat_type;
	msg->origin_server_id = server_id;

	memcpy(m_buffer + sizeof(crossgameprotocal::GameHiddenMessage), data, length);

	return this->SendToCross(m_buffer, length + sizeof(crossgameprotocal::GameHiddenMessage));
}


bool InternalComm::TransferBigCrossMsgToAllOriginGame(const char* data, int length)
{
	return this->TransferBigCrossMsgToOriginGame(0, 0, data, length);
}

bool InternalComm::TransferBigCrossMsgToOriginGame(int plat_type, int server_id, const char* data, int length)
{
	assert(CrossConfig::Instance().IsHiddenServer());

	if (length
		+ sizeof(crossgameprotocal::HiddenGameMessage)
		+ sizeof(crossgameprotocal::HiddenGameTransferBigcrossMsgToOrigin)
		>= MAX_MSG_SIZE)
	{
		return false;
	}

	// ps: 该接口使用了m_buffer封装数据，后续调用的发送接口不可对m_buffer进行封装数据，否则出现数据覆盖

	// 封装第一层
	{
		crossgameprotocal::HiddenGameMessage* msg = (crossgameprotocal::HiddenGameMessage*)m_buffer;
		msg->header.msg_type = crossgameprotocal::MT_HIDDEN_GAME_MESSAGE;
		msg->type = crossgameprotocal::HIDDEN_GAME_MESSAGE_TYPE_DEFAULT;
		msg->origin_plat_type = plat_type;
		msg->origin_server_id = server_id;
		msg->send_to_uid = 0;
	}

	// 封装第二层
	{
		crossgameprotocal::HiddenGameTransferBigcrossMsgToOrigin* msg =
			(crossgameprotocal::HiddenGameTransferBigcrossMsgToOrigin*)
			(m_buffer + sizeof(crossgameprotocal::HiddenGameMessage));
		msg->header.msg_type = crossgameprotocal::MT_HIDDEN_GAME_TRANSFER_BIGCROSS_MSG_TO_ORIGIN;
	}

	// 拼接内容
	memcpy(m_buffer
		+ sizeof(crossgameprotocal::HiddenGameMessage)
		+ sizeof(crossgameprotocal::HiddenGameTransferBigcrossMsgToOrigin)
		, data, length);

	int total_length = length
		+ sizeof(crossgameprotocal::HiddenGameMessage)
		+ sizeof(crossgameprotocal::HiddenGameTransferBigcrossMsgToOrigin);

	return this->SendToCross(m_buffer, total_length);
}

DBSession* InternalComm::GetDBSession(NetID netid)
{
	DBSession* db_session = m_db_handler.GetDBSession(netid);
	if (NULL != db_session) return db_session;

	ServerDBMap::iterator it = m_server_db_map.begin();
	for (; it != m_server_db_map.end(); ++it)
	{
		db_session = it->second->GetDBSession(netid);
		if (NULL != db_session) return db_session;
	}

	return NULL;
}

DBSession* InternalComm::GetDBSessionByReconnectHandle(int reconnect_handle)
{
	DBSession* db_session = m_db_handler.GetDBSessionByReconnectHandle(reconnect_handle);
	if (NULL != db_session) return db_session;

	ServerDBMap::iterator it = m_server_db_map.begin();
	for (; it != m_server_db_map.end(); ++it)
	{
		db_session = it->second->GetDBSessionByReconnectHandle(reconnect_handle);
		if (NULL != db_session) return db_session;
	}

	return NULL;
}

void InternalComm::CloseAllDBSession(IRMIModule* rmi)
{
	m_db_handler.CloseAllSession(rmi);

	ServerDBMap::iterator it = m_server_db_map.begin();
	for (; it != m_server_db_map.end(); ++it)
	{
		it->second->CloseAllSession(rmi);
	}
}