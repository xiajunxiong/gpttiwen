#ifndef __RAND_ACTIVITY_LUCKY_SIGN_CONFIG_HPP__
#define __RAND_ACTIVITY_LUCKY_SIGN_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raburningsummerparam.hpp"
#include <set>

struct RALuckySignRewardCfg
{
	RALuckySignRewardCfg() : type_seq(0), rate(0)
	{}

	int type_seq;
	int rate;
	std::vector<ItemConfigData> reward_list;
};

struct RALuckySignRewardGroupCfg
{
	RALuckySignRewardGroupCfg() : rate_count(0)
	{}

	int rate_count;
	std::vector<RALuckySignRewardCfg> node_list;
};

struct RALuckySignSignCfg
{
	RALuckySignSignCfg() : sign_id(0), rate(0)
	{}

	int sign_id;
	int rate;
};

struct RALuckySignSignGroupCfg
{
	RALuckySignSignGroupCfg() : rate_count(0)
	{}

	int rate_count;
	std::vector<RALuckySignSignCfg> node_list;
};

struct RALuckySignCfg
{
	RALuckySignCfg() : sign_group_id(0), reward_group_id(0)
	{}

	int sign_group_id;
	int reward_group_id;
};

struct RALuckySignSectionCfg
{
	RALuckySignSectionCfg() : section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RALuckySignCfg> node_list;
};

class RandActivityManager;
class RandActivityLuckySignConfig : public RandActivityConfig
{
public:
	RandActivityLuckySignConfig();
	virtual ~RandActivityLuckySignConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RALuckySignCfg * GetRandCfg(RandActivityManager * ramgr) const;
	const RALuckySignSignCfg * GetSignCfg(int sign_group_id)const;
	const RALuckySignRewardCfg * GetRewardCfg(int reward_group_id)const;

private: 
	int InitRewardGroupCfg(TiXmlElement *RootElement);
	int InitSignGroupCfg(TiXmlElement *RootElement);
	int InitCfg(TiXmlElement *RootElement);
	
	std::map<int, RALuckySignRewardGroupCfg> m_reward_group_map;			//key:reward_group_id
	std::map<int, RALuckySignSignGroupCfg> m_sign_group_map;				//key:sign_group_id
	std::vector<RALuckySignSectionCfg> m_lucky_sign_section_cfg;
};

#endif	//__RAND_ACTIVITY_LUCKY_SIGN_CONFIG_HPP__
