#ifndef __QINGYUAN_DUIDUIPENG_CONFIG_HPP__
#define __QINGYUAN_DUIDUIPENG_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include <vector>
#include <map>

struct QingYuanDuiDuiPengNpcCfg
{
	QingYuanDuiDuiPengNpcCfg() : seq(0) {}

	int seq;
	Posi pos;
};

struct QingYuanDuiDuiPengOtherCfg
{
	QingYuanDuiDuiPengOtherCfg() : start_time(0), end_time(0), scene_id(0), during_time(0), reward_times(0){}

	int start_time;				//开启时间
	int end_time;				//结束时间
	int scene_id;				//活动场景
	int during_time;			//持续时间
	int reward_times;			//每日奖励次数

	std::vector<ItemConfigData> reward_1;
	std::vector<ItemConfigData> reward_2;
	std::vector<ItemConfigData> reward_3;
};

static const int MAX_QINGYUAN_DUIDUIPENG_RANK_NUM = 100;

struct QingYuanDuiDuiPengRankCfg
{
	QingYuanDuiDuiPengRankCfg() : title_id(0) {}

	std::vector<ItemConfigData> reward;
	int title_id;
};

class QingYuanDuiDuiPengConfig : public ILogicConfig
{
public:
	QingYuanDuiDuiPengConfig();
	virtual ~QingYuanDuiDuiPengConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	std::map<int, QingYuanDuiDuiPengNpcCfg> & GetNpcMap() { return m_npc_cfg; }
	const QingYuanDuiDuiPengOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const QingYuanDuiDuiPengRankCfg * GetRankCfg(int rank_index);
	const int GetRewardTimeCfg(int match_couple);

private:
	int InitNpcCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitOtherCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitRankCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitRewardLevelCfg(TiXmlElement *RootElement, std::string& errormsg);

	std::map<int, QingYuanDuiDuiPengNpcCfg> m_npc_cfg;
	QingYuanDuiDuiPengOtherCfg m_other_cfg;
	std::map<int, QingYuanDuiDuiPengRankCfg> m_rank_cfg;
	std::map<int, int> m_reward_cfg;
};

#endif