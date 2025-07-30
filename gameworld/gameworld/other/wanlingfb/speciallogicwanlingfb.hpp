#ifndef __SPECIAL_LOGIC_WAN_LING_FB_HPP__
#define __SPECIAL_LOGIC_WAN_LING_FB_HPP__

#include "scene/speciallogic.hpp"
#include <set>
class SpecialLogicWanLingFB : public SpecialLogic
{
public:
	SpecialLogicWanLingFB(Scene* scene);
	virtual ~SpecialLogicWanLingFB() {};

	virtual void OnRoleEnterScene(Role *role);

	virtual bool CanDestroy() { return true; }
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack);
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_WAN_LING_FB; }

	virtual void OnTransport(Role *role, SpecialTransport * transport_obj);
	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create);
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader) { return true; }


	void SetFBInfo(int fb_index);
	void SendSceneInfo(Role * role);

	//������� ����: ���һ��������ɺ󷵻�true
	bool OnTaskFinish(bool from_msg);

	int GetFBIndex() const { return m_fb_index; };
	int GetTaskSeq() const { return m_task_seq; };

	//ս����ʼ
	void FightStart(Role *role, int monster_group_id);

private:
	void CreateNPC();

	void CreatLeaveTransport();

	int m_fb_index;		//����index
	int m_is_finish;	//������ɱ��
	
	int m_task_seq;		//��ǰ����

	//����������
	int m_can_enter_team;

	std::set<int> m_join_set;
};

#endif