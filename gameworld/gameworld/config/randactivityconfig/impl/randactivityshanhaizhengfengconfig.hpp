#ifndef __RAND_ACTIVITY_SHAN_HAI_ZHENG_FENG_CONFIG_HPP__
#define __RAND_ACTIVITY_SHAN_HAI_ZHENG_FENG_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raburningsummerparam.hpp"

struct RAShanHaiZhengFengOtherCfg
{
	RAShanHaiZhengFengOtherCfg() : consume_item_id(0), consume_num(0), all_pass_reward_group_id(0)
	{}

	int consume_item_id;
	int consume_num;
	int all_pass_reward_group_id;
};

struct RAShanHaiZhengFengBaseCfg
{
	RAShanHaiZhengFengBaseCfg() : boss_group_id(0), frist_reward_group_id(0), common_reward_group_id(0)
	{}

	int boss_group_id;
	int frist_reward_group_id;
	int common_reward_group_id;
};

struct RAShanHaiZhengFengSectionCfg
{
	RAShanHaiZhengFengSectionCfg () : section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::map<int, RAShanHaiZhengFengBaseCfg> m_base_cfg;		//key : activity_time
};

struct RAShanHaiZhengFengBossGroupCfg
{
	RAShanHaiZhengFengBossGroupCfg() : level_down(0), level_up(0)
	{
		memset(monster_gourp_list, 0, sizeof(monster_gourp_list));
	}

	int level_down;
	int level_up;
	int monster_gourp_list[MAX_RA_SUMMER_LI_LIAN_MONSTER_NUM];
};

struct RAShanHaiZhengFengRewardCfg
{
	RAShanHaiZhengFengRewardCfg() : exp(0), coin(0), rate(0)
	{}

	static const int EXP_BASE_NUM = 10000;

	int exp;			//角色等级升级经验 * exp(万分比)
	int coin;
	int rate;
	std::vector<ItemConfigData> reward_list;
};

struct RAShanHaiZhengFengRewardGroupCfg
{
	RAShanHaiZhengFengRewardGroupCfg() : rate_count(0)
	{}

	int rate_count;
	std::vector<RAShanHaiZhengFengRewardCfg> reward_cfg_list;
};


class RandActivityManager;
class RandActivityShanHaiZhengFengConfig : public RandActivityConfig
{
public:
	RandActivityShanHaiZhengFengConfig();
	virtual ~RandActivityShanHaiZhengFengConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAShanHaiZhengFengOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	const RAShanHaiZhengFengBaseCfg * GetBaseCfg(RandActivityManager* ramgr, int activity_time)const;
	const std::map<int, RAShanHaiZhengFengBaseCfg> * GetBaseMapCfg(RandActivityManager* ramgr)const;
	const RAShanHaiZhengFengBossGroupCfg * GetBossGroupCfg(int boss_group_id, int role_level)const;
	const RAShanHaiZhengFengRewardCfg * GetRewardCfg(int reward_group_id)const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitBossgGroupCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);
	int InitBaseCfg(TiXmlElement *RootElement);

	RAShanHaiZhengFengOtherCfg m_other_cfg;
	std::vector<RAShanHaiZhengFengSectionCfg> m_base_cfg_vec;						//基础配置
	std::map<int, std::vector<RAShanHaiZhengFengBossGroupCfg> > m_boss_group_cfg;	//首领配置	key:boss_group_id
	std::map<int, RAShanHaiZhengFengRewardGroupCfg> m_reward_group_cfg;				//奖励组配置 key:reward_group_id
};


#endif	//__RAND_ACTIVITY_SUMMER_LI_LIAN_CONFIG_HPP__