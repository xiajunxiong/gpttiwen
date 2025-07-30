#include "activitysilkroads.hpp"
#include "gamelog.h"

#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/silkroads/activitysilkroadsconfig.hpp"

#include "global/usercache/usercache.hpp"

#include "obj/character/role.h"
#include "other/route/mailroute.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "global/worldstatus/worldstatus.hpp"


ActivitySilkRoads::ActivitySilkRoads(ActivityManager * activity_manager)
	:Activity(activity_manager, ACTIVITY_TYPE_SILK_ROADS), m_next_refresh_timestamp(0), is_change_flag(false)
{
	m_role_data_map.clear();
	m_role_trading_map.clear();
	for (int i = 0; i < ARRAY_LENGTH(m_rank_list); i++)
	{
		m_rank_list[i].Reset();
	}
}

ActivitySilkRoads::~ActivitySilkRoads()
{
}

bool SortRank(const ActivitySilkRoadsRoleRankData & a, const ActivitySilkRoadsRoleRankData & b)
{
	if (a.cumulation_coin_count != b.cumulation_coin_count) return a.cumulation_coin_count > b.cumulation_coin_count;

	if (a.cumulation_trading_count != b.cumulation_trading_count) return a.cumulation_trading_count < b.cumulation_trading_count;

	return a.role_id < b.role_id;
}

void ActivitySilkRoads::OnActivityStatusChange(int from_status, int to_status)
{
	Activity::OnActivityStatusChange(from_status, to_status);
	if (!CrossConfig::Instance().IsHiddenServer())	//只能在跨服
	{
		return;
	}
	switch (to_status)
	{
		case ACTIVITY_STATUS_OPEN:
		{
			this->OnActivityOpen();
		}
		break;
		case ACTIVITY_STATUS_CLOSE:
		{
			this->OnActivityEnd();
		}
		break;
	}
}

void ActivitySilkRoads::OnUserLogin(Role * role)
{	
	if(NULL == role) return;

	Activity::OnUserLogin(role);
	if (!CrossConfig::Instance().IsHiddenServer())	//只能在跨服
	{
		return;
	}
	if (this->IsActivityOpen() && m_role_data_map.end() != m_role_data_map.find(role->GetUID()))
	{
		this->OnSendRoleAllInfo(role);
	}
}

void ActivitySilkRoads::Update(unsigned long interval, time_t now_second)
{
	Activity::Update(interval, now_second);

	if (!CrossConfig::Instance().IsHiddenServer() || !this->IsActivityOpen())
	{
		return;
	}
	
	if (is_change_flag && 0 != m_next_refresh_timestamp && m_next_refresh_timestamp <= (unsigned int)now_second)
	{
		this->OnUpdateRankList();
	}
}

void ActivitySilkRoads::OnReady(Role * role)
{
	if (!this->IsActivityOpen() || NULL == role)
	{
		return;
	}
	const ActivitySilkRoadsOtherCfg & other_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetOtherCfg();
	ActivitySilkRoadsRoleData & role_data = m_role_data_map[role->GetUID()];
	ActivitySilkRoadsTradingLogData & role_log_data = m_role_trading_map[role->GetUID()];
	if (role_data.finish_num >= other_cfg.act_num)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_FINISH_NUM_LIMIT);
		return;
	}
	if (role_data.line_group_id > 0)
	{
		return;
	}
	role_data.FinishReset();
	//随机路线图与技能
	const ActivitySilkRoadsLineCfg * line_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetRandLineCfg();
	const ActivitySilkRoadsSkillCfg * skill_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetRandSkillCfg();
	if (NULL == line_cfg || NULL == skill_cfg || line_cfg->line_group_id <= 0)
	{
		return;
	}

	role_data.line_group_id = line_cfg->line_group_id;
	//生成路线旅途的城镇信息
	if (!this->OnGenerateCityInfo(other_cfg.start_city_seq, role_data.city_list[0]))	//第一个城镇不生成消息
	{
		return;
	}
	int city_index = 1;
	int last_city_index = city_index - 1;
	for (int i = 0; city_index < ARRAY_LENGTH(role_data.city_list) && i < (int)line_cfg->line_vec.size(); i++)
	{
		if(last_city_index < 0 || last_city_index >= ARRAY_LENGTH(role_data.city_list)) break;

		if (!this->OnGenerateCityInfo(line_cfg->line_vec[i], role_data.city_list[city_index], &role_log_data.city_log_list, &role_data.city_list[last_city_index]))
		{
			continue;
		}
		last_city_index = city_index;
		city_index++;
	}
	role_data.cur_city_index = -1;
	role_data.city_count = city_index;		
	role_data.init_skill = skill_cfg->skill_id;
	role_data.warehouse_data.cur_max_grid_count = other_cfg.start_bag;
	role_data.have_supply_count = other_cfg.start_supply;
	role_data.have_coin_count = other_cfg.start_coin;
	switch (skill_cfg->skill_type)
	{
		case ACTIVITY_SILK_ROADS_SKILL_TYPE_WAREHOUSE:
		{
			role_data.warehouse_data.cur_max_grid_count += skill_cfg->skill_roads_skill_num;
		}
		break;
		case ACTIVITY_SILK_ROADS_SKILL_TYPE_FUNDING:
		{
			role_data.have_coin_count += skill_cfg->skill_roads_skill_num;
		}
		break;
		default:
		break;
	}

	this->OnSendSimpleInfo(role, Protocol::SCActivitySilkRoadsSimpleInfo::SILK_ROADS_SIMPLE_TYPE_READY, role_data.init_skill, role_data.line_group_id);
	this->OnSendRoleAllInfo(role);
	gamelog::g_log_activity_silk_roads.printf(LL_INFO, "%s user[%d, %s] [coin:%d, trading:%d] finish_num[%d] line_group_id[%d] skill_id[%d]", __FUNCTION__, role->GetUID(), 
		role->GetName(), role_data.cumulation_coin_count, role_data.cumulation_trading_count, role_data.finish_num, role_data.line_group_id, role_data.init_skill);
}

