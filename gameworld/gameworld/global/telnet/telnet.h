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

	// ��Ϊ��Ӧ��ָ���
	// ShowDataû�о����߼�ʵ��, ��Ҫ�����ȸ�����,��Ҫʹ�õ����Լ�ʵ�ֶ�Ӧ���߼�
	bool ShowData(std::istringstream & iss , std::string & out_str);
	// ����һ��չʾ���� ,�������ʱ���Բο� 
	// �����Ӧ�Ļ״̬����
	bool GetActivityStatus(std::istringstream & iss, std::string & out_str);

	bool m_check_flag;
	int m_check_error_count;

	TelnetHandlerMap m_telnet_handler_map;
};


#endif	//__TELNET_H__
