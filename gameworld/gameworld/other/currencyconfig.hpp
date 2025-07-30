#ifndef __COMMON_CONFIG_HPP__
#define __COMMON_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/servercommon.h"
#include "other/attributelist.hpp"
#include "servercommon/attributedef.hpp"

struct ExpCrystalCfg
{
	ExpCrystalCfg(){}

	int id;
	int exp_crystal_day_count;		// 打卡水晶每日赠送次数
	int exp_crystal_max_count;		// 打卡水晶最大保存次数
	int expend_num;				// 每次消耗次数
	int is_open;					// 默认状态
	int bonus_exp_num_percentage;	// 水晶经验加成
};

class Currencyconfig : public ILogicConfig
{
public:
	Currencyconfig();
	virtual ~Currencyconfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ExpCrystalCfg* GetExpCrystalCfg() { return &m_exp_crystal_cfg; }
	ItemID GetExpCrystalItemID() const { return m_exp_crystal_cfg.id; }
private:
	int InitExpCrystalCfg(TiXmlElement *RootElement); 
	
	
	ExpCrystalCfg m_exp_crystal_cfg;
};

#endif