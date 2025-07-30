#ifndef  __MSG_CACHE_MANAGER_HPP__
#define  __MSG_CACHE_MANAGER_HPP__

// version : v_0.1

#include <string>
#include <list>

#include "servercommon/multithread/mutex_guard.h"
#include "servercommon/multithread/condition_variable.h"
#include "servercommon/multithread/mutex_lock.h"
#include "common/platform/thread/thread.h"

static const int OPEN_MSG_CACHE_MANAGER = 1;

class MsgCacheBuf
{
public:
	void* operator new(size_t c);
	void operator delete(void* m);

	MsgCacheBuf();
	~MsgCacheBuf();
	MsgCacheBuf(const MsgCacheBuf& _o);
	MsgCacheBuf& operator= (const MsgCacheBuf& _o);

	/**
	* @brief  : 将数据添加到缓存区内
	* @param  _in_data : 需要写入的数据的指针
	* @param  _len : 需要写入的数据长度
	* @return  : 操作是否成功
	*/
	bool Append(const char * _in_data, int _len);

	const char * GetData() const { return m_data; }
	const int GetLength()const  { return m_total_lenth; }

private:
	int m_total_lenth;
	char * m_data;
};

class MsgCaCheManager
{
public:
	static MsgCaCheManager & Instance();

	bool AddMsg(MsgCacheBuf & _in_msg_buf);

	void StartThread();
	static DWORD Run(void* mgr_ptr);
	void ThreadLoop();
	void StopThread();

private:
	MsgCaCheManager();
	~MsgCaCheManager();

	MutexLock m_mutex;
	ConditionVariable m_cond;
	Thread m_async_thread;

	std::list<MsgCacheBuf> m_queue;
	volatile bool m_is_stop;
	bool m_has_started;
};


#endif	 // __MSG_CACHE_MANAGER_HPP__
