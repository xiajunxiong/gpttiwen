#include <memory.h>

#include "platform.h"
#include "platform91wan.h"
#include "platform4399.h"
#include "platformyujun.h"
#include "platform360.h"
#include "platform336.h"
#include "PlatformMIX.h"
#include "engineadapter.h"
#include "servercommon/servercommon.h"
#include "servercommon/serverconfig/serverconfigpool.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "server/protocal/msglogin.h"
#include "servercommon/userprotocal/msgsystem.h"
#include "servercommon/errornum.h"
#include "server/loginlog.h"

OLD_USE_INTERNAL_NETWORK();

Platform * Platform::Create()
{
	switch(ServerConfigPool::Instance().common_config.login_server_account_check_type)
	{
	case CommonConfig::PLAT_TYPE_YOUKE:
		return new Platform();	// ����֤�������ο�ģʽ
		break;

	case CommonConfig::PLAT_TYPE_MIX:
		return new PlatformMIX( );	
		break;
	default:
		return new Platform();
	};
}
bool Platform::CheckAccount(NetID netid, PlatName pname, unsigned int login_time, LoginStr str, short server)
{
    // ����˺����Ƿ�Ϊ��
    if (pname[0] == 0) 
    {
        loginlog::g_log_login.printf(LL_ERROR, "[Platform::CheckAccount] pname is empty. netid:%d server:%d", netid, server);
        return false;
    }

    // ��������ID�Ƿ�����
    if (!LocalConfig::Instance().IsServerIdAllowed(server))
    {
        loginlog::g_log_login.printf(LL_ERROR, "[Platform::CheckAccount] server_id Not Allowed. pname:%s server_id:%d netid:%d", pname, server, netid);
        return false;
    }

    // ����¼�ַ���ת��Ϊ��д
    for (int i = 0; i < (int)sizeof(LoginStr); ++i)
    {
        if (str[i] >= 'a' && str[i] <= 'z')
        {
            str[i] -= 32;
        }
    }
    
    // ������֤�߼�
    if (Check(netid, pname, login_time, str, server))
    {
        loginlog::g_log_login.printf(LL_INFO, "[Platform::CheckAccount] Check success. pname:%s server_id:%d netid:%d", pname, server, netid);
        return true;
    }

    // ��֤ʧ�ܴ���
    loginlog::g_log_login.printf(LL_ERROR, "[Platform::CheckAccount] Check failed. pname:%s login_time:%u server:%d login_str:%s netid:%d", 
                              pname, login_time, server, str, netid);
	return true;
	
    Protocol::SCAccountKeyError ake;
    EngineAdapter::Instance().NetSend(netid, (const char *)&ake, sizeof(ake));

    Protocol::SCNoticeNum error;
    error.notice_num = errornum::EN_LOGIN_ACCOUNT_KEY_ERROR;
    EngineAdapter::Instance().NetSend(netid, (const char *)&error, sizeof(error));  // �޸��˴˴����Ͷ�����������

    EngineAdapter::Instance().NetDisconnect(netid);

    return false;
}

bool Platform::Check(NetID netid, PlatName pname, unsigned int login_time, LoginStr str, short server)
{
	return true;
}

void Platform::CreateLoginStr(LoginStr login_str, PlatName pname, unsigned int login_time, char cm, short server)
{
	memset(login_str, 0, sizeof(LoginStr));
}

