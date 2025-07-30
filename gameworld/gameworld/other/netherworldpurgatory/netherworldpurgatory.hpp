#ifndef _NETHER_WORLD_PURGATORY_HPP_
#define _NETHER_WORLD_PURGATORY_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/netherworldgatorydef.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

class RoleModuleManager;
class NetherWorldPurgatory
{
public:
	NetherWorldPurgatory();
	~NetherWorldPurgatory();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr, const NetherWorldGatoryParam &param);
	void OnRoleUplevel();
	void GetInitParam(NetherWorldGatoryParam * param);

	void OnRoleLogin();
	void OnDayChange();
	void OnBattleFinishWin(BattleData_Business* business_data, int round_num);
	void OnBattleFinishLose(BattleData_Business* business_data);
	void OnNetherWorldPurgatoryOp(int req_type, int param1, int param2, int param3);

	int GetChallengeChapter() { return m_param.pass_challenge_chapters; }
	int GetChallengeId() { return m_param.pass_challenge_id; }

	int GetRewardNum();
	int GetMaxRoundNum(int chapter_id);

	void StarBattle(int chapter_id, int challenge_id);
	void GetChallengeReward(int chapter_id, int challenge_id, int task_id);//¡Ï»°±¶œ‰
	void GetMiLingReward(int chapter_id, int miling_type, int times);
	void SetVideoShow();
	int GetDeBuffLevel() { return m_param.buff_level; }

	void SendAllInfo();
	void SendInfo();
	void SendChapterInfo(int chapter_id);
	void SendSigleChallengeInfo(int chapter_id, int change_id);
	void SendMiLingInfo();
private:

	RoleModuleManager *m_module_mgr;
	NetherWorldGatoryParam m_param;
	bool send_check;
};
#endif