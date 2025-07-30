#ifndef __RAND_ACTIVITY_BONUS_CONFIG_HPP__
#define __RAND_ACTIVITY_BONUS_CONFIG_HPP__

#include "servercommon/activitydef.hpp"
#include "randactivityconfig.hpp"

struct RABonusOtherCfg
{
	RABonusOtherCfg() : version_id(0)
	{}

	int version_id;		// 版本ID
};

struct RABonusCfg
{
	RABonusCfg() : section_start(0), section_end(0), seq(0), type(0), activity_id(0), type_item(0)
	{}
	
	int section_start;
	int section_end;
	int seq;
	int type;				//活动的类型区分
	int activity_id;
	int type_item;			//是否奖励 0:否
	ItemConfigData reward;
};

class RandActivityBonusConfig : public ILogicConfig
{
public:
	RandActivityBonusConfig();
	virtual ~RandActivityBonusConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const RABonusOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const RABonusCfg * GetBonusCfgBySeq(int seq);
	int GetBonusCfgNum() { return (int)m_bonus_cfg_vec.size(); }
	const std::vector<RABonusCfg> * GetBounsCfgByActivityId(int activity_id);
	const RABonusCfg * GetBonusCfg(int activity_id, int type);

private:
	int InitBonusCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	RABonusOtherCfg m_other_cfg;
	std::vector<RABonusCfg> m_bonus_cfg_vec;
	std::map<int, std::vector<RABonusCfg> > n_bonus_cfg_map;	//key:activity_id
};



#endif