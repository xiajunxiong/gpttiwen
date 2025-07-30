#ifndef __MINI_GAME_CONFIG_HPP__
#define __MINI_GAME_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include <map>
#include <vector>

struct MiniGameIndexCfg
{
	MiniGameIndexCfg() : mini_game_id(0), stage_reward_group(0), phase_reward_group(0) {}

	int mini_game_id;
	int stage_reward_group;
	int phase_reward_group;
	std::string mini_game_name;
};

struct MiniGameStageRewardCfg
{
	MiniGameStageRewardCfg() : group_id(0), stage_id(0) {}

	int group_id;
	int stage_id;
	std::vector<ItemConfigData> reward_list;
};

struct MiniGamePhaseRewardCfg
{
	MiniGamePhaseRewardCfg() : group_id(0), phase_seq(0), clear_stage_num(0) {}

	int group_id;
	int phase_seq;
	int clear_stage_num;
	std::vector<ItemConfigData> reward_list;
};

struct MiniGameOtherCfg
{
	MiniGameOtherCfg() : mail_award_stage_num(0) {}

	int mail_award_stage_num;
};

class MiniGameConfig : public ILogicConfig
{
public:
	typedef std::map<int, MiniGameIndexCfg> IndexCfgMap;
	typedef std::pair<int, int> StageKey;		// <group_id, stage_id>
	typedef std::map<StageKey, MiniGameStageRewardCfg> StageRewardMap;
	typedef std::pair<int, int> PhaseKey;		// <group_id, phase_seq>
	typedef std::map<PhaseKey, MiniGamePhaseRewardCfg> PhaseRewardMap;

public:
	MiniGameConfig();
	virtual ~MiniGameConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const MiniGameIndexCfg* GetIndexCfg(int minigame_id) const;
	bool HasNextStage(int minigame_id, int clear_stage_num);
	const MiniGameStageRewardCfg* GetStageRewardCfg(int mini_game_id, int stage_id);
	const MiniGamePhaseRewardCfg* GetPhaseRewardCfg(int mini_game_id, int phase_id);
	const MiniGameOtherCfg& GetOtherCfg() const;
	int GetAwardStageNum();
private:
	int InitIndexCfg(TiXmlElement* root_element, std::string& err);
	int InitStageRewardCfg(TiXmlElement* root_element, std::string& err);
	int InitPhaseRewardCfg(TiXmlElement* root_element, std::string& err);
	int InitOtherCfg(TiXmlElement* root_element, std::string& err);

	const MiniGamePhaseRewardCfg* GetPhaseRewardCfg(const PhaseKey& key);
	const MiniGameStageRewardCfg* GetStageRewardCfg(const StageKey& key);

private:
	IndexCfgMap m_index_cfg_map;
	StageRewardMap m_stage_reward_map;
	PhaseRewardMap m_phase_reward_map;
	MiniGameOtherCfg m_other_cfg;
};

#endif