#ifndef _LUN_HUI_WANG_CHUAN_CONFIG_HPP_
#define _LUN_HUI_WANG_CHUAN_CONFIG_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/lunhuiwangchuandef.h"
#include "servercommon/configcommon.h"

struct LunHuiWangChuanOtherCfg
{
	LunHuiWangChuanOtherCfg() :refresh_day(0) {}
	int refresh_day;
};

struct LunHuiWangChuanPassRewardCfg
{
	LunHuiWangChuanPassRewardCfg() :role_level(0), challenge_id(0){}

	int role_level;
	int	challenge_id;
	std::vector<ItemConfigData> reward_list;
};

struct LunHuiWangChuanPassRewardListCfg
{
	LunHuiWangChuanPassRewardListCfg() {}
	LunHuiWangChuanPassRewardCfg pass_list[MAX_ROLE_LEVEL];
};

struct LunHuiWangChuanRewardCfg
{
	LunHuiWangChuanRewardCfg() : is_value(false), role_level(0), layer(0){}

	bool is_value;
	int	role_level;
	int	layer;
	std::vector<ItemConfigData> reward_list;
};

struct LunHuiWangChuanMonsterGroupCfg
{
	LunHuiWangChuanMonsterGroupCfg() :is_value(false), challenge_id(0), monster_group_id_1(0), monster_group_id_2(0) {}
	bool is_value;
	int challenge_id;
	int	monster_group_id_1;
	int	monster_group_id_2;
};

class RoleModuleManager;
class LunHuiWangChuanConfig : public ILogicConfig
{
public:
	LunHuiWangChuanConfig();
	virtual ~LunHuiWangChuanConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const LunHuiWangChuanOtherCfg & GetLunHuiWangChuanOhterCfg() { return m_other; }
	const LunHuiWangChuanPassRewardCfg * GetLunHuiWangChuanPassRewardCfg(int challenge_id, int level);
	const LunHuiWangChuanMonsterGroupCfg * GetLunHuiWangChuanMonsterDetailCfg(int challenge_id);
	const LunHuiWangChuanRewardCfg * GetLunHuiWangChuanRewardCfg(int role_level, int seq);
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitPassRewardGroupCfg(TiXmlElement * RootElement);
	int InitMonsterDetailCfg(TiXmlElement * RootElement);
	int InitLayerCfg(TiXmlElement * RootElement);

	LunHuiWangChuanOtherCfg m_other;
	std::map<int, LunHuiWangChuanPassRewardListCfg> m_pass_map;
	LunHuiWangChuanMonsterGroupCfg m_monster_group_map[MAX_LUN_HUI_WANG_CHUAN_INDEX];
	LunHuiWangChuanRewardCfg m_reward_map[MAX_ROLE_LEVEL][MAX_LUN_HUI_WANG_CHUAN_INDEX];
};
#endif
