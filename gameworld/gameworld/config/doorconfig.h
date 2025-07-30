#ifndef __DOORCONFIG_H__
#define __DOORCONFIG_H__

#include <vector>
#include <string>

#include "gamedef.h"

struct ConfigSceneTargetScene
{
	ConfigSceneTargetScene() : sceneid(0), x(0), y(0) {}

	int sceneid;
	int x;
	int y;
};

struct ConfigSceneDoor
{
	ConfigSceneDoor() : door_id(0), x(0), y(0), level(0), type(0){}

	int door_id;
	int x;
	int y;
	int level;
	int type;
	ConfigSceneTargetScene targetscene;
	bool Load(const std::string &configname, std::string &err);
};
typedef std::vector<ConfigSceneDoor> ConfigSceneDoorList;

#endif


