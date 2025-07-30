#include <map>

#include "globalconfig.h"
#include "servercommon/configcommon.h"
#include "engineadapter.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GlobalConfig *gs_global_config = NULL;

GlobalConfig * GlobalConfig::Instance()
{
	if (NULL == gs_global_config) 
	{
		gs_global_config = new GlobalConfig();
	}

	return gs_global_config;
}

bool GlobalConfig::Reload(const std::string& path, const std::string& configname, std::string* err)
{
	// 搞成static的函数就是防止在类的函数里面delete自己

	GlobalConfig *temp = new GlobalConfig();
	
	if (!temp->Init(path, configname, err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != gs_global_config) delete gs_global_config;

	gs_global_config = temp;

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool GlobalConfig::Init(const std::string &path, const std::string &configname, std::string *err)
{
	TiXmlDocument document;

	if (path == "" || !document.LoadFile(path+configname))
	{
		*err = "Load globalconfig manager Error.";
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	std::string configpath;

	if (!RootElement)
	{
		*err = configname + ": xml node error in root";
		return false;
	}

	{
		// 处理keyconfig
		if (!GetSubNodeValue(RootElement, "key", configpath))
		{
			*err = configname + ": xml node error in key";
			return false;
		}
		if (!m_key.Init(path+configpath, err))
		{
			return false;
		}
	}

	{
		// 处理otherconfig
 		if (!GetSubNodeValue(RootElement, "other", configpath))
		{
			*err = configname + ": xml node error in other";
			return false;
		}
		if (!m_other.Init(path + configpath, err))
		{
			return false;
		}
	}

	{
		// 活动配置
		//m_dynamicconfig

		std::string dynamic_cfg_path;
		int len = static_cast<int>(strlen(path.c_str()) - strlen("gameworld/"));

		if (len > 0)
		{
			std::string real_path = path;
			real_path.resize(len);
			dynamic_cfg_path = real_path + "dynamic_cfg.xml";
		}
		else
		{
			dynamic_cfg_path = "../config/dynamic_cfg.xml";
		}

		if (!m_dynamicconfig.Init(dynamic_cfg_path, err))
		{
			return false;
		}
	}

	{
		//渠道配置

		std::string spid_cfg_path;
		spid_cfg_path = path + "logicconfig/agent_adapt.xml";
		if (!m_spidconfig.Init(spid_cfg_path, err))
		{
			return false;
		}
	}

	return true;
}

const unsigned int GlobalConfig::GetKeyConfig_ZoneMatrixX() const
{
	return m_key.GetZoneMatrixX();
}

const unsigned int GlobalConfig::GetKeyConfig_ZoneMatrixY() const
{
	return m_key.GetZoneMatrixY();
}

const unsigned int GlobalConfig::GetKeyConfig_RoleAoiX() const
{
	return m_key.GetRoleAoiX();
}

const unsigned int GlobalConfig::GetKeyConfig_RoleAoiY() const
{
	return m_key.GetRoleAoiY();
}

const unsigned int GlobalConfig::GetKeyConfig_RoleAoiLeaveMoreThenEnter() const
{
	return m_key.GetRoleAoiLeaveMoreThenEnter();
}

const unsigned int GlobalConfig::GetKeyConfig_RoleAoiSensitiveDistance() const
{
	return m_key.GetRoleAoiSensitiveDistance();
}

const unsigned int GlobalConfig::GetKeyConfig_PetAoiSensitiveDistance() const
{
	return m_key.GetPetAoiSensitiveDistance();
}

const unsigned int GlobalConfig::GetKeyConfig_RoleSaveTime() const
{
	return m_key.GetRoleSaveTime();
}

const unsigned int GlobalConfig::GetOtherConfig_SortInOrder_Interval() const
{
	return m_other.bag_sortinorder_interval;
}

const bool GlobalConfig::GetOtherConfig_IsCheckValid() const
{
	return m_other.is_check_valid;
}

const unsigned int GlobalConfig::GetOtherConfig_MaxMoveSpeed() const
{
	return m_other.max_move_speed;
}

const unsigned int GlobalConfig::GetOtherConfig_MoveCheck_Length() const
{
	return m_other.move_check_length;
}

const unsigned int GlobalConfig::GetOtherConfig_MaxTransportLength() const
{
	return m_other.max_transport_length;
}

bool GlobalConfig::IsGmOpen() const
{
	return 0 != m_other.gm_switch;
}

bool GlobalConfig::IsOpenMsgCache() const
{
	return 0 != m_other.is_open_msg_cache;
}

bool GlobalConfig::IsOpenBigCross() const
{
	return m_other.is_open_bigcross;
}

int GlobalConfig::GetCenterCityBornPos(Posi &realive_pos)
{
	realive_pos.x = m_other.center_city_born_pos_x;
	realive_pos.y = m_other.center_city_born_pos_y;

	return m_other.center_city_scene_id;
}
