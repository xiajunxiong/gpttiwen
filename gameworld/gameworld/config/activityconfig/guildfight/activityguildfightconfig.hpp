#ifndef __ACTIVITY_GUILD_FIGHT_CONFIG__
#define __ACTIVITY_GUILD_FIGHT_CONFIG__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/commondata.hpp"

#include <map>
#include <set>
#include <vector>

static const int GUILD_FIGHT_AWARD_MAX_COUNT = 10;
static const int GUILD_FIGHT_CHEST_MAX_AWARD = 5;
static const int GUILD_FIGHT_BORN_POSI_MAX_NUM = 8;			//场景出生点位最大数

struct GuildFightDeleteData
{
	GuildFightDeleteData() :box_id(0), obj_id(0) {}
	GuildFightDeleteData(int _box_id, ObjID _obj_id) : box_id(_obj_id), obj_id(_obj_id) {}
	int box_id;
	ObjID obj_id;
};

struct GuildFightAwardCon
{
	GuildFightAwardCon() : award_num(0) {}

	int award_num;
	ItemConfigData award_list[GUILD_FIGHT_AWARD_MAX_COUNT];
};

struct GuildFightOtherCfg
{
	GuildFightOtherCfg() : scene_id(0), max_fail_times(0), max_rounds_num(0), wins_score(0),
		lose_score(0), draw_score(0), rank_refresh_interval(0), guild_fund(0), num(0), max_num(0), 
		countdown(0), join_reward_coin(0), join_reward_gongxian(0)
	{}

	int scene_id;					// 场景ID
	int max_fail_times;				// 最大失败次数
	int max_rounds_num;				// 单次战斗最大回合数
	int wins_score;					// 胜利所得积分
	int lose_score;					// 失败所得积分
	int draw_score;					// 平局所得积分
	int rank_refresh_interval;		// 积分排名表刷新间隔 - 单位:秒(s)
	int guild_fund;					// 家族资金
	int time;						// 刷新时间
	int num;						// 一次刷新宝箱个数
	int max_num;					// 同时存在宝箱个数
	int countdown;					// 入场倒计时

	int join_reward_coin;
	int join_reward_gongxian;
	GuildFightAwardCon pati_award;	// 参与奖
};

struct GuildFightChestCfg
{
	GuildFightChestCfg() : box_id(0), rate(0), award_id_num(0), caotain_award_id_num(0)
	{
		memset(award_id_list, 0, sizeof(award_id_list));
		memset(caotain_award_id_list, 0, sizeof(caotain_award_id_list));
	}

	int box_id;
	int rate;

	int award_id_num;
	int award_id_list[GUILD_FIGHT_CHEST_MAX_AWARD];

	int caotain_award_id_num;
	int caotain_award_id_list[GUILD_FIGHT_CHEST_MAX_AWARD];
};

struct GuildFightItemRand
{
	GuildFightItemRand() : prob(0) {}

	int prob;
	ItemConfigData award_item;
};

struct GuildFightRankAward		// 家族排名奖励
{
	GuildFightRankAward() : title(0), contribution(0), coin(0) {}

	int title;			// 称号奖励
	int contribution;
	int coin;			
	GuildFightAwardCon item_reward;
};

struct GuildFightSurvAward		// 存活奖励
{
	GuildFightSurvAward() : seq(0), coin_bind(0), experience(0) {}

	int seq;
	int coin_bind;
	int experience;
	GuildFightAwardCon item_reward;
};

struct GuildFightJoinAwardCfg
{
	GuildFightJoinAwardCfg() : min_level(0), max_level(0), exp(0), coin(0)
	{}
	
	int min_level;
	int max_level;
	int exp;
	int coin;
	std::vector<ItemConfigData> reward_list;
};

struct GuildFightFindRewardGroupCfg
{
	GuildFightFindRewardGroupCfg() :rate(0)
	{}

	int rate;
	ItemConfigData reward;
};

struct GuildFightFindCfg
{
	GuildFightFindCfg() :min_level(0), max_level(0), exp(0), coin(0), family_gongxian(0), group_id(0)
	{}

