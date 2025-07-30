#ifndef  __TELNET_H__
#define  __TELNET_H__

#include "engineadapter.h"
#include "inetworkmodule.h"
#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"

class Telnet
{
	typedef bool (Telnet::*TelnetHandler)(std::istringstream& iss, std::string & out_str);
	typedef std::map<int, Telnet::TelnetHandler> TelnetHandlerMap;

public:
	static Telnet & Instance();
	void OnTelnetAccept();
	void OnRecvFromTelnet(NetID netid, const char *data, int length);

private:
	Telnet();
	~Telnet();

	bool OnTelnet(const std::string &cmd, std::string & out_str);
	bool OnCheckKey(const std::string &cmd, std::string & out_str);
	void SendToTelnet(const std::string & msg);

	// 下为对应的指令处理
	// ShowData没有具体逻辑实现, 主要用于热更补丁,需要使用的人自己实现对应的逻辑
	bool ShowData(std::istringstream & iss , std::string & out_str);
	// 这是一个展示用例 ,后续添加时可以参考 
	// 输出对应的活动状态数据
	bool GetActivityStatus(std::istringstream & iss, std::string & out_str);

	bool m_check_flag;
	int m_check_error_count;

	TelnetHandlerMap m_telnet_handler_map;
};


#endif	//__TELNET_H__
