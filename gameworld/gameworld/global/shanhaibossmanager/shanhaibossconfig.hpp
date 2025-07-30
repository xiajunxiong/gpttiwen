#ifndef _SHAN_HAI_BOSS_CONFIG_HPP_
#define _SHAN_HAI_BOSS_CONFIG_HPP_

#include "servercommon/configcommon.h"
#include "gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include <vector>
#include <map>
#include <set>

static const int SHAN_HAI_MONSTER_ONE_TYPE_MAX = 4;		//同一种类型的怪物四只(普通)(困难)(地狱)(噩梦)
static const int SHAN_HAI_REWARD_ALL_WEIGHT = 10000;	//山海奖励组ID总概率固定为10000

enum SHAN_HAI_BOSS_GRADE_TYPE
{
	SHAN_HAI_BOSS_GRADE_TYPE_INVAILD = 0,
	SHAN_HAI_BOSS_GRADE_TYPE_EASY = 1,				//简单(普通)	
	SHAN_HAI_BOSS_GRADE_TYPE_DIFFICULT = 2,			//困难
	SHAN_HAI_BOSS_GRADE_TYPE_HELL = 3,				//地狱
	SHAN_HAI_BOSS_GRADE_TYPE_NIGHTMARE = 4,			//噩梦

	SHAN_HAI_BOSS_GRADE_TYPE_MAX
};

struct ShanHaiBossMonsterCfg
{
	ShanHaiBossMonsterCfg() : seq(0), npc_id(0), npc_type(0), monster_level(0), monster_group_id(0), fight_times(0), scene_group_id(0), poker_reward_id(0), poker_dec_reward_id(0) {}

	int seq;
	int npc_id;
	int npc_type;
	int monster_level;							//(无效)
	int monster_group_id;
	int fight_times;
	int scene_group_id;
	std::vector<int>  reward_id_vec;			// 正常奖励
	int poker_reward_id;						// 翻牌奖励(无效)
	int poker_dec_reward_id;					// 翻牌衰减奖励(改为正常衰减奖励 翻牌奖励取消)
	int grade_type;								// 难度
};

struct ShanHaiBossSceneGroupCfg
{
	ShanHaiBossSceneGroupCfg() : scene_id(0), rate(0), pos_group_id(0) {}

	int scene_id;
	int rate;
	int pos_group_id;
};
struct ShanHaiBossSceneGroupListCfg
{
	ShanHaiBossSceneGroupListCfg() : all_rate(0) {}

	int all_rate;
	std::vector<ShanHaiBossSceneGroupCfg> node;
};

struct ShanHaiBossPosGroupCfg
{
	ShanHaiBossPosGroupCfg() {}

	Posi boss_pos;
};


struct ShanHaiBossRewardCfg
{
	ShanHaiBossRewardCfg() : coin(0), luck(0), rate(0) {}

	int min_level;
	int max_level;
	int coin;
	int luck;
	int rate;
	ItemConfigData item;
};

struct ShanHaiBossOtherCfg
{
	ShanHaiBossOtherCfg() {}

	int day_time;
	int close_begin_time;
	int close_end_time;
	int monster_type_coune;
	int live_time;
	int limit_team_people;
	std::vector<int>  day_monster[7];	//一周每天的怪物类列表
	std::vector<int>  notice_id_vec;	// 公告奖励
};

class ShanHaiBossConfig : public ILogicConfig
{
public:
	ShanHaiBossConfig();
	~ShanHaiBossConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ShanHaiBossOtherCfg& GetOtherCfg() { return m_other_cfg; }
	const ShanHaiBossMonsterCfg* GetMonsterCfg(int monster_npc_id)const;

	const void GetMonsterListCfg(int wday, std::vector<ShanHaiBossMonsterCfg> &out_boss_list, std::vector<int> &out_scene_list, std::set<int> &out_scene_set_list, std::vector<Posi> &out_pos_list)const;
	const void GetMonsterCfgByType(int monster_type, std::vector<ShanHaiBossMonsterCfg> &out_boss_list, std::vector<int> &out_scene_list, std::set<int> &out_scene_set_list, std::vector<Posi> &out_pos_list)const;
	const void RandSceneCfg(int scene_group_id, int count, std::vector<ShanHaiBossMonsterCfg> &out_boss_list, std::vector<int> &out_scene_list, std::set<int> &out_scene_set_list, std::vector<Posi> &out_pos_list)const;
	const void RandPosiCfg(int scene_id, int posi_group_id, int count, std::vector<ShanHaiBossMonsterCfg> &out_boss_list, std::vector<int> &out_scene_list, std::vector<Posi> &out_pos_list)const;

	void GetRandRewardItem(int monster_npc_id, int* coin, int* luck_coin, std::vector<ItemConfigData> &out_reward, int role_level, bool use_dec_reward);
	void GetPokerRewardItem(int monster_npc_id, int* coin, int* luck_coin, std::vector<ItemConfigData> &out_reward, bool use_dec_reward = false);//(取消翻牌)
	void GetRandReward(int reward_id, std::vector<ItemConfigData> & out_list, int *coin, int *luck_coin, int role_level);
	int  GetInitiateCfg(int grade_type)const;

	bool IsGetItemNotice(int item_id);

private:
	int InitMonsterCfg(TiXmlElement *RootElement);
	int InitSceneCfg(TiXmlElement *RootElement);
	int InitPosCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitInitiateCfg(TiXmlElement *RootElement);

	ShanHaiBossOtherCfg m_other_cfg;
	std::vector<ShanHaiBossMonsterCfg> m_monster_cfg;
	std::map<int, ShanHaiBossSceneGroupListCfg> m_scene_cfg;				// key-场景组ID
	std::map<int, std::vector<ShanHaiBossPosGroupCfg> > m_pos_cfg;			// key-坐标组ID
	std::map<int, std::vector<ShanHaiBossRewardCfg> > m_reward_cfg;			// key-奖励组ID
	std::map<int, int>  m_initiate_cfg;										// key-怪物难度
};

#endif

