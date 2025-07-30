#ifndef __SPECIAL_LOGIC_ONLY_FIGHT_HPP__
#define __SPECIAL_LOGIC_ONLY_FIGHT_HPP__

#include "scene/speciallogic.hpp"
#include "servercommon/servercommon.h"
#include "engineadapter.h"

#include <set>

enum ONLY_FIGHT_RESULT_TYPE		// ��һ�а�(���˰������Ҷ�) - ս�����
{
	OF_TYPE_LOSE = 0,		// ʧ��
	OF_TYPE_VICT = 1,		// ʤ��
	OF_TYPE_DRAW = 2,		// ƽ��

	OF_TYPE_MAX
};

struct OnlyFightPrivateInfo
{
	OnlyFightPrivateInfo(): lose_time(0), total_score(0), victory_times(0), fight_times(0), score_change_timestamp(0) {}
	OnlyFightPrivateInfo(int _scroe, bool is_win, int _guild_id = 0)
	{
		lose_time = is_win ? 0 : 1;
		total_score = _scroe;
		victory_times = is_win ? 1 : 0;
		fight_times = 1;
		score_change_timestamp = (UInt32)EngineAdapter::Instance().Time();
	}

	UInt32 lose_time;				// ���ʧ�ܵĴ���
	UInt32 total_score;				// ����ܻ���
	UInt32 victory_times;			// ���ʤ������
	UInt32 fight_times;				// ���ս������
	UInt32 score_change_timestamp;	// ��һ�λ��ֱ䶯��ʱ���
};

struct OnlyFightRankInfo
{
	OnlyFightRankInfo(): uid(0) {}
	OnlyFightRankInfo(int _uid, const OnlyFightPrivateInfo & _info_con)
	{
		uid = _uid;
		info_con = _info_con;
	}

	int uid;
	OnlyFightPrivateInfo info_con;
};

struct OnlyFightPraInfo
{
	OnlyFightPraInfo(): m_level(0) {}
	OnlyFightPraInfo(int level, const UserID & id): m_level(level), user_id(id) {}

	int m_level;
	UserID user_id;
};

struct OnlyFightUserInfo
{
	OnlyFightUserInfo() : level(0), particip_award_flag(0), survival_award_flag (0) {}
	OnlyFightUserInfo(int _level) : level(_level), particip_award_flag(0), survival_award_flag(0) {}

	short level;
	char particip_award_flag;			// ���뽱��ʶ�� 0:δ��ȡ 1:����ȡ 
	char survival_award_flag;			// ������ʾ(������)
};

class SpecialLogicOnlyFight : public SpecialLogic 
{
public:
	SpecialLogicOnlyFight(Scene* scene);
	virtual ~SpecialLogicOnlyFight() {};

	virtual void OnRoleEnterScene(Role *role);
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);
	virtual bool CanJoinOtherActivity() { return false; }			//!< �Ƿ�������������
	virtual bool CanDestroy() { return false; }
	virtual bool CanLeaveGuild() { return false; }
	virtual void Update(unsigned long interval, time_t now_second);				// ������
	virtual bool CanChallengeOthers(Role * role, Role * target);

	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_ONLY_FIGHT; }
	virtual bool OnOpenTreasureChest(Role *role, TreasureChest * chest_obj);

	// �����߼�
	void OnActivityStandy();
	void OnActivityOpen();
	void OnActivityEnterTime();
	bool SceneRoleNumEmpty();     

	// ��߼�
	void RequestOnlyFightRankInfo(Role * role);
	void OnActivityEnd();			// ���������������������ǿ�ƽ���ս����

	// �����߼�
	void OnRoleLogin(Role * role);	
	
	//��¼�����а����
	void OnOpenRank(Role * role);
	void OnCloseRank(Role * role);

	void OnAdavanceFetch(Role * role, short op_type, int param1);
	void SendAdvanceFetchInfo(Role * role);

private:
	void InitFightingHall();												// ��ʼ������

	void OnOnlyFightOver(int uid, int result_type);	
	void RefreshOnlyFightRank();											// ����һ������
	void SendOnlyFightRank(Role * role = NULL, bool send_my_info = false);	// ����������Ϣ
	void SendInSceneAllRoleInfo();											// ���͵�ǰ���������������Ϣ
	void SendAllParticipNotice();											// ֪ͨ���в���������а���
	void SendOpenRankInfoUser();											// ���а���,�·���ͣ�������а��������
	void OnRefreshChest();													// ˢ�±���
	void ClearAllChest();													// ������
	bool RandAndPutChestAwardInBag(Role * role, int chest_id);				// �����ȡһ�α��佱���������������뱳��
	void OnlyFightIssueActivityAward();										// ���Ż����
	bool OnlyFightIssueSurvivalAward(int need_check_num);					// ���Ŵ���(����)
	void OnlyFightIssueSurvivalAward();										// ������������д���
	void OnlyFightIssueParticipAward();										// ���Ų��뽱��
	void SendOnlyFightSpecialInfo(Role * role = NULL);						// �����Ҷ�������Ϣ
	void ForceStopOnlyFightBattle();										// ǿ���ж���һ�аٻ��ս��

	bool m_need_refresh_rank;										// �Ƿ���Ҫˢ�»������а�
	time_t m_next_refresh_rank_timestamp;							// �´�����ˢ�»������а��ʱ���

	std::set<int> m_survival_award_need_check_num;					// ����ɫ�����
	std::map<int, std::set<ObjID> > m_chest_list;					// �����д��ڵı���� ObjID
	std::map<int, UInt64> m_next_chest_refersh_time;				// �����´�ˢ��ʱ������� - ����ID��ֵ - �´�ˢ�µ�ʱ���

	std::map<int, OnlyFightPrivateInfo> m_private_info_map;			// ������Ϣ�б��� - ���UID��ֵ - ��Ϣ����

	std::vector<OnlyFightRankInfo> m_of_rank_list;					// �����б�
	std::map<int, OnlyFightUserInfo> m_participate_player;			// ������������� - ���ʼʱ��¼һ�� key:role_id
	std::set<int> m_player_in_scene;								// �����ڻ�Ծ�����   ��¼uid
	std::set<int> m_scene_open_rank_list;							// �����д����а����� uid

	std::map<int, std::vector<OnlyFightPraInfo> > m_survival_map;	// ������¼ key:���������� 

	time_t next_box_refresh_tamp;									//����ˢ��ʱ���
	int m_box_num;													//δ����������
	time_t force_to_next_time;										//��������������ʱ�ر� (��ԭ���Ĺرջ��ΪT��ҳ�����)

	std::map<int, time_t> m_defeat_protect_time;
	bool is_refresh_box_flag;
	bool m_is_arena_close;											// �����Ƿ��ѹر�

	unsigned int m_delay_close_arena_time;							// ��������ʱ���õ�������ص�
	bool is_send_fight_special_info;
};

#endif