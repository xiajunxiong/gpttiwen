#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "roleinitattr.h"
#include "engineadapter.h"
#include "loginserver/config/loginconfig.h"
#include "loginservermodule.h"
#include "utility/configpath.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "spidconfig.h"

RoleInitAttr::RoleInitAttr()
	: m_scenecount(0)
{
	memset(&m_role_init_attr, 0, sizeof(m_role_init_attr));
	memset(m_newbie_scene, 0, sizeof(m_newbie_scene));
}

RoleInitAttr::~RoleInitAttr()
{

}

RoleInitAttr &RoleInitAttr::Instance()
{
	static RoleInitAttr ria;
	return ria;
}

bool RoleInitAttr::Init(const std::string& path, const std::string& configname, std::string* err)
{
	memset(&m_role_init_attr, 0, sizeof(m_role_init_attr));

	STRNCPY(m_role_init_attr.role_name, "role name", sizeof(m_role_init_attr.role_name));
	
	m_role_init_attr.avatar_type = AVATAR_TYPE_INVALID;
	m_role_init_attr.color = 0;

	TiXmlDocument document;
	if (path == "" || !document.LoadFile(path+configname))
	{
		*err = path + configname + ": RoleInitAttr::Inite Error: " + document.ErrorDesc();
		return false;
	}

	std::string configpath;

	TiXmlElement *RootElement = document.RootElement();
	if (!RootElement)
	{
		*err = configname + ": xml node error in root";
		return false;
	}

	TiXmlElement *cur_element = RootElement->FirstChildElement("scenes");
	if (!cur_element)
	{
		*err = configname + ": xml node error in [config -> scenes]";
		return false;
	}

	cur_element = cur_element->FirstChildElement("scene");
	if (!cur_element)
	{
		*err = configname + ": xml node error in [config->scenes->scene]";
		return false;
	}

	int scenenum = 0;
	while (cur_element)
	{
		if (scenenum >= NEWBIE_SCENE_NUM)
		{
			*err = configname + ": too much nodes in [config->scenes->scene]";
			return false;
		}

		if (!GetSubNodeValue(cur_element, "scene_id", m_newbie_scene[scenenum].scene_id) || m_newbie_scene[scenenum].scene_id <= 0)
		{
			*err = configname + ": xml node error in [config->scenes->scene->scene_id]";
			return false;
		}

		if (!GetSubNodeValue(cur_element, "scene_x", m_newbie_scene[scenenum].scene_x) || m_newbie_scene[scenenum].scene_x < 0)
		{
			*err = configname + ": xml node error in [config->scenes->scene->scene_x]";
			return false;
		}

		if (!GetSubNodeValue(cur_element, "scene_y", m_newbie_scene[scenenum].scene_y) || m_newbie_scene[scenenum].scene_y < 0)
		{
			*err = configname + ": xml node error in [config->scenes->scene->scene_y]";
			return false;
		}

		if (!GetSubNodeValue(cur_element, "range", m_newbie_scene[scenenum].range) || m_newbie_scene[scenenum].range < 0)
		{
			*err = configname + ": xml node error in [config->scenes->scene->range]";
			return false;
		}

		if (!GetSubNodeValue(cur_element, "last_scene_id", m_newbie_scene[scenenum].last_scene_id) || m_newbie_scene[scenenum].last_scene_id <= 0)
		{
			*err = configname + ": xml node error in [config->scenes->scene->last_scene_id]";
			return false;
		}

		if (!GetSubNodeValue(cur_element, "last_scene_x", m_newbie_scene[scenenum].last_scene_x) || m_newbie_scene[scenenum].last_scene_x <= 0)
		{
			*err = configname + ": xml node error in [config->scenes->scene->last_scene_x]";
			return false;
		}

		if (!GetSubNodeValue(cur_element, "last_scene_y", m_newbie_scene[scenenum].last_scene_y) || m_newbie_scene[scenenum].last_scene_y <= 0)
		{
			*err = configname + ": xml node error in [config->scenes->scene->last_scene_y]";
			return false;
		}

		{
			int is_open = 0;
			EngineAdapter::Instance().ConfigSyncValue(ROOT/LOGIN_MODULE/"NewbiePlot"/"IsOpen", &is_open, is_open);
			m_newbie_scene[scenenum].is_newbie_plot_open = is_open;
		}

		{
			int is_open = 0;
			EngineAdapter::Instance().ConfigSyncValue(ROOT / LOGIN_MODULE / "AutoGetShizhuang" / "IsOpen", &is_open, is_open);
			m_newbie_scene[scenenum].is_auto_get_shizhuang = is_open;
		}

		++scenenum;
		cur_element = cur_element->NextSiblingElement();
	}

	m_scenecount = scenenum;

	if (m_scenecount <= 0)
	{
		*err = configname + ": xml node error in [config->scenecount <= 0]";
		return false;
	}

	m_role_init_attr.scene_id = m_newbie_scene[0].scene_id;
	m_role_init_attr.scene_x =  m_newbie_scene[0].scene_x;
	m_role_init_attr.scene_y =  m_newbie_scene[0].scene_y;
	
	m_role_init_attr.last_scene_id = m_newbie_scene[0].last_scene_id;
	m_role_init_attr.last_scene_x =  m_newbie_scene[0].last_scene_x;
	m_role_init_attr.last_scene_y =  m_newbie_scene[0].last_scene_y;

	if (!GetSubNodeValue(RootElement, "coin_bind", m_role_init_attr.coin_bind))
	{
		*err = configname + ": xml node error in coin_bind";
		return false;
	}
	if (!GetSubNodeValue(RootElement, "immortal_coin", m_role_init_attr.immortal_coin))
	{
		*err = configname + ": xml node error in immortal_coin";
		return false;
	}
	if (!GetSubNodeValue(RootElement, "gold", m_role_init_attr.gold))
	{
		*err = configname + ": xml node error in gold";
		return false;
	}

	m_role_init_attr.level = 1;


	return true;
}

