#ifndef __SPECIAL_LOGIC_DREAM_NOTES_CHALLENGE_HPP__
#define __SPECIAL_LOGIC_DREAM_NOTES_CHALLENGE_HPP__

#include "scene/speciallogic.hpp"
#include "servercommon/servercommon.h"
#include "protocol/msgdreamnoteschallenge.hpp"
#include "config/otherconfig/dreamnoteschallengeconfig.hpp"

class SpecialLogicDreamNotesChallengeFb : public SpecialLogic
{
public:
	SpecialLogicDreamNotesChallengeFb(Scene * scene);
	virtual ~SpecialLogicDreamNotesChallengeFb();

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role *role);	//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< 主角离开场景

	virtual bool CanDestroy() { return true; }
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_DREAM_NOTES_CHALLENGE; }
	virtual bool IsForceEncounterMineMonster() { return false; }
	virtual bool CanEncounterMineMonster(Role * role) { return false; }
	virtual bool IsAutoRecover() { return true; }
	virtual void OnNPCChosedReward(Role *role, ObjID npc_objid, int option_index);
	virtual void OnNPCChosedTalk(Role *role, ObjID npc_objid);

	void SetInfo(int level) { m_level = level; }
	void SendSceneInfo(Role * role = NULL);
	void SendNotice(Role * role = NULL);

	void RefreshScene(Role * role, int level);

private:

	void CreateNpc();
	void CreateMonster();
	void ResetScene();

	bool m_is_init;
	int m_level;
	int m_boss_id;
	bool kill_boss_flag;		

	int m_plot_npc_id;		//默认寻找的剧情NPC_id(屏蔽)
	std::vector<Protocol::SpecialLogicDreamNotesChallengeObjInfo> m_obj_list;	

};


#endif