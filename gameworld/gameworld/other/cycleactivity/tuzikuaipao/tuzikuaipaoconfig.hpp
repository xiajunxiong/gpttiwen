#ifndef __TU_ZI_KUAI_PAO_CONFIG_HPP__
#define __TU_ZI_KUAI_PAO_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "gamedef.h"

#include <set>

struct TuZiKuaiPaoTuZiCfg
{
	int tu_iz_type;
	int speed;
	int rand_run_min_sec;
	int rand_run_max_sec;
	int rest_sec;
};

struct TuZiKuaiPaoEncourageCfg
{
	int encourage_type;
	int has_add_speed;
	std::vector<int> succ_rate;
};

struct TuZiKuaiPaoOtherCfg
{
	int scene_id;
	int reward_times;
	int use_carrot_max_times;
	int add_speed_sec;
	int carrot_add_speed;
	std::vector<ItemConfigData> win_rewards;
	std::vector<ItemConfigData> lose_rewards;
};

struct TuZiKuaiPaoTrackPointGroup
{
	Posi start_pos;
	Posi end_pos;
};

struct TuZiKuaiPaoTrackCfg
{
	int track_id;
	std::map<int, TuZiKuaiPaoTrackPointGroup> point_cfg;
};

class TuZiKuaiPaoConfig : public ILogicConfig
{
public:
	TuZiKuaiPaoConfig();
	virtual ~TuZiKuaiPaoConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const TuZiKuaiPaoTuZiCfg * GetTuZiKuaiPaoTuZiCfg(int tu_zi_type);
	const TuZiKuaiPaoEncourageCfg * GetTuZiKuaiPaoEncourageCfg(int encourage_type);
	const TuZiKuaiPaoTrackCfg * GetTuZiKuaiPaoTrackCfg(int track_type);
	const TuZiKuaiPaoOtherCfg & GetTuZiKuaiPaoOtherCfg();
	std::set<int> GetAllTuZiType();
private:
	int InitTuZiKuaiPaoEncourageCfg(TiXmlElement * RootElement);
	int InitTuZiKuaiPaoTrackCfg(TiXmlElement * RootElement);
	int InitTuZiKuaiPaoTuZiCfg(TiXmlElement * RootElement);	
	int InitOtherCfg(TiXmlElement * RootElement);

	std::map<int, TuZiKuaiPaoTrackCfg> m_track_cfg;
	std::map<int, TuZiKuaiPaoTuZiCfg> m_tu_zi_cfg;
	std::map<int, TuZiKuaiPaoEncourageCfg> m_encourage_cfg;
	TuZiKuaiPaoOtherCfg m_other_cfg;


	std::set<int> m_all_tu_zi_type;
};


#endif