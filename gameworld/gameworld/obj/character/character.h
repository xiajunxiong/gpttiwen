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

	//移动辅助模块  未调用创建函数时为空 切换场景后为空
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

	unsigned int m_last_check_valid_time;					// 上次检测移动非法的时间
	Posi m_last_check_pos;									// 上次检测移动非法的坐标

	int	m_speed_rate;										// 移动速度比率
	int	m_speed;											// 移动速度

	int m_action_status;									// 动作状态
	int m_logic_result;										// 逻辑结果

	float m_old_dir;										// 避免频繁调用cos sin 记录最后一次在方向
	float m_old_cos;										// 上个方向在cos值
	float m_old_sin;										// 上个方向在sin值

	//移动辅助模块  未调用创建函数时为空 切换场景为空
	MoveAssist* m_move_assist;
};

#endif