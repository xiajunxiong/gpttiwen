#ifndef __RAND_ACTIVITY_BAO_ZI_LAI_LE_CONFIG_HPP__
#define __RAND_ACTIVITY_BAO_ZI_LAI_LE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/activitydef.hpp"

struct BZLLBaseCfg
{
	BZLLBaseCfg():number(0), time(0)
	{}

	int number;		// 一回合包子数量
	int time;		// 回合时间
	std::vector<ItemConfigData> win_reward_list;
	std::vector<ItemConfigData> fail_reward_list;
};

struct BZLLBaseSectionCfg
{
	BZLLBaseSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::map<int, BZLLBaseCfg> reward_cfg;		// key-seq
};


struct BZLLOtherCfg
{
	BZLLOtherCfg():frequency(0)
	{}

	int frequency;		// 每日可挑战次数
};


class RandActivityManager;
class RandActivityBaoZiLaiLeConfig : public RandActivityConfig
{
public:
	RandActivityBaoZiLaiLeConfig();
	virtual ~RandActivityBaoZiLaiLeConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const std::vector<ItemConfigData>* GetRewardCfg(RandActivityManager* ramgr, int seq, bool is_win)const;
	const BZLLOtherCfg& GetOtherCfg()const { return m_other_cfg; }

private:
	int InitBaseCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);


	std::vector<BZLLBaseSectionCfg> m_base_section_cfg;
	BZLLOtherCfg m_other_cfg;
};


#endif
