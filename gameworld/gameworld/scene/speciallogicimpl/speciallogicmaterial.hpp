#ifndef __SPECIAL_LOGIC_MATERIAL_HPP__
#define __SPECIAL_LOGIC_MATERIAL_HPP__

#include "scene/speciallogic.hpp"
#include "servercommon/servercommon.h"
#include "protocol/msgmaterial.hpp"

class SpecialLogicMaterial : public SpecialLogic
{
public:
	SpecialLogicMaterial(Scene* scene);
	virtual ~SpecialLogicMaterial() {};

	virtual void OnRoleEnterScene(Role *role);
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);

	virtual bool CanDestroy() { return true; }
	virtual void Update(unsigned long interval, time_t now_second);				// ¼ì²â´æ»î½±Àø

	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_MATERIAL; }
	virtual bool IsForceEncounterMineMonster() { return false; }
	virtual bool CanEncounterMineMonster(Role * role) { return false; }
	virtual bool IsAutoRecover() { return true; }
	virtual void OnNPCChosedTalk(Role *role, ObjID npc_objid);

	void SetInfo(int chapter_id, int level_seq);
	int GetChapterId() { return m_chapter_id; }
	int GetLevelSeq() { return m_level_seq; }

	void SendSceneInfo(Role * role = NULL);

private:
	void CreateNpc();
	void CreateMonster();

	bool m_is_init;
	int m_chapter_id;
	int m_level_seq;
	int m_boss_id;
	bool kill_boss_flag;

	std::vector<Protocol::SpecialLogicMaterialObjInfo>  m_obj_list;

};




#endif