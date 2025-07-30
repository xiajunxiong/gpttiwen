#ifndef __SERVERLOGICMODULE_H__
#define __SERVERLOGICMODULE_H__

#include "imodule.h"
#include "utility/logagent.h"

static const char SERVER_MODULE[] = "ServerModule";

class ServerNetworkCallback;
class ServerInternalNetCallback;
class IClockModule;
class INetworkModule;
class IGameModule;
class IRMIModule;
class ITimerModule;
class IFileWriteModule;
class World;

class ServerLogic : public IModule
{
	friend class ServerNetworkCallback;
	friend class ServerInternalNetCallback;

public:
	static ServerLogic * GetInstServerLogic();

	ServerLogic(bool check_res_mode, bool unittest_mode, int total_server, int server_index, char* path);
	~ServerLogic();

	virtual int Init();
	virtual int Start();
	virtual int Update();
	virtual int Stop();
	virtual int Release();

	virtual void Free();

	void StopServer(const char* reason);

	int GetFrameElapse() { return m_frame_elapse_time; }

private:
	LogAgent			m_log;
	
	bool				m_check_res_mode;
	bool				m_unittest_mode;

	int					m_total_server;
	int					m_server_index;

	char*				m_workbench_item_txt_path;

	IClockModule		*m_clock;
	IGameModule			*m_game;
	INetworkModule		*m_network;
	INetworkModule		*m_internal_network;
	IRMIModule			*m_rmi;
	ITimerModule		*m_timer;
	IFileWriteModule	*m_filewriter;

	unsigned long		m_current_frame;									// 当前帧
	unsigned long		m_frame_start_time;									// 当前帧开始时间
	int					m_frame_elapse_time;								// 当前帧流逝时间

	ServerNetworkCallback *m_network_callback;
	ServerInternalNetCallback *m_internal_network_callback;
	World *m_world;
};

#endif
