#ifndef __OFFLINE_EXP_CONFIG_HPP__
#define __OFFLINE_EXP_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"

#include <map>

struct OfflineExpOtherCfg 
{
	OfflineExpOtherCfg() : oe_open_level(0), oe_start_calc_time(0), oe_max_calc_dur(0) {}

	int oe_open_level;			// ���ܿ���ʱ��
	int oe_start_calc_time;		// ��ʼ��ʱʱ��
	int oe_max_calc_dur;		// ����ۼ�����ʱ��
};

class OfflineExpConfig : public ILogicConfig
{
public:
	OfflineExpConfig();
	virtual ~OfflineExpConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	unsigned int CalcOfflineExpAddition(int role_level, int offline_dur_m, int *out_m);
	int GetOeMaxCalcDur() { return m_offline_exp_other_cfg.oe_max_calc_dur; }
	const OfflineExpOtherCfg & GetOtherCfg() { return m_offline_exp_other_cfg; }

private:
	int InitExpAccuOtherCfg(TiXmlElement * RootElement);
	int InitExpAccuLevelCfg(TiXmlElement * RootElement);

	OfflineExpOtherCfg m_offline_exp_other_cfg;
	std::map<int, float> m_exp_accumu_level_list;		// ��������б��� �� ��ҵȼ���ֵ �� ÿ���Ӿ����ֵ
};

#endif