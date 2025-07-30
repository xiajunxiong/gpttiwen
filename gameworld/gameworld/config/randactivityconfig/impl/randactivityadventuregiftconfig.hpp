#ifndef __RAND_ACTIVITY_ADVENTURE_GIFT_CONFIG_HPP__
#define __RAND_ACTIVITY_ADVENTURE_GIFT_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raadventuregiftparam.hpp"
#include "item/itembase.h"

struct RAAdventureGiftItemCfg
{
	RAAdventureGiftItemCfg()
	{
		seq = 0;
		param1 = 0;
		param2 = 0;
		during_time = 0;

		is_broadcast = 0;
		consume_type = 0;
		consume_value = 0;
	}

	int seq;
	int param1;
	int param2;
	int during_time;

	int is_broadcast;
	int consume_type;
	int consume_value;
	ItemConfigData reward;
};

struct RAAdventureOhterCfg
{
	RAAdventureOhterCfg()
	{
		during_time = 0;
	}

	int during_time;
};

class RandActivityManager;

class RandActivityAdventureGiftConfig : public RandActivityConfig
{
public:
	RandActivityAdventureGiftConfig();
	virtual ~RandActivityAdventureGiftConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAAdventureGiftItemCfg * CheckAdventureGift(int type, int old_value, int curr_value) const ;
	const RAAdventureGiftItemCfg * GetAdventureGiftCfg(int seq) const ;
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitAdventureGiftCfg(TiXmlElement *RootElement);

	RAAdventureOhterCfg m_other_cfg;
	std::map<int, std::vector<RAAdventureGiftItemCfg> > m_gift_cfg;
};

#endif	//__RAND_ACTIVITY_ADVENTURE_GIFT_CONFIG_HPP__