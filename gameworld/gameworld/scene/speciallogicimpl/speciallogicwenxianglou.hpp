#ifndef __SPECIAL_LOGIC_WENXIANGLOU_FB_HPP__
#define __SPECIAL_LOGIC_WENXIANGLOU_FB_HPP__

#include "scene/speciallogic.hpp"
#include "protocol/msgscene.h"
#include "servercommon/taskdef.h"
#include <vector>
#include <map>

enum WXL_LOAD_TYPE
{
	WXL_LOAD_TYPE_0 = 0,//��ʼ
	WXL_LOAD_TYPE_1 = 1,//��֤
	WXL_LOAD_TYPE_2 = 2,//ָ��
	WXL_LOAD_TYPE_3 = 3,//��̽
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

	//npc,�������߲���
	void TalkToNpc(Role * role, int npc_seq);//����
	void findItem(Role * role, int item_seq);//�ҵ���
	void IdentificatinNpc(Role * role, int npc_seq);//ָ��NPC
	void Finalfight(Role * role);//����ս��
	void CheckObjEnterPosi();//npc�����ض�λ��

	void SendNPCInfo(Role * role = NULL);//NPC��ϸ
	void SendSceneInfo(Role * role = NULL);//������Ϣ
private:
	void OnFbFinish();
	unsigned int check_enter_posi_tamp;
	bool role_enter_posi;
	bool role_enter_posi2;

	char wenxianglou_has_enter;						//�Ƿ��Ѿ����������
	char load_status;								// ���н׶�
	char npc_sucspicion_list[MAX_NPC_NUM_WXL];		// npc����ֵ
	char npc_talk_list[MAX_NPC_NUM_WXL];				// npc�Ի���¼
	char npc_identity_list[MAX_NPC_NUM_WXL];			// npcָ�ϼ�¼
	char npc_check_list[MAX_NPC_NUM_WXL];			// npc��֤��¼����פ��

	bool has_find_identity;//�Ƿ�ָ����ȷ
	bool has_finish_fb;		//�Ƿ���ɸ���

	//��ʱ���߱���                                                                                                                                                                       
	int reward_count;
	int reward_list[MAX_SPECIAL_ITEM_NUM];//���⸱�����߰� -- ��������20��
};

#endif