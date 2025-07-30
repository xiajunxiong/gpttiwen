#ifndef __RAND_ACTIVITY_LUCKY_BLIND_BOX_CONFIG_HPP__
#define __RAND_ACTIVITY_LUCKY_BLIND_BOX_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/activitydef.hpp"
#include "servercommon/roleactivity/raluckyblindboxparam.hpp"

struct LuckyBlindBoxBaseCfg
{
	LuckyBlindBoxBaseCfg() :type(0), section_start(0), section_end(0), seq(0), reward_group(0) {}

	int type;
	int	section_start;
	int	section_end;
	int	seq;
	int	reward_group;
};

struct LuckyBlindBoxShopCfg
{
	LuckyBlindBoxShopCfg() :type(0), section_start(0), section_end(0), seq(0), limit_type(LIMIT_TYPE_ENUM_0), limit_times(0), price(0), price_type(0), cfg_or(0) {}
	enum LIMIT_TYPE_ENUM
	{
		LIMIT_TYPE_ENUM_0 = 0,//无限制
		LIMIT_TYPE_ENUM_1 = 1,//活动限购
		LIMIT_TYPE_ENUM_2 = 2,//每日限购
	};
	int type;
	int	section_start;
	int	section_end;
	int	seq;
	int	limit_type;
	int	limit_times;
	int	price;
	int price_type;
	int	cfg_or;
	std::vector<ItemConfigData> reward_vec;
};

struct LuckyBlindBoxRechargeCfg
{
	LuckyBlindBoxRechargeCfg() :type(0), section_start(0), section_end(0), seq(0), add_recharge(0), get_type(LIMIT_TYPE_ENUM_0), get_times(0){}
	enum LIMIT_TYPE_ENUM
	{
		LIMIT_TYPE_ENUM_0 = 0,//无限制
		LIMIT_TYPE_ENUM_1 = 1,//活动限购
		LIMIT_TYPE_ENUM_2 = 2,//每日限购
	};
	int type;
	int	section_start;
	int	section_end;
	int	seq;
	int	add_recharge;
	int get_type;
	int	get_times;
	std::vector<ItemConfigData> reward_vec;
};

struct LuckyBlindBoxRewardCfg
{
	LuckyBlindBoxRewardCfg() :seq(0), rate(0), guarantee_times(0), hearsay(0) {}

	int seq;
	int	rate;
	int guarantee_times;
	int hearsay;
	ItemConfigData reward;
};

struct LuckyBlindBoxRewardListCfg
{
	LuckyBlindBoxRewardListCfg() : reward_group(0), weight(0) {}
	int reward_group;
	int weight;
	std::vector<LuckyBlindBoxRewardCfg> group_vec;
};

struct LuckyBlindBoxOtherCfg
{
	LuckyBlindBoxOtherCfg() :cfg_ver(0) {}
	int cfg_ver;
	ItemConfigData cost_item;
};

class RandActivityManager;
class RandActivityLuckyBlindBoxConfig : public RandActivityConfig
{
public:
	RandActivityLuckyBlindBoxConfig();
	virtual ~RandActivityLuckyBlindBoxConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	virtual int GetConfigVer() const;
	const LuckyBlindBoxBaseCfg * GetBaseCfg(int open_day)const;
	const LuckyBlindBoxShopCfg * GetShopCfg(int open_day, int seq)const;
	const LuckyBlindBoxRechargeCfg * GetRechargeCfg(int open_day, int seq)const;
	const LuckyBlindBoxRewardCfg * RandRewardCfg(int reward_group, int check_floor, BitMap<MAX_LUCKY_BLIND_BOX_REWARD_GROUP_NUM>* bit_map, bool *floor_back)const;
	const LuckyBlindBoxOtherCfg & GetOtherCfg()const { return other_cfg; }
private:
	int InitBaseCfg(TiXmlElement *RootElement);
	int InitShopCfg(TiXmlElement *RootElement);
	int InitRechargeCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	LuckyBlindBoxOtherCfg other_cfg;
	std::vector<LuckyBlindBoxBaseCfg> base_cfg_vec;
	std::vector<LuckyBlindBoxShopCfg> shop_cfg_vec;
	std::vector<LuckyBlindBoxRechargeCfg> recharge_cfg_vec;

	std::map<int, std::map<int, LuckyBlindBoxRewardCfg*> > reward_floor_map;//key1:奖励组，key2：保底次数
	std::map<int, LuckyBlindBoxRewardListCfg> reward_cfg_map;//key1：奖励组
};

#endif
