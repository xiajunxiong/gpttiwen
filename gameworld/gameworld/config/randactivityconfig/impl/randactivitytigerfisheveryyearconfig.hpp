#ifndef __RAND_ACTIVITY_TIGER_FISH_EVERY_YEAR_CONFIG_HPP__
#define __RAND_ACTIVITY_TIGER_FISH_EVERY_YEAR_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itempool.h"
#include "servercommon/activitydef.hpp"

// ª˘¥°≈‰÷√
struct RATigerFEYBasisCfg
{
	RATigerFEYBasisCfg():seq(0),task_type(0),task_param(0), reward_fish_num(0)
	{}

	int seq;
	int task_type;
	int task_param;
	int reward_fish_num;
	std::vector<ItemConfigData> reward_item;
};

struct RATigerFEYBasisLevelCfg
{
	RATigerFEYBasisLevelCfg():level(0)
	{}

	int level;
	std::vector<RATigerFEYBasisCfg> basis_cfg;
};

struct RATigerFEYBasisSectionCfg
{
	RATigerFEYBasisSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RATigerFEYBasisLevelCfg> basis_level_cfg;
};

// Ω±¿¯≈‰÷√
struct RATigerFEYRewardCfg
{
	RATigerFEYRewardCfg(): seq(0), fish_num(0)
	{}

	int seq;
	int fish_num;
	std::vector<ItemConfigData> reward_item;
};

struct RATigerFEYRewardLevelCfg
{
	RATigerFEYRewardLevelCfg(): level(0)
	{}
	
	int level;
	std::vector<RATigerFEYRewardCfg> reward_cfg;
};

struct RATigerFEYRewardSectionCfg
{
	RATigerFEYRewardSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RATigerFEYRewardLevelCfg> reward_level_cfg;
};



class RandActivityManager;
class RandActivityTigerFishEveryYearConfig : public RandActivityConfig
{
public:
	RandActivityTigerFishEveryYearConfig();
	virtual ~RandActivityTigerFishEveryYearConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const std::vector<RATigerFEYBasisCfg> * GetBasisCfg(RandActivityManager* ramgr, int role_level)const;
	const std::vector<RATigerFEYRewardCfg> * GetRewardCfg(RandActivityManager* ramgr, int role_level)const;

private:
	int InitBasisCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);

	std::vector<RATigerFEYBasisSectionCfg> m_basis_section_cfg;
	std::vector<RATigerFEYRewardSectionCfg> m_reward_section_cfg;
};




#endif