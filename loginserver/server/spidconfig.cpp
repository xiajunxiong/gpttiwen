#include "spidconfig.h"
#include "engineadapter.h"
#include "config/loginconfig.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "loginservermodule.h"
#include "utility/configpath.h"
#include "server/loginserver.h"

SpidConfig* g_spid_cfg_ptr = NULL;

SpidConfig * SpidConfig::Instance()
{	
	if (NULL == g_spid_cfg_ptr)
	{
		g_spid_cfg_ptr = new SpidConfig();
	}

	return g_spid_cfg_ptr;
}

bool SpidConfig::Init(const std::string &path, std::string *err)
{
	char errinfo[1024] = { 0 };
	int iRet = 0;

	TiXmlDocument document;
	if (path == "" || !document.LoadFile(path))
	{
		*err = path + " Load fail: " + document.ErrorDesc();
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	if (!RootElement)
	{
		*err = path + ": xml node error in root";
		return false;
	}

	{
		TiXmlElement *Element = RootElement->FirstChildElement("agent_adapt");
		if (NULL == Element)
		{
			*err = path + ": xml node error in [config->agent_adapt]";
			return false;
		}

		iRet = this->InitAgentAdapt(Element);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitAgentAdapt failed %d", path.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		TiXmlElement *Element = RootElement->FirstChildElement("company");
		if (NULL == Element)
		{
			*err = path + ": xml node error in [config->company]";
			return false;
		}

		iRet = this->InitAllowedIPList(Element);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitAllowedIPList failed %d", path.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		int is_open = 1; //默认开启创角限制， 除非配置里已经写上了不开启
		EngineAdapter::Instance().ConfigSyncValue(ROOT/LOGIN_MODULE/"CreateRoleControl"/"IsOpen", &is_open, is_open);
		this->SetCreateRoleControlSwitch(is_open);
	}

	{
		int threshold = 0; //默认没有在线数阈值限制
		EngineAdapter::Instance().ConfigSyncValue(ROOT / LOGIN_MODULE / "OnlineRoleNumThreshold", &threshold, threshold);
		this->SetOnlineRoleNumThreshold(threshold);
	}

	{
		int is_open = 0; //默认关闭自动创角，除非配置里已经写上了开启
		EngineAdapter::Instance().ConfigSyncValue(ROOT / LOGIN_MODULE / "AutoCreateRole" / "IsOpen", &is_open, is_open);
		this->SetAutoCreateRoleSwitch(is_open);
	}

	return true;
}

bool SpidConfig::Init_1(const std::string & path, std::string * err)
{
	char errinfo[1024] = { 0 };
	int iRet = 0;

	TiXmlDocument document;
	if (path == "" || !document.LoadFile(path))
	{
		*err = path + " Load fail: " + document.ErrorDesc();
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	if (!RootElement)
	{
		*err = path + ": xml node error in root";
		return false;
	}

	{
		TiXmlElement *Element = RootElement->FirstChildElement("random_scene");
		if (NULL == Element)
		{
			*err = path + ": xml node error in [config->random_scene]";
			return false;
		}

		iRet = this->InitAuditServerBirthSceneCfg(Element);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitAuditServerBirthSceneCfg failed %d", path.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}
	
	{
		TiXmlElement *Element = RootElement->FirstChildElement("shenhefu_cfg");
		if (NULL == Element)
		{
			*err = path + ": xml node error in [config->shenhefu_cfg]";
			return false;
		}

		iRet = this->InitAuditServerCfg(Element);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitAuditServerCfg failed %d", path.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}
	return true;
}

bool SpidConfig::Reload(const std::string& path, std::string* err)
{
	// 搞成static的函数就是防止在类的函数里面delete自己

	SpidConfig *temp = new SpidConfig();

	if (!temp->Init(path, err) || !temp->Init_1(path, err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != g_spid_cfg_ptr) delete g_spid_cfg_ptr;

	g_spid_cfg_ptr = temp;

	return true;
}

bool SpidConfig::CanCreateNewRole(int spid, IP ip /* = 0 */)
{
#ifdef _DEBUG
	return true;
#endif

	if (0 == m_create_role_control_switch)
	{
		return true;
	}

	std::map<int, SpidConfigItem>::iterator it = m_spid_cfg_map.find(spid);
	if (it == m_spid_cfg_map.end())
	{
		return true; //如果没有找到该渠道，默认可以创建新角色
	}

	int limit_days = it->second.new_role_limit_days;
	int limit_role_num = it->second.new_role_limit_role_num;

	time_t cur_time = EngineAdapter::Instance().Time(); //取当前时间做计算
	time_t open_server_timestamp = LOGIN_CONFIG->GetOpenServerConfig()->GetOpenServerZeroTime();
	int cur_real_open_day = GetDayIndex(open_server_timestamp, cur_time) + 1; // 活动开启时的开服天数，1~N

	if (cur_real_open_day <= 0)
	{
		cur_real_open_day = 1;
	}

	bool is_allowed_ip = false;
	if (0 != ip)
	{
		if (m_allow_ip_set.find(ip) != m_allow_ip_set.end())
		{
			is_allowed_ip = true;
		}
	}

	// 必须满足限制天数和限制角色数的条件
	bool is_limit_new_role = ((limit_days > 0 && cur_real_open_day > limit_days) || (limit_role_num > 0 && LoginServer::Instance().GetTotalRoleNum() >= limit_role_num));

	return is_allowed_ip || !is_limit_new_role;
}

int SpidConfig::InitAgentAdapt(TiXmlElement *RootElement)
{
	if (NULL == RootElement)
	{
		return -1;
	}

	TiXmlElement *data_element = RootElement->FirstChildElement("data");

	PlatName plat_name;

	while (NULL != data_element)
	{
		std::string spname;
		if (!GetSubNodeValue(data_element, "spid", spname))
		{
			return -2;
		}

		SNPRINTF(plat_name, sizeof(plat_name), "%s", spname.c_str());
		
		int spid = 0;
		if(!spname.empty())
		{
			PlatTypeUnion plat_type_union;

			plat_type_union.id = 0;

			for (int i = 0; i < 3; i++)
			{
				if ('\0' != plat_name[i])
				{
					plat_type_union.spid[i + 1] = plat_name[i];	// 这样做是为了确保id大于0 字节序的关系
				}
			}

			spid = plat_type_union.id;
		}

		static SpidConfigItem item;

		if (!GetSubNodeValue(data_element, "close_new_role", item.new_role_limit_days))
		{
			item.new_role_limit_days = 0;
		}

		if (!GetSubNodeValue(data_element, "limit_role_num", item.new_role_limit_role_num))
		{
			item.new_role_limit_role_num = 0;
		}

		m_spid_cfg_map[spid] = item;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int SpidConfig::InitAllowedIPList(TiXmlElement *RootElement)
{
	if (NULL == RootElement)
	{
		return -1;
	}

	TiXmlElement *data_element = RootElement->FirstChildElement("data");

	while (NULL != data_element)
	{
		std::string ip_str;
		if (!GetSubNodeValue(data_element, "seq", ip_str))
		{
			return -2;
		}

		IP ip = this->GetIP(ip_str);
		if (0 != ip)
		{
			m_allow_ip_set.insert(ip);
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int SpidConfig::InitAuditServerCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}
	int next_seq = 1;
	while (NULL != data_element)
	{	
		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq != next_seq)
		{
			return -1;
		}
		next_seq += 1;

		AuditServerCfg node_cfg;
		if (!GetSubNodeValue(data_element, "image_id", node_cfg.avatar_type) || node_cfg.avatar_type <= AVATAR_TYPE_INVALID || node_cfg.avatar_type >= AVATAR_TYPE_MAX)
		{
			return -2;
		}

		TiXmlElement * job_list_element = data_element->FirstChildElement("job");
		if (NULL == job_list_element) return -3;

		TiXmlElement * node_element = job_list_element->FirstChildElement("node");
		while (node_element != NULL)
		{
			int job = 0;
			if (!GetNodeValue(node_element, job) || job / PROFESSION_BASE <= 0 || job / PROFESSION_BASE >= PROFESSION_TYPE_NUM)
			{
				return -4;
			}

			node_cfg.job_list.insert(job);
			node_element = node_element->NextSiblingElement();
		}
		if (node_cfg.job_list.empty())
		{
			return -5;
		}

		if (!GetSubNodeValue(data_element, "birth_level", node_cfg.birth_level) || node_cfg.birth_level <= 0 || node_cfg.birth_level > MAX_ROLE_LEVEL)
		{
			return -6;
		}

		if (!GetSubNodeValue(data_element, "scene_group", node_cfg.scene_group_id) || 
			m_audit_server_birth_map.end() == m_audit_server_birth_map.find(node_cfg.scene_group_id))
		{
			return -7;
		}

		m_audit_server_cfg.push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}

	if ((int)m_audit_server_cfg.size() > MAX_INT8)
	{
		return -100;
	}

	return 0;
}

int SpidConfig::InitAuditServerBirthSceneCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int scene_group_id = 0;
		if (!GetSubNodeValue(data_element, "scene_group", scene_group_id) || scene_group_id < 0)
		{
			return -1;
		}

		AuditServerBirthSceneCfg node_cfg;
		if (!GetSubNodeValue(data_element, "scene_id", node_cfg.birth_scene_id) || node_cfg.birth_scene_id <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "pos_x", node_cfg.birth_pos.x) || node_cfg.birth_pos.x < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "pos_y", node_cfg.birth_pos.y) || node_cfg.birth_pos.y < 0)
		{
			return -4;
		}

		m_audit_server_birth_map[scene_group_id].push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}
	if (m_audit_server_birth_map.empty())
	{
		return -20;
	}

	return 0;
}

IP SpidConfig::GetIP(std::string ip_str)
{
	int ip_arr[4] = { 0 };
	std::string origin_ip_str = ip_str;
	ip_str.push_back('.'); //为了方便编程
	
	for (int i = 0; i < 4; ++i)
	{
		size_t pos = ip_str.find_first_of('.');
		if (std::string::npos == pos)
		{
			return 0;
		}
		std::string temp = ip_str.substr(0, pos);
		int temp_int = atoi(temp.c_str());
		if (temp_int < 0 || temp_int >= 256)
		{
			return 0;
		}
		
		ip_str = ip_str.erase(0, pos + 1);
		ip_arr[i] = temp_int;
	}

	char buff[32] = { 0 };
	SNPRINTF(buff, 32, "%d.%d.%d.%d", ip_arr[0], ip_arr[1], ip_arr[2], ip_arr[3]);
	if (0 != origin_ip_str.compare(buff)) //验证
	{
		return 0;
	}

	return (ip_arr[0] << 24) + (ip_arr[1] << 16) + (ip_arr[2] << 8) + ip_arr[3];
}

SpidConfig::SpidConfig() : m_create_role_control_switch(1), 
	m_auto_create_role_switch(0), m_online_role_num_threshold(0)
{

}

SpidConfig::~SpidConfig()
{

}

int SpidConfig::SetCreateRoleControlSwitch(int is_open)
{
	m_create_role_control_switch = is_open;
	return m_create_role_control_switch;
}

int SpidConfig::SetOnlineRoleNumThreshold(int threshold)
{
	m_online_role_num_threshold = threshold;
	return m_online_role_num_threshold;
}

int SpidConfig::SetAutoCreateRoleSwitch(int is_open)
{
	m_auto_create_role_switch = is_open;
	return m_auto_create_role_switch;
}

bool SpidConfig::IsLimitCreateRole(int cur_online_role_num)
{
	int threshold = this->GetOnlineRoleNumThreshold();
	if (threshold <= 0) return false;

	return cur_online_role_num >= static_cast<int>(0.75 * threshold);
}

bool SpidConfig::IsLimitLogin(int cur_online_role_num)
{
	int threshold = this->GetOnlineRoleNumThreshold();
	if (threshold <= 0) return false;

	return cur_online_role_num >= static_cast<int>(0.9 * threshold);
}

int SpidConfig::GetSpidByStrPlatName(const std::string& str_plat_name)
{
	PlatTypeUnion tmp_union;
	tmp_union.spid[0] = tmp_union.spid[1] = tmp_union.spid[2] = tmp_union.spid[3] = 0;
	for (unsigned int i = 0; i < 3; i++)
	{
		if (i < str_plat_name.length() && '\0' != str_plat_name[i])
		{
			tmp_union.spid[i + 1] = str_plat_name[i];	// 这样做是为了确保id大于0 字节序的关系
		}
	}

	return tmp_union.id;
}

int SpidConfig::GetPlatNameBySpid(int spid, char plat_name[4])
{
	PlatTypeUnion tmp_union;
	tmp_union.id = spid;
	for (int i = 0; i < 3; ++i)
	{
		tmp_union.spid[i] = tmp_union.spid[i + 1];
	}
	tmp_union.spid[3] = 0;
	F_STRNCPY(plat_name, tmp_union.spid, 4);
	
	return 0;
}

const AuditServerCfg * SpidConfig::GetAuditServerCfg(int audit_seq, const AuditServerBirthSceneCfg ** birth_cfg)
{
	if(audit_seq <= 0 || audit_seq > (int)m_audit_server_cfg.size()) return NULL;

	std::map<int, std::vector<AuditServerBirthSceneCfg> >::const_iterator it = m_audit_server_birth_map.find(m_audit_server_cfg[audit_seq - 1].scene_group_id);
	if (it != m_audit_server_birth_map.end() && !it->second.empty())
	{
		int birth_index = RandomNum((int)it->second.size());
		if (birth_index >= 0 && birth_index < (int)it->second.size())
		{
			*birth_cfg = &it->second[birth_index];
		}
	}

	return &m_audit_server_cfg[audit_seq - 1];
}

