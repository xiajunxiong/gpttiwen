#ifndef __RAND_ACTIVITY_ANIMAL_RACE_CONFIG_HPP__
#define __RAND_ACTIVITY_ANIMAL_RACE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raanimalraceparam.hpp"
#include "item/itembase.h"

struct RAAnimalRaceCfg
{
	RAAnimalRaceCfg() :seq(0), item_consume_id(0)
	{}

	int seq;
	int item_consume_id;				//所需消耗的道具ID
};

struct RAOtherCfg
{
	RAOtherCfg() :bet_upper_limit(0), scene_id(0)
	{}

	int bet_upper_limit;				//神兽下注上限
	int scene_id;			//活动所在的scene_id
};

struct RATimeCfg
{
	RATimeCfg() : start_time1(0), end_time1(0), start_time2(0), end_time2(0)
	{}

	int start_time1;
	int end_time1;
	int start_time2;
	int end_time2;
};

struct RARolePlaceCfg
{
	RARolePlaceCfg() :role_dir(0)
	{
		role_pos = Posi(0, 0);
	}

	Posi role_pos;
	Dir role_dir;
};

class RandActivityManager;

class RandActivityAnimalRaceConfig : public RandActivityConfig
{
public:
	RandActivityAnimalRaceConfig();
	virtual ~RandActivityAnimalRaceConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAOtherCfg & GetOtherCfg() const { return m_other_cfg; }
	const RAAnimalRaceCfg & GetAnimalRaceCfg() const { return m_animal_race_cfg; }
	const RATimeCfg & GetTimeCfg() const { return m_time_cfg; }
	const RARolePlaceCfg * GetRolePlaceCfg(int index) const ;

private:
	int InitAnimalRaceCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitTimeCfg(TiXmlElement *RootElement);
	int InitRolePlaceCfg(TiXmlElement *RootElement);


	RAOtherCfg m_other_cfg;
	RAAnimalRaceCfg m_animal_race_cfg;
	RATimeCfg m_time_cfg;
	RARolePlaceCfg m_role_place_cfg[MAX_ANIMAL_RACE_SC_NUM];
};

#endif	//__RAND_ACTIVITY_ANIMAL_RACE_CONFIG_HPP__