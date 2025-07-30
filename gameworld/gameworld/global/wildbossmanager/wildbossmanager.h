#ifndef _WILD_BOSS_MANAGER_HPP_
#define _WILD_BOSS_MANAGER_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/wildbossdef.hpp"
#include <map>

class Role;

class WildBossManager
{
public:
	static WildBossManager & Instance();

	void Init(const WildBossParam &param);
	void GetInitParam(WildBossParam *param);

	void OnRoleLogin();
	void OnRoleLogout(Role *role);
	void Update(unsigned long interval, time_t now_second);

	void GmCreateWildBoss(Role * role);

	void SendAllUser(int index);
	void SendInfoToRole(Role *role);
	void HiddenSendInfoToGameRole(int uid, int param);
	void SendMonsterPosiToRole(Role * role, int monster_seq);
	void HiddenSendMonsterPosiTorGameRole(int uid, int monster_seq);
	void OnKillMonster(int cfg_seq);
private:
	WildBossManager();
	~WildBossManager();
	void ResetPosi(int seq);
	void CheckExpirePoker();

	WildBossParam m_param;
	Posi monster_pos[MAX_WILD_BOSS_NUM];		//‘›¥ÊbossŒª÷√
	int wild_number_list[MAX_WILD_BOSS_NUM];	//‘›¥Êboss±‡∫≈

	//std::map<int, TeamWildBossPokerInfo> m_team_id_to_poker;
	OfflineList offline_role_list[MAX_PLAY_THIS_MODEL];

};

#endif

