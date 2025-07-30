#ifndef __ACTIVITY_CONFIG_HPP__
#define __ACTIVITY_CONFIG_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"



struct ActivitySignUpCfg					//限时活动报名
{
	ActivitySignUpCfg():seq(0), activity_id(0), open(0)
	{}

	int seq;								//索引
	int activity_id;						//活动ID
	int open;								//0:关闭 1:开启
	std::vector<ItemConfigData> item_list;	//奖励道具列表
};

struct ActivityJoinRewardCfg						//限时活动参与奖励
{
	ActivityJoinRewardCfg() :min_level(0), max_level(0), exp(0), coin(0)
	{}

	int min_level;									// 最低等级
	int max_level;									// 最大等级

	int exp;										// 经验奖励
	int coin;										// 铜币奖励
	std::vector<ItemConfigData> reward_vec;			// 道具奖励
};

class ActivityConfig : public ILogicConfig
{
public:
	ActivityConfig();
	virtual ~ActivityConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ActivitySignUpCfg* GetSignUpCfg(int seq);
	const ActivitySignUpCfg* GetSignUpCfgById(int activity_id);
	const ActivityJoinRewardCfg * GetJoinRewardCfg(int _activity_type, int _level);

private:
	int InitActivitySignUpCfg(TiXmlElement *RootElement);
	int InitActivityJoinRewardCfg(TiXmlElement *RootElement);

	std::vector<ActivitySignUpCfg> m_sign_up_cfg;					//限时活动报名
	std::map<int, ActivitySignUpCfg> m_sign_up_by_id_map;			//限时活动报名 以活动id为key

	std::map<int, std::vector<ActivityJoinRewardCfg> >	m_join_reward_map;	// < 活动号 , 配置信息>
};

#endif