void ActivitySilkRoads::OnStartOutSet(Role * role)
{
	if (!this->IsActivityOpen() || NULL == role)
	{
		return;
	}
	std::map<int, ActivitySilkRoadsRoleData>::iterator it = m_role_data_map.find(role->GetUID());
	if (it == m_role_data_map.end()) return;

	const ActivitySilkRoadsOtherCfg & other_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetOtherCfg();
	ActivitySilkRoadsRoleData & role_data = it->second;
	if (role_data.finish_num >= other_cfg.act_num)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_FINISH_NUM_LIMIT);
		return;
	}
	if (-1 != role_data.cur_city_index)
	{
		return;
	}
	role_data.cur_city_index = 0;
	role_data.trading_num += 1;
	this->OnGiveCityReward(role, role_data.warehouse_data, role_data.city_list[role_data.cur_city_index].city_map_seq);
	this->OnSendSimpleInfo(role, SCActivitySilkRoadsSimpleInfo::SILK_ROADS_SIMPLE_TYPE_START_OUTSET, role_data.cur_city_index, 0);
	EventHandler::Instance().OnParticipateLimitActivity(role, ACTIVITY_TYPE_SILK_ROADS, __FUNCTION__);
}

void ActivitySilkRoads::OnKeepGoing(Role * role)
{
	if (!this->IsActivityOpen() || NULL == role)
	{
		return;
	}
	std::map<int, ActivitySilkRoadsRoleData>::iterator it = m_role_data_map.find(role->GetUID());
	if (it == m_role_data_map.end()) return;

	const ActivitySilkRoadsOtherCfg & other_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetOtherCfg();
	ActivitySilkRoadsRoleData & role_data = it->second;
	ActivitySilkRoadsTradingLogData & role_log_data = m_role_trading_map[role->GetUID()];
	if (role_data.finish_num >= other_cfg.act_num)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_FINISH_NUM_LIMIT);
		return;
	}
	//还没出发
	if (role_data.cur_city_index < 0)
	{
		return;
	}
	//到达终点,此时无法再继续出发,只能选择结束
	if (role_data.cur_city_index + 1 >= role_data.city_count || role_data.cur_city_index + 1 >= ARRAY_LENGTH(role_data.city_list))	
	{
		return;
	}
	//先判断补给,有足够的补给才能移动
	int need_consume_supply = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetWarehouseExpend(role_data.warehouse_data.cur_has_commodity_count) + other_cfg.start_supply_num;
	const ActivitySilkRoadsSkillCfg * skill_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetSkillCfg(role_data.init_skill);
	if (NULL != skill_cfg && ACTIVITY_SILK_ROADS_SKILL_TYPE_TRADING == skill_cfg->skill_type)
	{
		need_consume_supply -= (int)((1LL * need_consume_supply * skill_cfg->skill_roads_skill_num) / 100);
	}
	if (role_data.have_supply_count < need_consume_supply)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_SUPPLY_NOT_ENOUGH);
		return;
	}
	
	role_data.have_supply_count -= need_consume_supply;
	role_data.cur_city_index += 1;
	//删除当前城镇的生成的消息列表 这里是由于消息生成是根据路线图顺序,这里移动也是,所以前面几个只有可能是当前城市(或者当前城市的消息已经被购买完了)
	for (std::vector<ActivitySilkRoadsRoleTradingLogData>::iterator log_it = role_log_data.city_log_list.begin(); log_it != role_log_data.city_log_list.end();)
	{
		if (role_data.city_list[role_data.cur_city_index].city_map_seq == log_it->target_city_map_seq)
		{
			log_it = role_log_data.city_log_list.erase(log_it);
		}
		else
		{
			break;
		}
	}

	role_data.trading_num += 1;
	this->OnGiveCityReward(role, role_data.warehouse_data, role_data.city_list[role_data.cur_city_index].city_map_seq);
	this->OnSendSimpleInfo(role, SCActivitySilkRoadsSimpleInfo::SILK_ROADS_SIMPLE_TYPE_KEEP_GOING, role_data.cur_city_index, (int)role_log_data.city_log_list.size());
	this->OnSendAssetsInfo(role);
}

void ActivitySilkRoads::OnBuyCommodity(Role * role, int commodity_seq, int count)
{
	if (!this->IsActivityOpen() || NULL == role || count <= 0)
	{
		return;
	}
	const ActivitySilkRoadsCommodityCfg * commodity_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetCommodityCfg(commodity_seq);
	if (NULL == commodity_cfg)
	{
		return;
	}
	std::map<int, ActivitySilkRoadsRoleData>::iterator it = m_role_data_map.find(role->GetUID());
	if (it == m_role_data_map.end()) return;

	const ActivitySilkRoadsOtherCfg & other_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetOtherCfg();
	ActivitySilkRoadsRoleData & role_data = it->second;
	if (role_data.finish_num >= other_cfg.act_num)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_FINISH_NUM_LIMIT);
		return;
	}
	//还没出发
	if (role_data.cur_city_index < 0 || role_data.cur_city_index >= ARRAY_LENGTH(role_data.city_list))
	{
		return;
	}
	if (role_data.warehouse_data.cur_has_commodity_count + count > role_data.warehouse_data.cur_max_grid_count)	//仓库没有空位
	{
		role->NoticeNum(errornum::EN_STORAGE_SPACE_NOT_ENOUGH);
		return;
	}

	int price = commodity_cfg->freight_market_price;
	ActivitySilkRoadsRoleCityData & city_data = role_data.city_list[role_data.cur_city_index];
	city_data.GetCommodity(commodity_seq, price);
	int price_count = price * count;
	if (role_data.have_coin_count < price_count)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_MONEY_NOT_ENOUGH);
		return;
	}
	if (!this->OnPutCommodity(role, role_data.warehouse_data, commodity_seq, count, price))
	{
		role->NoticeNum(errornum::EN_STORAGE_SPACE_NOT_ENOUGH);
		return;
	}

	role_data.have_coin_count -= price_count;
	this->OnSendAssetsInfo(role);
}

