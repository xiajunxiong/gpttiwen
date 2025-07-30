#ifndef __DREAM_NOTES_CHALLENGE_HPP__
#define __DREAM_NOTES_CHALLENGE_HPP__

#include "servercommon/dreamnoteschallengedef.hpp"

class RoleModuleManager;
class DreamNotesChallenge
{
public:
	DreamNotesChallenge();
	~DreamNotesChallenge();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * module_mgr, const DreamNotesChallengeParam & param);
	void GetInitParam(DreamNotesChallengeParam * param);
	
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnRoleLogin();

	bool IsHasTimes();
	bool IsCanChallenge(int level);
	void SendPassReward(int level);
	void OnFastPassFb(int level);
	int GetPassLevel() { return m_param.pass_level; }
	void OnBuyTimes(int buy_times);
	void OnUseItemAddTimes(int add_times);
	void OnReissueReward(int count);
	unsigned int GetPassNewLevelTimestamp() { return m_param.pass_new_level_timestamp; }
	bool IsPassLevel(int level);
	void GmSetPassLevel(int level);

	void OnRoleEnterScene();
	void OnRoleEnterFBUseEnergy(int use_energy_num);
	void OnRoleGetFBBossReward();

	void SendInfo();
private:
	RoleModuleManager * m_role_module_mgr;
	DreamNotesChallengeParam m_param;

};

#endif