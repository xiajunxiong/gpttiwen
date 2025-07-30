#ifndef __SPECIAL_LOGIC_FB_GU_JIE_HPP__
#define __SPECIAL_LOGIC_FB_GU_JIE_HPP__

#include "scene/speciallogic.hpp"

class SpecialLogicFBDuJie : public SpecialLogic
{
public:
	SpecialLogicFBDuJie(Scene* scene);
	virtual ~SpecialLogicFBDuJie() {};

	virtual void OnRoleEnterScene(Role *role);

	virtual bool CanDestroy() { return true; }
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack);
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_DU_JIE; }

	void SetFBInfo(int fb_level);
	void SendSceneInfo(Role * role);
private:
	void CreateMonster();

	int m_fb_level;		//副本level
	int m_is_finish;	//副本完成标记

	//boss
	int m_boss_npc_id;
	ObjID m_boss_obj_id;
};

#endif