#ifndef __ACTIVITY_BRAVE_GROUND_CONFIG__
#define __ACTIVITY_BRAVE_GROUND_CONFIG__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/commonsavedef.h"

#include <set>
#include <map>
#include <vector>

struct BraveGroundOtherCfg
{
	BraveGroundOtherCfg()
	{
		need_role_level = 1;
		need_role_num = 3;

		for (int i = 0; i < ARRAY_LENGTH(top_role_title); ++i)
		{
			top_role_title[i] = 0;
		}
	}

	int need_role_level;
	int need_role_num;
	int top_role_title[BRAVE_GROUND_TOP_ITEM_NUM];
};

struct BraveGroundLevelCfg
{
	BraveGroundLevelCfg()
	{
		need_world_level = 0;
		level = 0;
		role_level = 0;
		can_sao_dang = 0;
		scene_id = 0;
		boss_monster_npc_id = 0;
		npc_group_id = 0;
		monster_npc_id = 0;
		m_max_monster_num = 0;
		sceneborn_group_id = 0;
		birth_pos = Posi(0, 0);
		transport_pos = Posi(0, 0);
	}

	int GetCreateMonsterRandNum() const
	{
		int monster_num = 0;
		if (m_max_monster_num > 0)
		{
			monster_num = rand() % m_max_monster_num;
		}

		return monster_num;
	}

	int need_world_level;
	int level;
	int role_level;
	bool can_sao_dang;		//ÄÜ²»ÄÜÉ¨µ´
	int scene_id;
	int boss_monster_npc_id;
	Posi boss_pos;
	Dir boss_dir;
	
	//Ã¿²ã¸øµÄ½±Àøid
	int reward_id;
	//-----------------

	int npc_group_id;
	int monster_npc_id;
	int m_max_monster_num;
	int sceneborn_group_id;
	Posi birth_pos;
	Posi transport_pos;
	std::vector<int> drop_id_list;			//¾«Ó¢¹ÖµôÂä
};

struct BraveGroundLevelRewardCfg 
{
	BraveGroundLevelRewardCfg()
	{
		reward_id = 0;
		min_role_level = 0;
		max_role_level = 0;
		exp_reward = 0;
		coin = 0;
	}

	int reward_id;
	int min_role_level;
	int max_role_level;

	//Ã¿²ã¸øµÄ½±Àø
	int exp_reward;
	int coin;
	std::vector<ItemConfigData> rewards;
};


struct BraveGroundNpcCfg
{
	enum BRAVE_GROUND_NPC_TYPE
	{
		BRAVE_GROUND_NPC_TYPE_RECOVER = 1,
		BRAVE_GROUND_NPC_TYPE_TRANSPORT = 2,

	};

	BraveGroundNpcCfg()
	{
		npc_id = 0;
		npc_fun_type = 0;
		param1 = 0;
		param2 = 0;
		param3 = 0;
	}

	int npc_id;
	int npc_fun_type;
	int param1;
	int param2;
	int param3;
};

struct BraveGroundNpcGroupCfg
{
	const static int MAX_RATE_COUNT = 10000;

	struct ItemCfg
	{
		ItemCfg() :rate(0)
		{
			npc_id = 0;
		}

		int rate;
		int npc_id;
	};

	BraveGroundNpcGroupCfg()
	{
	}

	std::vector<ItemCfg> group_cfg;
};

struct BraveGroundRankRewardCfg
{
	BraveGroundRankRewardCfg()
	{
		min_role_level = 0;
		max_role_level = 0;
	}

	struct ItemCfg
	{
		ItemCfg()
		{
			rank_min = 0;
			rank_max = 0;
		}

		int rank_min;
		int rank_max;
		std::vector<ItemConfigData> rewards;
	};

	int min_role_level;
	int max_role_level;
	std::vector<ItemCfg> rewards;
};

struct BraveGroundPosCfg
{
	const static int MAX_RATE_COUNT = 10000;

	struct ItemCfg
	{
		ItemCfg()
		{
			rate = 0;
			birth_pos = Posi(0, 0);
			transport_pos = Posi(0, 0);
		}

		int rate;
		Posi birth_pos;
		Posi transport_pos;
	};

	BraveGroundPosCfg ()
	{
	}

	std::vector<ItemCfg> group_cfg;
};

struct  ActivityBraveGroundBuffCfg
{
	const static int MAX_RATE_COUNT = 10000;

	ActivityBraveGroundBuffCfg()
	{
		rate_count = 0;
	}

	struct ItemCfg
	{
		ItemCfg()
		{
			rate = 0;
			buff_id = 0;
		}

		int rate;
		int buff_id;
	};

	int rate_count ;
	std::vector<ItemCfg> group_cfg;
};

struct  ActivityBraveGroundSaoDangReward		//É¨µ´½±Àø
{
	ActivityBraveGroundSaoDangReward()
	{
		exp = 0;
		coin = 0;
	}

	int exp;
	int coin;
	std::vector<ItemConfigData> rewards;
};

struct ActivityBraveGroundUseItemLimitCfg
{
	ActivityBraveGroundUseItemLimitCfg()
	{
	}

	std::map<int, int> use_type_to_limit_times;
};

class ActivityBraveGroundConfig : public ILogicConfig
{
public:
	ActivityBraveGroundConfig();
	virtual ~ActivityBraveGroundConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const BraveGroundLevelCfg * GetLevelCfg(int level, bool get_pos = false);
	const BraveGroundLevelRewardCfg * GetLevelRewardCfg(int level, int role_level);
	const BraveGroundNpcCfg * GetRandNpcCfg(int npc_group_id);
	const BraveGroundNpcCfg * GetNpcCfg(int npc_id);
	const BraveGroundOtherCfg & GetOtherCfg();
	const BraveGroundRankRewardCfg::ItemCfg * GetRankRewardCfg(int rank,int role_level);
	int GetSaoDangRewardLevel(int max_reached_level, int role_level);
	ActivityBraveGroundSaoDangReward GetSaoDangLevelReward(int level, int role_level);
	int GetRandBuffId();
	int GetLastLevel();
	int GetCfgMaxLevel();
	int GetMaxSelectLevel(int role_level, int curr_reached_level, int max_reached_level);

	int GetUseItemLimitTimes(int use_limit_group_id, int use_type);
	int IsLimitTimesUseType(int use_limit_group_id, int use_type);
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitLevelCfg(TiXmlElement *RootElement);
	int InitNpcGroupCfg(TiXmlElement *RootElement);
	int InitNpcCfg(TiXmlElement *RootElement);
	int InitRankRewardCfg(TiXmlElement *RootElement);
	int InitPosCfg(TiXmlElement *RootElement);
	int InitBuffCfg(TiXmlElement * RootElement);
	int InitLevelRewardCfg(TiXmlElement * RootElement);
	//-----------------------
	void RefreshPos(BraveGroundLevelCfg ARG_OUT & cfg);

	std::vector<BraveGroundLevelCfg> m_level_cfg;
	std::map<int, BraveGroundNpcGroupCfg> m_npc_group_cfg;
	std::map<int, BraveGroundNpcCfg> m_npc_cfg;
	std::vector<BraveGroundRankRewardCfg> m_rank_reward_cfg;
	BraveGroundOtherCfg m_othter_cfg;
	std::map<int, BraveGroundPosCfg> m_pos_cfg;
	ActivityBraveGroundBuffCfg m_buff_cfg;

	std::map<int, ActivityBraveGroundUseItemLimitCfg> m_use_item_limit_cfg;
	std::map<int, std::vector<BraveGroundLevelRewardCfg> > m_level_reward_cfg;
};

#endif 