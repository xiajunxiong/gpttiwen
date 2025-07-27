#include "bigcrossserverconfig.hpp"
#include "servercommon/configcommon.h"


BigCrossServerConfig::BigCrossServerConfig()
{

}

BigCrossServerConfig::~BigCrossServerConfig()
{

}


BigCrossServerConfig & BigCrossServerConfig::Instance()
{
	static BigCrossServerConfig st_bcs;
	return st_bcs;
}


bool BigCrossServerConfig::Init(const std::string &cfg_dir, std::string *err)
{
	if (!this->InitBigCrossXML(cfg_dir + "bigcross.xml", err))
	{
		return false;
	}

	if (this->IsBigCrossServer())
	{
		if (!this->InitBigCrossDBXml(cfg_dir + "bigcross_db.xml", err))
		{
			return false;
		}
	}

	return true;
}

bool BigCrossServerConfig::InitBigCrossXML(const std::string &path, std::string *err)
{
	char errinfo[1024] = { 0 };
	TiXmlDocument document;
	if (path == "" || !document.LoadFile(path.c_str()))
	{
		SNPRINTF(errinfo, sizeof(errinfo), "Load config [%s] Error.%s", path.c_str(), document.ErrorStr());
		*err = errinfo;
		return false;
	}
	TiXmlElement *RootElement = document.RootElement();
	if (NULL == RootElement)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "Load config [%s] Error.cannot find root node.", path.c_str());
		*err = errinfo;
		return false;
	}

	int iRet = 0;

	{
		// BigCross 
		TiXmlElement *root_element = RootElement->FirstChildElement("BigCrossServerAddr");
		if (NULL == root_element)
		{
			*err = path + ": no [BigCrossServerAddr].";
			return false;
		}
		std::string error_msg;
		iRet = this->InitBigCrossConfig(root_element, error_msg);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitBigCrossConfig failed %d  errormsg[%s]", path.c_str(), iRet, error_msg.c_str());
			*err = errinfo;
			return false;
		}
	}

	{
		// BigCross Other
		TiXmlElement *root_element = RootElement->FirstChildElement("BigCrossOther");
		if (NULL == root_element)
		{
			*err = path + ": no [BigCrossOther].";
			return false;
		}
		std::string error_msg;
		iRet = this->InitBigCrossOtherConfig(root_element, error_msg);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitBigCrossOtherConfig failed %d  errormsg[%s]", path.c_str(), iRet, error_msg.c_str());
			*err = errinfo;
			return false;
		}
	}

	{
		// For Cross
		TiXmlElement *root_element = RootElement->FirstChildElement("ForCross");
		if (NULL == root_element)
		{
			*err = path + ": no [ForCross].";
			return false;
		}
		std::string error_msg;
		iRet = this->InitBigCrossConfigForCross(root_element, error_msg);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitBigCrossConfigForCross failed %d  errormsg[%s]", path.c_str(), iRet, error_msg.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

bool BigCrossServerConfig::InitBigCrossDBXml(const std::string &path, std::string *err)
{
	char errinfo[1024] = { 0 };
	TiXmlDocument document;
	if (path == "" || !document.LoadFile(path.c_str()))
	{
		SNPRINTF(errinfo, sizeof(errinfo), "Load config [%s] Error.%s", path.c_str(), document.ErrorStr());
		*err = errinfo;
		return false;
	}
	TiXmlElement *RootElement = document.RootElement();
	if (NULL == RootElement)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "Load config [%s] Error.cannot find root node.", path.c_str());
		*err = errinfo;
		return false;
	}

	int iRet = 0;

	{
		// BigCross Dataaccess
		TiXmlElement *root_element = RootElement->FirstChildElement("BigCrossDB");
		if (NULL == root_element)
		{
			*err = path + ": no [BigCrossDB].";
			return false;
		}
		std::string error_msg;
		iRet = this->InitBigCrossDataaccessConfig(root_element, error_msg);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitBigCrossDataaccessConfig failed %d  errormsg[%s]", path.c_str(), iRet, error_msg.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

int BigCrossServerConfig::InitBigCrossConfig(TiXmlElement *RootElement, std::string& errormsg)
{
	if (NULL == RootElement)
		return -101010;

	NetAddress addr;
	if (!GetSubNodeValue(RootElement, "Ip", addr.ipstr) || addr.ipstr.empty())
	{
		errormsg = STRING_SPRINTF("[config->BigCrossServerAddr->IP] error");
		return -1;
	}
	if (!GetSubNodeValue(RootElement, "Port", addr.port) || 0 == addr.port)
	{
		errormsg = STRING_SPRINTF("[config->BigCrossServerAddr->Port] error");
		return -2;
	}

	m_big_cross_server_config.big_cross_server_addr = addr;

	{
		TiXmlElement* ip_Element = RootElement->FirstChildElement("AllowIPPrefix");
		if (NULL == ip_Element) return 0;

		while (NULL != ip_Element)
		{
			// DB_CONFIG
			std::string temp_ip;
			if (!GetNodeValue(ip_Element, temp_ip) || temp_ip.empty())
			{
				errormsg = STRING_SPRINTF("[BigCrossServerAddr->AllowIPPrefix] error");
				return -3;
			}

			m_big_cross_server_config.allpw_ip_prefix_set.insert(temp_ip);
			ip_Element = ip_Element->NextSiblingElement();
		}

	}

	return 0;
}

int BigCrossServerConfig::InitBigCrossDataaccessConfig(TiXmlElement *RootElement, std::string& errormsg)
{
	TiXmlElement * db_config_element = RootElement->FirstChildElement("DB_Config");
	if (NULL == db_config_element)
	{
		return -101010;
	}

	{
		// DB_CONFIG
		if (!GetSubNodeValue(db_config_element, "Fast_DB_Name", m_big_cross_dataaccess_config.fast_db_name) || m_big_cross_dataaccess_config.fast_db_name.empty())
		{
			errormsg = STRING_SPRINTF("[DB_Config->Fast_DB_Name] error");
			return -1;
		}

		if (!GetSubNodeValue(db_config_element, "Fast_DB_Init_Size_MB", m_big_cross_dataaccess_config.fast_db_init_size_MB) || m_big_cross_dataaccess_config.fast_db_init_size_MB < 1)
		{
			errormsg = STRING_SPRINTF("[DB_Config->Fast_DB_Init_Size_MB] error");
			return -2;
		}

		if (!GetSubNodeValue(db_config_element, "Mysql_DB_Username", m_big_cross_dataaccess_config.mysql_db_username) || m_big_cross_dataaccess_config.mysql_db_username.empty())
		{
			errormsg = STRING_SPRINTF("[DB_Config->Mysql_DB_Username] error");
			return -3;
		}

		if (!GetSubNodeValue(db_config_element, "Mysql_DB_Password", m_big_cross_dataaccess_config.mysql_db_password) || m_big_cross_dataaccess_config.mysql_db_password.empty())
		{
			errormsg = STRING_SPRINTF("[DB_Config->Mysql_DB_Password] error");
			return -4;
		}

		{
			//mysql_addr
			TiXmlElement * mysql_addr_element = db_config_element->FirstChildElement("Mysql_Addr");
			if (NULL == mysql_addr_element)
			{
				return -5;
			}

			NetAddress addr;
			if (!GetSubNodeValue(mysql_addr_element, "Ip", addr.ipstr) || addr.ipstr.empty())
			{
				errormsg = STRING_SPRINTF("[DB_Config->Mysql_DB_Password->Mysql_Addr->IP] error");
				return -501;
			}
			if (!GetSubNodeValue(mysql_addr_element, "Port", addr.port) || 0 == addr.port)
			{
				errormsg = STRING_SPRINTF("[DB_Config->Mysql_DB_Password->Mysql_Addr->Port] error");
				return -502;
			}

			m_big_cross_dataaccess_config.mysql_addr = addr;
		}

		if (!GetSubNodeValue(db_config_element, "Mysql_DB_Charset", m_big_cross_dataaccess_config.mysql_db_charset) || m_big_cross_dataaccess_config.mysql_db_charset.empty())
		{
			errormsg = STRING_SPRINTF("[DB_Config->Mysql_DB_Charset] error");
			return -6;
		}

		if (!GetSubNodeValue(db_config_element, "Mysql_DB_Name", m_big_cross_dataaccess_config.mysql_db_name) || m_big_cross_dataaccess_config.mysql_db_name.empty())
		{
			errormsg = STRING_SPRINTF("[DB_Config->Mysql_DB_Name] error");
			return -7;
		}

		if (!GetSubNodeValue(db_config_element, "FlushInterval", m_big_cross_dataaccess_config.flushinterval) || m_big_cross_dataaccess_config.flushinterval < 1)
		{
			errormsg = STRING_SPRINTF("[DB_Config->FlushInterval] error");
			return -8;
		}
	}

	TiXmlElement * modile_addr_element = RootElement->FirstChildElement("Module_Addr");
	if (NULL == modile_addr_element)
	{
		return -202020;
	}

	{
		// module_addr
		NetAddress addr;
		if (!GetSubNodeValue(modile_addr_element, "Ip", addr.ipstr) || addr.ipstr.empty())
		{
			errormsg = STRING_SPRINTF("[BigCrossDB->Module_Addr->IP] error");
			return -9;
		}
		if (!GetSubNodeValue(modile_addr_element, "Port", addr.port) || 0 == addr.port)
		{
			errormsg = STRING_SPRINTF("[BigCrossDB->Module_Addr->Port] error");
			return -10;
		}

		m_big_cross_dataaccess_config.big_cross_dataaccess_module_addr = addr;
	}

	return 0;
}

int BigCrossServerConfig::InitBigCrossOtherConfig(TiXmlElement *RootElement, std::string& errormsg)
{
	if (NULL == RootElement) return -10086;

	int is_bigcross = 0;
	if (!GetSubNodeValue(RootElement, "IsBigCross", is_bigcross))
	{
		errormsg = STRING_SPRINTF("[config->BigCrossOther->IsBigCross] error");
		return -1;
	}
	m_big_cross_other_cfg.is_bigcross = is_bigcross > 0;

	if (!GetSubNodeValue(RootElement, "BattleServerNum", m_big_cross_other_cfg.battle_server_num) || m_big_cross_other_cfg.battle_server_num <= 0)
	{
		errormsg = STRING_SPRINTF("[config->BigCrossOther->BattleServerNum] error");
		return -2;
	}

	return 0;
}

int BigCrossServerConfig::InitBigCrossConfigForCross(TiXmlElement *RootElement, std::string& errormsg)
{
	if (NULL == RootElement) return -10088;

	int is_open_bigcross = 0;
	if (!GetSubNodeValue(RootElement, "IsOpenBigCross", is_open_bigcross))
	{
		errormsg = STRING_SPRINTF("[config->ForCross->IsOpenBigCross] error");
		return -1;
	}
	m_big_cross_cfg_for_cross.is_open_big_cross = (is_open_bigcross > 0);

	return 0;
}
