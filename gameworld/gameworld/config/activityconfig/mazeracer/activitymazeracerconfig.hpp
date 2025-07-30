#ifndef __ACTIVITY_MAZE_RACER_CONFIG__
#define __ACTIVITY_MAZE_RACER_CONFIG__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"

#include <set>
#include <map>
#include <vector>

static const int MAX_MAZE_RACE_BOX_REWARD_GROUP_NUM = 5;

struct MazeRacerOthersCfg
{
	MazeRacerOthersCfg() : act_reward_time(0), waiting_interval(0), waiting_num_max(0), 
		trans_npc_id(0), min_member_need(0), box_refresh_time(0), box_max_num(0), box_refresh_num(0) {}

	int act_reward_time;		// 活动奖励发放时间
	int waiting_interval;		// 等待大厅开始间隔（秒）
	int waiting_num_max;		// 等待大厅最大人数
	int trans_npc_id;			// 传送 NPC 的 ID
	int min_member_need;		// 所需的最小队员数
	int box_refresh_time;		// 宝箱刷新时间
	int box_max_num;			// 宝箱存在最大数量
	int box_refresh_num;		// 宝箱刷新数量
};

struct MazeRacerSceneCfg
{
	MazeRacerSceneCfg() : min_lv(0), max_lv(0), maze_id(0), wait_scene_id(0) {}

	int min_lv;
	int max_lv;
	int maze_id;			// 迷宫ID
	int wait_scene_id;		// 等待场景的场景ID
};

struct MazeRacerAwardCfg	// 奖励配置
{
	MazeRacerAwardCfg() : award_prob(0) {}

	int award_prob;
	ItemConfigData award_item;
};

struct MazeRacerAumletCfg		// 符咒配置
{
	MazeRacerAumletCfg() :effect_type(0), effect_val(0), target_type(0), duration(0), drop_rate(0) {}

	int effect_type;
	int effect_val;
	int target_type;
	int duration;
	int drop_rate;
};

struct MazeRacerWaitingHall
{
	MazeRacerWaitingHall(int _maze_id = 0, int _min_lv = 0, int _max_level = 0) : target_maze_id(_maze_id), 
		min_level_limit(_min_lv), max_level_limit(_max_level) {}

	int target_maze_id;
	int min_level_limit;
	int max_level_limit;
};

struct RankAward_Level
{
	RankAward_Level() {}
	std::vector<int> m_rank_award_list;
};

struct RankAward_Rank
{
	RankAward_Rank() {}
	std::map<ParamSection, RankAward_Level> level_posi;
};

struct RankAward
{
	RankAward() {}
	std::map<ParamSection, RankAward_Rank> rank_posi;
};

struct ActivityRankRewardCfg
{
	ActivityRankRewardCfg() : min_level(0), max_level(0)
	{}

	int min_level;
	int max_level;
	std::vector<ItemConfigData> reward_list;
};

struct MazeRaceBoxCfg
{
	MazeRaceBoxCfg() : box_id(0), reward_num(0), captain_reward_num(0), rate(0)
	{
		memset(reward_group_id_list, 0, sizeof(reward_group_id_list));
		memset(captain_reward_group_id_list, 0, sizeof(captain_reward_group_id_list));
	}

	int box_id;

	int reward_num;
	int reward_group_id_list[MAX_MAZE_RACE_BOX_REWARD_GROUP_NUM];

	int captain_reward_num;
	int captain_reward_group_id_list[MAX_MAZE_RACE_BOX_REWARD_GROUP_NUM];
	int rate;
};

struct MazeRaceBoxRewardCfg
{
	MazeRaceBoxRewardCfg() : rate(0)
	{}

	int rate;
	ItemConfigData item_reward;
};

class ActivityMazeRacerConfig : public ILogicConfig
{
public:
	ActivityMazeRacerConfig();
	virtual ~ActivityMazeRacerConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	int GetARandAmulet();
	int GetMaxTierByMazeID(int maze_id);
	int RandASceneIDByMazeAndTier(int maze_id, int tier, std::set<int> & eliminate_list);
	void GetAllWaitingHallSceneID(std::set<int> & out_list);
	bool RandActivityAward(int maze_id, int rank_posi, int role_level, std::vector<ItemConfigData> & out_list);
	void RandCopyRankAward(int maze_id, int rank_posi,int level, std::vector<ItemConfigData> & out_list);

	const MazeRacerOthersCfg & GetOthersConfig() { return m_mr_other_cfg; }
	const MazeRacerAumletCfg * GetAmuletConfig(int amulet_id);
	const MazeRacerSceneCfg * GetSceneConfig(int level);

	const MazeRacerWaitingHall * GetWaitingHallInfoBySceneID(int scene_id);

	const MazeRaceBoxCfg * GetRandMazeRaceBoxCfg();
	void GetMazeRaceBoxReward(int box_id, std::vector<ItemConfigData>& out_reward_list, bool is_leader);

private:
	int InitMazeRacerOtherInfor(TiXmlElement * RootElement);
	int InitMazeRacerBasicInfor(TiXmlElement * RootElement);
	int InitMazeRacerTierConfig(TiXmlElement * RootElement);
	int InitMazeRacerSceneGroup(TiXmlElement * RootElement);
	int InitMazeRacerAwardGroup(TiXmlElement * RootElement);
	int InitMazeRacerRankReward(TiXmlElement * RootElement);
	int InitMazeRacerCopyReward(TiXmlElement * RootElement);
	int InitMazeRacerAmuletsCfg(TiXmlElement * RootElement);
	int InitMazeRacerBoxCfg(TiXmlElement * RootElement);
	int InitMazeRacerBoxRewardGroupCfg(TiXmlElement * RootElement);

	bool IsTimerCfg(int check_time);
	int CheckSceneGroup();		// 检测场景组的合法性

	MazeRacerOthersCfg m_mr_other_cfg;
	std::map<ParamSection, MazeRacerSceneCfg> m_scene_cfg;										// 基本配置；	键 - 等级区间；
	std::map<int, std::map<int, int> > m_tier_cfg;												// 层数配置；	键 - 迷宫ID、层数；		值 - 场景组ID
	std::map<int, std::set<int> > m_scene_group_cfg;											// 场景组配置；	键 - 场景组ID；			值 - 场景ID
	std::map<int, std::vector<MazeRacerAwardCfg> > m_award_cfg;									// 奖励配置；	键 - 奖励组ID；
	std::map<int, std::map<ParamSection, std::vector<ActivityRankRewardCfg> > > m_rank_award;	// 活动排名奖励；	键 - 迷宫ID、排名；		值 - 奖励组ID 列表
	std::map<int, RankAward > m_copy_award;														// 副本排名奖励；	键 - 迷宫ID、排名；		值 - 奖励组ID 列表
	std::map<int, MazeRacerAumletCfg> m_aumlet_cfg;												// 符咒配置；	键 - 符咒ID

	// 预处理
	std::map<int, MazeRacerWaitingHall> m_hall_info_cfg;										// 等候大厅信息；	键 - 大厅的场景ID；		值 - 信息容器

	int box_weight;																	//宝箱总权重
	std::map<int, MazeRaceBoxCfg> m_box_cfg;										//宝箱配置 key:box_id
	std::map<int, std::vector<MazeRaceBoxRewardCfg> > m_box_reward_group_cfg;		//宝箱奖励组 key:reward_group_id
};

#endif // !__ACTIVITY_MAZE_RACER_CONFIG__