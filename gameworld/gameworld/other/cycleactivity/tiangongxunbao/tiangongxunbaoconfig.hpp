#ifndef __TIAN_GONG_XUN_BAO_CONFIG_HPP__
#define __TIAN_GONG_XUN_BAO_CONFIG_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

#include <map>
#include <vector>

struct TGXBRewardCfg
{
	TGXBRewardCfg():is_broadcast(0)
	{}

	int is_broadcast;
	ItemConfigData reward;
};

struct TGXBOtherCfg
{
	TGXBOtherCfg():item_id(0),consume(0), max_action_grid(0)
	{}

	int item_id;
	int consume;
	int max_action_grid;
};

class TianGongXunBaoConfig : public ILogicConfig
{
public:
	TianGongXunBaoConfig();
	virtual ~TianGongXunBaoConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const TGXBRewardCfg * GetNextReward(ARG_OUT int& cur_grid)const;		//cur_grid返回时会变为下一个格点
	const TGXBOtherCfg& GetOtherCfg()const { return m_other_cfg; }

private:
	int InitReardGroupCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitOtherCfg(TiXmlElement *RootElement, std::string& errormsg);

	std::vector<int> m_rate_cfg;  
	std::map<int, TGXBRewardCfg> m_reward_cfg;	// key-seq
	TGXBOtherCfg m_other_cfg;
};

#endif