	int min_level;
	int max_level;
	int exp;
	int coin;
	int family_gongxian;
	int group_id;
};

struct GuildFightFindGroupCfg
{
	GuildFightFindGroupCfg()
	{}

	std::vector<GuildFightFindCfg> node_list;
};

struct GuildFightRankBuffCfg
{
	GuildFightRankBuffCfg()
	{
	}

	std::vector<int> buff_id;
};

struct GuildFightRoleLevelRankAward
{
	GuildFightRoleLevelRankAward()
	{
		min_role_level = 0;
		max_role_level = 0;
	}

	int min_role_level;
	int max_role_level;

	std::map<ParamSection, GuildFightRankAward> rank_reward;
};

class ActivityGuildFightConfig : public ILogicConfig
{
public:
	ActivityGuildFightConfig() {}
	virtual ~ActivityGuildFightConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	void GetChestIDList(std::vector<int> & out_list);
	void GetSurvivalCheckNumList(std::set<int> & out_list);

	const GuildFightOtherCfg & GetGuildFightOtherCfg() { return m_other_cfg; }
	const GuildFightChestCfg * GetGuildFightChestCfg(int chest_id);
	const GuildFightRankAward * GetGuildFightActivityAward(int rank_posi, int role_level);
	const GuildFightSurvAward * GetGuildFightSurvivalAward(int check_num, int level);
	bool RandAwardListByChestID(int chest_id, std::vector<ItemConfigData> & out_award_list, bool is_leader);
	const GuildFightChestCfg* GetRandGuildFightChestCfg();
	const GuildFightJoinAwardCfg * GetGuildFightJoinAwardCfg(int role_level);

	const GuildFightFindCfg * GetFindCfg(int find_type, int role_level)const;
	const GuildFightFindRewardGroupCfg * GetFindRewardGroupCfg(int group_id)const;

	Posi GetRandBornPosi();
	int GetGuildFightRankTitle() { return guild_fight_title_id; }
private:
	int InitGuildFightOthersCfg(TiXmlElement * RootElement);
	int InitGuildFightRankAward(TiXmlElement * RootElement);
	int InitGuildFightSurvAward(TiXmlElement * RootElement);
	int InitGuildFightChestsCfg(TiXmlElement * RootElement);
	int InitGuildFightItemsRand(TiXmlElement * RootElement);
	int InitGuildFightBornPosiCfg(TiXmlElement * RootElement);
	int InitGuildFightJoinAwardCfg(TiXmlElement * RootElement);
	int InitGuildFightFindAwardGroupCfg(TiXmlElement * RootElement);
	int InitGuildFightFindAwardCfg(TiXmlElement * RootElement);

	GuildFightOtherCfg m_other_cfg;
	std::vector<GuildFightRoleLevelRankAward> m_rank_award;						// 活动排名奖励：	键 - 排名区间
	std::map<int, std::map<ParamSection, GuildFightSurvAward> > m_survival_award;	// 幸存者奖励：	键 - 幸存者临界数量、等级区间
	int chest_weight;
	std::map<int, GuildFightChestCfg> m_chest_cfg;									// 宝箱配置：	键 - 宝箱ID
	std::map<int, std::vector<GuildFightItemRand> > m_item_group_cfg;				// 宝箱奖励组：	键 - 组ID

	int m_born_count;																// 当前配置数量
	Posi m_born_list[GUILD_FIGHT_BORN_POSI_MAX_NUM];								// 出生点位
	int guild_fight_title_id;														// 家族大乱斗排名奖励ID

	std::vector<GuildFightJoinAwardCfg> m_join_award_vec;							// 参与奖励
	std::map<int, std::vector<GuildFightFindRewardGroupCfg> > m_find_reward_group_map;			// 找回道具组:key - group_id
	GuildFightFindGroupCfg m_find_cfg[RB_TYPE_MAX];			// 奖励找回
};

#endif // !__ACTIVITY_GUILD_FIGHT_CONFIG__