#ifndef __RAND_ACTIVITY_SERVER_COMPETITION_CONFIG_HPP__
#define __RAND_ACTIVITY_SERVER_COMPETITION_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/roleactivity/raservercompetitionparam.hpp"

static const int MAX_RA_SERVER_COMPETITION_CFG_NUM = 10;		//版号数量

struct RAServerCompetitionOtherCfg
{
	RAServerCompetitionOtherCfg() 
	{}

	std::vector<ItemConfigData> day_reward_list;
};

enum SERVER_COMPETITION_REWARD_TYPE
{
	SERVER_COMPETITION_REWARD_TYPE_BEGIN = 0,
	SERVER_COMPETITION_REWARD_TYPE_RANK = 1,					// 排名奖励
	SERVER_COMPETITION_REWARD_TYPE_SCORE = 2,					// 评分奖励
	SERVER_COMPETITION_REWARD_TYPE_COLOSSEUM_RANK = 3,			// 竞技场排名
	SERVER_COMPETITION_REWARD_TYPE_COURAGE_CHALLENGE = 4,		// 锢魔之塔星数奖励

	SERVER_COMPETITION_REWARD_TYPE_MAX
};

struct RASeverCompetitionRewardCfg
{
	RASeverCompetitionRewardCfg() : rank_type(0), seq(0), start_time(0), continuou_times(0), reward_type(0), param1(0), param2(0), is_broadcast(false), param3(0)
	{}

	int rank_type;
	int seq;
	int start_time;
	int continuou_times;
	int reward_type;
	int param1;
	int param2;
	std::vector<ItemConfigData> reward_list;
	bool is_broadcast;
	int param3;
};

struct RASeverCompetitionTimeCfg
{
	RASeverCompetitionTimeCfg() : start_time(0), continuou_times(0)
	{}

	int start_time;			//开始天数
	int continuou_times;	//持续天数
};

union RAServerCompetitionKey
{
	int param[2];
	long long unique_key;
};

class RandActivityManager;
class RandActivityServerCompetitionConfig : public RandActivityConfig
{
public:
	RandActivityServerCompetitionConfig();
	virtual ~RandActivityServerCompetitionConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAServerCompetitionOtherCfg & GetOtherCfg() const { return m_other_cfg; }
	const RASeverCompetitionRewardCfg * GetRewardCfg(RandActivityManager* ramgr, int rank_type, int seq)const;
	void GetRewardMapCfgByRankType(RandActivityManager* ramgr, int rank_type, std::vector<RASeverCompetitionRewardCfg> & rank_reward_list)const;
	const std::map<int, RASeverCompetitionTimeCfg> * GetCompetitionMap(RandActivityManager* ramgr)const;

	int GetVersion(unsigned int timestamp)const;
	bool IsHasVersion(int version) const;

private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitRewardCfg(TiXmlElement * RootElement);
	int InitVersionTimestampCfg(TiXmlElement * RootElement);

	long long GetSectionKey(int section_start, int section_end);
	long long GetTypeKey(int rank_type, int seq)const;
	void GetInfoByKey(long long _info_key, int * info_param1, int * info_param2)const;
	
	RAServerCompetitionOtherCfg m_other_cfg;
	//活动奖励 list - version key1:GetSectionKey key2:GetTypeKey
	std::map<long long , std::map<long long, RASeverCompetitionRewardCfg> > m_competition_reward_list[MAX_RA_SERVER_COMPETITION_CFG_NUM];
	//list - versionkey1:GetSectionKey key2:rank_type second:start_time
	std::map<long long, std::map<int, RASeverCompetitionTimeCfg> > m_competition_map[MAX_RA_SERVER_COMPETITION_CFG_NUM];			

	std::map<unsigned int, int> m_competition_version_map;		//版号配置 key:ver_timestamp second:version
};


#endif	//__RAND_ACTIVITY_SERVER_COMPETITION_CONFIG_HPP__