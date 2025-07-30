#ifndef __RAND_ACTIVITY_TRUN_TABLE_CONFIG_HPP__
#define __RAND_ACTIVITY_TRUN_TABLE_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/ratruntableparam.hpp"
#include "item/itembase.h"

struct RATurnTableCfg
{
	RATurnTableCfg() :seq(0), weight(0)
	{}

	int seq;
	ItemConfigData item;
	int weight;				//权重
};

struct RATurnTableSectionCfg
{
	RATurnTableSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;

	RATurnTableCfg node_cfg[RA_TURN_TABLE_MAX_NUM];
};

class RandActivityManager;

class RandActivityTrunTableConfig : public RandActivityConfig
{
public:
	RandActivityTrunTableConfig();
	virtual ~RandActivityTrunTableConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RATurnTableSectionCfg* GetTurnTableItemCfg(RandActivityManager* ramgr)const;
	const RATurnTableCfg* GetTurnTableReward(const RATurnTableSectionCfg *turntablereward_cfg) const;
private:
	int InitCfg(TiXmlElement * RootElement);

	std::vector<RATurnTableSectionCfg> m_turn_table_sections;					// 开服转盘
};

#endif	//__RAND_ACTIVITY_TRUN_TABLE_CONFIG_HPP__