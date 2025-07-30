#ifndef __HUNDRED_GHOST_CONFIG_HPP__
#define __HUNDRED_GHOST_CONFIG_HPP__

#include "global/activity/activityimpl/activityhundredghost.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include <vector>
#include <map>

static const int HUNDRED_GHOST_REWARD_ALL_WEIGHT = 10000;	//奖励组ID总概率固定为10000

enum HUNDRED_GHOST_MONSTER_TYPE
{
	HUNDRED_GHOST_MONSTER_TYPE_0,		//普通鬼怪
	HUNDRED_GHOST_MONSTER_TYPE_1,		//普通鬼王
	HUNDRED_GHOST_MONSTER_TYPE_2,		//隐藏鬼王
	HUNDRED_GHOST_MONSTER_TYPE_3,		//鬼皇
};

//百鬼夜行 配置
struct HundredGhostMonsterCfg
{
	HundredGhostMonsterCfg()
	{
		seq = -1;
		scene_id = 0;
		npc_id = 0;
		monster_group_id = 0;
		monster_type = 0;
		param_1 = 0;
	}

	int seq;
	int scene_id;
	int npc_id;
	int monster_group_id;
	int monster_type;
	int param_1;
	std::vector<int>  reward_id_vec;
};

struct HundredGhostRewardCfg
{
	HundredGhostRewardCfg()
	{
		min_level = 0;
		max_level = 0;
		rate = 0;
		coin = 0;
	}

	int min_level;
	int max_level;
	ItemConfigData item;
	int rate;
	int coin;
};

struct HundredGhostOtherCfg
{
	HundredGhostOtherCfg()
	{
		max_num = 0;
		scene_num = 0;
		team_num = 0;
		fight_min_level = 0;
	}

	int max_num;						//最大怪物数量
	int scene_num;						//场景怪物数量
	int team_num;						//要求最低人数
	int fight_min_level;				//要求挑战等级
	std::vector<int>  notice_id_vec;	// 公告奖励
	int max_rounds;						// 最大回合数
};

struct HundredGhostPosiCfg
{
	HundredGhostPosiCfg() : seq(0) {}

	int seq;
	Posi pos;
};

// -----------------------------------------------------------------------------------
class HundredGhostConfig : public ILogicConfig
{
public:
	HundredGhostConfig();
	virtual ~HundredGhostConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const HundredGhostOtherCfg& GetOtherCfg()const { return m_other_cfg; }
	const HundredGhostMonsterCfg* GetMonsterCfg(int npc_id)const;
	const void GetMonsterListCfg(int num, std::vector<HundredGhostMonsterCfg>& monster_list)const;
	const void GetOneMonsterCfg(HundredGhostMonsterCfg& monster, std::vector<HundredGhostNpcInfo> info_list)const;
	const HundredGhostMonsterCfg* GetMonsterBossCfg(int death_num)const;					//用杀掉怪物数量获取隐藏鬼王
	const HundredGhostMonsterCfg* GetMonsterBigBossCfg(int death_num, std::vector<int> npc_id_list)const;			//获取鬼皇
	const void GetRewradCfg(int npc_id, int level, int* coin, std::vector<ItemConfigData>& out_reward);
	const void GetRandReward(int group_id, int level, int* coin, std::vector<ItemConfigData>& out_reward);
	const bool IsItemNotice(int item_id);
	const void GetMonsterPosiListCfg(int num, std::vector<Posi>& posi_list)const;
	const void GetOneMonsterPosiCfg(Posi& posi, std::vector<HundredGhostNpcInfo> info_list)const;
	const int GetBigBossNum()const { return m_monstar_big_boss_num; }

private:
	int InitMonsterCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitPosiCfg(TiXmlElement *RootElement);

	HundredGhostOtherCfg m_other_cfg;
	std::vector<HundredGhostMonsterCfg> m_monster_cfg;
	std::map<int, std::vector<HundredGhostRewardCfg> > m_reward_cfg;
	std::vector<HundredGhostPosiCfg> m_posi_cfg;

	int m_monstar_boss_num;				//HUNDRED_GHOST_MONSTER_TYPE_2, 隐藏鬼王数量
	int m_monstar_big_boss_num;			//HUNDRED_GHOST_MONSTER_TYPE_3, 鬼皇数量
};

#endif // !__HUNDRED_GHOST_CONFIG_HPP__
