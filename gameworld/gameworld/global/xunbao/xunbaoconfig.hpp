#ifndef _XUN_BAO_CONFIG_HPP_
#define _XUN_BAO_CONFIG_HPP_

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"

#include <map>
#include<vector>

static const int FIRST_MANUAL_COMMON_PET_XUN_BAO_DRAW_TYPE = 6;		//首次手动寻宝给奖励的固定池子

enum XUN_BAO_REWARD_TYPE
{
	XUN_BAO_REWARD_TYPE_OTTHER = 0,			
	XUN_BAO_REWARD_TYPE_SILVER = 1,			//银卡宠物
	XUN_BAO_REWARD_TYPE_GOLD = 2,			//金卡宠物

	XUN_BAO_REWARD_TYPE_MAX
};
UNSTD_STATIC_CHECK(MAX_XUN_BAO_BLESS_PET_NUM >= XUN_BAO_REWARD_TYPE_MAX);

struct XunBaoRewardGroup
{
	XunBaoRewardGroup()
	{
		reward_group_id = 0;
		rate_count = 0;

	}

	struct ItemCfg
	{
		ItemCfg()
		{
			rate = 0;
			floor_times = 0;
			curr_floor_times = 0;
			is_broadcast = false;
			xi_you_bao_di_type = 0;
		}

		int rate;
		int floor_times;
		int curr_floor_times;
		bool is_broadcast;
		int xi_you_bao_di_type;		//稀有池出X个金宠后抽取1个稀有保底。 值1金宠是加次数，值2表示稀有是重置次数
		ItemConfigData reward;
	};
	struct ItemGroupCfg
	{
		ItemGroupCfg(): rate_count(0), rate_up(0)
		{}

		int rate_count;
		int rate_up;
		std::vector<ItemCfg> reward_list;
	};

	bool Draw(ItemConfigData & out_item, bool & out_is_broadcast, int & outxi_you_type, int silver_pet_id, int gold_pet_id)
	{
		bool ret = false;

		if (rate_count == 0)
		{
			return ret;
		}

		bool is_rand_succ = false;
		int r = RandomNum(rate_count);
		int index = -1;
		for (std::map<int, ItemGroupCfg>::iterator it = reward_type_map.begin(); it != reward_type_map.end(); it++)
		{
			if (r < it->second.rate_count && !is_rand_succ)
			{
				int pet_id = 0;
				if (0 != it->second.rate_up)
				{
					switch (it->first)
					{
					case XUN_BAO_REWARD_TYPE_SILVER:
					{
						if (0 != silver_pet_id)
						{
							pet_id = silver_pet_id;
						}
					}
					break;
					case XUN_BAO_REWARD_TYPE_GOLD:
					{
						if (0 != gold_pet_id)
						{
							pet_id = gold_pet_id;
						}
					}
					break;
					default:
						break;
					}
				}
				if (0 != pet_id)
				{
					std::vector<ItemCfg> reward_list = it->second.reward_list;
					int rand_rate = r;
					for (int i = 0; i < (int)reward_list.size(); i++)
					{
						if (pet_id == reward_list[i].reward.item_id)
						{	
							// 公式由(rate_num + x)/(rate_count + x) = new_rate转换,rate_num:对应权重 rate_count:区间总权重 new_rate:新概率(百分比) x:修改至新概率所需调整的权重
							int add_rate = ((it->second.rate_count * it->second.rate_up - reward_list[i].rate * 100) / (100 - it->second.rate_up));
							reward_list[i].rate += add_rate;
							int this_rate_count = it->second.rate_count + add_rate;
							rand_rate = RandomNum(this_rate_count);
							break;
						}
					}
					
					for (int i = 0; i < (int)reward_list.size(); i++)
					{	
						if (rand_rate < reward_list[i].rate || (reward_list[i].floor_times != 0 && reward_list[i].curr_floor_times >= reward_list[i].floor_times))
						{
							ret = true;
							index = i;
							out_item = reward_list[i].reward;
							out_is_broadcast = reward_list[i].is_broadcast;
							outxi_you_type = reward_list[i].xi_you_bao_di_type;
							is_rand_succ = true;
							break;
						}
						rand_rate -= reward_list[i].rate;
					}
				}
			}

			for (int i = 0; i < (int)it->second.reward_list.size(); i++)
			{
				it->second.reward_list[i].curr_floor_times += 1;
				if (!is_rand_succ)
				{
					if (r < it->second.reward_list[i].rate || (it->second.reward_list[i].floor_times != 0 && it->second.reward_list[i].curr_floor_times >= it->second.reward_list[i].floor_times))
					{
						ret = true;
						it->second.reward_list[i].curr_floor_times = 0;
						out_item = it->second.reward_list[i].reward;
						out_is_broadcast = it->second.reward_list[i].is_broadcast;
						outxi_you_type = it->second.reward_list[i].xi_you_bao_di_type;
						is_rand_succ = true;
					}
					r -= it->second.reward_list[i].rate;
				}
				else if (index == i)
				{
					index = -1;
					it->second.reward_list[i].curr_floor_times = 0;
				}
			}
		}

		return ret;
	}

