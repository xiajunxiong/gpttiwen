#ifndef __SPECIAL_LOGIC_ADVANCE_FB_HPP__
#define __SPECIAL_LOGIC_ADVANCE_FB_HPP__

#include "scene/speciallogic.hpp"
#include "gameworld/other/advancefb/advancefbconfig.hpp"
#include <vector>
#include <map>

class SpecialLogicAdvanceFb : public SpecialLogic
{
public:
	SpecialLogicAdvanceFb(Scene * scene);
	virtual ~SpecialLogicAdvanceFb();
	struct SpecialLogicAdvanceFbTask//进阶副本特殊任务
	{
		SpecialLogicAdvanceFbTask():task_id(0),progress(0){}
		int task_id;
		int progress;
	};

	enum SPECIALLOGIC_OBJ_TYPE
	{
		SPECIALLOGIC_OBJ_TYPE_NO = -1,
		SPECIALLOGIC_OBJ_TYPE_BOSS = 0,
		SPECIALLOGIC_OBJ_TYPE_TRANSPROT = 1,
		SPECIALLOGIC_OBJ_TYPE_NPC = 2,
		SPECIALLOGIC_OBJ_TYPE_MAX,
	};

	struct SpecialObjType
	{
		SpecialObjType():obj_type(SPECIALLOGIC_OBJ_TYPE_NO),obj_id(-1),obj_pos(0,0) {}
		int obj_type;
		ObjID obj_id;
		Posi obj_pos;
	};
	virtual  const AttributeList & ReCalcAttrPer(AttributeList &base_add, const AttributeList & cpy_attr_list);

	virtual bool CanDestroy() { return true; }
	virtual void OnRoleEnterScene(Role *role);
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);
	virtual void OnTransport(Role *role, SpecialTransport * transport_obj);
	virtual bool IsForceEncounterMineMonster() { return true; }
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_ADVANCE_FB; }
	virtual bool IsAutoRecover() { return false; }
	void InitAdvanceFbScene(int play_type, int param, int layer, std::map<int, std::vector<ItemConfigData> > * item_vec = NULL);
	virtual bool IsDelayDeleteScene() { return true; }
	virtual bool CanChangeTeamLeader(Role * role, Team * team) { return false; }
	/*void RequestFightCloneBoss(Role * role);*/

	virtual void OnRoleLeaveTeam(Role * role);
	virtual void OnNPCChosedReward(Role *role, ObjID npc_objid, int option_index);	//!< NPC选项操作直接奖励

	void RoleTeamCure(Role * role, int cure_present);
	void AloneCure(Role * role, int cure_present);

	void TeamReward(Role *role, const AdvanceNpcOption  * reward);

	void SendAllObjInfo();
	void SendSpecialTaskInfo();
	void SendRoleDropInfo(Role *role);
private:
	void CreateTransToNextLayer();
	void OnFinishAdvanceFb();
	void ChangeBattleData(RoleBattleData * out_data, int attr_change_rate);
	void OnkillMonster(const BattleData_Statistic *data);
	bool HasMonsterDropByID(const std::vector<int>* monster_drop_list,int monster_id);
	int m_adv;				// 进阶阶级
	int m_play_type;		// 玩法类型
	int m_sp_param;			// 特殊参数
	int m_layer_num;		// 副本层数 - 当前副本是第几层
	int m_boss_num;
	int m_npc_num;
	ObjID m_boss_id;		// BOSS ID
	ObjID m_boss_id2;		// BOSS ID2
	SpecialLogicAdvanceFbTask m_sp_task;
	std::vector<SpecialObjType> m_obj_vec;//场景内所有obj信息（除玩家外）

	//临时道具背包
	std::map<int,std::vector<ItemConfigData> > m_reward_vec;//记录通关后给予的奖励 uid - list
};
/*
const DefeatMonsGroups * group_cfg = TASKPOOL->GetDefeatMonsGroups(p_tic->param1);
if (NULL != group_cfg && group_cfg->drop_monster_list.end() != group_cfg->drop_monster_list.find(p1))
{
	DROPPOOL->DropToRole(task_role, group_cfg->drop_id);
	p_task.prog_num = m_mgr->GetKnapsack()->Count(group_cfg->need_item);
}*/

#endif