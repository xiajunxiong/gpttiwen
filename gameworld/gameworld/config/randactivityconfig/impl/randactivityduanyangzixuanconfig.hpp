#ifndef __RAND_ACTIVITY_DUAN_YANG_ZI_XUAN_CONFIG_HPP__
#define __RAND_ACTIVITY_DUAN_YANG_ZI_XUAN_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

struct RandActivityDuanYangZiXuanOtherCfg
{
	int need_chong_zhi_gold;
	int cfg_ver;
};

struct  RandActivityDuanYangZiXuanCfg
{
	int day;
	int reward_group_id;
};

struct  RandActivityDuanYangZiXuanSectionCfg
{
	RandActivityDuanYangZiXuanSectionCfg() :section_start(0), section_end(0)
	{
	
	}

	int section_start;
	int section_end;

	std::map<int, RandActivityDuanYangZiXuanCfg> item_list;
};

struct  RandActivityDuanYangZiXuanRewardGroupCfgKey
{
	bool operator<(const RandActivityDuanYangZiXuanRewardGroupCfgKey & other)const
	{
		if (group_id < other.group_id)
			return true;
		if (group_id > other.group_id)
			return false;

		if (reward_seq < other.reward_seq)
			return true;
		if (reward_seq > other.reward_seq)
			return false;

		return false;
	}

	int group_id;
	int reward_seq;
};

struct  RandActivityDuanYangZiXuanRewardGroupCfg
{
	int group_id;
	int reward_seq;

	std::vector<ItemConfigData> rewards;
};


class RandActivityManager;
class RandActivityDuanYangZiXuanConfig : public RandActivityConfig
{
public:
	RandActivityDuanYangZiXuanConfig();
	virtual ~RandActivityDuanYangZiXuanConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RandActivityDuanYangZiXuanOtherCfg & GetOtherCfg() const;
	const RandActivityDuanYangZiXuanRewardGroupCfg * GetRandActivityDuanYangZiXuanRewardGroupCfg(RandActivityManager * ramgr, int group_id, int reward_seq) const;
	const RandActivityDuanYangZiXuanCfg * GetRandActivityDuanYangZiXuanCfg(RandActivityManager * ramgr, int day) const;
	const RandActivityDuanYangZiXuanSectionCfg * GetRandActivityDuanYangZiXuanSectionCfg(RandActivityManager * ramgr, time_t time = 0) const;
	
	virtual int GetConfigVer() const;
private: 
	int InitOtherfg(TiXmlElement * RootElement);
	int InitRewardGroupCfg(TiXmlElement * RootElement);
	int InitRandActivityDuanYangZiXuanCfg(TiXmlElement * RootElement);

	std::vector<RandActivityDuanYangZiXuanSectionCfg> m_section_cfg;
	RandActivityDuanYangZiXuanOtherCfg m_other_cfg;
	std::map<RandActivityDuanYangZiXuanRewardGroupCfgKey, RandActivityDuanYangZiXuanRewardGroupCfg> m_reward_group_cfg;
};

#endif	