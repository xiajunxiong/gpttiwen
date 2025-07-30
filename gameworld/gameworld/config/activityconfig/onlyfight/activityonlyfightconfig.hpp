#ifndef __ACTIVITY_ONLY_FIGHT_CONFIG__
#define __ACTIVITY_ONLY_FIGHT_CONFIG__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/commondata.hpp"
#include <set>

static const int ONLY_FIGHT_AWARD_MAX_COUNT = 10;
static const int ONLY_FIGHT_BOX_MAX_AWARD = 5;
static const int ONLY_FIGHT_BORN_POSI_MAX_NUM = 8;


struct OnlyFightDeleteData
{
	OnlyFightDeleteData() :box_id(0), obj_id(0) {}
	OnlyFightDeleteData(int _box_id, ObjID _obj_id) : box_id(_obj_id), obj_id(_obj_id) {}
	int box_id;
	ObjID obj_id;
};

struct OnlyFightSceneCfg
{
	OnlyFightSceneCfg():min_level(0), max_level(0), scene_id(0)
	{}

	int min_level;
	int max_level;
	int scene_id;
};

struct OnlyFightOtherCfg
{
	OnlyFightOtherCfg() : max_fail_times(0), max_rounds_num(0), wins_score(0),lose_score(0), 
	draw_score(0), rank_refresh_interval(0), num(0), max_num(0), countdown(0) {}

	int max_fail_times;				// 最大失败次数
	int max_rounds_num;				// 单次战斗最大回合数
	int wins_score;					// 胜利所得积分
	int lose_score;					// 失败所得积分
	int draw_score;					// 平局所得积分
	int rank_refresh_interval;		// 积分排名表刷新间隔 - 单位:秒(s)
	int time;						// 刷新时间
	int num;						// 一次刷新宝箱个数
	int max_num;					// 同时存在宝箱个数
	int countdown;					// 进场倒计时
};

struct OnlyFightParticipationAward		//参与奖励
{
	OnlyFightParticipationAward() : coin_bind(0), experience(0) {}

	int coin_bind;			// 铜币奖励
	int experience;			// 经验奖励
	std::vector<ItemConfigData> item_reward;	//允许无道具奖励
};

struct OnlyFightRankAward		// 以一敌百(单人版家族大乱斗)排名奖励
{
	OnlyFightRankAward() : title(0), coin(0) {}

	int title;			// 称号奖励
	int coin;			// 铜币奖励
	std::vector<ItemConfigData> item_reward;		//允许无道具奖励
};

struct OnlyFightRoleLevelRankAward		
{
	OnlyFightRoleLevelRankAward()
	{
		min_role_level = 0;
		max_role_level = 0;
	}

	int min_role_level;
	int max_role_level;
	std::map<ParamSection, OnlyFightRankAward> rank_reward;
};

struct OnlyFightSurvAward		// 存活奖励
{
	OnlyFightSurvAward() : seq(0), coin_bind(0), experience(0) {}

	int seq;
	int coin_bind;			// 铜币奖励
	int experience;			// 经验奖励
	std::vector<ItemConfigData> item_reward;	//允许无道具奖励
};

struct OnlyFightBoxCfg
{
	OnlyFightBoxCfg() : box_id(0), rate(0), reward_id_num(0)
	{
		memset(reward_id_list, 0, sizeof(reward_id_list));
	}

	int box_id;						// 宝箱ID
	int rate;						// 权重

	int reward_id_num;				// 读取的数量
	int reward_id_list[ONLY_FIGHT_BOX_MAX_AWARD];		// 宝箱奖励组ID
};

struct OnlyFightBoxRewardCfg
{
	OnlyFightBoxRewardCfg(): reward_id(0), rate(0)
	{}

	int reward_id;				//宝箱奖励组ID
	int rate;
	ItemConfigData reward_item;
};

struct OnlyFightFindRewardGroupCfg
{
	OnlyFightFindRewardGroupCfg() :rate(0)
	{}

	int rate;
	ItemConfigData reward;
};

struct OnlyFightFindCfg
{
	OnlyFightFindCfg() :min_level(0), max_level(0), exp(0), coin(0), family_gongxian(0), group_id(0)
	{}

