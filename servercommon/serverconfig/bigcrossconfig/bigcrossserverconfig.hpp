#ifndef __BIG_CROSS_SERVER_CONFIG_H__
#define __BIG_CROSS_SERVER_CONFIG_H__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include <string>
#include <string.h>
#include <set>

struct BigCrossServerConfigItem									// big_cross配置
{
	NetAddress big_cross_server_addr;							// big_cross地址
	std::set<std::string> allpw_ip_prefix_set;
};

struct BigCrossServerConfigForCross
{
	BigCrossServerConfigForCross() : is_open_big_cross(false) {}

	bool is_open_big_cross;
};

struct BigCrossOtherConfig
{
	BigCrossOtherConfig() : is_bigcross(false), battle_server_num(0) {}

	bool is_bigcross;
	int battle_server_num;
};

struct BigCrossDataaccessConfigItem								// big_cross Dataaccess 配置
{
	BigCrossDataaccessConfigItem() :fast_db_name("dbname"), fast_db_init_size_MB(1024), mysql_db_username("root"), mysql_db_password("root"),
		mysql_addr(), mysql_db_charset("utf8"), mysql_db_name("dbname"), flushinterval(0), big_cross_dataaccess_module_addr()
	{}

	std::string fast_db_name;								// fast db名
	unsigned int fast_db_init_size_MB;						// fast db 最大使用共享内存大小
	std::string mysql_db_username;							// 数据库用户名
	std::string mysql_db_password;							// 数据库密码
	NetAddress mysql_addr;									// ip和端口
	std::string mysql_db_charset;							// 数据库连接字符集
	std::string mysql_db_name;								// 数据库名
	unsigned short flushinterval;							// 刷入数据库时间间隔

	NetAddress big_cross_dataaccess_module_addr;				// big_cross Dataaccess  module 地址
};

class BigCrossServerConfig
{
public:
	static BigCrossServerConfig & Instance();

	bool Init(const std::string &cfg_dir, std::string *err);

	const BigCrossServerConfigItem & GetBigCrossServerConfig() { return m_big_cross_server_config; }
	const BigCrossDataaccessConfigItem & GetBigCrossDataaccessConfig() { return m_big_cross_dataaccess_config; }
	bool IsBigCrossServer() const { return m_big_cross_other_cfg.is_bigcross; }
	int GetBattleServerNum() const { return m_big_cross_other_cfg.battle_server_num; }
	bool IsOpenBigCross() const { return m_big_cross_cfg_for_cross.is_open_big_cross; }
private:
	bool InitBigCrossXML(const std::string &path, std::string *err);
	bool InitBigCrossDBXml(const std::string &path, std::string *err);
	int InitBigCrossConfig(TiXmlElement *RootElement, std::string& errormsg);
	int InitBigCrossDataaccessConfig(TiXmlElement *RootElement, std::string& errormsg);
	int InitBigCrossOtherConfig(TiXmlElement *RootElement, std::string& errormsg);
	int InitBigCrossConfigForCross(TiXmlElement *RootElement, std::string& errormsg);

	BigCrossServerConfig();
	~BigCrossServerConfig();

	BigCrossServerConfigItem			m_big_cross_server_config;		// big_cross 配置
	BigCrossDataaccessConfigItem		m_big_cross_dataaccess_config;	// big_cross dataaccess 配置
	BigCrossOtherConfig					m_big_cross_other_cfg;
	BigCrossServerConfigForCross		m_big_cross_cfg_for_cross;
};

#endif	// __BIG_CROSS_SERVER_CONFIG_H__

