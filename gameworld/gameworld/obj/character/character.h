#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "obj/obj.h"

class MoveAssist;

class Character : public Obj
{
public:
	enum _ActionStatus
	{
		ACTION_IDLE,
		ACTION_RUN,
		ACTION_COUNT,
	};

	enum LogicResult
	{
		LOGIC_NONE,
		MOVE_STATIC,
		MOVE_RUN,
	};

public:
	Character(ObjType type);
	Character(ObjType type, const GameName _name);
	virtual ~Character();

	virtual void Update(unsigned long interval, time_t now_second, unsigned int now_dayid);

	int GetActionStatus() { return m_action_status; }
	int GetLogicResult() { return m_logic_result; }

	int	GetSpeedRate() const { return m_speed_rate; }
	virtual int SetSpeedRate(int rate);

	int GetSpeed() const;
	int SetSpeed(int speed);

	virtual bool Move(Axis _x, Axis _y, Dir _dir, Scalar distance, short height, bool check_valid = false, bool is_move_assist = false);
	void ActionStop();
	void MoveStop(bool is_notice = false);
	bool IsStatic() { return (m_dir == Dir_S) || (m_dir_distance < 0.0001); }
	void SetDir(Dir _dir) { m_dir = _dir; };

	//�ƶ�����ģ��  δ���ô�������ʱΪ�� �л�������Ϊ��
	MoveAssist* GetMoveAssist(bool can_creat);
	MoveAssist* CreatMoveAssist();
	void SetMoveLogic(Posi _target_posi);

protected:
	int MoveLogic(unsigned long interval);
	void SendPosiInfo();

	virtual bool CheckMapValidate() { return true; }
	virtual bool CheckMoveFrequency() { return true; }
	virtual bool CanCharacterAction(bool is_notify_client = false, bool is_move = false) { return true; }
	virtual bool CheckWaiGuaMove() { return true; }

	unsigned int m_last_check_valid_time;					// �ϴμ���ƶ��Ƿ���ʱ��
	Posi m_last_check_pos;									// �ϴμ���ƶ��Ƿ�������

	int	m_speed_rate;										// �ƶ��ٶȱ���
	int	m_speed;											// �ƶ��ٶ�

	int m_action_status;									// ����״̬
	int m_logic_result;										// �߼����

	float m_old_dir;										// ����Ƶ������cos sin ��¼���һ���ڷ���
	float m_old_cos;										// �ϸ�������cosֵ
	float m_old_sin;										// �ϸ�������sinֵ

	//�ƶ�����ģ��  δ���ô�������ʱΪ�� �л�����Ϊ��
	MoveAssist* m_move_assist;
};

#endif