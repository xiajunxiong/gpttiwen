#ifndef __SPECIAL_LOGIC_BRAVE_GROUND_HPP__
#define __SPECIAL_LOGIC_BRAVE_GROUND_HPP__

#include "scene/speciallogic.hpp"
#include <map>
#include <set>

struct SpecialLogicBraveGroundUseItemInfo
{
	SpecialLogicBraveGroundUseItemInfo()
	{
	}

	std::map<int, int> use_type_to_use_times;		//��ͬʹ��������Ʒ��Ӧʹ�ô���
};

class SpecialLogicBraveGround : public SpecialLogic
{
public:
	SpecialLogicBraveGround(Scene* scene);
	virtual ~SpecialLogicBraveGround();

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role *role);	//!< ���ǽ��볡��
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< �����뿪����

	virtual bool CanDestroy() { return true; }
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);
	virtual void OnRoleLeaveTeam(Role * role);
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader);
	virtual bool CanInvitationRole(Role * role, int uid);
	
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_BRAVE_GROUND; }
	virtual void OnTransport(Role *role, SpecialTransport * transport_obj);

	virtual bool IsForceEncounterMineMonster() { return true; }

	void OnActivityBraveGroundClose();

	void SendLayerInfo(Role * role = NULL);
	void SendLevelRewardInfo(int start_level, int end_level, Role * role);
	void SendNotify(Role * role);

	void UseNpcFunction(Role * role,int obj_id);
	void SelectLevel(Role * role, int level);
	
	void SendRoute(Role * role, int p1, int p2);
	void SetInfo(int team_index,int record_key,Posi transport_pos,int can_select_level,int role_level, bool is_first_enter_flag = false);
	int GetBuffId();

	void GmTransport(Role * role,int add_level);
private:
	void CreateMonster();
	void CreateTransport(bool is_on_kill_boss = false);
	void CreateNpc();

	void SetTargetLevelInfo(int scene_id,int scene_key,int level,Posi pos);
	void TransportHelp(int level);
	void DropRewardKillMonster(battlegameprotocol::BattleGameFightResultAck * ack);			//��ɱ��Ӣ�ָ�����
	Posi GetRandPosiExcludeTransportPos();			//��ȡһ���봫�͵㲻�ص��ĵ�

	void UpdateTeamRecord(int reached_level);

	bool m_is_first_enter_flag;				//�Ƿ����״ν���ؿ� (�����ж���ҵĲ������)
	bool m_is_update_record;
	int m_team_index;
	int m_record_key;
	int m_can_select_level_max;
	int m_role_level;

	bool m_is_init;
	int m_curr_level;
	ObjID m_boss_obj_id;
	Posi m_transport_pos;
	int m_buff_id;

	std::set<ObjID>	m_monster_objid;	//����objid
};

#endif