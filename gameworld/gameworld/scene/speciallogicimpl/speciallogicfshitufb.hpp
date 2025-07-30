#ifndef __SPECIAL_LOGIC_SHI_TU_FB_HPP__
#define __SPECIAL_LOGIC_SHI_TU_FB_HPP__

#include "scene/speciallogic.hpp"
#include <map>
#include <vector>
#include <set>

struct ShiTuFbMonsterInfo
{
	ShiTuFbMonsterInfo()
	{
		kill_num = 0;
	}

	int kill_num;
	std::vector<ShiTuFbMonsterInfoItem> monster_info;
};

class SpecialLogicShiTuFb : public SpecialLogic
{
public:
	SpecialLogicShiTuFb(Scene* scene);
	virtual ~SpecialLogicShiTuFb();

	virtual void OnRoleEnterScene(Role *role);	//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< 主角离开场景

	virtual bool CanDestroy() { return true; }
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleLeaveTeam(Role * role);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader);

	virtual bool IsAutoRecover() { return false; }

	void SetInfo(int master_uid, int create_role_level, int team_index);
	void SendSceneInfo(Role *role = NULL);
	void ReqFinish(Role * role);
private:
	void CreateMonster();	
	void CheckFinish(time_t now);

	bool m_is_finish;
	int m_team_index;
	int m_create_role_level;
	int m_master_uid;
	time_t m_end_time;
	ShiTuFbMonsterInfo m_group_monster_info;
};

#endif