#ifndef __ACTIVITY_SILK_ROADS_CONFIG_HPP__
#define __ACTIVITY_SILK_ROADS_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"

#include <set>

struct ActivitySilkRoadsOtherCfg
{
	ActivitySilkRoadsOtherCfg() : buy_bag(0), buy_supply(0), act_num(0), start_bag(0), start_coin(0), start_supply(0), city_reward_group_rate(0), 
	start_city_seq(0), start_supply_num(0), city_max_news(0), find_coin(0)
	{}

	int buy_bag;					//单次背包扩容数量
	int buy_supply;					//单次补给购买数量
	int act_num;					//每次活动次数
	int start_bag;					//默认初始格子数量
	int start_coin;					//默认初始货币数量
	int start_supply;				//默认初始补给数量
	int city_reward_group_rate;		//进入城镇给奖励的概率 万分比
	int start_city_seq;				//起点城市
	int start_supply_num;			//初始补给消耗
	int city_max_news;				//单个城镇能产生的消息数量
	int find_coin;					//找回铜币奖励
	ItemConfigData find_reward;		//找回物品奖励
};

struct ActivitySilkRoadsCityCfg
{
	ActivitySilkRoadsCityCfg() : city_map_seq(0), city_reward_group_id(0)
	{}

	int city_map_seq;					//对应地图序号
	int city_reward_group_id;			//进入城镇的奖励组ID
	std::set<int> city_specialty_list;	//城镇的特产列表
	std::vector<int> double_price_list;	//物品大涨概率	
};

struct ActivitySilkRoadsCommodityCfg
{
	static const int MAX_COMMODITY_BASE_NUM = 100;		
	ActivitySilkRoadsCommodityCfg() :city_specialty_seq(0), freight_market_price(0), min_price(0), max_price(0),min_price_up(0), max_price_up(0), lowest_price(0)
	{}

	int city_specialty_seq;					//货物序号
	int freight_market_price;				//货物原价(市场价)
	int min_price;							//最低价浮动(百分比)
	int max_price;							//最高价浮动(百分比)
	int min_price_up;						//大涨增幅最低百分比
	int max_price_up;						//大涨增幅最高百分比
	int lowest_price;						//最低价
};

struct ActivitySilkRoadsLineCfg
{
	ActivitySilkRoadsLineCfg(): line_group_id(0)
	{}

	int line_group_id;
	std::vector<int> line_vec;		//线路顺序
};

struct ActivitySilkRoadsSettlementBoxCfg
{
	ActivitySilkRoadsSettlementBoxCfg() : levels(0), num_end(0), coin(0)
	{}

	int levels;	
	int num_end;						//结算时的货币数量
	int coin;							//铜币奖励
	ItemConfigData reward_item;			//奖励物品
};

enum  ACTIVITY_SILK_ROADS_SKILL_TYPE
{
	ACTIVITY_SILK_ROADS_SKILL_TYPE_BEGIN = 0,	
	ACTIVITY_SILK_ROADS_SKILL_TYPE_WAREHOUSE = 1,	//货物满仓 额外赠送仓位X
	ACTIVITY_SILK_ROADS_SKILL_TYPE_FUNDING = 2,		//富家子弟 初始资金增加X
	ACTIVITY_SILK_ROADS_SKILL_TYPE_TRADING = 3,		//贸易天才 旅途补给消耗减少X%
	ACTIVITY_SILK_ROADS_SKILL_TYPE_NEWS = 4,		//面容和善 打赏消耗减少10%

	ACTIVITY_SILK_ROADS_SKILL_TYPE_MAX
};

struct ActivitySilkRoadsSkillCfg
{
	ActivitySilkRoadsSkillCfg() : skill_id(0), skill_type(0), skill_roads_skill_num(0)
	{}

	int skill_id;						//技能ID
	int skill_type;						//技能类型
	int skill_roads_skill_num;			//对应类型数值
};

struct ActivitySilkRoadsCityRewardCfg
{
	ActivitySilkRoadsCityRewardCfg() : rate(0), commodity_count(0)
	{}

	int rate;
	std::set<int> reward_commodity_list;		//奖励的货物列表
	int commodity_count;						//货物数量
};

class  ActivitySilkRoadsConfig : public ILogicConfig
{
public:
	ActivitySilkRoadsConfig();
	virtual ~ActivitySilkRoadsConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ActivitySilkRoadsOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const ActivitySilkRoadsCityCfg * GetCityCfg(int city_map_seq);
	const ActivitySilkRoadsCommodityCfg* GetCommodityCfg(int commodity_seq);
	const std::vector<ActivitySilkRoadsCommodityCfg> & GetCommodityList() { return m_commodity_list; }
	const ActivitySilkRoadsLineCfg * GetLineCfg(int group);
	const ActivitySilkRoadsLineCfg * GetRandLineCfg();
	int GetRandFreightId(int type);
	int GetBuyWarehousePrice(int buy_times);
	int GetMaxBuyWarehouseTimes();
	int GetWarehouseExpend(int warehouse_grid_count);
	int GetBuySupplyPrice(int buy_times);
	int GetMaxBuySupplyTimes();
	int GetBuyNewsPrice(int buy_times);
	int GetMaxBuyNewsTimes();

	const ActivitySilkRoadsSettlementBoxCfg * GetSettlementBoxCfg(int coin_count);
	const ItemConfigData * GetRankReward(int rank);
	const ActivitySilkRoadsSkillCfg * GetSkillCfg(int skill_id);
	const ActivitySilkRoadsSkillCfg * GetRandSkillCfg();
	const ActivitySilkRoadsCityRewardCfg * GetRandCityRewardByCity(int city_map_seq);
	const ActivitySilkRoadsCityRewardCfg * GetRandCityReward(int reward_group_id);

private:
	int InitOtherCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitCityCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitCommodityCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitShoppingCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitLineCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitBuyWarehouseCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitWarehouseExpendCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitBuySupplyPeiceCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitBuyNewsCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitSettlementBoxCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitRankListCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitRoadsSkillCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitCityRewardCfg(TiXmlElement *RootElement, std::string& errormsg);

	ActivitySilkRoadsOtherCfg m_other_cfg;
	std::map<int, ActivitySilkRoadsCityCfg> m_city_map;		//城镇配置 key:city_map_seq
	std::vector<ActivitySilkRoadsCommodityCfg> m_commodity_list;	//货物配置 key:seq
	std::map<int, ActivitySilkRoadsLineCfg> m_line_map;		//线路图配置 key:group
	std::set<int> m_shopping_list[SILK_ROADS_LOG_TYPE_MAX];		//收购信息配置 

	std::map<int, int> m_buy_warehouse_pirce_map;			//仓库扩充价格配置 key:购买次数 second:价格
	std::map<int, int> m_warehouse_expend_map;				//仓库格子对应补给消耗配置 key:格子数量 second:补给价格
	std::map<int, int> m_buy_supply_peice_map;				//补给购买价格配置 key:购买次数 second:补给价格
	std::map<int, int> m_buy_news_map;						//购买消息价格配置 key:购买次数 second:价格

	std::map<int, ActivitySilkRoadsSettlementBoxCfg> m_settlement_box_map;		// 结算奖励 key:货币数量
	std::map<ParamSection, ItemConfigData> m_rank_reward_map;		// 排行榜奖励 key:排名区间
	std::map<int, ActivitySilkRoadsSkillCfg> m_skill_map;			// 技能效果 key:技能ID
	std::map<int, std::vector<ActivitySilkRoadsCityRewardCfg> > m_city_reward_cfg;	//进入城镇奖励组 key:奖励组ID

};


#endif