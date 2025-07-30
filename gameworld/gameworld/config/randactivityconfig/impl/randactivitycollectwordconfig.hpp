#ifndef __RAND_ACTIVITY_COLLECT_WORD_CONFIG_HPP__
#define __RAND_ACTIVITY_COLLECT_WORD_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/racollectwordparam.hpp"
#include "item/itemextern.hpp"
#include "item/itembase.h"

struct RaCollectWordExchangeCfg
{
	RaCollectWordExchangeCfg()
	{
		limit_exchange_times = 0;
		limit_total_exchange_times = 0;
		is_broadcast = 0;
	}

	int limit_exchange_times;
	int limit_total_exchange_times;
	int is_broadcast;
	ItemConfigData reward;
	std::vector<ItemExtern::ItemConsumeConfig> consume_item;
};

struct RACollectWordOtherCfg
{
	RACollectWordOtherCfg() : count(0)
	{}

	int count;
	ItemConfigData day_reward_list[RA_COLLECT_WORD_MAX_DAY_GIFT_REWARD_NUM];
};

class RandActivityManager;

class RandActivityCollectWordConfig : public RandActivityConfig
{
public:
	RandActivityCollectWordConfig();
	virtual ~RandActivityCollectWordConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RaCollectWordExchangeCfg * GetExchangeCfg(int seq) const ;
	const RACollectWordOtherCfg& GetOtherCfg() const { return m_other_cfg; }
private:
	int InitExchangeCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	RACollectWordOtherCfg m_other_cfg;
	std::vector<RaCollectWordExchangeCfg> m_exchange_cfg;
};

#endif	//__RAND_ACTIVITY_COLLECT_WORD_CONFIG_HPP__