#include "engineadapter.h"
#include "inetworkmodule.h"
#include "servercommon/internalprotocal/wgprotocal.h"
#include "servercommon/servercommon.h"
#include "gamelog.h"
#include "protocol/msghuanjiezhanchang.h"

EngineAdapter::EngineAdapter():m_server_index(-1), m_game_thread(-1), m_serial_num_timestamp(0u), m_serial_num_index(0)
{
	m_buffer = new char[MAX_MSG_SIZE];	// 4M, 不可能有比4M还大的一个网络包，一定够用了
	memset(m_buffer, 0, MAX_MSG_SIZE);

	/*
	static const int SERIALIZE_BUFFER_SIZE = MAX_MSG_SIZE;
	m_serialize_buffer = new char[SERIALIZE_BUFFER_SIZE];
	memset(m_serialize_buffer, 0, sizeof(SERIALIZE_BUFFER_SIZE));
	m_serialize_buffer_size = SERIALIZE_BUFFER_SIZE;
	*/
}

EngineAdapter::~EngineAdapter()
{
	delete []m_buffer;
}

void EngineAdapter::Init(ILogModule *log, 
						 IConfigModule *config, 
						 IGameModule *game, 
						 INetworkModule *network, 
						 IClockModule *clock, 
						 IRMIModule *rmi_module,
						 ITimerModule *timer,
						 IFileWriteModule *filewrite)
{
	m_log = log;
	m_config = config;
	m_game = game;
	m_network = network;
	m_clock = clock;
	m_rmi = rmi_module;
	m_time = timer;
	m_filewrite = filewrite;
}

unsigned int EngineAdapter::WeekID()
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	unsigned int next_week_zero_time = now + EngineAdapter::Instance().NextWeekInterval(1, 0, 0, 0);
	static const int WEEK_SECONDS = 24 * 60 * 60 * 7;
	return next_week_zero_time - WEEK_SECONDS;
}

bool EngineAdapter::NetSend(const GSNetID &gsnetid, const void *data, unsigned int length)
{
	if (NULL == data || length > MAX_MSG_SIZE - sizeof(wgprotocal::GWNetSendMsg)) return false;

	wgprotocal::GWNetSendMsg *msg = (wgprotocal::GWNetSendMsg*)m_buffer;
	*msg = wgprotocal::GWNetSendMsg();

	msg->header.netid = gsnetid.netid;

	memcpy(m_buffer + sizeof(wgprotocal::GWNetSendMsg), data, length);

#ifdef _DEBUG
	if (length >= 8192)
	{
		Protocol::MessageHeader* h = (Protocol::MessageHeader*)data;
		gamelog::g_log_world.printf(LL_MAINTANCE, "EngineAdapter::NetSend  length > 8192,  length:%d, msg_type:%d", length, h->msg_type);
	}

	//std::map<GSNetID, int>::iterator it = m_data_size_recorder.find(gsnetid);
	//if (it != m_data_size_recorder.end())
	//{
	//	it->second += length;
	//}
#endif

#ifdef _DEBUG
 	//Protocol::MessageHeader* h = (Protocol::MessageHeader*)data;
 	//printf("EngineAdapter::NetSend type:%d length:%d\n", h->msg_type, length);
 	//gamelog::g_log_messagehandler.printf(LL_DEBUG, "EngineAdapter::NetSend type:%d length:%d", h->msg_type, length);
#endif

#ifdef _DEBUG
	if (length > 100000)
	{
		printf("NetSend length=%u\n", length);
		PrintStackBackTrace(false);
	}
#endif

	return m_network->Send(gsnetid.gateway_netid, m_buffer, length + sizeof(wgprotocal::GWNetSendMsg));
}

bool EngineAdapter::NetSendSerializeMsg(const GSNetID &gsnetid, IMessageSerializer* msg)
{
	if (NULL == msg) return false;

	int max_length = MAX_MSG_SIZE - sizeof(wgprotocal::GWNetSendMsg);
	int serialized_length = 0;

	wgprotocal::GWNetSendMsg *buffer = (wgprotocal::GWNetSendMsg*)m_buffer;
	*buffer = wgprotocal::GWNetSendMsg();

	buffer->header.netid = gsnetid.netid;

	if (!msg->Serialize(m_buffer + sizeof(wgprotocal::GWNetSendMsg), max_length, &serialized_length))
	{
		return false;
	}

#ifdef _DEBUG
	if (serialized_length + sizeof(wgprotocal::GWNetSendMsg) >= 8192)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "NetSendSerializeMsg  length > 8192,  length:%d, msg_type:%d", static_cast<int>(serialized_length + sizeof(wgprotocal::GWNetSendMsg)), msg->GetMessageType());
	}
#endif

#ifdef _DEBUG
	if (serialized_length + sizeof(wgprotocal::GWNetSendMsg) > 100000)
	{
		printf("EngineAdapter::NetSendSerializeMsg length=%d\n", static_cast<int>(serialized_length + sizeof(wgprotocal::GWNetSendMsg)));
		PrintStackBackTrace(false);
	}
#endif

	return m_network->Send(gsnetid.gateway_netid, m_buffer, serialized_length + sizeof(wgprotocal::GWNetSendMsg));
}

bool EngineAdapter::NetSendSerializeMsg2(const GSNetID & netid, const IMessageSerializer2 * msg)
{
	static char buff[MAX_MSG_SIZE] = { 0 };

	TLVSerializer2 s;
	s.Reset(buff, ARRAY_LENGTH(buff));


	bool is_succ = msg->Serialize(&s);

	if (is_succ && s.Size() > 0)
	{
		return this->NetSend(netid, s.Ptr(), s.Size());
	}

	return false;
}

