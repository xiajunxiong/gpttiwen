#include "funopenconfig.hpp"
#include "servercommon/servercommon.h"
#include "checkresourcecenter.hpp"
#include "gamelog.h"

FunOpenConfig::FunOpenConfig()
{
	
}

FunOpenConfig::~FunOpenConfig()
{

}

bool FunOpenConfig::Init(const std::string &configname, std::string *err)
{
	char errinfo[1024] = { 0 };
	int iRet = 0;

	TiXmlDocument document;
	if (configname == "" || !document.LoadFile(configname))
	{
		SNPRINTF(errinfo, sizeof(errinfo), "Load FunOpenConfig [%s] Error.%s", configname.c_str(), document.ErrorDesc());
		*err = errinfo;
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	if (NULL == RootElement)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "Load FunOpenConfig [%s] Error.cannot find root node.", configname.c_str());
		*err = errinfo;
		return false;
	}

	{
		// 功能开启
		TiXmlElement * funopen_element = RootElement->FirstChildElement("funopen");
		if (NULL == funopen_element)
		{
			*err = configname + ": xml node error in [config->funopen]";
			return false;
		}

		iRet = this->InitFunOpenConfig(funopen_element);
		if (0 != iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitFunOpenConfig  failed %d ", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	return true;
}

const FunOpenCfg* FunOpenConfig::GetFunOpenCfg(int fun_type)
{
	if (fun_type <= FUN_TYPE_INVALID || fun_type >= FUN_TYPE_MAX)
	{
		return NULL;
	}

	return &m_fun_cfg_list[fun_type];
}

int FunOpenConfig::InitFunOpenConfig(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int server_fun_id = 0;
		if (!GetSubNodeValue(dataElement, "server_id", server_fun_id) || 
			server_fun_id < FUN_TYPE_INVALID || server_fun_id >= FUN_TYPE_MAX)
		{
			return -1;
		}

		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq < 1)
		{
			return -4;
		}

		int level = 0;
		if (!GetSubNodeValue(dataElement, "level", level) || level < 0)
		{
			return -2;
		}
		if (FUN_TYPE_INVALID == server_fun_id)
		{
			dataElement = dataElement->NextSiblingElement();
			continue;
		}

		if (m_fun_cfg_list[server_fun_id].fun_id > 0)
		{
			// 重复了哦
			return -100;
		}

		

		int task = 0;
		if (!GetSubNodeValue(dataElement, "task", task) || task < 0)
		{
			return -3;
		}

		int open_day = 0;
		if (!GetSubNodeValue(dataElement, "open_day", open_day) || open_day < 0)
		{
			return -4;
		}

		if (task > 0)
		{
			CheckResourceCenter::Instance().GetTaskCheck()->Add(task, __FUNCTION__);
		}

		m_fun_cfg_list[server_fun_id].fun_id = server_fun_id;
		m_fun_cfg_list[server_fun_id].limit_level = level;
		m_fun_cfg_list[server_fun_id].limit_task = task;
		m_fun_cfg_list[server_fun_id].limit_open_day = open_day;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

