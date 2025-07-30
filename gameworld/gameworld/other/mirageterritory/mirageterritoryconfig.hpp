#ifndef _MIRAGE_TERRITORY_CONFIG_HPP_
#define _MIRAGE_TERRITORY_CONFIG_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

struct MirageTerritoryOtherCfg
{
	MirageTerritoryOtherCfg() : open_level(0), partner_num(0), dispatch_time(0), dispatch_num(0) {}
	int open_level;
	int partner_num;

	int dispatch_time;
	int dispatch_num;
};

struct MirageTerritoryAreaCfg
{
	MirageTerritoryAreaCfg() : seq(0), area_name(0), job1(0), job2(0) {}
	enum MIRAGE_TERRITORY_AREA_TYPE
	{
		MIRAGE_TERRITORY_AREA_TYPE_0 = 0,//青阳境
		MIRAGE_TERRITORY_AREA_TYPE_1 = 1,//朱明境
		MIRAGE_TERRITORY_AREA_TYPE_2 = 2,//白藏境
		MIRAGE_TERRITORY_AREA_TYPE_3 = 3,//玄英境
		MIRAGE_TERRITORY_AREA_TYPE_MAX,
	};
	int seq;
	int	area_name;
	int	job1;
	int	job2;
};

struct MirageTerritoryChalleCfg
{
	MirageTerritoryChalleCfg() :seq(0), area(0), num(0), reward_id(0), monster_group(0) {}
	int seq;
	int area;
	int num;
	int reward_id;
	int monster_group;
};

struct MirageTerritoryRewardCfg
{
	MirageTerritoryRewardCfg() : seq(0), arm_chip_num(0) {}
	int seq;
	int arm_chip_num;
	std::vector<ItemConfigData> reward_vec;
	std::vector<ItemConfigData> pass_reward_vec;
};

struct PartnerArmChipCfg
{
	PartnerArmChipCfg() :partner_id(0), arms_id(0) {}
	int partner_id;
	int arms_id;
};

class RoleModuleManager;
class MirageTerritoryConfig : public ILogicConfig
{
public:
	MirageTerritoryConfig();
	virtual ~MirageTerritoryConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const MirageTerritoryOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const MirageTerritoryAreaCfg * GetAreaCfg(int seq);
	const MirageTerritoryChalleCfg * GetChallengeCfg(int area, int num);
	const MirageTerritoryRewardCfg * GetRewardCfg(int seq);
	const PartnerArmChipCfg * GetParnterRewardCfg(int partner_id);
private:
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitChallengeCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitRewardCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitAreaCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitPartnerRewardCfg(TiXmlElement * RootElement, std::string& errormsg);

	MirageTerritoryOtherCfg m_other_cfg;
	std::vector<MirageTerritoryChalleCfg> m_challenge_vec;
	std::vector<MirageTerritoryRewardCfg> m_reward_cfg;
	std::vector<MirageTerritoryAreaCfg> m_area_cfg;
	std::vector<PartnerArmChipCfg> m_partner_reward_vec;
};
#endif
