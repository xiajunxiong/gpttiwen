#ifndef __RAND_ACTIVITY_HERITAGE_TREASIRE_CONFIG_HPP__
#define __RAND_ACTIVITY_HERITAGE_TREASIRE_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raheritagetreasureparam.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/bitmapfunction.hpp"
#include "item/itembase.h"
#include <set>
#include <vector>
#include "servercommon/maildef.hpp"

struct RAHeritageTreasureOtherCfg
{
	RAHeritageTreasureOtherCfg(int _section_start, int _section_end) : section_start(_section_start), section_end(_section_end)
	{
		active_clue_item_id = 0;
		end_day_time = 0;
		coin_num = 0;
		gold_num = 0;
		is_complement = 0;
	}

	int section_start;
	int section_end;

	int active_clue_item_id;
	std::vector<ItemConfigData> final_reward;
	int end_day_time;		//大奖可补足天数
	int coin_num;			//选择一种即可
	int gold_num;
	int is_complement;
};

struct RAHeritageTreasureRewardCfg
{
	struct ItemCfg
	{
		ItemCfg() :rate(0),reward_num(0)
		{
			is_broadcast = 0;
		}

		int is_broadcast;
		int rate;

		int reward_num;
		std::vector<ItemConfigData> reward;
	};

	RAHeritageTreasureRewardCfg()
	{
		count = 0;
	}

	int count;
	std::vector<ItemCfg> group_reward;
};


struct RAHeritageTreasureRewardSectionCfg
{
	RAHeritageTreasureRewardSectionCfg(int _section_start, int _section_end) : section_start(_section_start), section_end(_section_end)
	{}

	int section_start;
	int section_end;

	RAHeritageTreasureRewardCfg m_reward_cfg;
};

class RandActivityManager;

class RandActivityHeritageTreasireConfig : public RandActivityConfig
{
public:
	RandActivityHeritageTreasireConfig();
	virtual ~RandActivityHeritageTreasireConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAHeritageTreasureOtherCfg & GetHeritageTreasureOtherCfg(RandActivityManager* ramgr)const;
	const RAHeritageTreasureRewardCfg::ItemCfg * GetRandReward(RandActivityManager* ramgr) const;
	void GetRandClueId(unsigned char ARG_OUT(&clue_id)[RA_HERITAGE_TREASURE_MAX_PIECE]) ;
private:
	int InitClueCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitRewardCfg(TiXmlElement * RootElement);

	std::vector<RAHeritageTreasureOtherCfg> m_heritage_treasure_cfg;
	std::vector<int> m_clue_cfg;		//线索id，用来随机
	std::vector<RAHeritageTreasureRewardSectionCfg> m_reward_section_cfg;
};

#endif	//__RAND_ACTIVITY_HERITAGE_TREASIRE_CONFIG_HPP__