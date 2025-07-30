#ifndef __ELEMENT_ELF_CONFIG_HPP__
#define __ELEMENT_ELF_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include <set>

struct ElementElfRewardCfg
{
	ElementElfRewardCfg() : seq(0), rate(0) {}

	int seq;
	std::vector<ItemConfigData> rewards;
	int rate;
};

struct ElementElfShowUpTimesCfg
{
	int show_up_rate;
	int reward_group;
};

class ElementElfConfig : public ILogicConfig
{
public:
	static const int PERCENT_NUM = 10000;

public:
	ElementElfConfig();
	virtual ~ElementElfConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	bool RandElementElfShowUp(int monster_group_id, int battle_mode, int today_showup_times, int* out_reward_group) const;
	int RandMonsterId() const;
	const ElementElfRewardCfg* RandRewardCfg(int reward_group) const;

private:
	int InitElementElfMonsterList(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitMonsterGroupCfg(TiXmlElement *RootElement);
	int InitShowUpRateCfg(TiXmlElement* RootElement);
	int InitShowUpBattleMode(TiXmlElement* RootElement);
	bool IsShowUp(int today_showup_times, int* out_reward_group) const;
	bool CheckRewardGroupCfg(std::string* errmsg) const;
	bool CheckShowUpCfg(std::string* errmsg) const;

	std::vector<int> m_element_elf_monsters;
	std::map<int, std::vector<ElementElfRewardCfg> > m_reward_group_cfg;
	std::set<int> m_monster_group_noshow_set;
	std::vector<ElementElfShowUpTimesCfg> m_show_up_cfgs;
	std::set<int> m_showup_battle_mode_set;
};

#endif