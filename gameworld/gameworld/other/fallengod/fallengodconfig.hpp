#ifndef __FALLEN_GOD_CONFIG_HPP__
#define __FALLEN_GOD_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/commonsavedef.h"
#include "servercommon/bigdipperdef.h"

#include <set>
#include <map>
#include <vector>

struct FallenGodBossCfg
{
	FallenGodBossCfg() :sn(0), monster_guoup(0), type(0), help_times(0), pass_type(0), pass_sn(0) {}
	enum MONSTER_TYPE
	{
		MONSTER_TYPE_0 = 0,		//ÐÇ¾ý
		MONSTER_TYPE_1 = 1,		//µÛ¾ý
	};
	enum PASS_TYPE
	{
		PASS_TYPE_0 = 0,		// ±±¶·ÐÇ¾ý
		PASS_TYPE_1 = 1,		// éæÉñÖ®µØ
	};
	int sn;
	int monster_guoup;
	int type;
	std::vector<short> open_time_list;
	std::vector<ItemConfigData> reward_list;

	int help_times;
	std::vector<ItemConfigData> help_reward;
	int pass_type;
	int pass_sn;
};

struct FallenGodOtherCfg
{
	FallenGodOtherCfg() :challenge_num(0), title_id(0), scene_id(0), reward_rank(0), last_times(0){}
	int challenge_num;
	int title_id;
	int scene_id;
	int	reward_rank;
	int last_times;
};

class FallenGodConfig : public ILogicConfig
{
public:
	FallenGodConfig();
	virtual ~FallenGodConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const FallenGodOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const FallenGodBossCfg * GetBaseCfg(int sn);

private:
	int InitBaseCfg(TiXmlElement * RootElement);
	int InitOhterCfg(TiXmlElement * RootElement);

	FallenGodOtherCfg m_other_cfg;

	//Ëæ»úÐÇ¾ý
	std::vector<FallenGodBossCfg> m_boss_vec;
};

#endif