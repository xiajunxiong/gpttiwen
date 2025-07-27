#include "localconfig.hpp"

#include "servercommon/configcommon.h"
#include "servercommon/serverconfig/serverconfigpool.h"

LocalConfig::LocalConfig()
	: m_plat_type(0), m_is_check_server_id(true), m_is_ios_platform(false), m_is_taiwan_platform(false), m_mother_server_id(0)
{
	memset(m_plat_name, 0, sizeof(m_plat_name));
}

LocalConfig::~LocalConfig()
{
}

LocalConfig & LocalConfig::Instance()
{
	static LocalConfig clc;
	return clc;
}

void LocalConfig::GetPlatName(int plat_type, PlatName plat_name)
{
	if (plat_type >= CommonConfig::PLAT_TYPE_YOUKE && plat_type < CommonConfig::PLAT_TYPE_COUNT)
	{
		UNSTD_STATIC_CHECK(CommonConfig::PLAT_TYPE_COUNT == 7);
		const static PlatName plat_list[CommonConfig::PLAT_TYPE_COUNT] = 
		{
			"youke",
			"360",
			"37w",
			"yyg",
			"gts",
			"yyb",
			"mix",
		};

		F_STRNCPY(plat_name, plat_list[plat_type], sizeof(PlatName));
	}
	else
	{
		PlatTypeUnion plat_type_union;

		plat_type_union.id = plat_type;

		SNPRINTF(plat_name, sizeof(PlatName), "%c%c%c", plat_type_union.spid[1], plat_type_union.spid[2], plat_type_union.spid[3]);
	}
}

bool LocalConfig::Init(const std::string &path, std::string *err)
{
	TiXmlDocument document;

	if (path == "" || !document.LoadFile(path))
	{
		*err = path + "：Load CrossLocal Config Error,\n" + document.ErrorDesc();
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	if (NULL == RootElement)
	{
		*err = path + ": xml root node error.";
		return false;
	}

	std::string plat_name;
	if (!GetSubNodeValue(RootElement, "plat_name", plat_name) && plat_name.length() > 0 && plat_name.length() < sizeof(m_plat_name))
	{
		*err = path + ": xml node error in [config->plat_name]";
		return false;
	}

	const static int MAX_ZHUANFU_TYPE_LIMIT = 10; // 限制一下专服类型的大小
	if (!GetSubNodeValue(RootElement, "rand_act_zhuanfu_type", m_rand_act_zhuanfu_type) || m_rand_act_zhuanfu_type <= 0 || m_rand_act_zhuanfu_type > MAX_ZHUANFU_TYPE_LIMIT)
	{
		*err = path + ": xml node error in [config->rand_act_zhuanfu_type]";
		return false;
	}

	SNPRINTF(m_plat_name, sizeof(m_plat_name), "%s", plat_name.c_str());

	if ('i' == m_plat_name[0])
	{
		m_is_ios_platform = true;
	}

	if (0 == plat_name.compare("ieu") || 0 == plat_name.compare("aeu"))
	{
		m_is_taiwan_platform = true;
	}

	if (!plat_name.empty())
	{
		PlatTypeUnion plat_type_union;

		plat_type_union.id = 0;

		for (int i = 0; i < 3; i++)
		{
			if ('\0' != m_plat_name[i])
			{
				plat_type_union.spid[i + 1] = m_plat_name[i];	// 这样做是为了确保id大于0 字节序的关系
			}
		}

		m_plat_type = plat_type_union.id;
	}
	else
	{
		m_plat_type = CommonConfig::PLAT_TYPE_YOUKE;
	}

	if (!GetSubNodeValue(RootElement, "is_check_server_id", m_is_check_server_id))
	{
		*err = path + ": xml node error in [config->is_check_server_id]";
		return false;
	}

	{
		TiXmlElement *ServerListElement = RootElement->FirstChildElement("allow_server_id_list");
		if (NULL == ServerListElement)
		{
			*err = path + ": xml node error in [config->allow_server_id_list]";
			return false;
		}

		int count = 0;
		TiXmlElement *ServerIdElement = ServerListElement->FirstChildElement("server_id");
		while (NULL != ServerIdElement)
		{
			if (count >= MAX_ALLOW_SERVER_ID_COUNT)
			{
				*err = path + ": xml node error in [config->allow_server_id_list->server_id], too many";
				return false;
			}

			int server_id = 0;
			if (!GetNodeValue(ServerIdElement, server_id) || server_id < 0 || server_id >= MAX_DB_IDNEX)
			{
				*err = path + ": xml node error in [config->allow_server_id_list->server_id]";
				return false;
			}

			if (m_allow_server_id_set.find(server_id) != m_allow_server_id_set.end())
			{
				*err = path + ": xml node error in [config->allow_server_id_list->server_id], repeat";
				return false;
			}

			if (0 == m_mother_server_id)
			{
				m_mother_server_id = server_id;
			}

			m_allow_server_id_set.insert(server_id);

			++ count;
			ServerIdElement = ServerIdElement->NextSiblingElement("server_id");
		}

		if (m_is_check_server_id && count <= 0)
		{
			*err = path + ": xml node error in [config->allow_server_id_list->server_id], must one server_id at least";
			return false;
		}

		m_unique_sid = UniqueServerID(m_plat_type, m_mother_server_id);
	}

	return true;
}

void LocalConfig::GetPlatName(PlatName plat_name)
{
	F_STRNCPY(plat_name, m_plat_name, sizeof(PlatName));
}

bool LocalConfig::IsServerIdAllowed(int server_id)
{
	return (!m_is_check_server_id || (m_allow_server_id_set.find(server_id) != m_allow_server_id_set.end()));
}

bool LocalConfig::IsMergedServer()
{
	//return m_allow_server_id_set.size() > 1;
	return false; // TODO 等客户端做好多角色界面才去改回来
}

bool LocalConfig::IsAllowedServerId(int server_id)
{
	return (m_allow_server_id_set.find(server_id) != m_allow_server_id_set.end());
}

int LocalConfig::GetMergeServerId()
{
	if (m_allow_server_id_set.empty())
	{
		return 0;
	}

	return m_mother_server_id;

	//std::set<int>::iterator it = m_allow_server_id_set.begin();

	//return (*it);
}

bool LocalConfig::IsBelongServer(int role_id)
{
	const std::set<int>& allow_servers = this->GetAllowServerIdSet();
	return allow_servers.find(IntToUid(role_id).db_index) != allow_servers.end();
}

const char* LocalConfig::GetPlatNameBySpid(int spid)
{
	if (spid <= 0)
	{
		return "nil";
	}

	static PlatTypeUnion plat_type_union;
	plat_type_union.id = spid;

	for (int i = 0; i < 3; ++i)
	{
		plat_type_union.spid[i] = plat_type_union.spid[i + 1];
	}
	plat_type_union.spid[3] = 0;
	return plat_type_union.spid;
}

const UniqueServerID& LocalConfig::GetUniqueServerID() const
{
	return m_unique_sid;
}