const RoleInitParam & RoleInitAttr::GetInitParam(char avatar_type, char color, int plat_spid)
{
	if (m_scenecount <= 0) return m_role_init_attr;

	int r = rand() % m_scenecount;

	static const int SIGN_ARRAY[2] = { -1, 1 };

	m_role_init_attr.last_scene_id = m_newbie_scene[r].last_scene_id;
	m_role_init_attr.last_scene_x = m_newbie_scene[r].last_scene_x + SIGN_ARRAY[rand() % 2] * (m_newbie_scene[r].range == 0 ? 0 : (rand() % m_newbie_scene[r].range));
	m_role_init_attr.last_scene_y = m_newbie_scene[r].last_scene_y + SIGN_ARRAY[rand() % 2] * (m_newbie_scene[r].range == 0 ? 0 : (rand() % m_newbie_scene[r].range));

	m_role_init_attr.scene_id = m_role_init_attr.last_scene_id;
	m_role_init_attr.scene_x = m_role_init_attr.last_scene_x;
	m_role_init_attr.scene_y = m_role_init_attr.last_scene_y;
	
	m_role_init_attr.create_time = static_cast<long long>(EngineAdapter::Instance().Time());
	m_role_init_attr.profession = PROF_TYPE_INVALID;

	m_role_init_attr.last_online_dayid = EngineAdapter::Instance().DayID();
	m_role_init_attr.top_level = 0;
	m_role_init_attr.next_top_level_refresh_timestamp = 0;
	m_role_init_attr.role_fly_flag = 0;

	return m_role_init_attr;
}

bool RoleInitAttr::SetNewbiePlotOpenClose(int is_open)
{
	if (m_newbie_scene[0].is_newbie_plot_open != is_open)
	{
		m_newbie_scene[0].is_newbie_plot_open = is_open;
		return true;
	}
	return false;
}

bool RoleInitAttr::SetNewbieAutoGetShizhuang(int flag)
{
	if (m_newbie_scene[0].is_auto_get_shizhuang != flag)
	{
		m_newbie_scene[0].is_auto_get_shizhuang = flag;
		return true;
	}
	return false;
}

