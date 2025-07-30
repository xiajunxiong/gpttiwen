
#ifndef SCENEMANAGERCONFIG_H
#define SCENEMANAGERCONFIG_H

#include <vector>
#include <string>

#include "sceneconfig.h"
#include "mapconfig.h"

struct ConfigMapItem
{
	std::string path;
	ConfigMap map;
};
typedef std::vector<ConfigMapItem> ConfigMapList;


struct ConfigSceneItem
{
	ConfigSceneItem():scene_type(0){}
	std::string path;
	int scene_type;
	ConfigScene scene;
};

typedef std::vector<ConfigSceneItem> ConfigSceneList;

struct ConfigSceneManager
{
	ConfigMapList maps;
	ConfigSceneList scenes;
};

class SceneManagerConfig
{
public:
	bool Load(const std::string &dir, const std::string& configname, int total_server, int server_index);
	std::string GetError(){ return lasterr; }

	bool CheckMap(int mapid, int x, int y) const;

	ConfigSceneManager configmanager;
	std::string lasterr;

private:
	bool IsMatch(int scene_type, int scene_id) const;
};

#endif


