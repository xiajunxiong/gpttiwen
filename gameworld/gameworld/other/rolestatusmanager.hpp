#ifndef __ROLE_STATUS_MANAGER__
#define __ROLE_STATUS_MANAGER__

class Role;
class RoleStatusManager
{
public:
	enum ROLE_STATUS
	{
		ROLE_STATUS_INVALID = 0,

		ROLE_STATUS_IDLE,		// ��ֹ
		ROLE_STATUS_MOVE,		// �ƶ�
		ROLE_STATUS_FIGHT,		// ս��
		ROLE_STATUS_GATHER,		// �ɼ�
		ROLE_STATUS_FISHING,	// ����

		ROLE_STATUS_MAX
	};

	enum ROLE_MOVE_STATUS
	{
		ROLE_MOVE_STATUS_NORMAL = 0,	//����
		ROLE_MOVE_STATUS_SNEAK = 1,		//Ǳ��
		ROLE_MOVE_STATUS_SHIFA = 2,		//ʩ��
		ROLE_MOVE_STATUS_YINDAO = 3,	//����
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
	int m_xun_you_status;	//Ѳ��״̬
};

#endif