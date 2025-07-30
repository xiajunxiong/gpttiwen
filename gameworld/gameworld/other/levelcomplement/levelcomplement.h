#ifndef __LEVEL_COMPLEMENT_H__
#define __LEVEL_COMPLEMENT_H__

#include "servercommon/commondata.hpp"
#include <map>

class RoleModuleManager;
class LevelComplement
{
public:
	LevelComplement();
	~LevelComplement();

	void Init(RoleModuleManager * mgr, const CommonDataParam& param);
	void GetInitParam(CommonDataParam* param);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void Update(time_t now_second);
	void OnAddExp(long long add_exp);
	void BuyExp();
	void SendInfo();

	void* operator new(size_t c);
	void operator delete(void* m);
private:
	RoleModuleManager * m_role_module_mgr;

	char level_complement_flag;
	long long accumulated_exp;
	unsigned int m_refresh_accumulated_tamp;
	unsigned int m_end_accumulate_tamp;

	int m_day_difference;
	int m_add_exp_times;

	long long m_alread_get_exp;
	long long m_save_exp;
	long long m_today_accumulated_exp;
};


#endif
