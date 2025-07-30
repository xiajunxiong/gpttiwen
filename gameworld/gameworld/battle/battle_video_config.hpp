#ifndef __BATTLE_VIDEO_CONFIG_HPP__
#define __BATTLE_VIDEO_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include <map>

struct VideoCfg
{
	VideoCfg() : battle_mode(0), version(0), monster_group_id(0), is_cross(0){}

	short battle_mode;
	short version;
	int monster_group_id;
	int is_cross;
};

class BattleVideoConfig : public ILogicConfig
{
public:
	typedef std::map<int, VideoCfg> VideoCfgMap;

public:
	BattleVideoConfig();
	virtual ~BattleVideoConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	
	const VideoCfg* GetVideoCfg(int monster_group_id);
private:
	int InitVideoMonsterGroupList(TiXmlElement *RootElement);

	VideoCfgMap m_video_cfg_map;
};

#endif