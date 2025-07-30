#ifndef __RAND_ACTIVITY_ZHAOZHI_FENXIANG_CONFIG_HPP__
#define __RAND_ACTIVITY_ZHAOZHI_FENXIANG_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/rachaozhifenxiangparam.hpp"

struct RAChaoZhiFenXiangTaskCfg
{
	RAChaoZhiFenXiangTaskCfg() : type(0), parameter(0) {}

	int type;
	int parameter;
};

struct RAChaoZhiFenXiangRewardCfg
{
	RAChaoZhiFenXiangRewardCfg() : task(0) {}

	int task;
	std::vector<ItemConfigData> reward_item;
};

struct RAChaoZhiFenXiangOtherCfg
{
	RAChaoZhiFenXiangOtherCfg() : price_type(0), price(0), buy_type(0) {}

	int price_type;
	int price;
	int buy_type;
	ItemConfigData reward_item;
};

class RandActivityManager;

class RandActivityChaoZhiFenXiangConfig : public RandActivityConfig
{
public:
	RandActivityChaoZhiFenXiangConfig();
	virtual ~RandActivityChaoZhiFenXiangConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const std::vector<RAChaoZhiFenXiangTaskCfg> * GetTaskCfg(RandActivityManager * ramgr, int add_day = 0) const;
	const RAChaoZhiFenXiangRewardCfg * GetRewardCfg(RandActivityManager * ramgr, int seq) const;

	const RAChaoZhiFenXiangOtherCfg & GetOtherCfg() const { return m_other_cfg; }
	int GetRewardNum(RandActivityManager * ramgr) const;
private:
	int InitTaskCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::map<int, std::vector<RAChaoZhiFenXiangTaskCfg> > m_task_cfg;
	std::map<int, std::vector<RAChaoZhiFenXiangRewardCfg> > m_reward_cfg;
	RAChaoZhiFenXiangOtherCfg m_other_cfg;
};

#endif	//__RAND_ACTIVITY_ZHAOZHI_FENXIANG_CONFIG_HPP__