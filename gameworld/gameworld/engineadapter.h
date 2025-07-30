
#ifndef ENGINEADAPTER_H
#define ENGINEADAPTER_H

#include "iclockmodule.h"
#include "igamemodule.h"
#include "iconfigmodule.h"
#include "irmimodule.h"
#include "itimermodule.h"
#include "ifilewritemodule.h"
#include "servercommon/serverdef.h"
#include <map>
#include "servercommon/userprotocal/msgheader.h"

class ILogModule;
class IScriptModule;
class INetworkModule;

class EngineAdapter
{
public:
	static EngineAdapter &Instance()
	{
		static EngineAdapter ea;
		return ea;
	}

	void Init(ILogModule *log, 
		IConfigModule *config, 
		IGameModule *game, 
		INetworkModule *network, 
		IClockModule *clock, 
		IRMIModule *rmi_module,
		ITimerModule *timer,
		IFileWriteModule *filewrite);

	unsigned long GetGameTime() { return m_clock->GetFrameTime(); }
	void SetLogDir(std::string log_dir) { m_log_dir = log_dir; }
	const std::string& GetLogDir() const { return m_log_dir; }

	unsigned int DayID() { return m_clock->DayID(); }
	unsigned int WeekID(); // 返回周一0点的时间戳作为一周的ID
	time_t Time() { return m_clock->Time(); }
	const tm *LocalTime() { return m_clock->LocalTime(); }
	const char *AscTime() { return m_clock->AscTime(); }
 
	int NextMinuteInterval(int second) { return m_clock->NextMinuteInterval(second); }
	int NextHourInterval(int minute, int second) { return m_clock->NextHourInterval(minute, second); }
	int NextDayInterval(int hour, int minute, int second) { return m_clock->NextDayInterval(hour, minute, second); }
	int NextWeekInterval(int wday, int hour, int minute, int second) { return m_clock->NextWeekInterval(wday, hour, minute, second); }
	int NextMouthInterval(int date, int hour, int minute, int second) { return m_clock->NextMouthInterval(date, hour, minute, second); }

	void StopGame() { m_game->StopGame(); }
	bool GetWorkPath(char* path, unsigned long size)const { return m_game->GetWorkPath(path, size); }

	template<class T>
	bool ConfigSyncValue(const char* path, T* result, const T& init_value)
	{
		return m_config->SyncValue(path, result, init_value);
	}

	template<class T>
	bool ConfigGetValue(const char* path, T* out_value)
	{
		return m_config->GetValue(path, out_value);
	}

	bool NetSend(const GSNetID &netid, const void *data, unsigned int length);
	bool NetSendSerializeMsg(const GSNetID &netid, IMessageSerializer* msg);
	bool NetSendSerializeMsg2(const GSNetID & netid,const IMessageSerializer2 * msg);
	bool NetMulSend(GSNetID *netidlist, int num, const char *data, unsigned int length);
	bool NetSendAll(NetID gateway_netid, const char *data, unsigned int length);
	void NetDisconnect(const GSNetID &netid, UserID user_id, const GameName role_name, int scene_id, const char *reason);
	void NetHasCheck(const GSNetID &gsnetid, const char* role_name, int role_id);
	const char* GetSerializeData(IMessageSerializer* msg, int* out_length);

	bool RMIObjRegister(rmi::RMIObject *obj)
	{
		return m_rmi->Register(obj);
	}

	rmi::Session RMICreateSession(const char *ip, Port port)
	{
		return m_rmi->CreateSession(ip, port);
	}

	void RMICloseSession(const rmi::Session &session)
	{
		m_rmi->CloseSession(session);
	}

	void RMIStartAsyncThread(int threadnum)
	{
		m_rmi->StartAsyncThread(threadnum);
	}

	void CreateTimer(unsigned int interval_ms, ITimerCallback *callback_obj)
	{
		m_time->CreateTimer(interval_ms, callback_obj);
	}

	void CreateTimerSecond(unsigned int interval_s, ITimerCallback *callback_obj)
	{
		m_time->CreateTimerSecond(interval_s, callback_obj);
	}

	bool AsynFileWrite(const char *path, const char *data, int length, bool is_append)
	{
		return m_filewrite->Write(path, data, length, is_append);
	}

	ILogModule * GetLogModule()
	{
		return m_log;
	}

	IClockModule* GetClockModule()
	{
		return m_clock;
	}

	/*
	* \brief 生成一个序列号
	* \param server_id 当前服务器ID
	* \param role_id 角色id
	* \param out_serial_num 用于返回的序列号缓冲区
	* \note 格式：role_id-timestamp-index 
	*/
	void GenerateSerialNum(int server_id, int role_id, SerialNum out_serial_num);

	void PrintErrorConfigLog(const char* path);

public:
	int					m_server_index;
	int					m_game_thread;
private:
	EngineAdapter();
	~EngineAdapter();

	IConfigModule		*m_config;
	IGameModule			*m_game;
	INetworkModule		*m_network;
	IClockModule		*m_clock;
	ILogModule			*m_log;
	IRMIModule			*m_rmi;
	ITimerModule		*m_time;
	IFileWriteModule	*m_filewrite;

	char				*m_buffer;	// 用于NetSend是组包

	unsigned int		m_serial_num_timestamp;
	int					m_serial_num_index;

	std::string			m_log_dir;
};

#endif

