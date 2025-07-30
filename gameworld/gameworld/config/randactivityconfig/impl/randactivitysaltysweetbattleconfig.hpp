#ifndef __RAND_ACTIVITY_SALTY_SWEET_BATTLE_CONFIG_HPP__
#define __RAND_ACTIVITY_SALTY_SWEET_BATTLE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/activitydef.hpp"

struct SaltySweetBaseCfg
{
	SaltySweetBaseCfg() : type(0), section_start(0), section_end(0), vote_reward_group(0) {}

	int type;
	int section_start;
	int	section_end;

	std::vector<ItemConfigData> win_reward_vec;
	std::vector<ItemConfigData> lose_reward_vec;

	int vote_reward_group;
};

struct SaltySweetVoteCfg
{
	SaltySweetVoteCfg() : seq(0), group(0), rate(0) {}

	int seq;
	int group;
	int rate;

	ItemConfigData reward;
};

struct SaltySweetOtherCfg
{
	SaltySweetOtherCfg() : times(0), salty_add_vote(0), sweet_add_vote(0), consume_item(0), vote_time_end(0), settlement_times(0) {}

	int times;
	int salty_add_vote;
	int sweet_add_vote;
	int	consume_item;
	int	vote_time_end;
	int	settlement_times;
};

class RandActivityManager;
class RandActivitySaltySweetBattleConfig : public RandActivityConfig
{
public:
	RandActivitySaltySweetBattleConfig();
	virtual ~RandActivitySaltySweetBattleConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	const SaltySweetOtherCfg & GetOtherCfg() const { return other_cfg; }
	const SaltySweetBaseCfg * GetBaseCfg(int open_day) const;
	const SaltySweetVoteCfg * GetVoteCfg(int group_id) const;
private:
	int InitBaseCfg(TiXmlElement *RootElement);
	int InitVoteCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	SaltySweetOtherCfg other_cfg;
	std::vector<SaltySweetBaseCfg> base_cfg;
	std::vector<SaltySweetVoteCfg> vote_cfg;
};

#endif
