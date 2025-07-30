#ifndef __SPECIAL_LOGIC_SHI_TU_CHUAN_GONG_HPP__
#define __SPECIAL_LOGIC_SHI_TU_CHUAN_GONG_HPP__

//ʦͽ���� FB

#include "scene/speciallogic.hpp"
#include "servercommon/shitudef.h"

class SpecialLogicShiTuChuanGong : public SpecialLogic
{
public:
	SpecialLogicShiTuChuanGong(Scene* scene);
	virtual ~SpecialLogicShiTuChuanGong() {};

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role *role);
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);

	virtual bool CanDestroy() { return true; }

	//����ս��
	virtual bool CanRoleStartFight(Role * role, CanRoleStartFightParam &fight_param) { return false; };

	//���ó�����Ϣ
	void SetRoleInfo(int uid, int shi_tu_type);
	void SetEnd();

	//��Ҳ���
	void OnReq(Role *role, int req_type, int p_1, int p_2);

	//����������Ϣ
	void SendSceneShowInfo(Role *role);
	void SendGameInfo(Role *role, int type, unsigned int param);
private:
	//����
	void RoleGameReady(Role* role);
	//�����Ϸ��������
	void RoleGameSorceSet(Role *role, int base_score, int game_score);
	//��Ϸ����
	void GameEnd();
	//�ж���Ϸ
	void BreakOff(Role *role);

	Role* GetRole(int uid);

	//�߻�����  ��;����� ����/�˳� ���߳�˫��
	int m_is_end;	//�����������
	time_t m_game_begin;
	time_t m_game_end;

	time_t m_next_send_game_info_time;

	//ʦͽ��Ϣ
	struct RoleInfo
	{
		RoleInfo()
		{
			role_uid = 0;

			other_uid = 0;
			type = 0;

			game_score = 0;

			base_score = 0;
			other_score = 0;
			game_ready = 0;
		}
		int role_uid;	//��ɫuid

		int other_uid;	//�Է�uid
		int type;	//ʦͽ����

		int game_score;	//��Ϸ�ܷ�

		int base_score;	//��������
		int other_score;//�Է���������

		int game_ready;	//��Ϸ״̬
	};

	//��ɫ��Ϣmap<uid, ��Ϣ>
	RoleInfo m_role_info_[SHI_TU_SEEKING_TYPE_MAX];

	//��ɫ������Ϣ
	RoleInfo * GetRoleInfo(int uid);
	RoleInfo * GetRoleInfoByType(int shi_tu_type);

	//׼����������
	int m_game_ready_count;
	//����֪ͨ
	int m_need_send_score;
};

#endif