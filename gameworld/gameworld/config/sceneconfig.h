#ifndef __SCENECONFIG_H__
#define __SCENECONFIG_H__

#include <vector>
#include <string>
#include "servercommon/basedef.h"
#include "doorconfig.h"

enum MAPTYPE
{
	COMMON = 0,	// һ���ͼ
	FB = 1,		// ����
};

static const int MINE_MONSTERS_NUM = 10;
static const int MINE_MONSTERS_DROP_NUM = 10;

struct ConfigSceneTownPoint
{
	ConfigSceneTownPoint() :sceneid(0), scenex(0), sceney(0) {}
	int sceneid;
	int scenex;
	int sceney;
};

struct ConfigScene
{
	ConfigScene():id(0), mapid(0), speciallogic_type(0), scene_timeout(0), is_timeout_kick_user(false)
	{

	}
	
	int id;
	std::string name;
	int mapid;
	int speciallogic_type;
	unsigned int scene_timeout;
	bool is_timeout_kick_user;

	int scene_level_min;					//!< ���볡���ȼ�����
	int scene_level_max;					//!< ���볡���ȼ�����

	//ConfigSceneNpcList npcs;
	ConfigSceneDoorList doors;
	ConfigSceneTownPoint townpoint;

	std::string lasterr;
	bool Load(const std::string &dir,const std::string &filename);
	const std::string& GetError(){ return lasterr; }
};

#endif