	int reward_group_id;
	int rate_count;
	//std::vector<ItemCfg> reward_list;
	std::map<int, ItemGroupCfg> reward_type_map;	//key:reward_type
};

enum XUN_BAO_TREASURE_TYPE
{
	XUN_BAO_TREASURE_TYPE_INVALID = 0,
	XUN_BAO_TREASURE_TYPE_GOD_PET = 1,			//金宠池
	XUN_BAO_TREASURE_TYPE_RARE_PET = 2,			//稀有池
	XUN_BAO_TREASURE_TYPE_PARTNER = 3,			//伙伴池
	XUN_BAO_TREASURE_TYPE_SOUL_EQUIP = 4,		//寻器池(魂器)

	XUN_BAO_TREASURE_TYPE_MAX
};

struct XunBaoDrawCfg
{
	struct LevelRewardItem
	{
		LevelRewardItem()
		{
			min_level = 0;
			max_level = 0;
		}

		int min_level;
		int max_level;
		XunBaoRewardGroup reward_group;
	};

	struct DrawFloorRewardItem
	{
		DrawFloorRewardItem()
		{
			rate = 0;
		}

		int rate;
		ItemConfigData reward;
	};

	struct DrawFirstDrawRewardItem
	{
		DrawFirstDrawRewardItem()
		{
			rate = 0;
		}

		int rate;
		ItemConfigData reward;
	};

	bool Draw(int role_level, ItemConfigData & out_item, bool & out_is_broadcast, int & outxi_you_type, int silver_pet_id, int gold_pet_id);
	bool GetFiveDrawFloorItem(ItemConfigData & out);
	bool GetFirstDrawFloorItem(ItemConfigData & out);

	XunBaoDrawCfg()
	{
		xun_bao_treasure_type = 0;
		is_open = false;
		open_day = 0;
		open_type = 0;
		start_time = 0;
		end_time = 0;
		week_open_flag = 0;		
		week_day_open_begin_time = 0;			
		week_day_open_end_time = 0;				

		draw_type = 0;
		once_item_id = 0;
		five_item_id = 0;
		is_has_pet_reward = true;
		five_draw_floor_item_rate_count = 0;
		first_draw_item_rate_count = 0;

		draw_times_reward_group_id = 0;

		exchange_material_id1 = 0;
		exchange_material_id2 = 0;
		exchange_get_id = 0;

		xi_you_bao_di_times = 0;
		free_draw_interval = 0;		//免费抽奖间隔s	
		free_times_num = 0;

		second_draw_bao_di_reward_group_id = 0;
		second_draw_bao_di_times = 0;

		can_cumulative_bao_di = false;
		cumulative_bao_di_times = 0;
		first_free_draw_interval = 0;
		is_reset_floors = true;
	}

	int xun_bao_treasure_type;
	bool is_open;
	int open_day;  //open_day不为0，当前开服时间小于这个这个值才能抽

	int open_type;
	//-------open_type为1时，
	time_t start_time;	//当前时间这个时间戳才能抽
	time_t end_time;	//当前时间这个时间戳才能抽
	//-------open_type为2时，与限时活动类型一样，周X，X点-X点开启；
	unsigned int week_open_flag;			//周几开启
	int  week_day_open_begin_time;			//周几开启每天X点开启
	int  week_day_open_end_time;				//周几开启每天x点结束

	int draw_type;
	int once_item_id;
	int five_item_id;
	bool is_has_pet_reward;
	std::vector<LevelRewardItem> level_draw_cfg;

	int five_draw_floor_item_rate_count;
	std::vector<DrawFloorRewardItem> five_draw_floor_cfg;		//五连抽必得一个

	int first_draw_item_rate_count;
	std::vector<DrawFirstDrawRewardItem> first_draw_item_cfg;

	int draw_times_reward_group_id;

