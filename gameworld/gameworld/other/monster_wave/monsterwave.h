#ifndef _MONSTER_WAVE_HPP_
#define _MONSTER_WAVE_HPP_

#include "servercommon/monsterwavedef.hpp"

class Role;
class RoleModuleManager;

class MonsterWave
{
public:
	MonsterWave();
	~MonsterWave();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const MonsterWaveParam & param);													//获得初始化Param
	void GetInitParam(MonsterWaveParam * param);
	void OnResetData(unsigned int old_dayid, unsigned int now_dayid);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	bool IsPlay();
	void AddPlayTimes();
	int GetPlayTimes() { return m_param.play_times; }

	void SendPassReward(int pass_wave);
	void SendFinishInfo(int role_level, int pass_wave);
	void SendInfo();
private:
	RoleModuleManager * m_role_module_mgr;

	MonsterWaveParam m_param;
};


#endif 
