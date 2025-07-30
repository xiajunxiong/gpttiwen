#ifndef __ACTIVITY_RELIC_LOOTING_CONFIG__
#define __ACTIVITY_RELIC_LOOTING_CONFIG__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/commonsavedef.h"
#include "global/activity/activityimpl/activityreliclooting.hpp"

#include <set>
#include <map>
#include <vector>

struct RelicLootingOtherCfg
{
	RelicLootingOtherCfg() 
	{
		scene_id = 0;
		duration = 0;
		pick_distance = 0;
		first_props = 0;
		props_time = 0;
		props_num = 0;
		memset(defender_pos_list, 0, sizeof(defender_pos_list));
		memset(attacker_pos_list, 0, sizeof(attacker_pos_list));
		once_team_players = 1;
		win_box_num = 0;

		box_score = 0;
		capture_score = 0;
		first_score = 0;
		second_score = 0;
		third_score = 0;
		continu_score = 0;
		time_score_3 = 0;
		time_score_5 = 0;
		time_score_8 = 0;
		winner_score = 0;
		loser_score = 0;
	}

	int scene_id;
	int duration;
	int pick_distance;
	int first_props;
	int	props_time;
	int props_num;
	Posi defender_pos_list[RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM];
	Posi attacker_pos_list[RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM];
	int once_team_players;
	int win_box_num;

	int box_score;
	int	capture_score;
	int first_score;
	int	second_score;
	int	third_score;
	int	continu_score;
	int	time_score_3;
	int	time_score_5;
	int	time_score_8;
	int	winner_score;
	int	loser_score;
};

struct RelicLootingSkillCfg
{
	RelicLootingSkillCfg() :sn(SKILL_ID_0), skill_type(SKILL_TYPE_NO), param1(0), param2(0), param3(0), param4(0), skill_limit(SKILL_LIMIT_0), res_id(0), res_dir(0), res_range(0) {}

	enum SKILL_ID
	{
		SKILL_ID_0 = 0,		//无
		SKILL_ID_1 = 1,		//抓捕		param1秒钟的技能引导时间后，对方未离开施法范围param2则成功抓捕，对方将进入禁锢状态无法移动，冷却时间param3秒。Param4抓捕范围
		SKILL_ID_2 = 2,		//疾走		技能引导param1秒后，移动速度增加param2%，持续param3秒，冷却时间param4秒
		SKILL_ID_3 = 3,		//传送		施法时间param1秒，传送到场景中的一个随机位置，冷却时间param2秒
		SKILL_ID_4 = 4,		//开锁		施法时间param1秒，打开一个宝箱的封印法阵。冷却时间param2秒，当范围内无宝箱时，无法释放，施法过程中移动将打断开锁技能。Param3开宝箱范围
		SKILL_ID_5 = 5,		//救援		施法时间param1秒，解除范围内一名友方角色的禁锢状态，冷却时间param2秒，当范围内无被禁锢队友时，无法释放，施法过程中移动将打断救援技能，技能进入冷却。Param3救援范围
		SKILL_ID_6 = 6,		//神隐		技能引导param1秒后，param2秒钟内为不可见状态，隐身状态释放技能将会现行，无法被抓捕。抓捕引导时间未离开抓捕范围，仍会被抓捕。冷却时间param3秒
		SKILL_ID_7 = 7,		//轻身符	技能引导param1秒，增加自身param2%移动速度，持续param3秒，效果结束后陷入虚弱状态，移动速度降低param4%，持续param5秒
		SKILL_ID_8 = 8,		//稻草人	施法时间param1秒，释放技能将在身前放置一个稻草人，param2秒后或再次释放技能则传送到稻草人的位置，param3稻草人NPCid
		SKILL_ID_9 = 9,		//天行梭	施法时间param1秒，随机传送到场上任一位置
		SKILL_ID_10 = 10,	//传送法阵	引导param1时间，询问队友是否立即传送到自身当前位置,引导时间内队友同意将传送到当前位置。
		SKILL_ID_11 = 11,	//陷阱		在当前位置放置一个隐形陷阱，陷阱存在param1秒，从陷阱上路过将会禁锢param2秒钟，无法移动。Param3触发范围，param4陷阱NPCid
		SKILL_ID_12 = 12,	//傀儡娃娃	在原地释放一个分身巡逻迷惑敌人，巡逻方向为释放时玩家方向。分身存在param1秒后消失
		SKILL_ID_13 = 13,	//隐身衣	技能引导param1秒后，param2秒钟内为不可见状态，隐身状态释放技能将会现行，无法被抓捕。抓捕引导时间未离开抓捕范围，仍会被抓捕
		SKILL_ID_14 = 14,	//挪移尺	释放后立即向前进方向挪移param1距离
	};

