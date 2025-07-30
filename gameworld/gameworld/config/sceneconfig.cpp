#include "sceneconfig.h"
#include "servercommon/configcommon.h"
#include "servercommon/servercommon.h"
#include "doorconfig.h"
#include "checkresourcecenter.hpp"
#include "gamelog.h"

bool ConfigScene::Load(const std::string &dir, const std::string &filename)
{
	std::string configname = dir + filename;
	TiXmlDocument m_document;
	char errinfo[1024];

	if (configname == "" || !m_document.LoadFile(configname))
	{
		lasterr = "Load SceneConfig Error. ";
		lasterr += filename;
		lasterr += " reason:";
		lasterr += m_document.ErrorDesc();
		return false;
	}

	TiXmlElement *RootElement = m_document.RootElement();
	TiXmlElement *currentElement = 0;

	if (!RootElement)
	{
		lasterr = configname + ": xml node error in root";
		return false;
	}

	//开始加载
	if (!GetSubNodeValue(RootElement, "id", id))
	{
		lasterr = configname + ": xml node error in node [id] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "name", name))
	{
		lasterr = configname + ": xml node error in node [name] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "mapid", mapid))
	{
		lasterr = configname + ": xml node error in node [mapid] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "speciallogic_type", speciallogic_type) || speciallogic_type < 0)
	{
		lasterr = configname + ": xml node error in node [speciallogic_type] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "scene_timeout", scene_timeout))
	{
		lasterr = configname + ": xml node error in node [scene_timeout] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "is_timeout_kick", is_timeout_kick_user))
	{
		lasterr = configname + ": xml node error in node [is_timeout_kick] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "scene_level_min", scene_level_min))
	{
		lasterr = configname + ": xml node error in node [m_scene_level_min] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "scene_level_max", scene_level_max))
	{
		lasterr = configname + ": xml node error in node [m_scene_level_max] ";
		return false;
	}

	if ((0 != scene_level_max) && scene_level_max < scene_level_min)
	{
		lasterr = configname + ": xml node error in node [scene_level_max < scene_level_min] ";
		return false;
	}

	if (0 != scene_level_min || 0 != scene_level_max)
	{
		gamelog::g_log_world.printf(LL_INFO, "World On INIT, Load Scene scene_level_limit[min:%d, max:%d]:\n%s", scene_level_min, scene_level_max, __FUNCTION__);
	}

	//处理doors，可为空
	currentElement = RootElement->FirstChildElement("doors");
	if (currentElement)
	{
		TiXmlElement* subElement = currentElement->FirstChildElement();
		while (subElement)
		{
			ConfigSceneDoor door;

			if (!GetSubNodeValue(subElement, "id", door.door_id))
			{
				lasterr = configname + ": xml node error in node [doors->door->id] ";
				return false;
			}

			if (!GetSubNodeValue(subElement, "x", door.x) || 0 > door.x)
			{
				lasterr = configname + ": xml node error in node [doors->door->x] ";
				return false;
			}

			if (!GetSubNodeValue(subElement, "y", door.y) || 0 > door.y)
			{
				lasterr = configname + ": xml node error in node [doors->door->y] ";
				return false;
			}

			// 检查传送门是否重复
			ConfigSceneDoorList::iterator tmpiter = doors.begin();
			for (; tmpiter != doors.end(); ++tmpiter)
			{
				if (tmpiter->door_id == door.door_id) break;
			}

			if (tmpiter != doors.end())
			{
				sprintf(errinfo, "%s:\nxml node error in node [doors->door->id]: the door id[%d] is repeat!", configname.c_str(), door.door_id);
				lasterr = errinfo;
				return false;
			}

			// 加载door
			std::string doorcofnigname;
			BaseTypeToString(door.door_id, doorcofnigname);
			std::string doorconfig = dir + "door/" + doorcofnigname + ".xml";
			if (!door.Load(doorconfig, lasterr))
			{
				lasterr = configname + ":" + lasterr;
				return false;
			}

			doors.push_back(door);
			subElement = subElement->NextSiblingElement();
		}
	}

	//处理relivepoint
	currentElement = RootElement->FirstChildElement("townpoint");
	if (!currentElement)
	{
		lasterr = configname + ": xml node error in node [townpoint] ";
		return false;
	}

	if (!GetSubNodeValue(currentElement, "sceneid", townpoint.sceneid))
	{
		lasterr = configname + ": xml node error in node [townpoint->sceneid] ";
		return false;
	}
	CheckResourceCenter::Instance().GetSceneCheck()->Add(townpoint.sceneid, __FUNCTION__);

	if (!GetSubNodeValue(currentElement, "scenex", townpoint.scenex))
	{
		lasterr = configname + ": xml node error in node [townpoint->scenex] ";
		return false;
	}
	if (!GetSubNodeValue(currentElement, "sceney", townpoint.sceney))
	{
		lasterr = configname + ": xml node error in node [townpoint->sceney] ";
		return false;
	}

	return true;
}




