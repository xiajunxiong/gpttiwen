#ifndef __SPECIAL_LOGIC_WORLD_TEAM_ARENA_HPP__
#define __SPECIAL_LOGIC_WORLD_TEAM_ARENA_HPP__

#include "scene/speciallogic.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/struct/global/worldteamarenaparam.hpp"
#include "engineadapter.h"

enum WORLD_TEAM_ARENA_RESULT_TYPE		// Ӣ�ۻ��� - ս�����
{
	WTA_TYPE_LOSE = 0,		// ʧ��
	WTA_TYPE_VICT = 1,		// ʤ��
	WTA_TYPE_DRAW = 2,		// ƽ��

	WTA_TYPE_MAX
};

struct WorldTeamArenaSceneUserInfo
{
	WorldTeamArenaSceneUserInfo() : level(0), particip_award_flag(0), reserve_ch(0) {}
	WorldTeamArenaSceneUserInfo(int _level) : level(_level), particip_award_flag(0), reserve_ch(0) {}

	short level;
	char particip_award_flag;			// ���뽱��ʶ�� 0:δ��ȡ 1:����ȡ 
	char reserve_ch;
};

class SpecialLogicWorldTeamArena : public SpecialLogic
{
public:
	SpecialLogicWorldTeamArena(Scene* scene);
	virtual ~SpecialLogicWorldTeamArena() {};

	virtual void OnRoleEnterScene(Role *role);
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);
	virtual void OnRoleLeaveTeam(Role * role);						// ����뿪����ʱ��������߳�����
	virtual bool CanJoinOtherActivity() { return false; }			//!< �Ƿ�������������
	virtual bool CanDestroy() { return false; }
	virtual bool CanLeaveGuild() { return false; }
	virtual void Update(unsigned long interval, time_t now_second);				
	virtual bool CanChallengeOthers(Role * role, Role * target);

	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create);						// �����ڲ��������
	virtual bool CanInvitationRole(Role *role, int uid) { return false; }
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader);

	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_WORLD_TEAM_ARENA; }

	int GetTeamIndexByUnqiueId(long long team_unique_id);

	//��߼�
	void OnActivityStandy();
	void OnActivityEnterTime();		// �볡ʱ��
	void OnActivityOpen();
	void OnActivityEnd();			// ���������������������ǿ�ƽ���ս����
	void RequestFightRankInfo(Role * role);

	void OnRoleLogin(Role * role);	
	void OnInitiateFight(Role * role, int target_uid);	//����ս��

	//��¼�����а����
	void OnOpenRank(Role * role);
	void OnCloseRank(Role * role);

	void OnAdavanceFetch(Role * role, short op_type, int param1);
	void SendAdvanceFetchInfo(Role * role);

private:
	void InitFightingHall();													// ��ʼ������

	void OnGuildFightOver(std::set<int> uid_list, int result_type);
	void RefresTeamArenaRank();													// ����һ������
	void SendWorldTeamArenaRank(Role * role = NULL, bool send_my_info = false);	// ����������Ϣ
	void SendInSceneAllRoleInfo();												// ���͵�ǰ���������������Ϣ
	void SendAllParticipNotice();												// ֪ͨ���в���������а���
	void SendOpenRankInfoUser();												// ���а���,�·���ͣ�������а��������
	void SendWorldTeamArenaSpecialInfo(Role * role = NULL);						// ����Ӣ�ۻ���������Ϣ
	void WorldTeamArenaParticipAward();											// ���Ų��뽱��
	void WorldTeamArenaRankAward();												// ���Ż��������
	void ForceStopWorldTeamArenaBattle();										// ǿ���ж�Ӣ�ۻ�����ս��

	std::map<long long, std::set<int> > m_scene_team_map;						// �����д��ڵ�ս�� key:team_unique_id value:role_id

	std::map<long long, WorldTeamArenaRankItem> m_team_arena_info_map;			// ս�Ӿ�����Ϣ key:team_unqiue_id
	std::vector<WorldTeamArenaRankItem> m_rank_list;							// ս�������б�
	std::map<int, WorldTeamArenaSceneUserInfo> m_participate_user_list;			// ������������� - ���ʼʱ��¼ key:role_id
	std::set<int> m_scene_open_rank_list;										// �����д����а����� uid

	bool m_need_refresh_rank;													// �Ƿ���Ҫˢ�»������а�
	time_t m_next_refresh_rank_timestamp;										// �´�����ˢ�»������а��ʱ���

	bool m_is_arena_close;														// �����Ƿ��ѹر�
	time_t force_to_next_time;													// ��������������ʱ�ر�

	std::map<long long, time_t> m_defeat_protect_time;							// ս�ӱ���ʱ�� key:team_unique_id
	unsigned int m_delay_close_arena_time;										// ��������ʱ���õ�������ص�
	bool is_send_fight_special_info;
};


#endif