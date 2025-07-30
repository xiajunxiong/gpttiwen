#ifndef __CHALLENGE_FB_CONFIG_HPP__
#define __CHALLENGE_FB_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/rolechallengefbdef.hpp"

#include <map>
#include <set>

struct ChallengeFbOthers
{
	ChallengeFbOthers() : min_mem_num(0) {}

	int min_mem_num;
};

struct ChallChapterInfor
{
	ChallChapterInfor()
	{
		fb_id = 0;
		chapter = 0;
		level_limit = 0;
		scene_id = 0;
		play_type = 0;
		param1_list_num = 0;
		memset(param1_list, 0, sizeof(param1_list));
		param2 = 0;
		item_limit_group = 0;
		reward_num = 0;
	}

	int fb_id;
	int chapter;
	int level_limit;
	int scene_id;
	int play_type;
	int param1_list_num;
	int param1_list[PARAM_1_LIST_COUNT];
	int param2;
	int item_limit_group;		// 道具限制组
	int reward_num;
	ItemConfigData chapter_reward[ITEM_REWARD_LIST_COUNT];
};

struct ChallEctypeReward
{
	ChallEctypeReward() : reward_num(0) {}

	int reward_num;
	ItemConfigData ectype_reward[ITEM_REWARD_LIST_COUNT];
};

struct ChallMonsterCfg
{
	ChallMonsterCfg(int _npc_id = 0, int _x = 0, int _y = 0) : npc_id(_npc_id), posi(_x, _y) {}

	int npc_id;
	Posi posi;

	bool operator < (const ChallMonsterCfg & cobj) const
	{
		return this->npc_id < cobj.npc_id;
	}

	bool operator == (const ChallMonsterCfg & cobj) const
	{
		return this->npc_id == cobj.npc_id;
	}
};

class ChallengeFbConfig : public ILogicConfig
{
public:
	ChallengeFbConfig();
	virtual ~ChallengeFbConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ChallengeFbOthers & GetChallFbOtherCfg() { return m_chall_other; }
	const ChallChapterInfor * GetChapterInfor(int fb_id, int chapter);
	const ChallEctypeReward * GetEctypeReward(int fb_id);

	const ChallMonsterCfg * RandAMonsterCfg(int scene_id);
	bool GetMonsterCfg(int scene_id, std::set<ChallMonsterCfg> & out_list);
	int GetItemUseTimesLimit(int fb_id, int chapter, int use_type);

private:
	int InitChallOthersCfg(TiXmlElement * RootElement);
	int InitChallInformCfg(TiXmlElement * RootElement);
	int InitStageRewardCfg(TiXmlElement * RootElement);
	int InitMonsterInfoCfg(TiXmlElement * RootElement);
	int InitItemUseLimitCfg(TiXmlElement * RootElement);

	ChallengeFbOthers m_chall_other;
	std::map<int, std::map<int, ChallChapterInfor> > m_chapter_infor_map;
	std::map<int, ChallEctypeReward> m_ectype_reward_map;
	std::map<int, std::set<ChallMonsterCfg> > m_monster_map;
	std::map<int, std::map<int, int> > m_item_use_limit_map;
};

#endif