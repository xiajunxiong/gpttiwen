#ifndef __RAND_ACTIVITY_QI_XI_TE_HUI_CONFIG_HPP__
#define __RAND_ACTIVITY_QI_XI_TE_HUI_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"

struct RAQiXiTeHuiOtherCfg
{
	RAQiXiTeHuiOtherCfg() : cfg_ver(0) {}

	int cfg_ver;
};

struct RAQiXiTeHuiBaseCfg
{
	RAQiXiTeHuiBaseCfg() :activity_day(0), reward_group(0), buy_limit(0), limit_times(0), price(0) {}

	int activity_day;
	int reward_group;
	int buy_limit;
	int limit_times;
	int price;
};

struct RAQiXiTeHuiBaseSectionCfg
{
	RAQiXiTeHuiBaseSectionCfg() :section_start(0), section_end(0) {}

	int section_start;
	int section_end;

	std::vector<RAQiXiTeHuiBaseCfg> base_list;
};

struct RAQiXiTeHuiRewardCfg
{
	RAQiXiTeHuiRewardCfg() : rate(0)
	{}

	int rate;
	std::vector<ItemConfigData> item_list;
};

struct RAQiXiTeHuiRewardRroupCfg
{
	RAQiXiTeHuiRewardRroupCfg() : rate_count(0)
	{}

	int rate_count;		//×Ü¸ÅÂÊ 
	std::vector<RAQiXiTeHuiRewardCfg> reward_list;	
};

struct RAQiXiTeHuiPhaseRewardCfg
{
	RAQiXiTeHuiPhaseRewardCfg() : buy_times(0)
	{}
	
	int buy_times;
	ItemConfigData reward;
};

struct RAQiXiTeHuiPhaseRewardSectionCfg
{
	RAQiXiTeHuiPhaseRewardSectionCfg() : section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RAQiXiTeHuiPhaseRewardCfg> reward_list;
};

class RandActivityManager;
class RandActivityQiXiTeHuiConfig : public RandActivityConfig
{
public:
	RandActivityQiXiTeHuiConfig();
	virtual ~RandActivityQiXiTeHuiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	virtual int GetConfigVer() const;
	const RAQiXiTeHuiBaseCfg * GetBaseCfg(RandActivityManager* ramgr, int seq)const;
	const RAQiXiTeHuiRewardCfg * GetRewardCfg(int reward_group_id)const;
	const RAQiXiTeHuiPhaseRewardCfg * GetPhaseRewardCfg(RandActivityManager* ramgr, int seq) const;
	const RAQiXiTeHuiPhaseRewardSectionCfg * GetPhaseSectionCfg(RandActivityManager* ramgr) const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitBaseCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);
	int InitPhaseCfg(TiXmlElement *RootElement);

	RAQiXiTeHuiOtherCfg m_other_cfg;
	std::vector<RAQiXiTeHuiBaseSectionCfg> m_base_section_cfg;
	std::map<int, RAQiXiTeHuiRewardRroupCfg> m_reward_group_cfg;			//½±Àø×é key:reward_group_id
	std::vector<RAQiXiTeHuiPhaseRewardSectionCfg> m_phase_reward_cfg;			
};

#endif	//__RAND_ACTIVITY_QI_XI_TE_HUI_CONFIG_HPP__