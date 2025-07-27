#ifndef __SILK_ROADS_DEF_HPP__
#define __SILK_ROADS_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MAX_SILK_ROADS_LINE_NUM = 20;					//路线图经过城镇最大值
static const int MAX_SILK_ROADS_CITY_COMMODITY_NUM = 20;		//城镇中特产最大值
static const int MAX_SILK_ROADS_GRID_NUM = 350;					//仓库格子最大值
static const int MAX_SILK_ROADS_RANK_NUM = 100;					//榜单人数限制
static const int MAX_SILK_ROADS_REFRESH_NUM = 60;				//榜单刷新时间间隔
static const int MAX_SILK_ROADS_TRADING_LOG_NUM	= 100;			//角色贸易日志最大值

static const int MAX_SILK_ROADS_REWAD_RATE_COUNT = 10000;		

//角色城镇中的信息
struct ActivitySilkRoadsRoleCityData
{
	ActivitySilkRoadsRoleCityData() { this->Reset(); }
	
	struct CommodityItem
	{
		CommodityItem() { this->Reset(); }

		void Reset()
		{
			commodity_seq = 0;
			price = 0;
		}
		
		short commodity_seq;			//货物序号
		unsigned short price;			//货物价格
	};

	void Reset()
	{
		reserve_sh = 0;
		cur_city_buy_news_count = 0;
		city_map_seq = 0;
		commondity_count = 0;
		for (int i = 0; i < ARRAY_LENGTH(commodity_list); i++)
		{
			commodity_list[i].Reset(); 
		}
	}

	void GetCommodity(int commodity_seq, int & price)
	{
		for (int i = 0; i < commondity_count && i < ARRAY_LENGTH(commodity_list); i++)
		{
			if (commodity_seq == commodity_list[i].commodity_seq)
			{
				price = (int)commodity_list[i].price;
				break;
			}
		}
	}

	short reserve_sh;
	short cur_city_buy_news_count;										//该城镇购买消息次数
	short city_map_seq;													//该城镇对应地图序号
	short commondity_count;												//货物数量
	CommodityItem commodity_list[MAX_SILK_ROADS_CITY_COMMODITY_NUM];	//该城镇的特产列表
};

enum SILK_ROADS_LOG_TYPE
{
	SILK_ROADS_LOG_TYPE_BEGIN = 0,
	SILK_ROADS_LOG_TYPE_PRICE_UP = 1,			//普通涨幅
	SILK_ROADS_LOG_TYPE_DOUBLE_PRICE_UP = 2,	//大涨

	SILK_ROADS_LOG_TYPE_MAX
};
UNSTD_STATIC_CHECK(SILK_ROADS_LOG_TYPE_MAX < MAX_INT16);

//角色贸易日志
struct ActivitySilkRoadsRoleTradingLogData
{
	ActivitySilkRoadsRoleTradingLogData() { this->Reset(); }

	void Reset()
	{
		log_type = SILK_ROADS_LOG_TYPE_BEGIN;
		freight_id = 0;
		from_city_map_seq = 0;
		target_city_map_seq = 0;
		commodity_seq = 0;
		reserve_sh = 0;
		price = 0;
	}
	short log_type;
	short freight_id;			//收购目标ID
	short from_city_map_seq;	//消息来源城镇地图序号
	short target_city_map_seq;	//目标城镇地图序号
	short commodity_seq;		//货物序号
	short reserve_sh;
	int price;					//价格
};

struct ActivitySilkRoadsRoleWarehouseData
{
	ActivitySilkRoadsRoleWarehouseData() { this->Reset(); }

	void Reset()
	{
		cur_max_grid_count = 0;
		cur_has_commodity_count = 0;
		for (int i = 0; i < ARRAY_LENGTH(grid_list); ++i)
		{
			grid_list[i].Reset();
		}
	}

	struct GridInfo
	{
		GridInfo() { this->Reset(); }
		void Reset()
		{
			commodity_seq = 0;
			commodity_count = 0;
			commodity_cost = 0;
		}
		
		short commodity_seq;		//货物序号
		short commodity_count;		//货物数量
		float commodity_cost;		//货物成本
	};

	short cur_max_grid_count;		//当前所能使用的数量
	short cur_has_commodity_count;	//当前已有的货物数量
	GridInfo grid_list[MAX_SILK_ROADS_GRID_NUM];		//记录的时候货物虽然堆叠了,但是实际占的一个货物占一个格子
};

struct ActivitySilkRoadsRoleData
{
	ActivitySilkRoadsRoleData() { this->Reset(); }

