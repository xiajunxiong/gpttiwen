#include "bigcrossserver_tableconfig.hpp"
#include "servercommon/configcommon.h"


BigCrossTableConfig::BigCrossTableConfig()
{

}

BigCrossTableConfig::~BigCrossTableConfig()
{

}


BigCrossTableConfig & BigCrossTableConfig::Instance()
{
	static BigCrossTableConfig st_bcs;
	return st_bcs;
}


bool BigCrossTableConfig::Init(const std::string &path, std::string *err)
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
		// NoCacheTable 
		TiXmlElement *root_element = RootElement->FirstChildElement("NoCacheTable");
		if (NULL == root_element)
		{
			*err = path + ": no [NoCacheTable].";
			return false;
		}
		std::string error_msg;
		iRet = this->InitNoCaCheConfig(root_element, error_msg);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitNoCaCheConfig failed %d  errormsg[%s]", path.c_str(), iRet, error_msg.c_str());
			*err = errinfo;
			return false;
		}
	}

	{
		// NoDelayTables 
		TiXmlElement *root_element = RootElement->FirstChildElement("NoDelayTables");
		if (NULL == root_element)
		{
			*err = path + ": no [NoDelayTables].";
			return false;
		}
		std::string error_msg;
		iRet = this->InitNoDelayConfig(root_element , error_msg);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitNoDelayConfig failed %d  errormsg[%s]" , path.c_str(), iRet , error_msg.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

int BigCrossTableConfig::InitNoCaCheConfig(TiXmlElement *RootElement, std::string& errormsg)
{
	if (NULL == RootElement) return -101010;

	TiXmlElement* TableElement = RootElement->FirstChildElement("Table");
	if (NULL == TableElement) return 0;

	while (NULL != TableElement)
	{
		// DB_CONFIG
		std::string temp_table;
		if (!GetNodeValue(TableElement, temp_table) || temp_table.empty())
		{
			errormsg = STRING_SPRINTF("[NoDelayTables->Table] error");
			return -1;
		}

		m_no_cache_config.no_cache_vec.push_back(temp_table);
		TableElement = TableElement->NextSiblingElement();
	}

	return 0;
}

int BigCrossTableConfig::InitNoDelayConfig(TiXmlElement *RootElement, std::string& errormsg)
{
	if (NULL == RootElement) return -101010;

	TiXmlElement* TableElement = RootElement->FirstChildElement("Table");
	if (NULL == TableElement) return 0;

	while (NULL != TableElement)
	{
		// DB_CONFIG
		std::string temp_table;
		if (!GetNodeValue(TableElement, temp_table) || temp_table.empty())
		{
			errormsg = STRING_SPRINTF("[NoDelayTables->Table] error");
			return -1;
		}

		m_no_delay_config.no_delay_vec.push_back(temp_table);
		TableElement = TableElement->NextSiblingElement();
	}

	return 0;
}

