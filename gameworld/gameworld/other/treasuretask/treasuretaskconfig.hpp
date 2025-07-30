#ifndef __TREASURE_TASK_CONFIG_HPP__
#define __TREASURE_TASK_CONFIG_HPP__
#pragma once

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"

#include <vector>
#include <map>

struct TreasureTaskOtherCfg
{
	TreasureTaskOtherCfg() : quantity(0), level(0), floors(0) {}

	short quantity;				// 每天任务数量
	int level;					// 开启等级
	int floors;					// 保底次数
};

struct TreasureTaskLevelCfg
{
	TreasureTaskLevelCfg() : seq(0), reward_group(0), rate(0), get_rate(0) {}

	short seq;
	short reward_group;
	int rate;				// 出现概率
	int get_rate;			// 获得概率
};

struct TreasureTaskGroupCfg
{
	TreasureTaskGroupCfg() : seq(0) {}

	short seq;
	ItemConfigData reward;
};

struct TreasureTaskGroupMap
{
	std::map<int, TreasureTaskGroupCfg> group_cfg;
};

struct TreasureTaskCoordinateCfg
{
	TreasureTaskCoordinateCfg() : scene_id(0) {}

	int scene_id;
	Posi pos;
};

struct TreasureTaskRouteMap
{
	std::vector<short> route;
};


class TreasureTaskConfig : public ILogicConfig
{
public:
	TreasureTaskConfig();
	virtual ~TreasureTaskConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const TreasureTaskOtherCfg& GetOtherCfg()const { return m_other_cfg; }
	const std::vector<TreasureTaskLevelCfg> * GetTaskLevelVec(int level);
	const TreasureTaskGroupCfg * GetGroupRewardCfg(int group_id);
	short GetRouteNum() { return m_route_num; }
	short GetCoordomate(short route, short order_id);

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitLevelCfg(TiXmlElement *RootElement);
	int InitGroupCfg(TiXmlElement *RootElement);
	int InitCoordomateCfg(TiXmlElement *RootElement);
	int InitRouteCfg(TiXmlElement *RootElement);

	TreasureTaskOtherCfg m_other_cfg;
	std::map<int, std::vector<TreasureTaskLevelCfg> > m_level_cfg;
	std::vector<TreasureTaskGroupMap> m_group_cfg;
	std::vector<TreasureTaskCoordinateCfg> m_coordomate_cfg;

	short m_route_num;
	std::vector<TreasureTaskRouteMap> m_route_cfg;
};

#endif // !__TREASURE_TASK_CONFIG_HPP__

