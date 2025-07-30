#ifndef __RAND_ACTIVITY_QIONG_LOU_YU_YAN_CONFIG_HPP__
#define __RAND_ACTIVITY_QIONG_LOU_YU_YAN_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raqionglouyuyanparam.hpp"
#include <vector>
#include <map>

struct RAQiongLouYuYanOtherCfg
{
	RAQiongLouYuYanOtherCfg() : prep_num(0), reward_group_2(0), banquet_num(0), banquet_times(0), phase_times(0), start_time(0), prepare_times(0), refresh(0)
	{
		memset(item_id_list, 0, sizeof(item_id_list));
	}
	const static int MAX_ITEM_NUM = 3;
	int item_id_list[MAX_ITEM_NUM];
	int	prep_num;
	int	reward_group_2;
	int	banquet_num;
	int	banquet_times;
	int	phase_times;
	int	start_time;
	int	prepare_times;
	int refresh;
};

struct RAQiongLouYuYanConfig
{
	RAQiongLouYuYanConfig() : type(0), section_start(0), section_end(0), seq(0), prep_down(0), prep_up(0), banquet_type(0), reward_group(0), sp_reward_group(0) {}
	int type;
	int	section_start;
	int	section_end;
	int	seq;
	int	prep_down;
	int	prep_up;
	int	banquet_type;
	int	reward_group;
	int sp_reward_group;
};

struct RAQiongLouYuYanRewardConfig
{
	RAQiongLouYuYanRewardConfig() : reward_group(0), rate(0){}
	int reward_group;
	int rate;
	std::vector<ItemConfigData> m_reward_vec;
};

struct RAQiongLouYuYanRewardGroupConfig
{
	RAQiongLouYuYanRewardGroupConfig() : weight(0){}
	int weight;
	std::vector<RAQiongLouYuYanRewardConfig> reward_vec;
};

class RandActivityManager;
class RandActivityQiongLouYuYanConfig : public RandActivityConfig
{
public:
	RandActivityQiongLouYuYanConfig();
	virtual ~RandActivityQiongLouYuYanConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	const RAQiongLouYuYanOtherCfg & GetOtherCfg() const { return m_other_cfg; }
	const RAQiongLouYuYanConfig * GetConfig(int open_day, int num) const;
	const RAQiongLouYuYanConfig * GetConfigBySeq(int open_day, int seq) const;
	const RAQiongLouYuYanConfig * GetConfigByBanquetType(int open_day, int banquet_type) const;
	const RAQiongLouYuYanRewardConfig * RandRewardByGroup(int reward_group) const;
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitBanquetCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);

	RAQiongLouYuYanOtherCfg m_other_cfg;
	std::vector<RAQiongLouYuYanConfig> qiong_lou_yu_yan_vec;
	std::map<int, RAQiongLouYuYanRewardGroupConfig> qiong_lou_yu_yan_reward_map;
};

#endif //__RAND_ACTIVITY_SUMMER_STORE_CONFIG_HPP__
