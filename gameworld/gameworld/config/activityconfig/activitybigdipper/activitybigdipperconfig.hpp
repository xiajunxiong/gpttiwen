#ifndef __ACTIVITY_BIG_DIPPER_CONFIG__
#define __ACTIVITY_BIG_DIPPER_CONFIG__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/commonsavedef.h"
#include "servercommon/bigdipperdef.h"

#include <set>
#include <map>
#include <vector>

struct BigDipperOtherCfg
{
	BigDipperOtherCfg() : open_num(0) {}
	int open_num;
};

struct BigDipperCfg
{
	BigDipperCfg() : sn(0), npc_seq(0), monster_guoup(0), type(0), monster_stage_reward_num(0), help_times(0) {}
	enum MONSTER_TYPE
	{
		MONSTER_TYPE_0 = 0,		//ÐÇ¾ý
		MONSTER_TYPE_1 = 1,		//µÛ¾ý

		MONSTER_TYPE_MAX,
	};
	int sn;
	int	npc_seq;
	int	monster_guoup;
	int type;
	std::vector<short> open_time_list;
	std::vector<ItemConfigData>	first_reward;
	int monster_stage_reward_num;
	std::vector<ItemConfigData> reward_list[MAX_BIG_DIPPER_MONSTER_REWARD_INDEX];

	int help_times;
	std::vector<ItemConfigData> help_reward;
};

class ActivityBigDipperConfig : public ILogicConfig
{
public:
	ActivityBigDipperConfig();
	virtual ~ActivityBigDipperConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const BigDipperOtherCfg & GetOtherCfg() { return other_cfg; }
	const BigDipperCfg * GetBaseCfg(int sn);

	void GetRandMonsterCfg(int monster_num, std::vector<BigDipperCfg>* rand_list);
private:
	int InitBaseCfg(TiXmlElement * RootElement);
	int InitOhterCfg(TiXmlElement * RootElement);

	BigDipperCfg * base_cfg[MAX_BIG_DIPPER_MONSTER_NUM];

	BigDipperOtherCfg other_cfg;

	//Ëæ»úÐÇ¾ý
	std::vector<BigDipperCfg*> rand_monst_cfg;
	std::vector<BigDipperCfg*> rand_boss_cfg;

	std::vector<BigDipperCfg> m_monster_vec;
};

#endif 