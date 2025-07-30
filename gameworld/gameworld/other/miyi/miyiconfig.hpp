#ifndef __MI_YI_CONFIG_HPP__
#define __MI_YI_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/miyidef.hpp"
#include "gamedef.h"
#include <vector>
#include <map>

enum REWARD_PROPORTION_TYPE
{
	REWARD_PROPORTION_TYPE_ALL = 0,		//翻牌全奖励
	REWARD_PROPORTION_TYPE_FIRST = 1,	//翻牌第一次衰减奖励
	REWARD_PROPORTION_TYPE_SECOND = 2,	//翻牌第二次衰减奖励

	REWARD_PROPORTION_TYPE_MAX
};
UNSTD_STATIC_CHECK(REWARD_PROPORTION_TYPE_MAX == MI_YI_MAX_REWARD_PROPORTION_NUM);

struct MiyiOtherCfg
{
	MiyiOtherCfg():monster_num(0), weekly_kill_times(0)
	{
		get_xia_yi_zhi_times = 0;
	}

	int monster_num;			//boss最大数量
	int weekly_kill_times;		//每周击杀boss总次数（超过不给固定奖励）
	ItemConfigData reward;		// 翻牌固定奖励
	int get_xia_yi_zhi_times;
};

struct MiYiMonsterCfg
{
	MiYiMonsterCfg()
	{
		monster_seq = 0;
		monster_level = 0;
		npc_monster_id = 0;
		npc_monster_pos = Posi(0, 0);
		add_exp = 0;
		add_coin = 0;
		dir_y = 0.f;
		role_level_reward_group_id = 0;
	}

	int monster_seq;
	int monster_level;
	int npc_monster_id;
	Posi npc_monster_pos;
	int add_exp;
	int add_coin;
	Dir dir_y;			// NPC转向字段
	int role_level_reward_group_id;
	std::vector<int> first_pass_reward_id_vec;
	std::vector<int> weekly_reward_id_vec;
	std::vector<ItemConfigData>  week_first_kill_reward;
};

struct MiYiLevelCfg
{
	MiYiLevelCfg()
	{
		seq = -1;
		need_role_level = 0;
		scene_id = 0;
		enter_pos = Posi(-1, -1);
		monsters.clear();
		sao_dang_need_role_level = 0;
		sao_dang_capability = 0;
		can_sao_dang = false;
		pre_task = 0;
	}

	const MiYiMonsterCfg * GetMiYiMonsterCfg(int npc_monster_id) const
	{
		for (int i = 0; i < (int)monsters.size(); ++i)
		{
			if (monsters[i].npc_monster_id == npc_monster_id)
			{
				return &monsters[i];
			}
		}

		return NULL;
	}

	int seq;
	int need_role_level;						// 开启等级
	int scene_id;
	Posi enter_pos;								// 坐标
	int sao_dang_need_role_level;
	int sao_dang_capability;	
	bool can_sao_dang;
	std::vector<MiYiMonsterCfg >  monsters;
	int pre_task;								// 前置任务
};

struct MiYiSimpleCfg
{
	MiYiSimpleCfg(int _seq, int _monster_seq): seq(_seq), monster_seq(_monster_seq) {}
	MiYiSimpleCfg() : seq(0), monster_seq(0) {}

	int seq;
	int monster_seq;
};

struct MiYiRewardCfg
{
	const static int MAX_RATE_COUNT = 10000;

	struct ItemCfg
	{
		ItemCfg() :rate(0)
		{
		}

		int rate;
		std::vector<ItemConfigData> reward;
	};

	MiYiRewardCfg()
	{
	}

	std::vector<ItemCfg> group_reward;
};

struct MiYiRoleLevelRewardCfg
{
	struct ItemCfg
	{
		ItemCfg() 
		{
			min_role_level = 0;
			max_role_level = 0;
		}

		int min_role_level;
		int max_role_level;
		std::vector<int> reward_group_id;
		std::vector<int> reward_group_id_1;
		std::vector<int> reward_group_id_2;
	};

	MiYiRoleLevelRewardCfg()
	{
	}

	std::vector<ItemCfg> group_reward;
};

// -----------------------------------------------------------------------------------
class MiYiConfig : public ILogicConfig
{
public:
	MiYiConfig();
	virtual ~MiYiConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const MiyiOtherCfg & GetMiYiOtherCfg() { return m_other_cfg; }
	const MiYiLevelCfg * GetMiYiLevelCfgBySceneId(int scene_id);
	const MiYiLevelCfg * GetMiYiLevelCfgBySeq(int seq);
	const MiYiSimpleCfg * GetMiYiLevelSimpleCfg(int monster_group_id);
	std::vector<ItemConfigData> GetRandRewardItem(std::vector <int > reward_id);	
	std::vector<ItemConfigData> GetRewardItem(int level_reward_group_id, int role_level, int pass_count = 0);
	int GetMaxRewardProportion() { return max_reward_proportion; }
	int GetRewardProportionType(int pass_count);

	const std::map<int, MiYiLevelCfg> & GetAllLevelCfg() const;
private:
	int InitLevelCfg(TiXmlElement *RootElement);
	int InitLevelCfg2(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitRoleLevelRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitRewardProportionCfg(TiXmlElement *RootElement);

	void GetRandReward(int reward_id, std::vector<ItemConfigData> & out_list);
	std::vector<int> GetRoleLevelRewardId(int level_reward_group_id, int role_level, int pass_count = 0);

	MiyiOtherCfg m_other_cfg;
	std::map<int, MiYiLevelCfg> m_level_cfg;
	std::map<int, MiYiLevelCfg> m_level_cfg_by_seq;
	std::map<int, MiYiSimpleCfg> m_level_cfg_by_monster;		
	std::map<int, MiYiRewardCfg > m_rewards_cfg;
	std::map<int, MiYiRoleLevelRewardCfg > m_role_level_reward_cfg;		//按角色不同等级给奖励
	int reward_proportion[MI_YI_MAX_REWARD_PROPORTION_NUM];				//奖励衰减-击杀BOSS总次数
	int max_reward_proportion;
};

#endif