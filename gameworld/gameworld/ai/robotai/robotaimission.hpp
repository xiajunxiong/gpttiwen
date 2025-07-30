#ifndef __ROBOT_AI_MISSION_HPP__
#define __ROBOT_AI_MISSION_HPP__

#include "ai/aibase.hpp"
#include "obj/character/character.h"
#include "robotaibase.hpp"



class Role;

class RobotAIMission : public RobotAIBase
{
public:
	static const int TASK_STATUS_OUT_TIME = 300;

	//����״̬
	enum TASK_STATUS
	{
		TASK_STATUS_INVALID = -1,	//						����֡
		TASK_STATUS_GO_ACCEPT,		// ǰ����ȡ
		TASK_STATUS_ACCEPT,			// �ѽ�����(���)		����֡
		TASK_STATUS_ONGOING,		// ���������
		TASK_STATUS_DONE,			// �������(���)		����֡
		TASK_STATUS_GO_COMMIT,		// �����ύ��
		TASK_STATUS_COMMIT,			// �����ύ���(���)	����֡
		TASK_STATUS_END,			// �������				����֡

	};

	//����չʾ��Ϊ
	enum TASK_SHOW_ACTION
	{
		TASK_SHOW_ACTION_NONE = 0,		//�� ����֡
		TASK_SHOW_ACTION_EMPLOY,			//����(����) Ѱ·0
		TASK_SHOW_ACTION_EMPLOY_GATHER,		//����(�ɼ�) Ѱ·0 -> �ɼ�1
		TASK_SHOW_ACTION_EMPLOY_FIGHT,		//����(ս��) Ѱ·0 -> ս��1
		TASK_SHOW_ACTION_EMPLOY_FB,			//����(����) Ѱ·0 -> ����1
		TASK_SHOW_ACTION_EMPLOY_RANGER,		//Ѳ��		ѭ��0 Ѱ·++ -> ս��++ д��ѭ��3��

		TASK_SHOW_ACTION_MAX,
	};

	struct InitParam
	{
		InitParam()
		{//��ʼ��ֻ����0
			m_last_task_id = 0;
			m_cur_task_id = 0;

			m_force_next_task_status = 0;
			m_task_status = 0;
			m_task_show_action = 0;
			m_task_show_action_param = 0;

			m_action_target_posi = Posi(0, 0);
		}
		int m_last_task_id;				// ���һ������id

		int m_cur_task_id;	//��ǰ����id

		time_t m_force_next_task_status;//ǿ���������ʱ��
		int m_task_status;	// ����״̬
		int m_task_show_action;			// ������Ϊ
		int m_task_show_action_param;	// ������Ϊ����

		//������ΪĿ���
		Posi m_action_target_posi;
	};
public:
	RobotAIMission(Character *me);
	virtual ~RobotAIMission();

	virtual void Init(const RobotAIParam &mp);
	virtual void GetInitParam(RobotAIParam& out_param);

	virtual void AI(time_t now_second);
	//����->����״̬->����չʾ��Ϊ->��������Ϊ
	bool AITask(time_t now_second);			//���������л�
	bool AITaskStatus(time_t now_second);	//��������״̬-���  ��������չʾ��Ϊ
	bool AITaskShowAction(time_t now_second);//��������չʾ��Ϊ ����״̬��ɼ��

	int GetTaskShowActionType(int task_condition, int task_meet_requires_type,
		int &need_change_scene, int &scene_id, int &scene_key, Posi &target_posi);


protected:
	virtual int OnActionEnd(time_t now_second);//��������չʾ��Ϊ ����״̬��ɼ��

	void DeleteMe();


	bool GetNPCTarget(int npc_seq_id, int now_scene_id, 
		int &need_change_scene, int &scene_id, int &scene_key, Posi &target_posi) const;

	void SetTaskShowAction(int task_show_action);
	void SetTaskStatus(int task_status);

	int m_last_task_id;				// ���һ������id

	time_t m_next_idle_check_time;

	int m_cur_task_id;	//��ǰ����id
	
	time_t m_force_next_task_status;//ǿ���������ʱ��
	int m_task_status;	// ����״̬
	int m_task_show_action;			// ������Ϊ
	int m_task_show_action_param;	// ������Ϊ����

	//������ΪĿ���
	Posi m_action_target_posi;

};


#endif