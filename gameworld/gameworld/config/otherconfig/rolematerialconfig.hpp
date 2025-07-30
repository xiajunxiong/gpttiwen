#ifndef __ROLE_MATERIAL_CONFIG_HPP__
#define __ROLE_MATERIAL_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/rolematerialdef.hpp"
#include "servercommon/struct/itemlistparam.h"

struct MaterialOtherCfg
{
	MaterialOtherCfg() : pass_time_max(0), buy_times_max(0), extra_buy_consume(0), use_power(0)
	{}

	int pass_time_max;			//每天通关次数上限
	int buy_times_max;			//每日额外购买次数上限
	int extra_buy_consume;		//额外购买消耗元宝
	int use_power;				//消耗的体力
};

struct MaterialSceneCfg
{
	MaterialSceneCfg()
	{
		seq = 0;
		chapter_id = 0;
		level_limit = 0;
		level_seq = 0;
		memset(condition, 0, sizeof(condition));
	//	memset(achievement_condition, 0, sizeof(achievement_condition));
		scene_id = 0;
		boss_id = 0;
		boss_pos = Posi(0, 0);
		boss_dir = 0;

		npc_1_id = 0;
		npc_1_pos = Posi(0, 0);
		npc_1_dir = 0;
		is_need_talk_1 = 0;
		npc_2_id = 0;
		npc_2_pos = Posi(0, 0);
		npc_2_dir = 0;
		is_need_talk_2 = 0;
		npc_3_id = 0;
		npc_3_pos = Posi(0, 0);
		npc_3_dir = 0;
		is_need_talk_3 = 0;
	}

	int seq;				//副本seq
	int chapter_id;
	int level_limit;
	int level_seq;
	int condition[MAX_MATERIAL_CONDITON_NUM];

//	int achievement_condition[MAX_MATERIAL_ACHIEVENMENT_CONDITION_NUM];
//	ItemConfigData achievement_reward[MAX_MATERIAL_ACHIEVENMENT_CONDITION_NUM];

	int scene_id;
	int boss_id;
	Posi boss_pos;
	Dir boss_dir;
	std::vector<ItemConfigData> first_reward_list;
	std::vector<int> common_reward_group_list;

	int npc_1_id;
	Posi npc_1_pos;
	Dir npc_1_dir;
	int is_need_talk_1;
	int npc_2_id;
	Posi npc_2_pos;
	Dir npc_2_dir;
	int is_need_talk_2;
	int npc_3_id;
	Posi npc_3_pos;
	Dir npc_3_dir;
	int is_need_talk_3;
};

struct MaterialRewardGroupCfg
{
	static const int MAX_RATE_COUNT_NUM = 10000;
	MaterialRewardGroupCfg() : rate(0)
	{}

	int rate;
	ItemConfigData reward;
};

struct MaterialBoxRewardCfg
{
	MaterialBoxRewardCfg() : chapter_id(0), max_score(0), coin_reawrd(0)
	{}

	int chapter_id;
	int max_score;
	int coin_reawrd;
	std::vector<ItemConfigData> reward_list;
};

class MaterialConfig : public ILogicConfig
{
public:
	MaterialConfig();
	virtual ~MaterialConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	
	const MaterialOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const MaterialSceneCfg * GetSceneCfg(int seq);
	const MaterialSceneCfg * GetSceneCfg(int chapter_id, int level_seq);
	int GetChapterLevelNum(int chapter_id);
	void GetPassReward(std::vector<int> reward_group_list, std::vector<ItemConfigData> * reward_list);
	const MaterialBoxRewardCfg * GetBoxRewardCfg(int seq);
	int GetExpCfg(int role_level);
	int GetBuyCfg(int buy_times);

private:

	int InitOtherCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);
	int InitBoxRewardCfg(TiXmlElement *RootElement);
	int InitSceneCfg(TiXmlElement *RootElement);
	int InitExpCfg(TiXmlElement *RootElement);
	int InitBuyCfg(TiXmlElement *RootElement);

	MaterialOtherCfg m_other_cfg;
	std::vector<MaterialSceneCfg> m_scene_vec;									//副本配置 : 副本seq
	std::map<int, std::vector<MaterialSceneCfg> > m_scene_map;					//副本配置 key : 章节ID second :level_seq(关卡)
	std::map<int, std::vector<MaterialRewardGroupCfg> > m_reward_group_cfg;		//奖励组配置 key:通关奖励组ID
	std::vector<MaterialBoxRewardCfg> m_box_reward_cfg;							//宝箱奖励 
	std::map<int, int> m_exp_cfg;												//经验配置 key:min_level * 1000 + max_level
	std::map<int, int> m_buy_cfg;												//元宝购买次数配置 key:次数 second:price
};

#endif