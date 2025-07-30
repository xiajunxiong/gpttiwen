#ifndef __RAND_ACTIVITY_MIAO_BI_SHENG_HUA_CONFIG_HPP__
#define __RAND_ACTIVITY_MIAO_BI_SHENG_HUA_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/activitydef.hpp"

struct MBSHBaseSectionCfg
{
	MBSHBaseSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<ItemConfigData> reward_list;
};

struct MBSHOtherCfg
{
	MBSHOtherCfg():consunme_item(0),consunme_num(0)
	{}

	int consunme_item;		// 要消耗的道具id
	int consunme_num;		// 消耗数量
};


class RandActivityManager;
class RandActivityMiaoBiShengHuaConfig : public RandActivityConfig
{
public:
	RandActivityMiaoBiShengHuaConfig();
	virtual ~RandActivityMiaoBiShengHuaConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const std::vector<ItemConfigData>* GetRewardCfg(RandActivityManager* ramgr)const;
	const MBSHOtherCfg& GetOtherCfg()const { return m_other_cfg; }

private:
	int InitBaseCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::vector<MBSHBaseSectionCfg> m_base_section_cfg;
	MBSHOtherCfg m_other_cfg;
};

#endif