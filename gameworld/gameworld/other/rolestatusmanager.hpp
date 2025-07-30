#ifndef __ROLE_STATUS_MANAGER__
#define __ROLE_STATUS_MANAGER__

class Role;
class RoleStatusManager
{
public:
	enum ROLE_STATUS
	{
		ROLE_STATUS_INVALID = 0,

		ROLE_STATUS_IDLE,		// 静止
		ROLE_STATUS_MOVE,		// 移动
		ROLE_STATUS_FIGHT,		// 战斗
		ROLE_STATUS_GATHER,		// 采集
		ROLE_STATUS_FISHING,	// 钓鱼

		ROLE_STATUS_MAX
	};

	enum ROLE_MOVE_STATUS
	{
		ROLE_MOVE_STATUS_NORMAL = 0,	//正常
		ROLE_MOVE_STATUS_SNEAK = 1,		//潜行
		ROLE_MOVE_STATUS_SHIFA = 2,		//施法
		ROLE_MOVE_STATUS_YINDAO = 3,	//引导
		ROLE_MOVE_STATUS_MAX,
	};
	UNSTD_STATIC_CHECK(ROLE_MOVE_STATUS_MAX < MAX_INT8);
public:
	RoleStatusManager();
	~RoleStatusManager();

	void SetRole(Role* role) { m_role = role; }

	int GetRoleStatus() { return m_role_status; }
	int GetRoleMoveStatus() { return m_move_status; }

	void OnRoleStartFight(int battle_mode);
	void OnRoleFinishFight();
	void OnRoleStartMove();
	void OnRoleStopMove();
	void OnRoleStartGather();
	void OnRoleStopGather();

	void OnRoleStartFishing();
	void OnRoleStopFishing();

	void OnRoleStartSneak();
	void OnRoleStartShiFa();
	void OnRoleStartYinDao();
	void OnRoleStopMoveStatus();

	bool CanRoleStartFight(int battle_mode);
	bool CanRoleMove();
	bool CanFlyByShoe(int target_scene_id);
	bool CanTransportTo(int target_scene_id = 0);
	bool CanGather(int seq_id);
	bool CanStartCross();
	bool CanFish();
	bool IsInBattleStatus();


	void SetXunYouStatus(int status) { m_xun_you_status = status; };
	int GetXunYouStatus() { return m_xun_you_status; };
private:
	void SetRoleStatus(int status);
	void SetRoleMoveStatus(int status);

	Role* m_role;

	int m_role_status;
	int m_move_status;
	int m_xun_you_status;	//巡游状态
};

#endif