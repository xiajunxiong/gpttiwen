#ifndef __RAND_ACTIVITY_MAZE_SUMMER_CONFIG_HPP__
#define __RAND_ACTIVITY_MAZE_SUMMER_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/ramazesummerparam.hpp"

struct RAMazeSummerOtherCfg
{
	RAMazeSummerOtherCfg() : consume_item(0), first_num(0), sec_num(0), third_num(0), four_more_num(0), shop_item_num_down(0), shop_item_num_up(0) {}
	int consume_item;
	int	first_num;
	int	sec_num;
	int	third_num;
	int	four_more_num;
	int shop_item_num_down;
	int	shop_item_num_up;
};

struct RAMazeSummerMapCfg
{
	RAMazeSummerMapCfg() : type(0), seq(0), grid(0), event_group(0), reward_group(0) {}
	int type;
	int	seq;
	int	grid;
	int	event_group;
	int	reward_group;
};

struct RAMazeSummerMapListCfg
{
	RAMazeSummerMapListCfg() : section_start(0), section_end(0) {}
	int	section_start;
	int	section_end;
	RAMazeSummerMapCfg map_info[MAX_MAZE_SUMMER_POS_X][MAX_MAZE_SUMMER_POS_Y];
};

struct RAMazeSummerEventGroupCfg
{
	RAMazeSummerEventGroupCfg() : event_group(0), event_type(RA_MAZE_SUMMER_MAP_TYPE_ROSE), param_1(0), rate(0) {}
	int event_group;
	int	event_type;
	int	param_1;
	int	rate;
};

struct RAMazeSummerEventGroupListCfg
{
	RAMazeSummerEventGroupListCfg() : weight(0) {}
	int weight;
	std::vector<RAMazeSummerEventGroupCfg> m_list;
};

struct RAMazeSummerRewardGroupCfg
{
	RAMazeSummerRewardGroupCfg() : reward_group(0), exp(0), coin(0), rate(0) {}
	int reward_group;
	int	exp;
	int	coin;
	int	rate;
	std::vector<ItemConfigData> m_reward_vec;
};

struct RAMazeSummerRewardGroupListCfg
{
	RAMazeSummerRewardGroupListCfg() : weight(0) {}

	int weight;
	std::vector<RAMazeSummerRewardGroupCfg> m_list;
};

struct RAMazeSummerShopCfg
{
	RAMazeSummerShopCfg() : shop_type(0), index(0), buy_times(0), money_type(0), price(0), rate(0) {}
	int shop_type;
	int	index;
	int	buy_times;
	int	money_type;
	int price;
	int rate;
	ItemConfigData shop_info;
};

struct RAMazeSummerShopGroupCfg
{
	RAMazeSummerShopGroupCfg() : weight(0){}

	int weight;
	std::vector<RAMazeSummerShopCfg> m_reward_vec;
};

struct RAMazeSummerTaskCfg
{
	RAMazeSummerTaskCfg() : type(0), section_start(0), section_end(0), task_type(0), task(0), param_1(0), param_2(0){}
	int type;
	int	section_start;
	int	section_end;
	int	task_type;
	int task;
	int param_1;
	int param_2;
	ItemConfigData reward;
};

struct RAMazeSummerTaskListCfg
{
	RAMazeSummerTaskListCfg() : section_start(0), section_end(0) {}
	int	section_start;
	int	section_end;
	std::vector<RAMazeSummerTaskCfg> m_task_list;
};

class RandActivityManager;
class RandActivityMazeSummerConfig : public RandActivityConfig
{
public:
	RandActivityMazeSummerConfig();
	virtual ~RandActivityMazeSummerConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	const RAMazeSummerOtherCfg &GetOtherCfg()const { return m_other_cfg; }
	const RAMazeSummerMapListCfg * GetMazeSummerMapListCfg(int open_day)const;
	const RAMazeSummerMapCfg * GetMazeSummerMapCfgByDayAndGrid(int open_day,int grid)const;
	const RAMazeSummerEventGroupCfg * GetMazeSummerEventInfoCfg(int event_group, bool has_shop = false)const;
	const RAMazeSummerShopGroupCfg * GetMazeSummerShopGroupCfg(int shop_group)const;
	const RAMazeSummerTaskListCfg * GetMazeSummerTaskListCfg(int open_day)const;
	const RAMazeSummerRewardGroupCfg * GetMazeSummerRewardCfg(int reward_group)const;
	const RAMazeSummerTaskCfg * GetMazeSummerTaskCfgByType(int type)const;
	const RAMazeSummerShopCfg * GetShopItemDetailCfg(int shop_id,int seq)const;
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitMazeSummerCfg(TiXmlElement * RootElement);
	int InitEventGroupCfg(TiXmlElement * RootElement);
	int InitMazeRewardGroupCfg(TiXmlElement * RootElement);
	int InitMazeShopGroupCfg(TiXmlElement * RootElement);
	int InitMazeTaskCfg(TiXmlElement * RootElement);

	enum SECTION_TYPE
	{
		SECTION_TYPE_START = 0,
		SECTION_TYPE_END = 1,
		SECTION_TYPE_MAX,
	};
	union KeyAndSection
	{
		int section[SECTION_TYPE_MAX];
		long long key;
	};
	long long GetKeyBy(int section_start, int section_end);
	void GetSectionByKey(long long key, int *section_start, int *section_end);

	RAMazeSummerOtherCfg m_other_cfg;
	std::map<long long, RAMazeSummerMapListCfg> m_map_info;
	std::map<int, RAMazeSummerEventGroupListCfg> m_event_group;
	std::map<int, RAMazeSummerRewardGroupListCfg> m_reward_group;
	std::map<int, RAMazeSummerShopGroupCfg> m_shop_group;
	std::map<long long, RAMazeSummerTaskListCfg> m_task_map;
	std::map<int, RAMazeSummerTaskCfg> m_task_type_map;
};

#endif