	void Reset()
	{
		cumulation_coin_count = 0;
		cumulation_trading_count = 0;
		finish_num = 0;
		line_group_id = 0;
		buy_warehouse_count = 0;
		buy_supply_count = 0;
		reserve_sh = 0;
		have_coin_count = 0;
		have_supply_count = 0;
		init_skill = 0;
		cur_city_index = -1;
		trading_num = 0;
		city_count = 0;
		warehouse_data.Reset();
		for (int i = 0; i < ARRAY_LENGTH(city_list); i++)
		{
			city_list[i].Reset();
		}
	}
	void FinishReset()
	{
		line_group_id = 0;
		buy_warehouse_count = 0;
		buy_supply_count = 0;
		reserve_sh = 0;
		have_coin_count = 0;
		have_supply_count = 0;
		init_skill = 0;
		cur_city_index = -1;
		trading_num = 0;
		city_count = 0;
		warehouse_data.Reset();
		for (int i = 0; i < ARRAY_LENGTH(city_list); i++)
		{
			city_list[i].Reset();
		}
	}

	int cumulation_coin_count;		//本次活动累积的货币数量 (计入时刻:完成次数)
	int cumulation_trading_count;	//本次活动累积的贸易次数
	int finish_num;					//本次活动完成次数

	short line_group_id;			//玩家本次选择的路线图ID	
	short buy_warehouse_count;		//仓库扩充次数
	short buy_supply_count;			//补给购买次数
	short reserve_sh;	
	int have_coin_count;			//当前拥有的货币
	int have_supply_count;			//当前拥有的补给
	short init_skill;				//初始技能
	short cur_city_index;			//当前所在的城镇(index为下面数组)
	short trading_num;				//贸易次数
	short city_count;				//该路线城市数量
	ActivitySilkRoadsRoleWarehouseData warehouse_data;					//仓库信息
	ActivitySilkRoadsRoleCityData city_list[MAX_SILK_ROADS_LINE_NUM];	//玩家选择的路线图中每个城镇的信息
};
typedef char ActivitySilkRoadsRoleDataHex[sizeof(ActivitySilkRoadsRoleData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ActivitySilkRoadsRoleDataHex) <= 10240);

struct ActivitySilkRoadsRoleRankData
{
	ActivitySilkRoadsRoleRankData(int _role_id, int _cumulation_coin_count, int _cumulation_trading_count, int _finish_num) 
		: role_id(_role_id), cumulation_coin_count(_cumulation_coin_count), cumulation_trading_count(_cumulation_trading_count), finish_num(_finish_num) {}
	ActivitySilkRoadsRoleRankData() { this->Reset(); }
	
	void Reset()
	{
		role_id = 0;
		cumulation_coin_count = 0;
		cumulation_trading_count = 0;
		finish_num = 0;
	}

	int role_id;
	int cumulation_coin_count;		//本次活动累积的货币数量 (计入时刻:完成次数)
	int cumulation_trading_count;	//本次活动累积的贸易次数
	int finish_num;					//本次活动完成次数
};

struct ActivitySilkRoadsRoleRankInfo
{
	ActivitySilkRoadsRoleRankInfo() { this->Reset(); }
	ActivitySilkRoadsRoleRankInfo(const ActivitySilkRoadsRoleRankData & rank_info)
	{
		role_id = rank_info.role_id;
		memset(role_name, 0, sizeof(role_name));
		avatar_type = 0;
		headshot_id = 0;
		cumulation_coin_count = rank_info.cumulation_coin_count;
		cumulation_trading_count = rank_info.cumulation_trading_count;
	}

	void Reset()
	{
		role_id = 0;
		memset(role_name, 0, sizeof(role_name));
		avatar_type = 0;
		headshot_id = 0;
		cumulation_coin_count = 0;
		cumulation_trading_count = 0;	
		
	}

	int role_id;
	GameName role_name;
	short avatar_type;				// 虚拟人物类型
	short headshot_id;				// 头像ID 
	int cumulation_coin_count;		// 本次活动累积的货币数量
	int cumulation_trading_count;	// 本次活动累积的贸易次数
};

#pragma pack(pop)

struct ActivitySilkRoadsTradingLogData
{
	ActivitySilkRoadsTradingLogData() { this->Reset(); }

	void Reset()
	{
		city_log_list.clear();
		buy_count = 0;
		for (int i = 0; i < ARRAY_LENGTH(buy_city_log_list); ++i)
		{
			buy_city_log_list[i].Reset();
		}
	}

	std::vector<ActivitySilkRoadsRoleTradingLogData> city_log_list;		//一开始生成的所有消息列表
	int buy_count;
	ActivitySilkRoadsRoleTradingLogData buy_city_log_list[MAX_SILK_ROADS_TRADING_LOG_NUM]; 	//玩家购买的消息列表
};


#endif