void ActivitySilkRoads::OnSaleCommodity(Role * role, int commodity_seq, int count)
{
	if (!this->IsActivityOpen() || NULL == role || count <= 0)
	{
		return;
	}
	const ActivitySilkRoadsCommodityCfg * commodity_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetCommodityCfg(commodity_seq);
	if (NULL == commodity_cfg)
	{
		return;
	}
	std::map<int, ActivitySilkRoadsRoleData>::iterator it = m_role_data_map.find(role->GetUID());
	if (it == m_role_data_map.end()) return;

	const ActivitySilkRoadsOtherCfg & other_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetOtherCfg();
	ActivitySilkRoadsRoleData & role_data = it->second;
	if (role_data.finish_num >= other_cfg.act_num)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_FINISH_NUM_LIMIT);
		return;
	}
	//还没出发
	if (role_data.cur_city_index < 0 || role_data.cur_city_index >= ARRAY_LENGTH(role_data.city_list))
	{
		return;
	}
	//此时没有货物
	if (role_data.warehouse_data.cur_has_commodity_count <= 0)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_SALE_COMMODITY_FAILED);
		return;
	}
	int price = commodity_cfg->freight_market_price;
	ActivitySilkRoadsRoleCityData & city_data = role_data.city_list[role_data.cur_city_index];
	city_data.GetCommodity(commodity_seq, price);
	int price_count = price * count;
	if (!this->OnConsumeCommodity(role, role_data.warehouse_data, commodity_seq, count))
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_SALE_COMMODITY_FAILED);
		return;
	}

	role_data.have_coin_count += price_count;
	this->OnSendAssetsInfo(role);
}

void ActivitySilkRoads::OnBuyNews(Role * role)
{
	if (!this->IsActivityOpen() || NULL == role)
	{
		return;
	}
	std::map<int, ActivitySilkRoadsRoleData>::iterator it = m_role_data_map.find(role->GetUID());
	if (it == m_role_data_map.end()) return;

	const ActivitySilkRoadsOtherCfg & other_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetOtherCfg();
	ActivitySilkRoadsRoleData & role_data = it->second;
	ActivitySilkRoadsTradingLogData & role_log_data = m_role_trading_map[role->GetUID()];
	if (role_data.finish_num >= other_cfg.act_num)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_FINISH_NUM_LIMIT);
		return;
	}
	//还没出发
	if (role_data.cur_city_index < 0 || role_data.cur_city_index >= ARRAY_LENGTH(role_data.city_list))
	{
		return;
	}
	ActivitySilkRoadsRoleCityData & city_data = role_data.city_list[role_data.cur_city_index];
	int max_buy_times = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetMaxBuyNewsTimes();
	if (city_data.cur_city_buy_news_count >= max_buy_times)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_BUY_TRADING_LOG_LIMIT);
		return;
	}
	int price = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetBuyNewsPrice(city_data.cur_city_buy_news_count + 1);
	const ActivitySilkRoadsSkillCfg * skill_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetSkillCfg(role_data.init_skill);
	if (NULL != skill_cfg && ACTIVITY_SILK_ROADS_SKILL_TYPE_NEWS == skill_cfg->skill_type)
	{
		price -= (int)((1LL * price * skill_cfg->skill_roads_skill_num) / 100);
	}
	if (role_data.have_coin_count < price)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_MONEY_NOT_ENOUGH);
		return;
	}
	//查询此时剩余的消息
	if (role_log_data.city_log_list.empty() || role_log_data.buy_count >= ARRAY_LENGTH(role_log_data.buy_city_log_list))
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_TRADING_LOG_ALL_SHOW);
		return;
	}
	bool is_get_log = false;
	int rand_index = RandomNum((int)role_log_data.city_log_list.size());
	for (int i = 0; i < (int)role_log_data.city_log_list.size(); i++)
	{
		if (rand_index == i)
		{
			is_get_log = true;
			ActivitySilkRoadsRoleTradingLogData log_data;
			log_data = role_log_data.city_log_list[i];
			log_data.from_city_map_seq = role_data.city_list[role_data.cur_city_index].city_map_seq;
			role_log_data.buy_city_log_list[role_log_data.buy_count] = log_data;
			role_log_data.buy_count++;
			this->OnSendSingleTradingLogInfo(role, role_log_data.buy_count - 1, log_data);
			role_log_data.city_log_list.erase(role_log_data.city_log_list.begin() + i);
			break;
		}
	}
	if (!is_get_log)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_TRADING_LOG_ALL_SHOW);
		return;
	}
	
	role_data.have_coin_count -= price;
	city_data.cur_city_buy_news_count += 1;
	this->OnSendSimpleInfo(role, SCActivitySilkRoadsSimpleInfo::SILK_ROADS_SIMPLE_TYPE_BUY_NEWS, city_data.cur_city_buy_news_count, (int)role_log_data.city_log_list.size());	
	this->OnSendAssetsInfo(role);
}