	enum SKILL_TYPE
	{
		SKILL_TYPE_NO = 0,	//无
		SKILL_TYPE_STOP = 1,//施法
		SKILL_TYPE_MOVE = 2,//引导
	};

	enum SKILL_LIMIT
	{
		SKILL_LIMIT_0 = 0,//通用技能
		SKILL_LIMIT_1 = 1,//守卫者技能
		SKILL_LIMIT_2 = 2,//盗宝者技能
	};

	int sn;
	int	skill_type;
	int	param1;
	int	param2;
	int	param3;
	int	param4;
	int	param5;
	int	skill_limit;
	int	res_id;
	int	res_dir;
	int	res_range;
};

struct SkillDropCfg
{
	SkillDropCfg() :sn(0), res_chance(0), npc_id(0) {}
	int sn;
	int res_chance;
	int npc_id;
};

struct DropPositionCfg
{
	DropPositionCfg() :box_sn(0), box_x(0), box_y(0), boss_dir(0), npc_id(0){}
	int box_sn;
	int	box_x;
	int	box_y;
	int	boss_dir;
	int npc_id;
};

struct RewardBoxCfg
{
	RewardBoxCfg() : sn(0), param(0){}
	int sn;
	int param_type;
	int param;
	std::vector<ItemConfigData> m_reward_vec;
};

struct RankRewardCfg
{
	RankRewardCfg() : sn(0), rank_range_min(0), rank_range_max(0) {}
	int sn;
	int	rank_range_min;
	int	rank_range_max;
	std::vector<ItemConfigData> m_rank_reward_vec;
};

class ActivityRelicLootingConfig : public ILogicConfig
{
public:
	ActivityRelicLootingConfig();
	virtual ~ActivityRelicLootingConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const RelicLootingOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const std::map<int, RelicLootingSkillCfg> * GetSkillInfoList() { return &m_skill_info_map; }
	const RelicLootingSkillCfg * GetSkillDetail(int skill_id);
	const std::vector<DropPositionCfg> * GetDropPositionVec() { return &m_drop_posi_vec; }
	const SkillDropCfg* RandSkill();
	const RankRewardCfg * GetRankRewardCfg(int rank_pos);
	const RewardBoxCfg * GetRewardCfg(int sn);
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitSkillCfg(TiXmlElement *RootElement);
	int InitResReleaseCfg(TiXmlElement *RootElement);
	int InitBoxPositionCfg(TiXmlElement *RootElement);
	int InitBoxRewardCfg(TiXmlElement *RootElement);
	int InitRankRewardCfg(TiXmlElement *RootElement);

	RelicLootingOtherCfg m_other_cfg;
	std::map<int, RelicLootingSkillCfg> m_skill_info_map;
	int drop_weight;
	std::vector<SkillDropCfg> m_skill_drop_vec;

	std::vector<DropPositionCfg> m_drop_posi_vec;
	std::map<int, RewardBoxCfg> m_reward_box_map;
	std::map<int, RankRewardCfg> m_rank_reward_map;
};

#endif 