	int exchange_material_id1;
	int exchange_material_id2;
	int exchange_get_id;

	unsigned int xi_you_bao_di_times;
	unsigned int free_draw_interval;		//免费抽奖间隔s	
	unsigned int first_free_draw_interval;		//首次免费抽奖间隔s
	int free_times_num;

	int second_draw_bao_di_reward_group_id; //首20抽必得一个
	int second_draw_bao_di_times;

	bool can_cumulative_bao_di;
	int cumulative_bao_di_times;
	bool is_reset_floors;
};

struct XunBaoExchangeComsumeCfg
{
	XunBaoExchangeComsumeCfg()
	{
		group_id = 0;
		pet_id = 0;
		strengthen_level = 0;
		num = 0;
	}

	int group_id;
	int pet_id;
	int strengthen_level;
	int num;
};

struct XunBaoExchangeGetCfg
{
	XunBaoExchangeGetCfg()
	{
		group_id = 0;
		pet_id = 0;
		strengthen_level = 0;
		num = 0;
	}

	int group_id;
	int pet_id;
	int strengthen_level;
	int num;
};

struct XunBaoOtherCfg
{
	XunBaoOtherCfg()
	{
		xunbao_pet_id_3 = 0;
	}

	int xunbao_pet_id_3;
};

struct XunBaoDrawCumulativeRewardCfg
{
	struct ItemCfg
	{
		ItemCfg() :rate(0)
		{
		}

		int rate;
		ItemConfigData reward;
	};

	XunBaoDrawCumulativeRewardCfg()
	{
		rate_count = 0;
	}

	int rate_count;
	std::vector<ItemCfg> group_reward;
};

struct XunBaoXiYouBaoDiRewardCfg
{
	struct ItemCfg
	{
		ItemCfg() :rate(0)
		{
		}

		int rate;
		ItemConfigData reward;
	};

	XunBaoXiYouBaoDiRewardCfg()
	{
		rate_count = 0;
	}

	int rate_count;
	std::vector<ItemCfg> group_reward;
};

class XunBaoConfig : public ILogicConfig
{
public:
	XunBaoConfig();
	~XunBaoConfig();

	virtual bool Init(const std::string & configname, std::string * err);

	XunBaoDrawCfg * GetXunBaoDrawCfg(int draw_type);
	std::set<int> GetXunBaoAllDrawTypeCfg();
	std::vector<const XunBaoDrawCfg *> GetOpenDrawCfg();
	bool IsHitDrawCumulativeReward(int group_id, const std::vector<ItemConfigData> & item_list);
	const ItemConfigData * GetDrawCumulativeReward(int group_id, int draw_times, int need_times);
	const ItemConfigData * GetXiYouBaoReward(int draw_type);
	const XunBaoOtherCfg & GetOtherCfg() const { return m_other_cfg; }

	const XunBaoExchangeComsumeCfg * GetExchangeComsumeCfg(int group_id, int seq);
	const XunBaoExchangeGetCfg * GetExchangeGetCfg(int group_id, int seq);
private:
	int InitDrawFloorRewardCfg(TiXmlElement * RootElement);
	int InitFirstDrawRewardCfg(TiXmlElement * RootElement);
	int InitXunBaoCfg(TiXmlElement * RootElement);
	int InitRewardGroupCfg(TiXmlElement * RootElement);
	int InitLevelCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitDrawCumulativeRewardCfg(TiXmlElement * RootElement);
	int InitExchangeComsumeCfg(TiXmlElement * RootElement);
	int InitExchangeGetCfg(TiXmlElement * RootElement);
	int InitBlessRateCfg(TiXmlElement * RootElement);
	int InitXiYouBaoDiCfg(TiXmlElement * RootElement);

	std::map<int, XunBaoRewardGroup > m_reward_group;

	std::map<int, XunBaoDrawCfg > m_draw_cfg;

	XunBaoOtherCfg m_other_cfg;
	std::map<int, XunBaoDrawCumulativeRewardCfg > m_draw_cumulative_reward_cfg;

	std::map<int, std::vector<XunBaoExchangeComsumeCfg> > m_exchange_comsume_cfg;
	std::map<int, std::vector<XunBaoExchangeGetCfg> > m_exchange_get_cfg;
	std::map<int, int> m_bless_map;			//祈福配置 key:reward_type second:rate_up(百分比)
	std::map<int, XunBaoXiYouBaoDiRewardCfg> m_xi_you_bao_di_map_cfg;			
};



#endif // _WUSHUANGDIANCONFIG_HPP_