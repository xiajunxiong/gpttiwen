#ifndef __SPECIAL_LOGIC_MAZE_RACER_HPP__
#define __SPECIAL_LOGIC_MAZE_RACER_HPP__

#include "scene/speciallogic.hpp"
#include "protocol/msgactivity.hpp"
#include "obj/character/npc.h"

#include <map>
#include <set>

#include <vector>

static const int MAZE_RACER_TRANSFER_NPC_NUM = 3;					// ����NPC ������
static const int MAZE_RACER_TRANSFER_TARGET_POSI_NUM = 6;			// �������Ŀ����������

class SpecialLogicMazeRacer: public SpecialLogic
{
public:
	SpecialLogicMazeRacer(Scene* scene);
	virtual ~SpecialLogicMazeRacer();

	virtual bool CanDestroy();
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnRoleEnterScene(Role *role);											// ���볡��ʱ��������Ƕӳ���������ID�������
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);							// �뿪����ʱ�������ֱ�ǻ�ԭ
	virtual void OnRoleLeaveTeam(Role * role);											// ����뿪����ʱ��������߳�����
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);		// ս���������
	virtual void OnTeamDismiss(Role * leader_role, Team * team);						// �����ɢʱ���Ӷ����б���ɾ������
	virtual void OnTransport(Role *role, SpecialTransport * transport_obj);				// ���ϴ�����
	virtual void OnAOIEnterRole(Role * role, Role * target_role);						// ������Ұ���·�������Ϣ
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_MAZE_RACER; }
	virtual bool IsAutoRecover() { return false; }
	virtual bool IsForceEncounterMineMonster() { return true; }
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader);

	// ����Ч������
	virtual bool GetBattleAnimationSpeedType(Role* role, ARG_OUT int* speed, ARG_OUT bool* is_force_speed);	// 0: ���ٷ� / 1: ���ٷ�������һ��������ս�������趨
	virtual bool CanMoveOn(Role* role);									// 2: �����������һ��ʱ�䣬�ƶ�����趨
	virtual bool CanEncounterMineMonster(Role * role);					// 3: ��ս��������һ��ʱ�䣬��������趨
	virtual long long GetMineMonsterEncounterInterval(Role * role);		// 4: ��ս��������һ�����������м���趨

	// �����߼�����
	void InitMazeRacerScene(int copy_id, int maze_id, int layer, time_t  start_time);
	void RequestUseAmulet(Role * role, int amulet_id);	// ʹ�÷���
	void RequestTransport(Role * role);					// ������ - �ڵ�ǰ�����ڴ���
	void GetTeamIndexList(std::vector<int> & out_list);
	void SendEctypeInfoToAllRole(Protocol::SCActivityMazeRacerCopysInfo * msg, int send_len);
	void OnMRActivityFinish();		// 
	void SendAmuletInfo(Role * role, Role * target_role = NULL);
	void SendSceneInfo(Role * role);

	bool IsPassTeam(int uid_team);
	void OnTeamChangeLeader(Role * leader, int last_leader_id, Team * team);
	// GM ����
	void GmMazeRacerOrder(Role * role, int op_type, int param1, int param2);

private:
	int m_is_finish;			//������ǰ����Ϊ���

	int m_copy_id;				// �������
	int m_maze_id;				// �Թ�ID
	int m_layer;				// ����

	time_t m_next_refresh_rank_timestamp;		// �´�ˢ��������ʱ���
	bool m_need_refresh_rank;	// �Ƿ���Ҫˢ������
	Posi trans_posi_list[MAZE_RACER_TRANSFER_TARGET_POSI_NUM];		// �������Ŀ������λ�ã�0 ������һ���������

	std::vector<int> m_team_id_vec;				// ����ID�б�
	std::set<int> m_pass_team_index;			// ͨ�ض���index

	std::map<int, time_t> m_team_enter_time;		//��¼ÿ���������ʱ�䣬���ͻ�����
	time_t m_start_time;
};

#endif