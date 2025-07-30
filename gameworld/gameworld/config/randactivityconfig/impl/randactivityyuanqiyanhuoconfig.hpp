#ifndef __RAND_ACTIVITY_YUAN_QI_YAN_HUO_CONFIG_HPP__
#define __RAND_ACTIVITY_YUAN_QI_YAN_HUO_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

struct RandActivityYuanQiYanHuoOtherCfg
{
	int need_chong_zhi_gold;
	std::vector<ItemConfigData> buy_rewards; //直购必得物品
	std::vector<ItemConfigData> free_rewards; //每天免费领取奖励
	int yuan_huo_item_id;
	int yuan_huo_item_open_begin_time;
	int yuan_huo_item_open_end_time;
	int cfg_ver;
};

struct  RandActivityYuanQiYanHuoItemCfg
{
	RandActivityYuanQiYanHuoItemCfg()
	{
		seq = 0;		
	}

	int seq;
	std::vector<ItemConfigData> rewards;
};

struct  RandActivityYuanQiYanHuoSectionCfg
{
	RandActivityYuanQiYanHuoSectionCfg() :section_start(0), section_end(0)
	{
	
	}

	int section_start;
	int section_end;

	std::map<int, RandActivityYuanQiYanHuoItemCfg> item_list;
};

struct  RandActivityYuanQiYanHuoYanHuoRewardItemCfg
{
	RandActivityYuanQiYanHuoYanHuoRewardItemCfg()
	{
		rate = 0;
	}

	int rate;
	std::vector<ItemConfigData> rewards;
};

struct  RandActivityYuanQiYanHuoYanHuoRewardSectionCfg
{
	RandActivityYuanQiYanHuoYanHuoRewardSectionCfg() :section_start(0), section_end(0)
	{
		rate_count = 0;
	}

	int section_start;
	int section_end;

	int rate_count;
	std::vector<RandActivityYuanQiYanHuoYanHuoRewardItemCfg> item_list;
};

class RandActivityManager;
class RandActivityYuanQiYanHuoConfig : public RandActivityConfig
{
public:
	RandActivityYuanQiYanHuoConfig();
	virtual ~RandActivityYuanQiYanHuoConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RandActivityYuanQiYanHuoItemCfg * GetRandActivityYuanQiYuanHuoItemCfg(RandActivityManager * ramgr,int seq) const;
	const RandActivityYuanQiYanHuoOtherCfg & GetOtherCfg() const;
	const RandActivityYuanQiYanHuoSectionCfg * GetRandActivityYuanQiYuanHuoSectionCfg(RandActivityManager * ramgr,time_t time) const;
	const RandActivityYuanQiYanHuoYanHuoRewardItemCfg * GetRandActivityYuanQiYanHuoYanHuoRewardItemCfg(RandActivityManager * ramgr) const;

	virtual int GetConfigVer() const;
private:
	int InitRewardCfg(TiXmlElement * RootElement);
	int InitOtherfg(TiXmlElement * RootElement);
	int InitYanHuoRewardCfg(TiXmlElement * RootElement);

	std::vector<RandActivityYuanQiYanHuoSectionCfg> m_section_cfg;
	RandActivityYuanQiYanHuoOtherCfg m_other_cfg;

	std::vector<RandActivityYuanQiYanHuoYanHuoRewardSectionCfg> m_yan_huo_section_cfg;
};

#endif	