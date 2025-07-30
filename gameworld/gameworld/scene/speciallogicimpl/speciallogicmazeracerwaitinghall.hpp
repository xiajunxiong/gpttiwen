#ifndef __SPECIAL_LOGIC_MAZE_RACER_WAITING_HALL_HPP__
#define __SPECIAL_LOGIC_MAZE_RACER_WAITING_HALL_HPP__

#include "scene/speciallogic.hpp"
#include "globalconfig/globalconfig.h"

#include <map>
#include <set>

struct MazeRaceDeleteData
{
	MazeRaceDeleteData() :box_id(0), obj_id(0) {}
	MazeRaceDeleteData(int _box_id, ObjID _obj_id) : box_id(_obj_id), obj_id(_obj_id) {}
	int box_id;
	ObjID obj_id;
};


class SpecialLogicMazeRacerWaitingHall: public SpecialLogic
{
public:
	SpecialLogicMazeRacerWaitingHall(Scene* scene);
	virtual ~SpecialLogicMazeRacerWaitingHall();
	void Reset();

	virtual bool CanDestroy();
	virtual void Update(unsigned long interval, time_t now_second);	// ʱ���������ͣ���ˢ��ʱ���
	virtual void OnRoleEnterScene(Role * role);						// ���һ�γ������������� 50 ���ʹ��ͽ���������ˢ��ʱ���
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);		// �ӳ��뿪�������Ѵ洢������ɾ��
	virtual void OnRoleLeaveTeam(Role * role);						// �뿪���飬������߳�ȥ
	virtual void OnTeamDismiss(Role * leader_role, Team * team);	// �����ɢ���߳���Ա��ɾ���������
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader);
	virtual bool CanInvitationRole(Role * role, int uid);
	virtual bool OnOpenTreasureChest(Role *role, TreasureChest * chest_obj);

	void OnActivityOpen();
	void OnActivityEnd();

	void GmTransportImmidiately();
private:
	void TransAllTeamIntoEctype();
	void SendWaitingHallInfo(Role * role = NULL);
	void OnRefreshChest();											// ˢ�±���
	void ExtinctAllChest();											// ������
	bool RandAndPutChestAwardInBag(Role * role, int chest_id, bool is_leader = false);		// �����ȡһ�α��佱���������������뱳��

	bool m_need_send_info;
	time_t m_next_send_timestamp;


	int m_target_maze_id;
	time_t m_next_begin_timestamp;		// �´δ��͵�ʱ���
	std::set<int> m_team_index_list;		// ������Ϣ�б��� - ����Index

	bool m_has_role_enter;

	std::map<int, std::set<ObjID> > m_chest_list;					// �����д��ڵı���� ObjID
	time_t next_box_refresh_tamp;									//����ˢ��ʱ���
	int m_box_num;													//δ����������
	bool is_refresh_box_flag;										//�Ƿ�ˢ�±���
};

#endif