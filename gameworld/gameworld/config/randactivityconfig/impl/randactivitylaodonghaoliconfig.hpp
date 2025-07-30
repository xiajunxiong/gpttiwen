#ifndef __RAND_ACTIVITY_LAO_DONG_HAO_LI_CONFIG_HPP__
#define __RAND_ACTIVITY_LAO_DONG_HAO_LI_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/ratianjianghaoliparam.hpp"
#include "item/itembase.h"

struct RALaoDongHaoLiOtherCfg
{
	RALaoDongHaoLiOtherCfg() :reward_num(0), map_case_num(0), start_time(0), end_time(0), refresh_time(0) {}

	unsigned short reward_num;
	unsigned short map_case_num;
	unsigned short start_time;			//宝箱刷新时间
	unsigned short end_time;			//宝箱结束时间
	unsigned int refresh_time;
};

struct RALaoDongHaoLiPointCfg
{
	RALaoDongHaoLiPointCfg() :scene_id(0), npc_gather_id(0) {}

	int scene_id;
	int npc_gather_id;		//采集物ID
};

struct RALaoDongHaoLiRewardCfg
{
	RALaoDongHaoLiRewardCfg() :weight(0), hearsay(0) {}

	int weight;
	int hearsay;
	ItemConfigData reward_item;
};

class RandActivityManager;

class RandActivityLaoDongHaoLiConfig : public RandActivityConfig
{
public:
	RandActivityLaoDongHaoLiConfig();
	virtual ~RandActivityLaoDongHaoLiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RALaoDongHaoLiOtherCfg & GetOtherCfg() const { return m_other_cfg; }
	const RALaoDongHaoLiPointCfg * GetPointCfg(int seq) const;
	const std::vector<RALaoDongHaoLiPointCfg> & GetAllPointCfg() const { return m_point_cfg; }
	const RALaoDongHaoLiRewardCfg * GetRewardCfg() const;

	int GetPointCfgMaxCount() { return (int)m_point_cfg.size(); }
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitPointCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);

	RALaoDongHaoLiOtherCfg m_other_cfg;
	std::vector<RALaoDongHaoLiPointCfg> m_point_cfg;
	std::vector<RALaoDongHaoLiRewardCfg> m_reward_cfg;

	int m_all_weight;
};

#endif	//__RAND_ACTIVITY_TIANG_JIANG_HAO_LI_CONFIG_HPP__