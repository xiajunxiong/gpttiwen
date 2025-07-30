#ifndef __SPECIAL_LOGIC_SHAN_HAI_BOSS_HPP__
#define __SPECIAL_LOGIC_SHAN_HAI_BOSS_HPP__

#include "scene/speciallogic.hpp"
#include "servercommon/servercommon.h"
#include "global/shanhaibossmanager/shanhaibossconfig.hpp"
#include "gameworld/item/knapsack.h"
#include "servercommon/shanhaibossdef.hpp"

class SpecialLogicShanHaiBoss :public SpecialLogic
{
public:
	SpecialLogicShanHaiBoss(Scene* scene);
	virtual ~SpecialLogicShanHaiBoss();

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role *role);	//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< 主角离开场景

	virtual bool CanDestroy() { return true; }
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	virtual void OnRoleLeaveTeam(Role * role);
	virtual bool IsAutoRecover() { return false; }

	void SetInfo(int npc_id, int max_level);

//	void GetPoker(Role *role, int index);
private:
	void CreateMonster();
	void SendPokerRewardInfo();

	bool m_is_init;

	int m_boss_id;
	int m_team_max_level;
	int m_boss_seq_id;

	int m_draw_group_id;
	unsigned int m_draw_group_timeout;

	//ShanHaiBossPoker poker_list[SIDE_MAX_ROLE_NUM];
};


#endif