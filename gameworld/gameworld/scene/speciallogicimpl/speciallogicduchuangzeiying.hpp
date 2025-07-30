#ifndef __SPECIAL_LOGIC_DU_CHUANG_ZEI_YING_HPP__
#define __SPECIAL_LOGIC_DU_CHUANG_ZEI_YING_HPP__

#include "scene/speciallogic.hpp"
#include "protocol/msgduchuangzeiying.hpp"

using namespace Protocol;
class SpecialLogicDuChuangZeiYing : public SpecialLogic
{
public:
	SpecialLogicDuChuangZeiYing(Scene * scene);
	virtual ~SpecialLogicDuChuangZeiYing() {}

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role *role);	//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< 主角离开场景

	virtual bool CanDestroy() { return true;};
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);
	virtual void OnTransport(Role *role, SpecialTransport * transport_obj);
	virtual bool CanEncounterMineMonster(Role * role) { return false; }

	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create) { return false; }
	virtual bool CanInvitationRole(Role *role, int uid) { return false; }
	virtual void OnNPCChosedReward(Role *role, ObjID npc_objid, int option_index);

	void SetInfo(int layer, const Posi & transport_pos);
	void SendSceneInfo(Role * role = NULL);
	void SendRoleNotice(Role * role, int npc_id);
	bool IsLastLayer();

private:
	void CreateNpcOrMonster();	
	void CreateTransport();
	void CreateBossNpc(Role * role);
	void TransportNextLayer(Role *role, SpecialTransport * transport_obj);
	
	bool m_is_init;
	bool m_boss_is_create;
	int m_layer;									//当前层数						

	Posi m_transport_pos;							//!<传送阵坐标
	std::vector<SpecialLogicDuChuangZeiYingObjInfo> m_obj_info_list;	// 记录第一层的巡逻BOSS
};



#endif 