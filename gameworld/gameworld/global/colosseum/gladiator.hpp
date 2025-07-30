#ifndef __GLADIATOR_HPP__
#define __GLADIATOR_HPP__

#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/colosseumdef.hpp"

class Role;
class Gladiator
{
public:
	Gladiator();
	~Gladiator();

	void * operator new(size_t c);
	void operator delete(void *m);

	Gladiator* Clone();

	UserID GetUserID() const { return IntToUid(m_uid); }
	int GetUid() const { return m_uid; }

	void UserLogin(Role * role);

	void SyncRoleData(Role* role);
	void SyncRoleBattleData(const RoleBattleData& role_battle_data);
	int GetTargetRank(int index) const;
 
	void SysReport(Gladiator* gladiator, bool is_win, bool is_gladiator_attacker);
	void SendEveryDayRankRewardInfo();
	void OnResetColosseunmModuleData(bool is_in_pool);
	void SendEveryDayRankReward();
	void OnSeasonChange();		// 赛季改变

	bool IsInPool() const { return m_is_in_pool; }
	void SetIsInPool(bool is_in_pool) { m_is_in_pool = is_in_pool; }
	void OnAddInPool();
	int GetPos() const { return m_pos; }
	void SetPos(int pos, bool is_from_battle = false);
	const char* GetName() const { return m_name; }

	bool IsRobot() { return COLOSSEUM_ROBOT_SERVER_ID == this->GetUserID().db_index; }
	static bool IsRobot(int uid) { return COLOSSEUM_ROBOT_SERVER_ID == IntToUid(uid).db_index; }

	const Appearance& GetAppearance() const { return m_appearance; }
	void SetAppearance(const Appearance& appearance) { m_appearance = appearance; }

	static void AssignShowLevel(Gladiator* gladiator, int rank_pos);
	void AdjustRobotAttr();

	static int CalcRoleCapability(const RoleBattleData& role_battle_data);
	static int CalcPartnerCapability(const BattleCharacterData& partner_data);
	static int CalcPetCapability(const BattleCharacterData& pet_data);
	static int CalcCapability(const RoleBattleData& role_battle_data);

	//清空竞技场累积
	void SetClearCheck();
	void ClearCheck();

	void SetColosseumParam(int colosseum_type, int param);
	int GetColosseumParam(int colosseum_type);
	void SendColosseumInfo(Role *role);
	void OnHistoryHighestRankChanged(int old_history_pos, int new_history_pos);
	void SendMsgToUser(void* data, unsigned int length);
	bool CheckHighestRankReward();

	inline const RoleBattleData& GetRoleBattleData() const { return m_battle_data; }

	void GmClearFightCount() { m_today_fight_count = 0; }

	//竞技场
	int colosseum_clear_check;
	ColosseumParam colosseum_param;
	int m_uid;
	int m_level;
	int m_show_level;
	int m_profession;
	int m_avatar_type;
	GameName m_name;
	int m_today_fight_count;
	int m_fetch_exp_flag;
	int m_worship_count;
	
	RoleBattleData m_battle_data;
	int m_target_list[CHALLENGE_FIELD_OPPONENT_MAX];
	std::list<Report> m_report;
	time_t last_refresh_target_time;
	int m_highest_rank;
	long long m_rank_award_fetch_flag;
	Appearance m_appearance;

	bool m_is_in_pool;
	int m_pos;

	int m_total_capability;
};

#endif // !__ARENA_USER_HPP__