void ActivitySilkRoads::OnExpansionWarehouse(Role * role, int count)
{
	if (!this->IsActivityOpen() || NULL == role || count <= 0)
	{
		return;
	}

	std::map<int, ActivitySilkRoadsRoleData>::iterator it = m_role_data_map.find(role->GetUID()); 
	if (it == m_role_data_map.end()) return;

	const ActivitySilkRoadsOtherCfg & other_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetOtherCfg();
	ActivitySilkRoadsRoleData & role_data = it->second;
	if (role_data.finish_num >= other_cfg.act_num)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_FINISH_NUM_LIMIT);
		return;
	}
	//还没出发
	if (role_data.cur_city_index < 0 || role_data.cur_city_index >= ARRAY_LENGTH(role_data.city_list))
	{
		return;
	}

	int max_buy_times = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetMaxBuyWarehouseTimes();
	if (role_data.buy_warehouse_count + count > max_buy_times || other_cfg.buy_bag * count + role_data.warehouse_data.cur_max_grid_count > MAX_SILK_ROADS_GRID_NUM)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_WAREHOUSE_EXP_LIMIT);
		return;
	}
	int price_count = 0;
	for (int i = 0; i < count && i < max_buy_times; i++)
	{
		int buy_times = role_data.buy_warehouse_count + 1 + i;
		price_count += LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetBuyWarehousePrice(buy_times);
	}

	if (role_data.have_coin_count < price_count)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_MONEY_NOT_ENOUGH);
		return;
	}

	role_data.have_coin_count -= price_count;
	role_data.buy_warehouse_count += count;
	role_data.warehouse_data.cur_max_grid_count += other_cfg.buy_bag * count;
	this->OnSendSimpleInfo(role, Protocol::SCActivitySilkRoadsSimpleInfo::SILK_ROADS_SIMPLE_TYPE_EXPANSION_WAREHOUSE, role_data.buy_warehouse_count, 0);
	this->OnSendAssetsInfo(role);
}

void ActivitySilkRoads::OnBuySupply(Role * role, int count)
{
	if (!this->IsActivityOpen() || NULL == role || count <= 0)
	{
		return;
	}

	std::map<int, ActivitySilkRoadsRoleData>::iterator it = m_role_data_map.find(role->GetUID());
	if (it == m_role_data_map.end()) return;

	const ActivitySilkRoadsOtherCfg & other_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetOtherCfg();
	ActivitySilkRoadsRoleData & role_data = it->second;
	if (role_data.finish_num >= other_cfg.act_num)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_FINISH_NUM_LIMIT);
		return;
	}
	//还没出发
	if (role_data.cur_city_index < 0 || role_data.cur_city_index >= ARRAY_LENGTH(role_data.city_list))
	{
		return;
	}
	int max_buy_times = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetMaxBuySupplyTimes();
	if (role_data.buy_supply_count + count > max_buy_times)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_BUY_SUPPLY_LIMIT);
		return;
	}
	int price_count = 0;
	for (int i = 0; i < count && i < max_buy_times; i++)
	{
		int buy_times = role_data.buy_supply_count + 1 + i;
		price_count += LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetBuySupplyPrice(buy_times);
	}

	if (role_data.have_coin_count < price_count)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_MONEY_NOT_ENOUGH);
		return;
	}
	role_data.have_coin_count -= price_count;
	role_data.buy_supply_count += count;
	role_data.have_supply_count += other_cfg.buy_supply * count;
	this->OnSendSimpleInfo(role, Protocol::SCActivitySilkRoadsSimpleInfo::SILK_ROADS_SIMPLE_TYPE_BUY_SUPPLY, role_data.buy_supply_count, 0);
	this->OnSendAssetsInfo(role);
}

void ActivitySilkRoads::OnEnd(Role * role)
{
	if(NULL == role) return;

	std::map<int, ActivitySilkRoadsRoleData>::iterator it = m_role_data_map.find(role->GetUID());
	if (it == m_role_data_map.end()) return;

	const ActivitySilkRoadsOtherCfg & other_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetOtherCfg();
	ActivitySilkRoadsRoleData & role_data = it->second;
	if (role_data.finish_num >= other_cfg.act_num)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_FINISH_NUM_LIMIT);
		return;
	}
	//还没出发
	if (role_data.cur_city_index < 0 || role_data.cur_city_index >= ARRAY_LENGTH(role_data.city_list))
	{
		return;
	}
	int need_consume_supply = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetWarehouseExpend(role_data.warehouse_data.cur_has_commodity_count) + other_cfg.start_supply_num;
	const ActivitySilkRoadsSkillCfg * skill_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetSkillCfg(role_data.init_skill);
	if (NULL != skill_cfg && ACTIVITY_SILK_ROADS_SKILL_TYPE_TRADING == skill_cfg->skill_type)
	{
		need_consume_supply -= (int)((1LL * need_consume_supply * skill_cfg->skill_roads_skill_num) / 100);
	}
	if (role_data.have_supply_count >= need_consume_supply && role_data.cur_city_index + 1 != role_data.city_count)	//补给充足,不在终点无法提前结算
	{
		role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_NOT_ADVANCE_END);
		return;
	}

	//自动出售所有货物
	ActivitySilkRoadsRoleWarehouseData & warehouse_data = role_data.warehouse_data;
	ActivitySilkRoadsRoleCityData & city_data = role_data.city_list[role_data.cur_city_index];
	for (int i = 0; i < warehouse_data.cur_max_grid_count && i < ARRAY_LENGTH(warehouse_data.grid_list); ++i)
	{
		if(warehouse_data.cur_has_commodity_count <= 0)break;

		if (warehouse_data.grid_list[i].commodity_seq > 0 && warehouse_data.grid_list[i].commodity_count > 0)
		{
			const ActivitySilkRoadsCommodityCfg * commodity_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetCommodityCfg(warehouse_data.grid_list[i].commodity_seq);
			if(NULL == commodity_cfg) continue;
			
			int price = commodity_cfg->freight_market_price;
			city_data.GetCommodity(warehouse_data.grid_list[i].commodity_seq, price);

			role_data.have_coin_count += price * warehouse_data.grid_list[i].commodity_count;
			warehouse_data.cur_has_commodity_count -= warehouse_data.grid_list[i].commodity_count;
			warehouse_data.grid_list[i].Reset();
		}
	}

	//发送结算奖励(分红)
	const ActivitySilkRoadsSettlementBoxCfg * settlement_box_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetSettlementBoxCfg(role_data.have_coin_count);
	if (NULL != settlement_box_cfg)
	{
		static MailContentParam contentparam; contentparam.Reset();
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = settlement_box_cfg->coin;
		if (NULL != ITEMPOOL->GetItem(settlement_box_cfg->reward_item.item_id))
		{
			contentparam.item_list[0] = ItemDataWrapper(settlement_box_cfg->reward_item.item_id, settlement_box_cfg->reward_item.num, settlement_box_cfg->reward_item.is_bind ? 1 : 0);
		}
		float have_coin_count = 1.0F * role_data.have_coin_count / 10000;
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_silk_roads_settlement_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_silk_roads_settlement_reward_content, role_data.trading_num, 
			have_coin_count, 1, settlement_box_cfg->coin / 10000);
		if (lensub > 0 && length > 0) MailRoute::MailToUser(role->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
	}
	this->OnSendSimpleInfo(role, Protocol::SCActivitySilkRoadsSimpleInfo::SILK_ROADS_SIMPLE_TYPE_END, role_data.have_coin_count, 0);
	role_data.cumulation_coin_count += role_data.have_coin_count;
	role_data.cumulation_trading_count += role_data.trading_num;
	role_data.finish_num += 1;
	role_data.FinishReset();
	m_role_trading_map.erase(role->GetUID());
	is_change_flag = true;
	role->GetRoleModuleManager()->GetCommonData().silk_roads_count += 1;
	gamelog::g_log_activity_silk_roads.printf(LL_INFO, "%s user[%d, %s] [coin:%d, trading:%d] finish_num[%d]", __FUNCTION__, role->GetUID(), role->GetName(), 
		role_data.cumulation_coin_count, role_data.cumulation_trading_count, role_data.finish_num);
}

