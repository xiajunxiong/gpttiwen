#ifndef __RAND_ACTIVITY_MONPOLY_GIFT_CONFIG_HPP__
#define __RAND_ACTIVITY_MONPOLY_GIFT_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itemextern.hpp"
#include "item/itembase.h"
#include <map>
#include <vector>
#include "servercommon/roleactivity/racowparam.hpp"


struct RAMonpolyGiftMapCfg
{
	RAMonpolyGiftMapCfg() : type(0), seq(0), layer_number(0), layer_number_type(0), parameter(0), exp(0), reward_rate(0){}

	enum MAP_TYPE
	{
		MAP_TYPE_MIN = 0,
		MAP_TYPE_FIGHT,		//战斗
		MAP_TYPE_FORWARD,	//再次行动		+，前进；-，后退
		MAP_TYPE_ANSWER,	//答题
		MAP_TYPE_NULL,		//无操作
		MAP_TYPE_GIFT,		//礼包
		MAP_TYPE_NEXT_MAP,	//下一层
		MAP_TYPE_MAX,
	};
	const static int EXP_PERSENT = 10000;//万分比角色经验
	int type;
	int seq;
	int layer_number;
	int layer_number_type;
	int parameter;

	int exp;
	int reward_rate;
	std::vector<ItemConfigData> reward_vec;
};

struct RAMonpolyGiftMapSectionCfg
{
	RAMonpolyGiftMapSectionCfg(int _section_start, int _section_end) : section_start(_section_start), section_end(_section_end) {}

	int section_start;
	int section_end;
	RAMonpolyGiftMapCfg play_map[MAX_MAP_COUNT][MAX_ONE_MAP_INDEX];
};

struct RAMonpolyGiftRewardCfg
{
	RAMonpolyGiftRewardCfg() : type(0),rate(0) {}

	int type;
	int rate;
	ItemConfigData reward;
};

struct RAMonpolyGiftOtherCfg
{
	RAMonpolyGiftOtherCfg() : points1(0), rate1(0), points2(0), rate2(0), points3(0), rate3(0), rate_count(0), cost_item(0) {}

	int points1;
	int	rate1;
	int	points2;
	int	rate2;
	int	points3;
	int	rate3;
	int	rate_count;
	int	cost_item;
};

struct RAMonpolyGiftExchangeCfg
{
	RAMonpolyGiftExchangeCfg() : item_seq(0), today_times(0){}

	int item_seq;
	int today_times;

	ItemConfigData exchange_item;
	std::vector<ItemConfigData> reward_vec;
};

class RandActivityManager;

class RandActivityMonpolyGiftConfig : public RandActivityConfig
{
public:
	RandActivityMonpolyGiftConfig();
	virtual ~RandActivityMonpolyGiftConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAMonpolyGiftMapCfg * GetMapInfoCfg(RandActivityManager * ramgr, int map_id,int map_index) const;
	const RAMonpolyGiftRewardCfg * GetRewardCfg(int reward_group_id) const;
	const RAMonpolyGiftOtherCfg & GetOtherCfg() const { return m_other_cfg; }
	const int GetRandPoint()const;
	const RAMonpolyGiftExchangeCfg * GetExchangeCfg(int item_seq) const;
private:
	int InitMonpolyGiftCfg(TiXmlElement *RootElement);
	int InitMonpolyGiftRewardCfg(TiXmlElement *RootElement);
	int InitMonpolyGiftExchangeCfg(TiXmlElement *RootElement);
	int InitMonpolyGiftOhterCfg(TiXmlElement *RootElement);

	RAMonpolyGiftOtherCfg m_other_cfg;

	std::vector<RAMonpolyGiftMapSectionCfg> m_play_section_cfg;

	//地图信息
	int weight_count;
	std::map<int, RAMonpolyGiftRewardCfg> m_index_map;
	std::map<int, RAMonpolyGiftExchangeCfg> m_exchange_map;

};

#endif	//__RAND_ACTIVITY_GIVE_PET_DRAW_CONFIG_HPP__