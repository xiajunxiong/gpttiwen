#include "scenemanagerconfig.h"
#include "servercommon/configcommon.h"
#include "gamelog.h"
#include "scene/map.h"
#include "scene/scene.h"

bool SceneManagerConfig::CheckMap(int mapid, int x, int y) const
{
	if ( 0> x || 0 > y)
	{
		return false;
	}

	for (ConfigMapList::const_iterator map_iter = configmanager.maps.begin();
		map_iter != configmanager.maps.end(); ++map_iter)
	{
		if (map_iter->map.id == mapid)
		{
			if (map_iter->map.width >= x
				|| map_iter->map.height >= y)
			{
				return true;
			}
		}
	}
	return false;
}

bool SceneManagerConfig::Load(const std::string &dir,const std::string &configname, int total_server, int server_index)
{
	if (total_server <= 0 || server_index < 0) return false;

	char errinfo[1024] = {0};

	TiXmlDocument document;
	
	if (configname == "" || !document.LoadFile(dir+configname))
	{
		lasterr = "Load SceneManagerConfig Error.";
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	TiXmlElement *currentElement = 0;

	if (!RootElement)
	{
		lasterr = configname + ": xml node error in root";
		return false;
	}

	//开始加载

	//处理maps
	currentElement = RootElement->FirstChildElement("maps");
	if (!currentElement)
	{
		lasterr = configname + ": xml node error in node [maps]. ";
		return false;
	}
	else
	{
		TiXmlElement *subElement = currentElement->FirstChildElement();
		while (subElement)
		{
			ConfigMapItem item;

			if (!GetSubNodeValue(subElement, "path", item.path))
			{
				lasterr = configname + ": xml node error in node [scenemanager->maps->path] ";
				return false;
			}

			if (!item.map.Load(dir + item.path))
			{
				lasterr = dir + item.path + ":" + item.map.GetError();
				return false;
			}
	
			if (item.map.id >= 10000)
			{
				lasterr = dir + item.path + " id >= 10000";
				return false;
			}

			for(ConfigMapList::const_iterator mapiter = configmanager.maps.begin();
				mapiter != configmanager.maps.end(); ++mapiter)
			{
				if (item.map.id == mapiter->map.id)
				{
					sprintf(errinfo,"%s: %s map id[%d] repeat.",configname.c_str(), item.path.c_str(), item.map.id);
					lasterr = errinfo;
					return false;
				}
			}

			gamelog::g_log_world.printf(LL_INFO, "Load Map[%d] Config Success.", item.map.id);
			configmanager.maps.push_back(item);

			subElement = subElement->NextSiblingElement();
		}
	}

	//处理scenes
	currentElement = RootElement->FirstChildElement("scenes");
	if (!currentElement)
	{
		lasterr = configname + ": xml node error in node [scenes]. ";
		return false;
	}
	else
	{
		TiXmlElement *subElement = currentElement->FirstChildElement();
		while (subElement)
		{
			ConfigSceneItem item;

			if ( !GetSubNodeValue(subElement, "path", item.path) )
			{
				lasterr = configname + ": xml node error in node [scenemanager->scenes->path] ";
				return false;
			}
			if ( !GetSubNodeValue(subElement, "scene_type", item.scene_type) )
			{
				lasterr = configname + ": xml node error in node [scenemanager->scenes->scene_type] ";
				return false;
			}

			if (item.scene_type < 0 || item.scene_type >= Scene::SCENE_TYPE_COUNT)
			{
				lasterr = configname + ": xml node error in node [scenemanager->scenes->scene_type], scene_type unknow.";
				printf("ERROR load scene type[%d], remember to change config and return false\n", item.scene_type);
				return false;
			}

			if ( !item.scene.Load(dir,item.path) )
			{
				lasterr = item.scene.GetError();
				return false;
			}

			if (!this->IsMatch(item.scene_type, item.scene.id))
			{
				char err_str[256] = {0};
				sprintf(err_str, " scene_type:%d not match scene_id:%d", item.scene_type, item.scene.id);

				lasterr = configname + err_str;
				return false;
			}

			for(ConfigSceneList::const_iterator scene_iter = configmanager.scenes.begin();
				scene_iter != configmanager.scenes.end(); ++scene_iter)
			{
				if (scene_iter->scene.id == item.scene.id)
				{
					sprintf(errinfo,"%s: %s scene id[%d] repeat.",configname.c_str(), item.path.c_str(), item.scene.id);
					lasterr = errinfo;
					return false;
				}
			}

			gamelog::g_log_world.printf(LL_INFO, "Load Scene[%d] Config Success.", item.scene.id);
			
			configmanager.scenes.push_back(item);
			subElement = subElement->NextSiblingElement();
		}
	}

	// 检查场景和地图配置
	for(ConfigSceneList::const_iterator iter = configmanager.scenes.begin();
		iter != configmanager.scenes.end(); ++iter)
	{
		/*TODO
		// 检查npc相关
		for(ConfigSceneNpcList::const_iterator npc_iter = iter->scene.npcs.begin();
			npc_iter != iter->scene.npcs.end(); ++npc_iter)
		{
			if (!CheckMap(iter->scene.mapid,npc_iter->x,npc_iter->y))
			{
				sprintf(errinfo, "%s Scene[%d] : the [x:%d y:%d] of npc is error where npc id is %d.", configname.c_str(), iter->scene.id, npc_iter->x, npc_iter->y, npc_iter->id);
				lasterr = errinfo;
				return false;
			}
		}

		// 检查触发区域坐标
		for(ConfigSceneTriggerAreaList::const_iterator area_iter = iter->scene.triggerareas.begin();
			area_iter != iter->scene.triggerareas.end(); ++area_iter)
		{
			if (!CheckMap(iter->scene.mapid,area_iter->x,area_iter->y))
			{
				sprintf(errinfo, "%s Scene[%d] : the [x:%d y:%d] of trigger area is error.", configname.c_str(), iter->scene.id, area_iter->x, area_iter->y);
				lasterr = errinfo;
				return false;
			}
			if (!CheckMap(iter->scene.mapid,area_iter->x+area_iter->range,area_iter->y+area_iter->range))
			{
				sprintf(errinfo, "%s Scene[%d] : the [range:%d] of trigger area is error.", configname.c_str(), iter->scene.id, area_iter->range);
				lasterr = errinfo;
				return false;
			}
		}

		// 检查怪物生长点坐标
		for (ConfigSceneMonsterPointList::const_iterator monsterpoint_iter = iter->scene.monsterpoints.begin();
			monsterpoint_iter != iter->scene.monsterpoints.end(); ++monsterpoint_iter)
		{
			if (!CheckMap(iter->scene.mapid, monsterpoint_iter->x, monsterpoint_iter->y))
			{
				sprintf(errinfo, "%s Scene[%d] : the [x:%d y:%d] of monster generater point is error.", configname.c_str(), iter->scene.id, 
					monsterpoint_iter->x, monsterpoint_iter->y);
				lasterr = errinfo;
				return false;
			}
		}

		// 检查采集点坐标
		for (ConfigSceneGatherList::const_iterator gather_iter = iter->scene.gathers.begin();
			gather_iter != iter->scene.gathers.end(); ++gather_iter)
		{
			if (!CheckMap(iter->scene.mapid, gather_iter->x, gather_iter->y))
			{
				sprintf(errinfo, "%s Scene[%d] : the [x:%d y:%d] id[%d] of gather generater point is error.", configname.c_str(), iter->scene.id, 
					gather_iter->x, gather_iter->y, gather_iter->id);
				lasterr = errinfo;
				return false;
			}
		}

		// 检查触发器相关
		for(ConfigSceneTriggerList::const_iterator trigger_iter = iter->scene.triggers.begin();
			trigger_iter != iter->scene.triggers.end(); ++ trigger_iter)
		{
			std::string check_trigger_err;
			if (!Trigger::CheckTriggerConfig(*trigger_iter, this, iter->scene.mapid, &check_trigger_err))
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s Scene[%d] : %s", configname.c_str(), iter->scene.id, check_trigger_err.c_str());
				lasterr = errinfo;
				return false;
			}
		}
		*/
	}
	

	return true;
}

bool SceneManagerConfig::IsMatch(int scene_type, int scene_id) const
{

	return true;
}
