#ifndef __RAND_ACTIVITY_TU_JIAN_TREASURE_CONFIG_HPP__
#define __RAND_ACTIVITY_TU_JIAN_TREASURE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/ratujiantreasureparam.hpp"
#include "item/itembase.h"

struct RATuJianTreasureOtherCfg
{
	RATuJianTreasureOtherCfg() :open_grade(0), open_time(0) {}

	int open_grade;
	int open_time;
};

struct RATuJianTreasureLoginCfg
{
	RATuJianTreasureLoginCfg() :seq(0), sign_day(0) {}

	int seq;
	int sign_day;
	ItemConfigData item;
};

struct RATuJianTreasureRewardCfg
{
	RATuJianTreasureRewardCfg() :seq(0), type(0), param1(0), param2(0), param3(0) {}

	int seq;
	int type;
	int param1;
	short param2;
	short param3;
	ItemConfigData item;
};

class RandActivityManager;

class RandActivityTuJianTreasureConfig : public RandActivityConfig
{
public:
	RandActivityTuJianTreasureConfig();
	virtual ~RandActivityTuJianTreasureConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RATuJianTreasureOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	const RATuJianTreasureLoginCfg * GetLoginCfg(int seq)const;
	const RATuJianTreasureRewardCfg * GetRewardCfg(int seq)const;
	const int GetRewardCfgLen() const { return (int)m_reward_cfg.size(); }

private:
	int InitRATuJianTreasureOtherCfg(TiXmlElement *RootElement);
	int InitRATuJianTreasureLoginCfg(TiXmlElement *RootElement);
	int InitRATuJianTreasureRewardCfg(TiXmlElement *RootElement);

	RATuJianTreasureOtherCfg m_other_cfg;
	std::vector<RATuJianTreasureLoginCfg> m_login_cfg;
	std::vector<RATuJianTreasureRewardCfg> m_reward_cfg;
};

#endif	//__RAND_ACTIVITY_TU_JIAN_TREASURE_CONFIG_HPP__