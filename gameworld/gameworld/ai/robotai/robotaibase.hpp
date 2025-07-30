#ifndef __ROBOT_AI_BASE_HPP__
#define __ROBOT_AI_BASE_HPP__

#include "ai/aibase.hpp"
#include "servercommon/struct/global/robotaiparam.hpp"


enum ROBOT_AI_TYPE
{
	ROBOT_AI_TYPE_DEF = 0,			//�������߼�
	ROBOT_AI_TYPE_MISSION = 1,		//��ͼ
	ROBOT_AI_TYPE_RANGER = 2,		//Ѳ��

	ROBOT_AI_TYPE_MAX,
};

class GatherObj;
//TODO::��Ϊ ��̬  ��Ϊlist-> ��Ϊ
// ai   ���� ��Ϊ   ROBOT_AI_IDEA --AI--> ROBOT_AI_ACTION
// ��Ϊ ���� ״̬	ROBOT_AI_ACTION --ActionChange--> OnRoleStatusChange
class RobotAIBase : public AIBase
{
	friend class Robot;
public:
	static const int DEF_ENCOUNTER_FIGHT_TIME = 20;	//Ĭ��ս�����
	static const int DEF_VIRTUAL_FIGHT_TIME_MIN = 60;	//��С����ս��ʱ��
	static const int DEF_VIRTUAL_FIGHT_TIME_MAX = 120;	//�������ս��ʱ��
	static const int DEF_VIRTUAL_FB_TIME_MIN = 180;		//��С���⸱��ʱ��
	static const int DEF_VIRTUAL_FB_TIME_MAX = 240;		//������⸱��ʱ��
public:
	//��Ϊ
	enum ROBOT_AI_ACTION
	{								//      end_time
		ROBOT_AI_ACTION_IDLE,		// ��ֹ	��Ч
		ROBOT_AI_ACTION_WAIT,		// �ȴ�	�ȴ�����
		ROBOT_AI_ACTION_MOVE,		// �ƶ�	�ƶ�����
		ROBOT_AI_ACTION_FIGHT,		// ս�� ս������
		ROBOT_AI_ACTION_GATHER,		// �ɼ� �ɼ�����
		ROBOT_AI_ACTION_HIDE,		// ���� ���ؽ���

		ROBOT_AI_ACTION_MAX,
	};
public:
	RobotAIBase(Character *me, int robot_ai_type);
	virtual ~RobotAIBase();

	virtual void Init(const RobotAIParam &mp) {};
	virtual void GetInitParam(RobotAIParam& out_param) {};


	virtual void Update(unsigned long interval, time_t now_second);
	virtual void AI(time_t now_second) {};

	//������Ϊ  ��Ϊ����, ����ʱ���(action_end_time < now_time ����ͬ֡����)
	virtual void SetAction(int action_type, time_t action_end_time);


	//�����Ϊ��Ӧ�Ľ�ɫ״̬����
	virtual int GetRoleStatus(int action_type = -1);
	//AI��Ϊ��Ӧ�Ľ�ɫ״̬�ı�
	virtual void OnRobotAIChangeRoleStatus(int action_type);

	virtual int GetAIType() { return m_robot_ai_type; }

protected:
	 
	//��Ϊ����
	virtual int OnActionEnd(time_t now_second) { return m_action_type; };

	int m_robot_ai_type;

	int m_action_type;		//��Ϊ����
	time_t m_action_end_time;	//��Ϊ����ʱ���
};

#endif