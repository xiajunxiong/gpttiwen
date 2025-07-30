#ifndef __DUEL_HPP__
#define __DUEL_HPP__

#include "servercommon/dueldef.h"
#include "other/attributelist.hpp"

class RoleModuleManager;
class Role;

class Duel				//¾ö¶·
{
public:
	Duel();
	~Duel();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const DuelParam & param);
	void GetInitParam(DuelParam * param);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void Update(unsigned long interval, time_t now_second, unsigned int now_dayid);

	void OnRoleLogin();

	bool ChangeKillValue(int value, const char* reason);

	void CheckTitle();				//¼ì²â³ÆºÅ
	void LossDeduce();				//Ê§°Ü¿Û³ý¾­Ñé¡¢Í­±Ò
	void CheckTeam();

	void SendInfo();
	void OnAoiNotice();

	int OnBattleFinish(int duel_type, bool is_attack, bool is_win, int level_diff, int scene_id, UInt64 coin);
	void NoticeResult(int duel_type, bool is_attack, bool is_win, int change_value, bool target_is_team, const char* name, bool target_is_red, UInt64 coin);

	void OnDuelEscort(int escort_task_seq, int escort_npc_id);

	void AddUseRedeceItemTime() { m_param.use_reduce_item_num++; }
	bool CanUseRedeceItem();

	bool IsRedName() { return m_param.kill_value ? true : false; }
	const int GetKillValue() { return m_param.kill_value; }
private:

	RoleModuleManager * m_mgr;
	DuelParam m_param;
};

#endif
