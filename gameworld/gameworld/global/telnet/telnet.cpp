#include "telnet.h"
#include "world.h"
#include "servercommon/internalprotocal/telnetgameworldprotocal.h"
#include "global/activity/activitymanager.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/activity/activity.hpp"

#define REGISTER_TELNET_HANLDER(telnet_str, telnet_handler) { \
	int telnet_hash = GetStringHash(telnet_str); \
	if (m_telnet_handler_map.find(telnet_hash) != m_telnet_handler_map.end()) { \
		printf("REGISTER_TELNET_HANLDER telnet_handler[%s] hash conflit...", telnet_str); \
		assert(0); \
	} \
	m_telnet_handler_map.insert(TelnetHandlerMap::value_type(telnet_hash, telnet_handler)); \
}

// 验证用KEY 目前先写死
static const char key[256] = { "!q@w#e$r" }; 
//< 输出缓存
static const int MAX_MSG_BUFF_LENGTH = 1024 * 20;
static char msg_buff[MAX_MSG_BUFF_LENGTH] = { 0 };

Telnet & Telnet::Instance()
{
	static Telnet g_telnet;
	return g_telnet;
}

void Telnet::OnTelnetAccept()
{
	// 给刚连上的客户端打个招呼
	std::string str = "hello ! Welcome to Telnet!\nPlease enter the secret key:\n";
	this->SendToTelnet(str);
}

void Telnet::OnRecvFromTelnet(NetID netid, const char *data, int length)
{
	telnetgameprotocol::TelnetGameCmdReq *req = (telnetgameprotocol::TelnetGameCmdReq*)data;
	int header_len = sizeof(telnetgameprotocol::TelnetGameCmdReq);
	int msg_len = length - header_len;
	if (msg_len <= 0)
		return;

	const char * str = (data + header_len);
	std::string msg = str;

	bool ret = true;
	std::string out_str;
	if (m_check_flag)
	{
		this->OnTelnet(msg , out_str);
	}
	else
	{
		ret = this->OnCheckKey(msg , out_str);
	}

	this->SendToTelnet(out_str);

	if (!ret)
	{
		World::GetInstWorld()->OnDisconnectTelnet();
	}
}

static const int MAX_CHECK_ERROR_NUM = 3;	// 达到3次错误则会断开连接

Telnet::Telnet():m_check_flag(false), m_check_error_count(0)
{
	// 注册处理函数
	REGISTER_TELNET_HANLDER("ShowData", &Telnet::ShowData);
	REGISTER_TELNET_HANLDER("GetActivityStatus", &Telnet::GetActivityStatus);
}

Telnet::~Telnet()
{

}

bool Telnet::OnTelnet(const std::string &cmd, std::string & out_str)
{
	// 由于cmd包含了命令字符串，个别可能还有参数，因此定义格式为空格分隔，
	// 比如 forbidtalk 1234 ，其中forbidtalk为命令，1234为参数（role_id),中间用空格隔开

	std::istringstream iss;
	iss.str(cmd);

	std::string c;
	if ((iss >> c).fail())
	{
		out_str = "Telnet::OnTelnet (iss >> c).fail()";
		return false;
	}

	int cmd_hash = GetStringHash(c.length(), c.c_str());
	TelnetHandlerMap::iterator it = m_telnet_handler_map.find(cmd_hash);
	if (it == m_telnet_handler_map.end())
	{
		out_str = "The command does not exist or the command is incorrectly entered.";
		return false;
	}
	else
	{
		TelnetHandler f = it->second;
		bool ret = (this->*f)(iss, out_str);
		if (ret)
		{
			printf("Suc %s \n",  cmd.c_str());
		}
		else
		{
			printf("Fail %s \n",cmd.c_str());
			std::string error_str = "The command does not exist or the command is incorrectly entered.";
			error_str += out_str;
			out_str = error_str;
		}
		return ret;
	}

	return true;
}

bool Telnet::OnCheckKey(const std::string &cmd, std::string & out_str)
{
	if (0 != strcmp(cmd.c_str(), key))
	{
		++m_check_error_count;
		if (m_check_error_count >= MAX_CHECK_ERROR_NUM)
		{
			out_str = "Secret key error ,Disconnect from the GameWorld.";
			return false;
		}
		else
		{
			out_str = "Secret key error , Please check the secret key.";
			return false;
		}
	}
	else
	{
		m_check_flag = true;
		m_check_error_count = 0;
		out_str = "successful authentication!\nWelcome to use Telnet!\n";
	}

	return true;
}

void Telnet::SendToTelnet(const std::string & msg)
{
	int size = msg.length() + 1;
	telnetgameprotocol::GameTelnetCmdAck ack;
	ack.data_length = size;
	int header_len = sizeof(telnetgameprotocol::GameTelnetCmdAck);
	memcpy(msg_buff, &ack, header_len);
	int real_size = header_len + size;
	if (real_size >= MAX_MSG_BUFF_LENGTH)
	{
		// 直接给掐断
		real_size = MAX_MSG_BUFF_LENGTH - 1;
	}
	memcpy(msg_buff + header_len, msg.c_str(), real_size);
	// 因为size本身就+1了 所以这里用real_size
	msg_buff[real_size] = '\0';
	World::GetInstWorld()->SendToTelnet(msg_buff, header_len + real_size);
}

bool Telnet::ShowData(std::istringstream & iss, std::string & out_str)
{
	// 这个接口主要用于热更补丁, 需要使用的人自行完善对应的逻辑

	out_str += "Telnet::ShowData [TEST]";
	return true;
}

bool Telnet::GetActivityStatus(std::istringstream & iss, std::string & out_str)
{
	char temp_str[128] = { 0 };
	// 这是一个展示用例 ,后续执行添加时可以参考 
	int activity_type = 0;
	if (!(iss >> activity_type).fail())
	{
		if (activity_type > ACTIVITY_TYPE_INVALID && activity_type < ACTIVITY_TYPE_MAX)
		{
			Activity * activity = ActivityManager::Instance().ForceGetActivity(activity_type);
			if (NULL == activity)
			{
				SNPRINTF(temp_str, 128, "%d", activity_type);
				out_str += temp_str;
				out_str += " no exist;";
				return false;
			}
			else
			{
				activity->GetActivityStausInfo(out_str);
			}
		}
		else if(activity_type > RAND_ACTIVITY_TYPE_BEGIN && activity_type < RAND_ACTIVITY_TYPE_END)
		{
			RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
			if (NULL == ramgr)
			{
				out_str = " NULL == RandActivityManager ";
				return false;
			}

			RandActivity * ra = ramgr->ForceGetRandActivity(activity_type);
			if (NULL == ra)
			{
				SNPRINTF(temp_str, 128, "%d", activity_type);
				out_str += temp_str;
				out_str += " no exist;";
				return false;
			}
			else
			{
				ra->GetRandActivityStausInfo(out_str);
			}
		}
		else
		{
			SNPRINTF(temp_str, 128, "%d", activity_type);
			out_str += temp_str;
			out_str += "no exist;";
			return false;
		}
	}
	else
	{
		out_str = "(iss >> activity_type).fail()";
		return false;
	}

	return true;
}
