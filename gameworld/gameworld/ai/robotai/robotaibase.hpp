#ifndef __ROBOT_AI_BASE_HPP__
#define __ROBOT_AI_BASE_HPP__

#include "ai/aibase.hpp"
#include "servercommon/struct/global/robotaiparam.hpp"


enum ROBOT_AI_TYPE
{
	ROBOT_AI_TYPE_DEF = 0,			//无特殊逻辑
	ROBOT_AI_TYPE_MISSION = 1,		//跑图
	ROBOT_AI_TYPE_RANGER = 2,		//巡逻

	ROBOT_AI_TYPE_MAX,
};

class GatherObj;
//TODO::行为 多态  行为list-> 行为
// ai   控制 行为   ROBOT_AI_IDEA --AI--> ROBOT_AI_ACTION
// 行为 导致 状态	ROBOT_AI_ACTION --ActionChange--> OnRoleStatusChange
class RobotAIBase : public AIBase
{
	friend class Robot;
public:
	static const int DEF_ENCOUNTER_FIGHT_TIME = 20;	//默认战斗间隔
	static const int DEF_VIRTUAL_FIGHT_TIME_MIN = 60;	//最小虚拟战斗时长
	static const int DEF_VIRTUAL_FIGHT_TIME_MAX = 120;	//最大虚拟战斗时长
	static const int DEF_VIRTUAL_FB_TIME_MIN = 180;		//最小虚拟副本时长
	static const int DEF_VIRTUAL_FB_TIME_MAX = 240;		//最大虚拟副本时长
public:
	//行为
	enum ROBOT_AI_ACTION
	{								//      end_time
		ROBOT_AI_ACTION_IDLE,		// 静止	无效
		ROBOT_AI_ACTION_WAIT,		// 等待	等待结束
		ROBOT_AI_ACTION_MOVE,		// 移动	移动结束
		ROBOT_AI_ACTION_FIGHT,		// 战斗 战斗结束
		ROBOT_AI_ACTION_GATHER,		// 采集 采集结束
		ROBOT_AI_ACTION_HIDE,		// 隐藏 隐藏结束

		ROBOT_AI_ACTION_MAX,
	};
public:
	RobotAIBase(Character *me, int robot_ai_type);
	virtual ~RobotAIBase();

	virtual void Init(const RobotAIParam &mp) {};
	virtual void GetInitParam(RobotAIParam& out_param) {};


	virtual void Update(unsigned long interval, time_t now_second);
	virtual void AI(time_t now_second) {};

	//设置行为  行为类型, 结束时间戳(action_end_time < now_time 会在同帧结束)
	virtual void SetAction(int action_type, time_t action_end_time);


	//获得行为对应的角色状态类型
	virtual int GetRoleStatus(int action_type = -1);
	//AI行为对应的角色状态改变
	virtual void OnRobotAIChangeRoleStatus(int action_type);

	virtual int GetAIType() { return m_robot_ai_type; }

protected:
	 
	//行为结束
	virtual int OnActionEnd(time_t now_second) { return m_action_type; };

	int m_robot_ai_type;

	int m_action_type;		//行为类型
	time_t m_action_end_time;	//行为结束时间戳
};

#endif