#include "crossconfig.hpp"

#include "servercommon/configcommon.h"

#include "servercommon/serverconfig/configstruct.h"

CrossConfig::CrossConfig() : m_is_open_cross(false), m_is_hidden_server(false), m_hidden_server_idx(0)
{

}

CrossConfig::~CrossConfig()
{

}

CrossConfig & CrossConfig::Instance()
{
	static CrossConfig cc;
	return cc;
}

bool CrossConfig::Init(const std::string &path, std::string *err)
{
	TiXmlDocument document;
	if (path == "" || !document.LoadFile(path))
	{
		*err = path + "£ºLoad Cross Config Error,\n" + document.ErrorDesc();
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	if (NULL == RootElement)
	{
		*err = path + ": xml root node error.";
		return false;
	}

	{
		TiXmlElement *AddrElement = RootElement->FirstChildElement("CrossServerAddr");
		if (NULL == AddrElement)
		{
			*err = path + ": xml node error in [config->CrossServerAddr]";
			return false;
		}

		NetAddress addr;
		if (!GetSubNodeValue(AddrElement, "Ip", addr.ipstr) || addr.ipstr.empty())
		{
			*err = path + ": xml node error in [config->CrossServerAddr->Ip]";
			return false;
		}
		if (!GetSubNodeValue(AddrElement, "Port", addr.port) || 0 == addr.port)
		{
			*err = path + ": xml node error in [config->CrossServerAddr->Port]";
			return false;
		}

		m_cross_config.cross_server_addr = addr;
	}

	{
		if (!GetSubNodeValue(RootElement, "open_cross", m_is_open_cross))
		{
			*err = path + ": xml node error in [config->open_cross]";
			return false;
		}

		if (!GetSubNodeValue(RootElement, "is_hidden_server", m_is_hidden_server))
		{
			*err = path + ": xml node error in [config->is_hidden_server]";
			return false;
		}

		m_hidden_server_idx = 0;
		if (m_is_hidden_server)
		{
#ifdef _DEBUG
			int hidden_server_idx = 0;
			if (GetSubNodeValue(RootElement, "hidden_server_idx", hidden_server_idx))
			{
				if (hidden_server_idx < 0)
				{
					*err = path + ": xml node error in [config->hidden_server_idx]";
					return false;
				}
				m_hidden_server_idx = hidden_server_idx;
			}
#else
			if (!GetSubNodeValue(RootElement, "hidden_server_idx", m_hidden_server_idx) || m_hidden_server_idx < 0)
			{
				*err = path + ": xml node error in [config->hidden_server_idx]";
				return false;
			}
#endif
		}

		if (m_is_hidden_server && !m_is_open_cross)
		{
			*err = path + ": xml node error in [config->is_hidden_server] and [config->open_cross], hidden server must open cross!";
			return false;
		}
	}

	return true;
}

bool CrossConfig::Reload(const std::string &path, std::string *err, bool *is_cross_addr_change, bool *is_open_cross_change)
{
	CrossConfigItem old_cross_config = m_cross_config;
	bool old_open_cross = m_is_open_cross;

	bool ret = this->Init(path, err);
	if (ret)
	{
		if (NULL != is_cross_addr_change) 
		{
			*is_cross_addr_change = (old_cross_config.cross_server_addr != m_cross_config.cross_server_addr);
		}

		if (NULL != is_open_cross_change) 
		{
			*is_open_cross_change = (old_open_cross != m_is_open_cross);
		}
	}

	return ret;
}


