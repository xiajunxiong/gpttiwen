#ifndef __SPECIAL_LOGIC_WENXIANGLOU_FB_HPP__
#define __SPECIAL_LOGIC_WENXIANGLOU_FB_HPP__

#include "scene/speciallogic.hpp"
#include "protocol/msgscene.h"
#include "servercommon/taskdef.h"
#include <vector>
#include <map>

enum WXL_LOAD_TYPE
{
	WXL_LOAD_TYPE_0 = 0,//初始
	WXL_LOAD_TYPE_1 = 1,//查证
	WXL_LOAD_TYPE_2 = 2,//指认
	WXL_LOAD_TYPE_3 = 3,//试探
	WXL_LOAD_TYPE_MAX = 4,
};

class SpecialLogicWenXiangLouFb : public SpecialLogic
{
public:
	SpecialLogicWenXiangLouFb(Scene * scene);
	virtual ~SpecialLogicWenXiangLouFb();

	virtual void Update(unsigned long interval, time_t now_second);

	virtual bool CanDestroy() { return true; }
	virtual void OnRoleEnterScene(Role *role);
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);
	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create);

	//virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_ADVANCE_FB; }
	void InitFbScene(Role *role);

	virtual bool IsDelayDeleteScene() { return true; }

	//npc,副本道具操作
	void TalkToNpc(Role * role, int npc_seq);//交流
	void findItem(Role * role, int item_seq);//找道具
	void IdentificatinNpc(Role * role, int npc_seq);//指认NPC
	void Finalfight(Role * role);//进行战斗
	void CheckObjEnterPosi();//npc进入特定位置

	void SendNPCInfo(Role * role = NULL);//NPC详细
	void SendSceneInfo(Role * role = NULL);//场景信息
private:
	void OnFbFinish();
	unsigned int check_enter_posi_tamp;
	bool role_enter_posi;
	bool role_enter_posi2;

	char wenxianglou_has_enter;						//是否已经进入过副本
	char load_status;								// 进行阶段
	char npc_sucspicion_list[MAX_NPC_NUM_WXL];		// npc嫌疑值
	char npc_talk_list[MAX_NPC_NUM_WXL];				// npc对话记录
	char npc_identity_list[MAX_NPC_NUM_WXL];			// npc指认记录
	char npc_check_list[MAX_NPC_NUM_WXL];			// npc查证记录（常驻）

	bool has_find_identity;//是否指认正确
	bool has_finish_fb;		//是否完成副本

	//临时道具背包                                                                                                                                                                       
	int reward_count;
	int reward_list[MAX_SPECIAL_ITEM_NUM];//虚拟副本道具包 -- 不允许超过20个
};

#endif