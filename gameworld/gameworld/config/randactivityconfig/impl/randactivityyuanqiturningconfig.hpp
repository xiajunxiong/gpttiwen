#ifndef __RAND_ACTIVITY_YUAN_QI_TURNING_CONFIG_HPP__
#define __RAND_ACTIVITY_YUAN_QI_TURNING_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/racoolsummerparam.hpp"

struct RAYuanQiTurningInfoBank
{
	RAYuanQiTurningInfoBank() : type(0), section_start(0), section_end(0), seq(0), sign_group(0), reward_group(0) {}

	int type;
	int	section_start;
	int	section_end;
	int	seq;
	int	sign_group;
	int	reward_group;
};

struct RAYuanQiTurningInfoBankList
{
	RAYuanQiTurningInfoBankList() : section_start(0), section_end(0) {}

	int	section_start;
	int	section_end;
	std::vector<RAYuanQiTurningInfoBank> bank_vec;
};

struct RAYuanQiTurningInfoDetail
{
	RAYuanQiTurningInfoDetail() : sign_group_id(0), sign_id(0), rate(0){}

	int sign_group_id;
	int	sign_id;
	int	rate;
};

struct RAYuanQiTurningInfoDetailList
{
	RAYuanQiTurningInfoDetailList() : weight(0) {}

	int weight;
	std::vector<RAYuanQiTurningInfoDetail> info;
};

struct RAYuanQiTurningRewardGroup
{
	RAYuanQiTurningRewardGroup() : type(0), reward_group_id(0), rate(0) {}
	int type;
	int	reward_group_id;
	int rate;
	std::vector<ItemConfigData> reward_vec;
};

struct RAYuanQiTurningRewardGroupList
{
	RAYuanQiTurningRewardGroupList() : weight(0) {}
	int weight;
	std::vector<RAYuanQiTurningRewardGroup> reward_vec;
};

class RandActivityManager;
class RandActivityYuanQiTurningConfig : public RandActivityConfig
{
public:
	RandActivityYuanQiTurningConfig();
	virtual ~RandActivityYuanQiTurningConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAYuanQiTurningInfoBank * RankInfoBank(int open_day) const;
	const RAYuanQiTurningInfoDetail * RankInfoDetail(int bank_group) const;
	const RAYuanQiTurningRewardGroup * GetRewardCfg(int reward_group) const;
private:
	int InitBankCfg(TiXmlElement * RootElement);
	int InitBankDetailCfg(TiXmlElement * RootElement);
	int InitRewardCfg(TiXmlElement * RootElement);

	std::vector<RAYuanQiTurningInfoBankList> m_info_bank;
	std::map<int, RAYuanQiTurningInfoDetailList> m_info_detail;
	std::map<int, RAYuanQiTurningRewardGroupList> m_reward_group;
};

#endif