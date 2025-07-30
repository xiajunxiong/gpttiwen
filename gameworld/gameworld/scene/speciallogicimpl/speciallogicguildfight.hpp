#ifndef __SPECIAL_LOGIC_GUILD_FIGHT_HPP__
#define __SPECIAL_LOGIC_GUILD_FIGHT_HPP__

#include "scene/speciallogic.hpp"
#include "engineadapter.h"

#include <set>

enum GUILD_FIGHT_RESULT_TYPE		// ������Ҷ� - ս�����
{
	GF_TYPE_LOSE = 0,		// ʧ��
	GF_TYPE_VICT = 1,		// ʤ��
	GF_TYPE_DRAW = 2,		// ƽ��

	GF_TYPE_MAX
};

struct GuildFightFamilysInfo		// ������Ϣ����
{
	GuildFightFamilysInfo() : total_score(0), victory_times(0), fight_times(0), score_change_time(0) {}
	GuildFightFamilysInfo(int _score, bool _is_win)
	{
		total_score = _score;
		victory_times = _is_win ? 1 : 0;
		fight_times = 1;
		score_change_time = (UInt32)EngineAdapter::Instance().Time();
	}

	UInt32 total_score;			// �ܻ���
	UInt32 victory_times;		// ��������ҵ�ʤ������
	UInt32 fight_times;			// ��������ҵ�ս������
	UInt32 score_change_time;	// ��һ�λ��ֱ䶯��ʱ���
};

struct GuildFightPrivateInfo		// ��Ҹ�����Ϣ����
{
	GuildFightPrivateInfo() : failures(0) {}
	GuildFightPrivateInfo(bool is_win) : failures(is_win ? 0 : 1) {}

	int failures;	// ս��ûӮ�Ĵ�����
};

struct GuildFightRankInfo
{
	GuildFightRankInfo() : guild_id(0) {}
	GuildFightRankInfo(int _guild_id, const GuildFightFamilysInfo & _info_con)
	{
		guild_id = _guild_id;
		info_con = _info_con;
	}

	int guild_id;
	GuildFightFamilysInfo info_con;
};

struct GuildFightPraInfo
{
	GuildFightPraInfo() : m_level(0) {}
	GuildFightPraInfo(int level, const UserID & id) : m_level(level), user_id(id) {}

	int m_level;
	UserID user_id;
};

struct GuildFightUserInfo
{
	GuildFightUserInfo() : level(0), particip_award_flag(0), survival_award_flag(0) {}
	GuildFightUserInfo(int _level) : level(_level), particip_award_flag(0), survival_award_flag(0) {}

	short level;
	char particip_award_flag;			// ���뽱��ʶ�� 0:δ��ȡ 1:����ȡ 
	char survival_award_flag;			// ������ʾ(������)
};

class SpecialLogicGuildFight: public SpecialLogic		// ������Ҷ�
{
public:
	SpecialLogicGuildFight(Scene* scene);
	virtual ~SpecialLogicGuildFight() {};

	virtual bool CanDestroy() { return false; }
	virtual bool CanLeaveGuild() { return false; }
	virtual void Update(unsigned long interval, time_t now_second);				// ������
	virtual void OnRoleEnterScene(Role *role);
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);
	virtual bool CanChallengeOthers(Role * role, Role * target);
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);
	virtual bool OnOpenTreasureChest(Role *role, TreasureChest * chest_obj);
	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create);						
	virtual bool CanInvitationRole(Role *role, int uid);
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader);
	virtual int CreateTeamType() { return 165; }

	// �����߼�
	void OnActivityStandy();
	void OnActivityOpen();
	void OnActivityEnterTime();
	
	// ��߼�
	void RequestGuildFightRankInfo(Role * role);
	void OnActivityEnd();		// ���������������������ǿ�ƽ���ս����

	// �����߼�
	void OnRoleLogin(Role * role);

	//��¼�����а����
	void OnOpenRank(Role * role);
	void OnCloseRank(Role * role);

	void OnAdavanceFetch(Role * role, short op_type, int param1);
	void SendAdvanceFetchInfo(Role * role);

private:
	void InitFightingHall();		// ��ʼ������

	void OnGuildFightOver(std::set<int> uid_list, int result_type);
	void RefreshGuildFightRank();									// ����һ������
	void SendGuildFightRank(Role * role = NULL, bool send_my_family_info = false);
	void SendInSceneAllRoleInfo();									// ���͵�ǰ���������������Ϣ
	void SendActivityEndNoticeAndInfo();							// ���͸����в����ļ���������������(��ʾ���㵯��)
	void SendOpenRankInfoUser();									// ���а���,�·���ͣ�������а��������
	void OnRefreshChest();											// ˢ�±���
	void ExtinctAllChest();											// ������
	bool RandAndPutChestAwardInBag(Role * role, int chest_id, bool is_leader = false);		// �����ȡһ�α��佱���������������뱳��
	void GuildFightIssueActivityAward();							// ���Ż����
	bool GuildFightIssueSurvivalAward(int need_check_num);			// ���Ŵ���(����)
	void GuildFightIssueSurvivalAward();							// �������д���(���������)
	void GuildFightIssueParticipAward();							// ���Ų��뽱��
	void SendGuildParticipateRoleNum(Role * role = NULL);			// ���ͱ�������������
	void SendGuildFightSpecialInfo(Role * role = NULL);				// �����Ҷ�������Ϣ
	void ForceStopGuildFightBattle();
	void SetGuildHonorBattleNeedInfo();				//���ù���ս��Ҫ����Ϣ

	bool m_need_refresh_rank;				// �Ƿ���Ҫˢ�»������а�
	time_t m_next_refresh_rank_timestamp;	// �´�����ˢ�»������а��ʱ���
	
	std::set<int> m_survival_award_need_check_num;					// ����ɫ�����

	std::map<int, std::set<ObjID> > m_chest_list;					// �����д��ڵı���� ObjID
	std::map<int, UInt64> m_next_chest_refersh_time;				// �����´�ˢ��ʱ������� - ����ID��ֵ - �´�ˢ�µ�ʱ���

	std::map<int, GuildFightFamilysInfo> m_familys_info_map;		// ������Ϣ�б��� - ����ID��ֵ - ��Ϣ����
	std::map<int, GuildFightPrivateInfo> m_private_info_map;		// ������Ϣ�б��� - ���UID��ֵ - ��Ϣ����

	std::vector<GuildFightRankInfo> m_gf_rank_list;					// �����б�
	std::map<int, GuildFightUserInfo> m_participate_player;			// ������������� - ���ʼʱ��¼һ�� key:role_id
	std::map<int, std::set<int> > m_family_palyer_in_scene;			// �����ڵĻ�Ծ������ң��� - ����ID
	std::map<int, int> m_guild_participate_num;						// �������������	�� - ����ID - ���ʼʱ��¼һ��
	std::set<int> m_scene_open_rank_list;							// �����д����а����� uid

	std::map<int, std::vector<GuildFightPraInfo> > m_survival_map;	// ������¼ key:���������� 
	time_t next_box_refresh_tamp;//����ˢ��ʱ���
	int m_box_num;				//δ����������
	time_t force_to_next_time;	//��������������ʱ�ر�

	std::map<int, time_t> m_defeat_protect_time;

	bool is_refresh_box_flag;	//�Ƿ�ˢ�±���
	bool m_is_arena_close;		//�����Ƿ��ѹر�

	unsigned int m_delay_close_arena_time;	// ��������ʱ���õ�������ص�
	bool is_send_fight_special_info;		
};

#endif