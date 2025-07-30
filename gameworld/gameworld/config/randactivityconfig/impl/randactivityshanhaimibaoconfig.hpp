#ifndef __RAND_ACTIVITY_SHANHAI_MI_BAO_CONFIG_HPP__
#define __RAND_ACTIVITY_SHANHAI_MI_BAO_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/racoolsummerparam.hpp"

struct RAShanHaiMiBaoCfg
{
	RAShanHaiMiBaoCfg() : active_item_id(0), active_item_num(0)
	{}

	ItemID active_item_id;
	int active_item_num;
};

struct RAShanHaiMiBaoSectionCfg
{
	RAShanHaiMiBaoSectionCfg() : section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	RAShanHaiMiBaoCfg node_list[MAX_RA_LOTUS_MI_BAO_ROW_NUM][MAX_RA_LOTUS_MI_BAO_COLUMN_NUM];
};

struct RAShanHaiMiBaoRewardCfg
{
	RAShanHaiMiBaoRewardCfg() : rate(0)
	{}

	int rate;
	std::vector<ItemConfigData> reward_list;
};

struct RAShanHaiMiBaoRewardGroupCfg
{
	RAShanHaiMiBaoRewardGroupCfg() : rate_count(0)
	{}

	int rate_count;
	std::vector<RAShanHaiMiBaoRewardCfg> node_list;
};

struct RAShanMiBaoFinishCondRewardCfg
{
	RAShanMiBaoFinishCondRewardCfg() : reward_group_id(0), is_has_fixe_reward(false)
	{}

	int reward_group_id;
	bool is_has_fixe_reward;	
	ItemConfigData fixe_reward;
};

class RandActivityManager;
class RandActivityShanHaiMiBaoConfig : public RandActivityConfig
{
public:
	RandActivityShanHaiMiBaoConfig();
	virtual ~RandActivityShanHaiMiBaoConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAShanHaiMiBaoCfg * GetBaseCfg(RandActivityManager * ramgr, int row, int column)const;
	const RAShanHaiMiBaoRewardCfg * GetRandRewardCfg(int reward_group_id)const;
	const RAShanMiBaoFinishCondRewardCfg * GetCondRewardCfg(int cond_id, int param)const;

private:
	int InitBaseCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);
	int InitFinishCondRewardCfg(TiXmlElement *RootElement);
	
	std::vector<RAShanHaiMiBaoSectionCfg> m_base_section_cfg;
	std::map<int, RAShanHaiMiBaoRewardGroupCfg> m_reward_group_map;							//奖励组配置 key:reward_group_id
	std::map<int, std::map<int, RAShanMiBaoFinishCondRewardCfg> > m_finish_cond_reward_map;	//奖励完成条件配置 key1:cond key2:param

};

#endif