void ActivitySilkRoads::OnSendSimpleInfo(Role * role, int simple_type, int param1, int param2)
{
	if(NULL == role) return;

	Protocol::SCActivitySilkRoadsSimpleInfo info;
	info.simple_type = simple_type;
	info.param1 = param1;
	info.param2 = param2;

	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, sizeof(info));
}

void ActivitySilkRoads::OnSendRoleAllInfo(Role * role)
{
	if(NULL == role)  return;

	std::map<int, ActivitySilkRoadsRoleData>::const_iterator it = m_role_data_map.find(role->GetUID());
	if(it == m_role_data_map.end()) return;
	ActivitySilkRoadsTradingLogData & role_log_data = m_role_trading_map[role->GetUID()];

	static Protocol::SCActivitySilkRoadsRoleAllInfo info;
	info.role_data = it->second;
	info.reserve_sh = 0;
	info.trading_log_count = 0;
	info.news_pool_count = (int)role_log_data.city_log_list.size();
	for (int i = 0; i < role_log_data.buy_count && i < ARRAY_LENGTH(role_log_data.buy_city_log_list) && info.trading_log_count < ARRAY_LENGTH(info.trading_log_list); i++)
	{
		info.trading_log_list[info.trading_log_count] = role_log_data.buy_city_log_list[i];
		info.trading_log_count++;
	}

	EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &info);
}

void ActivitySilkRoads::OnSendAssetsInfo(Role * role)
{
	if (NULL == role)  return;

	std::map<int, ActivitySilkRoadsRoleData>::const_iterator it = m_role_data_map.find(role->GetUID());
	if (it == m_role_data_map.end()) return;

	const ActivitySilkRoadsRoleData & role_data = it->second;
	Protocol::SCActivitySilkRoadsAssetsInfo info;
	info.have_coin_count = role_data.have_coin_count;
	info.have_supply_count = role_data.have_supply_count;
	info.cur_max_grid_count = role_data.warehouse_data.cur_max_grid_count;
	info.reserve_sh = 0;

	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, sizeof(info));
}

void ActivitySilkRoads::OnSendWarehouseGirdInfo(Role * role, short notice_type, const ActivitySilkRoadsRoleWarehouseData & warehouse_info, int grid_index)
{
	if(NULL == role || grid_index < 0 || grid_index >= ARRAY_LENGTH(warehouse_info.grid_list)) return;
	 
	Protocol::SCActivitySilkRoadsWarehouseChange info;
	info.warehouse_grid_index = grid_index;
	info.notice_type = notice_type;
	info.warehouse_commodity_seq = warehouse_info.grid_list[grid_index].commodity_seq;
	info.warehouse_commodity_count = warehouse_info.grid_list[grid_index].commodity_count;
	info.warehouse_commodity_cost = warehouse_info.grid_list[grid_index].commodity_cost;

	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, sizeof(info));
}

void ActivitySilkRoads::OnSendSingleTradingLogInfo(Role * role, int trading_log_index, ActivitySilkRoadsRoleTradingLogData log_data)
{
	if(NULL == role) return;

	Protocol::SCActivitySilkRoadsSingleTradingLogNotice info;
	info.trading_log_index = trading_log_index;
	info.trading_log = log_data;

	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, sizeof(info));
}

void ActivitySilkRoads::OnSendAllRankInfo(Role * role)
{
	if(NULL == role) return;

	static Protocol::SCActivitySilkRoadsAllRankInfo info;
	info.rank_count = 0;
	for (int i = 0; i < ARRAY_LENGTH(m_rank_list) && info.rank_count < ARRAY_LENGTH(info.rank_list); ++i)
	{
		if(m_rank_list->role_id <= 0) break;

		UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(m_rank_list[i].role_id);
		if(NULL == user_cache_node) continue;

		info.rank_list[info.rank_count] = m_rank_list[i];
		user_cache_node->GetName(info.rank_list[info.rank_count].role_name);
		info.rank_list[info.rank_count].avatar_type = user_cache_node->avatar_type;
		info.rank_list[info.rank_count].headshot_id = user_cache_node->appearance.headshot_id;
		info.rank_count++;
	}

	int length = sizeof(info) - (ARRAY_LENGTH(info.rank_list) - info.rank_count) * sizeof(info.rank_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, length);
}

void ActivitySilkRoads::OnSendAllTradingLogInfo(Role * role)
{
	if(NULL == role) return; 

	Protocol::SCActivitySilkRoadsAllTradingLogInfo info;
	info.reserve_sh = 0;
	info.trading_log_count = 0;
	const ActivitySilkRoadsTradingLogData & role_log_data = m_role_trading_map[role->GetUID()];
	for (int i = 0; i < role_log_data.buy_count && i < ARRAY_LENGTH(role_log_data.buy_city_log_list) && info.trading_log_count < ARRAY_LENGTH(info.trading_log_list); i++)
	{
		info.trading_log_list[info.trading_log_count] = role_log_data.buy_city_log_list[i];
		info.trading_log_count++;
	}

	int length = sizeof(info) - (ARRAY_LENGTH(info.trading_log_list) - info.trading_log_count) * sizeof(info.trading_log_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, length);
}

