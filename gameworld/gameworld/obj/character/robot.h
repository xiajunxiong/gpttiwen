#ifndef __ROBOT_H__
#define __ROBOT_H__

#include "character.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "gamedef.h"

#include "ai/robotai/robotaibase.hpp"


class Team;

struct RobotParam;
class Robot : public Character
{
public:
	Robot();
	virtual ~Robot();

	void* operator new(size_t c);
	void operator delete(void *m);

	void Init(const RobotParam& param);
	void GetInitParam(RobotParam* out_param);

	virtual	void Update(unsigned long interval, time_t now_second, unsigned int now_dayid);

	inline int GetRobotID() const { return m_robot_id; }
	inline const UserID& GetRobotUserID() const { return m_robot_user_id; }
	inline int GetRobotType() const { return m_robot_type; }
	inline int GetLevel() const { return m_level; }
	inline int GetProfession() const { return m_profession; }
	inline int GetAvatarType() const { return m_avatar_type; }
	inline const RoleBattleData& GetRoleBattleData() const { return m_battle_data; }
	inline const Appearance& GetAppearance() const { return m_battle_data.appearance; }

	inline int GetBehaviorState() const { return m_robot_ai->GetRoleStatus(); };
	inline int GetVipLevel() const { return 0; }
	void SetTeam(Team* team);
	inline void SetRobotInvalid() { m_robot_id = 0; m_robot_user_id = INVALID_USER_ID; }
	inline bool IsRobotInvalid() { return 0 == m_robot_id; }
	inline Team* GetTeam() const { return m_team; }
	inline int GetTeamIndex() const { return m_team_index; }

	inline void AddPosData(const PosData& pos_data) { m_pos_data = pos_data; }
	inline const PosData& GetPosData() const { return m_pos_data; }

	bool ResetPos(Axis x, Axis y);
	void UpdateHpMp(int remain_hp, int remain_mp);
	void UpdatePetHpMp(int pet_id, int pet_origin_idx, int remain_hp, int remain_mp);
	void SetDelayDelete();
	void SetSpecialAppearance(short special_appearance_type, int special_appearance_param);
	void GetSpecialAppearance(short* special_appearance_type, int* special_appearance_param);

	inline int GetWearTitle() {	return m_battle_data.wear_title_id;	};

	void OnRobotStartBattle();
	void OnRobotStopBattle();

	inline void SetBoundTeamType(int team_type) { m_bound_team_type = team_type; }
	inline int GetBoundTeamType() const { return m_bound_team_type; }

	inline void SetBoundBattleMode(int battle_mode) { m_bound_battle_mode = battle_mode; }
	inline int GetBoundBattleMode() const { return m_bound_battle_mode; }

	bool ChangeHp(int ch_value, bool is_percent = false);
	bool ChangeMp(int ch_value, bool is_percent = false);

	bool ChangePetHp(int ch_value, bool is_percent = false);
	bool ChangePetMp(int ch_value, bool is_percent = false);

	void SetRobotLevel(int level);
	void SetRealRoleUID(int uid) { m_real_role_uid = uid; }
	inline int GetRealRoleUID() const { return m_real_role_uid; }
	void OnLeaveTeam();
protected:
	void SendHpMpInfoToTeam();
	
	int m_robot_id;
	int m_robot_type;
	UserID m_robot_user_id;
	int m_level;
	int m_profession;
	int m_avatar_type;
	RoleBattleData m_battle_data;

	Team* m_team;
	int m_team_index;	// 用于内网验证

	PosData m_pos_data;

	unsigned int m_delay_delete_time;

	short m_special_appearance_type;
	int m_special_appearance_param;

	int m_bound_team_type;
	int m_bound_battle_mode;

	int m_robot_ai_type;
	RobotAIBase *m_robot_ai;

	int m_real_role_uid;
};

#endif