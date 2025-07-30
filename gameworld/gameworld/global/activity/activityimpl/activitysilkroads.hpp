#ifndef __ACTIVITY_SILK_ROADS_HPP__
#define __ACTIVITY_SILK_ROADS_HPP__

#include "global/activity/activity.hpp"
#include "servercommon/silkroadsdef.hpp"
#include <set>

class Role;
class ActivitySilkRoads : public Activity
{
public:
	ActivitySilkRoads(ActivityManager *activity_manager);
	~ActivitySilkRoads();

	virtual void OnActivityStatusChange(int from_status, int to_status);
	virtual void OnUserLogin(Role *role);
	virtual void Update(unsigned long interval, time_t now_second);

	void OnReady(Role * role);											//准备,随机生成一些数据
	void OnStartOutSet(Role * role);									//开始出发
	void OnKeepGoing(Role * role);										//继续出发
	void OnBuyCommodity(Role * role, int commodity_seq, int count);		//购买货物 
	void OnSaleCommodity(Role * role, int commodity_seq, int count);	//售卖货物
	void OnBuyNews(Role * role);										//购买消息(打赏)
	void OnExpansionWarehouse(Role * role, int count);					//扩充仓库
	void OnBuySupply(Role * role, int count);							//购买补给
	void OnEnd(Role * role);											//旅途结束,结算

	void OnSendRoleAllInfo(Role * role);
	void OnSendAllRankInfo(Role * role);
	void OnSendAllTradingLogInfo(Role * role);

	bool FindReward(Role * role, int consume_type, int count);
private:
	
	void OnSendSimpleInfo(Role * role, int simple_type, int param1, int param2);
	void OnSendAssetsInfo(Role * role);
	void OnSendWarehouseGirdInfo(Role *role, short notice_type, const ActivitySilkRoadsRoleWarehouseData & warehouse_info, int grid_index);
	void OnSendSingleTradingLogInfo(Role * role, int trading_log_index, ActivitySilkRoadsRoleTradingLogData log_data);

	void OnActivityEnd();
	void OnActivityOpen();
	void OnUpdateRankList();
	bool OnGenerateCityInfo(int city_map_seq, ActivitySilkRoadsRoleCityData & city_data, std::vector<ActivitySilkRoadsRoleTradingLogData> * role_log_data = NULL, 
		const ActivitySilkRoadsRoleCityData * last_city_data = NULL);
	void OnGiveCityReward(Role * role, ActivitySilkRoadsRoleWarehouseData & role_warehouse_data, int city_map_seq);
	bool OnPutCommodity(Role * role, ActivitySilkRoadsRoleWarehouseData & role_warehouse_data, short commodity_seq, short commodity_count, int commodity_cost);
	bool OnConsumeCommodity(Role *role, ActivitySilkRoadsRoleWarehouseData & role_warehouse_data, short commodity_seq, short consume_count);

	std::map<int, ActivitySilkRoadsRoleData> m_role_data_map;					//缓存此时所有参与活动玩家信息
	std::map<int, ActivitySilkRoadsTradingLogData> m_role_trading_map;			//缓存此时参与玩家的贸易日志

	unsigned int m_next_refresh_timestamp;										//下一次刷新榜单的时间戳
	ActivitySilkRoadsRoleRankData m_rank_list[MAX_SILK_ROADS_RANK_NUM];			//此时的榜单
	bool is_change_flag;														
};


#endif