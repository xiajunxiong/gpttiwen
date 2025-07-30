#ifndef __RAND_ACTIVITY_DAN_BI_FAN_LI_CONFIG_HPP__
#define __RAND_ACTIVITY_DAN_BI_FAN_LI_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itempool.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/roleactivity/radanbifanliparam.hpp"

struct RADanBiFanLiOtherCfg
{
	RADanBiFanLiOtherCfg() : cfg_ver(0)
	{}

	int cfg_ver;
};

struct RADanBiFanLiRewardRewardCfg
{
	RADanBiFanLiRewardRewardCfg() :seq(0), gold_num(0), gold_give(0), buy_money(0)
	{}

	int seq;
	int gold_num;		//本身给予元宝数
	int gold_give;		//额外赠送元宝数
	int buy_money;		//购买金额(当前单位为元宝)
};

struct RADanBiFanLiRewardSectionCfg
{
	RADanBiFanLiRewardSectionCfg() : section_start(0), section_end(0), reward_pool_count(0)
	{}

	bool IsInclude(int real_open_server_day) const
	{
		return (section_start <= real_open_server_day && real_open_server_day <= section_end);
	}

	int section_start;
	int section_end;
	int reward_pool_count;
	std::vector<RADanBiFanLiRewardRewardCfg> reward_pool_list[MAX_RA_DAN_BI_FAN_LI_POOL_NUM];
};

struct RADanBiFanLiPoolCfg
{
	RADanBiFanLiPoolCfg() : limit_type(0), times(0)
	{}

	int limit_type;				// \see ACTVITY_BUY_LIMIT_TYPE_MAX
	int times;
};

struct RADanBiFanLiPoolSectionCfg
{
	RADanBiFanLiPoolSectionCfg() : section_start(0), section_end(0), pool_count(0)
	{}

	bool IsInclude(int real_open_server_day) const
	{
		return (section_start <= real_open_server_day && real_open_server_day <= section_end);
	}

	int section_start;
	int section_end;
	int pool_count;
	RADanBiFanLiPoolCfg pool_list[MAX_RA_DAN_BI_FAN_LI_POOL_NUM];
};

class RandActivityDanBiFanLiConfig : public RandActivityConfig
{
public:
	RandActivityDanBiFanLiConfig();
	virtual ~RandActivityDanBiFanLiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RADanBiFanLiRewardRewardCfg* GetRewardCfg(RandActivityManager* ramgr, int pool_seq, int seq)const;
	const RADanBiFanLiPoolCfg* GetPoolCfg(RandActivityManager* ramgr, int pool_seq)const;

	virtual int GetConfigVer() const;

private:
	int InitOtherCfg(TiXmlElement* RootElement, std::string& err);
	int InitRewardCfg(TiXmlElement* RootElement, std::string& err);
	int InitPoolCfg(TiXmlElement* RootElement, std::string& err);

	int CheckConfig();

	RADanBiFanLiOtherCfg m_other_cfg;

	std::vector<RADanBiFanLiRewardSectionCfg> m_reward_section_cfg;
	std::vector<RADanBiFanLiPoolSectionCfg> m_pool_section_cfg;
};

#endif	// __RAND_ACTIVITY_DAN_BI_FAN_LI_CONFIG_HPP__