bool ActivitySilkRoads::FindReward(Role * role, int consume_type, int count)
{
	if (NULL == role) return false;

	if (count > role->GetRoleModuleManager()->GetCommonData().silk_roads_find_count)
	{
		role->NoticeNum(errornum::EN_FIND_COUNT_NOT_ENOUGH);
		return false;
	}

	const ActivitySilkRoadsOtherCfg & other_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetOtherCfg();

	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPut2(other_cfg.find_reward))
	{
		role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}

	role->GetRoleModuleManager()->GetKnapsack()->Put(other_cfg.find_reward, PUT_REASON_FIND_REWARD);
	role->GetRoleModuleManager()->GetMoney()->AddCoinBind(LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetOtherCfg().find_coin, __FUNCTION__);

	role->GetRoleModuleManager()->GetCommonData().silk_roads_find_count -= count;
	role->GetRoleModuleManager()->GetCommonData().today_activity_type_find_count[DAY_ACTIVITY_TYPE_SILK_ROADS_ACTIVE] += count;
	role->GetRoleModuleManager()->GetCommonData().today_find_reward_flag.SetBit(DAY_ACTIVITY_TYPE_SILK_ROADS_ACTIVE);
	return true;
}

void ActivitySilkRoads::OnActivityEnd()
{
	//先给所有玩家结算
	static MailContentParam contentparam;
	const ActivitySilkRoadsOtherCfg & other_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetOtherCfg();
	for (std::map<int, ActivitySilkRoadsRoleData>::iterator it = m_role_data_map.begin(); it != m_role_data_map.end(); ++it)
	{
		//已经经过结算
		if(it->second.finish_num >= other_cfg.act_num || it->second.cur_city_index < 0 || it->second.cur_city_index >= ARRAY_LENGTH(it->second.city_list)) continue;

		//自动出售所有货物
		ActivitySilkRoadsRoleWarehouseData & warehouse_data = it->second.warehouse_data;
		ActivitySilkRoadsRoleCityData & city_data = it->second.city_list[it->second.cur_city_index];
		for (int i = 0; i < warehouse_data.cur_max_grid_count && i < ARRAY_LENGTH(warehouse_data.grid_list); ++i)
		{
			if (warehouse_data.cur_has_commodity_count <= 0)break;

			if (warehouse_data.grid_list[i].commodity_seq > 0 && warehouse_data.grid_list[i].commodity_count > 0)
			{
				const ActivitySilkRoadsCommodityCfg * commodity_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetCommodityCfg(warehouse_data.grid_list[i].commodity_seq);
				if (NULL == commodity_cfg) continue;

				int price = commodity_cfg->freight_market_price;
				city_data.GetCommodity(warehouse_data.grid_list[i].commodity_seq , price);

				it->second.have_coin_count += price * warehouse_data.grid_list[i].commodity_count;
				warehouse_data.cur_has_commodity_count -= warehouse_data.grid_list[i].commodity_count;
				warehouse_data.grid_list[i].Reset();
			}
		}

		//发放结算奖励
		const ActivitySilkRoadsSettlementBoxCfg * settlement_box_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetSettlementBoxCfg(it->second.have_coin_count);
		if (NULL != settlement_box_cfg)
		{
			contentparam.Reset();
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = settlement_box_cfg->coin;
			if (NULL != ITEMPOOL->GetItem(settlement_box_cfg->reward_item.item_id))
			{
				contentparam.item_list[0] = ItemDataWrapper(settlement_box_cfg->reward_item.item_id, settlement_box_cfg->reward_item.num, settlement_box_cfg->reward_item.is_bind ? 1 : 0);
			}
			float have_coin_count = 1.0F * it->second.have_coin_count / 10000;
			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_silk_roads_settlement_reward_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_silk_roads_settlement_reward_content, it->second.trading_num, 
				have_coin_count, 1, settlement_box_cfg->coin / 10000);
			if (lensub > 0 && length > 0) MailRoute::MailToUser(it->first, MAIL_REASON_DEFAULT, contentparam);
		}

		it->second.cumulation_coin_count += it->second.have_coin_count;
		it->second.cumulation_trading_count += it->second.trading_num;
		it->second.finish_num += 1;
		it->second.FinishReset();
	}

	//重新排序
	std::vector<ActivitySilkRoadsRoleRankData> rank_list;
	for (std::map<int, ActivitySilkRoadsRoleData>::const_iterator it = m_role_data_map.begin(); it != m_role_data_map.end(); ++it)
	{
		if (it->second.finish_num > 0)
		{
			rank_list.push_back(ActivitySilkRoadsRoleRankData(it->first, it->second.cumulation_coin_count, it->second.cumulation_trading_count, it->second.finish_num));
		}
	}

	std::sort(rank_list.begin(), rank_list.end(), SortRank);
	gamelog::g_log_activity_silk_roads.printf(LL_INFO, "%s send rank reward mail ready, role_count[%d]", __FUNCTION__, (int)rank_list.size());
	//发放排名奖励
	for (int i = 0; i < (int)rank_list.size(); i++)
	{
		int role_rank = i + 1;
		const ItemConfigData * rank_reward = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetRankReward(role_rank);
		if (NULL == rank_reward)
		{
			continue; 
		}
		gamelog::g_log_activity_silk_roads.printf(LL_INFO, "%s user[%d] [coin:%d, trading:%d] finish_num[%d] send rank reward mail ready", __FUNCTION__, rank_list[i].role_id,
			rank_list[i].cumulation_coin_count, rank_list[i].cumulation_trading_count, rank_list[i].finish_num);
		contentparam.Reset();
		contentparam.item_list[0] = ItemDataWrapper(rank_reward->item_id, rank_reward->num, rank_reward->is_bind ? 1 : 0);

		float cumulation_coin_count = rank_list[i].cumulation_coin_count * 1.0F / 10000;
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_silk_roads_rank_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_silk_roads_rank_reward_content, rank_list[i].finish_num,
			cumulation_coin_count, 1, role_rank);
		if (lensub > 0 && length > 0) 
		{
			MailRoute::MailToUser(rank_list[i].role_id, MAIL_REASON_DEFAULT, contentparam);

			Role *user = World::GetInstWorld()->GetSceneManager()->GetRole(rank_list[i].role_id);
			if (NULL != user)
			{
				user->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_ACTIVITY_END);
			}
			gamelog::g_log_activity_silk_roads.printf(LL_INFO, "%s user[%d] send rank reward mail succ", __FUNCTION__, rank_list[i].role_id);
		}
	}
	gamelog::g_log_activity_silk_roads.printf(LL_INFO, "%s send rank reward mail succ", __FUNCTION__);

	//清除数据
	m_role_data_map.clear();
	m_role_trading_map.clear();
	for (int i = 0; i < ARRAY_LENGTH(m_rank_list); i++)
	{
		m_rank_list[i].Reset();
	}
	m_next_refresh_timestamp = 0;
	is_change_flag = false;
}

