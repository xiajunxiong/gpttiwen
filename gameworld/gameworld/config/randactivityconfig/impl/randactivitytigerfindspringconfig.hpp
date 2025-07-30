#ifndef __RAND_ACTIVITY_TIGER_FIND_SPRING_CONFIG_HPP__
#define __RAND_ACTIVITY_TIGER_FIND_SPRING_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itempool.h"
#include "servercommon/activitydef.hpp"


// ��������
struct RATFSRewardCfg
{
	RATFSRewardCfg(): activity_day(0), seq(0), scene_id(0), reward_pos(0,0)
	{}

	int activity_day;		//���n��
	int seq;
	int scene_id;
	Posi reward_pos;		//��������
	std::vector<ItemConfigData> reward_list;
};

struct RATFSRewardSectionCfg
{
	RATFSRewardSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RATFSRewardCfg> reward_cfg;
};

// ��������
struct RATFSOtherCfg
{
	RATFSOtherCfg():explore_radius(0)
	{}

	int explore_radius;		//��ͼ̽������뾶
};


class RandActivityManager;
class RandActivityTigerFindSpringConfig : public RandActivityConfig
{
public:
	RandActivityTigerFindSpringConfig();
	virtual ~RandActivityTigerFindSpringConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RATFSRewardCfg * GetRewardCfg(RandActivityManager* ramgr, bool is_yesterday = false)const;
	const RATFSOtherCfg & GetOtherCfg()const { return m_other_cfg; }

private:
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::vector<RATFSRewardSectionCfg> m_reward_section_cfg;
	RATFSOtherCfg m_other_cfg;
};


#endif