bool EngineAdapter::NetMulSend(GSNetID *netidlist, int num, const char *data, unsigned int length)
{
	if (NULL == data) return false;

	const int MAX_GATEWAY_ID = 256;
	bool gsmark[MAX_GATEWAY_ID];
	unsigned short gslist[MAX_GATEWAY_ID];
	int gsnum = 0;
	memset(gsmark, 0, sizeof(gsmark));

	GSNetID *netid_p = netidlist;
	GSNetID *netid_end = netidlist + num;
	while (netid_p != netid_end)
	{
		unsigned short gwn = netid_p->gateway_netid;
		if (gwn < MAX_GATEWAY_ID && !gsmark[gwn])
		{
			gsmark[gwn] = true;
			gslist[gsnum++] = gwn;
		}
		++netid_p;
	}

	bool ret = true;
	for (int cur = 0; cur < gsnum; ++cur)
	{
		wgprotocal::GWNetMulSendMsg *msg = (wgprotocal::GWNetMulSendMsg*)m_buffer;
		*msg = wgprotocal::GWNetMulSendMsg();
		msg->count = 0;

		unsigned short *beg_netids = (unsigned short*)(m_buffer + sizeof(wgprotocal::GWNetMulSendMsg));

		int gwid = gslist[cur];

		unsigned short *end_netids = beg_netids;
		GSNetID *netid_p = netidlist;
		while (netid_p != netid_end)
		{
			if (netid_p->gateway_netid == gwid)
			{
				*end_netids++ = netid_p->netid;
			}
			++netid_p;
		}
		msg->count = int(end_netids - beg_netids);

		char *data_pos = (char *)end_netids;
		int total_len = int(data_pos - m_buffer) + length;
		if (total_len > MAX_MSG_SIZE)
		{
			ret = false;
			continue;
		}
		memcpy(data_pos, data, length);

#ifdef _DEBUG
		if (length >= 8192)
		{
			Protocol::MessageHeader* h = (Protocol::MessageHeader*)data;
			gamelog::g_log_world.printf(LL_MAINTANCE, "EngineAdapter::NetMulSend  length > 8192,  length:%d, msg_type:%d", length, h->msg_type);
		}
#endif

#ifdef _DEBUG
		if (length > 100000)
		{
			printf("EngineAdapter::NetMulSend length=%u\n", length);
			PrintStackBackTrace(false);
		}
#endif

		ret &= m_network->Send(gwid, m_buffer, total_len);
	}

	return ret;
}

bool EngineAdapter::NetSendAll(NetID gateway_netid, const char *data, unsigned int length)
{
	if (NULL == data || length > MAX_MSG_SIZE - sizeof(wgprotocal::GWNetSendAll)) return false;

	wgprotocal::GWNetSendAll *msg = (wgprotocal::GWNetSendAll*)m_buffer;
	*msg = wgprotocal::GWNetSendAll();

	memcpy(m_buffer + sizeof(wgprotocal::GWNetSendAll), data, length);

#ifdef _DEBUG
	if (length >= 8192)
	{
		Protocol::MessageHeader* h = (Protocol::MessageHeader*)data;
		gamelog::g_log_world.printf(LL_MAINTANCE, "EngineAdapter::NetSendAll  length > 8192,  length:%d, msg_type:%d", length, h->msg_type);
	}
#endif

#ifdef _DEBUG
	if (length > 100000)
	{
		printf("NetSendAll length=%u\n", length);
		PrintStackBackTrace(false);
	}
#endif

	return m_network->Send(gateway_netid, m_buffer, length + sizeof(wgprotocal::GWNetSendAll));
}

void EngineAdapter::NetDisconnect(const GSNetID &gsnetid, UserID user_id, const GameName role_name, int scene_id, const char *reason)
{
	wgprotocal::GWNetDisconnect ndis;
	ndis.header.netid = gsnetid.netid;
	ndis.uid = UidToInt(user_id);
	ndis.scene_id = scene_id;
	STRNCPY(ndis.role_name, role_name, sizeof(ndis.role_name));
	STRNCPY(ndis.server_name, "GameWorld", sizeof(ndis.server_name));
	STRNCPY(ndis.reason, reason, sizeof(ndis.reason));
	m_network->Send(gsnetid.gateway_netid, (const char *)&ndis, sizeof(wgprotocal::GWNetDisconnect));
}

void EngineAdapter::NetHasCheck(const GSNetID &gsnetid, const char* role_name, int role_id)
{
	wgprotocal::GWHasCheck hc;
	hc.header.netid = gsnetid.netid;
	hc.role_id = role_id;
	F_STRNCPY(hc.role_name, role_name, sizeof(hc.role_name));

	m_network->Send(gsnetid.gateway_netid, (const char *)&hc, sizeof(wgprotocal::GWHasCheck));
}

const char* EngineAdapter::GetSerializeData(IMessageSerializer* msg, int* out_length)
{
	if (NULL == msg || NULL == out_length) return NULL;

	*out_length = 0;
	if (!msg->Serialize(m_buffer, MAX_MSG_SIZE, out_length)) return NULL;

	return m_buffer;
}

void EngineAdapter::GenerateSerialNum(int server_id, int role_id, SerialNum out_serial_num)
{
	unsigned int now = static_cast<unsigned int>(this->Time());
	if (now > m_serial_num_timestamp)
	{
		m_serial_num_timestamp = now;
		m_serial_num_index = 0;
	}

	SNPRINTF(out_serial_num, sizeof(SerialNum), "%d-%u-%d-%d", role_id, m_serial_num_timestamp, m_serial_num_index, server_id);
	
	++m_serial_num_index;
}

void EngineAdapter::PrintErrorConfigLog(const char* path)
{
	printf("PrintErrorConfigLog path[%s]\n", path);
}