#ifndef __RAND_ACTIVITY_LING_YU_JIN_JIN_CONFIG_HPP__
#define __RAND_ACTIVITY_LING_YU_JIN_JIN_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RALingYuJiJinCfg
{
	RALingYuJiJinCfg()
	{
		type = 0;
		need_active_phase = 0;
		seq = 0;
		need_role_level = 0;
		reward_ling_yu_num = 0;
	}

	int type;
	int need_active_phase;
	int seq;
	int need_role_level;
	int reward_ling_yu_num;
};

struct RALingYuJiJinPhareCfg
{
	RALingYuJiJinPhareCfg()
	{
		phase = 0;
		reward_ling_yu_num = 0;
		need_chong_zhi = 0;
	}

	int phase;
	int reward_ling_yu_num;
	int need_chong_zhi;
};

struct RALingYuJiJinOtherCfg
{
	RALingYuJiJinOtherCfg()
	{
		cfg_ver = 0;
	}

	int cfg_ver;
};

class RandActivityManager;
class RandActivityLingYuJiJinConfig : public RandActivityConfig
{
public:
	RandActivityLingYuJiJinConfig();
	virtual ~RandActivityLingYuJiJinConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RALingYuJiJinPhareCfg * GetRALingYuJiJinPhareCfg(int phase) const;
	const RALingYuJiJinCfg * GetRALingYuJiJinCfg(int type) const;
	const RALingYuJiJinOtherCfg & GetnOtherCfg() const;
	virtual int GetConfigVer() const;
private:
	int InitFetchCfg(TiXmlElement * RootElement);
	int InitBuyCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::map<int, RALingYuJiJinPhareCfg>  m_buy_phase_cfg;
	std::vector<RALingYuJiJinCfg> m_fetch_reward_cfg;	
	RALingYuJiJinOtherCfg m_other_cfg;
};

#endif	