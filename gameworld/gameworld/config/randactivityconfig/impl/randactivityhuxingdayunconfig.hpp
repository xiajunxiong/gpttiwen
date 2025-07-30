#ifndef __RAND_ACTIVITY_HU_XING_DA_YUN_CONFIG_HPP__
#define __RAND_ACTIVITY_HU_XING_DA_YUN_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"


struct RAHuXingDaYunGiftCfg
{
	RAHuXingDaYunGiftCfg() :repeat_reward_group(0), be_invited_reward_group(0) {}

	std::vector<ItemConfigData> first_reward;
	short repeat_reward_group;
	short be_invited_reward_group;
};

struct RAHuXingDaYunGroupMap
{
	RAHuXingDaYunGroupMap() : max_rate(0) {}

	int max_rate;
	std::map<int, std::vector<ItemConfigData> > map_cfg;
};

struct RAHuXingDaYunOtherCfg
{
	RAHuXingDaYunOtherCfg() : be_invite_times(0), consume_time(0) {}

	int be_invite_times;
	int consume_time;
};


class RandActivityManager;
class RandActivityHuXingDaYunConfig : public RandActivityConfig
{
public:
	RandActivityHuXingDaYunConfig();
	virtual ~RandActivityHuXingDaYunConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAHuXingDaYunGiftCfg * GetGiftCfg(RandActivityManager * ramgr, int seq) const;
	const std::vector<ItemConfigData> * GetGroupCfg(int group_id) const;
	const RAHuXingDaYunOtherCfg * GetOtherCfg() const { return &m_other_cfg; }

private:
	int InitGiftCfg(TiXmlElement *RootElement);
	int InitGroupCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::map<short, std::vector<RAHuXingDaYunGiftCfg> > m_gift_cfg;
	std::map<short, RAHuXingDaYunGroupMap> m_group_cfg;
	RAHuXingDaYunOtherCfg m_other_cfg;
};


#endif	// __RAND_ACTIVITY_HU_XING_DA_YUN_CONFIG_HPP__