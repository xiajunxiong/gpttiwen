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

	//任务状态
	enum TASK_STATUS
	{
		TASK_STATUS_INVALID = -1,	//						处理帧
		TASK_STATUS_GO_ACCEPT,		// 前往接取
		TASK_STATUS_ACCEPT,			// 已接任务(标记)		处理帧
		TASK_STATUS_ONGOING,		// 任务进行中
		TASK_STATUS_DONE,			// 任务完成(标记)		处理帧
		TASK_STATUS_GO_COMMIT,		// 任务提交中
		TASK_STATUS_COMMIT,			// 任务提交完成(标记)	处理帧
		TASK_STATUS_END,			// 任务结束				处理帧

	};

	//任务展示行为
	enum TASK_SHOW_ACTION
	{
		TASK_SHOW_ACTION_NONE = 0,		//无 处理帧
		TASK_SHOW_ACTION_EMPLOY,			//交互(立刻) 寻路0
		TASK_SHOW_ACTION_EMPLOY_GATHER,		//交互(采集) 寻路0 -> 采集1
		TASK_SHOW_ACTION_EMPLOY_FIGHT,		//交互(战斗) 寻路0 -> 战斗1
		TASK_SHOW_ACTION_EMPLOY_FB,			//交互(副本) 寻路0 -> 副本1
		TASK_SHOW_ACTION_EMPLOY_RANGER,		//巡逻		循环0 寻路++ -> 战斗++ 写死循环3次

		TASK_SHOW_ACTION_MAX,
	};

	struct InitParam
	{
		InitParam()
		{//初始化只能是0
			m_last_task_id = 0;
			m_cur_task_id = 0;

			m_force_next_task_status = 0;
			m_task_status = 0;
			m_task_show_action = 0;
			m_task_show_action_param = 0;

			m_action_target_posi = Posi(0, 0);
		}
		int m_last_task_id;				// 最后一个任务id

		int m_cur_task_id;	//当前任务id

		time_t m_force_next_task_status;//强制完成任务时间
		int m_task_status;	// 任务状态
		int m_task_show_action;			// 任务行为
		int m_task_show_action_param;	// 任务行为参数

		//任务行为目标点
		Posi m_action_target_posi;
	};
public:
	RobotAIMission(Character *me);
	virtual ~RobotAIMission();

	virtual void Init(const RobotAIParam &mp);
	virtual void GetInitParam(RobotAIParam& out_param);

	virtual void AI(time_t now_second);
	//任务->任务状态->任务展示行为->机器人行为
	bool AITask(time_t now_second);			//设置任务切换
	bool AITaskStatus(time_t now_second);	//处理任务状态-标记  设置任务展示行为
	bool AITaskShowAction(time_t now_second);//处理任务展示行为 任务状态完成检测

	int GetTaskShowActionType(int task_condition, int task_meet_requires_type,
		int &need_change_scene, int &scene_id, int &scene_key, Posi &target_posi);


protected:
	virtual int OnActionEnd(time_t now_second);//处理任务展示行为 任务状态完成检测

	void DeleteMe();


	bool GetNPCTarget(int npc_seq_id, int now_scene_id, 
		int &need_change_scene, int &scene_id, int &scene_key, Posi &target_posi) const;

	void SetTaskShowAction(int task_show_action);
	void SetTaskStatus(int task_status);

	int m_last_task_id;				// 最后一个任务id

	time_t m_next_idle_check_time;

	int m_cur_task_id;	//当前任务id
	
	time_t m_force_next_task_status;//强制完成任务时间
	int m_task_status;	// 任务状态
	int m_task_show_action;			// 任务行为
	int m_task_show_action_param;	// 任务行为参数

	//任务行为目标点
	Posi m_action_target_posi;

};


#endif