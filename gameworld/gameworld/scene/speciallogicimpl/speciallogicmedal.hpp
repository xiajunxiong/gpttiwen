#ifndef __SPECIAL_LOGIC_MEDAL_HPP__
#define __SPECIAL_LOGIC_MEDAL_HPP__

#include "scene/speciallogic.hpp"
#include <map>

class SpecialLogicMedal : public SpecialLogic
{
public:
	SpecialLogicMedal(Scene* scene);
	virtual ~SpecialLogicMedal();

	virtual void OnRoleEnterScene(Role *role);	//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< 主角离开场景

	virtual bool CanDestroy() { return true; }
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);
	virtual void OnRoleLeaveTeam(Role * role);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader) { return true; }
	virtual bool CanInvitationRole(Role * role, int uid);

	virtual void OnTransport(Role *role, SpecialTransport * transport_obj);
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_MEDAL_FB; }
	virtual bool IsForceEncounterMineMonster() { return true; }
	virtual bool IsAutoRecover() { return false; }

	void SetInfo(int fb_seq,int layer);
	void SendSceneInfo(Role *role = NULL);
	void SendKillBoss();

	virtual void OnNPCChosedReward(Role * role, ObjID npc_objid, int option_index);
	void RecoryHelp(Role * role, int cure_present);
private:
	void CreateMonster();	
	void CreateTransport();
	void CreateNpc();
	void ClearMonster();
	bool IsLastLayer(int fb_layer = 0);
	void TransportNextLayer(Role *role, SpecialTransport * transport_obj);	

	int m_fb_seq;
	int m_curr_layer;

	bool m_is_init;
	bool m_is_finish;

	std::set<ObjID> m_monster_obj;			//记录创建出来的明怪，只要有个一个击杀其他也删除。并创建进入下一层的传送门
	ObjID m_recory_npc_objid;
};

#endif