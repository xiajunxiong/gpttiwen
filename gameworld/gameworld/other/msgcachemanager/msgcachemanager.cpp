#include "msgcachemanager.hpp"

#include <assert.h>
#include "servercommon/multithread/mutex_guard.h"
#include "internalcomm.h"
#include "global/cross/crossmanager.h"
#include "gamelog.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/serverclmempool.h"


#ifdef __linux__
#include <unistd.h>
#endif

MsgCacheBuf::MsgCacheBuf()
	:m_total_lenth(0), m_data(NULL)
{

}

MsgCacheBuf::MsgCacheBuf(const MsgCacheBuf& _o)
	: m_total_lenth(0), m_data(NULL)
{
	this->Append(_o.GetData(), _o.GetLength());
}

MsgCacheBuf & MsgCacheBuf::operator=(const MsgCacheBuf& _o)
{
	this->Append(_o.GetData(), _o.GetLength());

	return *this;
}

MsgCacheBuf::~MsgCacheBuf()
{
	ServerMsgMem *tmp = (ServerMsgMem*)m_data;
	delete[]tmp;
	m_data = NULL;
}

bool MsgCacheBuf::Append(const char * _in_data, int _len)
{
	if (NULL == _in_data)
		return false;

	if (NULL != m_data)
	{
		delete[] m_data;
		m_data = NULL;
		m_total_lenth = 0;
	}

	m_data = (char *)(new ServerMsgMem[sizeof(char) * _len]);
	assert(NULL != m_data);

	memcpy(m_data , _in_data, _len);
	m_total_lenth = _len;

	return true;
}

static MutexLock g_st_mm_mutex;
static MsgCaCheManager * g_st_mm = NULL;

MsgCaCheManager & MsgCaCheManager::Instance()
{
	if (NULL == g_st_mm)
	{
		MutexGuard guard(&g_st_mm_mutex);
		if (NULL == g_st_mm)
		{
			g_st_mm = new MsgCaCheManager();
		}
	}

	return *g_st_mm;
}

bool MsgCaCheManager::AddMsg(MsgCacheBuf & _in_msg_buf)
{
	{
		MutexGuard guard(&m_mutex);
		m_queue.push_back(_in_msg_buf);
	}

	m_cond.Notify();

	return true;
}

void MsgCaCheManager::StartThread()
{
	m_has_started = true;
	m_async_thread.Run(MsgCaCheManager::Run, this);
}

DWORD MsgCaCheManager::Run(void* mgr_ptr)
{
	MsgCaCheManager* async_mgr = (MsgCaCheManager*)mgr_ptr;
	async_mgr->ThreadLoop();

	return 0;
}

void MsgCaCheManager::ThreadLoop()
{
	while (!m_is_stop )
	{
		if (!CrossManager::Instance().IsCrossServerConnected() ||
			CrossManager::Instance().IsCrossServerConnecting())
		{
#ifdef __linux__
			usleep(100 * 1000);	 // linux中  参数为微妙 1毫秒 = 1000微秒
#else
			Sleep(100);	// 如果还在连接中 或者没连接上 ,则当前线程休息100毫秒
#endif
			continue;
		}

		bool ret = false;
		MsgCacheBuf temp_buf ;

		{
			MutexGuard guard(&m_mutex);
			while (m_queue.empty())
			{
				if (m_is_stop)
					return;			// 关服就退出循环,退出线程

				m_cond.Wait(m_mutex);
			}

			temp_buf = m_queue.front();
		}
	
		int total_count = 0;
		int count = 0;
		do
		{
			if (0 != count && 0 == count % 8)
			{
				// 连续失败8次了 , 那再休息一小会不要一直占着CPU了
#ifdef __linux__
				usleep(5 * 1000);	 // linux中  参数为微妙 1毫秒 = 1000微秒
#else
				Sleep(5);	// 则当前线程休息5毫秒
#endif
			}

			ret = InternalComm::Instance().NetSend(InternalComm::Instance().m_cross_server_netid, temp_buf.GetData(), temp_buf.GetLength());
		
#ifdef _DEBUG
			crossgameprotocal::MessageHeader *header = (crossgameprotocal::MessageHeader*)temp_buf.GetData();
			int msg_type = header->msg_type;
			int sub_msg_type = 0;
			if (crossgameprotocal::MT_GAME_HIDDEN_MESSAGE == msg_type)
			{
				crossgameprotocal::MessageHeader * sub_header = (crossgameprotocal::MessageHeader*)(temp_buf.GetData() + sizeof(crossgameprotocal::GameHiddenMessage));
				sub_msg_type = sub_header->msg_type;
			}
			else if (crossgameprotocal::MT_HIDDEN_GAME_MESSAGE == msg_type)
			{
				crossgameprotocal::MessageHeader * sub_header = (crossgameprotocal::MessageHeader*)(temp_buf.GetData() + sizeof(crossgameprotocal::HiddenGameMessage));
				sub_msg_type = sub_header->msg_type;
			}
			gamelog::g_log_debug.printf(LL_INFO, "%s  msg_type:%d  sub_msg_type:%d  length:%d ", __FUNCTION__,msg_type, sub_msg_type, temp_buf.GetLength());
#endif
		
			++count;
		} while (!ret && total_count++ < 32); // 最大尝试 32次

		if (ret)
		{
			// 发送成功了
			MutexGuard guard(&m_mutex);
			if (!m_queue.empty())
			{
				m_queue.pop_front();
			}
		}
		else if (32 <= total_count)
		{
			crossgameprotocal::MessageHeader *header = (crossgameprotocal::MessageHeader*)temp_buf.GetData();
			int msg_type = header->msg_type;
			int sub_msg_type = 0;
			if (crossgameprotocal::MT_GAME_HIDDEN_MESSAGE == msg_type)
			{
				crossgameprotocal::MessageHeader * sub_header = (crossgameprotocal::MessageHeader*)(temp_buf.GetData() + sizeof(crossgameprotocal::GameHiddenMessage));
				sub_msg_type = sub_header->msg_type;
			}
			else if (crossgameprotocal::MT_HIDDEN_GAME_MESSAGE == msg_type)
			{
				crossgameprotocal::MessageHeader * sub_header = (crossgameprotocal::MessageHeader*)(temp_buf.GetData() + sizeof(crossgameprotocal::HiddenGameMessage));
				sub_msg_type = sub_header->msg_type;
			}
			gamelog::g_log_serious_error.printf(LL_INFO, "%s  times >= 32  msg_type:%d  sub_msg_type:%d  length:%d ", __FUNCTION__,  msg_type, sub_msg_type, temp_buf.GetLength());

			// 连续失败达到了32次了 , 那再休息一小会不要一直占着CPU了
#ifdef __linux__
			usleep(50 * 1000);	 // linux中  参数为微妙 1毫秒 = 1000微秒
#else
			Sleep(50);	// 则当前线程休息50毫秒
#endif
			continue;
		}
	}

}

void MsgCaCheManager::StopThread()
{
	if (m_has_started && !m_is_stop)
	{
		m_is_stop = true;
		m_cond.NotifyAll();
		m_async_thread.Join();
	}
}

MsgCaCheManager::MsgCaCheManager()
	:m_is_stop(false), m_has_started(false)
{

}

MsgCaCheManager::~MsgCaCheManager()
{
	this->StopThread();
}
