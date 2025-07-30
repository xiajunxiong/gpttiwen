#ifndef _YAO_XING_SHI_LIAN_CONFIG_HPP_
#define _YAO_XING_SHI_LIAN_CONFIG_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/yaoxingshiliandef.hpp"
#include <vector>

struct YaoXingShiLianOtherCfg
{
	YaoXingShiLianOtherCfg() :max_rounds(0), rmb(0) {}
	int max_rounds;
	int rmb;
};

struct YaoXingShiLianLevelCfg
{
	YaoXingShiLianLevelCfg() :seq(0), num(0), level_limit(0), score(0), monster_one(0), monster_two(0), coin(0) {}
	int seq;
	int	num;
	int level_limit;
	int	score;
	int	monster_one;
	int	monster_two;
	int coin;
	std::vector<ItemConfigData> reward_vec;
};

enum YAO_XING_ACH_TYPE
{
	YAO_XING_ACH_TYPE_NO = 0,//无
	YAO_XING_ACH_TYPE_1 = 1,//消耗货币
	YAO_XING_ACH_TYPE_2 = 2,//角色等级
	YAO_XING_ACH_TYPE_3 = 3,//综合评分
	YAO_XING_ACH_TYPE_4 = 4,//直购
	YAO_XING_ACH_TYPE_MAX,
};

struct YaoXingShiLianRewardCfg
{
	YaoXingShiLianRewardCfg() :seq(0), num(0), achievement(YAO_XING_ACH_TYPE_NO), param1(0), param2(0) {}
	int seq;
	int num;
	int achievement;
	int param1;
	int param2;
	std::vector<ItemConfigData> reward_vec;
};

struct YaoXingShiLianReward2Cfg
{
	YaoXingShiLianReward2Cfg() :seq(0), num(0), achievement(0), achievement_2(YAO_XING_ACH_TYPE_NO), param1(0), param2(0) {}
	int seq;
	int num;
	int achievement;
	int achievement_2;
	int param1;
	int param2;
	std::vector<ItemConfigData> reward_vec;
};

class RoleModuleManager;
class YaoXingShiLianConfig : public ILogicConfig
{
public:
	YaoXingShiLianConfig();
	virtual ~YaoXingShiLianConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const YaoXingShiLianOtherCfg & GetOhterCfg() { return m_other_cfg; }
	const YaoXingShiLianLevelCfg * GetLevelCfg(int seq);
	const YaoXingShiLianRewardCfg * GetRewardCfg(int seq);
	const YaoXingShiLianReward2Cfg * GetReward2Cfg(int seq);
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitLevelCfg(TiXmlElement * RootElement);
	int InitAchievementCfg(TiXmlElement * RootElement);
	int InitAchievement2Cfg(TiXmlElement * RootElement);

	YaoXingShiLianOtherCfg m_other_cfg;
	std::map<int, YaoXingShiLianLevelCfg> m_level_cfg;

	YaoXingShiLianRewardCfg* reward_list[MAX_YAOXINGSHILIAN_REWARD_LIST];
	std::vector<YaoXingShiLianRewardCfg> m_reward_vec;

	YaoXingShiLianReward2Cfg* reward_list2[MAX_YAOXINGSHILIAN_REWARD2_LIST];
	std::vector<YaoXingShiLianReward2Cfg> m_reward2_vec;
};
#endif
