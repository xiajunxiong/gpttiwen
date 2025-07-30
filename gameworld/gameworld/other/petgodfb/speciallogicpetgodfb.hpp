#ifndef __SPECIAL_LOGIC_PET_GOD_FB_HPP__
#define __SPECIAL_LOGIC_PET_GOD_FB_HPP__

#include "scene/speciallogic.hpp"

class SpecialLogicPetGodFB : public SpecialLogic
{
public:
	SpecialLogicPetGodFB(Scene* scene);
	virtual ~SpecialLogicPetGodFB() {};

	virtual void OnRoleEnterScene(Role *role);

	virtual bool CanDestroy() { return true; }
//	virtual bool CanJoinOtherActivity() { return false; }			//!< 是否允许加入其它活动

	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_PET_GOD_FB; }
	virtual bool IsAutoRecover() { return false; }

	virtual void OnNPCChosedReward(Role *role, ObjID npc_objid, int option_index);	//!< NPC选项操作直接奖励

	void SetFBInfo(int role_uid, int fb_seq, int role_level);

	void SendSceneInfo(Role * role);


private:
	void CreateMonster();
	void CreateNpc();

	int m_is_set_init;

	int m_fb_seq;		//副本seq
	int m_role_level;	//角色等级

	int m_is_finish;	//副本完成标记

	//boss
	int m_boss_npc_id;
	ObjID m_boss_obj_id;

	int m_role_uid;

};

#endif