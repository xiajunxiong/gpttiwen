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
	int exp_crystal_day_count;		// ��ˮ��ÿ�����ʹ���
	int exp_crystal_max_count;		// ��ˮ����󱣴����
	int expend_num;				// ÿ�����Ĵ���
	int is_open;					// Ĭ��״̬
	int bonus_exp_num_percentage;	// ˮ������ӳ�
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