void ActivitySilkRoads::OnActivityOpen()
{
	m_role_data_map.clear();
	m_role_trading_map.clear();
	for (int i = 0; i < ARRAY_LENGTH(m_rank_list); i++)
	{
		m_rank_list[i].Reset();
	}
	m_next_refresh_timestamp = (unsigned int)(EngineAdapter::Instance().Time() + MAX_SILK_ROADS_REFRESH_NUM);
	is_change_flag = false;
}

//外部已经判断过了
void ActivitySilkRoads::OnUpdateRankList()
{
	if(m_role_data_map.empty()) return;

	std::vector<ActivitySilkRoadsRoleRankData> rank_list;
	for (std::map<int, ActivitySilkRoadsRoleData>::const_iterator it = m_role_data_map.begin(); it != m_role_data_map.end(); ++it)
	{
		if (it->second.finish_num > 0)
		{
			rank_list.push_back(ActivitySilkRoadsRoleRankData(it->first, it->second.cumulation_coin_count, it->second.cumulation_trading_count, it->second.finish_num));
		}
	}

	std::sort(rank_list.begin(), rank_list.end(), SortRank);
	for (int i = 0; i < (int)rank_list.size() && i < ARRAY_LENGTH(m_rank_list); i++)
	{
		m_rank_list[i] = rank_list[i];
	}
	m_next_refresh_timestamp = (unsigned int)(EngineAdapter::Instance().Time() + MAX_SILK_ROADS_REFRESH_NUM); 
	is_change_flag = false;
}

bool ActivitySilkRoads::OnGenerateCityInfo(int city_map_seq, ActivitySilkRoadsRoleCityData & city_data, std::vector<ActivitySilkRoadsRoleTradingLogData> * role_log_data,
	const ActivitySilkRoadsRoleCityData * last_city_data)
{
	const ActivitySilkRoadsCityCfg * city_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetCityCfg(city_map_seq);
	if (NULL == city_cfg) return false;

	const ActivitySilkRoadsOtherCfg & other_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetOtherCfg();

	//打乱特产顺序
	std::vector<int> city_specialty_list;
	city_specialty_list.assign(city_cfg->city_specialty_list.begin(), city_cfg->city_specialty_list.end());
	std::random_shuffle(city_specialty_list.begin(), city_specialty_list.end());

	//取出高额奖励的列表(有可能有为空)
	std::set<int> double_list;		
	for (int i = 0; i < (int)city_cfg->double_price_list.size() && i < (int)city_specialty_list.size(); i++)
	{
		int rate_num = RandomNum(MAX_SILK_ROADS_REWAD_RATE_COUNT);
		if (rate_num < city_cfg->double_price_list[i])
		{
			double_list.insert(city_specialty_list[i]);
		}
	}
	//大涨优先录入消息列表
	int log_count = 0;
	int double_log_count = 0;
	if (!double_list.empty() && NULL != role_log_data)
	{
		double_log_count = log_count = GetMin((int)double_list.size(), other_cfg.city_max_news);
	}

	const std::vector<ActivitySilkRoadsCommodityCfg> & commodity_list = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetCommodityList();
	city_data.commondity_count = 0;
	for (int i = 0; i < (int)commodity_list.size() && city_data.commondity_count < ARRAY_LENGTH(city_data.commodity_list); i++)
	{
		ActivitySilkRoadsRoleCityData::CommodityItem & commodity_item = city_data.commodity_list[city_data.commondity_count];
		//大涨
		int has_log_type = 0;
		int min_price_percent = ActivitySilkRoadsCommodityCfg::MAX_COMMODITY_BASE_NUM, max_price_percent = ActivitySilkRoadsCommodityCfg::MAX_COMMODITY_BASE_NUM;
		if (double_list.end() != double_list.find(commodity_list[i].city_specialty_seq))
		{
			min_price_percent += commodity_list[i].min_price_up;
			max_price_percent += commodity_list[i].max_price_up;
			if (double_log_count > 0)
			{
				has_log_type = SILK_ROADS_LOG_TYPE_DOUBLE_PRICE_UP;
				double_log_count -= 1;
			}
		}
		else
		{
			min_price_percent += commodity_list[i].min_price;
			max_price_percent += commodity_list[i].max_price;
		}
		int old_price = commodity_list[i].freight_market_price;
		if (NULL != last_city_data)
		{
			old_price = last_city_data->commodity_list[i].price;
		}
		int min_price = (old_price * min_price_percent) / ActivitySilkRoadsCommodityCfg::MAX_COMMODITY_BASE_NUM;
		int max_price = (old_price * max_price_percent) / ActivitySilkRoadsCommodityCfg::MAX_COMMODITY_BASE_NUM;

		commodity_item.price = GetMax(RandomNum(min_price, max_price + 1), commodity_list[i].lowest_price);	//需要加1,否则随机不到最高价
		commodity_item.commodity_seq = commodity_list[i].city_specialty_seq;

		//该货物是特产并且有涨幅
		if ((0 == has_log_type) && city_cfg->city_specialty_list.end() != city_cfg->city_specialty_list.find(commodity_item.commodity_seq) && 
			commodity_item.price > old_price && log_count < other_cfg.city_max_news)
		{
			has_log_type = SILK_ROADS_LOG_TYPE_PRICE_UP;
		}
		
		if (NULL != role_log_data && 0 != has_log_type)
		{
			ActivitySilkRoadsRoleTradingLogData log_data;
			log_data.log_type = has_log_type;
			log_data.freight_id = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetRandFreightId(log_data.log_type);
			log_data.target_city_map_seq = city_map_seq;
			log_data.commodity_seq = commodity_item.commodity_seq;
			log_data.price = commodity_item.price;
			role_log_data->push_back(log_data);
			if (SILK_ROADS_LOG_TYPE_DOUBLE_PRICE_UP != has_log_type)	//大涨已经先加上了次数
			{
				log_count++;
			}
		}
		city_data.commondity_count++;
	}

	city_data.city_map_seq = city_map_seq;
	return true;
}

