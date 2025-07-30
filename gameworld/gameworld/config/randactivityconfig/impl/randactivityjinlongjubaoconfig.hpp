#ifndef __RAND_ACTIVITY_JIN_LONG_JU_BAO_CONFIG_HPP__
#define __RAND_ACTIVITY_JIN_LONG_JU_BAO_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

enum RAND_ACTIVITY_JIN_LONG_JU_BAO_REWARD_TYPE
{
	RAND_ACTIVITY_JIN_LONG_JU_BAO_REWARD_TYPE_SELECT,
	RAND_ACTIVITY_JIN_LONG_JU_BAO_REWARD_TYPE_RAND,
};

struct  RandActivityJinLongJuBaoCfg
{
	int type;
	int reward_group_id;
};

struct  RandActivityJinLongJuBaoSectionCfg
{
	RandActivityJinLongJuBaoSectionCfg() :section_start(0), section_end(0)
	{

	}

	int section_start;
	int section_end;

	std::map<int, RandActivityJinLongJuBaoCfg> item_list;
};

struct  RandActivityJinLongJuBaoRewardGroupItemCfg
{
	int rate;
	int reward_group_id;
	int reward_seq;
	int bao_di_id;
	int bao_di_times;
	std::vector<ItemConfigData> rewards;
};


struct  RandActivityJinLongJuBaoRewardGroupCfg
{
	int reward_group_id;
	std::map<int, RandActivityJinLongJuBaoRewardGroupItemCfg> item_list;
};

struct  RandActivityJinLongJuBaoTimesRewardCfg
{
	int seq;
	int times;
	std::vector<ItemConfigData> rewards;
};

struct  RandActivityJinLongJuBaoTimesRewardSectionCfg
{
	RandActivityJinLongJuBaoTimesRewardSectionCfg() :section_start(0), section_end(0)
	{

	}

	int section_start;
	int section_end;

	std::map<int, RandActivityJinLongJuBaoTimesRewardCfg> item_list;
};

struct  RandActivityJinLongJuBaBuyItemCfg
{
	RandActivityJinLongJuBaBuyItemCfg()
	{
		seq = 0;
		need_chong_zhi_gold = 0;
		limit_type = 0;
		limit_buy_times_day = 0;
		limit_buy_times = 0;
	}

	int seq;
	int need_chong_zhi_gold;
	int limit_type;
	int limit_buy_times_day;
	int limit_buy_times;

	std::vector<ItemConfigData> rewards;
};

struct  RandActivityJinLongJuBaBuyItemSectionCfg
{
	RandActivityJinLongJuBaBuyItemSectionCfg() :section_start(0), section_end(0)
	{

	}

	int section_start;
	int section_end;

	std::map<int, RandActivityJinLongJuBaBuyItemCfg> item_list;
};

struct  RandActivityJinLongJuBaoOtherCfg
{
	int cfg_ver;	
	int draw_need_item_id;
	int draw_need_item_num;
};

class RandActivityManager;
class RandActivityJinLongJuBaoConfig : public RandActivityConfig
{
public:
	RandActivityJinLongJuBaoConfig();
	virtual ~RandActivityJinLongJuBaoConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RandActivityJinLongJuBaoOtherCfg & GetOtherCfg() const;
	const RandActivityJinLongJuBaoCfg * GetRandActivityJinLongJuBaoCfg(RandActivityManager * ramgr, int reward_type) const;
	const RandActivityJinLongJuBaoRewardGroupItemCfg * GetRandActivityJinLongJuBaoRewardGroupItemCfg(RandActivityManager * ramgr, int reward_type, int reward_seq) const;
	const RandActivityJinLongJuBaoRewardGroupCfg * GetRandActivityJinLongJuBaoRewardGroupCfg(RandActivityManager * ramgr, int reward_type) const;
	const RandActivityJinLongJuBaBuyItemCfg * GetRandActivityJinLongJuBaBuyItemCfg(RandActivityManager * ramgr, int buy_seq) const;
	const RandActivityJinLongJuBaoTimesRewardCfg * GetRandActivityJinLongJuBaoTimesRewardCfg(RandActivityManager * ramgr, int reward_seq) const;
	const RandActivityJinLongJuBaoTimesRewardSectionCfg * GetRandActivityJinLongJuBaoTimesRewardSectionCfg(RandActivityManager * ramgr, time_t time = 0) const;
	
	virtual int GetConfigVer() const;
private: 
	int InitOtherfg(TiXmlElement * RootElement);
	int InitRandActivityJinLongJuBaoCfg(TiXmlElement * RootElement);
	int InitRandActivityJinLongJuBaoTimesRewardCfg(TiXmlElement * RootElement);
	int InitRandActivityJinLongJuBaoRewardGroupItemCfg(TiXmlElement * RootElement);
	int InitRandActivityJinLongJuBaBuyItemCfg(TiXmlElement * RootElement);

	std::vector<RandActivityJinLongJuBaoSectionCfg> m_section_cfg;
	std::vector<RandActivityJinLongJuBaoTimesRewardSectionCfg> m_times_reward_section_cfg;
	std::vector<RandActivityJinLongJuBaBuyItemSectionCfg> m_buy_section_cfg;
	std::map<int, RandActivityJinLongJuBaoRewardGroupCfg> m_reward_group_cfg;
	RandActivityJinLongJuBaoOtherCfg m_other_cfg;
};

#endif	