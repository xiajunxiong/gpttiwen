#ifndef __SPECIAL_LOGIC_WAN_LING_FB_HPP__
#define __SPECIAL_LOGIC_WAN_LING_FB_HPP__

#include "scene/speciallogic.hpp"
#include <set>
class SpecialLogicWanLingFB : public SpecialLogic
{
public:
	SpecialLogicWanLingFB(Scene* scene);
	virtual ~SpecialLogicWanLingFB() {};

	virtual void OnRoleEnterScene(Role *role);

	virtual bool CanDestroy() { return true; }
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack);
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_WAN_LING_FB; }

	virtual void OnTransport(Role *role, SpecialTransport * transport_obj);
	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create);
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader) { return true; }


	void SetFBInfo(int fb_index);
	void SendSceneInfo(Role * role);

	//任务完成 返回: 最后一个任务完成后返回true
	bool OnTaskFinish(bool from_msg);

	int GetFBIndex() const { return m_fb_index; };
	int GetTaskSeq() const { return m_task_seq; };

	//战斗开始
	void FightStart(Role *role, int monster_group_id);

private:
	void CreateNPC();

	void CreatLeaveTransport();

	int m_fb_index;		//副本index
	int m_is_finish;	//副本完成标记
	
	int m_task_seq;		//当前任务

	//允许进入队伍
	int m_can_enter_team;

	std::set<int> m_join_set;
};

#endif