void ActivitySilkRoads::OnGiveCityReward(Role * role, ActivitySilkRoadsRoleWarehouseData & role_warehouse_data, int city_map_seq)
{
	if(NULL == role) return;

	if (role_warehouse_data.cur_has_commodity_count < role_warehouse_data.cur_max_grid_count)	//此时仓库还有空位
	{
		const ActivitySilkRoadsCityRewardCfg * city_reward_cfg = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetRandCityRewardByCity(city_map_seq);
		if (NULL != city_reward_cfg)
		{
			//超出不给
			int put_count = (int)city_reward_cfg->reward_commodity_list.size() * city_reward_cfg->commodity_count;
			if(role_warehouse_data.cur_has_commodity_count + put_count > role_warehouse_data.cur_max_grid_count) return;

			for (std::set<int>::const_iterator it = city_reward_cfg->reward_commodity_list.begin(); it != city_reward_cfg->reward_commodity_list.end(); ++it)
			{
				//进入城市的奖励,免费给,成本为0
				if (!this->OnPutCommodity(role, role_warehouse_data, *it, city_reward_cfg->commodity_count, 0))
				{
					break;
				}
			}
		}
	}
}

bool ActivitySilkRoads::OnPutCommodity(Role * role, ActivitySilkRoadsRoleWarehouseData & role_warehouse_data, short commodity_seq, short commodity_count, int commodity_cost)
{
	if(NULL == role || commodity_seq <= 0 || commodity_count <= 0) return false;

	//剩余仓位
	int last_num = role_warehouse_data.cur_max_grid_count - role_warehouse_data.cur_has_commodity_count;	
	if(commodity_count > last_num) return false;

	int find_index = -1;
	for (int i = 0; i < role_warehouse_data.cur_max_grid_count && i < ARRAY_LENGTH(role_warehouse_data.grid_list); i++)	
	{
		ActivitySilkRoadsRoleWarehouseData::GridInfo & grid_info = role_warehouse_data.grid_list[i];
		if (grid_info.commodity_seq == commodity_seq)
		{
			find_index = i;
			break;
		}
		if (-1 == find_index && grid_info.commodity_seq <= 0)
		{
			find_index = i;
		}
	}
	if (find_index < 0)
	{
		return false;
	}
	ActivitySilkRoadsRoleWarehouseData::GridInfo & grid_info = role_warehouse_data.grid_list[find_index];
	//计算成本需要放在数量改变前
	grid_info.commodity_cost = (1.0F * (grid_info.commodity_count * grid_info.commodity_cost) + (commodity_cost * commodity_count)) / (grid_info.commodity_count + commodity_count);	
	grid_info.commodity_seq = commodity_seq;
	grid_info.commodity_count += commodity_count;
	role_warehouse_data.cur_has_commodity_count += commodity_count;
	this->OnSendWarehouseGirdInfo(role, 0 != commodity_cost ? Protocol::SCActivitySilkRoadsWarehouseChange::NOTICE_TYPE_BUY_COMMODITY : Protocol::SCActivitySilkRoadsWarehouseChange::NOTICE_TYPE_REWARD,
		role_warehouse_data, find_index);
	return true;
}

bool ActivitySilkRoads::OnConsumeCommodity(Role * role, ActivitySilkRoadsRoleWarehouseData & role_warehouse_data, short commodity_seq, short consume_count)
{
	if(NULL == role || commodity_seq <= 0 || consume_count <= 0) return false;

	int find_index = -1;
	for (int i = 0; i < role_warehouse_data.cur_max_grid_count && i < ARRAY_LENGTH(role_warehouse_data.grid_list); i++)
	{
		ActivitySilkRoadsRoleWarehouseData::GridInfo & grid_info = role_warehouse_data.grid_list[i];
		if (grid_info.commodity_seq == commodity_seq)
		{
			find_index = i;
			break;
		}
	}
	if (find_index < 0)
	{
		return false;
	}

	ActivitySilkRoadsRoleWarehouseData::GridInfo & grid_info = role_warehouse_data.grid_list[find_index];
	if (grid_info.commodity_count < consume_count)
	{
		return false;
	}

	grid_info.commodity_count -= consume_count;
	role_warehouse_data.cur_has_commodity_count -= consume_count;
	if (grid_info.commodity_count <= 0)
	{
		grid_info.Reset();
	}
	this->OnSendWarehouseGirdInfo(role,  Protocol::SCActivitySilkRoadsWarehouseChange::NOTICE_TYPE_SALE_COMMODITY, role_warehouse_data, find_index);
	return true;
}
