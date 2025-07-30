#ifndef __SPECIAL_LOGIC_CHIEF_ELECTION_SEA_HPP__
#define __SPECIAL_LOGIC_CHIEF_ELECTION_SEA_HPP__

#include "scene/speciallogic.hpp"
#include <map>
#include <set>

struct ChiefElectionSeaRoleInfo
{
	bool operator<(const ChiefElectionSeaRoleInfo & other) const;

	ChiefElectionSeaRoleInfo(int role_id);
	ChiefElectionSeaRoleInfo();

	int role_id;
	int socre;
	int win_times;
	int lose_times;
	int total_times;
	time_t time;
	bool re_link;
	bool is_lose;
}; 


enum CHIEF_ELECTION_SEA_RESULT_TYPE
{
	CHIEF_ELECTION_SEA_RESULT_TYPE_WIN = 0,		// ʤ��
	CHIEF_ELECTION_SEA_RESULT_TYPE_LOSE = 1,		// ʧ��
	CHIEF_ELECTION_SEA_RESULT_TYPE_DRAW = 2,		// ƽ��
};

class SpecialLogicChiefElectionSea : public SpecialLogic
{
public:
	SpecialLogicChiefElectionSea(Scene* scene);
	virtual ~SpecialLogicChiefElectionSea();
	void Reset();

	virtual void OnRoleEnterScene(Role *role);					//!< ���ǽ��볡��
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< �����뿪����

	virtual void OnRemoveObj(Obj * obj);							//!< obj��ʧʱ

	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create);
	virtual bool CanInvitationRole(Role * role, int uid);
	virtual bool CanChallengeOthers(Role * role, Role * target);

	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight * ack);
	virtual int GetMineMonsterBattleMode();

	virtual bool CanJoinOtherActivity() { return false; }			//!< �Ƿ�������������
	virtual bool CanDestroy();
	virtual void Update(unsigned long interval, time_t now_second);
	virtual bool CanChangeProf(Role * role);

	void SendSceneInfo();
	void SendRoleInfo(Role * role);
	void SendRankInfo(Role * role = NULL);

	void SetSceneInfo(int which_prof_match);
	void OnActivityClose();
	void OnReady();
	void OnStart();
	void CheckFinish();
	void OnFinish(bool is_from_activity = false);

	void OnAdavanceFetch(Role * role, short op_type, int param1);
	void SendAdvanceFetchInfo(Role * role);
private:
	void SetResult(int role_id, int result_type);
	void SortRank(bool is_focre = false);

	int m_exist_role_num;

	int m_which_prof_match;
	bool m_start_fight;
	bool m_is_finish;

	bool m_need_update_rank;
	time_t m_next_update_rank_time;
	std::vector<ChiefElectionSeaRoleInfo> m_rank_info;
	std::map<int, ChiefElectionSeaRoleInfo> m_role_info;
	std::map<int, bool> m_participate_map;					// �볡ʱ�����,��ս��ʼʱ��¼һ�³������ (���Ͳ��뽱��) key:role_id second:���뽱���Ƿ�����ȡ

	std::map<int, time_t> m_defeat_protect_time;
};

#endif