	int min_level;
	int max_level;
	int exp;
	int coin;
	int family_gongxian;
	int group_id;
};

struct OnlyFightFindGroupCfg
{
	OnlyFightFindGroupCfg()
	{}

	std::vector<OnlyFightFindCfg> node_list;
};

class ActivityOnlyFightConfig : public ILogicConfig
{
public:
	ActivityOnlyFightConfig();
	virtual ~ActivityOnlyFightConfig();
	
	virtual bool Init(const std::string &configname, std::string *err);
	void GetChestIDList(std::vector<int> & out_list);
	void GetSurvivalCheckNumList(std::set<int> & out_list);

	const OnlyFightOtherCfg & GetOnlyFightOtherCfg() { return m_other_cfg; }
	const OnlyFightParticipationAward * GetOnlyFightParCfg(int level);
	const OnlyFightRankAward * GetOnlyFightRankCfg(int rank, int role_level);
	const OnlyFightSurvAward * GetOnlyFightSurvAward(int level, int check_num);
	const OnlyFightBoxCfg * GetOnlyFightBoxCfg(int box_id);
	bool RandAwardListByBoxID(int chest_id, std::vector<ItemConfigData> & out_award_list);
	const OnlyFightBoxCfg * GetOnlyFightBoxCfg();
	const OnlyFightSceneCfg * GetOnlyFightSceneCfgByLevel(int level);
	const std::vector<OnlyFightSceneCfg> * GetOnlyFightSceneCfgVec() { return &m_scene_cfg; }

	Posi GetRandBornPosi();
	const std::vector<int> * GetRankTitleListCfg() { return &m_rank_reward_title_list_cfg;}

	const OnlyFightFindCfg * GetFindCfg(int find_type, int role_level)const;
	const OnlyFightFindRewardGroupCfg * GetFindRewardGroupCfg(int group_id)const;

private:
	int InitOnlyFightOthersCfg(TiXmlElement * RootElement);
	int InitOnlyFightParticicpationCfg(TiXmlElement * RootElement);
	int InitOnlyFightRankAward(TiXmlElement * RootElement);
	int InitOnlyFightSurvAward(TiXmlElement * RootElement);
	int InitOnlyFightBoxCfg(TiXmlElement * RootElement);
	int InitOnlyFightBoxRewardCfg(TiXmlElement * RootElement);
	int InitOnlyFightScecnCfg(TiXmlElement * RootElement);
	int InitOnlyFightBornPosiCfg(TiXmlElement * RootElement);
	int InitOnlyFightFindAwardGroupCfg(TiXmlElement * RootElement);
	int InitOnlyFightFindAwardCfg(TiXmlElement * RootElement);

	OnlyFightOtherCfg m_other_cfg;
	std::map<ParamSection, OnlyFightParticipationAward> m_particicpation_map;		// 参与奖励：    键 - 等级区间
	std::vector<OnlyFightRoleLevelRankAward> m_rank_award;						// 活动排名奖励：	键 - 排名区间
	std::map<int, std::map<ParamSection, OnlyFightSurvAward> > m_survival_award;	// 幸存者奖励：	键 - 幸存者临界数量、等级区间
	int chest_weight;
	std::map<int, OnlyFightBoxCfg> m_chest_cfg;										// 宝箱配置：	键 - 宝箱ID
	std::map<int, std::vector<OnlyFightBoxRewardCfg> > m_item_group_cfg;			// 宝箱奖励组：	键 - 组ID

	std::vector<OnlyFightSceneCfg> m_scene_cfg;										// 场景配置 : 区分玩家等级段

	int m_born_count;
	Posi m_born_list[ONLY_FIGHT_BORN_POSI_MAX_NUM];									// 出生点位
	std::vector<int> m_rank_reward_title_list_cfg;									// 存储排名奖励的称号

	std::map<int, std::vector<OnlyFightFindRewardGroupCfg> > m_find_reward_group_map;		// 找回道具组:key - group_id
	OnlyFightFindGroupCfg m_find_cfg[RB_TYPE_MAX];											// 奖励找回
};



#endif