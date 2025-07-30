#ifndef __RAND_ACTIVITY_CHARISMATIC_TANABATA_CONFIG_HPP__
#define __RAND_ACTIVITY_CHARISMATIC_TANABATA_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itempool.h"
#include "servercommon/activitydef.hpp"

struct CharismaticTanabataBaseConfig
{
	CharismaticTanabataBaseConfig(): add_intimate(0) {}

	int	add_intimate;
	std::vector<ItemConfigData> reward_vec;
};

struct CharismaticTanabataBaseListConfig
{
	CharismaticTanabataBaseListConfig() : start_section(0), end_section(0){}
	enum BASE_TYPE
	{
		BASE_TYPE_LOVE = 0,//°®Ä½Öµ
		BASE_TYPE_MEILI = 1,//÷ÈÁ¦Öµ
		BASE_TYPE_MAX,
	};
	int	start_section;
	int	end_section;
	std::map<int, CharismaticTanabataBaseConfig> reward_type_map[BASE_TYPE_MAX];
};

struct CharismaticTanabataRewardConfig
{
	CharismaticTanabataRewardConfig() : start_section(0), end_section(0), seq(0), ranking(0) {}
	int	start_section;
	int	end_section;
	int	seq;
	int	ranking;
	std::vector<ItemConfigData> reward_vec;
};

struct CharismaticTanabataItemConfig
{
	CharismaticTanabataItemConfig() :seq(0), charm_item(0), add_charm(0) {}
	int seq;
	int charm_item;
	int add_charm;
};

class RandActivityManager;
class RandActivityCharismaticTanabataConfig : public RandActivityConfig
{
public:
	RandActivityCharismaticTanabataConfig();
	virtual ~RandActivityCharismaticTanabataConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	const CharismaticTanabataBaseConfig * GetBaseCfg(int open_day, int reward_type, int seq)const;
	const CharismaticTanabataRewardConfig * GetRewardCfg(int open_day, int rank_idx)const;
private:
	int InitBaseCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);

	int GetKeyByTimeBeginEnd(int begin_day, int end_day) { return begin_day * 1000 + end_day; }
	void GetDayDataByKey(int key, int *begin_day, int *end_day) const;

	std::map<int, CharismaticTanabataBaseListConfig> base_map;
	std::vector<CharismaticTanabataRewardConfig> reward_vec;
};

#endif