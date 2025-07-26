#include "common/md5/md5cpp.h"
#include "platformasqjh.h"
#include "common/bintohex.h"
#include "engineadapter.h"

#include "servercommon/servercommon.h"

bool PlatformSqjh::Check(NetID netid,PlatName pname,unsigned int login_time,LoginStr str,short server)
{
	const static char game_login_key[] = "24f357b73580979e75cb0d40dfcb6f94";
	static const unsigned int LOGIN_OUT_OF_TIME = 3600 * 24;	// 超过24小时则平台session不能再用
	if ( EngineAdapter::Instance( ).Time( ) - login_time > LOGIN_OUT_OF_TIME )
	{
		return false;
	}

	char str_tmp[512] = {0};
	int str_len = SNPRINTF(str_tmp,sizeof(str_tmp),"%s%u%s",pname,login_time,game_login_key);
	if ( str_len <= 0 ) return false;

	MD5 md5;
	MD5_buffer(&md5,str_tmp,(unsigned long)str_len);

	char md5tmp[sizeof(MD5) * 2];
	BinToHex((const char*)md5,sizeof(MD5),md5tmp);

	if ( memcmp(str,md5tmp,sizeof(MD5) * 2) == 0 )
	{
		return true;
	}

	return false;
}

