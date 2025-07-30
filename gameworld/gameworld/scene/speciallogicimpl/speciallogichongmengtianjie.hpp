#ifndef __SPECIAL_LOGIC_HONGMENG_TIANJIE_HPP__
#define __SPECIAL_LOGIC_HONGMENG_TIANJIE_HPP__

#include "scene/speciallogic.hpp"
#include "obj/character/npc_monster.h"

#include <map>

class SpecialLogicHongMengTianJie : public SpecialLogic
{
public:
	SpecialLogicHongMengTianJie(Scene * scene);
	virtual ~SpecialLogicHongMengTianJie();

	//virtual void Update(unsigned long interval, time_t now_second);

	virtual bool CanDestroy() { return true; }
	
	virtual void OnRoleEnterScene(Role *role);
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);

	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_HONGMENG_TIANJIE; }

	virtual void OnTransport(Role *role, SpecialTransport * transport_obj);
	virtual bool CanEncounterMineMonster(Role * role) { return false; }
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader);
	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create);
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);

	bool InitScene(int fb_id, int difficulty, Posi door_pos, int fb_type, int fb_seq);
	void SetTeamMemberList(int member_num, int* uid_list);
	bool IsTeamMember(int uid) const;

	void OnReq(Role * role, void * msg);

	void SendSceneInfo(Role * role = NULL);
	void SendTalkInfo(int talk_id, int talk_seq);
	void SendPassItemInfo(Role * role);

	int GetDifficulty() { return m_difficulty; }
	int GetFBStage() { return m_stage; }
private:
	bool CreateAMonster();
	void OnKillAMonster(int seq_id);		// ����һ������

	void CreatLeaveTransport();

	int m_fb_id;			// ����ID
	Posi m_door_pos;
	bool m_is_world_pass;	// �Ƿ�ȫ����ͨ��
	unsigned int m_role_leave_times;

	int m_difficulty;		// �����Ѷ�
	short m_stage;			// �����׶�
	int m_monster_seq;		// ��ǰBOSS seqid;

	std::map<int, int> pass_monster_group;			// ȫ��û����ͨʱ ��¼�Լ�����Ĺ������¼���ļ�
	std::map<int, std::map<ItemID, int> > m_user_item_list;

	int m_fb_type;
	int m_fb_seq;

	std::set<int> m_uid_list;
};

#endif