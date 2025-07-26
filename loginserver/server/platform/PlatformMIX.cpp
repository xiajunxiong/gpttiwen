#include "common/md5/md5cpp.h"
#include "common/bintohex.h"
#include "PlatformMIX.h"
#include "engineadapter.h"

#include "servercommon/servercommon.h"

#include "server/loginlog.h"

bool PlatformMIX::Check(NetID netid, PlatName pname, unsigned int login_time, LoginStr str, short server)
{
	const static char appKey[] = "f70d9181fb6e75e58219d0d76c513a1c";
	static const unsigned int LOGIN_OUT_OF_TIME = 3600 * 24;	// 超过24小时则平台session不能再用
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance( ).Time( ));
	if ( now > LOGIN_OUT_OF_TIME + login_time )
	{
		std::string tmp_str(str, sizeof(LoginStr));
		loginlog::g_log_login.printf(LL_INFO, "PlatformMIX::Check LoginStr LOGIN_OUT_OF_TIME, netid:%d plat_name:%s login_time:%u, str:%s, server:%d  now:%u\n", netid, pname, login_time, tmp_str.c_str(), server, now);

		return false;
	}

	char str_tmp[512] = {0};
	int str_len = SNPRINTF(str_tmp,sizeof(str_tmp),"%s%u%s",pname,login_time,appKey);
	if ( str_len <= 0 )
	{
		std::string tmp_str(str, sizeof(LoginStr));
		loginlog::g_log_login.printf(LL_INFO, "PlatformMIX::Check LoginStr str_len invalid, netid:%d plat_name:%s login_time:%u, str:%s, server:%d  str_len:%d\n", netid, pname, login_time, tmp_str.c_str(), server, str_len);
		return false;
	}

	MD5 md5;
	MD5_buffer(&md5,str_tmp,(unsigned long)str_len);

	char md5tmp[sizeof(MD5) * 2];
	BinToHex((const char*)md5,sizeof(MD5),md5tmp);

	if ( memcmp(str,md5tmp,sizeof(MD5) * 2) != 0 )
	{
		std::string tmp_str(str, sizeof(LoginStr));
		loginlog::g_log_login.printf(LL_INFO, "PlatformMIX::Check LoginStr md5 mismatch, netid:%d plat_name:%s login_time:%u, str:%s, server:%d\n", netid, pname, login_time, tmp_str.c_str(), server);
		return false;
	}

	return true;
}

