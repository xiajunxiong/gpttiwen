#ifndef __RAND_ACTIVITY_LOTUS_MI_BAO_CONFIG_HPP__
#define __RAND_ACTIVITY_LOTUS_MI_BAO_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/racoolsummerparam.hpp"

struct RALotusMiBaoCfg
{
	RALotusMiBaoCfg() : active_item_id(0), active_item_num(0)
	{}

	ItemID active_item_id;
	int active_item_num;
};

struct RALotusMiBaoSectionCfg
{
	RALotusMiBaoSectionCfg() : section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	RALotusMiBaoCfg node_list[MAX_RA_LOTUS_MI_BAO_ROW_NUM][MAX_RA_LOTUS_MI_BAO_COLUMN_NUM];
};

struct RALotusMiBaoRewardCfg
{
	RALotusMiBaoRewardCfg() : rate(0)
	{}

	int rate;
	std::vector<ItemConfigData> reward_list;
};

struct RALotusMiBaoRewardGroupCfg
{
	RALotusMiBaoRewardGroupCfg() : rate_count(0)
	{}

	int rate_count;
	std::vector<RALotusMiBaoRewardCfg> node_list;
};

struct RALotsMiBaoFinishCondRewardCfg
{
	RALotsMiBaoFinishCondRewardCfg() : reward_group_id(0), is_has_fixe_reward(false)
	{}

	int reward_group_id;
	bool is_has_fixe_reward;	
	ItemConfigData fixe_reward;
};

class RandActivityManager;
class RandActivityLotusMiBaoConfig : public RandActivityConfig
{
public:
	RandActivityLotusMiBaoConfig();
	virtual ~RandActivityLotusMiBaoConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RALotusMiBaoCfg * GetBaseCfg(RandActivityManager * ramgr, int row, int column)const;
	const RALotusMiBaoRewardCfg * GetRandRewardCfg(int reward_group_id)const;
	const RALotsMiBaoFinishCondRewardCfg * GetCondRewardCfg(int cond_id, int param)const;

private:
	int InitBaseCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);
	int InitFinishCondRewardCfg(TiXmlElement *RootElement);
	
	std::vector<RALotusMiBaoSectionCfg> m_base_section_cfg;
	std::map<int, RALotusMiBaoRewardGroupCfg> m_reward_group_map;							//奖励组配置 key:reward_group_id
	std::map<int, std::map<int, RALotsMiBaoFinishCondRewardCfg> > m_finish_cond_reward_map;	//奖励完成条件配置 key1:cond key2:param

};

#endif