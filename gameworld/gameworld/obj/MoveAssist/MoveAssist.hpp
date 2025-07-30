#ifndef __MOVE_ASSIST_HPP__
#define __MOVE_ASSIST_HPP__

#include "gamedef.h"
#include "protocol/msgmoveassist.h"

static const int MOVE_ASSIXT_WAY_POINT_NUM = 120;		//辅助移动 路径点数量

class Character;

//切场景会删掉
class MoveAssist
{
	friend class Character;
public:
	enum MOVE_ASSIST_STATUS_TYPE
	{
		MOVE_ASSIST_STATUS_TYPE_NONE = 0,			//未开启
		MOVE_ASSIST_STATUS_TYPE_WAIT_WAY_RET = 1,	//等待路径点返回
		MOVE_ASSIST_STATUS_TYPE_WAY_MOVE = 2,		//按路径移动
		MOVE_ASSIST_STATUS_TYPE_WAIT_MOVE = 3,		//暂停按路径移动

		MOVE_ASSIST_STATUS_TYPE_MAX,
	};

	enum MOVE_ASSIST_REQ_TYPE
	{
		MOVE_ASSIST_REQ_TYPE_NONE = 0,		//	无效
		MOVE_ASSIST_REQ_TYPE_MOVE_TO = 1,	//	移动到目标点
		MOVE_ASSIST_REQ_TYPE_AROND = 2,		//	场景内巡逻
		MOVE_ASSIST_REQ_TYPE_EMPLOY = 3,	//	交互

		MOVE_ASSIST_REQ_TYPE_MAX,
	};
public:
	int GetMoveAssistStatus() { return m_move_assixt_status; };

	//操作逻辑尽量在Update中延迟执行 以确保不会因事件调用死循环
	void Update(unsigned long interval, time_t now_second);

	//设置目的地  接受移动辅助返回后会重设一次
	void SetDestination(Posi target_pos, int assist_type);
	//清空目的地
	void ReSetDestination();
	//暂停
	void MovePause();
	//继续
	void MoveContinue();

	//事件
	void OnMoveToOther();	//非移动辅助的移动操作

	//接受移动辅助
	void OnMoveAssistRet(Protocol::CSMoveAssistRet & ret_info);
protected:
	//操作逻辑尽量在Update中延迟执行 以确保不会因事件调用死循环
	//发送移动辅助请求
	void SendMoveAssisReq();
	//设置路径
	void SetWayPointList(int way_point_num, const Posi * way_point_list);

	//TODO:超时事件
	UNSTD_STATIC_CHECK(4 == MOVE_ASSIST_STATUS_TYPE_MAX);



	Character * m_this_cha;
	unsigned int m_move_assist_id;

	//移动辅助-状态
	int m_move_assixt_status;		//当前状态
	//(TODO:调用对应超时事件)
	//time_t m_status_begin_time;	//当前状态开始时间
	//time_t m_status_end_time;		//当前状态开始时间

	int m_assist_type;				//辅助类型
	Posi m_target_posi;				//目的地

	//路径点
	int m_way_point_count;
	Posi m_way_point_list[MOVE_ASSIXT_WAY_POINT_NUM];

	//下个路径点index
	int m_next_way_point_index;

	//状态参数
	//等待路径点返回  再次发送请求协议时间
	time_t m_next_move_assist_req_time;
	//辅助移动 移动失败 重试时间间隔
	time_t m_next_move_assist_move_time;


private:
	MoveAssist(Character* self_cha, unsigned int move_assist_id);
	~MoveAssist();

	void* operator new(size_t c);
	void operator delete(void* m);
};

#endif