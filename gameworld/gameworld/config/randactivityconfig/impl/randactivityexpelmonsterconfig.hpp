#ifndef __RAND_ACTIVITY_EXPLE_MONSTER_CONFIG_HPP__
#define __RAND_ACTIVITY_EXPLE_MONSTER_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raexpelmonsterparam.hpp"
#include "item/itembase.h"

struct ExpelMonsterOtherCfg
{
	ExpelMonsterOtherCfg() : reward_frequency(0) {}

	int reward_frequency;
};

struct ExpelMonsterConsumeCfg
{
	ExpelMonsterConsumeCfg() : seq(0), consume_type(0), need_gold(0) {}

	int seq;
	int consume_type;
	int need_gold;
	ItemConfigData item;
};

struct ExpelMonsterRewardCfg
{
	ExpelMonsterRewardCfg() : seq(0), seq1(0), grade_start(0), grade_end(0), hearsay(0),rate(0) {}

	int seq;
	int seq1;
	int grade_start;
	int grade_end;
	int hearsay;
	int rate;
	std::vector<ItemConfigData> item;
};

struct ExpelMonsterRewardOpenDayCfg
{
	ExpelMonsterRewardOpenDayCfg(int _day_start, int _day_end) : day_start(_day_start), day_end(_day_end)
	{}

	int day_start;
	int day_end;

	std::vector<ExpelMonsterRewardCfg> reward_cfg;
};

struct ExpelMonsterRewardSectionCfg
{
	ExpelMonsterRewardSectionCfg(int _section_start, int _section_end) : section_start(_section_start), section_end(_section_end)
	{}

	int section_start;
	int section_end;

	std::vector<ExpelMonsterRewardOpenDayCfg> open_day_cfg;
};

struct ExpelMonsterDemonWhipCfg
{
	ExpelMonsterDemonWhipCfg() : monster_id(0), item_id(0), lethal_rate(0) {}

	int monster_id;
	int item_id;
	int lethal_rate;
};

struct ExpelMonsterCoordinateCfg
{
	ExpelMonsterCoordinateCfg() : seq(0), scene_id(0), npc_id(0), monster_pos(0, 0) {}

	int seq;
	int scene_id;
	int npc_id;
	Posi monster_pos;
};

class RandActivityManager;

class RandActivityExpelMonsterConfig : public RandActivityConfig
{
public:
	RandActivityExpelMonsterConfig();
	virtual ~RandActivityExpelMonsterConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const ExpelMonsterOtherCfg & GetOtherCfg() const { return m_other_cfg; }
	const ExpelMonsterConsumeCfg * GetConsumeCfg(int seq)const;
	const ExpelMonsterRewardCfg * GetRewardCfg(int role_level, RandActivityManager * ramgr) const;
	const ExpelMonsterCoordinateCfg * GetMonsterCoordinateCfg(int seq)const;
	const std::map<int, ExpelMonsterCoordinateCfg> & GetMonsterList() const { return m_cooddinate_cfg; }
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitDispelConsumeCfg(TiXmlElement *RootElement);
	int InitDispelRewardCfg(TiXmlElement *RootElement);
	int InitMonsterCoordinate(TiXmlElement *RootElement);

	ExpelMonsterOtherCfg m_other_cfg;
	std::map<int, ExpelMonsterConsumeCfg> m_consume_cfg;
	std::vector<ExpelMonsterRewardSectionCfg> m_reward_section_cfg;
	std::map<int, ExpelMonsterDemonWhipCfg> m_demon_whip_cfg;
	std::map<int, ExpelMonsterCoordinateCfg> m_cooddinate_cfg;
};

#endif	//__RAND_ACTIVITY_EXPLE_MONSTER_CONFIG_HPP__