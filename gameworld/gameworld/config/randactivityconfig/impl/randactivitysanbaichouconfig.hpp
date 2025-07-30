#ifndef __RAND_ACTIVITY_300_CHOU_CONFIG_HPP__
#define __RAND_ACTIVITY_300_CHOU_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"


struct RASanBaiChouTypeCfg
{
	RASanBaiChouTypeCfg():level(0), extract_times(0), reward_group(0) {}

	int level;
	int extract_times;
	int reward_group;
};

struct RASanBaiChouItem
{
	short seq;
	int rate;
	bool is_baodi;
};

struct RASanBaiChouGroupCfg
{
	RASanBaiChouGroupCfg() : max_rate(0), baodi_rate(0) {}

	int max_rate;
	std::vector<RASanBaiChouItem> item_list;

	int baodi_rate;
	std::vector<RASanBaiChouItem> baodi_item_list;
};


struct RASanBaiChouOtherCfg
{
	RASanBaiChouOtherCfg() :reward_extract_times(0), guaranteed_times(0){}

	int reward_extract_times;				// 大奖连抽次数
	int guaranteed_times;					// 保底次数
};


class RandActivityManager;
class RandActivitySanBaiChouConfig : public RandActivityConfig
{
public:
	RandActivitySanBaiChouConfig();
	virtual ~RandActivitySanBaiChouConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RASanBaiChouTypeCfg * GetTypeCfg(int type) const;
	const RASanBaiChouGroupCfg * GetGroupCfg(int group) const;
	const ItemConfigData * GetItemCfg(int seq) const;

	const RASanBaiChouOtherCfg * GetOtherCfg() const { return &m_other_cfg; }

private:
	int InitTypeCfg(TiXmlElement *RootElement);
	int InitGroupCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::vector<RASanBaiChouTypeCfg> m_type_cfg;
	std::map<int, RASanBaiChouGroupCfg> m_group_cfg;
	std::vector<ItemConfigData> m_seq_item_cfg;
	RASanBaiChouOtherCfg m_other_cfg;
};


#endif	// __RAND_ACTIVITY_300_CHOU_CONFIG_HPP__