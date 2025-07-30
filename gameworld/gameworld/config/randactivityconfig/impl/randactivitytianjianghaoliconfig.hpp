#ifndef __RAND_ACTIVITY_TIANG_JIANG_HAO_LI_CONFIG_HPP__
#define __RAND_ACTIVITY_TIANG_JIANG_HAO_LI_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/ratianjianghaoliparam.hpp"
#include "item/itembase.h"

struct RATianJiangHaoLiOtherCfg
{
	RATianJiangHaoLiOtherCfg() :reward_num(0), map_case_num(0), start_time(0), end_time(0), refresh_time(0) {}

	unsigned short reward_num;
	unsigned short map_case_num;
	unsigned short start_time;			//宝箱刷新时间
	unsigned short end_time;			//宝箱结束时间
	unsigned int refresh_time;
};

struct RATianJiangHaoLiPointCfg
{
	RATianJiangHaoLiPointCfg() :scene_id(0), npc_gather_id(0) {}

	int scene_id;
	int npc_gather_id;		//采集物ID
};

struct RATianJiangHaoLiRewardCfg
{
	RATianJiangHaoLiRewardCfg() :weight(0), hearsay(0) {}

	int weight;
	int hearsay;
	ItemConfigData reward_item;
};

struct RATianJiangHaoLiRewardSectionCfg
{
	RATianJiangHaoLiRewardSectionCfg(int _section_start, int _section_end) : section_start(_section_start), section_end(_section_end)
	{
		m_all_weight = 0;
	}

	int section_start;
	int section_end;

	int m_all_weight;
	std::vector<RATianJiangHaoLiRewardCfg> m_reward_cfg;
};

class RandActivityManager;

class RandActivityTianJiangHaoLiConfig : public RandActivityConfig
{
public:
	RandActivityTianJiangHaoLiConfig();
	virtual ~RandActivityTianJiangHaoLiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RATianJiangHaoLiOtherCfg & GetOtherCfg() const { return m_other_cfg; }
	const RATianJiangHaoLiPointCfg * GetPointCfg(int seq) const;
	const std::vector<RATianJiangHaoLiPointCfg> & GetAllPointCfg() const { return m_point_cfg; }
	const RATianJiangHaoLiRewardCfg * GetRewardCfg(RandActivityManager * ramgr) const;

	int GetPointCfgMaxCount() { return (int)m_point_cfg.size(); }
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitPointCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);

	RATianJiangHaoLiOtherCfg m_other_cfg;
	std::vector<RATianJiangHaoLiPointCfg> m_point_cfg;
	std::vector<RATianJiangHaoLiRewardSectionCfg> m_point_section_cfg;

};

#endif	//__RAND_ACTIVITY_TIANG_JIANG_HAO_LI_CONFIG_HPP__