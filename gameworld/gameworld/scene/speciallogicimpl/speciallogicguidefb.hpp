#ifndef __SPECIAL_LOGIC_GUIDE_FB_HPP__
#define __SPECIAL_LOGIC_GUIDE_FB_HPP__

#include "scene/speciallogic.hpp"

#include <map>

class SpecialLogicGuideFb : public SpecialLogic
{
public:
	SpecialLogicGuideFb(Scene * scene);
	virtual ~SpecialLogicGuideFb();

	virtual bool CanDestroy() { return true; }

	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_GUIDE_FB; }
	virtual void OnRoleEnterScene(Role *role);
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);
	virtual void OnTransport(Role *role, SpecialTransport * transport_obj);
	virtual bool OnOpenTreasureChest(Role *role, TreasureChest * transport_obj);
	virtual bool IsForceEncounterMineMonster() { return true; }
	virtual bool IsAutoRecover() { return false; }

	void SendSceneInfo(Role * role = NULL);
private:
	void OnFinish();
	void CreateMonster();
	void CreateBox();
	void CreateTransport();

	bool m_is_init;
	int m_layer;
	int m_box_num;
	int m_monster_num;

	ObjID m_boss_obj_id;
};

#endif